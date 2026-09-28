__AFL_FUZZ_INIT();
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* the brute force algorithm used for comparing occurrences */
static inline int bf_search(unsigned char *x, int m, unsigned char *y, int n) {
  int count = 0;
  for (int j = 0; j <= n - m; ++j) {
    if (memcmp(x, &y[j], m) == 0)
      count++;
  }
  return count;
}

// avoid shell-escapes
static unsigned char printable(unsigned char c) {
  if (c <= 10)
    c += '0';
  else {
    c += '@';
    if (c > 'Z' && c < 'a')
      c += 6;
    if (c >= '{' && c <= 127)
      c += 5;
  }
  do {
    c = (rand() % 74) + '0';
  } while (!isalnum(c));
  return c;
}

#pragma clang optimize off
#pragma GCC optimize("O0")
int main(void) {
  int m, n;
  unsigned char *p = NULL, *t = NULL;
  char procname[80];
  char fn[80];
  FILE *f;

  srand(time(NULL));
  __AFL_INIT();
  p = __AFL_FUZZ_TESTCASE_BUF;
  m = __AFL_FUZZ_TESTCASE_LEN;
  n = (rand() % 120) + 32;
  t = malloc(n + 1);
  for (int i = 0; i < n; i++) {
    if (!(rand() % 2) && (i < m))
      t[i] = p[i];
    else {
      t[i] = printable(rand() % 16);
    }
  }
  t[n] = '\0';

  // TODO dump only on crash, with the crash id
  snprintf(fn, sizeof(fn), "/proc/%d/comm", getpid());
  f = fopen(fn, "r");
  fgets(procname, sizeof(procname), f);
  fclose(f);
  procname[strcspn(procname, "\n")] = 0;
  snprintf(fn, sizeof(fn), "fuzz/%s/default/data.t", procname);
  f = fopen(fn, "w");
  fwrite(t, n, 1, f);
  fclose(f);

  const int min_m =
#if MIN_M
    MIN_M
#else
    2
#endif
    ;
  const int max_m =
#if MAX_M
    MAX_M
#else
    63
#endif
    ;

  while (__AFL_LOOP(10000)) {
    if (m > n)
      m = n;
    if (m >= min_m && m <= max_m) {
      int ret = bf_search(p, m, t, n);
      int occ = search(p, m, t, n);
      assert(ret == occ);
    }
  }
  free(t);
  return 0;
}
