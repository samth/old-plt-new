
#ifndef UNIT_TEST
# include "schpriv.h"
#endif

//This will be TRUE if primitive tracking has been enabled 
//by the program

#ifndef FUTURES_ENABLED

/* Futures not enabled, but make a stub module */

static Scheme_Object *future(int argc, Scheme_Object *argv[])
{
  scheme_signal_error("future: not enabled");
  return NULL;
}

static Scheme_Object *touch(int argc, Scheme_Object *argv[])
{
  scheme_signal_error("touch: not enabled");
  return NULL;
}

static Scheme_Object *processor_count(int argc, Scheme_Object *argv[])
{
  scheme_signal_error("processor-count: not enabled");
  return NULL;
}

# define FUTURE_PRIM_W_ARITY(name, func, a1, a2, env) GLOBAL_PRIM_W_ARITY(name, func, a1, a2, env)

void scheme_init_futures(Scheme_Env *env)
{
  Scheme_Env *newenv;
  
  newenv = scheme_primitive_module(scheme_intern_symbol("#%futures"), 
                                   env);

  FUTURE_PRIM_W_ARITY("future",           future,           1, 1, newenv);
  FUTURE_PRIM_W_ARITY("touch",            touch,            1, 1, newenv);
  FUTURE_PRIM_W_ARITY("processor-count",  processor_count,  1, 1, newenv);

  scheme_finish_primitive_module(newenv);
  scheme_protect_primitive_provide(newenv, NULL);
}

#else

#include "future.h"
#include <stdlib.h>
#include <string.h>
#ifdef UNIT_TEST
# include "./tests/unit_test.h"
#endif 

static Scheme_Object *future(int argc, Scheme_Object *argv[]);
static Scheme_Object *touch(int argc, Scheme_Object *argv[]);
static Scheme_Object *processor_count(int argc, Scheme_Object *argv[]);
static void futures_init(void);
static void init_future_thread(struct Scheme_Future_State *fs, int i);

#define THREAD_POOL_SIZE 12
#define INITIAL_C_STACK_SIZE 500000

typedef struct Scheme_Future_State {
  struct Scheme_Future_Thread_State *pool_threads[THREAD_POOL_SIZE];

  void *signal_handle;

  int future_queue_count;
  future_t *future_queue;
  future_t *future_queue_end;
  future_t *future_waiting_atomic;
  int next_futureid;

  pthread_mutex_t future_mutex;
  pthread_cond_t future_pending_cv;
  pthread_cond_t gc_ok_c;
  pthread_cond_t gc_done_c;

  int gc_not_ok, wait_for_gc;

  int *gc_counter_ptr;

  int future_threads_created;
} Scheme_Future_State;

typedef struct Scheme_Future_Thread_State {
  int id;
  pthread_t threadid;
  int worker_gc_counter;
  pthread_cond_t worker_can_continue_cv;
  future_t *current_ft;
  long runstack_size;

  volatile int *fuel_pointer;
  volatile unsigned long *stack_boundary_pointer;
  volatile int *need_gc_pointer;
} Scheme_Future_Thread_State;

THREAD_LOCAL_DECL(static Scheme_Future_State *scheme_future_state);
THREAD_LOCAL_DECL(void *jit_future_storage[2]);

#ifdef MZ_PRECISE_GC
THREAD_LOCAL_DECL(extern unsigned long GC_gen0_alloc_page_ptr);
#endif

#ifdef MZ_PRECISE_GC
static void register_traversers(void);
#endif

static void start_gc_not_ok(Scheme_Future_State *fs);
static void end_gc_not_ok(Scheme_Future_Thread_State *fts, 
                          Scheme_Future_State *fs, 
                          Scheme_Object **current_rs);

static void *worker_thread_future_loop(void *arg);
static void invoke_rtcall(Scheme_Future_State * volatile fs, future_t * volatile future);
static future_t *enqueue_future(Scheme_Future_State *fs, future_t *ft);;
static future_t *get_pending_future(Scheme_Future_State *fs);

#ifdef MZ_PRECISE_GC
# define scheme_future_setjmp(newbuf) scheme_jit_setjmp((newbuf).jb)
# define scheme_future_longjmp(newbuf, v) scheme_jit_longjmp((newbuf).jb, v)
#else
# define scheme_future_setjmp(newbuf) scheme_setjmp(newbuf)
# define scheme_future_longjmp(newbuf, v) scheme_longjmp(newbuf, v)
#endif

/**********************************************************************/
/*   Semaphore helpers                                                */
/**********************************************************************/

typedef struct sema_t {
  int ready;
  pthread_mutex_t m;
  pthread_cond_t c;
} sema_t;

static void sema_wait(sema_t *s)
{
  pthread_mutex_lock(&s->m);
  while (!s->ready) {
    pthread_cond_wait(&s->c, &s->m);
  }
  --s->ready;
  pthread_mutex_unlock(&s->m);
}

static void sema_signal(sema_t *s)
{
  pthread_mutex_lock(&s->m);
  s->ready++;
  pthread_cond_signal(&s->c);
  pthread_mutex_unlock(&s->m);
}

static void sema_init(sema_t *s)
{
  pthread_mutex_init(&s->m, NULL);
  pthread_cond_init(&s->c, NULL);
  s->ready = 0;
}

static void sema_destroy(sema_t *s)
{
  pthread_mutex_destroy(&s->m);
  pthread_cond_destroy(&s->c);
}

/**********************************************************************/
/* Arguments for a newly created future thread                        */
/**********************************************************************/

typedef struct future_thread_params_t {
  struct sema_t ready_sema;
  struct NewGC *shared_GC;
  Scheme_Future_State *fs;
  Scheme_Future_Thread_State *fts;
  Scheme_Thread *thread_skeleton;
  Scheme_Object **runstack_start;

  Scheme_Object ***scheme_current_runstack_ptr;
  Scheme_Object ***scheme_current_runstack_start_ptr;
  Scheme_Thread **current_thread_ptr;
  void *jit_future_storage_ptr;
} future_thread_params_t;

/**********************************************************************/
/* Plumbing for MzScheme initialization                               */
/**********************************************************************/

//Invoked by the runtime on startup to make 
//primitives known
void scheme_init_futures(Scheme_Env *env)
{
  Scheme_Object *v;
  Scheme_Env *newenv;

  futures_init();

  v = scheme_intern_symbol("#%futures");
  newenv = scheme_primitive_module(v, env);

  scheme_add_global_constant(
                             "future", 
                             scheme_make_prim_w_arity(
                                                      future, 
                                                      "future", 
                                                      1, 
                                                      1), 
                             newenv);

  scheme_add_global_constant(
                             "processor-count", 
                             scheme_make_prim_w_arity(
                                                      processor_count, 
                                                      "processor-count", 
                                                      0, 
                                                      0), 
                             newenv);

  scheme_add_global_constant(
                             "touch", 
                             scheme_make_prim_w_arity(
                                                      touch, 
                                                      "touch", 
                                                      1, 
                                                      1), 
                             newenv);

  scheme_finish_primitive_module(newenv);
  scheme_protect_primitive_provide(newenv, NULL);
}

void futures_init(void)
{
  Scheme_Future_State *fs;
  void *hand;

  fs = (Scheme_Future_State *)malloc(sizeof(Scheme_Future_State));
  memset(fs, 0, sizeof(Scheme_Future_State));
  scheme_future_state = fs;

  REGISTER_SO(fs->future_queue);
  REGISTER_SO(fs->future_queue_end);
  REGISTER_SO(fs->future_waiting_atomic);
  
  pthread_mutex_init(&fs->future_mutex, NULL);
  pthread_cond_init(&fs->future_pending_cv, NULL);
  pthread_cond_init(&fs->gc_ok_c, NULL);
  pthread_cond_init(&fs->gc_done_c, NULL);

  fs->gc_counter_ptr = &scheme_did_gc_count;

  hand = scheme_get_signal_handle();
  fs->signal_handle = hand;

#ifdef MZ_PRECISE_GC
  register_traversers();
#endif
}

static void init_future_thread(Scheme_Future_State *fs, int i)
{
  Scheme_Future_Thread_State *fts;
  future_thread_params_t params;
  pthread_t threadid;
  GC_CAN_IGNORE pthread_attr_t attr;

  //Create the worker thread pool.  These threads will
  //'queue up' and wait for futures to become available   
  pthread_attr_init(&attr);
  pthread_attr_setstacksize(&attr, INITIAL_C_STACK_SIZE);

  fts = (Scheme_Future_Thread_State *)malloc(sizeof(Scheme_Future_Thread_State));
  memset(fts, 0, sizeof(Scheme_Future_Thread_State));
  fts->id = i;

  params.shared_GC = GC;
  params.fts = fts;
  params.fs = fs;

  /* Make enough of a thread record to deal with multiple values. */
  params.thread_skeleton = MALLOC_ONE_TAGGED(Scheme_Thread);
  params.thread_skeleton->so.type = scheme_thread_type;

  {
    Scheme_Object **rs_start, **rs;
    long init_runstack_size = 1000;
    rs_start = scheme_alloc_runstack(init_runstack_size);
    rs = rs_start XFORM_OK_PLUS init_runstack_size;
    params.runstack_start = rs_start;
    fts->runstack_size = init_runstack_size;
  }

  sema_init(&params.ready_sema);
  pthread_create(&threadid, &attr, worker_thread_future_loop, &params);
  sema_wait(&params.ready_sema);
  sema_destroy(&params.ready_sema);

  fts->threadid = threadid;
	
  scheme_register_static(&fts->current_ft, sizeof(void*));
  scheme_register_static(params.scheme_current_runstack_ptr, sizeof(void*));
  scheme_register_static(params.scheme_current_runstack_start_ptr, sizeof(void*));	
  scheme_register_static(params.jit_future_storage_ptr, 2 * sizeof(void*));
  scheme_register_static(params.current_thread_ptr, sizeof(void*));

  fs->pool_threads[i] = fts;
}

static void start_gc_not_ok(Scheme_Future_State *fs)
{
  while (fs->wait_for_gc) {
    pthread_cond_wait(&fs->gc_done_c, &fs->future_mutex);
  }

  fs->gc_not_ok++;

#ifdef MZ_PRECISE_GC
  {
    Scheme_Future_Thread_State *fts = scheme_future_thread_state;
    if (fts->worker_gc_counter != *fs->gc_counter_ptr) {
      GC_gen0_alloc_page_ptr = 0; /* forces future to ask for memory */
      fts->worker_gc_counter = *fs->gc_counter_ptr;
    }
  }
#endif
}

static void end_gc_not_ok(Scheme_Future_Thread_State *fts, 
                          Scheme_Future_State *fs, 
                          Scheme_Object **current_rs)
{
  scheme_set_runstack_limits(MZ_RUNSTACK_START, 
                             fts->runstack_size,
                             (current_rs
                              ? current_rs XFORM_OK_MINUS MZ_RUNSTACK_START
                              : fts->runstack_size),
                             fts->runstack_size);

  /* FIXME: clear scheme_current_thread->ku.multiple.array ? */

  --fs->gc_not_ok;
  pthread_cond_signal(&fs->gc_ok_c);
}

void scheme_future_block_until_gc()
{
  Scheme_Future_State *fs = scheme_future_state;
  int i;

  if (!fs) return;

  pthread_mutex_lock(&fs->future_mutex);
  fs->wait_for_gc = 1;
  pthread_mutex_unlock(&fs->future_mutex);

  for (i = 0; i < THREAD_POOL_SIZE; i++) { 
    if (fs->pool_threads[i]) {
      *(fs->pool_threads[i]->need_gc_pointer) = 1;
      *(fs->pool_threads[i]->fuel_pointer) = 0;
      *(fs->pool_threads[i]->stack_boundary_pointer) += INITIAL_C_STACK_SIZE;
    }
  }
  asm("mfence");

  pthread_mutex_lock(&fs->future_mutex);
  while (fs->gc_not_ok) {
    pthread_cond_wait(&fs->gc_ok_c, &fs->future_mutex);
  }
  pthread_mutex_unlock(&fs->future_mutex);
}

void scheme_future_continue_after_gc()
{
  Scheme_Future_State *fs = scheme_future_state;
  int i;

  if (!fs) return;

  for (i = 0; i < THREAD_POOL_SIZE; i++) {
    if (fs->pool_threads[i]) {
      *(fs->pool_threads[i]->need_gc_pointer) = 0;
      *(fs->pool_threads[i]->fuel_pointer) = 1;
      *(fs->pool_threads[i]->stack_boundary_pointer) -= INITIAL_C_STACK_SIZE;
    }
  }

  pthread_mutex_lock(&fs->future_mutex);
  fs->wait_for_gc = 0;
  pthread_cond_broadcast(&fs->gc_done_c);
  pthread_mutex_unlock(&fs->future_mutex);
}

void scheme_future_gc_pause()
/* Called in future thread */
{
  Scheme_Future_Thread_State *fts = scheme_future_thread_state;
  Scheme_Future_State *fs = scheme_future_state;

  pthread_mutex_lock(&fs->future_mutex);
  end_gc_not_ok(fts, fs, MZ_RUNSTACK);
  start_gc_not_ok(fs); /* waits until wait_for_gc is 0 */
  pthread_mutex_unlock(&fs->future_mutex);
}

/**********************************************************************/
/* Primitive implementations                    					  */
/**********************************************************************/

Scheme_Object *future(int argc, Scheme_Object *argv[])
/* Called in runtime thread */
{
  Scheme_Future_State *fs = scheme_future_state;
  int futureid, count;
  future_t *ft;
  Scheme_Native_Closure *nc;
  Scheme_Native_Closure_Data *ncd;
  Scheme_Object *lambda = argv[0];

  //Input validation
  scheme_check_proc_arity("future", 0, 0, argc, argv);

  if (fs->future_threads_created < THREAD_POOL_SIZE) {
    pthread_mutex_lock(&fs->future_mutex);
    count = fs->future_queue_count;
    pthread_mutex_unlock(&fs->future_mutex);
    if (count >= fs->future_threads_created) {
      init_future_thread(fs, fs->future_threads_created);
      fs->future_threads_created++;
    }
  }

  nc = (Scheme_Native_Closure*)lambda;
  ncd = nc->code;

  //Create the future descriptor and add to the queue as 'pending'    
  ft = MALLOC_ONE_TAGGED(future_t);     
  ft->so.type = scheme_future_type;

  futureid = ++fs->next_futureid;
  ft->id = futureid;
  ft->orig_lambda = lambda;
  ft->status = PENDING;
   
  //JIT compile the code if not already jitted
  //Temporarily repoint MZ_RUNSTACK
  //to the worker thread's runstack -
  //in case the JIT compiler uses the stack address
  //when generating code
  if (ncd->code == scheme_on_demand_jit_code)
    {
      scheme_on_demand_generate_lambda(nc, 0, NULL);
    }

  ft->code = (void*)ncd->code;

  pthread_mutex_lock(&fs->future_mutex);
  enqueue_future(fs, ft);
  //Signal that a future is pending
  pthread_cond_signal(&fs->future_pending_cv);
  pthread_mutex_unlock(&fs->future_mutex);

  return (Scheme_Object*)ft;
}


int future_ready(Scheme_Object *obj)
/* Called in runtime thread by Scheme scheduler */
{
  Scheme_Future_State *fs = scheme_future_state;
  int ret = 0;
  future_t *ft = (future_t*)obj;

  pthread_mutex_lock(&fs->future_mutex);
  if (ft->work_completed || ft->rt_prim) {
    ret = 1;
  }
  pthread_mutex_unlock(&fs->future_mutex);

  return ret;
}

static void dequeue_future(Scheme_Future_State *fs, future_t *ft)
/* called from both future and runtime threads */
{
  START_XFORM_SKIP;

  if (ft->prev == NULL)
    fs->future_queue = ft->next;
  else
    ft->prev->next = ft->next;
  
  if (ft->next == NULL)
    fs->future_queue_end = ft->prev;
  else
    ft->next->prev = ft->prev;

  ft->next = NULL;
  ft->prev = NULL;

  --fs->future_queue_count;

  END_XFORM_SKIP;
}

Scheme_Object *touch(int argc, Scheme_Object *argv[])
/* Called in runtime thread */
{
  Scheme_Future_State *fs = scheme_future_state;
  Scheme_Object *retval = NULL;
  future_t *ft;

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_future_type))
    scheme_wrong_type("touch", "future", 0, argc, argv);

  ft = (future_t*)argv[0];

#ifdef DEBUG_FUTURES 
  LOG("touch (future %d)", futureid);	
  dump_state();
#endif

  pthread_mutex_lock(&fs->future_mutex);
  if (ft->status == PENDING) {
    ft->status = RUNNING;
    pthread_mutex_unlock(&fs->future_mutex);

    retval = _scheme_apply(ft->orig_lambda, 0, NULL);

    pthread_mutex_lock(&fs->future_mutex);
    ft->work_completed = 1;
    ft->retval = retval;
    ft->status = FINISHED;
    dequeue_future(fs, ft);
    pthread_mutex_unlock(&fs->future_mutex);

    return retval;
  }
  pthread_mutex_unlock(&fs->future_mutex);

  //Spin waiting for primitive calls or a return value from
  //the worker thread
 wait_for_rtcall_or_completion:
  scheme_block_until(future_ready, NULL, (Scheme_Object*)ft, 0);
  pthread_mutex_lock(&fs->future_mutex);
  if (ft->work_completed)
    {
      retval = ft->retval;

      LOG("Successfully touched future %d\n", ft->id);
      // fflush(stdout);

      pthread_mutex_unlock(&fs->future_mutex);
    }
  else if (ft->rt_prim)
    {
      //Invoke the primitive and stash the result
      //Release the lock so other threads can manipulate the queue
      //while the runtime call executes
      pthread_mutex_unlock(&fs->future_mutex);
      LOG("Invoking primitive %p on behalf of future %d...", ft->rt_prim, ft->id);
      invoke_rtcall(fs, ft);
      LOG("done.\n");

      goto wait_for_rtcall_or_completion;
    }
  else
    {
      pthread_mutex_unlock(&fs->future_mutex);
      goto wait_for_rtcall_or_completion;
    }

  if (!retval) {
    scheme_signal_error("touch: future previously aborted");
  }

  return retval;
}

#ifdef linux 
#include <unistd.h>
#elif OS_X 
#include <sys/param.h>
#include <sys/sysctl.h>
#elif WINDOWS 
#include <windows.h>
#endif 

Scheme_Object *processor_count(int argc, Scheme_Object *argv[])
/* Called in runtime thread */
{
  int cpucount = 0;

#ifdef linux 
  cpucount = sysconf(_SC_NPROCESSORS_ONLN);
#elif OS_X 
  size_t size = sizeof(cpucount) ;

  if (sysctlbyname("hw.ncpu", &cpucount, &size, NULL, 0))
	{
	  cpucount = 1;
	}
#elif WINDOWS 
  SYSTEM_INFO sysinfo;
  GetSystemInfo(&sysinfo);
  cpucount = sysinfo.dwNumberOfProcessors;
#else
  cpucount = THREAD_POOL_SIZE;
#endif

  return scheme_make_integer(cpucount);
}

//Entry point for a worker thread allocated for
//executing futures.  This function will never terminate
//(until the process dies).
void *worker_thread_future_loop(void *arg)
/* Called in future thread; runtime thread is blocked until ready_sema
  is signaled. */
{
  START_XFORM_SKIP;
  /* valid only until signaling */
  future_thread_params_t *params = (future_thread_params_t *)arg;
  Scheme_Future_Thread_State *fts = params->fts;
  Scheme_Future_State *fs = params->fs;
  Scheme_Object *v;
  Scheme_Object* (*jitcode)(Scheme_Object*, int, Scheme_Object**);
  future_t *ft;
  mz_jmp_buf newbuf;

  scheme_init_os_thread();

  scheme_future_state = fs;
  scheme_future_thread_state = fts;

  GC = params->shared_GC;
  scheme_current_thread = params->thread_skeleton;

  //Set processor affinity
  /*pthread_mutex_lock(&fs->future_mutex);
      static unsigned long cur_cpu_mask = 1;
    if (pthread_setaffinity_np(pthread_self(), sizeof(g_cur_cpu_mask), &g_cur_cpu_mask))
    {
    printf(
    "Could not set CPU affinity (%lu) for thread %p!\n", 
    ++g_cur_cpu_mask, 
    pthread_self());
    }

    pthread_mutex_unlock(&fs->future_mutex);
  */

  pthread_cond_init(&fts->worker_can_continue_cv, NULL);

  scheme_use_rtcall = 1;

  scheme_fuel_counter = 1;
  scheme_jit_stack_boundary = ((unsigned long)&v) - INITIAL_C_STACK_SIZE;

  fts->need_gc_pointer = &scheme_future_need_gc_pause;
  fts->fuel_pointer = &scheme_fuel_counter;
  fts->stack_boundary_pointer = &scheme_jit_stack_boundary;

  MZ_RUNSTACK_START = params->runstack_start;
  MZ_RUNSTACK = MZ_RUNSTACK_START + fts->runstack_size;

  params->scheme_current_runstack_ptr = &scheme_current_runstack;
  params->scheme_current_runstack_start_ptr = &scheme_current_runstack_start;
  params->current_thread_ptr = &scheme_current_thread;
  params->jit_future_storage_ptr = &jit_future_storage[0];

  sema_signal(&params->ready_sema);

 wait_for_work:
  pthread_mutex_lock(&fs->future_mutex);
  start_gc_not_ok(fs);
  while (!(ft = get_pending_future(fs))) {
    end_gc_not_ok(fts, fs, NULL);
    pthread_cond_wait(&fs->future_pending_cv, &fs->future_mutex);
    start_gc_not_ok(fs);
  }

  LOG("Got a signal that a future is pending...");
        
  //Work is available for this thread
  ft->status = RUNNING;
  pthread_mutex_unlock(&fs->future_mutex);

  ft->threadid = fts->threadid;
  ft->thread_short_id = fts->id;

  //Set up the JIT compiler for this thread 
  scheme_jit_fill_threadlocal_table();
        
  jitcode = (Scheme_Object* (*)(Scheme_Object*, int, Scheme_Object**))(ft->code);

  fts->current_ft = ft;

  //Run the code
  //Passing no arguments for now.
  //The lambda passed to a future will always be a parameterless
  //function.
  //From this thread's perspective, this call will never return
  //until all the work to be done in the future has been completed,
  //including runtime calls. 
  //If jitcode asks the runrtime thread to do work, then
  //a GC can occur.
  LOG("Running JIT code at %p...\n", ft->code);    

  scheme_current_thread->error_buf = &newbuf;
  if (scheme_future_setjmp(newbuf)) {
    /* failed */
    v = NULL;
  } else {
    v = jitcode(ft->orig_lambda, 0, NULL);
  }

  LOG("Finished running JIT code at %p.\n", ft->code);

  // Get future again, since a GC may have occurred
  ft = fts->current_ft;

  //Set the return val in the descriptor
  pthread_mutex_lock(&fs->future_mutex);
  ft->work_completed = 1;
  ft->retval = v;

  //Update the status 
  ft->status = FINISHED;
  dequeue_future(fs, ft);

  scheme_signal_received_at(fs->signal_handle);

  end_gc_not_ok(fts, fs, NULL);

  pthread_mutex_unlock(&fs->future_mutex);

  goto wait_for_work;

  return NULL;
  END_XFORM_SKIP;
}

void scheme_check_future_work()
/* Called in the runtime thread by the scheduler */
{
  /* Check for work that future threads need from the runtime thread
     and that can be done in any Scheme thread (e.g., get a new page
     for allocation). */
  future_t *ft;
  Scheme_Future_State *fs = scheme_future_state;

  if (!fs) return;

  while (1) {
    /* Try to get a future waiting on a atomic operation */
    pthread_mutex_lock(&fs->future_mutex);
    ft = fs->future_waiting_atomic;
    if (ft) {
      fs->future_waiting_atomic = ft->next_waiting_atomic;
      ft->next_waiting_atomic = NULL;
      ft->waiting_atomic = 0;
    }
    pthread_mutex_unlock(&fs->future_mutex);

    if (ft) {
      if (ft->rt_prim && ft->rt_prim_is_atomic) {
        invoke_rtcall(fs, ft);
      }
    } else
      break;
  }
}

//Returns 0 if the call isn't actually executed by this function,
//i.e. if we are already running on the runtime thread.  Otherwise returns
//1, and 'retval' is set to point to the return value of the runtime
//call invocation.
static void future_do_runtimecall(Scheme_Future_Thread_State *fts,
                                  void *func,
                                  int is_atomic)
/* Called in future thread */
{
  START_XFORM_SKIP;
  future_t *future;
  Scheme_Future_State *fs = scheme_future_state;

  //Fetch the future descriptor for this thread
  future = fts->current_ft;

  //set up the arguments for the runtime call
  //to be picked up by the main rt thread
  pthread_mutex_lock(&fs->future_mutex);

  future->prim_func = func;
  future->rt_prim = 1;
  future->rt_prim_is_atomic = is_atomic;

  if (is_atomic) {
    if (!future->waiting_atomic) {
      future->next_waiting_atomic = fs->future_waiting_atomic;
      fs->future_waiting_atomic = future;
      future->waiting_atomic = 1;
    }
  }

  //Update the future's status to waiting 
  future->status = WAITING_FOR_PRIM;

  scheme_signal_received_at(fs->signal_handle);

  //Wait for the signal that the RT call is finished
  future->can_continue_cv = &fts->worker_can_continue_cv;
  while (future->can_continue_cv) {
    end_gc_not_ok(fts, fs, MZ_RUNSTACK);
    pthread_cond_wait(&fts->worker_can_continue_cv, &fs->future_mutex);
    start_gc_not_ok(fs);
    //Fetch the future instance again, in case the GC has moved the pointer
    future = fts->current_ft;
  }

  pthread_mutex_unlock(&fs->future_mutex);

  if (future->no_retval) {
    future->no_retval = 0;
    scheme_future_longjmp(*scheme_current_thread->error_buf, 1);
  }

  END_XFORM_SKIP;
}


/**********************************************************************/
/* Functions for primitive invocation                   			  */
/**********************************************************************/
void scheme_rtcall_void_void_3args(const char *who, int src_type, prim_void_void_3args_t f)
/* Called in future thread */
{
  START_XFORM_SKIP;
  Scheme_Future_Thread_State *fts = scheme_future_thread_state;
  future_t *future = fts->current_ft;

  future->prim_protocol = SIG_VOID_VOID_3ARGS;

  future->arg_S0 = MZ_RUNSTACK;

  future->time_of_request = scheme_get_inexact_milliseconds();
  future->source_of_request = who;
  future->source_type = src_type;

  future_do_runtimecall(fts, (void*)f, 1);

  future->arg_S0 = NULL;

  END_XFORM_SKIP;
}

unsigned long scheme_rtcall_alloc_void_pvoid(const char *who, int src_type, prim_alloc_void_pvoid_t f)
/* Called in future thread */
{
  START_XFORM_SKIP;
  future_t *future;
  unsigned long retval;
  Scheme_Future_Thread_State *fts = scheme_future_thread_state;
  Scheme_Future_State *fs = scheme_future_state;

  while (1) {
    future = fts->current_ft;
    future->time_of_request = 0; /* takes too long?: scheme_get_inexact_milliseconds(); */
    future->source_of_request = who;
    future->source_type = src_type;
  
    future->prim_protocol = SIG_ALLOC_VOID_PVOID;

    future_do_runtimecall(fts, (void*)f, 1);

    future = fts->current_ft;
    retval = future->alloc_retval;
    future->alloc_retval = 0;

    if (*fs->gc_counter_ptr == future->alloc_retval_counter)
      break;
  }

  return retval;
  END_XFORM_SKIP;
}

static void receive_special_result(future_t *f, Scheme_Object *retval)
/* Called in future thread */
{
  if (SAME_OBJ(retval, SCHEME_MULTIPLE_VALUES)) {
    Scheme_Thread *p = scheme_current_thread;

    p->ku.multiple.array = f->multiple_array;
    p->ku.multiple.count = f->multiple_count;
    f->multiple_array = NULL;
  } else if (SAME_OBJ(retval, SCHEME_TAIL_CALL_WAITING)) {
    Scheme_Thread *p = scheme_current_thread;

    p->ku.apply.tail_rator = f->tail_rator;
    p->ku.apply.tail_rands = f->tail_rands;
    p->ku.apply.tail_num_rands = f->num_tail_rands;
  }
}

#include "jit_ts_future_glue.c"

static void send_special_result(future_t *f, Scheme_Object *retval)
{
  if (SAME_OBJ(retval, SCHEME_MULTIPLE_VALUES)) {
    Scheme_Thread *p = scheme_current_thread;

    f->multiple_array = p->ku.multiple.array;
    f->multiple_count = p->ku.multiple.count;
    if (SAME_OBJ(p->ku.multiple.array, p->values_buffer))
      p->values_buffer = NULL;
  } else if (SAME_OBJ(retval, SCHEME_TAIL_CALL_WAITING)) {
    Scheme_Thread *p = scheme_current_thread;

    f->tail_rator = p->ku.apply.tail_rator;
    f->tail_rands = p->ku.apply.tail_rands;
    f->num_tail_rands = p->ku.apply.tail_num_rands;
    p->ku.apply.tail_rator = NULL;
    p->ku.apply.tail_rands = NULL;
  }
}

//Does the work of actually invoking a primitive on behalf of a 
//future.  This function is always invoked on the main (runtime) 
//thread.
static void do_invoke_rtcall(Scheme_Future_State *fs, future_t *future)
/* Called in runtime thread */
{
#ifdef DEBUG_FUTURES
  g_rtcall_count++;
#endif

  future->rt_prim = 0;

  if (scheme_log_level_p(scheme_main_logger, SCHEME_LOG_DEBUG)) {
    const char *src;

    src = future->source_of_request;
    if (future->source_type == FSRC_RATOR) {
      int len;
      if (SCHEME_PROCP(future->arg_s0))
        src = scheme_get_proc_name(future->arg_s0, &len, 1);
    } else if (future->source_type == FSRC_PRIM) {
      const char *src2;
      src2 = scheme_look_for_primitive(future->prim_func);
      if (src2) src = src2;
    }

    scheme_log(scheme_main_logger, SCHEME_LOG_DEBUG, 0,
               "future: %d waiting for runtime at %f: %s",
               (long)future->thread_short_id,
               future->time_of_request,
               src);
  }
  
  switch (future->prim_protocol)
    {
    case SIG_VOID_VOID_3ARGS:
      {
        prim_void_void_3args_t func = (prim_void_void_3args_t)future->prim_func;

        func(future->arg_S0);

        break;
      }
    case SIG_ALLOC_VOID_PVOID:
      {
        unsigned long ret;
        prim_alloc_void_pvoid_t func = (prim_alloc_void_pvoid_t)future->prim_func;
        ret = func();
        future->alloc_retval = ret;
        future->alloc_retval_counter = scheme_did_gc_count;
        break;
      }
# include "jit_ts_runtime_glue.c"
    default:
      scheme_signal_error("unknown protocol %d", future->prim_protocol);
      break;
    }

  pthread_mutex_lock(&fs->future_mutex);
  //Signal the waiting worker thread that it
  //can continue running machine code
  if (future->can_continue_cv) {
    pthread_cond_signal(future->can_continue_cv);
    future->can_continue_cv= NULL;
  }
  pthread_mutex_unlock(&fs->future_mutex);
}

static void invoke_rtcall(Scheme_Future_State * volatile fs, future_t * volatile future)
{
  Scheme_Thread *p = scheme_current_thread;
  mz_jmp_buf newbuf, * volatile savebuf;

  savebuf = p->error_buf;
  p->error_buf = &newbuf;
  if (scheme_setjmp(newbuf)) {
    pthread_mutex_lock(&fs->future_mutex);
    future->no_retval = 1;
    //Signal the waiting worker thread that it
    //can continue running machine code
    pthread_cond_signal(future->can_continue_cv);
    future->can_continue_cv = NULL;
    pthread_mutex_unlock(&fs->future_mutex);
    scheme_longjmp(*savebuf, 1);
  } else {
    do_invoke_rtcall(fs, future);
  }
  p->error_buf = savebuf;
}


/**********************************************************************/
/* Helpers for manipulating the futures queue                         */
/**********************************************************************/

future_t *enqueue_future(Scheme_Future_State *fs, future_t *ft)
/* Called in runtime thread */
{
  if (fs->future_queue_end) {
    fs->future_queue_end->next = ft;
    ft->prev = fs->future_queue_end;
  }
  fs->future_queue_end = ft;
  if (!fs->future_queue)
    fs->future_queue = ft;
  fs->future_queue_count++;
    
  return ft;
}

future_t *get_pending_future(Scheme_Future_State *fs)
/* Called in future thread */
{
  START_XFORM_SKIP;
  future_t *f;

  for (f = fs->future_queue; f != NULL; f = f->next) {
    if (f->status == PENDING)
      return f;
  }

  return NULL;
  END_XFORM_SKIP;
}

/**********************************************************************/
/*                           Precise GC                               */
/**********************************************************************/

#ifdef MZ_PRECISE_GC

START_XFORM_SKIP;

#define MARKS_FOR_FUTURE_C
#include "mzmark.c"

static void register_traversers(void)
{
  GC_REG_TRAV(scheme_future_type, future);
}

END_XFORM_SKIP;

#endif

#endif
