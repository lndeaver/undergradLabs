/*
  Filename   : mm.c
  Author     : Lauren Deaver
  Course     : CSCI 380-01
  Assignment : Assignment 8: Malloc(Dynamic Memory Allocation)
  Description: Write own version of malloc, free, and realloc in
                order to become more familiar with concepts of dynamic 
                storage allocators. Use static inline functions as 
                descriped on the guide instead of using macros, and 
                write a mm_check to check the heap and fucntions 
                for debugging and accuracy
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "memlib.h"
#include "mm.h"

/****************************************************************/
// Useful type aliases

typedef uint64_t word;
typedef uint32_t tag;
typedef uint8_t  byte;
typedef byte*    address; 

/****************************************************************/
// Useful constants

const uint8_t WORD_SIZE = sizeof (word);
const uint8_t DWORD_SIZE = 2 * sizeof (word);
const uint8_t MIN_BLOCK_SIZE = 2;
const uint8_t OVERHEAD = 2 * sizeof(tag);
// Add others... 

address g_heapBase;
/****************************************************************/
// Inline functions

static inline tag*
header (address p);

static inline bool
isAllocated (address p);

static inline tag
sizeOf (address p);

static inline tag*
footer (address p);

static inline address
nextBlock (address p);

static inline tag*
prevFooter (address p);

static inline tag*
nextHeader (address p);

static inline address
prevBlock (address p);

static inline void
makeBlock (address p, uint32_t size, bool alloc);

static inline address
toggleBlock (address p);

static inline uint32_t
allignmentCalc (uint32_t size);

static inline address
extend_heap (address p, uint32_t num_word);

static inline address
coalesce (address p);

/****************************************************************/
// Non-inline functions
/****************************************************************/

/* mm_init - initalize heap base
* moves the g_heapBase pointer to be 16 past the beginning of the 
* heap, then sets prologue and epiloge footer/header for containing
* blocks in the heap.
* starts off with zero free block in heap
*/
int
mm_init (void)
{
  address heap_listprt;
  if ((heap_listprt = mem_sbrk(2 * WORD_SIZE)) == (void *)-1)
  {
    return -1;
  }
  g_heapBase = heap_listprt + DWORD_SIZE;
  
  *prevFooter(g_heapBase) = (0 | true); //prologue footer
  *header(g_heapBase) = (0 | true); //epilogue header
  
  return 0;
}

/****************************************************************/

/* mm_malloc - takes a size of bytes and allocates a block in the
* heap based on the byte size given.
* if given a size of 0 btyes will not allocate or add any blocks to
* heap.
* checks each block to see if it is allocated and will be a fit for
* how many bytes need to be allocated, if the block if bigger than
* needed and has enough overhang for a block of at least 2, the block
* will be slip accordingly, if the block is exactly the size need it
* will be allocated, and if no blocks are found free that will fit
* the heap will be expanded and then a block will be allocated.
*/
void*
mm_malloc (uint32_t size)
{
  address temp_ptr = g_heapBase;
  uint32_t num_words = allignmentCalc(size);
  uint32_t ptr_size = sizeOf(temp_ptr);

  if (size == 0)
  {
    return NULL;
  }

  while (sizeOf(temp_ptr) != 0)
  {
    if(isAllocated(temp_ptr))
    {
      temp_ptr = nextBlock(temp_ptr);
      ptr_size = sizeOf(temp_ptr);
      continue;
    }

    if ((!isAllocated(temp_ptr) && ((sizeOf(temp_ptr) - num_words) >= MIN_BLOCK_SIZE)) && num_words <= ptr_size)
    {

      makeBlock(temp_ptr, num_words, true);
      makeBlock(nextBlock(temp_ptr), ptr_size - num_words, false);

      return temp_ptr;
    }
    else if (!isAllocated(temp_ptr) && (ptr_size == num_words))
    {
      toggleBlock(temp_ptr);
      return temp_ptr;
    }
    temp_ptr = nextBlock(temp_ptr);
    ptr_size = sizeOf(temp_ptr);
  }

  if ((temp_ptr = extend_heap(temp_ptr, num_words)) == NULL)
  {
    return NULL;
  }

  makeBlock (temp_ptr, num_words, true);
  return temp_ptr;
}

/****************************************************************/

/* mm_free - takes in the address to a pointer and frees the block
* by toggling its allocated bit, then checks to see if it can be
* coalesced with any other free blocks around it
*/
void
mm_free (void *ptr)
{
  toggleBlock(ptr);
  coalesce(ptr);
}

/****************************************************************/

/* mm_realloc - takes a pointer and a size in bytes.
* pointer is referring to which address needing to be changes, and
* size if how mnay bytes the size should be changed to. If the size
* matches the size it's already at, return the original pointer, if
* the size is smaller than what it's already at, split the block 
* based on what needs to still be allocated then free the rest, if 
* size is bigger then what it already is malloc a new block of needed
* size, use mem_cpy to move the information over into the newly made
* block, then free the old block
*/
void*
mm_realloc (void *ptr, uint32_t size)
{
  void *newptr;
  uint32_t num_words = allignmentCalc(size);
  uint32_t old_num_words = sizeOf(ptr);

  if (sizeOf(ptr) == num_words)
  {
    return ptr;
  }

  else if (sizeOf(ptr) > num_words)
  {
    makeBlock (ptr, num_words, true);
    makeBlock (nextBlock(ptr), (old_num_words - sizeOf(ptr)), false);
    return ptr;
  }

  else
  {
    newptr = mm_malloc(size);
    memcpy(newptr, ptr, size);
    mm_free(ptr);
    return newptr;
  }
  fprintf(stderr, "realloc block at %p to %u\n", ptr, size);
  return NULL;
}

/****************************************************************/

/* returns the header of a block */
static inline tag*
header (address p)
{
  return (tag*) (p-sizeof(tag));
}

/****************************************************************/

/* returns true if the block is allocated
* false if it's not allocated
*/
static inline bool
isAllocated (address p)
{
  return *header(p) & (tag)1;
}

/****************************************************************/

/* return the block size */
static inline tag
sizeOf (address p)
{
  return *header(p) & (tag)-2;
}

/****************************************************************/

/* return the footer of a block */
static inline tag*
footer (address p)
{
  return (tag*)p + (sizeOf(p) * 2) - (WORD_SIZE / 4);
}

/****************************************************************/

/* return the address of the next block */
static inline address
nextBlock (address p)
{
  return p + (sizeOf(p) * WORD_SIZE);
}

/****************************************************************/

/* return the footer of the previous block */
static inline tag*
prevFooter (address p)
{
  return (tag*)(p - WORD_SIZE);
}

/****************************************************************/

/* return the header of the next block */
static inline tag*
nextHeader (address p)
{
  return header(nextBlock(p));
}

/****************************************************************/

/* return the address of the previous block */
static inline address
prevBlock (address p)
{
  return p - ((*prevFooter(p) & (tag)-2) * WORD_SIZE);
}

/****************************************************************/

/* creates a new block at address p given the size and allocation
* status
*/
static inline void
makeBlock (address p, uint32_t size, bool alloc)
{
  *header(p) = (size | alloc);
  *footer(p) = (size | alloc);
}

/****************************************************************/

/* change the allocation of a block at address p to the opposite 
* of what it currently is
*/
static inline address
toggleBlock (address p)
{
  *header(p) ^= 1;
  *footer(p) ^= 1;
  return p;
}

/****************************************************************/

/* given a size in bytes, calculates the block size needed to
* satisfy in the heap while also keeping with allignment
*/
static inline uint32_t
allignmentCalc (uint32_t size)
{
  uint32_t size_adjusted = size + OVERHEAD;
  uint32_t num_dwords = (size_adjusted + (uint32_t)(DWORD_SIZE - 1)) / DWORD_SIZE;
  uint32_t num_words = (num_dwords * 2);
  return num_words;
}

/****************************************************************/

/* given address p, will check the blocks around it to see if any
* free blocks can be added together to create a larger free block
* in the heap
*/
static inline address
coalesce (address p)
{
  bool prev_alloc = *prevFooter(p) & (tag)1;
  bool next_alloc = isAllocated(nextBlock(p));
  uint32_t size = sizeOf(p);

  if (prev_alloc && next_alloc)
  {
    return p;
  }

  else if (prev_alloc && !next_alloc)
  {
    size += sizeOf(nextBlock(p));
    makeBlock(p, size, false);
    return p;
  }

  else if (!prev_alloc && next_alloc)
  {
    size += sizeOf(prevBlock(p));
    makeBlock(prevBlock(p), size, false);
    return prevBlock(p);
  }

  else
  {
      size += (sizeOf(prevBlock(p)) + sizeOf(nextBlock(p)));
      makeBlock(prevBlock(p), size, false);
      return prevBlock(p);
  }
  
}

/****************************************************************/

/* given an address and a number of words, will extend the heap at
* p to be of size num_words, if there are free blocks available will 
* use those to help grow, other wise creates new block from scratch
*/
static inline address
extend_heap (address p, uint32_t num_word)
{
  address last_block = prevBlock(p);
  uint32_t expand_by;

  if (isAllocated(last_block))
  {
    expand_by = num_word * WORD_SIZE;
  }
  
  else
  {
    expand_by = (num_word - sizeOf(last_block)) * WORD_SIZE;
  }
  
  if ((p = mem_sbrk ((int)expand_by)) == (void *)-1)
  {
    return (void *)-1;
  }

  makeBlock(p, (expand_by/WORD_SIZE), false);
  *header(nextBlock(p)) = (0 | true); //epilogue header

  return coalesce(p);
  
}

/****************************************************************/

/* given by Dr.Zoppetti, used in mm_check to be able to print out
* blocks for error checking while writing code 
*/
void
printBlock (address p)
{
  printf ("Block Addr %p; Size %u; Alloc %d\n",
	  p, sizeOf (p), isAllocated (p));  
}

/****************************************************************/

/* checks that functions are working correctly and that g_heapBase
* is at the correct spot once the heap is initialized, comments 
* above if blocks state what the heap if currently looking like
* before the next commend is run, statements print when the command
* was successful
*/
int 
mm_check(void)
{
  mem_init();
  //check that init works
  if (mm_init() == -1)
  {
    printf("mm_init failed\n");
  }
  address p0 = mem_heap_lo();
  if (g_heapBase - p0 == 16)
  {
    printf("heapBase in correct spot\n");
  }

  //just 256 block in heap
  address p1 = mm_malloc(2040);
  if (p1 != NULL)
  {
    printf("malloc successfull allocated block\n");
  }

  //no change to heap
  address p2 = mm_malloc(0);
  if(p2 == NULL)
  {
    printf("tried to malloc size 0 - not added\n");
  }

  //free block of 256 in heap
  mm_free(p1);
  if(p1 != NULL)
  {
    printf("free first malloc block- success\n");
  }
  
  //8 block allocated, 248 block free in heap
  address p3 = mm_malloc(48);
  if (p3 != NULL)
  {
    printf("malloc successfull allocated block after a free\n");
  }

  //no change because realloc of same size
  address p4 = mm_realloc(p3, 48);
  if (p4 != NULL)
  {
    printf("realloc success on no size change\n");
  }

  //free block of 8, allocated block of 256
  address p5 = mm_realloc(p4, 2040);
  if (p5 != NULL)
  {
    printf("realloc success on bigger size change\n");
  }

  //free block of 8, allocated block of 4, free block of 252
  address p6 = mm_realloc(p5, 22);
  if (p6 != NULL)
  {
    printf("realloc success on smaller size change\n");
  }

  printf("all blocks at a glance:\n");

  for (address p = g_heapBase; sizeOf (p) != 0; p = nextBlock (p))
  {
    printBlock (p);
  }
  return 0;
}

/****************************************************************/
