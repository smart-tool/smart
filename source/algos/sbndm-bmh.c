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
 * This is an implementation of the SBNDM algorithm with Horspool shift
 * in J. Holub and B. Durian. Talk: Fast variants of bit parallel approach to suffix automata. 
 * The Second Haifa Annual International Stringology Research Workshop of the Israeli Science Foundation, (2005).
 */

#include "include/define.h"
#include "include/main.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int j,i, last, first, hbc[SIGMA];
   unsigned int D, B[SIGMA], s;
   int mM1 = m-1;
   int mM2 = m-2;
   int count = 0, restore[XSIZE+1], shift;
   if(m>32) return search_large(x,m,y,n);   
   if(m<2) return -1;
    
   /* preprocessing */
   BEGIN_PREPROCESSING
   for (i=0; i<SIGMA; i++)  B[i] = 0;
   for (i=0; i<m; i++) B[x[m-i-1]] |= (unsigned int)1 << (i+WORD-m);
   for (i=0;i<SIGMA;i++)   hbc[i]=2*m;
   for (i=0;i<m;i++)     hbc[x[i]]=(2*m)-i-1;
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
   if( !memcmp(x,y,m) ) count++;
   i = m;
   while (1) {
      while ((D = B[y[i]]) == 0) i += hbc[y[i+m]];
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
         count++;
         i += shift;
      } 
      else {
         i = j+m;
      }
   }
}

/*
 * Simplified Backward Nondeterministic DAWG Matching with Horspool shift designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
   int j,i, last, first, p_len, k, hbc[SIGMA];
   unsigned int D, B[SIGMA], s;
   int mM1 = m-1;
   int mM2 = m-2;
   int count = 0, restore[XSIZE+1], shift;

   p_len = m;
   m = 32;
   int diff = p_len-m;

   /* preprocessing */
   BEGIN_PREPROCESSING
   for (i=0; i<SIGMA; i++)  B[i] = 0;
   for (i=0; i<m; i++) B[x[m-i-1]] |= (unsigned int)1 << (i+WORD-m);
   for (i=0;i<SIGMA;i++)   hbc[i]=2*m;
   for (i=0;i<m;i++)     hbc[x[i]]=(2*m)-i-1;
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
   if( !memcmp(x,y,p_len) ) OUTPUT(0);
   i = m;
   while (1) {
      while ((D = B[y[i]]) == 0) i += hbc[y[i+m]];
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
