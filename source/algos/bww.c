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
 * This is an implementation of the Bitparallel Wide Window algorithm
 * in L. He and B. Fang and J. Sui. 
 * The wide window string matching algorithm. 
 * Theor. Comput. Sci., vol.332, n.1-3, pp.391--404, Elsevier Science Publishers Ltd., Essex, UK, (2005).
 */

#include "include/define.h"
#include "include/main.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, k, left, r, ell, end, count;
   unsigned int B[SIGMA], C[SIGMA], s, t, R, L;
   unsigned int pre, cur;
 
   if(m>30) return search_large(x,m,y,n);

   /* Preprocessing */
   BEGIN_PREPROCESSING
   /* Left to right automaton */
   memset(B, 0, SIGMA*sizeof(int));
   s = 1;
   for (i = 0; i < m; ++i){
      B[x[i]] |= s;
      s <<= 1;
   }
   s >>= 1;

   /* Right to left automaton */
   memset(C, 0, SIGMA*sizeof(int));
   t = 1;
   for (i = m-1; i >= 0; --i){
      C[x[i]] |= t;
      t <<= 1;
   }
   t >>= 1;
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   end = n/m;
   if (n%m > 0) ++end;
   for (k = 1; k < end; ++k) {
      /* Left to right scanning */
      r = pre = left = 0;
      R = ~0;
      cur = s;
      while (R != 0 && k*m-1+r < n) {
         R &= B[y[k*m-1+r]];
         ++r;
         if ((R & s) != 0) {
            pre |= cur;
            left = MAX(left,m+1-r);
         }
         R <<= 1;
         cur >>= 1;
      }
      /* Right to left scanning */
      L = ~0;
      cur = 1;
      ell = 0;
      while (L != 0 && left > ell) {
         L &= C[y[k*m-1-ell]];
         if ((L&t) != 0 && (cur&pre) != 0) OUTPUT(k*m-1-ell);
         L <<= 1;
         cur <<= 1;
         ++ell;
      }
   }
   /* Test the last portion of the text */
   for (j = (end-1)*m; j <= n - m; ++j) {
      for (i = 0; i < m && x[i] == y[i + j]; ++i);
      if (i >= m) OUTPUT(i);
   }
   END_SEARCHING
   return count;
}

/*
 * Bitparallel Wide Window algorithm designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
   int i, k, left, r, ell, end, count, p_len, first, j;
   unsigned int B[SIGMA], C[SIGMA], s, t, R, L;
   unsigned int pre, cur;
 
   p_len=m;
   m=30;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   /* Left to right automaton */
   memset(B, 0, SIGMA*sizeof(int));
   s = 1;
   for (i = 0; i < m; ++i){
      B[x[i]] |= s;
      s <<= 1;
   }
   s >>= 1;

   /* Right to left automaton */
   memset(C, 0, SIGMA*sizeof(int));
   t = 1;
   for (i = m-1; i >= 0; --i){
      C[x[i]] |= t;
      t <<= 1;
   }
   t >>= 1;
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   end = n/m;
   if (n%m > 0) ++end;
   for (k = 1; k < end; ++k) {
      /* Left to right scanning */
      r = pre = left = 0;
      R = ~0;
      cur = s;
      while (R != 0 && k*m-1+r < n) {
         R &= B[y[k*m-1+r]];
         ++r;
         if ((R & s) != 0) {
            pre |= cur;
            left = MAX(left,m+1-r);
         }
         R <<= 1;
         cur >>= 1;
      }
      /* Right to left scanning */
      L = ~0;
      cur = 1;
      ell = 0;
      while (L != 0 && left > ell) {
         L &= C[y[k*m-1-ell]];
         if ((L&t) != 0 && (cur&pre) != 0) {
            j = m; first = k*m-1-ell;
            while(j<p_len && x[j]==y[first+j]) j++;
            if(j==p_len) OUTPUT(first);
         }
         L <<= 1;
         cur <<= 1;
         ++ell;
      }
   }
   for (j = (end-1)*m; j <= n - m; ++j) {
      for (i = 0; i < m && x[i] == y[i + j]; ++i);
      if (i >= m) OUTPUT(i);
   }
   END_SEARCHING
   return count;
}




