#include "yz.h"
#include <stdlib.h>

#define BUF_SIZE 65536
#define ENTRY_SIZE 1024
char line[1024];

typedef struct {
   char entry[ENTRY_SIZE];
   int score;
   int frequency_score;
} Entry;

typedef struct {
   Entry *items;
   size_t count;
   size_t capacity;
} Entries;

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


int comp(const void *a, const void *b) {
   Entry *ea = (Entry *)a;
   Entry *eb = (Entry *)b;

   return eb->score - ea->score;
}

int main(int argc, char *argv[]) {
   if (argc < 2) {
      fprintf(stderr, "Usage: %s yomama\n", argv[0]);
      fprintf(stderr, "Missing options and arguments\n");
      exit(69);
   }

   const char *db_path = "/home/chocodeerzynx/dev/amazingporn/yeezy/db";

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

      for (size_t i = 0; i < arr.count; ++i) {
         arr.items[i].score = get_fzscore(argv[2], arr.items[i].entry);
      }
      qsort(arr.items, arr.count, sizeof(Entry), comp);
      printf("%s -> %d\n", arr.items[1].entry, arr.items[1].score);

      fclose(db);
      return 0;
   }

   if (!strcmp(argv[1], "add")) {
      if (!argv[2]) return 1;

      FILE *db = fopen(db_path, "ab");
      printf("yesmama\n");

      Entry new = {0};
      snprintf(new.entry, ENTRY_SIZE, "%s", argv[2]);
      new.frequency_score = 10;
      new.score = 0;
      fwrite(&new, sizeof(Entry), 1, db);

      fclose(db);
      return 0;
   }

   if (!strcmp(argv[1], "load")) {
      FILE *db = fopen(db_path, "rb");
      printf("loading\n");

      Entries arr = {0};
      for (int i = 0; ; ++i) {
         Entry buf;
         fread(&buf, sizeof(Entry), 1, db);
         da_append(arr, buf);
         if (feof(db)) break;
      }

      for (int i = 0; i < (int)arr.count; ++i) {
         printf("%s -> %d\n", arr.items[i].entry, arr.items[i].score);
      }

      fclose(db);
      return 0;
   }

   // FILE *db = fopen("./db", "r");
   // if (!db) fprintf(stderr, "Error: db does not exist");
   //
   // int db_count = count_lines(db);
   // rewind(db);
   //
   // char **s = malloc(sizeof(char *) * (db_count+1));
   // for (int i = 0; fgets(line, sizeof(line), db); ++i) {
   //    line[strcspn(line, "\r\n")] = '\0';
   //    s[i] = malloc(BUF_SIZE);
   //    strcpy(s[i], line);
   // }
   //
   // Rank ranks[BUF_SIZE];
   // for (int i = 0; s[i]; ++i) {
   //    ranks[i].dir = s[i];
   //    ranks[i].score = get_fzscore(argv[1], s[i]);
   // }
   //
   // qsort(ranks, db_count, sizeof(Rank), comp);
   // // for (int i = 0; s[i]; ++i)
   // // printf("%s -> %d\n", ranks[0].dir, ranks[0].score);
   // printf("%s\n", ranks[0].dir);
   //
   // for (int i = 0; i < 3; i++) free(s[i]);
   // free(s);
   // fclose(db);
   // return 0;

   // for (int i = 0; i < 5; ++i) {
   //    results[i].text = items[i];
   //    results[i].score = score_fuzzy("po", items[i]);
   // }
   //
   // qsort(results, ARR_COUNT(results), sizeof(Result), comp);
   //
   // for (int i = 0; i < 5; ++i) {
   //    printf("%s -> %d\n", results[i].text, results[i].score);
   // }
   //
}
