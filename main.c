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

double get_decayed_score(char *pattern, Entry entry, time_t now) {
      double lambda = 8.02e-6;

      double diff_time = difftime(now, entry.last_visited);
      double decay = exp(-lambda * diff_time);

      double frequency = entry.frequency_score * decay;
      int match = get_fzscore(pattern, entry.entry);

      return frequency * match;
}

int comp(const void *a, const void *b) {
   Scored_Entry *ea = (Scored_Entry *)a;
   Scored_Entry *eb = (Scored_Entry *)b;

   if (eb->score < ea->score) return -1;
   if (eb->score > ea->score) return 1;
   return 0;
}

int main(int argc, char *argv[]) {
   parse_flags(argc, argv);

   const char *db_path = get_data_home();
   const time_t time_now = time(NULL);
   FILE *db = NULL;
   int state;

   if (!strcmp(argv[1], "add")) {
      db = fopen(db_path, "ab");
      state = 1;
   }
   else {
      db = fopen(db_path, "rb");
      state = 2;
   }

   // if (!strcmp(argv[1], "query")) {
   //    if (!argv[2]) return 1;
   //    char *pattern = argv[2];
   //
   //    FILE *db = fopen(db_path, "rb");
   //    printf("query\n");
   //
   //    Entries arr = {0};
   //    for (int i = 0; ; ++i) {
   //       Entry buf;
   //       fread(&buf, sizeof(Entry), 1, db);
   //       if (strstr(buf.entry, pattern)) {
   //          da_append(arr, buf);
   //       }
   //       if (feof(db)) break;
   //    }
   //
   //    // for (size_t i = 0; i < arr.count; ++i) {
   //    //    arr.items[i].score = get_fzscore(argv[2], arr.items[i].entry);
   //    // }
   //    // qsort(arr.items, arr.count, sizeof(Entry), comp);
   //    // printf("%s -> %d\n", arr.items[1].entry, arr.items[1].score);
   //
   //    fclose(db);
   //    return 0;
   // }

   if (!strcmp(argv[1], "add")) {
      if (!argv[2]) return 1;

      printf("yesmama\n");

      Entry new = {0};
      snprintf(new.entry, ENTRY_SIZE, "%s", argv[2]);
      new.frequency_score = 1;
      new.last_visited = time(NULL);
      fwrite(&new, sizeof(Entry), 1, db);
   }

   if (!strcmp(argv[1], "list")) {
      printf("loading\n");

      Entries arr = {0};
      Entry buf;
      while (fread(&buf, sizeof(Entry), 1, db)) {
         da_append(arr, buf);
      }

      for (size_t i = 0; i < arr.count; ++i) {
         printf("%s -> %d : %ld\n", arr.items[i].entry, arr.items[i].frequency_score, arr.items[i].last_visited);
      }
   }

   if (!strcmp(argv[1], "query")) {
      if (!argv[2]) return 1;
      char *pattern = argv[2];

      // Cache entries from db
      Entries arr = {0};
      Entry buf;
      while (fread(&buf, sizeof(Entry), 1, db)) {
            da_append(arr, buf);
      }

      // Filter cached entries
      Scored_Entries filtered_arr = {0};
      for (size_t i = 0; i < arr.count; ++i) {
         if (strstr(arr.items[i].entry, pattern)) {
            if (filtered_arr.count >= filtered_arr.capacity) {
               if (filtered_arr.capacity == 0) filtered_arr.capacity = 256;
               else filtered_arr.capacity *= 2;
               filtered_arr.items = realloc(filtered_arr.items, filtered_arr.capacity*sizeof(*filtered_arr.items));
            }
            filtered_arr.items[filtered_arr.count++].name = &arr.items[i];
         }
      }

      for (size_t i = 0; i < filtered_arr.count; ++i)
         filtered_arr.items[i].score = get_decayed_score(argv[2], *filtered_arr.items[i].name, time_now);
      qsort(filtered_arr.items, filtered_arr.count, sizeof(Scored_Entry), comp);

      // filtered_arr.items[0].name->frequency_score++;
      filtered_arr.items[0].name->last_visited = time_now;
      // printf("%s -> %f : %ld\n", filtered_arr.items[0].name->entry, filtered_arr.items[0].score, filtered_arr.items[0].name->last_visited);
      fprintf(stdout, "%s", filtered_arr.items[0].name->entry);
   }

   fclose(db);
   return 0;
}
