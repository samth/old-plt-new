/*
  Provides:
  Requires:
   [page_range.c exports]
   [page allocator]
*/

static Page_Range protect_range;

static void initialize_protect_page_ranges(void *block, unsigned long size)
{
  initialize_page_ranges(&protect_range, block, size);
}

static void flush_protect_page_ranges(int writeable)
{
  Range *work;

  compact_page_ranges(&protect_range);

  for (work = protect_range.range_start; work; work = work->next) {
    protect_pages((void *)work->start, work->len, writeable);
  }

  reset_page_ranges(&protect_range);
}

static void add_protect_page_range(void *_start, unsigned long len, unsigned long alignment, int writeable)
{
  if (!add_page_range(&protect_range, _start, len, alignment)) {
    flush_protect_page_ranges(writeable);
    add_page_range(&protect_range, _start, len, alignment);
  }
}
