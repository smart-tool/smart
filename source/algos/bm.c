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
 * This is an implementation of the Boyer-Moore algorithm
 * in R. S. Boyer and J. S. Moore. 
 * A fast string searching algorithm. Commun. ACM, vol.20, n.10, pp.762--772, (1977).
 */

#include "include/define.h"
#include "include/mainstats.h"

void preBmBc(unsigned char *x, int m, int bmBc[]) {
   int i;
   for (i = 0; i < SIGMA; ++i)
      bmBc[i] = m;
   for (i = 0; i < m - 1; ++i)
      bmBc[x[i]] = m - i - 1;
}
 
 
void suffixes(unsigned char *x, int m, int *suff) {
   int f, g, i;
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
 
void preBmGs(unsigned char *x, int m, int bmGs[]) {
   int i, j, suff[XSIZE];
   suffixes(x, m, suff);
   for (i = 0; i < m; ++i) bmGs[i] = m;
   j = 0;
   for (i = m - 1; i >= 0; --i)
      if (suff[i] == i + 1)
         for (; j < m - 1 - i; ++j)
            if (bmGs[j] == m)
               bmGs[j] = m - 1 - i;
   for (i = 0; i <= m - 2; ++i)
      bmGs[m - 1 - suff[i]] = m - 1 - i;
}
 
 
int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, bmGs[XSIZE], bmBc[SIGMA], count;
 
   /* Preprocessing */
   BEGIN_PREPROCESSING
   preBmGs(x, m, bmGs);
   preBmBc(x, m, bmBc);
   END_PREPROCESSING
 
   /* Searching */
   BEGIN_SEARCHING
   j = 0;
   count = 0;
   while (j <= n - m) {
      for (i = m - 1; i >= 0 && x[i] == y[i + j]; --i);
      if (i < 0) {
         OUTPUT(j);
         j += bmGs[0];
      }
      else
         j += MAX(bmGs[i], bmBc[y[i + j]] - m + 1 + i);
   }
   END_SEARCHING
   return count;
}

struct searchInfo searchStats(unsigned char *x, int m, unsigned char *y, int n) {
    // bmGs is defined with XSIZE, maximum pattern length in the original code, but only m is required for Gs.
    int i, j, bmGs[XSIZE], bmBc[SIGMA];

    /* Preprocessing */
    preBmGs(x, m, bmGs);
    preBmBc(x, m, bmBc);

    /* Basic search info */
    struct searchInfo results = {0};
    initStats2(&results, n, SIGMA, sizeof(int), m, sizeof(int));

    sumTable(0, &results, bmGs, m + 1);
    sumTable(1, &results, bmBc, SIGMA);

    /* Searching */
    j = 0;
    while (j <= n - m) {
        results.mainLoopCount++;

        for (i = m - 1; i >= 0 && ++results.textBytesRead && ++results.validationBytesRead && x[i] == y[i + j]; --i);
        results.validationCount++;

        if (i < 0) {
            results.matchCount++;
            j += bmGs[0];
            results.indexLookupCount++;
            results.numShifts++;
            results.validationShifts += bmGs[0];
        }
        else {
            j += MAX(bmGs[i], bmBc[y[i + j]] - m + 1 + i);
            results.indexLookupCount += 2;
            results.textBytesRead++;
            results.numShifts++;
        }
    }
    return results;
}

struct algoValueNames getAlgoValueNames() {
    struct algoValueNames names = {0};
    setAlgoValueName(&names, 0, "GsShifts", "Sum of shifts in Gs table");
    setAlgoValueName(&names, 1, "BcShifts", "Sum of shifts in Bc table");
    return names;
}


