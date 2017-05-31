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
 * This is an implementation of the Bit Parallel^2 Wide Window algorithm
 * in D. Cantone and S. Faro and E. Giaquinta. 
 * Bit-Parallelism^2: Getting to the Next Level of Parallelism. 
 * Fun with Algorithms, Lecture Notes in Computer Science, vol.6099, pp.166--177, Springer-Verlag, Berlin, (2010).
 */

#include "include/define.h"
#include "include/main.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, k, left, l, end, count, p_len, j1, j2;
   unsigned int B[SIGMA] = {0}, C[SIGMA] = {0}, M, D, H, c;
   unsigned int suf;
 
   p_len=m;
   if (m > 16) m = 16;
    if((n/m)<2) return -1;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   M = 1;
   for (i = 0; i < m; ++i){
      /* Left to right automaton */
      B[x[i]] |= M;
      /* Right to left automaton */
      C[x[m-1-i]] |= M;
      M <<= 1;
   }
   H = 1 << (m-1);
   M = (H << 16) | H;
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   end = n/m;
   if (n%m > 0) ++end;
   for (k = 1; k < end-1; k+=2) {
      j1 = k*m-1;
      j2 = (k+1)*m-1;
      /* Left to right scanning */
      l = suf = left = 0;
      D = ~0;
      while ( D ) {
         D &= (B[y[j1+l]]<<16) | B[y[j2+l]];
         H = D & M;
         if ( H ) {
            suf |= H >> l;
            if (left == 0)
               left = m-l;
            }
            ++l;
            D <<= 1;
      }
      /* Right to left scanning */
      D = ~0;
      l = 0;
      while (D != 0 && left > l) {
         D &= (C[y[j1-l]]<<16) | C[y[j2-l]];
         H = D & M;
         if ( H ) {
            H &= suf << (m-1-l);
            if (H & 0xffff0000) {
               i = m;
               while (i < p_len && x[i] == y[j1-l+i]) i++;
               if (i == p_len) OUTPUT(j1-l);
            }
            if (H & 0x0000ffff) {
               i = m;
               while (i < p_len && x[i] == y[j2-l+i]) i++;
               if (i == p_len) OUTPUT(j2-l);
            }
         }
         D <<= 1;
         ++l;
      }
   }
   for (k = j2+1; k <= n - p_len; ++k) {
      for (i = 0; i < p_len && x[i] == y[i + k]; ++i);
      if (i >= p_len) OUTPUT(i);
   }
   END_SEARCHING
   return count;
}

