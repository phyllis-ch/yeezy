#include "yz.h"

void db_write(FILE *db, Entry *entry)
{
   fwrite(&entry->pathname_len, sizeof(size_t), 1, db);
   fwrite(entry->pathname, sizeof(char), entry->pathname_len, db);
   fwrite(&entry->frecency_score, sizeof(double), 1, db);
   fwrite(&entry->last_visited, sizeof(time_t), 1, db);
}

void db_read(FILE *db, Entries *entries)
{
   Entry buf;
   while (fread(&buf.pathname_len, sizeof(size_t), 1, db)) {
      buf.pathname = malloc(buf.pathname_len + 1);
      fread(buf.pathname, sizeof(char), buf.pathname_len, db);
      fread(&buf.frecency_score, sizeof(double), 1, db);
      fread(&buf.last_visited, sizeof(time_t), 1, db);
      buf.pathname[buf.pathname_len] = '\0';

      if (entries->count >= entries->capacity) {
         if (entries->capacity == 0) entries->capacity = 256;
         else entries->capacity *= 2;
         entries->items = realloc(entries->items, entries->capacity*sizeof(*entries->items));
      }
      entries->items[entries->count++] = buf;
   }
}

void db_add(FILE *db, char *str)
{
   Entry new;
   new.pathname = str;
   new.pathname_len = strlen(str);
   new.last_visited = time(NULL);
   new.frecency_score = 1;

   db_write(db, &new);
}
