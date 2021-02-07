#ifndef _MPOOL_H_
#define _MPOOL_H_

#include <stddef.h>
#ifdef __cplusplus
extern "C"
{
#endif
  typedef struct Chunk
  {
    struct Chunk* next;
  } chunk_t;

  // typedef struct Block
  // {

  // } block_t;

  typedef struct Pool
  {
    size_t chunk_size;    // (sizeof(obj))
    size_t chunk_per_blk; // * chunk_size = block_size
    chunk_t* head;        // the current allocatable chunck
    chunk_t** blocks;
    size_t n_blocks;
  } pool_t;

  pool_t* pool_new(size_t chunk_size, size_t block_size);
  void pool_del(pool_t* pool);

  void* pool_alloc(pool_t* pool);
  void pool_free(pool_t* pool, void* obj);

  chunk_t* pool_allocBlock(pool_t* pool);

#ifdef __cplusplus
}
#endif
#endif
