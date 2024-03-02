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
 * This is an implementation of the Linear DAWG Matching algorithm
 * in L. He and B. Fang and J. Sui. The wide window string matching algorithm.
 * Theor. Comput. Sci., vol.332, n.1-3, pp.391--404, Elsevier Science Publishers
 * Ltd., Essex, UK, (2005).
 *
 * Broken for m=2, off-by-one
 * Constraints: requires m>=2
 */

#include "include/define.h"
#include "include/main.h"
#include "include/AUTOMATON.h"
#include "include/search_small.h"

static unsigned char s_xR[(M_CUTOFF + 1) * sizeof(unsigned char)];
static int s_ttrans[3 * M_CUTOFF * SIGMA * sizeof(int)];
static int s_tlength[3 * M_CUTOFF * sizeof(int)];
static int s_tsuffix[3 * M_CUTOFF * sizeof(int)];
static unsigned char s_tterminal[3 * M_CUTOFF * sizeof(unsigned char)];
static int s_ttransSMA[(M_CUTOFF + 1) * SIGMA * sizeof(int)];

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int k, R, L, r, ell, end, count;
  int *ttrans, *tlength, *tsuffix;
  int *ttransSMA;
  unsigned char *tterminal;
  unsigned char *xR;
  if (m < 2)
    return search_small(x, m, y, n);

  BEGIN_PREPROCESSING
  count = 0;
  if (m > M_CUTOFF) {
    xR = ureverse(x, m);
    ttrans = (int *)malloc(3 * m * SIGMA * sizeof(int));
    tlength = (int *)calloc(3 * m, sizeof(int));
    tsuffix = (int *)calloc(3 * m, sizeof(int));
    tterminal = (unsigned char *)calloc(3 * m, sizeof(unsigned char));
    ttransSMA = (int *)malloc((m + 1) * SIGMA * sizeof(int));
  } else {
    xR = s_xR;
    s_ureverse(xR, x, m);
    ttrans = s_ttrans;
    tlength = s_tlength;
    tsuffix = s_tsuffix;
    tterminal = s_tterminal;
    ttransSMA = s_ttransSMA;
  }
  memset(ttrans, -1, 3 * m * SIGMA * sizeof(int));
  buildSimpleSuffixAutomaton(xR, m, ttrans, tlength, tsuffix, tterminal);
  memset(ttransSMA, -1, (m + 1) * SIGMA * sizeof(int));
  preSMA(x, m, ttransSMA);
  end = n / m;
  if (n % m > 0)
    ++end;
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  for (k = 1; k < end; ++k) {
    R = L = r = ell = 0;
    int iy = k * m - 1 - ell;
    while (L != UNDEFINED && iy < n && iy >= 0) {
      L = getTarget(L, y[iy]);
      ++ell;
      iy--;
      if (L != UNDEFINED && isTerminal(L))
        R = ell;
    }
    while (R > r - k * m) {
      if (R == m)
        OUTPUT(k * m + r - m);
      ++r;
      if (r == m)
        break;
      iy = k * m - 1 + r;
      if (iy < n)
        R = getSMA(R, y[iy]);
    }
  }
  for (k = (end - 1) * m; k <= n - m; ++k) {
    for (r = 0; r < m && x[r] == y[r + k]; ++r)
      ;
    if (r >= m)
      count++;
  }
  if (m > M_CUTOFF) {
    free(ttransSMA);
    free(tterminal);
    free(tsuffix);
    free(tlength);
    free(ttrans);
    free(xR);
  }
  END_SEARCHING
  return count;
}
