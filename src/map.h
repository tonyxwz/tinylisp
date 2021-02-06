#ifndef _MAP_H_
#define _MAP_H_
#include <stdint.h>
#include <string.h>
#include "lobj.h"


typedef struct entry_t
{
  char* key;
  lobj* val;
  struct entry_t* next;
} entry_t;

typedef struct
{
  entry_t** entries;
  int max_size;
} map_t;

uint32_t
hash(const map_t* map, const char* key); // http://www.cse.yorku.ca/~oz/hash.html

entry_t*
entry_new(const char* key, const lobj* val);
entry_t*
entry_copy(entry_t* other);
void
entry_free(entry_t* entry);

map_t*
map_new(int size);
void
map_free(map_t* map);
map_t*
map_copy(map_t* other);

void
map_insert(map_t* map, char* key, lobj* val);
lobj*
map_get(map_t* map, char* key);
int
map_erase(map_t* map, char* key);
#endif
