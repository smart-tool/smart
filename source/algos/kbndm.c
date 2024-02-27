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
 * This is an implementation of the Factorized BNDM algorithm
 * in D. Cantone and S. Faro and E. Giaquinta. 
 * A Compact Representation of Nondeterministic (Suffix) Automata for the Bit-Parallel Approach. 
 * Combinatorial Pattern Matching, Lecture Notes in Computer Science, vol.6129, pp.288--298, Springer-Verlag, Berlin, (2010).
 */

#include "include/define.h"
#include "include/main.h"

#define CHAR_BIT 8
#define WORD_TYPE unsigned int
#define WORD_BITS (sizeof(WORD_TYPE)*CHAR_BIT)

int search(unsigned char *x, int m, unsigned char *y, int n)
{
   int i, j, k, m1;
   int beg, end;
   WORD_TYPE M;
   WORD_TYPE B[SIGMA][SIGMA] = {{0}};
   WORD_TYPE L[SIGMA] = {0};
   unsigned count = 0;
   if(m<2) return -1;
    
   BEGIN_PREPROCESSING
   end = 0;
   for (k = 0; k < WORD_BITS-1; k++) {
      char occ[SIGMA] = {0};
      while (end < m && occ[x[end]] == 0) {
         occ[x[end]] = 1;
         end++;
      }
   }
   m1 = end;
   end = k = 0;
   beg = 0;
   for (;;) {
      char occ[SIGMA] = {0};
      while (end < m1 && occ[x[m1-1-end]] == 0) {
         occ[x[m1-1-end]] = 1;
         end++;
      }
      for (i = beg+1; i < end; i++)
         B[x[m1-i]][x[m1-1-i]] |= 1L << k;
      if (end < m1) {
         B[x[m1-end]][x[m1-1-end]] |= 1L << k;
         L[x[m1-end]] |= 1L << k;
      } else {
         M = 1L << k;
         if (end > beg+1) {
            L[x[m1-end+1]] |= 1L << k;
            M <<= 1;
         }
         break;
      }
      beg = end;
      k++;
   }
   END_PREPROCESSING

   BEGIN_SEARCHING
   j = m1-1;
   while (j < n) {
      int k = 1;
      int l = y[j] == x[0];
      unsigned char c = y[j];
      WORD_TYPE D = ~0;
      WORD_TYPE D_;
      do {
         D = D & B[c][y[j-k]];
         D_ = D & L[c];
         D += D_;
         c = y[j-k];
         k++;
         if (D & M) {
            if (k == m1) {
               if (!strncmp(x+m1, y+j+1, m-m1)) {
                  count++;
               }
               break;
            }
            l = k;
         }
      } while (D);
      j += m1-l;
   }
   END_SEARCHING
   return count;
}
