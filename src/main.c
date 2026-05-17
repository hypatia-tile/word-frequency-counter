#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 8

#define GROW_CAPACITY(x) (x == 0 ? 8 : x * 2)

#define GROW_ARRAY(type, arr, n) ((type *)reallocate(arr, sizeof(type) * n))

#define FREE_ARRAY(arr) (reallocate(arr, 0))

#define LOAD_FACTOR 0.75

void *reallocate(void *arr, size_t n) {
  if (n == 0) {
    free(arr);
    return NULL;
  }
  return realloc(arr, n);
}

typedef double Value;

typedef struct {
  char *data;
  int length;
} KeyStr;

typedef struct {
  KeyStr key;
  Value value;
} Entry;

typedef struct {
  size_t capacity;
  size_t count;
  Entry *entries;
} Table;

void initTable(Table *table) {
  table->entries = GROW_ARRAY(Entry, NULL, CAPACITY);
  for (size_t i = 0; i < CAPACITY; i++) {
    table->entries[i].key.data = NULL;
    table->entries[i].key.length = 0;
    table->entries[i].value = 0;
  }
  table->capacity = CAPACITY;
  table->count = 0;
}

void freeTable(Table *table) {
  free(table->entries);
  table->entries = NULL;
  table->capacity = 0;
  table->count = 0;
}

uint32_t hashString(const char *key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

static inline void rearrangeEntries(Table *table) {
  Entry *oldEntries = table->entries;
  size_t oldCap = table->capacity;
  table->capacity = GROW_CAPACITY(oldCap);
  table->entries = GROW_ARRAY(Entry, NULL, table->capacity);
  for (size_t i = 0; i < table->capacity; i++) {
    table->entries[i].key.data = NULL;
    table->entries[i].key.length = 0;
    table->entries[i].value = 0;
  }

  table->count = 0;
  for (size_t j = 0; j < oldCap; j++) {
    const Entry oldEntry = oldEntries[j];
    if (oldEntry.key.data == NULL)
      continue;
    uint32_t hash =
        hashString(oldEntry.key.data, oldEntry.key.length) % table->capacity;
    for (size_t i = 0; i < table->capacity; i++) {
      Entry *newEntry = &table->entries[hash];
      if (newEntry->key.data == NULL) {
        if (newEntry->value == 0) {
          newEntry->key = oldEntry.key;
          newEntry->value = oldEntry.value;
          table->count++;
          break;
        }
      } else if (newEntry->key.length == oldEntry.key.length &&
                 strncmp(newEntry->key.data, oldEntry.key.data,
                         oldEntry.key.length) == 0) {
        newEntry->value = oldEntry.value;
        break;
      }
      hash = (hash + 1) % table->capacity;
    }
  }
  FREE_ARRAY(oldEntries);
  return;
}

void insert(Table *table, char *key, int length, Value value) {
  if (table->count >= table->capacity * LOAD_FACTOR)
    rearrangeEntries(table);

  uint32_t hash = hashString(key, length) % table->capacity;
  Entry *const entries = table->entries;
  for (size_t i = 0; i < table->capacity; i++) {
    if (entries[hash].key.data == NULL && entries[hash].value == 0) {
      entries[hash].key.data = key;
      entries[hash].key.length = length;
      entries[hash].value = value;
      table->count++;
      return;
    } else if (entries[hash].key.length == length &&
               strncmp(entries[hash].key.data, key, length) == 0) {
      entries[hash].value = value;
      return;
    } else {
      hash = (hash + 1) % table->capacity;
    }
  }
  // Unreachable
  fprintf(stderr, "Error: Table is full, cannot insert key: %s\n", key);
  exit(EXIT_FAILURE);
}

int lookup(Value *value, const Table *table, const char *key, int length) {
  uint32_t hash = hashString(key, length) % table->capacity;
  for (size_t i = 0; i < table->capacity; i++) {
    if (table->entries[hash].key.data != NULL &&
        table->entries[hash].key.length == length &&
        strncmp(table->entries[hash].key.data, key, length) == 0) {
      *value = table->entries[hash].value;
      return hash;
    }
    hash = (hash + 1) % table->capacity;
  }
  return -1;
}

void countup(Table *table, char *key, int length) {
  Value val;
  int idx;
  if ((idx = lookup(&val, table, key, length)) >= 0) {
    insert(table, key, length, val + 1);
  } else {
    insert(table, key, length, 1);
  }
}

void static search(const Table *table, const char *key) {
  Value val;
  int idx = lookup(&val, table, key, strlen(key));
  if (idx < 0) {
    printf("No entry found for key: %s\n", key);
  } else {
    printf("[%d] Entry found for key: %s, value: %g\n", idx, key, val);
  }
}

int main() {
  Table table;
  char *inputs[] = {"apple", "banana", "banana", "blueberry", "grape"};
  initTable(&table);
  for (size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
    countup(&table, inputs[i], strlen(inputs[i]));
  }
  search(&table, "apple");
  search(&table, "banana");
  search(&table, "blueberry");
  search(&table, "grape");
  search(&table, "orange");
  freeTable(&table);
  return 0;
}
