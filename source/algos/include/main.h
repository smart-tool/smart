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

#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#else
//TODO https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory
#define key_t int
#define shmctl(a,b,c)
#endif
#include <sys/types.h>

#define BEGIN_PREPROCESSING                                                    \
  {                                                                            \
    timer_start(_timer);                                                       \
    start = clock();                                                           \
  }
#define BEGIN_SEARCHING                                                        \
  {                                                                            \
    timer_start(_timer);                                                       \
    start = clock();                                                           \
  }
#define END_PREPROCESSING                                                      \
  {                                                                            \
    timer_stop(_timer);                                                        \
    end = clock();                                                             \
    if (pre_time)                                                              \
      (*pre_time) = timer_elapsed(_timer) * 1000;                              \
  }
#define END_SEARCHING                                                          \
  {                                                                            \
    timer_stop(_timer);                                                        \
    end = clock();                                                             \
    if (run_time)                                                              \
      (*run_time) = timer_elapsed(_timer) * 1000;                              \
  }

/* global variables used for computing preprocessing and searching times */
double *run_time = NULL, // searching time
    *pre_time = NULL;    // preprocessing time
clock_t start, end;
TIMER *_timer;

int search(unsigned char *p, int m, unsigned char *t, int n);

int main(int argc, char *argv[]) {
  _timer = (TIMER *)malloc(sizeof(TIMER));
  unsigned char *p, *t;
  int m, n;
  if (!strcmp("shared", argv[1])) {
    if (argc < 7) {
      printf("error in input parameter\nfive parameters needed when used with "
             "shared memory\n");
      return 1;
    }
    int pshmid, tshmid, eshmid, preshmid;
    key_t pkey = atoi(argv[2]); // segment name for the pattern
    m = atoi(argv[3]);          // segment size for the pattern
    key_t tkey = atoi(argv[4]); // segment name for the text
    n = atoi(argv[5]);          // segment size for the text
    key_t ekey = atoi(argv[7]); // segment name for the running time
    key_t prekey =
        atoi(argv[8]); // segment name for the preprocessing running time
#ifndef _WIN32
    /* Locate the pattern. */
    if ((pshmid = shmget(pkey, m, 0666)) < 0) {
      perror("shmget");
      return 1;
    }
    /* Now we attach the segment to our data space. */
    if ((p = shmat(pshmid, NULL, 0)) == (unsigned char *)-1) {
      perror("shmat");
      return 1;
    }
    /* Locate the text. */
    if ((tshmid = shmget(tkey, n, 0666)) < 0) {
      perror("shmget");
      return 1;
    }
    /* Now we attach the segment to our data space. */
    if ((t = shmat(tshmid, NULL, 0)) == (unsigned char *)-1) {
      perror("shmat");
      return 1;
    }
    /* Locate the running time variable */
    if ((eshmid = shmget(ekey, 8, 0666)) < 0) {
      perror("shmget");
      return 1;
    }
    /* Now we attach the segment to our time variable space. */
    if ((run_time = shmat(eshmid, NULL, 0)) == (double *)-1) {
      perror("shmat");
      return 1;
    }
    /* Locate the preprocessing running time variable */
    if ((preshmid = shmget(prekey, 8, 0666)) < 0) {
      perror("shmget");
      return 1;
    }
    /* Now we attach the segment to our time variable space. */
    if ((pre_time = shmat(preshmid, NULL, 0)) == (double *)-1) {
      perror("shmat");
      return 1;
    }
#endif

    // timer_start(_timer);
    // start = clock();
    int count = search(p, m, t, n);
    // timer_stop(_timer);
    // end = clock();
    //(*run_time) = timer_elapsed(_timer)*1000;

    int rshmid, *result;
    key_t rkey = atoi(argv[6]); // segment name for the occurrences
                                //  Locate the int value.
#ifndef _WIN32
    if ((rshmid = shmget(rkey, 4, 0666)) < 0) {
      perror("shmget");
      return 1;
    }
    // Now we attach the segment to our data space.
    if ((result = shmat(rshmid, NULL, 0)) == (int *)-1) {
      perror("shmat");
      return 1;
    }
#endif
    *result = count;
    return 0;
  } else {
    if (argc < 5) {
      printf("error in input parameter\nfour parameters needed in standard "
             "mode\n");
      return 1;
    }
    size_t lp = strlen(argv[1]);
    size_t lt = strlen(argv[3]);
    // for SSE code
#define PAD_8(x) (((x) + 7) & ~7)
    p = (unsigned char *)calloc(PAD_8(lp + 1), 1);
    strcpy((char*)p, argv[1]);
    m = atoi(argv[2]);
    // boyer-moore requires space at the end of t. (tunbm)
    t = (unsigned char *)calloc(PAD_8(lt + m + 1), 1);
    strcpy((char*)t, argv[3]);
    n = atoi(argv[4]);
    if (m > (int)lp)
      fprintf(stderr, "Invalid 2nd arg m=%d, should be <= %u\n", m, (unsigned)lp);
    if (n > (int)lt)
      fprintf(stderr, "Invalid 4nd arg n=%d, should be <= %u\n", n, (unsigned)lt);
    int occ = search(p, m, t, n);
    free (p);
    free (t);
    printf("found %d occurrences\n", occ);
    return 0;
  }
}
