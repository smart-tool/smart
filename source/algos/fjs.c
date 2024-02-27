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
 * This is an implementation of the FJS algorithm
 * in F. Franek and C. G. Jennings and W. F. Smyth. 
 * A simple fast hybrid pattern-matching algorithm. J. Discret. Algorithms, vol.5, pp.682--695, (2007).
 */

#include "include/define.h"
#include "include/main.h"

void preQsBc(unsigned char *x, int m, int qbc[]) {
   int i;
   for(i=0;i<SIGMA;i++)   qbc[i]=m+1;
   for(i=0;i<m;i++) qbc[x[i]]=m-i;
}

void preKmp(unsigned char *x, int m, int kmpNexy[]) {
   int i, j;
   i = 0;
   j = kmpNexy[0] = -1;
   while (i < m) {
      while (j > -1 && x[i] != x[j])
         j = kmpNexy[j];
      i++;
      j++;
      if (i<m && x[i] == x[j])
         kmpNexy[i] = kmpNexy[j];
      else
         kmpNexy[i] = j;
   }
}

int search( unsigned char *x, int m, unsigned char *y, int n ) {
   int i, s, count, qsbc[SIGMA], kmp[XSIZE];

   /* Preprocessing */
   BEGIN_PREPROCESSING
   preQsBc(x,m,qsbc);
   preKmp(x,m,kmp);
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   s = 0;
   count = 0;
   while(s<=n-m) {
      while(s<=n-m && x[m-1]!=y[s+m-1]) s+=qsbc[y[s+m]];
      if(s>n-m) {
          END_SEARCHING
          return count;
      }
      i=0; 
      while(i<m && x[i]==y[s+i]) i++;
      if(i>=m) count++;
      s+=(i-kmp[i]);
   }
   END_SEARCHING
   return count;
}

