#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define ENTRY_SIZE 256

typedef struct {
   char entry[ENTRY_SIZE];
   double frequency_score;
} Entry;

typedef struct {
   Entry *items;
   size_t count;
   size_t capacity;
} Entries;

// Wrapper struct for pointer to Entry with score
typedef struct {
   Entry *name;
   double score;
   size_t db_index;
} Scored_Entry;

typedef struct {
   Scored_Entry *items;
   size_t count;
   size_t capacity;
} Scored_Entries;

#define ARR_COUNT(arr) (int)sizeof(arr) / (int)sizeof(arr[0])

#define da_append(xs, x)\
   do {\
      if (xs.count >= xs.capacity) {\
         if (xs.capacity == 0) xs.capacity = 256;\
         else xs.capacity *= 2;\
         xs.items = realloc(xs.items, xs.capacity*sizeof(*xs.items));\
      }\
      xs.items[xs.count++] = x;\
   } while(0)

int match_seperator(char c);
int get_fzscore(const char *pattern, const char *text);
