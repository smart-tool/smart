// fallback for algos with m>X constraints

// from ts.c
int search_small(unsigned char *x, int m, unsigned char *y, int n) {
  int s, j, i, k, h, dim, count;
  if (m < 1)
    return 0;

  BEGIN_PREPROCESSING
  END_PREPROCESSING

  BEGIN_SEARCHING
  count = 0;
  /* phase n.1*/
  s = 0;
  i = m - 1;
  k = m - 1;
  dim = 1;
  while (s <= n - m && i - dim >= 0) {
    if (x[i] != y[s + i])
      s++;
    else {
      for (j = 0; j < m && x[j] == y[s + j]; j++)
        ;
      if (j == m)
        count++;
      for (h = i - 1; h >= 0 && x[h] != x[i]; h--)
        ;
      if (dim < i - h) {
        k = i;
        dim = i - h;
      }
      s += i - h;
      i--;
    }
  }

  /* phase n.2 */
  while (s <= n - m) {
    if (x[k] != y[s + k])
      s++;
    else {
      j = 0;
      while (j < m && x[j] == y[s + j])
        j++;
      if (j == m)
        count++;
      s += dim;
    }
  }
  END_SEARCHING
  return count;
}
