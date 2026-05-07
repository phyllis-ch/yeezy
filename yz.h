#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

typedef struct {
   char *pathname;
   size_t pathname_len;
   double frecency_score;
   time_t last_visited;
} Entry;

typedef struct {
   Entry *items;
   size_t count;
   size_t capacity;
} Entries;

// Wrapper struct for pointer to Entry with score
typedef struct {
   Entry *entry;
   double score;
} Entry_Wrapper;

typedef struct {
   Entry_Wrapper *items;
   size_t count;
   size_t capacity;
} Wrappers;

typedef struct {
   unsigned int hash;
   Entry *ptr;
} Node;

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

// main
void parse_flags(int argc, char *argv[]);
void da_filter(Wrappers *filtered_arr, Entries *arr, char *pattern, size_t idx);
double get_decayed_score(char *pattern, Entry entry, double decay);
char *get_data_home(void);
int comp_score(const void *a, const void *b);
int comp_freq(const void *a, const void *b);

// db
void db_write(FILE *db, Entry *entry);
void db_read(FILE *db, Entries *arr);
void db_add(FILE *db, char *str);

// fuzzy
char *get_basename(char *text);
int match_seperator(char c);
int get_basescore(const char *pattern, char *text);
int get_fzscore(const char *pattern, char *text);
