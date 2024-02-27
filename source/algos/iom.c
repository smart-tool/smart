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
 * This is an implementation of the Improved Occurrence Heuristic algorithm (IOH)
 * in Simone Faro and Domenico Cantone, Improved and Self-Tuned Occurrence Heuristics,
 * Proceedings of the Prague Stringology Conference 2013, PSC 2013, pp.92-106 (2013)
 */

#include "include/define.h"
#include "include/main.h"

void PrecomputeIOH(unsigned char *p, int m, int step, int ibc[]) {
	for(int i=0; i<SIGMA; i++) ibc[i] = step + 1;
	for(int i=0; i<m; i++) ibc[p[i]] = step - i;
}

int search(unsigned char *P, int m, unsigned char *T, int n) {
   int i, s, count, step1, step2, ibc1[SIGMA], ibc2[SIGMA];
   
   BEGIN_PREPROCESSING
   step1 = step2 = 2*m-1;
   for(i=0; i<m-1; i++)
      if(P[i]==P[m-1]) step1 = 2*m-i-2;
      else step2 = 2*m-i-2;
   PrecomputeIOH(P, m, step1, ibc1);
   PrecomputeIOH(P, m, step2, ibc2);
   END_PREPROCESSING

   BEGIN_SEARCHING
   s = 0;
   count = 0;
   while(s<=n-m) {
      if(P[m-1] == T[s+m-1]) {
	      i=0;
   	      while(i<m && P[i]==T[s+i]) i++;
          if(i==m) count++;
          s+=ibc1[T[s+step1]];
      }
      else s+=ibc2[T[s+step2]];
   }
   END_SEARCHING
   return count;
}
