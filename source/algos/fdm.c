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
 * This is an implementation of the Forward DAWG Matching algorithm
 * in M. Crochemore and W. Rytter. Text algorithms. Oxford University Press,
 * (1994).
 */

#include "include/define.h"
#include "include/main.h"
#include "include/AUTOMATON.h"

static int s_ttrans[3 * M_CUTOFF * SIGMA * sizeof(int)];
static int s_tlength[3 * M_CUTOFF * sizeof(int)];
static int s_tsuffix[3 * M_CUTOFF * sizeof(int)];
static unsigned char s_tterminal[3 * M_CUTOFF * sizeof(unsigned char)];

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int j, init, ell, state, count;
  int *ttrans, *tlength, *tsuffix;
  unsigned char *tterminal;

  /* Preprocessing */
  BEGIN_PREPROCESSING
  if (m > M_CUTOFF) {
    ttrans = (int *)malloc(3 * m * SIGMA * sizeof(int));
    tlength = (int *)calloc(3 * m, sizeof(int));
    tsuffix = (int *)calloc(3 * m, sizeof(int));
    tterminal = (unsigned char *)calloc(3 * m, sizeof(unsigned char));
  } else {
    ttrans = s_ttrans;
    tlength = s_tlength;
    tsuffix = s_tsuffix;
    tterminal = s_tterminal;
  }
  memset(ttrans, -1, 3 * m * SIGMA * sizeof(int));
  buildSimpleSuffixAutomaton(x, m, ttrans, tlength, tsuffix, tterminal);
  init = 0;
  count = 0;
  ell = 0;
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  state = init;
  for (j = 0; j < n; ++j) {
    if (getTarget(state, y[j]) != UNDEFINED) {
      ++ell;
      state = getTarget(state, y[j]);
    } else {
      while (state != init && getTarget(state, y[j]) == UNDEFINED)
        state = getSuffixLink(state);
      if (getTarget(state, y[j]) != UNDEFINED) {
        ell = getLength(state) + 1;
        state = getTarget(state, y[j]);
      } else {
        ell = 0;
        state = init;
      }
    }
    if (ell == m)
      OUTPUT(j - m + 1);
  }
  END_SEARCHING
  if (m > M_CUTOFF) {
    free(tterminal);
    free(tsuffix);
    free(tlength);
    free(ttrans);
  }
  return count;
}
