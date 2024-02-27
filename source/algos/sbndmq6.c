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
 * This is an implementation of the Simplified BNDM with q-grams
 * in B. Durian and J. Holub and H. Peltola and J. Tarhio. 
 * Tuning BNDM with q-Grams. 
 * Proceedings of the Workshop on Algorithm Engineering and Experiments, ALENEX 2009, pp.29--37, SIAM, New York, New York, USA, (2009).
 */


#include "include/define.h"
#include "include/main.h"
#define GRAM6(j) (B[y[j]]<<5)&(B[y[j-1]]<<4)&(B[y[j-2]]<<3)&(B[y[j-3]]<<2)&(B[y[j-4]]<<1)&B[y[j-5]]

int search(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int B[SIGMA], D, q, shift;
   int i, j, pos, mMinusq, mq, count;
   q = 6;

   if(m<q) return -1;
   if(m>32) return search_large(x,m,y,n);

   /* Preprocessing */
   BEGIN_PREPROCESSING
   count = 0;
   mMinusq = m - q +1;
   mq = m - q;
   for(i=0; i<SIGMA; i++) B[i]=0;
   for (i = 1; i <= m; ++i)
      B[x[m-i]] |= (1<<(i-1));

   D = B[x[m-2]]; j=1; shift=0;
   if(D & (1<<(m-1))) shift = m-j;
   for(i=m-3; i>=0; i--) {
      D = (D<<1) & B[x[i]];
      j++;
      if(D & (1<<(m-1))) shift = m-j;
   }
   END_PREPROCESSING

   BEGIN_SEARCHING
   /* Searching */
   if( !memcmp(x,y,m) ) OUTPUT(0);
   j = m;
   while (j < n) {
      D = GRAM6(j);
      if (D != 0) {
         pos = j;
         while (D=(D<<1) & B[y[j-q]]) --j;
         j += mq;
         if (j == pos) {
            OUTPUT(j);
            j+=shift;
         }
      }
      else j+=mMinusq;
   }
   END_SEARCHING
   return count;
}

/*
 * Simplified Backward Nondeterministic DAWG Matching using q-grams designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int B[SIGMA], D, q, shift;
   int i, j, pos, mMinusq, mq, count, p_len;
   q = 6;

   p_len = m;
   m = 32;
   int diff = p_len-m;
   
   BEGIN_PREPROCESSING
   /* Preprocessing */
   count = 0;
   mMinusq = m - q +1;
   mq = m - q;
   for(i=0; i<SIGMA; i++) B[i]=0;
   for (i = 1; i <= m; ++i)
      B[x[m-i]] |= (1<<(i-1));

   D = B[x[m-2]]; j=1; shift=0;
   if(D & (1<<(m-1))) shift = m-j;
   for(i=m-3; i>=0; i--) {
      D = (D<<1) & B[x[i]];
      j++;
      if(D & (1<<(m-1))) shift = m-j;
   }
   END_PREPROCESSING

   BEGIN_SEARCHING
   /* Searching */
   if( !memcmp(x,y,p_len) ) OUTPUT(0);
   j = m;
   while (j+diff < n) {
      D = GRAM6(j);
      if (D != 0) {
         pos = j;
         while (D=(D<<1) & B[y[j-q]]) --j;
         j += mq;
         if (j == pos) {
            for(i=m+1; i<p_len && x[i]==y[j-m+1+i]; i++);
            if (i==p_len) OUTPUT(j-m+1);
            j+=shift;
         }
      }
      else j+=mMinusq;
   }
   END_SEARCHING
   return count;
}



