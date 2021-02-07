#include "mpool.h"
#include <stdlib.h>

// static inline chunk_t*
// advance_by_chunk(pool_t* pool, chunk_t* chunk, size_t n)
// {
//   return (chunk_t*)((char*)chunk + n * pool->chunk_size);
// }

pool_t*
pool_new(size_t chunk_size, size_t chunk_per_blk)
{
  pool_t* pool = malloc(sizeof(pool_t));
  pool->chunk_size = chunk_size;
  pool->chunk_per_blk = chunk_per_blk;
  pool->head = NULL;
  pool->blocks = NULL;
  pool->n_blocks = 0;
  return pool;
}

// destroy the pool
void
pool_del(pool_t* pool)
{
  if (pool->blocks) {
    for (size_t i = 0; i < pool->n_blocks; ++i) {
      free(pool->blocks[i]);
    }
    free(pool->blocks);
  }
  free(pool);
}

void*
pool_alloc(pool_t* pool)
{
  if (!pool->head)
    pool->head = pool_allocBlock(pool); // allocate block by block
  chunk_t* allocted = pool->head;
  pool->head = pool->head->next;;
  return allocted;
}

chunk_t*
pool_allocBlock(pool_t* pool)
{
  chunk_t* block_begin =
    (chunk_t*)malloc(pool->chunk_size * pool->chunk_per_blk);
  chunk_t* chunk = block_begin;
  for (size_t i = 0; i < pool->chunk_per_blk - 1; ++i) {
    chunk->next = (chunk_t*)((char*)chunk + pool->chunk_size);
    chunk = chunk->next;
  }
  chunk->next = NULL; // last chunk

  pool->blocks = realloc(pool->blocks, sizeof(chunk_t*) * (pool->n_blocks + 1));
  pool->blocks[pool->n_blocks] = block_begin;
  pool->n_blocks++;

  return block_begin;
}

// free chucks allocated by the pool
void
pool_free(pool_t* pool, void* obj)
{
  chunk_t* chunk = (chunk_t*)obj;
  chunk->next = pool->head;
  pool->head = chunk;
}
