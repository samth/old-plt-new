/* 
   Provides:
      posix_memalign-based allocator (uses alloc_cache.c)
      determine_max_heap_size() (uses rlimit_heapsize.c)
   Requires:
      my_qsort (for alloc_cache.c)
      LOGICALLY_ALLOCATING_PAGES(len)
      ACTUALLY_ALLOCATING_PAGES(len)
      LOGICALLY_FREEING_PAGES(len)
      ACTUALLY_FREEING_PAGES(len)
   Optional:
      CHECK_USED_AGAINST_MAX(len)
      DONT_NEED_MAX_HEAP_SIZE --- to disable a provide
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>

#ifndef CHECK_USED_AGAINST_MAX
# define CHECK_USED_AGAINST_MAX(x) /* empty */
#endif

static int page_size; /* OS page size */

static void *malloc_pages(size_t len, size_t alignment)
{
  void *r;
  size_t extra = 0;

  if (!page_size)
    page_size = getpagesize();

  CHECK_USED_AGAINST_MAX(len);

  /* Round up to nearest page: */
  if (len & (page_size - 1))
    len += page_size - (len & (page_size - 1));

  /* Something from the cache, perhaps? */
  r = find_cached_pages(len, alignment);
  if (r)
    return r;

  if (posix_memalign(&r, alignment, len))
    return NULL;

  ACTUALLY_ALLOCATING_PAGES(len);
  LOGICALLY_ALLOCATING_PAGES(len);

  return r;
}

static void system_free_pages(void *p, size_t len)
{
  free(p);
}

static void protect_pages(void *p, size_t len, int writeable)
{
  if (len & (page_size - 1)) {
    len += page_size - (len & (page_size - 1));
  }

  mprotect(p, len, (writeable ? (PROT_READ | PROT_WRITE) : PROT_READ));
}

# include "alloc_cache.c"

/*************************************************************/

# include "rlimit_heapsize.c"
