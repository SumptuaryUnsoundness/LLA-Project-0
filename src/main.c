#include <stdio.h>
#include <kv.h>
#include <assert.h>

int main() {
  kv_t *db = kv_init(16);
  printf("%p\n", db);
  printf("%ld\n", db->capacity);

  printf("k %i\n", kv_put(db, "hehe", "haha"));
  printf("k %i\n", kv_put(db, "hehe", "hoho"));
  printf("k %i\n", kv_put(db, "lala", "test"));

  for (int i = 0; i < db->capacity; i++) {
    if (db->entries[i].key) {
      printf("[%d] %s: %s\n", i, db->entries[i].key, db->entries[i].value);
    }
  }

  kv_free(db);
  return 0;
}
