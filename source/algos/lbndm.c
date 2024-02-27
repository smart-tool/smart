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
 * This is an implementation of the LBNDM algorithm
 * in H. Peltola and J. Tarhio. 
 * Alternative Algorithms for Bit-Parallel String Matching. 
 * Proceedings of the 10th International Symposium on String Processing and Information Retrieval SPIRE'03, 
 * Lecture Notes in Computer Science, vol.2857, pp.80--94, Springer-Verlag, Berlin, Manaus, Brazil, (2003).
 */

#include "include/define.h"
#include "include/main.h"

int verify(unsigned char *y, int left, unsigned char *x, int m, int k) {
   int j,i;
   int count = 0;
   for (j=0; j<k; j++) {
      if (m+j > left) break;
      i = 0;
      while(i<m && y[i]==x[i]) i++;
      if(i>=m) count++;
      y++;
   }
   return count;
}

int search(unsigned char *x, int m, unsigned char *y, int n)
{
   unsigned int B[SIGMA] = {0};
   unsigned int M;
   int k;
   int i, j, l;
   int m1, m2, rmd;
   if(m<2) return -1;
    
   BEGIN_PREPROCESSING
   /* Preprocessing */
   M = 1 << (WORD-1);
   k = (m-1)/WORD+1;
   m1 = m-1;
   m2 = m1-k;
   rmd = m-(m/k)*k;
   for (i=m/k, l=m; i>0; i--, l-=k)
      for (j=k; j>0; j--)
         B[x[l-j]] |= 1 << (WORD-i);
   END_PREPROCESSING

   BEGIN_SEARCHING
   /* Searching */
   int count = 0;
   j = 0;
   while (j <= n-m) {
      unsigned int D = B[y[j+m1]];
      int last = (D & M) ? m-k-rmd : m-rmd;
      l = m2;
      while (D) {
         D = (D << 1) & B[y[j+l]];
         if (D & M) {
            if (l < k+rmd) {
               count += verify(y+j, n-j, x, m, k);
               break;
            }
            last = l-(k+rmd)+1;
         }
         l -= k;
      }
      j += last;
   }
   END_SEARCHING
   return count;
}
