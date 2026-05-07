#include "yz.h"

unsigned long hash(const char *str)
{
   unsigned long hash = 1469598103934665603;

   while (*str) {
      hash ^= (unsigned char)(*str++);
      hash *= 1099511628211;
   }

   return hash;
}

void hm_push(Node *map, unsigned long hash_result, Entry *entry)
{
   int idx = hash_result % 1024;
   map[idx].hash = hash_result;
   map[idx].ptr = entry;
}
