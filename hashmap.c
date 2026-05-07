#include "yz.h"

unsigned long hash(const char *str)
{
   unsigned long hash = 1469598103934665603; /* constant I took from wikipedia*/

   while (*str) {
      hash ^= (unsigned char)(*str++);
      hash *= 1099511628211; /* constant I took from wikipedia*/
   }

   return hash;
}

void hm_push(Node *map, unsigned long hash_result, int bucket_size, Entry *entry)
{
   int idx = hash_result % bucket_size;
   map[idx].hash = hash_result;
   map[idx].ptr = entry;
}
