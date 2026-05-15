#include "yz.h"

int cmd_query(FILE *db, const char *db_path, char *argv[], Entries entries)
{
   (void)db_path;

   if (!argv[2]) return 1;
   if (check_special_paths(argv)) {
      if (db) fclose(db);
      return 0;
   }

   Wrappers filtered_entries = {0};  /* Filter database entries */
   for (size_t i = 0; i < entries.count; ++i) {
      da_filter(&filtered_entries, &entries.items[i], argv[2]);
   }
   if (!filtered_entries.count) {
      fprintf(stderr, "Yeezy: no match found\n");
      return 1;
   }

   for (size_t i = 0; i < filtered_entries.count; ++i)
      filtered_entries.items[i].score = get_decayed_score(argv[2], filtered_entries.items[i].entry, time(NULL));
   qsort(filtered_entries.items, filtered_entries.count, sizeof(Entry_Wrapper), comp_by_score);

   Entry *chosen = filtered_entries.items->entry;
   puts(chosen->pathname);

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

   char *basename = get_basename(argv[2]);
   Wrappers filtered_entries = {0};  /* Filter database entries */
   for (size_t i = 0; i < entries.count; ++i) {
      da_filter(&filtered_entries, &entries.items[i], basename);
   }

   for (size_t i = 0; i < filtered_entries.count; ++i) {
      if (!strcmp(argv[2], filtered_entries.items[i].entry->pathname)) {
         filtered_entries.items[i].entry->frecency_score++;
         filtered_entries.items[i].entry->last_visited = time(NULL);

         db = fopen(db_path, "wb");
         for (size_t i = 0; i < entries.count; ++i)
            db_write(db, &entries.items[i]);

         fclose(db);
         free(filtered_entries.items);
         return 0;
      }
   }

   db = fopen(db_path, "ab");
   db_append(db, argv[2]);

   fclose(db);
   free(filtered_entries.items);
   return 0;
}

int cmd_list(FILE *db, const char *db_path, char *argv[], Entries entries)
{
   (void)db;
   (void)db_path;
   (void)argv;

   qsort(entries.items, entries.count, sizeof(Entry), comp_by_freq);

   for (size_t i = 0; i < entries.count; ++i) {
      printf("%s -> %zu | ", entries.items[i].pathname, entries.items[i].pathname_len);
      printf("score: %f | time: %ld\n", entries.items[i].frecency_score, entries.items[i].last_visited);
   }

   return 0;
}

int cmd_remove(FILE *db, const char *db_path, char *argv[], Entries entries)
{
   if (!argv[2]) return 1;

   if (strrchr(argv[2], '/')) {  /* Remove trailing backslash */
      char *bs_ptr = strrchr(argv[2], '/');
      if (*(bs_ptr+1) == '\0') {
         *bs_ptr='\0';
      }
   }

   for (size_t i = 0; i < entries.count; ++i) {
      if (!strcmp(argv[2], entries.items[i].pathname)) {
         entries.items[i].pathname = NULL;
         break;
      }
   }

   db = fopen(db_path, "wb");
   for (size_t i = 0; i < entries.count; ++i) {
      if (entries.items[i].pathname)
         db_write(db, &entries.items[i]);
   }

   fclose(db);
   return 0;
}
