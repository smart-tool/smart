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
*/

#include "include/define.h"
#include "include/main.h"

int pow2(int n) {
   int p, i;
   p = 1;
   i = 0;
   while (i < n) { 
      p *= 2;      
      ++i;         
   } 
   return p;
}

int mylog2(int unsigned n) {
   int ell;
   ell = 0;
   while (n >= 2) {
      ++ell;
      n /= 2;
   }
   return ell;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, z, count;
   unsigned int b, D, Delta, mask, mask2, T[SIGMA];
   if(m>30) return search_large(x,m,y,n);   

   /* Preprocessing */
   BEGIN_PREPROCESSING
   z = WORD;
   mask = 1;
   for (i = 1; i < m; ++i) mask = (mask << 1) | 1;
   for (i = 0; i < SIGMA; ++i) T[i] = mask;
   mask2 = 1;
   for (i = 0; i < m; ++i) {
      T[x[i]] &= ~mask2;
      mask2 <<= 1;
   }
   mask2 >>= 1;
   END_PREPROCESSING
   
   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   D = 0;
   b = mask;
   i = m - 1;
   j = i;
   while (i < n) {
      D |= (T[y[j]] << (i - j));
      D &= mask;
      b &= ~pow2(m - i + j - 1);
      if ((D & mask2) == 0) {
         if (b == 0) {
            D |= mask2;
            OUTPUT(i - m + 1);
         }
         else {
            j = i - (m - mylog2(b) - 1);
            continue;
         }
      }
      if (D == mask) {
         D = 0;
         b = mask;
         i += m;
      }
      else {
         Delta = m - mylog2(~D & mask) - 1;
         D <<= Delta;
         b = ((b | ~mask) >> Delta) & mask;
         i += Delta;
      }
      j = i;
   }
   END_SEARCHING
   return count;
}

int search_large(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, z, k, count, first, p_len;
   unsigned int b, D, Delta, mask, mask2, T[SIGMA];
   p_len = m;
   m = 30;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   z = WORD;
   mask = 1;
   for (i = 1; i < m; ++i) mask = (mask << 1) | 1;
   
   for (i = 0; i < SIGMA; ++i) T[i] = mask;
   mask2 = 1;
   for (i = 0; i < m; ++i) {
      T[x[i]] &= ~mask2;
      mask2 <<= 1;
   }
   mask2 >>= 1;
   END_PREPROCESSING
   
   /* searching */
   BEGIN_SEARCHING
   count = 0;
   D = 0;
   b = mask;
   i = m - 1;
   j = i;
   while (i < n) {
      D |= (T[y[j]] << (i - j));
      D &= mask;
      b &= ~pow2(m - i + j - 1);
      if ((D & mask2) == 0) {
         if (b == 0) {
            D |= mask2;
            k=0;
            first = i-m+1;
            while(k<p_len && x[k]==y[first+k]) k++;
            if(k==p_len) count++;
         }
         else {
            j = i - (m - mylog2(b) - 1);
            continue;
         }
      }
      if (D == mask) {
         D = 0;
         b = mask;
         i += m;
      }
      else {
         Delta = m - mylog2(~D&mask) - 1;
         D <<= Delta;
         b = ((b | ~mask) >> Delta) & mask;
         i += Delta;
      }
      j = i;
   }
   END_SEARCHING
   return count;
}


