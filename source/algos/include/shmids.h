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

#ifndef _WIN32
#include <sys/ipc.h>
#include <sys/shm.h>
#else
// TODO https://learn.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory
#define key_t int
#define shmctl(a, b, c)
#endif

#define ATTEMPT 40 // number of attempts to allocate shared memory
typedef enum {
  shm_T,
  shm_P,
  shm_e,
  shm_pre,
  shm_r,
} shmids_e;

struct shmids {
  const char *name;
  int id;
  key_t key;
  size_t size;
} shmids[5] = {
  [shm_T] = {"T", 0, 0, 0},
  [shm_P] = {"P", 0, 0, 0},
  [shm_e] = {"e", 0, 0, sizeof(double)},
  [shm_pre] = {"pre", 0, 0, sizeof(double)},
  [shm_r] = {"r", 0, 0, sizeof(int)},
};

void *shmalloc(shmids_e e, size_t size) {
#ifdef HAVE_SHM
  char *buf;
  int try = 0;
  const char *name = shmids[e].name;
  int id = shmids[e].id;
  key_t key = shmids[e].key;
  shmids[e].size = size;
  do {
    key = rand() % 1000;
    id = shmget(key, size, IPC_CREAT | 0666);
  } while (++try < ATTEMPT && id < 0);
  //fprintf(stderr, "shmalloc %s %zu, key=%d (id=%d)\n", name, size, (int)key, id);
  if (id < 0) {
    fprintf(stderr, "%s: shmget: %d\n", name, id);
    perror("shmget");
    exit(1);
  }
  shmids[e].key = key;
  shmids[e].id = id;
  if ((buf = shmat(id, NULL, 0)) == (char *)-1) {
    fprintf(stderr, "%s: shmat: %d\n", name, id);
    perror("shmat");
    exit(1);
  }
  return (void*)buf;
#else
  shmids[e].size = size;
  return malloc(size);
#endif
}

#ifdef HAVE_SHM
void *shmretrieve(shmids_e e, key_t key, size_t size) {
  void *buf;
  int id;
  if ((id = shmget(key, size, 0666)) < 0) {
    fprintf(stderr, "shmretrieve %s: key: %d size: %zu\n", shmids[e].name, (int)key, size);
    perror("shmget");
    return NULL;
  }
  /* Now we attach the segment to our data space. */
  if ((buf = shmat(id, NULL, 0)) == (unsigned char *)-1) {
    fprintf(stderr, "shmretrieve %s: key: %d, size: %zu, id: %d\n", shmids[e].name,
            (int)key, size, id);
    perror("shmat");
    return NULL;
  }
  shmids[e].size = size;
  shmids[e].key = key;
  shmids[e].id = id;
  return buf;
}
#endif

void free_shm(unsigned char *T, unsigned char *P, int *count, double *e_time,
              double *pre_time) {
#ifdef HAVE_SHM
  // T is shared between test and smart
  if (T) {
    shmdt(T);
    shmctl(shmids[shm_T].id, IPC_RMID, 0);
  }
  if (P) {
    shmdt(P);
    shmctl(shmids[shm_P].id, IPC_RMID, 0);
  }
  shmdt(count);
  shmdt(e_time);
  shmdt(pre_time);
  shmctl(shmids[shm_r].id, IPC_RMID, 0);
  shmctl(shmids[shm_e].id, IPC_RMID, 0);
  shmctl(shmids[shm_pre].id, IPC_RMID, 0);
#else
  free (T);
  free (P);
  free (count);
  free (e_time);
  free (pre_time);
#endif
}
