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
 * download the tool at:  https://github.com/rurban/smart/
 *
 * file test.c
 * this program is used for testing correctness of algorithms
 * test the algorithm on different conditions and
 * check the number of occurrences reported
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <time.h>
#include <ctype.h>

#include "algos/include/define.h"
#include "sets.h"

#ifndef BINDIR
#define BINDIR "bin"
#endif

//#define SIGMA 256
#define TSIZE 1048576
//#define XSIZE 4200 // maximal length of the pattern
#define YSIZE 4232

unsigned char *T, *P;
double *e_time, *pre_time;
int *count;

//NOLINTBEGIN(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)

/* the brute force algorithm used for comparing occurrences */
int bf_search(unsigned char *x, int m, unsigned char *y, int n) {
  int count, j;

  /* Searching */
  count = 0;
  for (j = 0; j <= n - m; ++j) {
    if (memcmp(x, &y[j], m) == 0)
      count++;
    //for (i = 0; i < m && x[i] == y[i + j]; ++i)
    //  ;
    //if (i >= m)
    //  count++;
  }
  return count;
}

void printManual() {
  printf("\n\tSMART UTILITY FOR TESTING STRING MATCHING ALGORITHMS\n\n");
  printf("\tusage: ./test ALGONAME [-nv][text [patlen]] | [--files pattern_file text_file]\n");
  printf("\tTest the program named \"ALGONAME\" for correctness.\n");
  printf("\tThe program \"ALGONAME\" must be located in %s/\n", BINDIR);
  printf("\tOnly programs in smart format can be tested.\n");
  printf("\t-nv non-verbose (i.e. silent)\n");
  printf("\ttext: optional data/text corpus(es) to use\n");
  printf("\tpatlen: only use this pattern length, not all.\n");
  printf("\tfiles: ./test-asan algo --files fuzz/algo/default/crashes/id:... fuzz/algo/default/data.t\n");
  printf("\n\n");
}


int hexquote(char *out, unsigned char *P, const int m) {
  // hexquote P and T
  if (!out)
    return 0;
  int pos = 0;
  int is_printable = 1;
  for (int i = 0; i < m; i++) {
    if (isalnum(P[i])) {
      out[pos++] = P[i];
    }
    else {
      unsigned char hex[5];
      to_hex(P[i], hex);
      is_printable = 0;
      memcpy(&out[pos], hex, 4);
      pos += 4;
    }
  }
  out[pos] = '\0';
  return is_printable;
}

int execute(char *algoname, unsigned char *P, int m, unsigned char *T, int n,
            int *count, int verbose) {
#ifndef HAVE_SHM
  char *command;
  // hexquote P and T
  char *cmd1 = malloc((4 * m) + 1);
  char *cmd2 = malloc((4 * n) + 1);
  if (!cmd1 || !cmd2) {
    free(cmd1);
    free(cmd2);
    return 0;
  }
  int is_printable1 = hexquote(cmd1, P, m);
  int is_printable2 = hexquote(cmd2, T, n);
  size_t sz = 20 + 8 /*the 2 numbers (max 4 digit)*/ + strlen(BINDIR) + strlen(algoname) + m + n;
  if (is_printable1 && is_printable2) {
    command = malloc(sz + 1);
    if (command)
      snprintf(command, sz, "%s/%s %s %d %s %d", BINDIR, algoname, P, m, T, n);
  }
  else if (is_printable1 && !is_printable2) {
    sz += 3 + strlen(cmd2);
    command = malloc(sz + 1);
    if (command)
      snprintf(command, sz, "%s/%s %s %d $'%s' %d", BINDIR, algoname, P, m, cmd2, n);
  }
  else if (!is_printable1 && is_printable2) {
    sz += 3 + strlen(cmd1);
    command = malloc(sz + 1);
    if (command)
      snprintf(command, sz, "%s/%s $'%s' %d %s %d", BINDIR, algoname, cmd1, m, T, n);
  }
  else {
    sz += 6 + strlen(cmd1) + strlen(cmd2);
    command = malloc(sz + 1);
    if (command)
      snprintf(command, sz, "%s/%s $'%s' %d $'%s' %d", BINDIR, algoname, cmd1, m, cmd2, n);
  }
  free(cmd1);
  free(cmd2);
  if (verbose)
    printf("%s\n", command);
#else
  char command[100];
  (void)P;
  (void)T;
  sprintf(command, "./%s/%s shared %d %d %d %d %d %d %d", BINDIR, algoname,
          shmids[shm_P].key, m, shmids[shm_T].key, n, shmids[shm_r].key,
          shmids[shm_e].key, shmids[shm_pre].key);
#endif
  // TODO fork/exec with timeout
  int res = system(command);
  if (!res) {
#ifndef HAVE_SHM
    free(command);
#endif
    return (*count);
  }
  else {
    if (verbose)
      fprintf(stderr, "%s failed => %d\n", command, res);
#ifndef HAVE_SHM
    free(command);
#endif
    return -1;
  }
}

int FREQ[SIGMA];

int attempt(int *rip, int *count, unsigned char *P, int m, unsigned char *T,
            int n, char *algoname, int verbose, int alpha /*ignored*/) {
  // printf("\b\b\b\b\b\b[%.3d%%]",(*rip)*100/18); fflush(stdout);
  char *pP = NULL;
  char *pT = NULL;
  (void)alpha;
  (*count) = 0;
  int is_printable1, is_printable2;
  if (verbose) {
    pP = printable((char *)P, m, &is_printable1);
    pT = printable((char *)T, n, &is_printable2);
#ifdef DEBUG
    if (is_printable1)
      printf("\t%d %s/%s %s %d ", *rip, BINDIR, algoname, P, m);
    else
      printf("\t%d %s/%s $'%s' %d", *rip, BINDIR, algoname, pP, m);
    if (is_printable2)
      printf("%s %d ", T, n);
    else
      printf("$'%s' %d ", pT, n);
#endif
  }
  int occur1 = bf_search(P, m, T, n);
  int occur2 = execute(algoname, P, m, T, n, count, verbose);

  if (occur2 < 0 || occur1 != occur2) {
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
    return 0; // fail
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
  return 1; //ok
}

void free_setP(unsigned char **setP, const int VOLTE) {
  for (int i = 0; i < VOLTE; i++)
    free(setP[i]);
  free(setP);
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
  int verbose = 1;
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
    verbose = 0;
  }
  char algopath[100] = BINDIR "/";
  strncat(algopath, algoname, SZNCAT(algopath));
  FILE *fp = fopen(algopath, "r");
  int id = search_ALGO(ALGO_NAME, algoname);
  if (!fp) {
    if (verbose)
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
  char *orig_T = malloc(TSIZE + 1);
  char text[100] = {0};
  if (argc > argn) { // text=%s

    if (strcmp(argv[argn], "--files") == 0 && argc > argn + 2) {
      // usage: ./test algo --files fuzz/ac/default/data.t fuzz/ac/default/crashes/id:000000,sig:11,src:000017,time:25,execs:1557,op:havoc,rep:2
      char *p_file = argv[++argn];
      char *t_file = argv[++argn];
      FILE *fp = fopen(p_file, "r");
      FILE *ft = fopen(t_file, "r");
      if (!ft || !fp) {
        fprintf(stderr, "%s or %s not found\n", t_file, p_file);
        exit(1);
      }
      fseek(ft, 0L, SEEK_END);
      n = ftell(ft);
      fseek(ft, 0L, SEEK_SET);
      if (n > TSIZE) {
        fprintf(stderr, "%s too large: %u > %u\n", t_file, (unsigned)n, (unsigned)TSIZE);
        exit(1);
      }
      if (!fread(T, n, 1, ft))
        exit(1);
      fclose(ft);
      T[n] = '\0';
      fseek(fp, 0L, SEEK_END);
      m = ftell(fp);
      fseek(fp, 0L, SEEK_SET);
      P = shmalloc(shm_P, m + 1);       // pattern
      if (!fread(P, m, 1, fp))
        exit(1);
      fclose(fp);
      if (m > n) {
        fprintf(stderr, "m %d > n %d: fixup to %d\n", m, n, n);
        m = n;
      }
      P[m] = '\0';

      count = shmalloc(shm_r, sizeof(int));         // number of occurrences
      e_time = shmalloc(shm_e, sizeof(double));     // running time
      pre_time = shmalloc(shm_pre, sizeof(double)); // preprocessing
      int rip = 0;
      if (!attempt(&rip, count, P, m, T, n, algoname, verbose, 2))
        goto free_shm1;
      free_shm(T, P, count, e_time, pre_time);
      exit(0);
    }
    strncpy(text, argv[argn], SZNCPY(text));
    if (strcmp(text, "all") == 0) {
      char list_of_filenames[NumSetting][50];
      int num_buffers = split_filelist(text, list_of_filenames);
      for (int k = 0; k < num_buffers; k++) {
        char fullpath[800];
        snprintf(fullpath, sizeof(fullpath), "data/%s", list_of_filenames[k]);
        // initialize the frequency vector
        if (!(n = getText(T, fullpath, FREQ, TSIZE, verbose))) {
          exit(1);
        }
      }
    } else {
      char fullpath[800];
      snprintf(fullpath, sizeof(fullpath), "data/%s", text);
      n = getText(T, fullpath, FREQ, TSIZE, verbose);
    }
    argn++;
    memcpy (orig_T, T, TSIZE);
  } else {
    n = YSIZE;
    for (int h = 0; h < YSIZE; h++)
      RANDCH(T[h]);
  }
  if (argc > argn) { // m=%d
    m = string2decimal(argv[argn]);
    if (m > XSIZE || m < 1) {
      fprintf(stderr, "Invalid m %d\n", m);
      goto free_shm1;
    }
  } else
    m = 0; // loop over PATT_SIZE
  unsigned long seed = time(NULL);
  //NOLINTBEGIN(clang-analyzer-security.insecureAPI.strcpy)
#ifdef DEBUG
  if (getenv("SEED"))
    sscanf(getenv("SEED"), "%lu", &seed);
  if (verbose)
    printf("seed=%lu\n", seed);
#endif
  srand(seed);
  // allocate in shared memory to be used between 2 processes
  P = shmalloc(shm_P, MAX(m + 1, XSIZE));       // pattern
  count = shmalloc(shm_r, sizeof(int));         // number of occurrences
  e_time = shmalloc(shm_e, sizeof(double));     // running time
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
        occur1 = bf_search(P,m,T,YSIZE);
        occur2 = execute(algoname,pkey,m,tkey,YSIZE,rkey,ekey,prekey,count,alpha);
        if(occur2>=0 && occur1 != occur2) {
          if(verbose) printf("\n\tERROR: test failed\n\n");
          free_shm();
          exit(1);
        }
      }
    }
  }*/
  fprintf(stderr, "%s\n", algoname);

  // fuzzer cases: NUL safety, NUL termination, loop termination
  // libc and musl require NUL-terminated input without embedded NUL bytes.
  if (strcmp(algoname, "libc") && strcmp(algoname, "musl")) {
    unsigned char fuzz_fsbndmq20_crashes_id_000000[] = {
      0x4d, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x01, 0x00,
      0x05, 0x04, 0x00, 0xfe, 0xee, 0x00, 0xff, 0x51, 0x03, 0x09, 0x89,
      0x79, 0xa0, 0x00, 0xff, 0x58, 0x04, 0x04, 0x02, 0x18, 0x08, 0x00,
      0xff, 0x59, 0x02, 0xff, 0x00, 0xa0, 0x00, 0xff, 0x58, 0x04, 0x04};
    T[YSIZE] = '\0';
    memcpy(P, fuzz_fsbndmq20_crashes_id_000000, 44);
    if (!attempt(&rip, count, P, 44, T, YSIZE, algoname, verbose, alpha))
      goto free_shm1;
  }

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
  if (!minlen || minlen < 3) {
    // 21) search for "aba" in "abababbbbb"
    strcpy((char *)P, "aba");
    strcpy((char *)T, "ababababab");
    if (!attempt(&rip, count, P, 3, T, 10, algoname, verbose, alpha))
      goto free_shm1;
  }

  // 22) dont find m=1024 in n=2048 at pos 16
  for (h = 0; h < YSIZE; h++)
    RANDCH(T[h]);
  assert(XSIZE + 16 < YSIZE);
  for (h = 0; h < 1024; h++)
    P[h] = T[h + 16];
  T[YSIZE] = P[1024] = '\0';
  if (!attempt(&rip, count, P, 1024, T, YSIZE, algoname, verbose, alpha))
    goto free_shm1;

  // 23) dont find m=1024 in n=2048
  for (h = 0; h < YSIZE; h++)
    RANDCH(T[h]);
  for (h = 0; h < 1024; h++)
    RANDCH(P[h]);
  T[YSIZE] = P[1024] = '\0';
  if (!attempt(&rip, count, P, 1024, T, YSIZE, algoname, verbose, alpha))
    goto free_shm1;

  if (*text) {
    // now do the same as smart
    const int VOLTE = 50;
    unsigned char **setP =
        (unsigned char **)malloc(sizeof(unsigned char *) * VOLTE);
    if (!setP)
      goto free_shm1;
    for (int i = 0; i < VOLTE; i++) {
      setP[i] = (unsigned char *)malloc(sizeof(unsigned char) * (XSIZE + 1));
      if (!setP[i]) {
        for (int j = 0; j < i; j++)
          free(setP[j]);
        free(setP);
        goto free_shm1;
      }
    }
    memcpy (T, orig_T, TSIZE);
    if (!m) {
      PATT_SIZE = PATT_LARGE_SIZE; // the set of pattern lengths (max 4096)
      for (int il = 0; PATT_SIZE[il] > 0; il++) {
        if (PATT_SIZE[il] <= (unsigned)n) {
          m = PATT_SIZE[il];
          if (m > XSIZE || m < 1 || m > n) {
            fprintf(stderr, "Invalid m %d\n", m);
            free_setP(setP, VOLTE);
            goto free_shm1;
          }
          setOfRandomPatterns(setP, m, T, n, VOLTE, (unsigned char *)"");
          if (verbose)
            printf("Searching for a set of %d patterns with m=%d in n=%d\n",
                   VOLTE, m, n);
          for (int k = 1; k <= VOLTE; k++) {
            int j;
            for (j = 0; j <= m; j++)
              P[j] = setP[k - 1][j];
            P[m] = '\0'; // ensure zero-termination and valid x[m]
            if (!attempt(&rip, count, P, m, T, n, algoname, verbose, alpha)) {
              free_setP(setP, VOLTE);
              goto free_shm1;
            }
          }
        }
      }
    } else {
      if (m > XSIZE || m < 1 || m > n) {
        fprintf(stderr, "Invalid m %d\n", m);
        free_setP(setP, VOLTE);
        goto free_shm1;
      }
      setOfRandomPatterns(setP, m, T, n, VOLTE, (unsigned char *)"");
      if (verbose)
        printf("Searching for a set of %d patterns with m=%d in n=%d\n", VOLTE,
               m, n);
      for (int k = 1; k <= VOLTE; k++) {
        int j;
        for (j = 0; j <= m; j++)
          P[j] = setP[k - 1][j];
        P[m] = '\0'; // ensure zero-termination and valid x[m]
        if (!attempt(&rip, count, P, m, T, n, algoname, verbose, alpha)) {
          free_setP(setP, VOLTE);
          goto free_shm1;
        }
      }
    }
    free_setP(setP, VOLTE);
  }
  //NOLINTEND(clang-analyzer-security.insecureAPI.strcpy)

  if (verbose)
    printf("%-12stested OK\n", algoname);

  // free shared memory
  free_shm(T, P, count, e_time, pre_time);
  free(orig_T);
  return 0;
free_shm1:
  free_shm(T, P, count, e_time, pre_time);
  free(orig_T);
  exit(1);
}

//NOLINTEND(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
