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
 * This is an implementation of the SBNDM algorithm with loop unrolling
 * in J. Holub and B. Durian. Talk: Fast variants of bit parallel approach to suffix automata. 
 * The Second Haifa Annual International Stringology Research Workshop of the Israeli Science Foundation, (2005).
 */

#include "include/define.h"
#include "include/main.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int B[SIGMA], D;
   int i, j, pos, mMinus1, m2, count, shift;
   if(m<2) return -1;
   if(m>32) return search_large(x,m,y,n);

   /* Preprocessing */
   BEGIN_PREPROCESSING
   mMinus1 = m - 1;
   m2 = m - 2;
   for(i=0; i<SIGMA; i++) B[i]=0;
   for (i = 1; i <= m; ++i) B[x[m-i]] |= (1<<(i-1));

   D = B[x[m-2]]; j=1; shift=0;
   if(D & (1<<(m-1))) shift = m-j;
   for(i=m-3; i>=0; i--) {
      D = (D<<1) & B[x[i]];
      j++;
      if(D & (1<<(m-1))) shift = m-j;
   }
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   if( !memcmp(x,y,m) ) OUTPUT(0);
   j = m;
   while (j < n) {
      D = (B[y[j]]<<1) & B[y[j-1]];
      if (D != 0) {
         pos = j;
         while (D=(D<<1) & B[y[j-2]])
            --j;
         j += m2;
         if (j == pos) {
            OUTPUT(j);
            j+=shift;
         }
      }
      else j+=mMinus1;
   }
   END_SEARCHING
   return count;
}

/*
 * Simplified Backward Nondeterministic DAWG Matching with loop unrolling designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int B[SIGMA], D;
   int i, j, pos, mMinus1, m2, count, p_len, shift;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   p_len = m;
   m = 32;
   int diff = p_len-m;
   count = 0;
   mMinus1 = m - 1;
   m2 = m - 2;
   for(i=0; i<SIGMA; i++) B[i]=0;
   for (i = 1; i <= m; ++i)
      B[x[m-i]] |= (1<<(i-1));
   D = B[x[m-1]]; j=1; shift=1;
   for(i=m-2; i>0; i--, j++) {
      if(D & (1<<(m-1))) shift = j;
      D = (D<<1) & B[x[i]];
   }
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   if( !memcmp(x,y,m) ) OUTPUT(0);
   j = m;
   while (j+diff < n) {
      D = (B[y[j]]<<1) & B[y[j-1]];
      if (D != 0) {
         pos = j;
         while (D=(D<<1) & B[y[j-2]])
            --j;
         j += m2;
         if (j == pos) {
            for(i=m+1; i<p_len && x[i]==y[j-m+1+i]; i++);
            if (i==p_len) OUTPUT(j-m+1);
            j+=shift;
         }
      }
      else j+=mMinus1;
   }
   END_SEARCHING
   return count;
}



