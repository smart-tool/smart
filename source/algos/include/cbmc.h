#include <assert.h>
#ifndef MIN_M
#define MIN_M 0
#endif
#ifndef MAX_M
#define MAX_M 8
#endif
#ifndef MAX_N
#define MAX_N 12
#endif
#undef XSIZE
#undef YSIZE
#define XSIZE MAX_M
#define YSIZE MAX_N

#ifndef ESBMC
#define __VERIFIER_assume(x)    __CPROVER_assume(x)
#define __VERIFIER_assert(x)    __CPROVER_assert(x)
#define __VERIFIER_input(a,b)   __CPROVER_input(a,b)
#define __VERIFIER_output(a,b)  __CPROVER_output(a,b)
#define __VERIFIER_nondet_uchar nondet_uchar
#define __VERIFIER_nondet_int   nondet_int
#else
void __VERIFIER_assert(int cond);
#define __VERIFIER_input(a,b)
#define __VERIFIER_output(a,b)
#endif
// not needed, and would need cbmc > 5.12
#define __CPROVER_loop_invariant(cond)

/* the brute force algorithm used for comparing occurrences */
static inline int bf_search(unsigned char *x, int m, unsigned char *y, int n) {
  assert(m < MAX_M);
  assert(n < MAX_N);
  int count = 0;
  for (int j = 0; j <= n - m; ++j)
    __CPROVER_loop_invariant(j >= __VERIFIER_loop_entry(j))
  {
    if (memcmp(x, &y[j], m) == 0)
      count++;
  }
  return count;
}

/* for MAX_N > 12 */
static inline int bf_search_large(unsigned char *x, int m, unsigned char *y, int n) {
  int count = 0;
  for (int j = 0; j <= n - m; ++j)
    __CPROVER_loop_invariant(j <= n - m)
    __CPROVER_loop_invariant(j >= __VERIFIER_loop_entry(j))
  {
    if (memcmp(x, &y[j], m) == 0)
      count++;
    //for (i = 0; i < m && x[i] == y[i + j]; ++i)
    //  ;
    //if (i >= m)
    //  count++;
  }
  return count;
}

unsigned char __VERIFIER_nondet_uchar();
int __VERIFIER_nondet_int();

int main(void) {
#define RANDCH(c)                                                              \
  {                                                                            \
    c = __VERIFIER_nondet_uchar();                                             \
    __VERIFIER_assume(c > 0 && c <= 255);                                      \
  }
  int m = MAX_M; // __VERIFIER_nondet_int();
#ifdef MIN_M
  __VERIFIER_assume(m > MIN_M && m < MAX_M);
#else
  __VERIFIER_assume(m > 0 && m < MAX_M);
#endif
  int n = MAX_N; // __VERIFIER_nondet_int();
  unsigned char P[65]; // the largest MAX_M is 64 and 512
  unsigned char T[256];
  for (int i = 0; i < MAX_M; i++)
    __CPROVER_loop_invariant(i < MAX_M)
    RANDCH(P[i]);
  for (int i = 0; i < MAX_N; i++)
    __CPROVER_loop_invariant(i < MAX_N)
    RANDCH(T[i]);
  __VERIFIER_input("P", P);
  //__VERIFIER_input("m", m);
  __VERIFIER_input("T", T);
  //__VERIFIER_input("n", n);
  int occ, ref;

#define M_N_LOOP(m, n)                          \
  T[n] = '\0';                                  \
  P[m] = '\0';                                  \
  occ = search(P, m, T, n);                     \
  __VERIFIER_output("occ", occ);                \
  ref = bf_search(P, m, T, n);                  \
  __VERIFIER_output("ref", ref);                \
  assert(ref == occ)

#define M_N_LOOP_LARGE(m, n)                    \
  T[n] = '\0';                                  \
  P[m] = '\0';                                  \
  occ = search(P, m, T, n);                     \
  __VERIFIER_output("occ", occ);                \
  ref = bf_search_large(P, m, T, n);            \
  __VERIFIER_output("ref", ref);                \
  assert(ref == occ)

#if MIN_M == 0
  M_N_LOOP(0, MAX_N-1);
#endif
#if MIN_M >= 1 || MIN_M == 0
  M_N_LOOP(1, MAX_N-1);
#endif  
#if MIN_M >= 2 || MIN_M == 0
  M_N_LOOP(2, MAX_N-1);
  M_N_LOOP(2, 2);
  M_N_LOOP(2, 3);
  M_N_LOOP(2, 4);
  M_N_LOOP(2, 5);
  M_N_LOOP(2, 6);
  M_N_LOOP(2, 7);
  M_N_LOOP(2, 8);
  M_N_LOOP(2, 9);
  M_N_LOOP(2, 10);
#endif
#if MIN_M >= 3 || MIN_M == 0
  M_N_LOOP(3, MAX_N-1);
  M_N_LOOP(3, 3);
#endif
#if MIN_M >= 4 || MIN_M == 0
  M_N_LOOP(4, MAX_N-1);
  M_N_LOOP(4, 8);
  M_N_LOOP(4, 5);
  M_N_LOOP(4, 4);
#endif
#if MIN_M >= 5 || MIN_M == 0
  M_N_LOOP(5, MAX_N-1);
  M_N_LOOP(5, 10);
  M_N_LOOP(5, 9);
  M_N_LOOP(5, 8);
  M_N_LOOP(5, 7);
  M_N_LOOP(5, 6);
  M_N_LOOP(5, 5);
#endif
#if MIN_M >= 6 || MIN_M == 0
  M_N_LOOP(6, MAX_N-1);
  M_N_LOOP(6, 10);
  M_N_LOOP(6, 8);
  M_N_LOOP(6, 7);
  M_N_LOOP(6, 6);
#endif
#if (MIN_M <= MAX_M || MIN_M == 0) && (MAX_M <= 64)
  M_N_LOOP(MAX_M-1, MAX_N-1);
#endif
#if MIN_M >= 7 || MIN_M == 0
  M_N_LOOP(7, 7);
  M_N_LOOP(7, 8);
  M_N_LOOP(7, MAX_N-2);
#endif

  strcpy((char*)T, "8TOLWd903QRXd4QMPL665XT8dPY0b214VPQMNZNRK0::4bW");
#if MIN_M >= 2 || MIN_M == 0
  M_N_LOOP_LARGE(2, 47);
  M_N_LOOP_LARGE(2, 32);
#endif
#if MIN_M >= 4 || MIN_M == 0
  M_N_LOOP_LARGE(4, 47);
  M_N_LOOP_LARGE(4, 32);
#endif
#if MIN_M >= 8 || MIN_M == 0
  M_N_LOOP_LARGE(8, 47);
  M_N_LOOP_LARGE(8, 32);
#endif
#if MIN_M >= 20 || MIN_M == 0
  M_N_LOOP_LARGE(20, 47);
  M_N_LOOP_LARGE(20, 32);
#endif
}

//#define NDEBUG
// but we must not use the search asserts to guide our verification
//#undef assert
//#define assert(x)
