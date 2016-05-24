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
 * This is an implementation of the Backward Fast Search algorithm
 * in D. Cantone and S. Faro. 
 * Fast-Search Algorithms: New Efficient Variants of the Boyer-Moore Pattern-Matching Algorithm. 
 * J. Autom. Lang. Comb., vol.10, n.5/6, pp.589--608, (2005).
 */

#include "include/define.h"
#include "include/main.h"

void PreBFS(unsigned char *x, int m, int bm_gs[XSIZE][SIGMA]) {
   int i, j, p, c, h, last, suffix_len, temp[XSIZE];
   suffix_len = 1;
   last = m-1;
   for(i=0;i<=m;i++) for(j=0; j<SIGMA;j++) bm_gs[i][j] = m;
   for(i=0; i<=m; i++) temp[i]=-1;
   for(i=m-2; i>=0; i--)
   if(x[i]==x[last]) {
      temp[last]=i;
      last = i;
   }
   suffix_len++;
   while(suffix_len<=m) {
      last = m-1;
      i = temp[last];
      while(i>=0) {
         if(i-suffix_len+1>=0) {
            if(x[i-suffix_len+1]==x[last-suffix_len+1]) {
               temp[last]=i;
               last=i;
            }
            if(bm_gs[m-suffix_len+1][x[i-suffix_len+1]]>m-1-i) bm_gs[m-suffix_len+1][x[i-suffix_len+1]]=m-1-i;
         }
         else {
            temp[last]=i;
            last = i;
            for(c=0; c<SIGMA; c++)
            if(bm_gs[m-suffix_len+1][c]>m-1-i) bm_gs[m-suffix_len+1][c]=m-1-i;
         }
         i = temp[i];
      }
      temp[last]=-1;
      suffix_len++;
   }
}

int search(unsigned char *x, int m, unsigned char *y, int n)
{
   int i, j, k, s, count, gs[XSIZE][SIGMA], bc[SIGMA];
   int *last, first;
   char ch = x[m-1];

   /* Preprocessing */    
   BEGIN_PREPROCESSING
   for(i=0;i<SIGMA;i++) bc[i]=m;
   for(i=0;i<m;i++) bc[x[i]]=m-i-1;
   for(i=0;i<m;i++) y[n+i]=x[i];
   PreBFS(x, m, gs);
   END_PREPROCESSING

   /* Searching */   
   BEGIN_SEARCHING
   count = 0;
   s=m-1;
   last = gs[m];
   first = gs[1][x[0]];
   while(s<n) {
      while((k=bc[y[s]])!=0)   s = s + k;
      for(j=s-1, k=m-1; k>0 && x[k-1]==y[j]; k--, j--);
      if( k==0 ) {
         if(s<n) count++;
         s += first;
      }
      else s += gs[k][y[j]];
   }
   END_SEARCHING
   return count;
}


