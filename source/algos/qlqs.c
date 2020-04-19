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
 * This is an implementation of the Quantum Leap Quick Search algorithm
 * in Bruce W. Watson, Derrick G. Kourie, and Loek Cleophas. 
 * Quantum Leap Pattern Matching: A New High Performance Quick Search-Style Algorithm.
 * The Prague Stringology Conference 2015, pp 104-117 (2015)
 */

#include "include/define.h"
#include "include/main.h"

void reverse(unsigned char *P, int m, unsigned char* R) {
	for(int i=0; i<m; i++) R[i] = P[m-1-i];
}

void preQsBc(unsigned char *P, int m, int qbc[]) {
   int i;
   for(i=0;i<SIGMA;i++)   qbc[i]=m+1;
   for(i=0;i<m;i++) qbc[P[i]]=m-i;
}

int search(unsigned char *P, int m, unsigned char *T, int n) {
   int i, s, k, count, qsf[SIGMA], qsb[SIGMA];
   unsigned char R[XSIZE];
   
   BEGIN_PREPROCESSING
   reverse(P,m,R);
   preQsBc(P, m, qsf);
   preQsBc(R, m, qsb);
   int z = m+m/2;
   END_PREPROCESSING

   BEGIN_SEARCHING
   i = 0;
   count = 0;
   while(i<=n-m) {
      k=0;
      while(k<m && P[k]==T[i+k]) k++;
      if(k==m) count++;
      i += ( qsf[T[i+m]]>z-qsb[T[i+z-1]] ? z : qsf[T[i+m]] );
   }
   END_SEARCHING
   return count;
}
