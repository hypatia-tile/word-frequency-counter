#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 8

typedef double Value;

typedef struct {
  char *data;
  int length;
} KeyStr;

typedef struct {
  KeyStr *key;
  Value value;
} Entry;

typedef struct {
  size_t capacity;
  size_t count;
  Entry *entries;
} Table;

void initTable(Table *table) {
  table->entries = (Entry *)malloc(CAPACITY * sizeof(Entry));
  for (size_t i = 0; i < CAPACITY; i++) {
    table->entries[i].key = (KeyStr *) malloc(sizeof(KeyStr));
    table->entries[i].key->data = NULL;
    table->entries[i].key->length = 0;
    table->entries[i].value = 0;
  }
  table->capacity = CAPACITY;
  table->count = 0;
}

void freeTable(Table *table) {
  for (size_t i = 0; i < table->capacity; i++) {
    free(table->entries[i].key);
    table->entries[i].key = NULL;
  }
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

void insert(Table *table, char *key, int length, Value value) {
  if (table->count >= table->capacity) {
    fprintf(stderr, "insert: Table is full\n");
    exit(EXIT_FAILURE);
  }
  uint32_t hash = hashString(key, length) % table->capacity;
  Entry *const entries = table->entries;
  for (;;) {
    if (entries[hash].key->data == NULL && entries[hash].value == 0) {
      entries[hash].key->data = key;
      entries[hash].key->length = length;
      entries[hash].value = value;
      table->count++;
      break;
    } else if (entries[hash].key->length == length &&
               strncmp(entries[hash].key->data, key, length) == 0) {
      entries[hash].value = value;
      break;
    } else {
      hash = (hash + 1) % table->capacity;
    }
  }
}

int lookup(Value *value, const Table *table, const char *key, int length) {
  uint32_t hash = hashString(key, length) % table->capacity;
  for (size_t i = 0; i < table->capacity; i++) {
    if (table->entries[hash].key->length == length &&
        strncmp(table->entries[hash].key->data, key, length) == 0) {
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
    table->entries[idx].value = val + 1;
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
