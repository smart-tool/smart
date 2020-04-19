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
 * This is an implementation of the Fast Average Optimal Shift Or algorithm
 * in K. Fredriksson and S. Grabowski. 
 * Practical and Optimal String Matching. SPIRE, Lecture Notes in Computer Science, vol.3772, pp.376--387, Springer-Verlag, Berlin, (2005).
 */

#include "include/define.h"
#include "include/main.h"
#include "include/log2.h"

void verify(unsigned char *y, int j, int n, unsigned char *x, int m, int q, int u, unsigned int D, unsigned int mm, int *count) {
   int s, c, mq, v, z, i, k;

   D = (D & mm)^mm;
   mq = m/q-1;
   while (D != 0) {
      s = LOG2(D);
      v = mq+u;
      c = -mq*q;
      z = s%v-mq;
      c -= (s/v + z*q);
      i = j+c;
      k = 0;
      if(i>=0 && i<=n-m) while(k<m && x[k]==y[i+k]) k++;
      if(k==m) (*count)++;
      D &= ~(1<<s);
   }
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int B[SIGMA], D, h, mm;
   unsigned int masq;
   int i, j, u, count;
   int uq, uq1, mq;
   int q=2;

   u = 2;
   if(m>32-u+1) return search_large(x,m,y,n,q);
   if(m<=q) return -1;   
   
   /* Preprocessing */
   BEGIN_PREPROCESSING
   masq = 0;
   mq = m/q;
   h = mq;
   for (j = 0; j < q; ++j) {
      masq |= (1<<h);
      masq |= (1<<h);
      h += mq;
      ++h;
   }
   for (i = 0; i < SIGMA; ++i) 
      B[i] = ~0;
   h=mm=0;
   for (j = 0; j < q; ++j) {
      for (i = 0; i < mq; ++i) {
         B[x[i*q+j]] &= ~(1<<h);
         ++h;
      }
      mm |= (1<<(h-1)); ++h;
      mm |= (1<<(h-1)); ++h;
      --h;
   }
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   D = ~mm;
   j = 0;
   uq = u*q;
   uq1 = (u-1)*q;
   while (j < n) {
      D = (D<<1)|(B[y[j]]&~masq);
      D = (D<<1)|(B[y[j+q]]&~masq);
      if ((D & mm) != mm)
         verify(y, j+uq1, n, x, m, q, u, D, mm, &count);
      D &= ~mm;
      j += uq;
   }
   END_SEARCHING
   return count;
}

/*
 * Fast Average Optimal Shift Or algorithm designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

void verify_large(unsigned char *y, int j, int n, unsigned char *x, int m, int q, int u, unsigned int D, unsigned int mm, int *count, int p_len) {
   int s, c, mq, v, z, i, k;

   D = (D & mm)^mm;
   mq = m/q-1;
   while (D != 0) {
      s = LOG2(D);
      v = mq+u;
      c = -mq*q;
      z = s%v-mq;
      c -= (s/v + z*q);
      i = j+c;
      k = 0;
      if(i>=0 && i<=n-m) while(k<p_len && x[k]==y[i+k]) k++;
      if(k==p_len) (*count)++;
      D &= ~(1<<s);
   }
}

int search_large(unsigned char *x, int m, unsigned char *y, int n, int q) {
   unsigned int B[SIGMA], D, h, mm;
   unsigned int masq;
   int i, j, u, count, p_len;
   int uq, uq1, mq;

   u = 2;
   p_len = m;
   m = 32-u+1;

   /* Preprocessing */
   BEGIN_PREPROCESSING
   masq = 0;
   mq = m/q;
   h = mq;
   for (j = 0; j < q; ++j) {
      masq |= (1<<h);
      masq |= (1<<h);
      h += mq;
      ++h;
   }
   for (i = 0; i < SIGMA; ++i) 
      B[i] = ~0;
   h=mm=0;
   for (j = 0; j < q; ++j) {
      for (i = 0; i < mq; ++i) {
         B[x[i*q+j]] &= ~(1<<h);
         ++h;
      }
      mm |= (1<<(h-1)); ++h;
      mm |= (1<<(h-1)); ++h;
      --h;
   }
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   D = ~mm;
   j = 0;
   uq = u*q;
   uq1 = (u-1)*q;
   while (j < n) {
      D = (D<<1)|(B[y[j]]&~masq);
      D = (D<<1)|(B[y[j+q]]&~masq);
      if ((D & mm) != mm)
         verify_large(y, j+uq1, n, x, m, q, u, D, mm, &count, p_len);
      D &= ~mm;
      j += uq;
   }
   END_SEARCHING
   return count;
}
