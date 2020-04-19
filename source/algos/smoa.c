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

/* Compute the next maximal suffix. */
void nextMaximalSuffix(unsigned char *x, int m,
                       int *i, int *j, int *k, int *p) {
   char a, b;
 
   while (*j + *k < m) {
      a = x[*i + *k];
      b = x[*j + *k];
      if (a == b)
         if (*k == *p) {
            (*j) += *p;
            *k = 1;
         }
         else
            ++(*k);
      else
         if (a > b) {
            (*j) += *k;
            *k = 1;
            *p = *j - *i;
         }
         else {
            *i = *j;
            ++(*j);
            *k = *p = 1;
         }
   }
}
 
 
/* String matching on ordered alphabets algorithm. */
int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, ip, j, jp, k, p, count;
 
   BEGIN_PREPROCESSING
   END_PREPROCESSING
   BEGIN_SEARCHING
   count = 0;
   /* Searching */
   ip = -1;
   i = j = jp = 0;
   k = p = 1;
   while (j <= n - m) {
      while (i + j < n && i < m && x[i] == y[i + j])
         ++i;
      if (i == 0) {
         ++j;
         ip = -1;
         jp = 0;
         k = p = 1;
      }
      else {
         if (i >= m)
            OUTPUT(j);
         nextMaximalSuffix(y + j, i+1, &ip, &jp, &k, &p);
         if (ip < 0 ||
             (ip < p &&
              memcmp(y + j, y + j + p, ip + 1) == 0)) {
            j += p;
            i -= p;
            if (i < 0)
               i = 0;
            if (jp - ip > p)
               jp -= p;
            else {
               ip = -1;
               jp = 0;
               k = p = 1;
            }
         }
         else {
            j += (MAX(ip + 1,
                      MIN(i - ip - 1, jp + 1)) + 1);
            i = jp = 0;
            ip = -1;
            k = p = 1;
         }
      }
   }
   END_SEARCHING
   return count;
}


