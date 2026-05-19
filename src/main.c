#include <ctype.h>
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
  const char *data;
  int length;
} KeyStr;

typedef enum {
  EMPTY,
  TOMBSTONE,
  OCCUPIED,
} BucketStatus;

typedef struct {
  KeyStr key;
  BucketStatus status;
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
    table->entries[i].status = EMPTY;
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
    table->entries[i].status = EMPTY;
  }

  table->count = 0;
  for (size_t j = 0; j < oldCap; j++) {
    const Entry oldEntry = oldEntries[j];
    if (oldEntry.status != OCCUPIED)
      continue;
    uint32_t hash =
        hashString(oldEntry.key.data, oldEntry.key.length) % table->capacity;
    for (size_t i = 0; i < table->capacity; i++) {
      Entry *newEntry = &table->entries[hash];
      if (newEntry->status == EMPTY) {
        newEntry->key = oldEntry.key;
        newEntry->value = oldEntry.value;
        newEntry->status = OCCUPIED;
        table->count++;
        break;
      } else {
        hash = (hash + 1) % table->capacity;
      }
    }
  }
  FREE_ARRAY(oldEntries);
  return;
}

void insert(Table *table, const char *key, int length, Value value) {
  if (table->count >= table->capacity * LOAD_FACTOR)
    rearrangeEntries(table);

  uint32_t hash = hashString(key, length) % table->capacity;
  Entry *const entries = table->entries;
  for (size_t i = 0; i < table->capacity; i++) {
    switch (entries[hash].status) {
    case EMPTY:
      entries[hash].key.data = key;
      entries[hash].key.length = length;
      entries[hash].value = value;
      entries[hash].status = OCCUPIED;
      table->count++;
      return;
    case TOMBSTONE:
      entries[hash].key.data = key;
      entries[hash].key.length = length;
      entries[hash].value = value;
      entries[hash].status = OCCUPIED;
      return;
    case OCCUPIED:
      if (entries[hash].key.length == length &&
          strncmp(entries[hash].key.data, key, length) == 0) {
        entries[hash].value = value;
        return;
      }
      hash = (hash + 1) % table->capacity;
    }
  }
  // Unreachable
  fprintf(stderr, "Error: Cannot insert entry, table is full.\n");
  exit(EXIT_FAILURE);
}

int lookup(Value *value, const Table *table, const char *key, int length) {
  uint32_t hash = hashString(key, length) % table->capacity;
  for (size_t i = 0; i < table->capacity; i++) {
    if (table->entries[hash].status == OCCUPIED &&
        table->entries[hash].key.length == length &&
        strncmp(table->entries[hash].key.data, key, length) == 0) {
      *value = table->entries[hash].value;
      return hash;
    }
    hash = (hash + 1) % table->capacity;
  }
  return -1;
}

void countup(Table *table, const char *key, int length) {
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

int deleteEntry(Table *table, const char *key, int length) {
  Value val;
  int idx = lookup(&val, table, key, length);
  if (idx < 0)
    return 0;
  table->entries[idx].status = TOMBSTONE;
  return 1;
}

static inline int isWordChar(char c) { return isalnum((unsigned char)c); }

void parseAndCount(Table *table, const char *text) {
  const char *start = text;
  const char *current = text;

  while (*current != '\0') {
    // Skip punctuations.
    while (*current != '\0' && !isWordChar(*current)) {
      current++;
    }
    // Sync
    start = current;
    // Scan word.
    while (*current != '\0' && isWordChar(*current)) {
      current++;
    }
    if (current > start)
      countup(table, start, current - start);
  }
}

int main() {
  Table table;
  initTable(&table);
  const char *test_doc =
      "Apple, Banana, banana! Blueberry... grape, apple; banana.";
  printf("Parsing text and counting words...\n");
  parseAndCount(&table, test_doc);
  search(&table, "apple");
  search(&table, "banana");
  search(&table, "blueberry");
  search(&table, "grape");
  search(&table, "orange");
  freeTable(&table);
  return 0;
}
