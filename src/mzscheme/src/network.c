/*
  MzScheme
  Copyright (c) 2004-2006 PLT Scheme Inc.
  Copyright (c) 2000-2001 Matthew Flatt

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

  libscheme
  Copyright (c) 1994 Brent Benson
  All rights reserved.
*/

/* This file implements the TCP and UDP interfaces. */

#include "schpriv.h"
#include <ctype.h>

#ifndef NO_TCP_SUPPORT
#ifdef USE_TCP

#ifdef UNISTD_INCLUDE
# include <unistd.h>
#endif
#ifdef USE_ULIMIT
# include <ulimit.h>
#endif
#ifdef FILES_HAVE_FDS
# include <sys/types.h>
# include <sys/time.h>
# ifdef BSTRING_INCLUDE
#  include <bstring.h>
# endif
# ifdef SELECT_INCLUDE
#  include <sys/select.h>
# endif
#endif
#ifdef IO_INCLUDE
# include <io.h>
#endif
#ifdef INCLUDE_OSKIT_SOCKET
# include <oskit/net/socket.h>
#endif
#ifdef NO_ERRNO_GLOBAL
static int mzerrno = 0;
# define errno mzerrno
#else
# include <errno.h>
#endif
#include "schfd.h"

#ifdef USE_UNIX_SOCKETS_TCP
# include <netinet/in.h>
# include <netdb.h>
# include <sys/socket.h>
# include <fcntl.h>
# define TCP_SOCKSENDBUF_SIZE 32768
# define NOT_WINSOCK(x) x
# define SOCK_ERRNO() errno
# define WAS_EAGAIN(e) ((e == EWOULDBLOCK) || (e == EAGAIN) || (e == EINPROGRESS) || (e == EALREADY))
# define WAS_WSAEMSGSIZE(e) 0
# define mz_AFNOSUPPORT EAFNOSUPPORT
#endif

#ifdef USE_WINSOCK_TCP
# include <process.h>
# include <winsock2.h>
# include <ws2tcpip.h>
struct SOCKADDR_IN {
  short sin_family;
  unsigned short sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};
# define NOT_WINSOCK(x) 0
# define SOCK_ERRNO() WSAGetLastError()
# define WAS_EAGAIN(e) ((e == WSAEWOULDBLOCK) || (e == WSAEINPROGRESS))
# define WAS_WSAEMSGSIZE(e) (e == WSAEMSGSIZE)
# define mz_AFNOSUPPORT WSAEAFNOSUPPORT
extern int scheme_stupid_windows_machine;
#endif

#define TCP_BUFFER_SIZE 4096

#ifdef USE_UNIX_SOCKETS_TCP
typedef long tcp_t;
# define INVALID_SOCKET (-1)
static void closesocket(long s) {
  int cr;
  do { 
    cr = close(s);
  } while ((cr == -1) && (errno == EINTR));
}
#endif

#ifdef USE_WINSOCK_TCP
typedef SOCKET tcp_t;
#endif

#ifdef USE_SOCKETS_TCP
typedef struct {
  Scheme_Object so;
  Scheme_Custodian_Reference *mref;
  int count;
  tcp_t s[1];
} listener_t;
#endif

typedef struct Scheme_Tcp_Buf {
  MZTAG_IF_REQUIRED
  short refcount;
  char *buffer, *out_buffer;
  short bufpos, bufmax;
  short hiteof, bufmode;
  short out_bufpos, out_bufmax;
  short out_bufmode;
} Scheme_Tcp_Buf;

typedef struct Scheme_Tcp {
  Scheme_Tcp_Buf b;
  tcp_t tcp;
  int flags;
} Scheme_Tcp;

# define MZ_TCP_ABANDON_OUTPUT 0x1
# define MZ_TCP_ABANDON_INPUT  0x2

#define UDP_IS_SUPPORTED

#ifdef UDP_IS_SUPPORTED

typedef struct Scheme_UDP {
  Scheme_Object so; /* scheme_udp_type */
  MZ_HASH_KEY_EX
  tcp_t s; /* ok, tcp_t was a bad name */
  char bound, connected;
  Scheme_Object *previous_from_addr;
  Scheme_Custodian_Reference *mref;
} Scheme_UDP;

#endif /* UDP_IS_SUPPORTED */

#endif /* USE_TCP */

static Scheme_Object *tcp_connect(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_connect_break(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_listen(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_stop(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_accept_ready(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_accept(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_accept_evt(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_accept_break(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_listener_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_addresses(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_abandon_port(int argc, Scheme_Object *argv[]);
static Scheme_Object *tcp_port_p(int argc, Scheme_Object *argv[]);

static Scheme_Object *make_udp(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_close(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_bound_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_connected_p(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_bind(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_connect(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_send_to(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_send(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_send_to_star(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_send_star(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_send_to_enable_break(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_send_enable_break(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_receive(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_receive_star(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_receive_enable_break(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_read_ready_evt(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_write_ready_evt(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_read_evt(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_write_evt(int argc, Scheme_Object *argv[]);
static Scheme_Object *udp_write_to_evt(int argc, Scheme_Object *argv[]);

static int tcp_check_accept_evt(Scheme_Object *ae, Scheme_Schedule_Info *sinfo);
static void tcp_accept_evt_needs_wakeup(Scheme_Object *_ae, void *fds);
#ifdef UDP_IS_SUPPORTED
static int udp_evt_check_ready(Scheme_Object *uw, Scheme_Schedule_Info *sinfo);
static void udp_evt_needs_wakeup(Scheme_Object *_uw, void *fds);
#endif

static void register_tcp_listener_sync();

#ifdef MZ_PRECISE_GC
static void register_traversers(void);
#endif

void scheme_init_network(Scheme_Env *env)
{
#ifdef MZ_PRECISE_GC
  register_traversers();
#endif

  scheme_add_global_constant("tcp-connect", 
			     scheme_make_prim_w_arity2(tcp_connect,
						       "tcp-connect", 
						       2, 4,
						       2, 2), 
			     env);
  scheme_add_global_constant("tcp-connect/enable-break", 
			     scheme_make_prim_w_arity2(tcp_connect_break,
						       "tcp-connect/enable-break", 
						       2, 4,
						       2, 2), 
			     env);
  scheme_add_global_constant("tcp-listen", 
			     scheme_make_prim_w_arity(tcp_listen,
						      "tcp-listen", 
						      1, 4),
			     env);
  scheme_add_global_constant("tcp-close", 
			     scheme_make_prim_w_arity(tcp_stop,
						      "tcp-close", 
						      1, 1), 
			     env);
  scheme_add_global_constant("tcp-accept-ready?", 
			     scheme_make_prim_w_arity(tcp_accept_ready,
						      "tcp-accept-ready?", 
						      1, 1), 
			     env);
  scheme_add_global_constant("tcp-accept", 
			     scheme_make_prim_w_arity2(tcp_accept,
						       "tcp-accept", 
						       1, 1,
						       2, 2), 
			     env);
  scheme_add_global_constant("tcp-accept-evt", 
			     scheme_make_prim_w_arity(tcp_accept_evt,
						      "tcp-accept-evt", 
						      1, 1), 
			     env);
  scheme_add_global_constant("tcp-accept/enable-break", 
			     scheme_make_prim_w_arity2(tcp_accept_break,
						       "tcp-accept/enable-break", 
						       1, 1,
						       2, 2), 
			     env);
  scheme_add_global_constant("tcp-listener?", 
			     scheme_make_folding_prim(tcp_listener_p,
						      "tcp-listener?", 
						      1, 1, 1), 
			     env);
  scheme_add_global_constant("tcp-addresses", 
			     scheme_make_prim_w_arity2(tcp_addresses,
						       "tcp-addresses", 
						       1, 1,
						       2, 2), 
			     env);
  scheme_add_global_constant("tcp-abandon-port", 
			     scheme_make_prim_w_arity(tcp_abandon_port,
						      "tcp-abandon-port", 
						      1, 1), 
			     env);
  scheme_add_global_constant("tcp-port?", 
			     scheme_make_folding_prim(tcp_port_p,
						      "tcp-port?", 
						      1, 1, 1), 
			     env);

  scheme_add_global_constant("udp-open-socket", 
			     scheme_make_prim_w_arity(make_udp,
						      "udp-open-socket", 
						      0, 2), 
			     env);
  scheme_add_global_constant("udp-close", 
			     scheme_make_prim_w_arity(udp_close,
						      "udp-close", 
						      1, 1), 
			     env);
  scheme_add_global_constant("udp?", 
			     scheme_make_folding_prim(udp_p,
						      "udp?", 
						      1, 1, 1), 
			     env);
  scheme_add_global_constant("udp-bound?", 
			     scheme_make_prim_w_arity(udp_bound_p,
						      "udp-bound?", 
						      1, 1), 
			     env);
  scheme_add_global_constant("udp-connected?", 
			     scheme_make_prim_w_arity(udp_connected_p,
						      "udp-connected?", 
						      1, 1), 
			     env);

  scheme_add_global_constant("udp-bind!", 
			     scheme_make_prim_w_arity(udp_bind,
						      "udp-bind!", 
						      3, 3), 
			     env);
  scheme_add_global_constant("udp-connect!", 
			     scheme_make_prim_w_arity(udp_connect,
						      "udp-connect!", 
						      3, 3), 
			     env);

  scheme_add_global_constant("udp-send-to", 
			     scheme_make_prim_w_arity(udp_send_to,
						      "udp-send-to", 
						      4, 6), 
			     env);
  scheme_add_global_constant("udp-send", 
			     scheme_make_prim_w_arity(udp_send,
						      "udp-send", 
						      2, 4), 
			     env);
  scheme_add_global_constant("udp-send-to*", 
			     scheme_make_prim_w_arity(udp_send_to_star,
						      "udp-send-to*", 
						      4, 6), 
			     env);
  scheme_add_global_constant("udp-send*", 
			     scheme_make_prim_w_arity(udp_send_star,
						      "udp-send*", 
						      2, 4), 
			     env);
  scheme_add_global_constant("udp-send-to/enable-break", 
			     scheme_make_prim_w_arity(udp_send_to_enable_break,
						      "udp-send-to/enable-break", 
						      4, 6), 
			     env);
  scheme_add_global_constant("udp-send/enable-break", 
			     scheme_make_prim_w_arity(udp_send_enable_break,
						      "udp-send/enable-break", 
						      2, 4), 
			     env);

  scheme_add_global_constant("udp-receive!", 
			     scheme_make_prim_w_arity(udp_receive,
						      "udp-receive!", 
						      2, 4), 
			     env);
  scheme_add_global_constant("udp-receive!*", 
			     scheme_make_prim_w_arity(udp_receive_star,
						      "udp-receive!*", 
						      2, 4), 
			     env);
  scheme_add_global_constant("udp-receive!/enable-break", 
			     scheme_make_prim_w_arity(udp_receive_enable_break,
						      "udp-receive!/enable-break", 
						      2, 4), 
			     env);
  scheme_add_global_constant("udp-receive-ready-evt", 
			     scheme_make_prim_w_arity(udp_read_ready_evt,
						      "udp-receive-ready-evt", 
						      1, 1), 
			     env);
  scheme_add_global_constant("udp-send-ready-evt", 
			     scheme_make_prim_w_arity(udp_write_ready_evt,
						      "udp-send-ready-evt", 
						      1, 1), 
			     env);
  scheme_add_global_constant("udp-receive!-evt", 
			     scheme_make_prim_w_arity(udp_read_evt,
						      "udp-receive!-evt", 
						      2, 4), 
			     env);
  scheme_add_global_constant("udp-send-evt", 
			     scheme_make_prim_w_arity(udp_write_evt,
						      "udp-send-evt", 
						      2, 4), 
			     env);
  scheme_add_global_constant("udp-send-to-evt", 
			     scheme_make_prim_w_arity(udp_write_to_evt,
						      "udp-send-to-evt", 
						      4, 6), 
			     env);

  register_tcp_listener_sync();
}


/*========================================================================*/
/*                             TCP glue                                   */
/*========================================================================*/


/* These two need o be outside of USE_TCP */
#define PORT_ID_TYPE "exact integer in [1, 65535]"
#define CHECK_PORT_ID(obj) (SCHEME_INTP(obj) && (SCHEME_INT_VAL(obj) >= 1) && (SCHEME_INT_VAL(obj) <= 65535))

#ifdef USE_TCP

#ifdef USE_SOCKETS_TCP
#define MAKE_TCP_ARG tcp_t tcp, 
#else
#define MAKE_TCP_ARG
#endif

#define REGISTER_SOCKET(s) /**/
#define UNREGISTER_SOCKET(s) /**/

#ifdef USE_UNIX_SOCKETS_TCP
typedef struct sockaddr_in mz_unspec_address;
#endif
#ifdef USE_WINSOCK_TCP
typedef struct SOCKADDR_IN mz_unspec_address;
# undef REGISTER_SOCKET
# undef UNREGISTER_SOCKET
# define REGISTER_SOCKET(s) winsock_remember(s)
# define UNREGISTER_SOCKET(s) winsock_forget(s)
#endif

/******************************* hostnames ************************************/

#ifdef OS_X
# define PTHREADS_OK_FOR_GHBN
#endif

# ifdef PROTOENT_IS_INT
#  define PROTO_P_PROTO PROTOENT_IS_INT
# else
static struct protoent *proto;
#  define PROTO_P_PROTO (proto ? proto->p_proto : 0)
# endif

# ifndef MZ_PF_INET
#  define MZ_PF_INET PF_INET
# endif

/* For getting connection names: */
#define MZ_SOCK_NAME_MAX_LEN 256
#define MZ_SOCK_HOST_NAME_MAX_LEN 64
#define MZ_SOCK_SVC_NAME_MAX_LEN 32

/* mz_addrinfo is defined in scheme.h */

#ifdef HAVE_GETADDRINFO
# define mzAI_PASSIVE AI_PASSIVE 
# define mz_getaddrinfo getaddrinfo
# define mz_freeaddrinfo freeaddrinfo
# define mz_gai_strerror gai_strerror
#else
# define mzAI_PASSIVE 0
static int mz_getaddrinfo(const char *nodename, const char *servname,
			  const struct mz_addrinfo *hints, struct mz_addrinfo **res)
{
  struct hostent *h;

  if (nodename)
    h = gethostbyname(nodename);
  else
    h = NULL;

  if (h || !nodename) {
    GC_CAN_IGNORE struct mz_addrinfo *ai;
    GC_CAN_IGNORE struct sockaddr_in *sa;
    int j, id = 0;

    ai = (struct mz_addrinfo *)malloc(sizeof(struct mz_addrinfo));
    sa = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    ai->ai_addr = (struct sockaddr *)sa;

    ai->ai_addrlen = sizeof(struct sockaddr_in);
    if (servname) {
      for (j = 0; servname[j]; j++) {
	id = (id * 10) + (servname[j] - '0');
      }
    }

    ai->ai_family = MZ_PF_INET;
    ai->ai_socktype = hints->ai_socktype;
    ai->ai_protocol = hints->ai_protocol;
    ai->ai_next = NULL;

    sa->sin_family = (id ? AF_INET : AF_UNSPEC);
    j = htons(id);
    sa->sin_port = j;
    memset(&(sa->sin_addr), 0, sizeof(sa->sin_addr));
    memset(&(sa->sin_zero), 0, sizeof(sa->sin_zero));
    if (h)
      memcpy(&sa->sin_addr, h->h_addr_list[0], h->h_length); 
    
    *res = ai;
    return 0;
  }
  return h_errno;
}
void mz_freeaddrinfo(struct mz_addrinfo *ai)
{
  free(ai->ai_addr);
  free(ai);
}
const char *mz_gai_strerror(int ecode)
{
  return hstrerror(ecode);
}
#endif

#if defined(USE_WINSOCK_TCP) || defined(PTHREADS_OK_FOR_GHBN)

# ifdef USE_WINSOCK_TCP
#  ifdef __BORLANDC__
#   define MZ_LPTHREAD_START_ROUTINE unsigned int (__stdcall*)(void*)
#  else
#   define MZ_LPTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE
#  endif
# else
#  include <pthread.h>
#   define MZ_LPTHREAD_START_ROUTINE void *(*)(void *)
# endif

static volatile int ghbn_lock;

typedef struct {
# ifdef USE_WINSOCK_TCP
  HANDLE th;
# else
  int pin;
# endif
  struct mz_addrinfo *result;
  int err;
  int done;
} GHBN_Rec;

static struct mz_addrinfo * volatile ghbn_result;
static volatile int ghbn_err;

/* For in-thread DNS: */
#define MZ_MAX_HOSTNAME_LEN 128
#define MZ_MAX_SERVNAME_LEN 32

static char ghbn_hostname[MZ_MAX_HOSTNAME_LEN];
static char ghbn_servname[MZ_MAX_SERVNAME_LEN];
static struct mz_addrinfo ghbn_hints;
# ifdef USE_WINSOCK_TCP
HANDLE ready_sema;
# else
int ready_fd;
# endif

#ifdef MZ_XFORM
START_XFORM_SKIP;
#endif

static long getaddrinfo_in_thread(void *data)
{
  int ok;
  struct mz_addrinfo *res, hints;
  char hn_copy[MZ_MAX_HOSTNAME_LEN], sn_copy[MZ_MAX_SERVNAME_LEN];
# ifndef USE_WINSOCK_TCP
  int fd = ready_fd;
# endif
  
  if (ghbn_result) {
    mz_freeaddrinfo(ghbn_result);
    ghbn_result = NULL;
  }

  strcpy(hn_copy, ghbn_hostname);
  strcpy(sn_copy, ghbn_servname);
  memcpy(&hints, &ghbn_hints, sizeof(hints));

# ifdef USE_WINSOCK_TCP
  ReleaseSemaphore(ready_sema, 1, NULL);  
# else
  write(fd, "?", 1);
# endif

  ok = mz_getaddrinfo(hn_copy[0] ? hn_copy : NULL, 
		      sn_copy[0] ? sn_copy : NULL, 
		      &hints, &res);

  ghbn_result = res;
  ghbn_err = ok;

# ifndef USE_WINSOCK_TCP
  {
    long v = 1;
    write(fd, &v, sizeof(v));
    close(fd);
  }
# endif

  return 1;
}

#ifdef MZ_XFORM
END_XFORM_SKIP;
#endif

static void release_ghbn_lock(GHBN_Rec *rec)
{
  ghbn_lock = 0;
# ifdef USE_WINSOCK_TCP
  CloseHandle(rec->th);
# else
  close(rec->pin);
# endif
}

static int ghbn_lock_avail(Scheme_Object *_ignored)
{
  return !ghbn_lock;
}

static int ghbn_thread_done(Scheme_Object *_rec)
{
  GHBN_Rec *rec = (GHBN_Rec *)_rec;

  if (rec->done)
    return 1;

# ifdef USE_WINSOCK_TCP
  if (WaitForSingleObject(rec->th, 0) == WAIT_OBJECT_0) {
    rec->result = ghbn_result;
    ghbn_result = NULL;
    rec->err = ghbn_err;
    rec->done = 1;
    return 1;
  }
# else
  {
    long v;
    if (read(rec->pin, &v, sizeof(long)) > 0) {
      rec->result = ghbn_result;
      ghbn_result = NULL;
      rec->err = ghbn_err;
      rec->done = 1;
      return 1;
    }
  }
# endif

  return 0;
}

static void ghbn_thread_need_wakeup(Scheme_Object *_rec, void *fds)
{
  GHBN_Rec *rec = (GHBN_Rec *)_rec;

# ifdef USE_WINSOCK_TCP
  scheme_add_fd_handle((void *)rec->th, fds, 0);
# else
  {
    void *fds2;
    
    fds2 = MZ_GET_FDSET(fds, 2);
    
    MZ_FD_SET(rec->pin, (fd_set *)fds);
    MZ_FD_SET(rec->pin, (fd_set *)fds2);
  }
# endif
}

static int MZ_GETADDRINFO(const char *name, const char *svc, struct mz_addrinfo *hints, struct mz_addrinfo **res)
{
  GHBN_Rec *rec;
  int ok;

  if ((name && ((strlen(name) >= MZ_MAX_HOSTNAME_LEN) || !name[0]))
      || (svc && ((strlen(svc) >= MZ_MAX_SERVNAME_LEN) || !svc[0]))) {
    /* Give up on a separate thread. */
    return mz_getaddrinfo(name, svc, hints, res);
  }

  rec = MALLOC_ONE_ATOMIC(GHBN_Rec);
  rec->done = 0;

  scheme_block_until(ghbn_lock_avail, NULL, NULL, 0);

  ghbn_lock = 1;

  if (name)
    strcpy(ghbn_hostname, name);
  else
    ghbn_hostname[0] = 0;
  if (svc)
    strcpy(ghbn_servname, svc);
  else
    ghbn_servname[0] = 0;
  memcpy(&ghbn_hints, hints, sizeof(ghbn_hints));

# ifdef USE_WINSOCK_TCP
  {
    DWORD id;
    long th;
    
    ready_sema = CreateSemaphore(NULL, 0, 1, NULL);
    th = _beginthreadex(NULL, 5000, 
			(MZ_LPTHREAD_START_ROUTINE)getaddrinfo_in_thread,
			NULL, 0, &id);
    WaitForSingleObject(ready_sema, INFINITE);
    CloseHandle(ready_sema);
    
    rec->th = (HANDLE)th;
    ok = 1;
  }
# else
  {
    int p[2];
    if (pipe(p)) {
      ok = 0;
    } else {
      pthread_t t;
      rec->pin = p[0];
      ready_fd = p[1];
      if (pthread_create(&t, NULL, 
			 (MZ_LPTHREAD_START_ROUTINE)getaddrinfo_in_thread,
			 NULL)) {
	close(p[0]);
	close(p[1]);
	ok = 0;
      } else {
	char buf[1];
	pthread_detach(t);
	read(rec->pin, buf, 1);
	fcntl(rec->pin, F_SETFL, MZ_NONBLOCKING);
	ok = 1;
      }
    }

    if (!ok) {
      getaddrinfo_in_thread(rec);
      rec->result = ghbn_result;
      ghbn_result = NULL;
      rec->err = ghbn_err;
    }
  }
# endif

  if (ok) {
    BEGIN_ESCAPEABLE(release_ghbn_lock, rec);
    scheme_block_until(ghbn_thread_done, ghbn_thread_need_wakeup, (Scheme_Object *)rec, 0);
    END_ESCAPEABLE();

# ifdef USE_WINSOCK_TCP
    CloseHandle(rec->th);
# else
    close(rec->pin);
# endif
  }

  ghbn_lock = 0;

  *res = rec->result;

  return rec->err;
}
#else
# define MZ_GETADDRINFO mz_getaddrinfo
#endif

#ifdef USE_SOCKETS_TCP

struct mz_addrinfo *scheme_get_host_address(const char *address, int id, int *err, 
					    int family, int passive, int tcp)
{
  char buf[32], *service;
  int ok;
  GC_CAN_IGNORE struct mz_addrinfo *r, hints;

  if (id) {
    service = buf;
    sprintf(buf, "%d", id);
  } else
    service = NULL;
  
  if (!address && !service) {
    *err = -1;
    return NULL;
  }

  memset(&hints, 0, sizeof(struct mz_addrinfo));
  hints.ai_family = ((family < 0) ? PF_UNSPEC : family);
  if (passive) {
    hints.ai_flags |= mzAI_PASSIVE;
  }
  if (tcp) {
    hints.ai_socktype = SOCK_STREAM;
# ifndef PROTOENT_IS_INT
    if (!proto) {
      proto = getprotobyname("tcp");
    }
# endif
    hints.ai_protocol= PROTO_P_PROTO;
  } else {
    hints.ai_socktype = SOCK_DGRAM;
  }

  ok = MZ_GETADDRINFO(address, service, &hints, &r);
  *err = ok;
  
  if (!ok)
    return r;
  else
    return NULL;
}

void scheme_free_host_address(struct mz_addrinfo *a)
{
  mz_freeaddrinfo(a);
}

const char *scheme_host_address_strerror(int errnum)
{
  return mz_gai_strerror(errnum);
}
#endif

/******************************* WinSock ***********************************/

#ifdef USE_WINSOCK_TCP

static int wsr_size = 0;
static tcp_t *wsr_array;

static void winsock_remember(tcp_t s)
{
  int i, new_size;
  tcp_t *naya;

  for (i = 0; i < wsr_size; i++) {
    if (!wsr_array[i]) {
      wsr_array[i] = s;
      return;
    }
  }

  if (!wsr_size) {
    REGISTER_SO(wsr_array);
    new_size = 32;
  } else
    new_size = 2 * wsr_size;

  naya = MALLOC_N_ATOMIC(tcp_t, new_size);
  for (i = 0; i < wsr_size; i++) {
    naya[i] = wsr_array[i];
  }

  naya[wsr_size] = s;

  wsr_array = naya;
  wsr_size = new_size;  
}

static void winsock_forget(tcp_t s)
{
  int i;

  for (i = 0; i < wsr_size; i++) {
    if (wsr_array[i] == s) {
      wsr_array[i] = (tcp_t)NULL;
      return;
    }
  }
}

static int winsock_done(void)
{
  int i;

  for (i = 0; i < wsr_size; i++) {
    if (wsr_array[i]) {
      closesocket(wsr_array[i]);
      wsr_array[i] = (tcp_t)NULL;
    }
  }

  return WSACleanup();
}

static void TCP_INIT(char *name)
{
  static int started = 0;
  
  if (!started) {
    WSADATA data;
    if (!WSAStartup(MAKEWORD(1, 1), &data)) {
      started = 1;
#ifdef __BORLANDC__
      atexit((void(*)())winsock_done);
#else      
      _onexit(winsock_done);
#endif
      return;
    }
  } else
    return;
  
  scheme_raise_exn(MZEXN_FAIL_UNSUPPORTED,
		   "%s: not supported on this machine"
		   " (no winsock driver)",
		   name);
}
#else
/* Not Winsock */
# define TCP_INIT(x) /* nothing */
#endif

/*========================================================================*/
/*                       TCP ports and listeners                          */
/*========================================================================*/

#ifdef USE_SOCKETS_TCP
#define LISTENER_WAS_CLOSED(x) (((listener_t *)(x))->s[0] == INVALID_SOCKET)
#endif
#ifndef LISTENER_WAS_CLOSED
#define LISTENER_WAS_CLOSED(x) 0
#endif

/* Forward declaration */
static int stop_listener(Scheme_Object *o);

static int tcp_check_accept(Scheme_Object *_listener)
{
#ifdef USE_SOCKETS_TCP
  tcp_t s, mx;
  listener_t *listener = (listener_t *)_listener;
  DECL_FDSET(readfds, 1);
  DECL_FDSET(exnfds, 1);
  struct timeval time = {0, 0};
  int sr, i;

  INIT_DECL_FDSET(readfds, 1);
  INIT_DECL_FDSET(exnfds, 1);

  if (LISTENER_WAS_CLOSED(listener))
    return 1;

  MZ_FD_ZERO(readfds);
  MZ_FD_ZERO(exnfds);

  mx = 0;
  for (i = 0; i < listener->count; i++) {
    s = listener->s[i];
    MZ_FD_SET(s, readfds);
    MZ_FD_SET(s, exnfds);
    if (s > mx)
      mx = s;
  }
  
  do {
    sr = select(mx + 1, readfds, NULL, exnfds, &time);
  } while ((sr == -1) && (NOT_WINSOCK(errno) == EINTR));

  if (sr) {
    for (i = 0; i < listener->count; i++) {
      s = listener->s[i];
      if (FD_ISSET(s, readfds)
	  || FD_ISSET(s, exnfds))
	return i + 1;
    }
  }

  return sr;
#endif
}

static void tcp_accept_needs_wakeup(Scheme_Object *_listener, void *fds)
{
#ifdef USE_SOCKETS_TCP
  if (!LISTENER_WAS_CLOSED(_listener)) {
    listener_t *listener = (listener_t *)_listener;
    int i;
    tcp_t s;
    void *fds2;

    fds2 = MZ_GET_FDSET(fds, 2);
    
    for (i = 0; i < listener->count; i++) {
      s = listener->s[i];
      MZ_FD_SET(s, (fd_set *)fds);
      MZ_FD_SET(s, (fd_set *)fds2);
    }
  }
#endif
}

static int tcp_check_connect(Scheme_Object *connector_p)
{
#ifdef USE_SOCKETS_TCP
  tcp_t s;
  DECL_FDSET(writefds, 1);
  DECL_FDSET(exnfds, 1);
  struct timeval time = {0, 0};
  int sr;

  INIT_DECL_FDSET(writefds, 1);
  INIT_DECL_FDSET(exnfds, 1);

  s = *(tcp_t *)connector_p;

  MZ_FD_ZERO(writefds);
  MZ_FD_ZERO(exnfds);

  MZ_FD_SET(s, writefds);
  MZ_FD_SET(s, exnfds);
    
  do {
    sr = select(s + 1, NULL, writefds, exnfds, &time);
  } while ((sr == -1) && (NOT_WINSOCK(errno) == EINTR));

  if (!sr)
    return 0;
  if (FD_ISSET(s, exnfds))
    return -1;
  else
    return 1;
#else
  return 0;
#endif
}

static void tcp_connect_needs_wakeup(Scheme_Object *connector_p, void *fds)
{
#ifdef USE_SOCKETS_TCP
  void *fds1, *fds2;
  tcp_t s = *(tcp_t *)connector_p;
  
  fds1 = MZ_GET_FDSET(fds, 1);
  fds2 = MZ_GET_FDSET(fds, 2);

  MZ_FD_SET(s, (fd_set *)fds1);
  MZ_FD_SET(s, (fd_set *)fds2);
#endif
}

static int tcp_check_write(Scheme_Object *port)
{
  Scheme_Tcp *data = (Scheme_Tcp *)((Scheme_Output_Port *)port)->port_data;

  if (((Scheme_Output_Port *)port)->closed)
    return 1;

#ifdef USE_SOCKETS_TCP
  {
    tcp_t s;
    DECL_FDSET(writefds, 1);
    DECL_FDSET(exnfds, 1);
    struct timeval time = {0, 0};
    int sr;
    
    INIT_DECL_FDSET(writefds, 1);
    INIT_DECL_FDSET(exnfds, 1);
    
    s = data->tcp;
    
    MZ_FD_ZERO(writefds);
    MZ_FD_SET(s, writefds);
    MZ_FD_ZERO(exnfds);
    MZ_FD_SET(s, exnfds);
    
    do {
      sr = select(s + 1, NULL, writefds, exnfds, &time);
    } while ((sr == -1) && (NOT_WINSOCK(errno) == EINTR));
    
    return sr;
  }
#else
  {
    TCPiopbX *xpb;
    TCPiopb *pb;
    int bytes;
    
    xpb = mac_make_xpb(data);
    pb = (TCPiopb *)xpb;
    
    pb->csCode = TCPStatus;
    if (mzPBControlSync((ParamBlockRec*)pb))
      bytes = -1;
    else {
      bytes = pb->csParam.status.sendWindow - pb->csParam.status.amtUnackedData;
      if (bytes < 0)
	bytes = 0;
    }
    
    return !!bytes;
  }
#endif
}

static void tcp_write_needs_wakeup(Scheme_Object *port, void *fds)
{
#ifdef USE_SOCKETS_TCP
  Scheme_Object *conn = ((Scheme_Output_Port *)port)->port_data;
  void *fds1, *fds2;
  tcp_t s = ((Scheme_Tcp *)conn)->tcp;
  
  fds1 = MZ_GET_FDSET(fds, 1);
  fds2 = MZ_GET_FDSET(fds, 2);
  
  MZ_FD_SET(s, (fd_set *)fds1);
  MZ_FD_SET(s, (fd_set *)fds2);
#endif
}


static Scheme_Tcp *make_tcp_port_data(MAKE_TCP_ARG int refcount)
{
  Scheme_Tcp *data;
  char *bfr;
  
  data = MALLOC_ONE_RT(Scheme_Tcp);
#ifdef MZTAG_REQUIRED
  data->b.type = scheme_rt_tcp;
#endif
#ifdef USE_SOCKETS_TCP
  data->tcp = tcp;
#endif

  bfr = (char *)scheme_malloc_atomic(TCP_BUFFER_SIZE);
  data->b.buffer = bfr;
  bfr = (char *)scheme_malloc_atomic(TCP_BUFFER_SIZE);
  data->b.out_buffer = bfr;

  data->b.bufpos = 0;
  data->b.bufmax = 0;
  data->b.hiteof = 0;
  data->b.refcount = refcount;

#ifdef USE_WINSOCK_TCP
  {
    unsigned long ioarg = 1;
    ioctlsocket(tcp, FIONBIO, &ioarg);
  }
#else
  fcntl(tcp, F_SETFL, MZ_NONBLOCKING);
#endif

  return data;
}

static int tcp_byte_ready (Scheme_Input_Port *port)
{
  Scheme_Tcp *data;
#ifdef USE_SOCKETS_TCP
  int sr;
  DECL_FDSET(readfds, 1);
  DECL_FDSET(exfds, 1);
  struct timeval time = {0, 0};

  INIT_DECL_FDSET(readfds, 1);
  INIT_DECL_FDSET(exfds, 1);
#endif

  if (port->closed)
    return 1;

  data = (Scheme_Tcp *)port->port_data;

  if (data->b.hiteof)
    return 1;
  if (data->b.bufpos < data->b.bufmax)
    return 1;

#ifdef USE_SOCKETS_TCP
  MZ_FD_ZERO(readfds);
  MZ_FD_ZERO(exfds);
  MZ_FD_SET(data->tcp, readfds);
  MZ_FD_SET(data->tcp, exfds);
    
  do {
    sr = select(data->tcp + 1, readfds, NULL, exfds, &time);
  } while ((sr == -1) && (NOT_WINSOCK(errno) == EINTR));

  return sr;
#endif

  return 0;
}

static long tcp_get_string(Scheme_Input_Port *port, 
			   char *buffer, long offset, long size,
			   int nonblock,
			   Scheme_Object *unless)
{
  int errid;
  int read_amt;
  Scheme_Tcp *data;

  data = (Scheme_Tcp *)port->port_data;

 top:

  if (scheme_unless_ready(unless))
    return SCHEME_UNLESS_READY;

  if (data->b.hiteof)
    return EOF;

  if (data->b.bufpos < data->b.bufmax) {
    int n;
    n = data->b.bufmax - data->b.bufpos;
    n = ((size <= n)
	 ? size
	 : n);
    
    memcpy(buffer + offset, data->b.buffer + data->b.bufpos, n);
    data->b.bufpos += n;
    
    return n;
  }
  
  while (!tcp_byte_ready(port)) {
    if (nonblock > 0)
      return 0;

#ifdef USE_SOCKETS_TCP
    scheme_block_until_unless((Scheme_Ready_Fun)tcp_byte_ready,
			      scheme_need_wakeup,
			      (Scheme_Object *)port,
			      0.0, unless,
			      nonblock);
#else
    do {
      scheme_thread_block_enable_break((float)0.0, nonblock);
      if (scheme_unless_ready(unless))
	break;
    } while (!tcp_byte_ready(port));
    scheme_current_thread->ran_some = 1;
#endif

    scheme_wait_input_allowed(port, nonblock);

    if (scheme_unless_ready(unless))
      return SCHEME_UNLESS_READY;
  }

  if (port->closed) {
    /* Another thread closed the input port while we were waiting. */
    /* Call scheme_get_byte to signal the error */
    scheme_get_byte((Scheme_Object *)port);
  }

  /* We assume that no other process has access to our sockets, so
     when we unblock, there's definitely something to read. */

  if (!data->b.bufmode || (size > TCP_BUFFER_SIZE))
    read_amt = TCP_BUFFER_SIZE;
  else
    read_amt = size;

#ifdef USE_SOCKETS_TCP
  {
    int rn;
    do {
      rn = recv(data->tcp, data->b.buffer, read_amt, 0);
    } while ((rn == -1) && (NOT_WINSOCK(errno) == EINTR));
    data->b.bufmax = rn;
  }
  errid = SOCK_ERRNO();

  /* Is it possible that an EAGAIN error occurs? That means that data
     isn't ready, even though select() says that data is ready. It
     seems to happen for at least one user, and there appears to be
     no harm in protecting against it. */
  if ((data->b.bufmax == -1) && WAS_EAGAIN(errid))
    goto top;

#endif
  
  if (data->b.bufmax == -1) {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "tcp-read: error reading (%e)",
		     errid);
    return 0;
  } else if (!data->b.bufmax) {
    data->b.hiteof = 1;
    return EOF;
  }

  {
    int n;
    n = data->b.bufmax;
    if (size < n)
      n = size;
    memcpy(buffer + offset, data->b.buffer, n);
    data->b.bufpos = n;

    return n;
  }
}

static void tcp_need_wakeup(Scheme_Input_Port *port, void *fds)
{
  Scheme_Tcp *data;

  data = (Scheme_Tcp *)port->port_data;

#ifdef USE_SOCKETS_TCP
  {
    void *fds2;
  
    fds2 = MZ_GET_FDSET(fds, 2);
    
    MZ_FD_SET(data->tcp, (fd_set *)fds);
    MZ_FD_SET(data->tcp, (fd_set *)fds2);
  }
#endif
}

static void tcp_close_input(Scheme_Input_Port *port)
{
  Scheme_Tcp *data;

  data = (Scheme_Tcp *)port->port_data;

#ifdef USE_SOCKETS_TCP
  if (!(data->flags & MZ_TCP_ABANDON_INPUT))
    shutdown(data->tcp, 0);
#endif

  if (--data->b.refcount)
    return;

#ifdef USE_SOCKETS_TCP
  UNREGISTER_SOCKET(data->tcp);
  closesocket(data->tcp);
#endif

  --scheme_file_open_count;
}

static int
tcp_in_buffer_mode(Scheme_Port *p, int mode)
{
  Scheme_Tcp *data;

  data = (Scheme_Tcp *)((Scheme_Input_Port *)p)->port_data;  
  if (mode < 0)
    return data->b.bufmode;
  else {
    data->b.bufmode = mode;
    return mode;
  }
}

static long tcp_do_write_string(Scheme_Output_Port *port, 
				const char *s, long offset, long len, 
				int rarely_block, int enable_break)
{
  /* We've already checked for buffering before we got here. */
  /* If rarely_block is 1, it means only write as much as
     can be flushed immediately, blocking only if nothing
     can be written. */
  /* If rarely_block is 2, it means only write as much as
     can be flushed immediately, never ever blocking. */

  Scheme_Tcp *data;
  int errid, would_block = 0;
  long sent;

  data = (Scheme_Tcp *)port->port_data;

 top:

#ifdef USE_SOCKETS_TCP
  do {
    sent = send(data->tcp, s XFORM_OK_PLUS offset, len, 0);
  } while ((sent == -1) && (NOT_WINSOCK(errno) == EINTR));

  if (sent != len) {
#ifdef USE_WINSOCK_TCP
# define SEND_BAD_MSG_SIZE(e) (e == WSAEMSGSIZE)
#else    
# ifdef SEND_IS_NEVER_TOO_BIG
#  define SEND_BAD_MSG_SIZE(errid) 0
# else
#  define SEND_BAD_MSG_SIZE(errid) (errid == EMSGSIZE)
# endif
#endif
    errid = SOCK_ERRNO();
    if (sent > 0) {
      /* Some data was sent. Return, or recur to handle the rest. */
      if (rarely_block)
	return sent;
      else
	sent += tcp_do_write_string(port, s, offset + sent, len - sent, 0, enable_break);
      errid = 0;
    } else if ((len > 1) && SEND_BAD_MSG_SIZE(errid)) {
      /* split the message and try again: */
      int half = (len / 2);
      sent = tcp_do_write_string(port, s, offset, half, rarely_block, enable_break);
      if (rarely_block)
	return sent;
      sent += tcp_do_write_string(port, s, offset + half, len - half, 0, enable_break);
      errid = 0;
    } else if (WAS_EAGAIN(errid)) {
      errid = 0;
      would_block = 1;
    }
  } else
    errid = 0;
#endif

  if (would_block) {
    if (rarely_block == 2)
      return 0;

    /* Block for writing: */
    scheme_block_until_enable_break(tcp_check_write, tcp_write_needs_wakeup, (Scheme_Object *)port, 
				    (float)0.0, enable_break);

    /* Closed while blocking? */
    if (((Scheme_Output_Port *)port)->closed) {
      /* Call write again to signal the error: */
      scheme_put_byte_string("tcp-write-string", (Scheme_Object *)port, s, offset, len, 0);
      return sent + len; /* shouldn't get here */
    }

    /* Ok - try again! */
    would_block = 0;
    goto top;
  }

  if (errid)
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "tcp-write: error writing (%e)",
		     errid);

  return sent;
}

static int tcp_flush(Scheme_Output_Port *port,
		     int rarely_block, int enable_break)
{
  Scheme_Tcp *data;
  int amt, flushed = 0;
  
  data = (Scheme_Tcp *)port->port_data;

  while (1) {
    if (data->b.out_bufpos == data->b.out_bufmax) {
      data->b.out_bufpos = 0;
      data->b.out_bufmax = 0;
      return flushed;
    }
    amt = tcp_do_write_string(port, data->b.out_buffer, data->b.out_bufpos, 
			      data->b.out_bufmax - data->b.out_bufpos,
			      rarely_block, enable_break);
    flushed += amt;
    data->b.out_bufpos += amt;
    if (rarely_block && (data->b.out_bufpos < data->b.out_bufmax))
      return flushed;
  }
}

static long tcp_write_string(Scheme_Output_Port *port, 
			     const char *s, long offset, long len, 
			     int rarely_block, int enable_break)
{
  Scheme_Tcp *data;

  data = (Scheme_Tcp *)port->port_data;

  if (!len) {
    /* Flush */
    return tcp_flush(port, rarely_block, enable_break);
  }

  if (rarely_block) {
    tcp_flush(port, rarely_block, enable_break);
    if (data->b.out_bufmax)
      return -1;
  } else {
    if (data->b.out_bufmode < 2) {
      if (data->b.out_bufmax + len < TCP_BUFFER_SIZE) {
	memcpy(data->b.out_buffer + data->b.out_bufmax, s + offset, len);
	data->b.out_bufmax += len;
	if (data->b.out_bufmode == 1) {
	  /* Check for newline */
	  int i;
	  for (i = 0; i < len; i++) {
	    if ((s[offset + i] == '\r')
		|| (s[offset + i] == '\n'))
	      break;
	  }
	  if (i < len)
	    tcp_flush(port, rarely_block, enable_break);
	}
	return len;
      }
    }
    tcp_flush(port, rarely_block, enable_break);
  }

  /* When we get here, the buffer is empty */
  return tcp_do_write_string(port, s, offset, len, rarely_block, enable_break);
}

static void tcp_close_output(Scheme_Output_Port *port)
{
  Scheme_Tcp *data;

  data = (Scheme_Tcp *)port->port_data;

  if (data->b.out_bufmax && !scheme_force_port_closed)
    tcp_flush(port, 0, 0);

#ifdef USE_SOCKETS_TCP
  if (!(data->flags & MZ_TCP_ABANDON_OUTPUT))
    shutdown(data->tcp, 1);
#endif

  if (--data->b.refcount)
    return;

#ifdef USE_SOCKETS_TCP
  UNREGISTER_SOCKET(data->tcp);
  closesocket(data->tcp);
#endif

  --scheme_file_open_count;
}

static int
tcp_out_buffer_mode(Scheme_Port *p, int mode)
{
  Scheme_Tcp *data;

  data = (Scheme_Tcp *)((Scheme_Output_Port *)p)->port_data;  
  if (mode < 0)
    return data->b.out_bufmode;
  else {
    int go;
    go = (mode > data->b.out_bufmode);
    data->b.out_bufmode = mode;
    if (go)
      tcp_flush((Scheme_Output_Port *)p, 0, 0);
    return mode;
  }
}

static Scheme_Object *
make_tcp_input_port(void *data, const char *name)
{
  Scheme_Input_Port *ip;
  
  ip = scheme_make_input_port(scheme_tcp_input_port_type,
			      data,
			      scheme_make_immutable_sized_utf8_string((char *)name, -1),
			      tcp_get_string,
			      NULL,
			      scheme_progress_evt_via_get,
			      scheme_peeked_read_via_get,
			      tcp_byte_ready,
			      tcp_close_input,
			      tcp_need_wakeup,
			      1);

  ip->p.buffer_mode_fun = tcp_in_buffer_mode;

  return (Scheme_Object *)ip;
}

static Scheme_Object *
make_tcp_output_port(void *data, const char *name)
{
  Scheme_Output_Port *op;

  op = scheme_make_output_port(scheme_tcp_output_port_type,
						  data,
						  scheme_make_immutable_sized_utf8_string((char *)name, -1),
						  scheme_write_evt_via_write,
						  tcp_write_string,
						  (Scheme_Out_Ready_Fun)tcp_check_write,
						  tcp_close_output,
						  (Scheme_Need_Wakeup_Output_Fun)tcp_write_needs_wakeup,
						  NULL,
						  NULL,
						  1);

  op->p.buffer_mode_fun = tcp_out_buffer_mode;

  return (Scheme_Object *)op;
}

#endif /* USE_TCP */

/*========================================================================*/
/*                         TCP Scheme interface                           */
/*========================================================================*/

#ifdef USE_SOCKETS_TCP
typedef struct Close_Socket_Data {
  tcp_t s;
  struct mz_addrinfo *src_addr, *dest_addr;
} Close_Socket_Data;

static void closesocket_w_decrement(Close_Socket_Data *csd)
{
  closesocket(csd->s);
  if (csd->src_addr)
    mz_freeaddrinfo(csd->src_addr);
  mz_freeaddrinfo(csd->dest_addr);  
  --scheme_file_open_count;
}
#endif

const char *scheme_hostname_error(int err)
{
#ifdef USE_SOCKETS_TCP
  return mz_gai_strerror(err);
#else
  return "?";
#endif
}

static Scheme_Object *tcp_connect(int argc, Scheme_Object *argv[])
{
  char * volatile address = "", * volatile src_address, * volatile errmsg = "";
  unsigned short origid, id, src_origid, src_id;
  int errpart = 0, errid = 0;
  volatile int nameerr = 0, no_local_spec;
  Scheme_Object *bs, *src_bs;
#ifdef USE_SOCKETS_TCP
  GC_CAN_IGNORE struct mz_addrinfo *tcp_connect_dest;
  GC_CAN_IGNORE struct mz_addrinfo * volatile tcp_connect_src;
#endif

  if (!SCHEME_CHAR_STRINGP(argv[0]))
    scheme_wrong_type("tcp-connect", "string", 0, argc, argv);
  if (!CHECK_PORT_ID(argv[1]))
    scheme_wrong_type("tcp-connect", PORT_ID_TYPE, 1, argc, argv);
  if (argc > 2)
    if (!SCHEME_CHAR_STRINGP(argv[2]) && !SCHEME_FALSEP(argv[2]))
      scheme_wrong_type("tcp-connect", "string or #f", 2, argc, argv);
  if (argc > 3)
    if (SCHEME_TRUEP(argv[3]) && !CHECK_PORT_ID(argv[3]))
      scheme_wrong_type("tcp-connect", PORT_ID_TYPE " or #f", 3, argc, argv);

#ifdef USE_TCP
  TCP_INIT("tcp-connect");
#endif

  bs = argv[0];
  if (SCHEME_CHAR_STRINGP(bs))
    bs = scheme_char_string_to_byte_string(bs);

  address = SCHEME_BYTE_STR_VAL(bs);
  origid = (unsigned short)SCHEME_INT_VAL(argv[1]);

  if ((argc > 2) && SCHEME_TRUEP(argv[2])) {
    src_bs = scheme_char_string_to_byte_string(argv[2]);
    src_address = SCHEME_BYTE_STR_VAL(src_bs);
  } else
    src_address = NULL;
   
  if ((argc > 3) && SCHEME_TRUEP(argv[3])) {
    no_local_spec = 0;
    src_origid = (unsigned short)SCHEME_INT_VAL(argv[3]);
  } else {
    no_local_spec = 1;
    src_origid = 0;
    if (src_address) {
      scheme_arg_mismatch("tcp-connect",
			  "no local port number supplied when local hostname was supplied: ",
			  argv[2]);
    }
  }

  scheme_security_check_network("tcp-connect", address, origid, 1);
  scheme_custodian_check_available(NULL, "tcp-connect", "network");

#ifdef USE_TCP
  id = origid;
  src_id = src_origid;
#endif

#ifdef USE_SOCKETS_TCP
  tcp_connect_dest = scheme_get_host_address(address, id, &errid, -1, 0, 1);
  if (tcp_connect_dest) {
    if (no_local_spec)
      tcp_connect_src = NULL;
    else
      tcp_connect_src = scheme_get_host_address(src_address, src_id, &errid, -1, 1, 1);
    if (no_local_spec || tcp_connect_src) {
      GC_CAN_IGNORE struct mz_addrinfo * volatile addr;
      for (addr = tcp_connect_dest; addr; addr = addr->ai_next) {
	tcp_t s;
	s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (s != INVALID_SOCKET) {
	  int status, inprogress;
	  if (no_local_spec
	      || !bind(s, tcp_connect_src->ai_addr, tcp_connect_src->ai_addrlen)) {
#ifdef USE_WINSOCK_TCP
	    unsigned long ioarg = 1;
	    ioctlsocket(s, FIONBIO, &ioarg);
#else
	    int size = TCP_SOCKSENDBUF_SIZE;
	    fcntl(s, F_SETFL, MZ_NONBLOCKING);
# ifndef CANT_SET_SOCKET_BUFSIZE
	    setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(int));
# endif
#endif
	    status = connect(s, addr->ai_addr, addr->ai_addrlen);
#ifdef USE_UNIX_SOCKETS_TCP
	    if (status)
	      status = errno;
	    if (status == EINTR)
	      status = EINPROGRESS;
	
	    inprogress = (status == EINPROGRESS);
#endif
#ifdef USE_WINSOCK_TCP
	    if (status)
	      status = WSAGetLastError();

	    inprogress = (status == WSAEWOULDBLOCK);
	    errno = status;
#endif

	    scheme_file_open_count++;
	  
	    if (inprogress) {
	      tcp_t *sptr;
	      Close_Socket_Data *csd;

	      sptr = (tcp_t *)scheme_malloc_atomic(sizeof(tcp_t));
	      *sptr = s;

	      csd = (Close_Socket_Data *)scheme_malloc_atomic(sizeof(Close_Socket_Data));
	      csd->s = s;
	      csd->src_addr = tcp_connect_src;
	      csd->dest_addr = tcp_connect_dest;

	      BEGIN_ESCAPEABLE(closesocket_w_decrement, csd);
	      scheme_block_until(tcp_check_connect, tcp_connect_needs_wakeup, (void *)sptr, (float)0.0);
	      END_ESCAPEABLE();

	      /* Check whether connect succeeded, or get error: */
	      {
		int so_len = sizeof(status);
		if (getsockopt(s, SOL_SOCKET, SO_ERROR, (void *)&status, &so_len) != 0) {
		  status = SOCK_ERRNO();
		}
		errno = status; /* for error reporting, below */
	      }

#ifdef USE_WINSOCK_TCP
	      if (scheme_stupid_windows_machine > 0) {
		/* getsockopt() seems not to work in Windows 95, so use the
		   result from select(), which seems to reliably detect an error condition */
		if (!status) {
		  if (tcp_check_connect((Scheme_Object *)sptr) == -1) {
		    status = 1;
		    errno = WSAECONNREFUSED; /* guess! */
		  }
		}
	      }
#endif
	    }
	
	    if (!status) {
	      Scheme_Object *v[2];
	      Scheme_Tcp *tcp;

	      if (tcp_connect_src)
		mz_freeaddrinfo(tcp_connect_src);
	      mz_freeaddrinfo(tcp_connect_dest);

	      tcp = make_tcp_port_data(s, 2);
	      
	      v[0] = make_tcp_input_port(tcp, address);
	      v[1] = make_tcp_output_port(tcp, address);
	      
	      REGISTER_SOCKET(s);

	      return scheme_values(2, v);
	    } else {
	      errid = errno;
	      closesocket(s);
	      --scheme_file_open_count;
	      errpart = 6;
	    }
	  } else {
	    errpart = 5;
	    errid = SOCK_ERRNO();
	  }
	} else {
	  errpart = 4;
	  errid = SOCK_ERRNO();
	}
      }
      if (tcp_connect_src)
	mz_freeaddrinfo(tcp_connect_src);
    } else {
      errpart = 2;
      nameerr = 1;
      errmsg = "; local host not found";
    } 
    if (tcp_connect_dest)
      mz_freeaddrinfo(tcp_connect_dest);
  } else {
    errpart = 1;
    nameerr = 1;
    errmsg = "; host not found";
  }
#endif

#ifdef USE_TCP
  scheme_raise_exn(MZEXN_FAIL_NETWORK,
		   "tcp-connect: connection to %s, port %d failed%s (at step %d: %N)",
		   address, origid, errmsg, errpart, nameerr, errid);
#else
  scheme_raise_exn(MZEXN_FAIL_UNSUPPORTED,
		   "tcp-connect: not supported on this platform");
#endif

  return NULL;
}

static Scheme_Object *
tcp_connect_break(int argc, Scheme_Object *argv[])
{
  return scheme_call_enable_break(tcp_connect, argc, argv);
}

static Scheme_Object *
tcp_listen(int argc, Scheme_Object *argv[])
{
  unsigned short id, origid;
  int backlog, errid;
  int reuse = 0;
#ifdef MZ_TCP_LISTEN_IPV6_ONLY_SOCKOPT
  int no_ipv6 = 0;
#endif
  const char *address;
  
  if (!CHECK_PORT_ID(argv[0]))
    scheme_wrong_type("tcp-listen", PORT_ID_TYPE, 0, argc, argv);
  if (argc > 1) {
    if (!SCHEME_INTP(argv[1]) || (SCHEME_INT_VAL(argv[1]) < 1))
      scheme_wrong_type("tcp-listen", "small positive integer", 1, argc, argv);
  }
  if (argc > 2)
    reuse = SCHEME_TRUEP(argv[2]);
  if (argc > 3) {
    if (!SCHEME_CHAR_STRINGP(argv[3]) && !SCHEME_FALSEP(argv[3]))
      scheme_wrong_type("tcp-listen", "string or #f", 3, argc, argv);
  }
    
#ifdef USE_TCP
  TCP_INIT("tcp-listen");
#endif

  origid = (unsigned short)SCHEME_INT_VAL(argv[0]);
  if (argc > 1)
    backlog = SCHEME_INT_VAL(argv[1]);
  else
    backlog = 4;
  if ((argc > 3) && SCHEME_TRUEP(argv[3])) {
    Scheme_Object *bs;
    bs = scheme_char_string_to_byte_string(argv[3]);
    address = SCHEME_BYTE_STR_VAL(bs);
  } else
    address = NULL;

  scheme_security_check_network("tcp-listen", address, origid, 0);
  scheme_custodian_check_available(NULL, "tcp-listen", "network");

#ifdef USE_TCP
  id = origid;
#endif

#ifdef MZ_TCP_LISTEN_IPV6_ONLY_SOCKOPT
 retry:
#endif

  {
    GC_CAN_IGNORE struct mz_addrinfo *tcp_listen_addr, *addr;
    int err, count = 0, pos = 0, i;
    listener_t *l = NULL;
#ifdef MZ_TCP_LISTEN_IPV6_ONLY_SOCKOPT
    int any_v4 = 0, any_v6 = 0;
#endif

    tcp_listen_addr = scheme_get_host_address(address, id, &err, 
#ifdef MZ_TCP_LISTEN_IPV6_ONLY_SOCKOPT
					      no_ipv6 ? MZ_PF_INET : -1,
#else
					      -1, 
#endif
					      1, 1);

    for (addr = tcp_listen_addr; addr; addr = addr->ai_next) {
#ifdef MZ_TCP_LISTEN_IPV6_ONLY_SOCKOPT
      if (addr->ai_family == MZ_PF_INET)
	any_v4 = 1;
      else if (addr->ai_family == PF_INET6)
	any_v6 = 1;
#endif
      count++;
    }
		
    if (tcp_listen_addr) {
      tcp_t s;
#ifdef MZ_TCP_LISTEN_IPV6_ONLY_SOCKOPT
      /* Try IPv6 listeners first, so we can retry and use just IPv4 if
	 IPv6 doesn't work right. */
      int v6_loop = (any_v6 && any_v4), skip_v6 = 0;
#endif

      errid = 0;
      for (addr = tcp_listen_addr; addr; ) {
#ifdef MZ_TCP_LISTEN_IPV6_ONLY_SOCKOPT
	if ((v6_loop && (addr->ai_family != PF_INET6))
	    || (skip_v6 && (addr->ai_family == PF_INET6))) {
	  addr = addr->ai_next;
	  if (v6_loop && !addr) {
	    v6_loop = 0;
	    skip_v6 = 1;
	    addr = tcp_listen_addr;
	  }
	  continue;
	}
#endif

	s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);

#ifdef MZ_TCP_LISTEN_IPV6_ONLY_SOCKOPT
	if (s == INVALID_SOCKET) {
	  /* Maybe it failed because IPv6 is not available: */
	  if ((addr->ai_family == PF_INET6) && (errno == EAFNOSUPPORT)) {
	    if (any_v4 && !pos) {
	      /* Maybe we can make it work with just IPv4. Try again. */
	      no_ipv6 = 1;
	      mz_freeaddrinfo(tcp_listen_addr);
	      goto retry;
	    }
	  }
	}
	if (s != INVALID_SOCKET) {
	  if (any_v4 && (addr->ai_family == PF_INET6)) {
	    int ok;
# ifdef IPV6_V6ONLY
	    int on = 1;
	    ok = setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on));
# else
	    ok = -1;
# endif
	    if (ok) {
	      if (!pos) {
		/* IPV6_V6ONLY doesn't work */
		no_ipv6 = 1;
		mz_freeaddrinfo(tcp_listen_addr);
		goto retry;
	      } else {
		errid = errno;
		closesocket(s);
		errno = errid;
		s = INVALID_SOCKET;
	      }
	    }
	  }
	}
#endif

	if (s != INVALID_SOCKET) {
#ifdef USE_WINSOCK_TCP
	  unsigned long ioarg = 1;
	  ioctlsocket(s, FIONBIO, &ioarg);
#else
	  fcntl(s, F_SETFL, MZ_NONBLOCKING);
#endif

	  if (reuse) {
	    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)(&reuse), sizeof(int));
	  }
      
	  if (!bind(s, addr->ai_addr, addr->ai_addrlen)) {
	    if (!listen(s, backlog)) {
	      if (!pos) {
		l = scheme_malloc_tagged(sizeof(listener_t) + ((count - 1) * sizeof(tcp_t)));
		l->so.type = scheme_listener_type;
		l->count = count;
		{
		  Scheme_Custodian_Reference *mref;
		  mref = scheme_add_managed(NULL,
					    (Scheme_Object *)l,
					    (Scheme_Close_Custodian_Client *)stop_listener,
					    NULL,
					    1);
		  l->mref = mref;
		}
	      }
	      l->s[pos++] = s;
	    
	      scheme_file_open_count++;
	      REGISTER_SOCKET(s);

	      if (pos == count) {
		mz_freeaddrinfo(tcp_listen_addr);

		return (Scheme_Object *)l;
	      }
	    } else {
	      errid = SOCK_ERRNO();
	      closesocket(s);
	      break;
	    }
	  } else {
	    errid = SOCK_ERRNO();
	    closesocket(s);
	    break;
	  }
	} else {
	  errid = SOCK_ERRNO();
	  break;
	}

	addr = addr->ai_next;

#ifdef MZ_TCP_LISTEN_IPV6_ONLY_SOCKOPT
	if (!addr && v6_loop) {
	  v6_loop = 0;
	  skip_v6 = 1;
	  addr = tcp_listen_addr;
	}
#endif
      }

      for (i = 0; i < pos; i++) {
	s = l->s[i];
	UNREGISTER_SOCKET(s);
	closesocket(s);
	--scheme_file_open_count;
      }
      
      mz_freeaddrinfo(tcp_listen_addr);
    } else {
      scheme_raise_exn(MZEXN_FAIL_NETWORK,
		       "tcp-listen: host not found: %s (%N)",
		       address, 1, err);
      return NULL;
    }
  }

#ifdef USE_TCP
  scheme_raise_exn(MZEXN_FAIL_NETWORK,
		   "tcp-listen: listen on %d failed (%E)",
		   origid, errid);
#else
  scheme_raise_exn(MZEXN_FAIL_UNSUPPORTED,
		   "tcp-listen: not supported on this platform");
#endif

  return NULL;
}

#ifdef USE_TCP
static int stop_listener(Scheme_Object *o)
{
  int was_closed = 0;

#ifdef USE_SOCKETS_TCP
  {
    listener_t *listener = (listener_t *)o;
    int i;
    tcp_t s;
    s = listener->s[0];
    if (s == INVALID_SOCKET)
      was_closed = 1;
    else {
      for (i = 0; i < listener->count; i++) {
	s = listener->s[i];
	UNREGISTER_SOCKET(s);
	closesocket(s);
	listener->s[i] = INVALID_SOCKET;
	--scheme_file_open_count;
      }
      scheme_remove_managed(((listener_t *)o)->mref, o);
    }
  }
#endif

  return was_closed;
}
#endif

static Scheme_Object *
tcp_stop(int argc, Scheme_Object *argv[])
{
#ifdef USE_TCP
  int was_closed;

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_listener_type))
    scheme_wrong_type("tcp-close", "tcp-listener", 0, argc, argv);

  TCP_INIT("tcp-close");

  was_closed = stop_listener(argv[0]);

  if (was_closed) {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "tcp-close: listener was already closed");
    return NULL;
  }

  return scheme_void;
#else
  scheme_wrong_type("tcp-close", "tcp-listener", 0, argc, argv);
  return NULL;
#endif
}

static Scheme_Object *
tcp_accept_ready(int argc, Scheme_Object *argv[])
{
#ifdef USE_TCP
  int ready;

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_listener_type))
    scheme_wrong_type("tcp-accept-ready?", "tcp-listener", 0, argc, argv);

  TCP_INIT("tcp-accept-ready?");

  if (LISTENER_WAS_CLOSED(argv[0])) {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "tcp-accept-ready?: listener is closed");
    return NULL;
  }

  ready = tcp_check_accept(argv[0]);

  return (ready ? scheme_true : scheme_false);
#else
  scheme_wrong_type("tcp-accept-ready?", "tcp-listener", 0, argc, argv);
  return NULL;
#endif
}

static Scheme_Object *
tcp_accept(int argc, Scheme_Object *argv[])
{
#ifdef USE_TCP
  int was_closed = 0, errid, ready_pos;
  Scheme_Object *listener;
# ifdef USE_SOCKETS_TCP
  tcp_t s;
  int l;
  GC_CAN_IGNORE char tcp_accept_addr[MZ_SOCK_NAME_MAX_LEN];
# endif

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_listener_type))
    scheme_wrong_type("tcp-accept", "tcp-listener", 0, argc, argv);

  TCP_INIT("tcp-accept");

  listener = argv[0];

  was_closed = LISTENER_WAS_CLOSED(listener);

  if (!was_closed) {
    ready_pos = tcp_check_accept(listener);
    if (!ready_pos) {
      scheme_block_until(tcp_check_accept, tcp_accept_needs_wakeup, listener, 0.0);
      ready_pos = tcp_check_accept(listener);
    }
    was_closed = LISTENER_WAS_CLOSED(listener);
  } else
    ready_pos = 0;

  if (was_closed) {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "tcp-accept: listener is closed");
    return NULL;
  }

  scheme_custodian_check_available(NULL, "tcp-accept", "network");
  
# ifdef USE_SOCKETS_TCP
  s = ((listener_t *)listener)->s[ready_pos-1];

  l = sizeof(tcp_accept_addr);

  do {
    s = accept(s, (struct sockaddr *)tcp_accept_addr, &l);
  } while ((s == -1) && (NOT_WINSOCK(errno) == EINTR));

  if (s != INVALID_SOCKET) {
    Scheme_Object *v[2];
    Scheme_Tcp *tcp;
    
#  ifdef USE_UNIX_SOCKETS_TCP
    int size = TCP_SOCKSENDBUF_SIZE;
#   ifndef CANT_SET_SOCKET_BUFSIZE
    setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(int));
#   endif
#  endif

    tcp = make_tcp_port_data(s, 2);

    v[0] = make_tcp_input_port(tcp, "[accepted]");
    v[1] = make_tcp_output_port(tcp, "[accepted]");

    scheme_file_open_count++;
    REGISTER_SOCKET(s);
    
    return scheme_values(2, v);
  }
  errid = SOCK_ERRNO();
# endif
  
  scheme_raise_exn(MZEXN_FAIL_NETWORK,
		   "tcp-accept: accept from listener failed (%E)", errid);
#else
  scheme_wrong_type("tcp-accept", "tcp-listener", 0, argc, argv);
#endif

  return NULL;
}

static Scheme_Object *
tcp_accept_break(int argc, Scheme_Object *argv[])
{
  return scheme_call_enable_break(tcp_accept, argc, argv);
}

static void register_tcp_listener_sync()
{
#ifdef USE_TCP
  scheme_add_evt(scheme_listener_type, tcp_check_accept, tcp_accept_needs_wakeup, NULL, 0);
  scheme_add_evt(scheme_tcp_accept_evt_type, (Scheme_Ready_Fun)tcp_check_accept_evt, tcp_accept_evt_needs_wakeup, NULL, 0);
# ifdef UDP_IS_SUPPORTED
  scheme_add_evt(scheme_udp_evt_type, (Scheme_Ready_Fun)udp_evt_check_ready, udp_evt_needs_wakeup, NULL, 0);
# endif
#endif
}

static Scheme_Object *tcp_listener_p(int argc, Scheme_Object *argv[])
{
   return (SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_listener_type)
	   ? scheme_true
	   : scheme_false);
}

void scheme_getnameinfo(void *sa, int salen, 
			char *host, int hostlen,
			char *serv, int servlen)
{
#ifdef HAVE_GETADDRINFO
  getnameinfo(sa, salen, host, hostlen, serv, servlen,
	      NI_NUMERICHOST | NI_NUMERICSERV);
#else
  if (host) {
    unsigned char *b;
    b = (unsigned char *)&((struct sockaddr_in *)sa)->sin_addr;
    sprintf(host, "%d.%d.%d.%d", b[0], b[1], b[2], b[3]);
  }
  if (serv) {
    int id;
    id = ntohs(((struct sockaddr_in *)sa)->sin_port);
    sprintf(serv, "%d", id);
  }
#endif
}

static Scheme_Object *tcp_addresses(int argc, Scheme_Object *argv[])
{
#ifdef USE_TCP
  Scheme_Tcp *tcp = NULL;
  int closed = 0;
  Scheme_Object *result[2];

  if (SCHEME_OUTPORTP(argv[0])) {
    Scheme_Output_Port *op;
    op = (Scheme_Output_Port *)argv[0];
    if (op->sub_type == scheme_tcp_output_port_type)
      tcp = op->port_data;
    closed = op->closed;
  } else if (SCHEME_INPORTP(argv[0])) {
    Scheme_Input_Port *ip;
    ip = (Scheme_Input_Port *)argv[0];
    if (ip->sub_type == scheme_tcp_input_port_type)
      tcp = ip->port_data;
    closed = ip->closed;
  }

  if (!tcp)
    scheme_wrong_type("tcp-addresses", "tcp-port", 0, argc, argv);

  if (closed)
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "tcp-addresses: port is closed");

# ifdef USE_SOCKETS_TCP
  {
    int l;
    char here[MZ_SOCK_NAME_MAX_LEN], there[MZ_SOCK_NAME_MAX_LEN];
    char host_buf[MZ_SOCK_HOST_NAME_MAX_LEN];
    int here_len, there_len;

    l = sizeof(here);
    if (getsockname(tcp->tcp, (struct sockaddr *)here, &l)) {
      scheme_raise_exn(MZEXN_FAIL_NETWORK,
		       "tcp-addresses: could not get local address (%e)",
		       SOCK_ERRNO());
    }
    here_len = l;

    l = sizeof(there);
    if (getpeername(tcp->tcp, (struct sockaddr *)there, &l)) {
      scheme_raise_exn(MZEXN_FAIL_NETWORK,
		       "tcp-addresses: could not get peer address (%e)",
		       SOCK_ERRNO());
    }
    there_len = l;

    scheme_getnameinfo((struct sockaddr *)here, here_len, 
		       host_buf, sizeof(host_buf),
		       NULL, 0);
    result[0] = scheme_make_utf8_string(host_buf);

    scheme_getnameinfo((struct sockaddr *)there, there_len, 
		       host_buf, sizeof(host_buf),
		       NULL, 0);
    result[1] = scheme_make_utf8_string(host_buf);
  }
# else
  result[0] = scheme_make_utf8_string("0.0.0.0");
  result[1] = result[1];
# endif

  return scheme_values(2, result);
#else
  /* First arg can't possible be right! */
  scheme_wrong_type("tcp-addresses", "tcp-port", 0, argc, argv);
#endif
}

static Scheme_Object *tcp_abandon_port(int argc, Scheme_Object *argv[])
{
#ifdef USE_TCP
  if (SCHEME_OUTPORTP(argv[0])) {
    Scheme_Output_Port *op;
    op = (Scheme_Output_Port *)argv[0];
    if (op->sub_type == scheme_tcp_output_port_type) {
      if (!op->closed) {
	((Scheme_Tcp *)op->port_data)->flags |= MZ_TCP_ABANDON_OUTPUT;
	scheme_close_output_port(argv[0]);
      }
      return scheme_void;
    }
  } else if (SCHEME_INPORTP(argv[0])) {
    /* Abandon is not really useful on input ports from the Schemer's
       perspective, but it's here for completeness. */
    Scheme_Input_Port *ip;
    ip = (Scheme_Input_Port *)argv[0];
    if (ip->sub_type == scheme_tcp_input_port_type) {
      if (!ip->closed) {
	((Scheme_Tcp *)ip->port_data)->flags |= MZ_TCP_ABANDON_INPUT;
	scheme_close_input_port(argv[0]);
      }
      return scheme_void;
    }
  }
#endif

  scheme_wrong_type("tcp-abandon-port", "tcp-port", 0, argc, argv);

  return NULL;
}

static Scheme_Object *tcp_port_p(int argc, Scheme_Object *argv[])
{
#ifdef USE_TCP
  if (SCHEME_OUTPORTP(argv[0])) {
    if (((Scheme_Output_Port *)argv[0])->sub_type == scheme_tcp_output_port_type) {
      return scheme_true;
    }
  } else if (SCHEME_INPORTP(argv[0])) {
    if (((Scheme_Input_Port *)argv[0])->sub_type == scheme_tcp_input_port_type) {
      return scheme_true;
    }
  }
#endif

  return scheme_false;
}


static Scheme_Object *tcp_accept_evt(int argc, Scheme_Object *argv[])
{
  Scheme_Object *r;

  if (!SAME_TYPE(SCHEME_TYPE(argv[0]), scheme_listener_type))
    scheme_wrong_type("tcp-accept-evt", "tcp-listener", 0, argc, argv);

  r = scheme_alloc_small_object();
  r->type = scheme_tcp_accept_evt_type;
  SCHEME_PTR_VAL(r) = argv[0];

  return r;
}

static int tcp_check_accept_evt(Scheme_Object *ae, Scheme_Schedule_Info *sinfo)
{
  if (tcp_check_accept(SCHEME_PTR_VAL(ae))) {
    Scheme_Object *a[2];
    a[0] = SCHEME_PTR_VAL(ae);
    tcp_accept(1, a);
    a[0] = scheme_current_thread->ku.multiple.array[0];
    a[1] = scheme_current_thread->ku.multiple.array[1];
    scheme_set_sync_target(sinfo, scheme_build_list(2, a), NULL, NULL, 0, 0);
    return 1;
  } else
    return 0;
}

static void tcp_accept_evt_needs_wakeup(Scheme_Object *ae, void *fds)
{
  tcp_accept_needs_wakeup(SCHEME_PTR_VAL(ae), fds);
}

int scheme_get_port_socket(Scheme_Object *p, long *_s)
{
#ifdef USE_TCP
  tcp_t s = 0;
  int s_ok = 0;

  if (SCHEME_OUTPORTP(p)) {
    Scheme_Output_Port *op;
    op = (Scheme_Output_Port *)p;
    if (op->sub_type == scheme_tcp_output_port_type) {
      if (!op->closed) {
	s = ((Scheme_Tcp *)op->port_data)->tcp;
	s_ok = 1;
      }
    }
  } else if (SCHEME_INPORTP(p)) {
    /* Abandon is not really useful on input ports from the Schemer's
       perspective, but it's here for completeness. */
    Scheme_Input_Port *ip;
    ip = (Scheme_Input_Port *)p;
    if (ip->sub_type == scheme_tcp_input_port_type) {
      if (!ip->closed) {
	s = ((Scheme_Tcp *)ip->port_data)->tcp;
	s_ok = 1;
      }
    }
  }

  if (s_ok) {
    *_s = (long)s;
    return 1;
  } else
    return 0;
#endif
}

/*========================================================================*/
/*                                 UDP                                    */
/*========================================================================*/

/* Based on a design and implemenation by Eduardo Cavazos. */

#ifdef UDP_IS_SUPPORTED

typedef struct Scheme_UDP_Evt {
  Scheme_Object so; /* scheme_udp_evt_type */
  Scheme_UDP *udp;
  short for_read, with_addr;
  int offset, len;
  char *str;
  char *dest_addr;
  int dest_addr_len;
} Scheme_UDP_Evt;

static int udp_close_it(Scheme_Object *_udp)
{
  Scheme_UDP *udp = (Scheme_UDP *)_udp;

  if (udp->s != INVALID_SOCKET) {
    closesocket(udp->s);
    udp->s = INVALID_SOCKET;

    scheme_remove_managed(udp->mref, (Scheme_Object *)udp);

    return 0;
  }

  return 1;
}

#endif

static Scheme_Object *make_udp(int argc, Scheme_Object *argv[])
{
#ifdef UDP_IS_SUPPORTED
  Scheme_UDP *udp;
  tcp_t s;
  char *address = "";
  unsigned short origid, id;

  TCP_INIT("udp-open-socket");

  if ((argc > 0) && !SCHEME_FALSEP(argv[0]) && !SCHEME_CHAR_STRINGP(argv[0]))
    scheme_wrong_type("udp-open-socket", "string or #f", 0, argc, argv);
  if ((argc > 1) && !SCHEME_FALSEP(argv[1]) && !CHECK_PORT_ID(argv[1]))
    scheme_wrong_type("udp-open-socket", PORT_ID_TYPE " or #f", 1, argc, argv);

  if ((argc > 0) && SCHEME_TRUEP(argv[0])) {
    Scheme_Object *bs;
    bs = scheme_char_string_to_byte_string(argv[0]);
    address = SCHEME_BYTE_STR_VAL(bs);
  } else
    address = NULL;
  if ((argc > 1) && SCHEME_TRUEP(argv[1]))
    origid = (unsigned short)SCHEME_INT_VAL(argv[1]);
  else
    origid = 0;

  scheme_security_check_network("udp-open-socket", address, origid, 0);
  scheme_custodian_check_available(NULL, "udp-open-socket", "network");

  if (address || origid) {
    int err;
    GC_CAN_IGNORE struct mz_addrinfo *udp_bind_addr = NULL;
    if (!origid)
      origid = 1025;
    id = origid;
    udp_bind_addr = scheme_get_host_address(address, id, &err, -1, 1, 0);
    if (!udp_bind_addr) {
      scheme_raise_exn(MZEXN_FAIL_NETWORK,
		       "udp-open-socket: can't resolve address: %s (%N)", 
		       address ? address : "<unspec>", 1, err);
      return NULL;
    }
    s = socket(udp_bind_addr->ai_family,
	       udp_bind_addr->ai_socktype,
	       udp_bind_addr->ai_protocol);
    mz_freeaddrinfo(udp_bind_addr);
  } else {
    s = socket(MZ_PF_INET, SOCK_DGRAM, 0);
  }

  if (s == INVALID_SOCKET) {
    int errid;
    errid = SOCK_ERRNO();
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "udp-open-socket: creation failed (%E)", errid);
    return NULL;
  }

  udp = MALLOC_ONE_TAGGED(Scheme_UDP);
  udp->so.type = scheme_udp_type;
  udp->s = s;
  udp->bound = 0;
  udp->connected = 0;
  udp->previous_from_addr = NULL;

#ifdef USE_WINSOCK_TCP
  {
    unsigned long ioarg = 1;
    BOOL bc = 1;
    ioctlsocket(s, FIONBIO, &ioarg);
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *)(&bc), sizeof(BOOL));
  }
#else
  fcntl(s, F_SETFL, MZ_NONBLOCKING);
# ifdef SO_BROADCAST
  {
    int bc = 1;
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &bc, sizeof(bc));
  }
# endif
#endif

  {
    Scheme_Custodian_Reference *mref;
    mref = scheme_add_managed(NULL,
			      (Scheme_Object *)udp,
			      (Scheme_Close_Custodian_Client *)udp_close_it,
			      NULL,
			      1);
    udp->mref = mref;
  }

  return (Scheme_Object *)udp;
#else
  scheme_raise_exn(MZEXN_FAIL_UNSUPPORTED,
		   "udp-open-socket: not supported on this platform");
  return NULL;
#endif
}

static Scheme_Object *
udp_close(int argc, Scheme_Object *argv[])
{
  if (!SCHEME_UDPP(argv[0]))
    scheme_wrong_type("udp-close", "udp socket", 0, argc, argv);

#ifdef UDP_IS_SUPPORTED
  if (udp_close_it(argv[0])) {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "udp-close: udp socket was already closed");
    return NULL;
  }
#endif

  return scheme_void;
}

static Scheme_Object *
udp_p(int argc, Scheme_Object *argv[])
{
  return (SCHEME_UDPP(argv[0]) ? scheme_true : scheme_false);
}

static Scheme_Object *
udp_bound_p(int argc, Scheme_Object *argv[])
{
  if (!SCHEME_UDPP(argv[0]))
    scheme_wrong_type("udp-bound?", "udp socket", 0, argc, argv);

#ifdef UDP_IS_SUPPORTED
  return (((Scheme_UDP *)argv[0])->bound ? scheme_true : scheme_false);
#else
  return scheme_void;
#endif
}

static Scheme_Object *
udp_connected_p(int argc, Scheme_Object *argv[])
{
  if (!SCHEME_UDPP(argv[0]))
    scheme_wrong_type("udp-connected?", "udp socket", 0, argc, argv);

#ifdef UDP_IS_SUPPORTED
  return (((Scheme_UDP *)argv[0])->connected ? scheme_true : scheme_false);
#else
  return scheme_void;
#endif
}

static Scheme_Object *udp_bind_or_connect(const char *name, int argc, Scheme_Object *argv[], int do_bind)
{
#ifdef UDP_IS_SUPPORTED
  Scheme_UDP *udp;
  char *address = "";
  unsigned short origid, id;
  GC_CAN_IGNORE struct mz_addrinfo *udp_bind_addr;
  int errid, err;

  udp = (Scheme_UDP *)argv[0];
#endif

  if (!SCHEME_UDPP(argv[0]))
    scheme_wrong_type(name, "udp socket", 0, argc, argv);

#ifdef UDP_IS_SUPPORTED
  if (!SCHEME_FALSEP(argv[1]) && !SCHEME_CHAR_STRINGP(argv[1]))
    scheme_wrong_type(name, "string or #f", 1, argc, argv);
  if ((do_bind || !SCHEME_FALSEP(argv[2])) && !CHECK_PORT_ID(argv[2]))
    scheme_wrong_type(name, (do_bind ? PORT_ID_TYPE : PORT_ID_TYPE " or #f"), 2, argc, argv);
		      
  if (SCHEME_TRUEP(argv[1])) {
    Scheme_Object *bs;
    bs = scheme_char_string_to_byte_string(argv[1]);
    address = SCHEME_BYTE_STR_VAL(bs);
  } else
    address = NULL;
  if (SCHEME_TRUEP(argv[2]))
    origid = (unsigned short)SCHEME_INT_VAL(argv[2]);
  else
    origid = 0;

  if (!do_bind && (SCHEME_TRUEP(argv[1]) != SCHEME_TRUEP(argv[2]))) {
    scheme_raise_exn(MZEXN_FAIL_CONTRACT,
		     "%s: last two arguments must be both #f or both non-#f, given: %V %V",
		     name, argv[1], argv[2]);
  }

  scheme_security_check_network(name, address, origid, !do_bind);

  if (udp->s == INVALID_SOCKET) {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "%s: udp socket was already closed: %V",
		     name,
		     udp);
    return NULL;
  }


  if (do_bind && udp->bound) {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "%s: udp socket is already bound: %V",
		     name,
		     udp);
    return NULL;
  }

  id = origid;

  if (address || id)
    udp_bind_addr = scheme_get_host_address(address, id, &err, -1, do_bind, 0);
  else
    udp_bind_addr = NULL;

  if (udp_bind_addr || !origid) {
    if (do_bind) {
      if (!bind(udp->s, udp_bind_addr->ai_addr, udp_bind_addr->ai_addrlen)) {
	udp->bound = 1;
	mz_freeaddrinfo(udp_bind_addr);
	return scheme_void;
      }
      errid = SOCK_ERRNO();
    } else {
      int ok;

#ifdef USE_NULL_TO_DISCONNECT_UDP
      if (!origid) {
	if (udp->connected)
	  ok = !connect(udp->s, NULL, 0);
	else
	  ok = 1;
      } else
#endif
	{
	  if (udp_bind_addr)
	    ok = !connect(udp->s, udp_bind_addr->ai_addr, udp_bind_addr->ai_addrlen);
#ifndef USE_NULL_TO_DISCONNECT_UDP
	  else {
	    GC_CAN_IGNORE mz_unspec_address ua;
	    ua.sin_family = AF_UNSPEC;
	    ua.sin_port = 0;
	    memset(&(ua.sin_addr), 0, sizeof(ua.sin_addr));
	    memset(&(ua.sin_zero), 0, sizeof(ua.sin_zero));
	    ok = !connect(udp->s, (struct sockaddr *)&ua, sizeof(ua));
	  }
#endif
	}
      
      if (!ok)
	errid = SOCK_ERRNO();
      else
	errid = 0;

      if (!ok && (errid == mz_AFNOSUPPORT) && !origid) {
	/* It's ok. We were trying to disconnect */
	ok = 1;
      }

      if (ok) {
	if (origid)
	  udp->connected = 1;
	else
	  udp->connected = 0;
	if (udp_bind_addr)
	  mz_freeaddrinfo(udp_bind_addr);
	return scheme_void;
      }
    }

    if (udp_bind_addr)
      mz_freeaddrinfo(udp_bind_addr);

    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "%s: can't %s to port: %d on address: %s (%E)", 
		     name,
		     do_bind ? "bind" : "connect",
		     origid,
		     address ? address : "#f",
		     errid);
    return NULL;
  } else {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "%s: can't resolve address: %s (%N)", 
		     name,
		     address, 1, err);
    return NULL;
  }
#else
  return scheme_void;
#endif
}

static Scheme_Object *udp_bind(int argc, Scheme_Object *argv[])
{
  return udp_bind_or_connect("udp-bind!", argc, argv, 1);
}

static Scheme_Object *udp_connect(int argc, Scheme_Object *argv[])
{
  return udp_bind_or_connect("udp-connect!", argc, argv, 0);
}

#ifdef UDP_IS_SUPPORTED

static int udp_check_send(Scheme_Object *_udp)
{
  Scheme_UDP *udp = (Scheme_UDP *)_udp;

  if (udp->s == INVALID_SOCKET)
    return 1;

  {
    DECL_FDSET(writefds, 1);
    DECL_FDSET(exnfds, 1);
    struct timeval time = {0, 0};
    int sr;
    
    INIT_DECL_FDSET(writefds, 1);
    INIT_DECL_FDSET(exnfds, 1);
    
    MZ_FD_ZERO(writefds);
    MZ_FD_SET(udp->s, writefds);
    MZ_FD_ZERO(exnfds);
    MZ_FD_SET(udp->s, exnfds);
    
    do {
      sr = select(udp->s + 1, NULL, writefds, exnfds, &time);
    } while ((sr == -1) && (NOT_WINSOCK(errno) == EINTR));
    
    return sr;
  }
}

static void udp_send_needs_wakeup(Scheme_Object *_udp, void *fds)
{
  Scheme_UDP *udp = (Scheme_UDP *)_udp;
  void *fds1, *fds2;
  tcp_t s = udp->s;
  
  fds1 = MZ_GET_FDSET(fds, 1);
  fds2 = MZ_GET_FDSET(fds, 2);
  
  MZ_FD_SET(s, (fd_set *)fds1);
  MZ_FD_SET(s, (fd_set *)fds2);
}

#endif

static Scheme_Object *do_udp_send_it(const char *name, Scheme_UDP *udp,
				     char *bstr, long start, long end,
				     char *dest_addr, int dest_addr_len, int can_block)
{
  long x;
  int errid = 0;

  while (1) {
    if (udp->s == INVALID_SOCKET) {
      /* socket was closed, maybe while we slept */
      scheme_raise_exn(MZEXN_FAIL_NETWORK,
		       "%s: udp socket is closed: %V",
		       name, udp);
      return NULL;
    }
    if ((!dest_addr && !udp->connected)
	|| (dest_addr && udp->connected)) {
      /* socket is unconnected, maybe disconnected while we slept */
      scheme_raise_exn(MZEXN_FAIL_NETWORK,
		       "%s: udp socket is%s connected: %V",
		       name, 
		       dest_addr ? "" : " not",
		       udp);
      return NULL;
    }

    udp->bound = 1; /* in case it's not bound already, send[to] binds it */

    if (dest_addr)
      x = sendto(udp->s, bstr XFORM_OK_PLUS start, end - start, 
		 0, (struct sockaddr *)dest_addr, dest_addr_len);
    else
      x = send(udp->s, bstr XFORM_OK_PLUS start, end - start, 0);

    if (x == -1) {
      errid = SOCK_ERRNO();
      if (WAS_EAGAIN(errid)) {
	if (can_block) {
	  /* Block and eventually try again. */
	  scheme_block_until(udp_check_send, udp_send_needs_wakeup, (Scheme_Object *)udp, 0);
	} else
	  return scheme_false;
      } else if (NOT_WINSOCK(errid) != EINTR)
	break;
    } else if (x != (end - start)) {
      /* this isn't supposed to happen: */
      scheme_raise_exn(MZEXN_FAIL_NETWORK,
		       "%s: didn't send enough (%d != %d)", 
		       name,
		       x, end - start);
      return NULL;
    } else
      break;
  }
    
  if (x > -1) {
    return (can_block ? scheme_void : scheme_true);
  } else {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "%s: send failed (%E)", 
		     name,
		     errid);
    return NULL;
  }
}

static Scheme_Object *udp_send_it(const char *name, int argc, Scheme_Object *argv[],
				  int with_addr, int can_block, Scheme_UDP_Evt *fill_evt)
{
#ifdef UDP_IS_SUPPORTED
  Scheme_UDP *udp;
  char *address = "";
  long start, end;
  int delta, err;
  unsigned short origid, id;
  GC_CAN_IGNORE struct mz_addrinfo *udp_dest_addr;

  udp = (Scheme_UDP *)argv[0];
#endif

  if (!SCHEME_UDPP(argv[0]))
    scheme_wrong_type(name, "udp socket", 0, argc, argv);

#ifdef UDP_IS_SUPPORTED
  if (with_addr) {
    if (!SCHEME_CHAR_STRINGP(argv[1]))
      scheme_wrong_type(name, "string", 1, argc, argv);
    if (!CHECK_PORT_ID(argv[2]))
      scheme_wrong_type(name, PORT_ID_TYPE, 2, argc, argv);
    delta = 0;
  } else
    delta = -2;

  if (!SCHEME_BYTE_STRINGP(argv[3 + delta]))
    scheme_wrong_type(name, "byte string", 3 + delta, argc, argv);
  
  scheme_get_substring_indices(name, argv[3 + delta], 
			       argc, argv,
			       4 + delta, 5 + delta, &start, &end);

  if (with_addr) {
    Scheme_Object *bs;
    bs = scheme_char_string_to_byte_string(argv[1]);
    address = SCHEME_BYTE_STR_VAL(bs);
    origid = (unsigned short)SCHEME_INT_VAL(argv[2]);

    scheme_security_check_network(name, address, origid, 1);

    id = origid;
  } else {
    address = NULL;
    id = origid = 0;
  }

  if (with_addr)
    udp_dest_addr = scheme_get_host_address(address, id, &err, -1, 0, 0);
  else
    udp_dest_addr = NULL;

  if (!with_addr || udp_dest_addr) {
    if (fill_evt) {
      char *s;
      fill_evt->str = SCHEME_BYTE_STR_VAL(argv[3+delta]);
      fill_evt->offset = start;
      fill_evt->len = end - start;
      if (udp_dest_addr) {
	s = (char *)scheme_malloc_atomic(udp_dest_addr->ai_addrlen);
	memcpy(s, udp_dest_addr->ai_addr, udp_dest_addr->ai_addrlen);
	fill_evt->dest_addr = s;
	fill_evt->dest_addr_len = udp_dest_addr->ai_addrlen;
	mz_freeaddrinfo(udp_dest_addr);
      }
      return scheme_void;
    } else {
      Scheme_Object *r;
      r = do_udp_send_it(name, udp,
			 SCHEME_BYTE_STR_VAL(argv[3+delta]), start, end,
			 (udp_dest_addr ? (char *)udp_dest_addr->ai_addr : NULL),
			 (udp_dest_addr ? udp_dest_addr->ai_addrlen : 0),
			 can_block);
      if (udp_dest_addr)
	mz_freeaddrinfo(udp_dest_addr);
      return r;
    }
  } else {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "%s: can't resolve address: %s (%N)", 
		     name,
		     address, 1, err);
    return NULL;
  }
#else
  return scheme_void;
#endif
}

static Scheme_Object *udp_send_to(int argc, Scheme_Object *argv[])
{
  return udp_send_it("udp-send-to", argc, argv, 1, 1, NULL);
}

static Scheme_Object *udp_send(int argc, Scheme_Object *argv[])
{
  return udp_send_it("udp-send", argc, argv, 0, 1, NULL);
}

static Scheme_Object *udp_send_to_star(int argc, Scheme_Object *argv[])
{
  return udp_send_it("udp-send-to*", argc, argv, 1, 0, NULL);
}

static Scheme_Object *udp_send_star(int argc, Scheme_Object *argv[])
{
  return udp_send_it("udp-send*", argc, argv, 0, 0, NULL);
}

static Scheme_Object *udp_send_to_enable_break(int argc, Scheme_Object *argv[])
{
  return scheme_call_enable_break(udp_send_to, argc, argv);
}

static Scheme_Object *udp_send_enable_break(int argc, Scheme_Object *argv[])
{
  return scheme_call_enable_break(udp_send, argc, argv);
}

#ifdef UDP_IS_SUPPORTED

static int udp_check_recv(Scheme_Object *_udp)
{
  Scheme_UDP *udp = (Scheme_UDP *)_udp;

  if (udp->s == INVALID_SOCKET)
    return 1;

  {
    DECL_FDSET(readfds, 1);
    DECL_FDSET(exnfds, 1);
    struct timeval time = {0, 0};
    int sr;
    
    INIT_DECL_FDSET(readfds, 1);
    INIT_DECL_FDSET(exnfds, 1);
    
    MZ_FD_ZERO(readfds);
    MZ_FD_SET(udp->s, readfds);
    MZ_FD_ZERO(exnfds);
    MZ_FD_SET(udp->s, exnfds);
    
    do {
      sr = select(udp->s + 1, readfds, NULL, exnfds, &time);
    } while ((sr == -1) && (NOT_WINSOCK(errno) == EINTR));
    
    return sr;
  }
}

static void udp_recv_needs_wakeup(Scheme_Object *_udp, void *fds)
{
  Scheme_UDP *udp = (Scheme_UDP *)_udp;
  void *fds1, *fds2;

  tcp_t s = udp->s;
  
  fds1 = MZ_GET_FDSET(fds, 0);
  fds2 = MZ_GET_FDSET(fds, 2);
  
  MZ_FD_SET(s, (fd_set *)fds1);
  MZ_FD_SET(s, (fd_set *)fds2);
}

#endif

static int do_udp_recv(const char *name, Scheme_UDP *udp, char *bstr, long start, long end, 
		       int can_block, Scheme_Object **v)
{
#ifdef UDP_IS_SUPPORTED
  long x;
  int errid = 0;
  char src_addr[MZ_SOCK_NAME_MAX_LEN];
  int asize = sizeof(src_addr);

  if (!udp->bound) {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "%s: udp socket is not bound: %V",
		     name,
		     udp);
    return 0;
  }

  while (1) {
    if (udp->s == INVALID_SOCKET) {
      /* socket was closed, maybe while we slept */
      scheme_raise_exn(MZEXN_FAIL_NETWORK,
		       "%s: udp socket is closed: %V",
		       name, udp);
      return 0;
    }

    {
      x = recvfrom(udp->s, bstr XFORM_OK_PLUS start, end - start, 0,
		   (struct sockaddr *)src_addr, &asize);
    }

    if (x == -1) {
      errid = SOCK_ERRNO();
      if (WAS_WSAEMSGSIZE(errid)) {
	x = end - start;
	errid = 0;
      } if (WAS_EAGAIN(errid)) {
	if (can_block) {
	  /* Block and eventually try again. */
	  scheme_block_until(udp_check_recv, udp_recv_needs_wakeup, (Scheme_Object *)udp, 0);
	} else {
	  v[0] = scheme_false;
	  v[1] = scheme_false;
	  v[2] = scheme_false;
	  return 0;
	}
      } else if (NOT_WINSOCK(errid) != EINTR)
	break;
    } else
      break;
  }
  
  if (x > -1) {
    char host_buf[MZ_SOCK_HOST_NAME_MAX_LEN];
    char prev_buf[MZ_SOCK_HOST_NAME_MAX_LEN];
    char svc_buf[MZ_SOCK_SVC_NAME_MAX_LEN];
    int j, id;

    v[0] = scheme_make_integer(x);

    scheme_getnameinfo((struct sockaddr *)src_addr, asize,
		       host_buf, sizeof(host_buf),
		       svc_buf, sizeof(svc_buf));
    
    if (udp->previous_from_addr) {
      mzchar *s;
      s = SCHEME_CHAR_STR_VAL(udp->previous_from_addr);
      for (j = 0; s[j]; j++) {
	prev_buf[j] = (char)s[j];
      }
      prev_buf[j] = 0;
    }

    if (udp->previous_from_addr && !strcmp(prev_buf, host_buf)) {
      v[1] = udp->previous_from_addr;
    } else {
      Scheme_Object *vv;
      vv = scheme_make_immutable_sized_utf8_string(host_buf, -1);
      v[1] = vv;
      udp->previous_from_addr = v[1];
    }

    id = 0;
    for (j = 0; svc_buf[j]; j++) {
      id = (id * 10) + (svc_buf[j] - '0');
    }

    v[2] = scheme_make_integer(id);

    return 1;
  } else {
    scheme_raise_exn(MZEXN_FAIL_NETWORK,
		     "%s: receive failed (%E)", 
		     name,
		     errid);
    return 0;
  }
#else
  return 0;
#endif
}

static Scheme_Object *udp_recv(const char *name, int argc, Scheme_Object *argv[], 
			       int can_block, Scheme_UDP_Evt *fill_evt)
{
  Scheme_UDP *udp;
  long start, end;
  Scheme_Object *v[3];

  udp = (Scheme_UDP *)argv[0];

  if (!SCHEME_UDPP(argv[0]))
    scheme_wrong_type(name, "udp socket", 0, argc, argv);
  if (!SCHEME_BYTE_STRINGP(argv[1]) || !SCHEME_MUTABLEP(argv[1]))
    scheme_wrong_type(name, "mutable byte string", 1, argc, argv);
  
  scheme_get_substring_indices(name, argv[1], 
			       argc, argv,
			       2, 3, &start, &end);
  if (fill_evt) {
    fill_evt->str = SCHEME_BYTE_STR_VAL(argv[1]);
    fill_evt->offset = start;
    fill_evt->len = end - start;
    return scheme_void;
  } else {
    do_udp_recv(name, udp, SCHEME_BYTE_STR_VAL(argv[1]), start, end, can_block, v);
    
    return scheme_values(3,v);
  }
}

static Scheme_Object *udp_receive(int argc, Scheme_Object *argv[])
{
  return udp_recv("udp-receive!", argc, argv, 1, NULL);
}

static Scheme_Object *udp_receive_star(int argc, Scheme_Object *argv[])
{
  return udp_recv("udp-receive!*", argc, argv, 0, NULL);
}

static Scheme_Object *udp_receive_enable_break(int argc, Scheme_Object *argv[])
{
  return scheme_call_enable_break(udp_receive, argc, argv);
}

static Scheme_Object *make_udp_evt(const char *name, int argc, Scheme_Object **argv, int for_read)
{
#ifdef UDP_IS_SUPPORTED
  Scheme_UDP_Evt *uw;
#endif

  if (!SCHEME_UDPP(argv[0]))
    scheme_wrong_type(name, "udp socket", 0, argc, argv);

#ifdef UDP_IS_SUPPORTED
  uw = MALLOC_ONE_TAGGED(Scheme_UDP_Evt);
  uw->so.type = scheme_udp_evt_type;
  uw->udp = (Scheme_UDP *)argv[0];
  uw->for_read = for_read;

  return (Scheme_Object *)uw;
#else
  return scheme_void;
#endif
}

static Scheme_Object *udp_read_ready_evt(int argc, Scheme_Object *argv[])
{
  return make_udp_evt("udp-receive-ready-evt", argc, argv, 1);
}

static Scheme_Object *udp_write_ready_evt(int argc, Scheme_Object *argv[])
{
  return make_udp_evt("udp-send-ready-evt", argc, argv, 0);
}

static Scheme_Object *udp_read_evt(int argc, Scheme_Object *argv[])
{
  Scheme_Object *evt;
  evt = make_udp_evt("udp-receive!-evt", argc, argv, 1);
  udp_recv("udp-receive!-evt", argc, argv, 0, (Scheme_UDP_Evt *)evt);
  return evt;
}

static Scheme_Object *udp_write_evt(int argc, Scheme_Object *argv[])
{
  Scheme_Object *evt;
  evt = make_udp_evt("udp-send-evt", argc, argv, 0);
  udp_send_it("udp-send-evt", argc, argv, 0, 0, (Scheme_UDP_Evt *)evt);
  return evt;
}

static Scheme_Object *udp_write_to_evt(int argc, Scheme_Object *argv[])
{
  Scheme_Object *evt;
  evt = make_udp_evt("udp-send-to-evt", argc, argv, 0);
  udp_send_it("udp-send-to-evt", argc, argv, 1, 0, (Scheme_UDP_Evt *)evt);
  ((Scheme_UDP_Evt *)evt)->with_addr = 1;
  return evt;
}

#ifdef UDP_IS_SUPPORTED
static int udp_evt_check_ready(Scheme_Object *_uw, Scheme_Schedule_Info *sinfo)
{
  Scheme_UDP_Evt *uw = (Scheme_UDP_Evt *)_uw;

  if (uw->for_read) {
    if (uw->str) {
      Scheme_Object *v[3];
      if (do_udp_recv("udp-receive!-evt", uw->udp, 
		      uw->str, uw->offset, uw->offset + uw->len, 
		      0, v)) {
	scheme_set_sync_target(sinfo, scheme_build_list(3, v), NULL, NULL, 0, 0);
	return 1;
      } else
	return 0;
    } else {
      return udp_check_recv((Scheme_Object *)uw->udp);
    }
  } else {
    if (uw->str) {
      Scheme_Object *r;
      r = do_udp_send_it("udp-send-evt", uw->udp, 
			 uw->str, uw->offset, uw->offset + uw->len, 
			 uw->dest_addr, uw->dest_addr_len,
			 0);
      if (SCHEME_TRUEP(r)) {
	scheme_set_sync_target(sinfo, scheme_void, NULL, NULL, 0, 0);
	return 1;
      } else
	return 0;
    } else
      return udp_check_send((Scheme_Object *)uw->udp);
  }
}

static void udp_evt_needs_wakeup(Scheme_Object *_uw, void *fds)
{
  Scheme_UDP_Evt *uw = (Scheme_UDP_Evt *)_uw;

  if (uw->for_read)
    udp_recv_needs_wakeup((Scheme_Object *)uw->udp, fds);
  else
    udp_send_needs_wakeup((Scheme_Object *)uw->udp, fds);
}
#endif

/*========================================================================*/
/*                       precise GC traversers                            */
/*========================================================================*/

#ifdef MZ_PRECISE_GC

START_XFORM_SKIP;

#define MARKS_FOR_NETWORK_C
#include "mzmark.c"

static void register_traversers(void)
{
#ifdef USE_TCP
  GC_REG_TRAV(scheme_rt_tcp, mark_tcp);
# ifdef UDP_IS_SUPPORTED
  GC_REG_TRAV(scheme_udp_type, mark_udp);
  GC_REG_TRAV(scheme_udp_evt_type, mark_udp_evt);
# endif
#endif
  GC_REG_TRAV(scheme_listener_type, mark_listener);  
}

END_XFORM_SKIP;

#endif

#endif /* !NO_TCP_SUPPORT */
