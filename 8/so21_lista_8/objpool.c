#include "csapp.h"
#include "bitstring.h"

typedef struct {
  int data[8];          /* contents does not matter, sizeof(...) = 32 */
} object_t;

#define ARENA_EXTRA                                                            \
  struct {                                                                     \
    size_t nitems;      /* number of items */                                  \
    size_t nfree;       /* number of free items */                             \
    void *items;        /* pointer to first item */                            \
    bitstr_t bitmap[0]; /* bitmap of free items */                             \
  }

#include "arena.h"

size_t level2_offset = 0;
size_t level1_offset;
size_t bitmap_offset;

#define TEST

#ifdef TEST
#define my_bit_set bit_set
#define my_bit_test bit_test
#define my_bit_clear bit_clear
#define my_bit_ffc bit_ffc
#endif

#ifndef TEST
static inline int my_bit_test(const bitstr_t *bitmap, size_t bit) {
  return bitmap[bitmap_offset + bit / 8] & (1 << (bit % 8));
}

static void my_bit_set(bitstr_t *bitmap, size_t bit) {
  size_t level1_idx = (bit / 8);
  size_t level2_idx = level1_idx / 8;
  bitmap[bitmap_offset + bit / 8] |= (1 << (bit % 8));
  //fprintf(stderr,"set bitmap_val: %x\n", bitmap[bitmap_offset + bit / 8]);
  if (bitmap[bitmap_offset + bit / 8] == 0xff) {
    bitmap[level1_offset + level1_idx / 8]  |= (1 << (level1_idx % 8));
    if (bitmap[level1_offset + level1_idx / 8] == 0xff) {
      bitmap[level2_offset + level2_idx / 8]  |= (1 << (level2_idx % 8));  
    }
  }
  return;
}

static void my_bit_clear(bitstr_t *bitmap, size_t bit) {
  size_t level1_idx = (bit / 8);
  size_t level2_idx = level1_idx / 8;
  //fprintf(stderr,"before set bit %d: %b\n", bit % 8, bitmap[bit / 8]);
  bitmap[bitmap_offset + bit / 8] &= ~(1 << (bit % 8));
  //fprintf(stderr, "after set bit %d: %b\n\n", bit % 8, bitmap[bit / 8]);
  // wyzerowany i-ty bit w bitmapie poziomu k
  // mówi, że w i-tym słowie maszynowym bitmapy poziomu k + 1 występuje co najmniej jeden wyzerowany bit.
  if (bitmap[level1_offset + level1_idx / 8] != 0 && (bitmap[bitmap_offset + bit / 8] != 0xff)) {
    bitmap[level1_offset + level1_idx / 8]  &= ~(1 << (level1_idx % 8));
    if (bitmap[level2_offset + level2_idx / 8] != 0 && (bitmap[level1_offset + level1_idx / 8] != 0xff)) {
      bitmap[level2_offset + level2_idx / 8]  &= ~(1 << (level2_idx % 8));  
    }
  }
  return;
}

static void my_bit_ffc(bitstr_t *bitmap, int *arg_idx) {
  int index;
  size_t level2_idx=0, level1_idx, bitmap_idx;
  bitstr_t *b_p = bitmap;
  while (*b_p == 0xff) {b_p++;level2_idx++;}
  //fprintf(stderr, "level2: %b\n", *b_p);
  //fprintf(stderr, "level2_idx: %b\n", level2_idx);
  __asm__("tzcnt %0, %1" : "=r"(index) : "r"(~(*b_p)));

  level1_idx = level2_idx * 8 + index;
  //fprintf(stderr, "level1: %b\n", bitmap[level1_offset + level1_idx]);
  //fprintf(stderr, "level1_idx: %b\n", level1_idx);
  __asm__("tzcnt %0, %1" : "=r"(index) : "r"(~(bitmap[level1_offset + level1_idx])));

 // fprintf(stderr,"index: %d\n", index);
  
  
  bitmap_idx = level1_idx * 8 + index;
  //fprintf(stderr,"bitmap_idx: %d\n\n", bitmap_idx);
  __asm__("tzcnt %0, %1" : "=r"(index) : "r"(~(bitmap[bitmap_offset + bitmap_idx])));
  bitmap_idx *= 8;
  bitmap_idx += index;
  //fprintf(stderr,"bitmap: %b\n", bitmap[bitmap_offset + bitmap_idx - index]);
  *arg_idx = bitmap_idx;
  //index += offset * sizeof(bitstr_t) * 8;
}
#endif

static arenalist_t arenas = STAILQ_HEAD_INITIALIZER(arenas);

static arena_t *init_arena(arena_t *ar) {
  /* TODO: Calculate nitems given ARENA_SIZE, size of arena_t and object_t. */
  // okay so we have a mmaped memory and at the beginning there's an area header and the bitmap is variable length. 
  size_t bitmap_size = bitstr_size((ARENA_SIZE - sizeof(arena_t)) / sizeof(object_t));
  size_t level1_size = bitmap_size / 8;
  size_t level2_size = level1_size / 8;
  
  level2_offset = 0;
  level1_offset = level2_size;
  bitmap_offset = level2_size + level1_size;
  size_t nitems = (ARENA_SIZE - sizeof(arena_t) - bitmap_size - level2_size - level1_size) / sizeof(object_t);
  ar->nitems = nitems;
  ar->nfree = nitems;
  /* Determine items address that is aligned properly. */
  ar->items = arena_end(ar) - nitems * sizeof(object_t);
  return ar;
}


static void *alloc_block(arena_t *ar) {
  assert(ar->nfree > 0);
  int index;
  /* TODO: Calculate index of free block and mark it used, update nfree. */
  // "Zoptymalizuj procedurę «alloc_block» z poprzedniego zadania. 
  // Główną przyczyną niskiej wydajności jest użycie funkcji «bit_ffc»"
  // i guess musimy użyć bit_ffc w takim razie
  
  //bit_ffc(ar->bitmap, ar->nitems, &index);
#ifdef TEST
  bitstr_t *b_p = ar->bitmap;
  int offset = 0;
  while (*b_p == 0xff) {b_p++;offset++;}
  __asm__("tzcnt %0, %1" : "=r"(index) : "r"(~(*b_p)));
  index += offset * sizeof(bitstr_t) * 8;
#else
  my_bit_ffc(ar->bitmap, &index);
#endif
  
  assert(index != -1);

  my_bit_set(ar->bitmap, index);
  ar->nfree -= 1;
  return ar->items + sizeof(object_t) * index;
}

static void free_block(arena_t *ar, void *ptr) {
  int index = (ptr - ar->items) / sizeof(object_t);
  /* TODO: Determine if ptr is correct and mark it free, update nfree. */
  if (my_bit_test(ar->bitmap, index)) {
    my_bit_clear(ar->bitmap, index);
    ar->nfree += 1;
  }
}

static void *objalloc(void) {
  /* Find arena with at least one free item. */
  arena_t *ar = NULL;
  STAILQ_FOREACH(ar, &arenas, arenalink) {
    if (ar->nfree > 0)
      return alloc_block(ar);
  }
  /* If none found then allocate an item from new arena. */
  return alloc_block(init_arena(alloc_after_arena(&arenas, NULL)));
}

static void objfree(void *ptr) {
  if (ptr == NULL)
    return;
  arena_t *ar = find_ptr_arena(&arenas, ptr);
  assert(ar != NULL);
  free_block(ar, ptr);
}

static void objmemcheck(void) {
  arena_t *ar;
  STAILQ_FOREACH(ar, &arenas, arenalink) {
    /* Check if nfree matches number of cleared bits in bitmap. */
    size_t nused = 0;
    for (int i = 0; i < ar->nitems; i++)
      nused += my_bit_test(ar->bitmap, i) ? 1 : 0;
    assert(nused == ar->nitems - ar->nfree);
  }
}

/* The test */

#define MAX_PTRS 10000
#define CYCLES 100

static void *alloc_fn(int *lenp) {
  *lenp = sizeof(object_t);
  return objalloc();
}

#define free_fn objfree
#define memchk_fn objmemcheck

#include "test.h"
