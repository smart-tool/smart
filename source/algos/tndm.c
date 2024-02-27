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
 * This is an implementation of the Two-Way Nondeterministic DAWG Matching algorithm
 * in H. Peltola and J. Tarhio. 
 * Alternative Algorithms for Bit-Parallel String Matching. 
 * Proceedings of the 10th International Symposium on String Processing and Information Retrieval SPIRE'03, (2003).
 */

#include "include/define.h"
#include "include/main.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i,j,s,last, restore[XSIZE+1];
   unsigned int d, B[SIGMA];
   int count = 0;
   if(m>32) return search_large(x,m,y,n);

   /* Preprocessing */
   BEGIN_PREPROCESSING
   for(i=0; i<SIGMA; i++) B[i]=0;
   s=1; 
   for (i=m-1; i>=0; i--){ 
      B[x[i]] |= s; 
      s <<= 1;
   }
   last = m;
   s = (unsigned int)(~0) >> (WORD-m);
   for (i=m-1; i>=0; i--) {
      s &= B[x[i]]; 
      if (s & ((unsigned int)1<<(m-1))) {
         if(i > 0)  last = i; 
      }
      restore[m-i] = last;
      s <<= 1;
   }
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   j=m-1;
   while (j < n){
      i=0;
      last=m;
      d = B[y[j]];
      if ((d&1) == 0) {
         while (d!=0 && !(d&((unsigned int)1<<i))) {
            i++;
            d &= B[y[j+i]]<<i;
         } 
         if (d==0 || j+i>=n ) {
            goto over;
         } 
         j += i; 
         last = restore[i]; 
      }
      do {
         i++;
         if (d & ((unsigned int)1<<(m-1))) {
            if(i < m)  last = m-i; 
            else {
               OUTPUT(j); 
               goto over;
            } 
         }
         d<<=1;
         d &= B[y[j-i]]; 
      } while(d != 0); 

      over:
      j += last; 
   } 
   END_SEARCHING
   return count;
}

/*
 * Two-Way Nondeterministic DAWG Matching algorithm designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n)
{
   int i,j,s,last, restore[XSIZE+1], p_len, k;
   unsigned int D, B[SIGMA];
   int count = 0;
   p_len = m;
   m = 32;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   for(i=0; i<SIGMA; i++) B[i]=0;
   s=1; 
   for (i=m-1; i>=0; i--){ 
      B[x[i]] |= s; 
      s <<= 1;
   }
   last = m;
   s = (unsigned int)(~0) >> (WORD-m);
   for (i=m-1; i>=0; i--) {
      s &= B[x[i]]; 
      if (s & ((unsigned int)1<<(m-1))) {
         if(i > 0)  last = i; 
      }
      restore[m-i] = last;
      s <<= 1;
   }
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   j=m-1;
   while (j < n){
      i=0;
      last=m;
      D = B[y[j]];
      if ((D & 1) == 0) {
         while (D!=0 && !(D & ((unsigned int)1<<i))) {
            i++;
            D &= B[y[j+i]]<<i;
         } 
         if (D==0 || j+i>=n ) {
            goto over;
         } 
         j += i; 
         last = restore[i]; 
      }

      do {
         i++;
         if (D & ((unsigned int)1<<(m-1))) {
            if(i < m)  last = m-i; 
            else {
               k = m;
               while(k<p_len && x[k]==y[j-m+1+k]) k++;
               if (k>=p_len) OUTPUT(j-m+1); 
               goto over;
            } 
         }
         D <<= 1;
         D &= B[y[j-i]]; 
      } while(D != 0); 

      over:
      j += last; 
   } 
    END_SEARCHING
  return(count);
}
