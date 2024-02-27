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
 * This is an implementation of the KMP Skip Search algorithm
 * in C. Charras and T. Lecroq and J. D. Pehoushek. 
 * A Very Fast String Matching Algorithm for Small Alphabets and Long Patterns. 
 * Proceedings of the 9th Annual Symposium on Combinatorial Pattern Matching, 
 * Lecture Notes in Computer Science, n.1448, pp.55--64, Springer-Verlag, Berlin, (1998).
 */

#include "include/define.h"
#include "include/main.h"

void preKmp(unsigned char *x, int m, int kmpNext[]) {
   int i, j;
   i = 0;
   j = kmpNext[0] = -1;
   while (i < m) {
      while (j > -1 && x[i] != x[j])
         j = kmpNext[j];
      i++;
      j++;
      if (i<m && x[i] == x[j])
         kmpNext[i] = kmpNext[j];
      else
         kmpNext[i] = j;
   }
}

void preMp(char *x, int m, int mpNext[]) {
   int i, j;

   i = 0;
   j = mpNext[0] = -1;
   while (i < m) {
      while (j > -1 && x[i] != x[j])
         j = mpNext[j];
      mpNext[++i] = ++j;
   }
}

int attempt(char *y, char *x, int m, int start, int wall) {
   int k;
   k = wall - start;
   while (k < m && x[k] == y[k + start])
      ++k;
   return(k);
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, k, kmpStart, per, start, wall, count;
   int kmpNext[XSIZE], list[XSIZE], mpNext[XSIZE], z[SIGMA];

   /* Preprocessing */
   BEGIN_PREPROCESSING
   preMp(x, m, mpNext);
   preKmp(x, m, kmpNext);
   memset(z, -1, SIGMA*sizeof(int));
   memset(list, -1, m*sizeof(int));
   z[x[0]] = 0;
   for (i = 1; i < m; ++i) {
      list[i] = z[x[i]];
      z[x[i]] = i;
   }
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   wall = 0;
   per = m - kmpNext[m];
   i = j = -1;
   do {
      j += m;
   } while (j < n && z[y[j]] < 0);
   if (j >= n) {
       END_SEARCHING
       return count;
   }
   i = z[y[j]];
   start = j - i;
   while (start <= n - m) {
      if (start > wall)
         wall = start;
      k = attempt(y, x, m, start, wall);
      wall = start + k;
      if (k == m) {
         OUTPUT(start);
         i -= per;
      }
      else
         i = list[i];
      if (i < 0) {
         do {
            j += m;
         } while (j < n && z[y[j]] < 0);
         if (j >= n) {
             END_SEARCHING
             return count;
         }
         i = z[y[j]];
      }
      kmpStart = start + k - kmpNext[k];
      k = kmpNext[k];
      start = j - i;
      while (start < kmpStart ||
             (kmpStart < start && start < wall)) {
         if (start < kmpStart) {
            i = list[i];
            if (i < 0) {
               do {
                  j += m;
               } while (j < n && z[y[j]] < 0);
               if (j >= n) {
                   END_SEARCHING
                   return count;
               }
               i = z[y[j]];
            }
            start = j - i;
         }
         else {
            kmpStart += (k - mpNext[k]);
            k = mpNext[k];
         }
      }
   }
   END_SEARCHING
   return count;
}

