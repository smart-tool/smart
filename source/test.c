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
 *
 * file test.c
 * this program is used for testing correctness of algorithms
 * test the algorithm on different conditions and
 * check the number of occurrences reported
 */

#include "sets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include <ctype.h>

#ifndef BINDIR
#define BINDIR "bin"
#endif

//#define SIGMA 256
#define TSIZE 1048576
#define XSIZE 1024
#define YSIZE 2048
#define VERBOSE strcmp(parameter, "-nv")

unsigned char *T, *P;
double *e_time, *pre_time;
int *count;

//NOLINTBEGIN(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)

/* the brute force algorithm used for comparing occurrences */
int search(unsigned char *x, int m, unsigned char *y, int n) {
  int i, count, j;

  /* Searching */
  count = 0;
  for (j = 0; j <= n - m; ++j) {
    for (i = 0; i < m && x[i] == y[i + j]; ++i)
      ;
    if (i >= m)
      count++;
  }
  return count;
}

void printManual() {
  printf("\n\tSMART UTILITY FOR TESTING STRING MATCHING ALGORITHMS\n\n");
  printf("\tusage: ./test ALGONAME [-nv][text [patlen]]\n");
  printf("\tTest the program named \"ALGONAME\" for correctness.\n");
  printf("\tThe program \"ALGONAME\" must be located in bin/\n");
  printf("\tOnly programs in smart format can be tested.\n");
  printf("\t-nv non-verbose (i.e. silent)\n");
  printf("\ttext: optional data/text corpus(es) to use\n");
  printf("\tpatlen: only use this pattern length, not all.\n");
  printf("\n\n");
}

int execute(char *algoname, unsigned char *P, int m, unsigned char *T, int n,
            int *count, int alpha) {
  char command[100];
#ifndef HAVE_SHM
  sprintf(command, "./%s/%s %s %d %s %d", BINDIR, algoname, P, m, T, n);
#else
  (void)P;
  (void)T;
  (void)alpha;
  sprintf(command, "./%s/%s shared %d %d %d %d %d %d %d", BINDIR, algoname,
          shmids[shm_P].key, m, shmids[shm_T].key, n, shmids[shm_r].key, shmids[shm_e].key,
          shmids[shm_pre].key);
  //fprintf(stderr, "%s\n", command);
#endif
  // TODO fork/exec with timeout
  // printf("%s\n",command);
  int res = system(command);
  if (!res)
    return (*count);
  else
    return -1;
}

int FREQ[SIGMA];

int attempt(int *rip, int *count, unsigned char *P, int m, unsigned char *T,
            int n, char *algoname, int verbose, int alpha /*ignored*/) {
  // printf("\b\b\b\b\b\b[%.3d%%]",(*rip)*100/18); fflush(stdout);
  (*count) = 0;
  char *pP = NULL;
  char *pT = NULL;
  if (verbose) {
    pP = printable((char *)P);
    pT = printable((char *)T);
#ifdef DEBUG
    printf("\t%d bin/%s %s %d %s %d ", *rip, algoname, pP, m, pT, n);
#endif
  }
  int occur1 = search(P, m, T, n);
  int occur2 = execute(algoname, P, m, T, n, count, alpha);

  if (occur2 >= 0 && occur1 != occur2) {
    if (verbose) {
      printf("%s\tERROR: test failed on case n.%d\n"
#ifndef DEBUG
             "\t%s/%s %s %d %s %d "
#endif
             "\tfound %d occ instead of %d\n\n",
             algoname, *rip,
#ifndef DEBUG
             BINDIR, algoname, pP, m, pT, n,
#endif
             occur2, occur1);
      free(pP);
      free(pT);
    }
    //free_shm();
    (*rip)++; //NOLINT(clang-analyzer-unix.Malloc)
    return 0;
  } else {
    if (verbose) {
#ifdef DEBUG
      printf("%d OK\n", *rip);
#endif
      free(pP);
      free(pT);
    }
  }
  (*rip)++; //NOLINT(clang-analyzer-unix.Malloc)
  return 1;
}

#ifdef PRETTY_RANDCH
#define RANDCH(c)                                                              \
  do {                                                                         \
    c = (rand() % 74) + '0';                                                   \
  } while (!isalnum(c))
#else
#define RANDCH(c) c = rand() % 256
#endif

int main(int argc, char *argv[]) {
  int n = 0, m = 0, minlen = 0;
  int verbose = 0;
  getAlgo(ALGO_NAME, EXECUTE);

  /* processing of input parameters */
  if (argc == 1) {
    printManual();
    return 0;
  }
  char algoname[50]; // algo
  strncpy(algoname, argv[1], SZNCPY(algoname));
  char parameter[100];
  int argn = 2;
  if (argc > argn && strcmp(argv[argn], "-nv") == 0) {
    strncpy(parameter, argv[argn++], SZNCPY(parameter));
    verbose = 1;
  }
  char algopath[100] = BINDIR "/";
  strncat(algopath, algoname, SZNCAT(algopath));
  FILE *fp = fopen(algopath, "r");
  int id = search_ALGO(ALGO_NAME, algoname);
  if (!fp) {
    if (VERBOSE)
      printf("\n\tERROR: unable to execute program %s\n\n", algopath);
    exit(1);
  }
  fclose(fp);
  if (id < 0) {
    printf("\n\tWARNING: unregistered algo %s\nDon't known which minlen tests "
           "to skip\n",
           algoname);
    minlen = 0;
  } else {
    minlen = ALGOS[id].minlen;
  }
  memset(FREQ, 0, SIGMA * sizeof(int));

  // allocate in shared memory
  T = shmalloc(shm_T, TSIZE + 1); // text
  char text[100] = {0};
  if (argc > argn) { // text=%s
    strncpy(text, argv[argn], SZNCPY(text));
    if (strcmp(text, "all") == 0) {
      char list_of_filenames[NumSetting][50];
      int num_buffers = split_filelist(text, list_of_filenames);
      for (int k = 0; k < num_buffers; k++) {
        char fullpath[800];
        snprintf(fullpath, sizeof(fullpath), "data/%s", list_of_filenames[k]);
        // initialize the frequency vector
        if (!(n = getText(T, fullpath, FREQ, TSIZE))) {
          exit(1);
        }
      }
    } else {
      char fullpath[800];
      snprintf(fullpath, sizeof(fullpath), "data/%s", text);
      n = getText(T, fullpath, FREQ, TSIZE);
    }
    argn++;
  }
  else
    n = YSIZE;
  if (argc > argn) // m=%d
    m = string2decimal(argv[argn]);
  else
    m = XSIZE;

  // allocate space for text in shared memory
  unsigned long seed = time(NULL);
  //NOLINTBEGIN(clang-analyzer-security.insecureAPI.strcpy)
#ifdef DEBUG
  if (getenv("SEED"))
    sscanf(getenv("SEED"), "%lu", &seed);
  if (VERBOSE)
    printf("seed=%lu\n", seed);
#endif
  srand(seed);
  // allocate in shared memory
  P = shmalloc(shm_P, MAX(m, XSIZE));  // pattern
  count = shmalloc(shm_r, sizeof(int)); // number of occurrences
  e_time = shmalloc(shm_e, sizeof(double)); // running time
  pre_time = shmalloc(shm_pre, sizeof(double)); // preprocessing

  // begin testing
  int rip = 0;
  int alpha = 2, h;
  /*
  int k, m, occur1, occur2, test = 1
  for(alpha = 2; alpha<=128; alpha*=2) {
          for(i=0; i<YSIZE; i++) T[i] = rand()%alpha;
          // compute the frequency of characters
          //for(j=0; j<SIGMA; j++) FREQ[j]=0;
          //for(j=0; j<YSIZE; j++) FREQ[T[j]]++;
          for(m = 2; m<=16; m*=2) {
                  for(j=0; j<10; j++) {
                          rip++;
                          printf("\b\b\b\b\b\b[%.3d%%]",rip*10/28);
                          fflush(stdout);
                          (*count) = 0;
                          k = j*2;
                          for(h=0; h<m; h++) P[h] = T[k+h];
                          P[m]='\0';
                          occur1 = search(P,m,T,YSIZE);
                          occur2 =
                          execute(algoname,pkey,m,tkey,YSIZE,rkey,ekey,prekey,count,alpha);
                          if(occur2>=0 && occur1 != occur2) {
                            if(VERBOSE) printf("\n\tERROR: test failed\n\n");
                            free_shm();
                            exit(1);
                          }
                  }
          }
  }*/
  fprintf(stderr, "%s\n", algoname);

  // 1) search for "a" in "aaaaaaaaaa"
  if (!minlen || minlen < 1) {
    strcpy((char *)P, "a");
    strcpy((char *)T, "aaaaaaaaaa");
    if (!attempt(&rip, count, P, 1, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 2) search for "aa" in "aaaaaaaaaa"
  if (!minlen || minlen < 2) {
    strcpy((char *)P, "aa");
    strcpy((char *)T, "aaaaaaaaaa");
    if (!attempt(&rip, count, P, 2, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 3) search for "aaaaaaaaaa" in "aaaaaaaaaa"
  strcpy((char *)P, "aaaaaaaaaa");
  strcpy((char *)T, "aaaaaaaaaa");
  if (!attempt(&rip, count, P, 10, T, 10, algoname, verbose, alpha))
    goto free_shm1;

  // 4) search for "b" in "aaaaaaaaaa"
  if (!minlen || minlen < 1) {
    strcpy((char *)P, "b");
    strcpy((char *)T, "aaaaaaaaaa");
    if (!attempt(&rip, count, P, 1, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 5) search for "ab" in "ababababab"
  if (!minlen || minlen < 2) {
    strcpy((char *)P, "ab");
    strcpy((char *)T, "ababababab");
    if (!attempt(&rip, count, P, 2, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 6) search for "a" in "ababababab"
  if (!minlen || minlen < 1) {
    strcpy((char *)P, "a");
    strcpy((char *)T, "ababababab");
    if (!attempt(&rip, count, P, 1, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 7) search for "aba" in "ababababab"
  if (!minlen || minlen < 3) {
    strcpy((char *)P, "aba");
    strcpy((char *)T, "ababababab");
    if (!attempt(&rip, count, P, 3, T, 10, algoname, verbose, alpha))
      goto free_shm1;

    // 8) search for "abc" in "ababababab"
    strcpy((char *)P, "abc");
    strcpy((char *)T, "ababababab");
    if (!attempt(&rip, count, P, 3, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 9) search for "ba" in "ababababab"
  if (!minlen || minlen < 2) {
    strcpy((char *)P, "ba");
    strcpy((char *)T, "ababababab");
    if (!attempt(&rip, count, P, 2, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 10) search for "babbbbb" in "ababababab"
  if (!minlen || minlen < 7) {
    strcpy((char *)P, "babbbbb");
    strcpy((char *)T, "ababababab");
    if (!attempt(&rip, count, P, 7, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 11) search for "bcdefg" in "bcdefghilm"
  if (!minlen || minlen < 6) {
    strcpy((char *)P, "bcdefg");
    strcpy((char *)T, "bcdefghilm");
    if (!attempt(&rip, count, P, 6, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 12) search for rand4 in rand100
  if (!minlen || minlen < 4) {
    for (h = 0; h < YSIZE; h++)
      RANDCH(T[h]);
    for (h = 0; h < 4; h++)
      P[h] = T[h];
    T[YSIZE] = P[4] = '\0';
    if (!attempt(&rip, count, P, 4, T, YSIZE, algoname, verbose, alpha))
      goto free_shm1;

    // 13) search for rand4 in rand10
    for (h = 0; h < 10; h++)
      RANDCH(T[h]);
    for (h = 0; h < 4; h++)
      P[h] = T[h];
    T[10] = P[4] = '\0';
    if (!attempt(&rip, count, P, 4, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 14) search for rand32 in rand64
  for (h = 0; h < 64; h++) {
    RANDCH(T[h]);
  }
  for (h = 0; h < 32; h++)
    P[h] = T[h];
  T[64] = P[32] = '\0';
  if (!attempt(&rip, count, P, 32, T, 64, algoname, verbose, alpha))
    goto free_shm1;

  // 15) search for same rand32 in rand64
  for (h = 0; h < 64; h++)
    RANDCH(T[h]);
  for (h = 0; h < 32; h++)
    P[h] = T[h];
  T[64] = P[32] = '\0';
  if (!attempt(&rip, count, P, 32, T, 64, algoname, verbose, alpha))
    goto free_shm1;

  // 16) search for a*32 in a*64
  for (h = 0; h < 64; h++)
    T[h] = 'a';
  for (h = 0; h < 32; h++)
    P[h] = 'a';
  T[64] = P[32] = '\0';
  if (!attempt(&rip, count, P, 32, T, 64, algoname, verbose, alpha))
    exit(1);

  // 17) search for ab*32 in ab*64
  for (h = 0; h < 64; h += 2)
    T[h] = 'a';
  for (h = 1; h < 64; h += 2)
    T[h] = 'b';
  for (h = 0; h < 32; h += 2)
    P[h] = 'a';
  for (h = 1; h < 32; h += 2)
    P[h] = 'b';
  T[64] = P[32] = '\0';
  if (!attempt(&rip, count, P, 32, T, 64, algoname, verbose, alpha))
    goto free_shm1;

  // 18) search for ab*30c in ab*64
  for (h = 0; h < 64; h += 2)
    T[h] = 'a';
  for (h = 1; h < 64; h += 2)
    T[h] = 'b';
  for (h = 0; h < 32; h += 2)
    P[h] = 'a';
  for (h = 1; h < 32; h += 2)
    P[h] = 'b';
  P[30] = 'c';
  T[64] = P[32] = '\0';
  if (!attempt(&rip, count, P, 32, T, 64, algoname, verbose, alpha))
    goto free_shm1;

  if (!minlen || minlen < 7) {
    // 19) search for "babbbbb" in "abababbbbb"
    strcpy((char *)P, "babbbbb");
    strcpy((char *)T, "abababbbbb");
    if (!attempt(&rip, count, P, 7, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  if (!minlen || minlen < 6) {
    // 20) search for "bababb" in "abababbbbb"
    strcpy((char *)P, "bababb");
    strcpy((char *)T, "abababbbbb");
    if (!attempt(&rip, count, P, 6, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }
  // 22) dont find m=1024 in n=2048 at pos 16
  for (h = 0; h < YSIZE; h++)
    RANDCH(T[h]);
  assert(XSIZE + 16 < YSIZE);
  for (h = 0; h < XSIZE; h++)
    P[h] = T[h + 16];
  T[YSIZE] = P[XSIZE] = '\0';
  if (!attempt(&rip, count, P, XSIZE, T, YSIZE, algoname, verbose, alpha))
    goto free_shm1;

  // 23) dont find m=1024 in n=2048
  for (h = 0; h < YSIZE; h++)
    RANDCH(T[h]);
  for (h = 0; h < XSIZE; h++)
    RANDCH(P[h]);
  T[YSIZE] = P[XSIZE] = '\0';
  if (!attempt(&rip, count, P, XSIZE, T, YSIZE, algoname, verbose, alpha))
    goto free_shm1;

  if (*text) {
    // now do the same as smart
    const int VOLTE = 500;
    unsigned char **setP =
      (unsigned char **)malloc(sizeof(unsigned char *) * VOLTE);
    for (int i = 0; i < VOLTE; i++)
      setP[i] = (unsigned char *)malloc(sizeof(unsigned char) * (XSIZE + 1));
    // for all m or just XSIZE?
    setOfRandomPatterns(setP, m, T, n, VOLTE, (unsigned char *)"", alpha);
    if (VERBOSE)
      printf("Searching for a set of %u patterns with m=%d in n=%d\n", VOLTE, m, n);
    for (int k = 1; k <= VOLTE; k++) {
      int j;
      for (j = 0; j <= m; j++)
        P[j] = setP[k - 1][j];
      P[j] = '\0'; // creates the pattern
      if (!attempt(&rip, count, P, m, T, n, algoname, verbose, alpha))
        goto free_shm1;
    }
    for (int i = 0; i < VOLTE; i++)
      free(setP[i]);
    free(setP);
  }
  //NOLINTEND(clang-analyzer-security.insecureAPI.strcpy)

  if (VERBOSE)
    printf("%-12stested OK\n", algoname);

  // free shared memory
  free_shm(T, P, count, e_time, pre_time);
  return 0;
 free_shm1:
  free_shm(T, P, count, e_time, pre_time);
  exit(1);
}

//NOLINTEND(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
