#include <stdint.h>

typedef double Value ;

typedef struct {
  char *key;
  Value value;
} Entry;

uint32_t hashString(const char *key, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t) key[i];
    hash *= 16777619;
  }
  return hash;
}

int main() {
  return 0;
}
