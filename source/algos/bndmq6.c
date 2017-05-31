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
 * This is an implementation of the BNDM with q-grams
 * in B. Durian and J. Holub and H. Peltola and J. Tarhio. 
 * Tuning BNDM with q-Grams. 
 * Proceedings of the Workshop on Algorithm Engineering and Experiments, ALENEX 2009, pp.29--37, SIAM, New York, New York, USA, (2009).
 */

#include "include/define.h"
#include "include/main.h"
#define GRAM6(i) (B[y[i+5]]<<5)&(B[y[i+4]]<<4)&(B[y[i+3]]<<3)&(B[y[i+2]]<<2)&(B[y[i+1]]<<1)&B[y[i]]

int search(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int D, B[SIGMA], M, s;
   int i,j,q, count, first;
   q = 6;
   if(m<q) return -1;
   if(m>WORD) return search_large(x,m,y,n);

   /* preprocessing */
    BEGIN_PREPROCESSING
   for (i=0; i<SIGMA; i++) B[i] = 0;
   s=1;
   for (i=m-1; i>=0; i--){
       B[x[i]] |= s;
       s <<= 1;
   }
   for (j=0; j<m; j++) y[n+j]=x[j];
   M = 1 << (m-1);
    END_PREPROCESSING

   /* searching */
    BEGIN_SEARCHING
   count = 0;
   if(!memcmp(x,y,m)) OUTPUT(0);
   i = m+1-q;
   while (i <= n - q) {
      D = GRAM6(i);
      if (D != 0) {
         j = i;
         first = i - (m - q);
         do {
            if ( D >= M ) {
               if (j > first) i = j-1;
               else OUTPUT(first);
            }
            j = j-1;
            D = (D<<1) & B[y[j]];
         } while (D != 0);
      }
      i = i+m-q+1;
   }
   END_SEARCHING
   return count;
}

/*
 * Backward Nondeterministic DAWG Matching using q-grams designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int D, B[SIGMA], M, s;
   int i,j,q, count, first, p_len, k;
   q = 6;
   if(m<=q) return 0;

   p_len = m;
   m = 32;

   /* Preprocessing */
    BEGIN_PREPROCESSING
   for (i=0; i<SIGMA; i++) B[i] = 0;
   s=1;
   for (i=m-1; i>=0; i--){
      B[x[i]] |= s;
      s <<= 1;
   }
   for (j=0; j<m; j++) y[n+j]=x[j];
   M = 1 << (m-1);
    END_PREPROCESSING

   /* Searching */
    BEGIN_SEARCHING
   count = 0;
   if(!memcmp(x,y,p_len)) OUTPUT(0);
   i = m+1-q;
   while (i <= n - q) {
      D = GRAM6(i);
      if (D != 0) {
         j = i;
         first = i - (m - q);
         do {
            if ( D >= M ) {
               if (j > first) i = j-1;
               else {
                   if(first <= n-p_len && !memcmp(x,y+first,p_len)) OUTPUT(first);
               }
            }
            j = j-1;
            D = (D<<1) & B[y[j]];
         } while (D != 0);
      }
      i = i+m-q+1;
   }
   END_SEARCHING
   return count;
}
