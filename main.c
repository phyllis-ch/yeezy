#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define da_append(xs, x)\
   do {\
      if (xs.count >= xs.capacity) {\
         if (xs.capacity == 0) xs.capacity = 256;\
         else xs.capacity *= 2;\
         xs.items = realloc(xs.items, xs.capacity*sizeof(*xs.items));\
      }\
      xs.items[xs.count++] = x;\
   } while(0)

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
   size_t db_index;
} Entry_Wrapper;

typedef struct {
   Entry_Wrapper *items;
   size_t count;
   size_t capacity;
} Wrappers;


void parse_flags(int argc, char *argv[])
{
   if (argc < 2) {
      fprintf(stderr, "Usage: %s [-h] [command] [<args>]\n", argv[0]);
      fprintf(stderr, "Missing command and arguments\n");
      exit(69);
   }

   if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
      printf("Usage: %s [-h] [command] [<args>]\n\n", argv[0]);
      printf("Commands:\n");
      printf("    add            Add an entry to the database\n");
      printf("    query          Query entry from the database\n");
      printf("    list           List all entries in the database\n");
      printf("Options:\n");
      printf("    -h, --help     Print help and exit.\n");
      exit(0);
   }
}

void db_add(FILE *db, char *str)
{
   Entry new;
   new.pathname = str;
   new.pathname_len = strlen(str);
   new.last_visited = time(NULL);
   new.frecency_score = 1;

   fwrite(&new.pathname_len, sizeof(size_t), 1, db);
   fwrite(new.pathname, sizeof(char), new.pathname_len, db);
   fwrite(&new.frecency_score, sizeof(double), 1, db);
   fwrite(&new.last_visited, sizeof(time_t), 1, db);
}

int match_seperator(char c)
{
   return c == '/' || c == '_' || c == '-' || c == ' ' || c == '.';
}

int get_fzscore(const char *pattern, const char *text)
{
   int score = 0;
   int pi = 0; // pattern index
   int consecutive_matches = 0;
   int first_match = -1;

   for (int ti = 0; text[ti] != '\0'; ++ti) {
      if (pattern[pi] == '\0') break; // premature exit

      if (tolower(text[ti]) == tolower(pattern[pi])) {
         if (first_match == -1) first_match = ti; // record text index of first match

         score += 10; // match
         score -= strlen(text);
         if (consecutive_matches > 0) score += 15;
         if (ti == 0 || match_seperator(text[ti - 1])) score += 20;

         consecutive_matches++;
         pi++;
      } else {
         score -= 1;
         consecutive_matches = 0;
      }
   }

   if (pattern[pi]) return -65536;  // No complete match for entire pattern
   if (first_match > 0) score -= first_match;  // Late first match

   return score;
}

double get_decayed_score(char *pattern, Entry entry, double decay) {
      double frequency = entry.frecency_score * decay;
      int match = get_fzscore(pattern, entry.pathname);

      return frequency * match;
}

char *get_data_home(void)
{
   char *path = getenv("XDG_DATA_HOME");
   if (path) return path;

   path = getenv("HOME");
   strcat(path, "/.local/share/yeezy.db");

   return path;
}

int comp_with_matching(const void *a, const void *b) {
   Entry_Wrapper *ea = (Entry_Wrapper *)a;
   Entry_Wrapper *eb = (Entry_Wrapper *)b;

   if (eb->score < ea->score) return -1;
   if (eb->score > ea->score) return 1;
   return 0;
}

int comp_freq(const void *a, const void *b) {
   Entry *ea = (Entry *)a;
   Entry *eb = (Entry *)b;

   if (eb->frecency_score < ea->frecency_score) return -1;
   if (eb->frecency_score > ea->frecency_score) return 1;
   return 0;
}


int main(int argc, char *argv[])
{
   parse_flags(argc, argv);
   const char *db_path = "./db";
   FILE *db = NULL;

   if (!strcmp(argv[1], "add")) {
      if (!argv[2]) return 1;
      db = fopen(db_path, "a+b");
      // TODO: hashmap
      db_add(db, argv[2]);
   }

   if (!strcmp(argv[1], "query")) {
      if (!argv[2]) return 1;
      db = fopen(db_path, "r+b");
      char *pattern = argv[2];
      double decay = 0.95;

      Entries arr = {0};
      Entry buf;
      while (fread(&buf.pathname_len, sizeof(size_t), 1, db)) {
         buf.pathname = malloc(sizeof(char)*buf.pathname_len);
         fread(buf.pathname, sizeof(char), buf.pathname_len, db);
         fread(&buf.frecency_score, sizeof(double), 1, db);
         fread(&buf.last_visited, sizeof(time_t), 1, db);

         da_append(arr, buf);
      }

      Wrappers filtered_arr = {0};
      for (size_t i = 0; i < arr.count; ++i) {
         if (strstr(arr.items[i].pathname, pattern)) {
            if (filtered_arr.count >= filtered_arr.capacity) {
               if (filtered_arr.capacity == 0) filtered_arr.capacity = 256;
               else filtered_arr.capacity *= 2;
               filtered_arr.items = realloc(filtered_arr.items, filtered_arr.capacity*sizeof(*filtered_arr.items));
            }
            filtered_arr.items[filtered_arr.count].db_index = i;
            filtered_arr.items[filtered_arr.count++].entry = &arr.items[i];
         }
      }

      for (size_t i = 0; i < filtered_arr.count; ++i)
         filtered_arr.items[i].score = get_decayed_score(argv[2], *filtered_arr.items[i].entry, decay);
      qsort(filtered_arr.items, filtered_arr.count, sizeof(Entry_Wrapper), comp_with_matching);

      printf("%.*s -> %f | ", (int)filtered_arr.items[0].entry->pathname_len, filtered_arr.items[0].entry->pathname, filtered_arr.items[0].score);
      printf("frecency: %f | time: %ld\n", filtered_arr.items[0].entry->frecency_score, filtered_arr.items[0].entry->last_visited);

      free(arr.items);
      free(filtered_arr.items);
   }

   if (!strcmp(argv[1], "list")) {
      db = fopen(db_path, "rb");

      Entries arr = {0};
      Entry buf;
      while (fread(&buf.pathname_len, sizeof(size_t), 1, db)) {
         buf.pathname = malloc(sizeof(char)*buf.pathname_len);
         fread(buf.pathname, sizeof(char), buf.pathname_len, db);
         fread(&buf.frecency_score, sizeof(double), 1, db);
         fread(&buf.last_visited, sizeof(time_t), 1, db);

         da_append(arr, buf);
      }

      for (size_t i = 0; i < arr.count; ++i) {
         printf("%.*s -> %zu | ", (int)arr.items[i].pathname_len, arr.items[i].pathname, arr.items[i].pathname_len);
         printf("score: %f | time: %ld\n", arr.items[i].frecency_score, arr.items[i].last_visited);
      }

      free(arr.items);
   }

   fclose(db);
   return 0;
}
