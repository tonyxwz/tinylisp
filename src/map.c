#include "map.h"
#include "lobj.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

uint32_t
hash(const map_t* map, const char* key)
{
  unsigned long int value = 0;
  unsigned int i = 0;
  unsigned int key_len = strlen(key);
  
  for(; i < key_len; ++i) {
    value = value * 33 + key[i];
  }
  return value % map->max_size;
}

entry_t*
entry_new(const char* key, const lobj* val)
{
  entry_t* entry = malloc(sizeof(entry_t));
  entry->key = malloc(strlen(key) + 1);
  strcpy(entry->key, key);
  entry->val = lobj_copy(val);
  entry->next = NULL; // in case of collision
  return entry;
}

void
entry_free(entry_t* entry)
{
  free(entry->key);
  lobj_del(entry->val);
  if (entry->next) // TODO
    entry_free(entry->next);
  free(entry);
}

entry_t*
entry_copy(entry_t* other)
{
  entry_t* new_entry = entry_new(other->key, other->val);
  return new_entry;
}

map_t*
map_new(int size)
{
  map_t* map = malloc(sizeof(map_t));
  map->max_size = size;
  map->entries = malloc(sizeof(entry_t*) * map->max_size);
  for (int i = 0; i < size; ++i)
    map->entries[i] = NULL;
  return map;
}

void
map_free(map_t* map)
{
  for (int i = 0; i < map->max_size; ++i)
    if (map->entries[i])
      entry_free(map->entries[i]);
  free(map->entries);
  free(map);
}

map_t*
map_copy(map_t* other)
{
  map_t* nm = map_new(other->max_size);
  for (int i = 0; i < other->max_size; ++i) {
    if (other->entries[i] != NULL) {
      entry_t* c1 = other->entries[i];

      entry_t* c2 = nm->entries[i];
      entry_t* p2 = NULL;

      while (c1) {
        c2 = entry_copy(c1);
        if (p2)
          p2->next = c2;
        p2 = c2;
        c1 = c1->next;
        c2 = c2->next;
      }
    }
  }
  return nm;
}

void
map_insert(map_t* map, char* key, lobj* val)
{
  uint32_t slot = hash(map, key);
  entry_t* entry = map->entries[slot];

  // entry_t* new_entry = entry_new(key, val);
  if (entry == NULL) { // no collision at all
    map->entries[slot] = entry_new(key, val);
    return;
  }

  entry_t* parent = NULL;
  while (entry) {
    // exist
    if (strcmp(key, entry->key) == 0) {
      lobj_del(entry->val);
      entry->val = lobj_copy(val);
      return;
    }
    parent = entry;
    entry = entry->next;
  }
  // hash collision but not exist
  // c is NULL, p is the last of the linked list
  parent->next = entry_new(key, val);
}

lobj*
map_get(map_t* map, char* key)
{
  uint32_t slot = hash(map, key);
  entry_t* entry = map->entries[slot];
  if (entry == NULL)
    return NULL;

  while (entry != NULL) {
    if (strcmp(entry->key, key) == 0) {
      return entry->val;
    }
    entry = entry->next;
  }
  return NULL;
}

int
map_erase(map_t* map, char* key)
{
  uint32_t slot = hash(map, key);
  entry_t* entry = map->entries[slot];
  if (entry == NULL)
    return -1; // not erased
  entry_t* parent = NULL;
  while (entry != NULL) {
    if (strcmp(entry->key, key) == 0) {
      if (parent) {
        parent->next = entry->next;
      } else {
        map->entries[slot] = entry->next;
      }
      entry->next = NULL;
      entry_free(entry);
      return 0;
    }
    parent = entry;
    entry = entry->next;
  }
  return -1;
}
