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
 * This is an implementation of the Not So Naive algorithm
 * in C. Hancart. Analyse exacte et en moyenne d'algorithmes de recherche d'un motif dans un texte. (1993).
 */

#include "include/define.h"
#include "include/main.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int j, k, ell, count;
  
   /* Preprocessing */
   BEGIN_PREPROCESSING
   if (x[0] == x[1]) {
      k = 2;
      ell = 1;
   }
   else {
      k = 1;
      ell = 2;
   }
   count = 0;
   END_PREPROCESSING
   /* Searching */
   BEGIN_SEARCHING
   j = 0;
   while (j <= n - m)
      if (x[1] != y[j + 1])
         j += k;
      else {
         if (memcmp(x + 2, y + j + 2, m - 2) == 0 &&
             x[0] == y[j])
            OUTPUT(j);
         j += ell;
      }
   END_SEARCHING
   return count;
}


