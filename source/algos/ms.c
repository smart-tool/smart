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
 * This is an implementation of the Maximal Shift algorithm
 * in D. M. Sunday. A very fast substring search algorithm. Commun. ACM, vol.33, n.8, pp.132--142, (1990).
 */

#include "include/define.h"
#include "include/main.h"

typedef struct patternScanOrder {
   int loc;
   char c;
} pattern;

int minShift[XSIZE];

void computeMinShift(unsigned char *x, int m) {
   int i, j;
   for (i = 0; i < m; ++i) {
      for (j = i - 1; j >= 0; --j)
          if (x[i] == x[j])
             break;
      minShift[i] = i - j;
   }
}

void orderPattern(unsigned char *x, int m, int (*pcmp)(), pattern *pat) {
   int i;
   for (i = 0; i < m; ++i) {
      pat[i].loc = i;
      pat[i].c = x[i];
   }
   qsort(pat, m, sizeof(pattern), pcmp);
}

void preQsBc(unsigned char *x, int m, int qbc[]) {
   int i;
   for(i=0;i<SIGMA;i++)   qbc[i]=m+1;
   for(i=0;i<m;i++) qbc[x[i]]=m-i;
}

int matchShift(unsigned char *x, int m, int ploc, int lshift, pattern *pat) {
   int i, j;
   for (; lshift < m; ++lshift) {
      i = ploc;
      while (--i >= 0) {
         if ((j = (pat[i].loc - lshift)) < 0)  continue;
         if (pat[i].c != x[j]) break;
      }
      if (i < 0) break;
   }
   return(lshift);
}

void preAdaptedGs(unsigned char *x, int m, int adaptedGs[], pattern *pat) {
   int lshift, i, ploc;

   adaptedGs[0] = lshift = 1;
   for (ploc = 1; ploc <= m; ++ploc) {
      lshift = matchShift(x, m, ploc, lshift, pat);
      adaptedGs[ploc] = lshift;
   }
   for (ploc = 0; ploc <= m; ++ploc) {
      lshift = adaptedGs[ploc];
      while (lshift < m) {
         i = pat[ploc].loc - lshift;
         if (i < 0 || pat[ploc].c != x[i])
            break;
         ++lshift;
         lshift = matchShift(x, m, ploc, lshift, pat);
      }
      adaptedGs[ploc] = lshift;
   }
}


int maxShiftPcmp(pattern *pat1, pattern *pat2) {
   int dsh;
   dsh = minShift[pat2->loc] - minShift[pat1->loc];
   return(dsh ? dsh : (pat2->loc - pat1->loc));
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, qsBc[SIGMA], adaptedGs[XSIZE], count;
   pattern pat[XSIZE];

   /* Preprocessing */
   BEGIN_PREPROCESSING
   computeMinShift(x ,m);
   orderPattern(x, m, maxShiftPcmp, pat);
   preQsBc(x, m, qsBc);
   preAdaptedGs(x, m, adaptedGs, pat);
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   j = 0;
   while (j <= n - m) {
      i = 0;
      while (i < m && pat[i].c == y[j + pat[i].loc])  ++i;
      if (i >= m)
         OUTPUT(j);
      j += MAX(adaptedGs[i], qsBc[y[j + m]]);
   }
   END_SEARCHING
   return count;
}

