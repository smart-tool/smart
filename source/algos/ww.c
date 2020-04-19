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
 * This is an implementation of the Wide Window algorithm
 * in L. He and B. Fang and J. Sui. 
 * The wide window string matching algorithm. 
 * Theor. Comput. Sci., vol.332, n.1-3, pp.391--404, Elsevier Science Publishers Ltd., Essex, UK, (2005).
 */

#include "include/define.h"
#include "include/main.h"
#include "include/AUTOMATON.h"

void preSMARev(unsigned char *x, int m, int *ttransSMA) {
   int c, i, state, target, oldTarget;

   memset(ttransSMA, 0, SIGMA*sizeof(int));
   for (state = 0, i = m-1; i >= 0; --i) {
      oldTarget = getSMA(state, x[i]);
      target = state+1;
      setSMA(state, x[i], target);
      for (c = 0; c < SIGMA; ++c)
         setSMA(target, c, getSMA(oldTarget, c));
      state = target;
   }
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int k, R, L, r, ell, end, count;
   int *ttrans, *tlength, *tsuffix;
   int *ttransSMA;
   unsigned char *tterminal;
   unsigned char *xR;
 
   /* Preprocessing */
   BEGIN_PREPROCESSING
   ttrans = (int *)malloc(3*m*SIGMA*sizeof(int));
   memset(ttrans, -1, 3*m*SIGMA*sizeof(int));
   tlength = (int *)calloc(3*m, sizeof(int));
   tsuffix = (int *)calloc(3*m, sizeof(int));
   tterminal = (char *)calloc(3*m, sizeof(char));
   buildSimpleSuffixAutomaton(x, m, ttrans, tlength, tsuffix, tterminal);
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   ttransSMA = (int *)malloc((m+1)*SIGMA*sizeof(int));
   memset(ttransSMA, -1, (m+1)*SIGMA*sizeof(int));
   preSMARev(x, m, ttransSMA);
   end = n/m;
   if (n%m > 0) ++end;
   for (k = 1; k < end; ++k) {
      R = L = r = ell = 0;
      while (R != UNDEFINED && k*m-1+r < n) {
         R = getTarget(R, y[k*m-1+r]);
         ++r;
         if (R != UNDEFINED && isTerminal(R))
            L = r;
      }
      while (L > ell) {
         if (L == m) OUTPUT(k*m-1-ell);
         ++ell;
         if (ell == m)
            break;
         L = getSMA(L, y[k*m-1-ell]);
      }
   }
   for (k = (end-1)*m; k <= n - m; ++k) {
      for (r = 0; r < m && x[r] == y[r + k]; ++r);
      if (r >= m) count++;
   }
   free(ttrans);
   free(tlength);
   free(tsuffix);
   free(tterminal);
   free(ttransSMA);
   END_SEARCHING
   return count;
}
