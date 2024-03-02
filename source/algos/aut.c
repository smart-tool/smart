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
 * This is an implementation of the string matching algorithm based on DFA
 */

#include "include/define.h"
#include "include/main.h"
#include "include/AUTOMATON.h"

static int s_ttransSMA[(M_CUTOFF + 1) * SIGMA * sizeof(int)];

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int j, state, count;
  int *ttransSMA;
  count = 0;

  /* Preprocessing */
  BEGIN_PREPROCESSING
  if (m > M_CUTOFF)
    ttransSMA = (int *)malloc((m + 1) * SIGMA * sizeof(int));
  else
    ttransSMA = s_ttransSMA;
  memset(ttransSMA, -1, (m + 1) * SIGMA * sizeof(int));
  preSMA(x, m, ttransSMA);
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  for (state = 0, j = 0; j < n; ++j) {
    state = getSMA(state, y[j]);
    if (state == m)
      OUTPUT(j - m + 1);
  }
  if (m > M_CUTOFF)
    free(ttransSMA);
  END_SEARCHING
  return count;
}
