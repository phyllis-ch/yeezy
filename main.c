#include "yz.h"

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

// int comp(const void *a, const void *b) {
//    Entry *ea = (Entry *)a;
//    Entry *eb = (Entry *)b;
//
//    double score_a = get_decayed_score(char *pattern, Entry entry, time_t now)
//
//    return eb->score - ea->score;
// }

int main(int argc, char *argv[]) {
   if (argc < 2) {
      fprintf(stderr, "Usage: %s yomama\n", argv[0]);
      fprintf(stderr, "Missing options and arguments\n");
      exit(69);
   }

   const char *db_path = get_data_home();
   const time_t time_now = time(NULL);

   if (!strcmp(argv[1], "query")) {
      if (!argv[2]) return 1;
      char *pattern = argv[2];

      FILE *db = fopen(db_path, "rb");
      printf("query\n");

      Entries arr = {0};
      for (int i = 0; ; ++i) {
         Entry buf;
         fread(&buf, sizeof(Entry), 1, db);
         if (strstr(buf.entry, pattern)) {
            da_append(arr, buf);
         }
         if (feof(db)) break;
      }

      // for (size_t i = 0; i < arr.count; ++i) {
      //    arr.items[i].score = get_fzscore(argv[2], arr.items[i].entry);
      // }
      // qsort(arr.items, arr.count, sizeof(Entry), comp);
      // printf("%s -> %d\n", arr.items[1].entry, arr.items[1].score);

      fclose(db);
      return 0;
   }

   if (!strcmp(argv[1], "add")) {
      if (!argv[2]) return 1;

      FILE *db = fopen(db_path, "ab");
      printf("yesmama\n");

      Entry new = {0};
      snprintf(new.entry, ENTRY_SIZE, "%s", argv[2]);
      new.frequency_score = 1;
      new.last_visited = time(NULL);
      fwrite(&new, sizeof(Entry), 1, db);

      fclose(db);
      return 0;
   }

   if (!strcmp(argv[1], "list")) {
      FILE *db = fopen(db_path, "rb");
      printf("loading\n");

      Entries arr = {0};
      Entry buf;
      while (fread(&buf, sizeof(Entry), 1, db)) {
         da_append(arr, buf);
      }

      for (size_t i = 0; i < arr.count; ++i) {
         printf("%s -> %d : %ld\n", arr.items[0].entry, arr.items[0].frequency_score, arr.items[0].last_visited);
      }

      fclose(db);
      return 0;
   }

   if (!strcmp(argv[1], "mama")) {
      if (!argv[2]) return 1;
      char *pattern = argv[2];

      FILE *db = fopen(db_path, "rb");
      printf("mathy\n");

      Entries arr = {0};
      Entry buf;
      while (fread(&buf, sizeof(Entry), 1, db)) {
         if (strstr(buf.entry, pattern))
            da_append(arr, buf);
      }

      printf("%f\n", get_decayed_score(argv[2], arr.items[0], time_now));

      for (size_t i = 0; i < arr.count; ++i) {
         printf("%s -> %d : %ld\n", arr.items[0].entry, arr.items[0].frequency_score, arr.items[0].last_visited);
      }

      fclose(db);
      return 0;
   }

   return 0;
}
