#include "yz.h"

// check for seperators in a base filename e.g. my_file.py
int match_seperator(char c) {
   return c == '/' || c == '_' || c == '-' || c == ' ' || c == '.';
}

int get_fzscore(const char *pattern, const char *text) {
   int score = 0;
   int pi = 0; // pattern index
   int consecutive_matches = 0;
   int first_match = -1;

   for (int ti = 0; text[ti] != '\0'; ++ti) {
      if (pattern[pi] == '\0') break; // premature exit

      if (tolower(text[ti]) == tolower(pattern[pi])) {
         if (first_match == -1) first_match = ti; // record text index of first match

         score += 10; // match
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
