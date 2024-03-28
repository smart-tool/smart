// fallback for algos with m>X constraints

void preSA(unsigned char *x, int m, unsigned int S[]) {
  unsigned int j;
  int i;
  for (i = 0; i < SIGMA; ++i)
    S[i] = 0;
  for (i = 0, j = 1; i < m; ++i, j <<= 1) {
    S[x[i]] |= j;
  }
}

// from sa.c, best for small patterns, very small alphabets
int search_small(unsigned char *x, int m, unsigned char *y, int n) {
  unsigned int D;
  unsigned int S[SIGMA], F;
  int j, count;
  if (m < 1)
    return 0;

  /* Preprocessing */
  BEGIN_PREPROCESSING
  preSA(x, m, S);
  F = 1U << (unsigned)(m - 1);
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  count = 0;
  for (D = 0, j = 0; j < n; ++j) {
    D = ((D << 1) | 1) & S[y[j]];
    if (D & F)
      OUTPUT(j - m + 1);
  }
  END_SEARCHING
  return count;
}
