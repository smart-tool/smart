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
 * This is an implementation of the Forward Nondeterministic DAWG Matching algorithm
 * in J. Holub and B. Durian. Talk: Fast variants of bit parallel approach to suffix automata. 
 * The Second Haifa Annual International Stringology Research Workshop of the Israeli Science Foundation, (2005).
 */

#include "include/define.h"
#include "include/main.h"
#define neg(K) (~K)

int search(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int D, B[SIGMA], NEG0, NEG0m1;
   int i,j,k,count, first;
   if(m>32) return search_large(x,m,y,n);

   /* Preprocessing */
   BEGIN_PREPROCESSING
   for(i=0; i<SIGMA; i++) B[i] = neg(0);
   for(j=0; j<m; j++) B[x[j]] = B[x[j]] & neg((1<<j));
   NEG0 = neg(0);
   NEG0m1 = neg(0)<<(m-1);
   END_PREPROCESSING
   
   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   i = m-1;
   while( i < n ) {
      D = B[y[i]];
      while (D != NEG0) {
         if (D < NEG0m1) {
            k = 0; first=i-m+1;
            while(k<m && x[k]==y[first+k]) k++;
            if (k==m && i<n) count++;
         }
         i = i+1;
         D = (D<<1) | B[y[i]];
      }
      i=i+m;
   }
   END_SEARCHING
   return count;
}

/*
 * Forward Nondeterministic DAWG Matching algorithm designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int D, B[SIGMA], NEG0, NEG0m1;
   int i,j,k,count, first, p_len;

   p_len = m;
   m = 32;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   for(i=0; i<SIGMA; i++) B[i] = neg(0);
   for(j=0; j<m; j++) B[x[j]] = B[x[j]] & neg((1<<j));
   NEG0 = neg(0);
   NEG0m1 = neg(0)<<(m-1);
   END_PREPROCESSING
   
   /* searching */
   BEGIN_SEARCHING
   count = 0;
   i = m-1;
   while( i < n ) {
      D = B[y[i]];
      while (D != NEG0) {
         if (D < NEG0m1) {
            k = 0; first=i-m+1;
            while(k<p_len && x[k]==y[first+k]) k++;
            if (k==p_len && i<n) count++;
         }
         i = i+1;
         D = (D<<1) | B[y[i]];
      }
      i=i+m;
   }
   END_SEARCHING
   return count;
}
