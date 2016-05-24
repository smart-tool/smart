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
 * This is an implementation of the Fast Search algorithm
 * in D. Cantone and S. Faro. 
 * Fast-Search Algorithms: New Efficient Variants of the Boyer-Moore Pattern-Matching Algorithm. 
 * J. Autom. Lang. Comb., vol.10, n.5/6, pp.589--608, (2005).
 */

#include "include/define.h"
#include "include/main.h"

void Pre_GS(unsigned char *x, int m, int bm_gs[]) {
   int i, j, p, f[XSIZE];
   for(i=0;i<XSIZE;i++) bm_gs[i]=0;
   f[m]=j=m+1;
   for (i=m; i > 0; i--) {
      while (j <= m && x[i-1] != x[j-1]) {
         if (bm_gs[j] == 0) bm_gs[j]=j-i;
         j=f[j];
      }
      f[i-1]=--j;
   }
   p=f[0];
   for (j=0; j <= m; ++j) {
      if (bm_gs[j] == 0) bm_gs[j]=p;
      if (j == p) p=f[p];
   }
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int a,i, j, k, s, count, bc[SIGMA], gs[XSIZE];
   char ch = x[m-1];

   /* Preprocessing */
   BEGIN_PREPROCESSING
   for (a=0; a < SIGMA; a++) bc[a]=m;
   for (j=0; j < m; j++) bc[x[j]]=m-j-1;
   Pre_GS(x, m, gs);
   for(i=0; i<m; i++) y[n+i]=ch;
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   if( !memcmp(x,y,m) ) count++; 
   s=m;
   while(s<n) {
      while((k=bc[y[s]]))   s += k;
      j=2;
      while (j<=m && x[m-j]==y[s-j+1]) j++;
      if( j>m && s<n) count++;
      s += gs[m-j+1];
   }
   END_SEARCHING
   return count;
}
