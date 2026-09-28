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
 * download the tool at:  https://github.com/rurban/smart/
 *
 * Note: Flaky tests with m=32,n=64 (off-by-one count)
 *       Broken!
 * Constraints: requires n >= m + 2, and m>=2 and m<XSIZE
 * Requires 2m space after y.
 */

#define MIN_M 2
#include "include/define.h"
#include "include/main.h"
#include "include/search_small.h"
#include "include/search_large.h"
#include <assert.h>

void TVSBSpreBrBc(unsigned char *x, int m, int brBc[SIGMA][SIGMA]) {
  int a, b, i;
  for (a = 0; a < SIGMA; ++a)
    for (b = 0; b < SIGMA; ++b)
      brBc[a][b] = m + 2;
  for (a = 0; a < SIGMA; ++a)
    brBc[a][x[0]] = m + 1;
  for (i = 0; i < m - 1; ++i)
    brBc[x[i]][x[i + 1]] = m - i;
  for (a = 0; a < SIGMA; ++a)
    brBc[x[m - 1]][a] = 1;
}

int search(unsigned char *x, int m, unsigned char *_y, int n) {
  return search_safe(x, m, _y, n);
}
