#include "yz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARR_COUNT(arr) (int)sizeof(arr) / (int)sizeof(arr[0])
#define BUF_SIZE 65536
#define ENTRY_SIZE 1024

typedef void (*fn_ptr)(void);

char line[1024];

typedef struct {
   char entry[ENTRY_SIZE];
   int score;
} Entry;

void db_write(void) {
}

int count_lines(FILE *file) {
   char buf[BUF_SIZE];
   int count = 0;

   for (;;) {
      int res = (int)fread(buf, 1, BUF_SIZE, file);
      if (ferror(file)) return 1;

      for (int i = 0; i < res; ++i)
         if (buf[i] == '\n') count++;

      if (feof(file)) break;
   }

   return count;
}

int comp(const void *a, const void *b) {
   Entry *ra = (Entry *)a;
   Entry *rb = (Entry *)b;

   return rb->score - ra->score;
}

int main(int argc, char *argv[]) {
   if (argc < 2) {
      fprintf(stderr, "Usage: %s [-o output] <file>\n", argv[0]);
      fprintf(stderr, "Missing options and arguments\n");
      exit(69);
   }

   if (!strcmp(argv[1], "add")) {
      if (!argv[2]) return 1;

      FILE *db = fopen("./db", "ab");
      printf("yesmama\n");

      Entry new = {0};
      snprintf(new.entry, ENTRY_SIZE, "%s", argv[2]);
      new.score = 0;
      fwrite(&new, sizeof(Entry), 1, db);

      fclose(db);
      return 0;
   }

   if (!strcmp(argv[1], "write")) {
      FILE *db = fopen("./db", "wb");
      printf("yomama\n");

      Entry items = {
         .entry = "mama",
         .score = 69
      };

      fwrite(&items, sizeof(Entry), 1, db);

      fclose(db);
      return 0;
   }

   if (!strcmp(argv[1], "read")) {
      FILE *db = fopen("./db", "rb");
      printf("mymama\n");

      Entry test = {0};
      fread(&test, sizeof(Entry), 1, db);

      printf("%s -> %d\n", test.entry, test.score);

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
