#include <kv.h>
#include <string.h>

#define TOMBSTONE ((char *)0x1)

size_t hash(const char *val, int capacity) {
  size_t hash = 0x13371337deadbeef;

  while (*val) {
    hash ^= *val;
    hash = hash << 8;
    hash += *val;

    val++;
  }

  return hash % capacity;
}

// fn kv_put
// params:
//  - db:    a pointer to the db
//  - key:   a pointer to the key value
//  - value: a pointer to the value itself
// returns: the index of the key, otherwise on
// error, returns -1, on db full return -2
int kv_put(kv_t *db, char *key, char *value) {
  if (!db || !key || !value) return -1;

  size_t idx = hash(key, db->capacity);
  if (db->count == db->capacity) return -2;

  for (size_t i = 0; i < db->capacity - 1; i++) {
    size_t real_idx = (idx + i) % db->capacity;
    kv_entry_t *entry = &db->entries[real_idx];

    // key already exists
    if (entry->key &&
      entry->key != TOMBSTONE &&
      strcmp(entry->key, key) == 0)
    {
      char *newval = strdup(value);
      if (!newval) return -1;
      free(entry->value);
      entry->value = newval;
      return 0; //real_idx;
    }

    // new/dead key
    if (!entry->key ||
      entry->key == TOMBSTONE)
    {
      char *newkey = strdup(key);
      char *newval = strdup(value);
      if (!newval || !newkey) {
        free(newkey);
        free(newval);
        return -1;
      }
      entry->key = newkey;
      entry->value = newval;
      db->count++;
      return 0; //real_idx;
    }
  }

  // the db is occupied
  return -2;
}

char *kv_get(kv_t *db, char *key) {
  if (!db || !key) return NULL;

  for (
    size_t idx = hash(key, db->capacity);
    idx < db->capacity;
    idx++
  ) {
    kv_entry_t *entry = &(db->entries[idx]);

    if (!entry->key) {
      break;
    }

    if (entry->key &&
      entry->key != TOMBSTONE &&
      strcmp(entry->key, key) == 0)
    {
      return entry->value;
    }
  }

  return NULL;
}

int kv_delete(kv_t *db, char *key) {
  if (!db || !key) return -1;

  for (
    size_t idx = hash(key, db->capacity);
    idx < db->capacity;
    idx++
  ) {
    kv_entry_t *entry = &(db->entries[idx]);

    if (!entry->key) {
      break;
    }

    if (entry->key &&
      entry->key != TOMBSTONE &&
      strcmp(entry->key, key) == 0)
    {
      free(entry->key);
      free(entry->value);
      entry->key = TOMBSTONE;
      entry->value = NULL;
      db->count--;
      return 0;
    }
  }

  return -1;
}

void kv_free(kv_t *db) {
  if (!db) return;

  for (size_t i = 0; i < db->capacity; i++) {
    kv_entry_t *entry = &db->entries[i];

    if (entry->key &&
      entry->key != TOMBSTONE)
    {
      free(entry->key);
      free(entry->value);
    }
  }
  free(db->entries);
  free(db);
}

kv_t *kv_init(size_t capacity) {
  if (capacity == 0) return NULL;

  kv_t *db = malloc(sizeof(kv_t));
  if (db == NULL) {
    return NULL;
  }

  db->capacity = capacity;
  db->count = 0;

  db->entries = calloc(sizeof(kv_entry_t), capacity);
  if (db->entries == NULL) {
    return NULL;
  }

  return db;
}
