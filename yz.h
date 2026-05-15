#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <math.h>

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

typedef struct { /* Wrapper struct for pointer to Entry with score */
   Entry *entry;
   double score;
} Entry_Wrapper;

typedef struct {
   Entry_Wrapper *items;
   size_t count;
   size_t capacity;
} Wrappers;

typedef struct {
   unsigned long hash;
   Entry *ptr;
} Node;

#define ARR_COUNT(arr) (int)sizeof(arr) / (int)sizeof(arr[0])

#define ENTRIES_PRINT(xs)\
   for (size_t i = 0; i < xs.count; ++i) {\
      printf("%s -> %f\n", xs.items[i].pathname, xs.items[i].frecency_score);\
   }

#define da_append(xs, x)\
   do {\
      if (xs.count >= xs.capacity) {\
         if (xs.capacity == 0) xs.capacity = 256;\
         else xs.capacity *= 2;\
         xs.items = realloc(xs.items, xs.capacity*sizeof(*xs.items));\
      }\
      xs.items[xs.count++] = x;\
   } while(0)

/* main */
int parse_flags(int argc, char *argv[]);
int comp_by_score(const void *a, const void *b);
int comp_by_freq(const void *a, const void *b);
int check_special_paths(char *argv[]);
double get_decayed_score(char *pattern, Entry *entry, time_t time_now);
char *get_data_home(void);
void da_filter(Wrappers *filtered_entries, Entry *entry, char *pattern);

/* cmd */
int cmd_query(FILE *db, const char *db_path, char *argv[], Entries entries);
int cmd_add(FILE *db, const char *db_path, char *argv[], Entries entries);
int cmd_list(FILE *db, const char *db_path, char *argv[], Entries entries);
int cmd_remove(FILE *db, const char *db_path, char *argv[], Entries entries);

/* db */
void db_write(FILE *db, Entry *entry);
void db_read(FILE *db, Entries *entries);
void db_append(FILE *db, char *str);

/* fuzzy */
char *get_basename(char *text);
int match_seperator(char c);
int get_basescore(const char *pattern, char *text);
int get_fzscore(const char *pattern, char *text);
