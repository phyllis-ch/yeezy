#include "yz.h"

unsigned int hash(char *str)
{
   unsigned int result = 0;

   for (int i = 0; str[i]; ++i) {
      result += str[i];
      result *= str[i];
   }
   return result;
}

void hm_push(Node *map, unsigned int hash_result, Entry *entry)
{
   int idx = hash_result % 1024;
   map[idx].hash = hash_result;
   map[idx].ptr = entry;
}
