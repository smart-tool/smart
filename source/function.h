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
#include <string.h>
#include <dirent.h>
#include <ctype.h>

#ifdef __GNUC__
#define ATTRIBUTE_MALLOC __attribute__ ((malloc))
#else
#define ATTRIBUTE_MALLOC
#endif

// strncat helper
#define SZNCAT(x) sizeof(x) - strlen(x) - 1

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
  unsigned sz = (n+1) * 4;
  char* ret = calloc(sz, 1);
  while (n >= 0) {
    if (!isgraph(s[n]) || s[n] == '\'' || s[n] == ' '|| s[n] == '"') {
      char tmp[5];
      snprintf(tmp, sizeof(tmp), "\\%03o", s[n]);
      if (strlen(ret) + n + 5 > sz) {
        sz = strlen(ret) + n + 5;
        ret = realloc(ret, sz);
      }
      strncat(ret, tmp, sz);
    }
    else {
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
  char* ret = calloc(n+1, 1);
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
  char* ret = calloc(n+1, 1);
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

  for (unsigned int id=0; id<ARRAY_SIZE(ALGOS); id++) {
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
        int id = search_ALGO(ALGO_NAME, algo);
        if (id == -1) {
          printf ("FAIL %s.c exists, but is missing in algorithms.h\n", algo);
          /*
          FILE *f = fopen("source/algorithms.h", "a");
          fseek(f, 0, SEEK_END);
          char *upname = str2upper(algo);
          fprintf(f, "   [_%s] = {_%s, 1, \"%s\", \"\", 0},\n", upname, upname, algo);
          free(upname);
          fclose(f);
          */
        }
        else if (ALGOS[id].missing)
          printf ("FAIL %s.c exists, but has a missing flag in algorithms.h\n", algo);
      }
    }
  }
  closedir(d);

  // set temp. EXECUTE flags
  FILE *fp = fopen("source/algorithms.lst", "r");
  if (!fp)
    return;
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

int split_filelist(char *filename, char list_of_filenames[NumSetting][50]) {
  int i, j, k;
  i = j = k = 0;
  int m = strlen(filename);
  while (i < m) {
    while (filename[i] != '-' && i < m) {
      list_of_filenames[k][j++] = filename[i];
      i++;
    }
    list_of_filenames[k][j] = '\0';
    if (filename[i] == '-') {
      k++;
      j = 0;
      i++;
    }
  }
  return (k + 1);
}
