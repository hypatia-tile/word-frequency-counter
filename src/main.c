#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef double Value;

typedef struct {
  char *key;
  Value value;
} Entry;

uint32_t hashString(const char *key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)key[i];
    hash *= 16777619;
  }
  return hash;
}

enum { I_A, I_B, I_C } buckets;

void insert(Entry *entries, char *key, int length) {
  if (strncmp("a", key, length) == 0) {
    entries[I_A].value++;
  } else if (strncmp("b", key, length) == 0) {
    entries[I_B].value++;
  } else if (strncmp("c", key, length) == 0) {
    entries[I_C].value++;
  }
}

int main() {
  char *inputs[] = {
      "a", "b", "b", "a", "b", "c",
  };
  Entry *entries = (Entry *)malloc(3 * sizeof(Entry));
  entries[I_A].key = "a";
  entries[I_A].value = 0;
  entries[I_B].key = "b";
  entries[I_B].value = 0;
  entries[I_C].key = "c";
  entries[I_C].value = 0;
  for (int i = 0; i < 6; i++) {
    insert(entries, inputs[i], strlen(inputs[i]));
  }
  for (int j = 0; j < 3; j++) {
    printf("%d. key: %s, count: %g\n", j, entries[j].key, entries[j].value);
  }
  return 0;
}
