/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012  Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 * contact the authors at: faro@dmi.unict.it, thierry.lecroq@univ-rouen.fr
 * download the tool at: http://www.dmi.unict.it/~faro/smart/
 */

#include "algorithms.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#ifndef _WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#include "algos/include/shmids.h"
#else
// TODO https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory
#define key_t int
#define shmctl(a, b, c)
#endif

#ifdef __GNUC__
#define ATTRIBUTE_MALLOC __attribute__((malloc))
#else
#define ATTRIBUTE_MALLOC
#endif

#define SIGMA 256

#define MAX(a, b) ((a) > (b) ? (a) : (b))
// strncat helper
#define SZNCAT(x) sizeof(x) - strlen(x) - 1
#define SZNCPY(x) sizeof(x) - 1

int string2decimal(char *s) {
  int i;
  int decimal;
  int len = strlen(s);
  decimal = 0;
  for (i = 0; i < len; i++) {
    decimal = (decimal * 10) + (s[i] - '0');
  }
  return decimal;
}

ATTRIBUTE_MALLOC
char *printable(const char *s) {
  int n = strlen(s) - 1;
  unsigned sz = (n + 1) * 4;
  char *ret = calloc(sz, 1);
  while (n >= 0) {
    if (!isgraph(s[n]) || s[n] == '\'' || s[n] == ' ' || s[n] == '"') {
      char tmp[5];
      snprintf(tmp, sizeof(tmp), "\\%03o", s[n]);
      if (strlen(ret) + n + 5 > sz) {
        sz = strlen(ret) + n + 5;
        char *tmp = realloc(ret, sz);
        if (tmp)
          ret = tmp;
        else
          abort();
      }
      strncat(ret, tmp, sz);
    } else {
      char tmp[4];
      snprintf(tmp, 4, "%c", s[n]);
      strcat(ret, tmp);
    }
    n--;
  }
  return ret;
}

/* returns 1 if s is an integer number. 0 otherwise */
int isInt(char *s) {
  unsigned int i;
  for (i = 0; i < strlen(s); i++)
    if (s[i] < '0' || s[i] > '9')
      return 0;
  return 1;
}

ATTRIBUTE_MALLOC
char *str2lower(const char *s) {
  int n = strlen(s) - 1;
  char *ret = calloc(n + 1, 1);
  while (n >= 0) {
    if (s[n] >= 'A' && s[n] <= 'Z')
      ret[n] = s[n] - 'A' + 'a';
    else
      ret[n] = s[n];
    n--;
  }
  return ret;
}

ATTRIBUTE_MALLOC
char *str2upper(const char *s) {
  int n = strlen(s) - 1;
  char *ret = calloc(n + 1, 1);
  while (n >= 0) {
    if (s[n] >= 'a' && s[n] <= 'z')
      ret[n] = s[n] - 'a' + 'A';
    else
      ret[n] = s[n];
    n--;
  }
  return ret;
}

int search_ALGO(const char *ALGO_NAME[], char *algo) {
  unsigned int i;
  char *low = str2lower(algo);
  for (i = 0; i < NumAlgo; i++)
    if (ALGO_NAME[i] && !strcmp(ALGO_NAME[i], algo)) {
      free(low);
      return i;
    }
  free(low);
  return -1;
}

void getAlgo(const char *ALGO_NAME[], int EXECUTE[]) {
  DIR *d;
  struct dirent *dir;

  for (unsigned int id = 0; id < ARRAY_SIZE(ALGOS); id++) {
    EXECUTE[id] = ALGOS[id].execute;
    ALGO_NAME[id] = ALGOS[id].name;
    ALGO_DESCRIPTION[id] = ALGOS[id].desc;
  }
  // cross-check against existing algos:
  char algo[50];
  d = opendir("source/algos");
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      strcpy(algo, dir->d_name);
      int len = strlen(algo);
      if (algo[len - 1] == 'c' && algo[len - 2] == '.') {
        algo[len - 2] = '\0';
        if (algo[0] == '.' && algo[1] == '#') // skip temp. emacs buffers
          continue;
        int id = search_ALGO(ALGO_NAME, algo);
        if (id == -1) {
          printf("WARNING %s.c exists, but is missing in algorithms.h\n", algo);
          /* add the missing algos:
          FILE *f = fopen("source/algorithms.h", "a");
          fseek(f, 0, SEEK_END);
          char *upname = str2upper(algo);
          fprintf(f, "   [_%s] = {_%s, 1, \"%s\", \"\", 0, 0},\n", upname, upname, algo);
          free(upname);
          fclose(f);
          */
        } else if (ALGOS[id].missing)
          printf(
              "WARNING %s.c exists, but has a missing flag in algorithms.h\n",
              algo);
      }
    }
  }
  closedir(d);

  // set temp. EXECUTE flags
  FILE *fp = fopen("algorithms.lst", "r");
  if (!fp) {
    fp = fopen("source/algorithms.lst", "r"); // moved
    if (!fp)
      return;
  }
  char *n;
  char c;
  int execute;
  int i = 0;
  while ((c = getc(fp)) != EOF)
    if (c == '#') {
      execute = (getc(fp) - '0');
      getc(fp);
      getc(fp);
      n = (char *)malloc(sizeof(char) * 20);
      unsigned int j = 0;
      while ((c = getc(fp)) != ' ')
        n[j++] = c;
      n[j] = '\0';
      i = search_ALGO(ALGO_NAME, n);
      if (i >= 0 && i < (int)NumAlgo && !strcmp(ALGOS[i].name, n))
        EXECUTE[i] = execute;
      free(n);
    }
  fclose(fp);
}

#define SPLITCHAR ':'
int split_filelist(char *filename, char list_of_filenames[NumSetting][50]) {
  int i, j, k;
  i = j = k = 0;
  int m = strlen(filename);
  while (i < m) {
    while (i < m && filename[i] != SPLITCHAR) {
      list_of_filenames[k][j++] = filename[i];
      i++;
    }
    list_of_filenames[k][j] = '\0';
    if (i < m && filename[i] == SPLITCHAR) {
      k++;
      j = 0;
      i++;
    }
  }
  return (k + 1);
}

static int u8_cmp(const void* a, const void* b) {
  return *(uint8_t*)a < *(uint8_t*)b ? -1 :
    *(uint8_t*)a > *(uint8_t*)b ? 1 : 0;
}

// display the frequency of characters and the dimension of the alphabet
void textStats(unsigned char *T, int n, int FREQ[SIGMA]) {
  int j;
  int nalpha = 0;
  int maxcode = 0;
  int mincode = 255;
  int maxfreq = 0;
  int median = 0;
  unsigned char *sorted;
  for (j = 0; j < SIGMA; j++)
    FREQ[j] = 0;
  for (j = 0; j < n; j++) {
    unsigned char c = T[j];
    if (FREQ[c] == 0)
      nalpha++;
    FREQ[c]++;
    if (maxcode < c)
      maxcode = c;
    if (mincode > c)
      mincode = c;
  }
  for (j = 0; j < SIGMA; j++)
    if (maxfreq < FREQ[j])
      maxfreq = j;
  median = (n % 2) ? (n+1)/2 : n/2;
  sorted = malloc(n);
  memcpy(sorted, T, n);
  qsort(sorted, n, 1, u8_cmp);
  printf("\t%d characters [%d-%d], median: %d, alpha: %d, highest freq: %d\n",
         n, mincode, maxcode,
         n % 2 ? sorted[median] : (sorted[median] + sorted[(n + 2)/2]) / 2,
         nalpha, maxfreq);
  //printf("\n\tText of %d chars : %s\n", n, T);
  //printf("\tPattern of %d chars : %s, alpha = %d\n", m, simplePattern, alpha);
  free(sorted);
}

int getText(unsigned char *T, char *path, int FREQ[SIGMA], int TSIZE) {
  // obtains the input text
  int j, i = 0;
  char indexfilename[100];
  strncpy(indexfilename, path, SZNCPY(indexfilename));
  strncat(indexfilename, "/index.txt", SZNCAT(indexfilename));
  FILE *index;
  if ((index = fopen(indexfilename, "r"))) {
    char c;
    while (i < TSIZE && (c = getc(index)) != EOF) {
      if (c == '#') {
        char filename[100];
        strncpy(filename, path, SZNCPY(filename));
        j = strlen(filename);
        filename[j++] = '/';
        while ((c = getc(index)) != '#')
          filename[j++] = c;
        filename[j] = '\0';
        printf("\tLoading the file %s\n", filename);
        FILE *input;
        if ((input = fopen(filename, "r"))) {
          int d;
          while (i < TSIZE && (d = getc(input)) != EOF)
            T[i++] = d;
          fclose(input);
        } else
          printf("\tError in loading text file %s\n", filename);
      }
    }
    fclose(index);
  } else
    printf("\tError in loading text buffer. No index file exists.\n");
  T[i] = '\0';
  // compute the frequency of characters and the dimension of the alphabet
  textStats(T, i, FREQ);
  return i;
}

void setOfRandomPatterns(unsigned char **setP, int m, unsigned char *T, int n,
                         int numpatt, unsigned char *simplePattern,
                         int alpha /*ignored*/) {
  int i, j, k;
  (void)alpha;
  for (i = 0; i < numpatt; i++) {
    if (strcmp((char *)simplePattern, ""))
      //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.strcpy)
      strcpy((char *)setP[i], (char *)simplePattern);
    else {
      k = rand() % (n - m); // generates a number between 0 and n-m
      // TODO: observe alpha
      for (j = 0; j < m; j++)
        setP[i][j] = T[k + j]; // creates the pattern
      setP[i][j] = '\0';
    }
  }
}
