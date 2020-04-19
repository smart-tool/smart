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
 * This is an implementation of the Quick Search algorithm
 * in D. M. Sunday. A very fast substring search algorithm. Commun. ACM, vol.33, n.8, pp.132--142, (1990).
 */

#include "include/define.h"
#include "include/main.h"

void preQsBc(unsigned char *P, int m, int qbc[]) {
   int i;
   for(i=0;i<SIGMA;i++)   qbc[i]=m+1;
   for(i=0;i<m;i++) qbc[P[i]]=m-i;
}

int search(unsigned char *P, int m, unsigned char *T, int n) {
   int i, s, count, qsbc[SIGMA];
   
   /* Preoprocessing */
   BEGIN_PREPROCESSING
   preQsBc(P, m, qsbc);
   END_PREPROCESSING
	
   /* Searching */
   BEGIN_SEARCHING
   s = 0;
   count = 0;
   while(s<=n-m) {
      i=0;
      while(i<m && P[i]==T[s+i]) i++;
      if(i==m) count++;
      s+=qsbc[T[s+m]];
   }
   END_SEARCHING
   return count;
}
