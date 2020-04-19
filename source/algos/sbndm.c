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
 * This is an implementation of the Simplified BNDM algorithm
 * in H. Peltola and J. Tarhio. Alternative Algorithms for Bit-Parallel String Matching. 
 * Proceedings of the 10th International Symposium on String Processing and Information Retrieval SPIRE'03, 
 * Lecture Notes in Computer Science, vol.2857, pp.80--94, Springer-Verlag, Berlin, (2003).
 */

#include "include/define.h"
#include "include/main.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int j,i, last, first;
   unsigned int D, B[SIGMA], s;
   int mM1 = m-1;
   int mM2 = m-2;
   int count = 0, restore[XSIZE+1], shift;

   if(m>32) return search_large(x,m,y,n);
    if(m<2) return -1;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   for (i=0; i<SIGMA; i++)  B[i] = 0;
   for (i=0; i<m; i++) B[x[m-i-1]] |= (unsigned int)1 << (i+WORD-m);

   last = m;
   s = (unsigned int)(~0) << (WORD-m);
   s = (unsigned int)(~0);
   for (i=m-1; i>=0; i--) {
      s &= B[x[i]]; 
      if (s & ((unsigned int)1<<(WORD-1))) {
         if(i > 0)  last = i; 
      }
      restore[i] = last;
      s <<= 1;
   }
        shift = restore[0];

   for(i=0; i<m; i++) y[n+i]=x[i];
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   if(!memcmp(x,y,m)) OUTPUT(0);
   i = m;
   while (1) {
      D = B[y[i]];
      j = i-1; first = i-m+1;
      while (1) {
         D = (D << 1) & B[y[j]];
         if (!((j-first) && D)) break;
         j--;
      }
      if (D != 0) {
         if (i >= n) {
   			END_SEARCHING
         	return count;
         }
         OUTPUT(first);
         i += shift;
      } 
      else {
         i = j+m;
      }
   }
}

/*
 * Simplified Backward Nondeterministic DAWG Matching algorithm designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
   int j,i, last, first, p_len, k;
   unsigned int D, B[SIGMA], s;
   int mM1 = m-1;
   int mM2 = m-2;
   int count = 0, restore[XSIZE+1], shift;

   p_len = m;
   m = 32;
   int diff = p_len-m;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   for (i=0; i<SIGMA; i++)  B[i] = 0;
   for (i=0; i<m; i++) B[x[m-i-1]] |= (unsigned int)1 << (i+WORD-m);

   last = m;
   s = (unsigned int)(~0) << (WORD-m);
   s = (unsigned int)(~0);
   for (i=m-1; i>=0; i--) {
      s &= B[x[i]]; 
      if (s & ((unsigned int)1<<(WORD-1))) {
         if(i > 0)  last = i; 
      }
      restore[i] = last;
      s <<= 1;
   }
   shift = restore[0];

   for(i=0; i<m; i++) y[n+i]=x[i];
   END_PREPROCESSING

   BEGIN_SEARCHING
   /* Searching */
   if(!memcmp(x,y,p_len)) OUTPUT(0);
   i = m;
   while (1) {
      while ((D = B[y[i]]) == 0) i += m;
      j = i-1; first = i-m+1;
      while (1) {
         D = (D << 1) & B[y[j]];
         if (!((j-first) && D)) break;
         j--;
      }
      if (D != 0) {
         if (i+diff >= n) {
   			END_SEARCHING
         	return count;
         }
         k=m;
         while(k<p_len && x[k]==y[first+k]) k++;
         if(k==p_len) OUTPUT(first);
         i += shift;
      } 
      else {
         i = j+m;
      }
   }
}
