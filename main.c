#include "yz.h"

int parse_flags(int argc, char *argv[])
{
   if (argc < 2) {
      fprintf(stderr, "Usage: %s [-h] [command] [<args>]\n", argv[0]);
      fprintf(stderr, "Missing command and arguments\n");
      return 69;
   }

   if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
      printf("Usage: %s [-h] [command] [<args>]\n\n", argv[0]);
      printf("Commands:\n");
      printf("    add            Add an entry to the database\n");
      printf("    query          Query entry from the database\n");
      printf("    list           List all entries in the database\n");
      printf("Options:\n");
      printf("    -h, --help     Print help and exit.\n");
      return 1;
   }

   return 0;
}

int comp_score(const void *a, const void *b)
{
   Entry_Wrapper *ea = (Entry_Wrapper *)a;
   Entry_Wrapper *eb = (Entry_Wrapper *)b;

   if (eb->score < ea->score) return -1;
   if (eb->score > ea->score) return 1;
   return 0;
}

int comp_freq(const void *a, const void *b)
{
   Entry *ea = (Entry *)a;
   Entry *eb = (Entry *)b;

   if (eb->frecency_score < ea->frecency_score) return -1;
   if (eb->frecency_score > ea->frecency_score) return 1;
   return 0;
}

int check_special_paths(char *argv[])
{
   // TODO: maybe use switch cases in future
   if (!strcmp(argv[2], ".")) {
      printf("%s\n", argv[2]);
      return 1;
   }

   if (!strcmp(argv[2], "..")) {
      printf("%s\n", argv[2]);
      return 1;
   }

   if (!strcmp(argv[2], "-")) {
      printf("%s\n", getenv("OLDPWD"));
      return 1;
   }

   if (strrchr(argv[2], '/')) {  /* Check for backslash for relative path mode */
      char *bs_ptr = strrchr(argv[2], '/');

      // TODO: In future, implement cd's direct path behaviour
      if (*(bs_ptr+1) == '\0') {
         printf("%s\n", argv[2]);
         return 1;
      }
   }

   return 0;
}

int cmd_query(FILE *db, const char *db_path, char *argv[], Entries entries)
{
   if (!argv[2]) return 1;
   if (check_special_paths(argv)) {
      free(entries.items);
      if (db) fclose(db);
      return 0;
   }

   double decay = 0.95;
   Wrappers filtered_entries = {0};  /* Filter database entries */
   for (size_t i = 0; i < entries.count; ++i) {
      da_filter(&filtered_entries, &entries.items[i], argv[2]);
   }
   if (!filtered_entries.count) {
      fprintf(stderr, "Yeezy: no match found\n");
      return 1;
   }

   for (size_t i = 0; i < filtered_entries.count; ++i)
      filtered_entries.items[i].score = get_decayed_score(argv[2], filtered_entries.items[i].entry, decay);
   qsort(filtered_entries.items, filtered_entries.count, sizeof(Entry_Wrapper), comp_score);

   Entry *chosen = filtered_entries.items->entry;
   fprintf(stdout, "%s\n", chosen->pathname);
   chosen->frecency_score *= decay;
   chosen->frecency_score++;

   db = fopen(db_path, "wb");
   for (size_t i = 0; i < entries.count; ++i) {
      db_write(db, &entries.items[i]);
   }

   fclose(db);
   free(filtered_entries.items);
   return 0;
}

int cmd_add(FILE *db, const char *db_path, char *argv[], Entries entries)
{
      if (!argv[2]) return 1;

      if (strrchr(argv[2], '/')) {  /* Remove trailing backslash */
         char *bs_ptr = strrchr(argv[2], '/');
         if (*(bs_ptr+1) == '\0') {
            *bs_ptr='\0';
         }
      }

      db = fopen(db_path, "ab");
      // TODO: hashmap
      db_append(db, argv[2]);

      fclose(db);

      return 0;
}

int cmd_list(FILE *db, const char *db_path, char *argv[], Entries entries)
{
   (void)db;
   (void)db_path;
   (void)argv;

   qsort(entries.items, entries.count, sizeof(Entry), comp_freq);

   for (size_t i = 0; i < entries.count; ++i) {
      printf("%s -> %zu | ", entries.items[i].pathname, entries.items[i].pathname_len);
      printf("score: %f | time: %ld\n", entries.items[i].frecency_score, entries.items[i].last_visited);
   }

   return 0;
}

double get_decayed_score(char *pattern, Entry *entry, double decay)
{
   double frequency = entry->frecency_score * decay;
   int match = get_fzscore(pattern, entry->pathname);

   return frequency * match;
}

char *get_data_home(void)
{
   char *str = getenv("XDG_DATA_HOME");
   if (str) return str;

   str = getenv("HOME");
   strcat(str, "/.local/share/yeezy/yeezy.db");

   return str;
}

void da_filter(Wrappers *filtered_entries, Entry *entry, char *pattern)
{
   if (strstr(entry->pathname, pattern)) {
      if (filtered_entries->count >= filtered_entries->capacity) {
         if (filtered_entries->capacity == 0) filtered_entries->capacity = 256;
         else filtered_entries->capacity *= 2;
         filtered_entries->items = realloc(filtered_entries->items, filtered_entries->capacity*sizeof(*filtered_entries->items));
      }
      filtered_entries->items[filtered_entries->count++].entry = entry;
   }
}

typedef struct {
   char * fn_name;
   int (*fn_ptr)(FILE *, const char *, char **, Entries);
} Commands_arr;

Commands_arr commands[] = {
   {"query", cmd_query},
   {"add", cmd_add},
   {"list", cmd_list},
};


int main(int argc, char *argv[])
{
   int ret_int = parse_flags(argc, argv);
   if (ret_int) return ret_int;
   const char *db_path = get_data_home();
   FILE *db = fopen(db_path, "rb");

   Entries arr = {0};
   if (db) {
      db_read(db, &arr);
      fclose(db);
      db = NULL;
   }

   for (int i = 0; i < ARR_COUNT(commands); ++i) {
      if (!strcmp(argv[1], commands[i].fn_name)) {
         commands[i].fn_ptr(db, db_path, argv, arr);
      }
   }

   free(arr.items);
   if (db) fclose(db);
   return 0;
}
