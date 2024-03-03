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
 * This is an implementation of the Ahmed-Kaykobad-Chowdhury algorithm
 * in M. Ahmed and M. Kaykobad and R. A. Chowdhury.
 * A New String Matching Algorithm. Int. J. Comput. Math., vol.80, pp.825--834,
 * (2003).
 */

#include "include/define.h"
#include "include/main.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

struct _pair1 {
  int nt, ptr;
};
typedef struct _pair1 *Pair1;

struct _pair2 {
  int loc, shift;
};
typedef struct _pair2 *Pair2;

void preBmBc_(unsigned char *x, int m, int *bmBc) {
  int i;
  for (i = 0; i < SIGMA; ++i)
    bmBc[i] = m;
  for (i = 0; i < m - 1; ++i)
    bmBc[x[i]] = m - i - 1;
}

void suffixes_(unsigned char *x, int m, int *suff) {
  int f = 0, g, i;
  suff[m - 1] = m;
  g = m - 1;
  for (i = m - 2; i >= 0; --i) {
    if (i > g && suff[i + m - 1 - f] < i - g)
      suff[i] = suff[i + m - 1 - f];
    else {
      if (i < g)
        g = i;
      f = i;
      while (g >= 0 && x[g] == x[g + m - 1 - f])
        --g;
      suff[i] = f - g;
    }
  }
}

void prePref(int m, int *suff, int *pref) {
  int i, j, k;
  k = -1;
  pref[0] = 0;
  pref[m] = m;
  for (i = 0; i < m; ++i) {
    if (suff[i] > i) {
      for (j = k; j < i; ++j)
        pref[m - j - 1] = m - k - 1;
      k = i;
    }
  }
}

int preLsp(int m, int *suff) {
  int i;
  for (i = m - 2; i >= 0; --i) {
    if (i < suff[i])
      return m - 1 - i;
  }
  return m;
}

void preBcGs_(unsigned char *x, int m, int *suff, struct _pair1 bcGsPtr[],
              struct _pair2 bcGsVal[]) {
  int i, f, k, ell, *link, *loc, *next, *shift;
  int a;
  unsigned char *c;
  int s_link[M_CUTOFF];
  int s_loc[M_CUTOFF];
  int s_next[2 * M_CUTOFF];
  int s_shift[2 * M_CUTOFF];
  unsigned char s_c[M_CUTOFF];

  if (m > M_CUTOFF) {
    link = (int *)malloc(m * sizeof(int));
    loc = (int *)malloc(m * sizeof(int));
    next = (int *)malloc(2 * m * sizeof(int));
    shift = (int *)malloc(2 * m * sizeof(int));
    c = (unsigned char *)malloc(m * sizeof(unsigned char));
  } else {
    link = s_link;
    loc = s_loc;
    next = s_next;
    shift = s_shift;
    c = s_c;
  }
  f = 0;
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(link, -1, m * sizeof(int));
  for (i = 0; i < m - 1; ++i) {
    k = suff[i];
    if ((k > 0) && (i >= k)) {
      c[f] = x[i - k];
      shift[f] = m - i - 1;
      next[f] = link[m - k];
      link[m - k] = f;
      ++f;
    }
  }
  for (a = 0; a < SIGMA; ++a) {
    bcGsPtr[a].nt = 0;
    bcGsPtr[a].ptr = -1;
  }
  f = m;
  for (i = m - 1; i > 0; --i) {
    ell = link[i];
    while (ell != -1) {
      a = c[ell];
      k = bcGsPtr[a].ptr;
      if ((k == -1) || (loc[k - m] > i)) {
        loc[f - m] = i;
        shift[f] = shift[ell];
        next[f] = k;
        bcGsPtr[a].nt = bcGsPtr[a].nt + 1;
        ;
        bcGsPtr[a].ptr = f;
        ++f;
      }
      ell = next[ell];
    }
  }
  f = 0;
  for (a = 0; a < SIGMA; ++a) {
    ell = bcGsPtr[a].ptr;
    if (ell != -1) {
      bcGsPtr[a].ptr = f;
      while (ell != -1) {
        bcGsVal[f].loc = loc[ell - m];
        bcGsVal[f].shift = shift[ell];
        ++f;
        ell = next[ell];
      }
    }
  }
  if (m > M_CUTOFF) {
    free(c);
    free(shift);
    free(next);
    free(loc);
    free(link);
  }
}

void preCLink(unsigned char *x, int m, int *clink) {
  int a, i, last[SIGMA];

  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(last, -1, SIGMA * sizeof(int));
  for (i = 0; i < m; ++i) {
    a = x[i];
    clink[i] = last[a];
    last[a] = i;
  }
}

void preSLink(unsigned char *x, int m, int *suff, int *pref, int *slink) {
  int i, f, k, ell, cptr[SIGMA], *len, *link, *loc, *next, *owner, *preLoc;
  int a;
  unsigned char *c;
  int s_len[M_CUTOFF];
  int s_link[M_CUTOFF];
  int s_loc[2 * M_CUTOFF];
  int s_next[2 * M_CUTOFF];
  int s_owner[M_CUTOFF];
  int s_preLoc[M_CUTOFF];
  unsigned char s_c[M_CUTOFF];

  if (m > M_CUTOFF) {
    len = (int *)malloc(m * sizeof(int));
    link = (int *)malloc(m * sizeof(int));
    loc = (int *)malloc(2 * m * sizeof(int));
    next = (int *)malloc(2 * m * sizeof(int));
    owner = (int *)malloc(m * sizeof(int));
    preLoc = (int *)malloc(m * sizeof(int));
    c = (unsigned char *)malloc(m * sizeof(unsigned char));
  } else {
    len = s_len;
    link = s_link;
    loc = s_loc;
    next = s_next;
    owner = s_owner;
    preLoc = s_preLoc;
    c = s_c;
  }
  ell = -1;
  for (i = 0; i < m; ++i) {
    if (suff[i] > i) {
      slink[i] = ell;
      ell = i;
    } else {
      slink[i] = m - pref[m - suff[i]] - 1;
    }
  }
  f = 0;
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(link, -1, m * sizeof(int));
  for (i = 0; i < m - 1; ++i) {
    k = suff[i];
    if ((k > 0) && (i >= k)) {
      c[f] = x[i - k];
      loc[f] = i;
      next[f] = link[k];
      link[k] = f;
      ++f;
    }
  }
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(cptr, -1, SIGMA * sizeof(int));
  f = m;
  for (i = m - 1; i > 0; --i) {
    ell = link[i];
    while (ell != -1) {
      a = c[ell];
      loc[f] = loc[ell];
      len[f - m] = suff[loc[ell]];
      next[f] = cptr[a];
      cptr[a] = f;
      ++f;
      ell = next[ell];
    }
  }
  //NOLINTNEXTLINE(clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling)
  memset(owner, -1, m * sizeof(int));
  for (a = 0; a < SIGMA; ++a) {
    ell = cptr[a];
    while (ell != -1) {
      k = len[ell - m];
      if (owner[k] == a) {
        slink[loc[ell]] = preLoc[k];
      }
      preLoc[k] = loc[ell];
      owner[k] = a;
      ell = next[ell];
    }
  }
  if (m > M_CUTOFF) {
    free(c);
    free(preLoc);
    free(owner);
    free(next);
    free(loc);
    free(link);
    free(len);
  }
}

void validateShift(int m, int *clink, int *slink, int ell, int *skip) {
  int i, j, k, p, *len, *loc, *next;
  int s_len[M_CUTOFF];
  int s_loc[M_CUTOFF];
  int s_next[M_CUTOFF];
  const int z = 2 * m;

  if (m > M_CUTOFF) {
    len = (int *)malloc(m * sizeof(int));
    loc = (int *)malloc(m * sizeof(int));
    next = (int *)malloc(m * sizeof(int));
  } else {
    len = s_len;
    loc = s_loc;
    next = s_next;
  }
  if (skip[ell] < m) {
    k = 0;
    j = ell;
    j = (j + z - 1) % z;
    k += skip[j];
    if (skip[ell] + k >= m)
      goto end;
    j = (j + z - 1) % z;
    k += skip[j];
    if (skip[ell] + k >= m)
      goto end;
    p = 0;
    while (skip[ell] + k < m) {
      j = (j + z - 1) % z;
      loc[p] = m - k - 1;
      len[p] = skip[j];
      if (skip[j] == 1)
        next[p] = clink[m - k - 1];
      else
        next[p] = slink[m - k - 1];
      p++;
      k += skip[j];
      if (skip[ell] + k >= m)
        break;
      j = (j + z - 1) % z;
      k += skip[j];
    }
    loc[p] = -1;
    while (skip[ell] < m) {
      i = 0;
      while (loc[i] >= skip[ell]) {
        while (next[i] > loc[i] - skip[ell]) {
          if (len[i] == 1)
            next[i] = clink[next[i]];
          else
            next[i] = slink[next[i]];
        }
        if (next[i] < loc[i] - skip[ell])
          break;
        ++i;
      }
      if (loc[i] < skip[ell])
        break;
      if (skip[(ell + z - 1) % z] == 1)
        skip[ell] = m - 1 - clink[m - skip[ell] - 1];
      else
        skip[ell] = m - 1 - slink[m - skip[ell] - 1];
    }
  }
 end:
  if (m > M_CUTOFF) {
    free(next);
    free(loc);
    free(len);
  }
}

int getBcGS(unsigned char a, int i, struct _pair1 bcGsPtr[], struct _pair2 bcGsVal[]) {
  int left, medium, right;

  left = bcGsPtr[a].ptr;
  if (left == -1)
    return -1;
  right = left + bcGsPtr[a].nt - 1;
  while (left <= right) {
    medium = (left + right) / 2;
    if (bcGsVal[medium].loc < i)
      left = medium + 1;
    else if (bcGsVal[medium].loc > i)
      right = medium - 1;
    else
      return bcGsVal[medium].shift;
  }
  return -1;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int i, j, k, ell, lsp, bmBc[SIGMA], *clink, *pref, *skip, *slink, *suff,
      count;
  int M;
  struct _pair1 bcGsPtr[SIGMA];
  struct _pair2 *bcGsVal;
  int s_suff[M_CUTOFF];
  int s_pref[M_CUTOFF + 1];
  int s_clink[M_CUTOFF];
  int s_slink[M_CUTOFF];
  int s_skip[2 * M_CUTOFF];
  struct _pair2 s_bcGsVal[M_CUTOFF];

  /* Preprocessing */
  BEGIN_PREPROCESSING
  if (m > M_CUTOFF) {
    suff = (int *)malloc(m * sizeof(int));
    pref = (int *)malloc((m + 1) * sizeof(int));
    clink = (int *)malloc(m * sizeof(int));
    slink = (int *)malloc(m * sizeof(int));
    skip = (int *)malloc(2 * m * sizeof(int));
    bcGsVal = (struct _pair2 *)malloc(m * sizeof(struct _pair2));
  } else {
    suff = s_suff;
    pref = s_pref;
    clink = s_clink;
    slink = s_slink;
    skip = s_skip;
    bcGsVal = (struct _pair2 *)&s_bcGsVal;
  }
  preBmBc_(x, m, bmBc);
  suffixes_(x, m, suff);
  prePref(m, suff, pref);
  lsp = preLsp(m, suff);
  preBcGs_(x, m, suff, bcGsPtr, bcGsVal);
  preCLink(x, m, clink);
  preSLink(x, m, suff, pref, slink);
  count = 0;
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  j = 0;
  ell = 0;
  skip[0] = m;
  M = 2 * m;
  while (j <= n - m) {
    //h = j;
    i = m - 1;
    k = skip[ell];
    while (i >= 0) {
      if (k == 0) {
        ell = (ell + M - 1) % M;
        i -= skip[ell];
        if (i < 0)
          break;
        ell = (ell + M - 1) % M;
        k = MIN(skip[ell], i + 1);
      }
      if (x[i] == y[i + j]) {
        --i;
        --k;
      } else
        break;
    }
    if (i < 0) {
      OUTPUT(j);
      skip[ell] = m - lsp;
      ell = (ell + 1) % M;
      skip[ell] = lsp;
    } else {
      skip[ell] = MAX(0, k - 1);
      ell = (ell + 1) % M;
      skip[ell] = m - i;
      ell = (ell + 1) % M;
      if (i == m - 1) {
        skip[ell] = bmBc[y[i + j]] - m + i + 1;
      } else {
        skip[ell] = getBcGS(y[i + j], i + 1, bcGsPtr, bcGsVal);
        if (skip[ell] == -1) {
          skip[ell] = pref[i + 1];
        }
      }
      validateShift(m, clink, slink, ell, skip);
    }
    j += skip[ell]; // shift
  }

  if (m > M_CUTOFF) {
    free(suff);
    free(pref);
    free(clink);
    free(slink);
    free(skip);
    free(bcGsVal);
  }
  END_SEARCHING
  return count;
}
