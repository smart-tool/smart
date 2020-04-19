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
 * This is an implementation of the Backward Nondeterministic DAWG Matching
 * in G. Navarro and M. Raffinot. 
 * A Bit-parallel Approach to Suffix Automata: Fast Extended String Matching. n.TR/DC--98--1, (1998).
 */

#include "include/define.h"
#include "include/main.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int B[SIGMA];
   int i, j, s, D, last, count;

   if (m > 32) return search_large(x,m,y,n);

	BEGIN_PREPROCESSING
   /* Preprocessing */
   for(i=0; i<SIGMA; i++) B[i]=0;
   s=1;
   for (i=m-1; i>=0; i--) {
      B[x[i]] |= s;
      s <<= 1;
   }
	END_PREPROCESSING

	BEGIN_SEARCHING
   /* Searching */
   j=0; count=0;
   while (j <= n-m) {
      i=m-1; last=m;
      D = ~0;
      while (i>=0 && D!=0) {
         D &= B[y[j+i]];
         i--;
         if (D != 0) {
            if (i >= 0) last = i+1;
            else count ++;
         }
         D <<= 1;
      }
      j += last;
   }
	END_SEARCHING
   return count;
  }

/*
 * Backward Nondeterministic DAWG Matching designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
   int B[SIGMA];
   int i, j, s, D, last, count, p_len, k;

   p_len = m;
   m = 32;

   /* Preprocessing */
    BEGIN_PREPROCESSING
   memset(B,0,SIGMA*sizeof(int));
   s=1;
   for (i=m-1; i>=0; i--) {
      B[x[i]] |= s;
      s <<= 1;
   }
    END_PREPROCESSING

   /* Searching */
    BEGIN_SEARCHING
   j=0; count=0;
   while (j <= n-m) {
      i=m-1; last=m;
      D = ~0;
      while (i>=0 && D!=0) {
         D &= B[y[j+i]];
         i--;
         if (D != 0) {
            if (i >= 0)
               last = i+1;
            else {
               k = m;
               while(k<p_len && x[k]==y[j+k]) k++;            
               if (k==p_len) count ++;
            }
         }
         D <<= 1;
      }
      j += last;
   }
   END_SEARCHING
   return count;
}




