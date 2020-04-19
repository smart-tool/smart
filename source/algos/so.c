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
 * This is an implementation of the Shift Or algorithm
 * in R. Baeza-Yates and G. H. Gonnet. 
 * A new approach to text searching. Commun. ACM, vol.35, n.10, pp.74--82, ACM, New York, NY, USA, (1992).
 */

#include "include/define.h"
#include "include/main.h"

int preSo(unsigned char *x, int m, unsigned int S[]) { 
   unsigned int j, lim; 
   int i; 
   for (i = 0; i < SIGMA; ++i) 
      S[i] = ~0; 
   for (lim = i = 0, j = 1; i < m; ++i, j <<= 1) { 
      S[x[i]] &= ~j; 
      lim |= j; 
   } 
   lim = ~(lim>>1); 
   return(lim); 
} 

int search(unsigned char *x, int m, unsigned char *y, int n) { 
   unsigned int lim, D; 
   unsigned int S[SIGMA]; 
   int j, count; 
   if (m > WORD) return search_large(x,m,y,n);

   /* Preprocessing */ 
   BEGIN_PREPROCESSING
   lim = preSo(x, m, S); 
   END_PREPROCESSING

   /* Searching */ 
   BEGIN_SEARCHING
   count = 0;
   for (D = ~0, j = 0; j < n; ++j) { 
      D = (D<<1) | S[y[j]]; 
      if (D < lim) 
        OUTPUT(j - m + 1); 
   } 
   END_SEARCHING
   return count;
} 

/*
 * Shift Or algorithm designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n) { 
   unsigned int lim, D,k,h,p_len; 
   unsigned int S[SIGMA]; 
   int j, count; 

   p_len = m;
   m=32;

   /* Preprocessing */ 
   BEGIN_PREPROCESSING
   lim = preSo(x, m, S); 
   END_PREPROCESSING

   /* Searching */ 
   BEGIN_SEARCHING
   count = 0;
   for (D = ~0, j = 0; j < n; ++j) { 
      D = (D<<1) | S[y[j]]; 
      if (D < lim) {
         k = 0;
         h = j-m+1;
         while(k<p_len && x[k]==y[h+k]) k++;
         if(k==p_len) OUTPUT(j - m + 1); 
      }
   } 
   END_SEARCHING
   return count;
} 

