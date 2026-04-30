#include "yz.h"

void parse_flags(int argc, char *argv[]) {
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

char *get_data_home(void) {
   char *path = getenv("XDG_DATA_HOME");
   if (path) return path;

   path = getenv("HOME");
   strcat(path, "/.local/share/yeezy.db");

   return path;
}

double get_decayed_score(char *pattern, Entry entry, double decay) {
      double frequency = entry.frequency_score * decay;
      int match = get_fzscore(pattern, entry.entry);

      return frequency * match;
}

int comp_with_matching(const void *a, const void *b) {
   Scored_Entry *ea = (Scored_Entry *)a;
   Scored_Entry *eb = (Scored_Entry *)b;

   if (eb->score < ea->score) return -1;
   if (eb->score > ea->score) return 1;
   return 0;
}

int comp_freq(const void *a, const void *b) {
   Entry *ea = (Entry *)a;
   Entry *eb = (Entry *)b;

   if (eb->frequency_score < ea->frequency_score) return -1;
   if (eb->frequency_score > ea->frequency_score) return 1;
   return 0;
}

unsigned int hash(char *str) {
   unsigned int result = 0;

   for (int i = 0; str[i]; ++i) {
      result += str[i];
      result *= str[i];
   }
   return result;
}

typedef struct {
   unsigned int hash;
   Entry *ptr;
} Node;

void hm_push(Node *map, unsigned int hash_result, Entry *entry) {
   int idx = hash_result % 1024;
   map[idx].hash = hash_result;
   map[idx].ptr = entry;
}

int main(int argc, char *argv[]) {
   parse_flags(argc, argv);

   const char *db_path = get_data_home();
   FILE *db = NULL;
   int state;

   if (!strcmp(argv[1], "add")) {
      db = fopen(db_path, "a+b");
      state = 1;
   } else if (!strcmp(argv[1], "query")) {
      db = fopen(db_path, "r+b");
      state = 2;
   } else {
      db = fopen(db_path, "rb");
      state = 3;
   }

   // Cache entries from db
   Entries arr = {0};
   Entry buf;
   while (fread(&buf, sizeof(Entry), 1, db)) {
      da_append(arr, buf);
   }

   if (!strcmp(argv[1], "add")) {
      if (!argv[2]) return 1;
      printf("sherlock\n");

      Node map[1024] = {0};

      for (size_t i = 0; i < arr.count; ++i)
         hm_push(map, hash(arr.items[i].entry), &arr.items[i]);

      // lookup
      int idx = hash(argv[2]) % 1024;
      if (!map[idx].ptr) {
         printf("empty\n");
         Entry new = {0};
         snprintf(new.entry, ENTRY_SIZE, "%s", argv[2]);
         new.frequency_score = 1;
         fwrite(&new, sizeof(Entry), 1, db);
         fflush(db);
      } else printf("full\n");
   }

   // if (!strcmp(argv[1], "add")) {
   //    if (!argv[2]) return 1;
   //
   //    printf("yesmama\n");
   //
   //    Entry new = {0};
   //    snprintf(new.entry, ENTRY_SIZE, "%s", argv[2]);
   //    new.frequency_score = 1;
   //    fwrite(&new, sizeof(Entry), 1, db);
   // }

   if (!strcmp(argv[1], "list")) {
      printf("loading\n");

      qsort(arr.items, arr.count, sizeof(Entry), comp_freq);
      for (size_t i = 0; i < arr.count; ++i) {
         printf("%s -> %f\n", arr.items[i].entry, arr.items[i].frequency_score);
      }
   }

   if (!strcmp(argv[1], "query")) {
      if (!argv[2]) return 1;
      char *pattern = argv[2];
      double decay = 0.95;

      // Filter cached entries
      Scored_Entries filtered_arr = {0};
      for (size_t i = 0; i < arr.count; ++i) {
         if (strstr(arr.items[i].entry, pattern)) {
            if (filtered_arr.count >= filtered_arr.capacity) {
               if (filtered_arr.capacity == 0) filtered_arr.capacity = 256;
               else filtered_arr.capacity *= 2;
               filtered_arr.items = realloc(filtered_arr.items, filtered_arr.capacity*sizeof(*filtered_arr.items));
            }
            filtered_arr.items[filtered_arr.count].db_index = i;
            filtered_arr.items[filtered_arr.count++].name = &arr.items[i];
         }
      }

      for (size_t i = 0; i < filtered_arr.count; ++i)
         filtered_arr.items[i].score = get_decayed_score(argv[2], *filtered_arr.items[i].name, decay);
      qsort(filtered_arr.items, filtered_arr.count, sizeof(Scored_Entry), comp_with_matching);

      // filtered_arr.items[0].name->frequency_score++;
      // printf("%s -> %f | Idx: %zu\n", filtered_arr.items[0].name->entry, filtered_arr.items[0].score, filtered_arr.items[0].db_index);
      fprintf(stdout, "%s", filtered_arr.items[0].name->entry);
      filtered_arr.items[0].name->frequency_score *= decay;
      filtered_arr.items[0].name->frequency_score++;

      // Update entry
      fseek(db, filtered_arr.items[0].db_index * sizeof(Entry), SEEK_SET);
      fwrite(filtered_arr.items->name, sizeof(Entry), 1, db);
      fflush(db);
   }

   fclose(db);
   return 0;
}
