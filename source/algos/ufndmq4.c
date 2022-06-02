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
 * This is an implementation of the Shift Or with q-grams algorithm
 * in B. Durian and J. Holub and H. Peltola and J. Tarhio. 
 * Tuning BNDM with q-Grams. Proceedings of the Workshop on Algorithm Engineering and Experiments, 
 * ALENEX 2009, pp.29--37, SIAM, New York, New York, USA, (2009).
 */

#include "include/define.h"
#include "include/main.h"
#define GRAM4(i) ( (B[y[i-3]]<<3) | (B[y[i-2]]<<2) | (B[y[i-1]]<<1) | B[y[i]] )

int search(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int D, F, mm, mask, B[SIGMA], S;
   int i, j, k, mq, q;
   int count = 0;
   q = 4;
   if(m<q) return -1;
   if(m+q > WORD) return search_large(x,m,y,n);

   /* Preprocessing */
   BEGIN_PREPROCESSING
   S = ~((unsigned char)7 << m);
   for (j = 0; j < SIGMA; ++j) B[j] = S;
   for (j = 0; j < m; ++j)   B[x[j]] &= ~((unsigned char)1<<j);
   for (i=0; i<m; i++) y[n+i]=y[n+m+i]=x[i];
   mm = (unsigned char)1 << (m+q-2);
   mask = ((unsigned char)1 << (m-1)) - 1;
   mq = m+q-1;
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   if( !memcmp(x,y,m) ) OUTPUT(0);
   i = 0;
   D = ~0;
   while (1) {
      while ( (D|7) == ~0 ) { 
         i += m;
         D = (D<<m) | GRAM4(i);
      }
      if (F=~(D|mask)) {
         for (k=i-mq+1; F; F<<=1, k++)
         if (F >= mm) {
            for (j=0; j<m; j++)
               if (y[k+j]!=x[j]) goto mismatch;
            if (k+m > n)  {
                END_SEARCHING
                return(count);
            }
            OUTPUT(k);
          
            mismatch:
            F -= mm;
         }
      }
      i += q;
      D = (D<<q) | GRAM4(i);
   }
   abort();  
}

/*
 * Shift Or with q-grams algorithm designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n)
{
   unsigned int D, F, mm, mask, B[SIGMA], S;
   int i, j, k, mq, q, p_len;
   int count = 0;
   q = 4;
   if (m<q) return 0;
   
   for (i=0; i<m; i++) y[n+i]=y[n+m+i]=x[i];
   p_len = m;
   m = 32-q;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   S = ~((unsigned char)7 << m);
   for (j = 0; j < SIGMA; ++j) B[j] = S;
   for (j = 0; j < m; ++j)   B[x[j]] &= ~((unsigned char)1<<j);
   mm = (unsigned char)1 << (m+q-2);
   mask = ((unsigned char)1 << (m-1)) - 1;
   mq = m+q-1;
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   if( !memcmp(x,y,p_len) ) OUTPUT(0);
   i = 0;
   D = ~0;
   while (1) {
      while ( (D|7) == ~0 ) { 
         i += m;
         D = (D<<m) | GRAM4(i);
      }
      if (F=~(D|mask)) {
         for (k=i-mq+1; F; F<<=1, k++)
            if (F >= mm) {
               for (j=0; j<p_len; j++)
                  if (y[k+j]!=x[j]) goto mismatch;
               if (k+p_len > n)  {
                   END_SEARCHING
                   return(count);
               }
               OUTPUT(k);
          
               mismatch:
               F -= mm;
            }
      }
      i += q;
      D = (D<<q) | GRAM4(i);
   }
   abort();
}
