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
 * This is an implementation of the Forward Fast Search algorithm
 * in D. Cantone and S. Faro. 
 * Fast-Search Algorithms: New Efficient Variants of the Boyer-Moore Pattern-Matching Algorithm. 
 * J. Autom. Lang. Comb., vol.10, n.5/6, pp.589--608, (2005).
 */

#include "include/define.h"
#include "include/main.h"

void Forward_Suffix_Function(unsigned char *x, int m, int bm_gs[XSIZE][SIGMA], int alpha) {
   int init;
   int i, j, last, suffix_len, temx[XSIZE];
   init = 0;
   for(i=0;i<m;i++) for(j=init; j<init+alpha;j++) bm_gs[i][j] = m+1;
   for(i=0; i<m; i++) temx[i]=i-1;
   for(suffix_len=0; suffix_len<=m; suffix_len++) {
      last = m-1;
      i = temx[last];
      while(i>=0) {
         if( bm_gs[m-suffix_len][x[i+1]]>m-1-i )
            if(i-suffix_len<0 || (i-suffix_len>=0 && x[i-suffix_len]!=x[m-1-suffix_len]))
               bm_gs[m-suffix_len][x[i+1]]=m-1-i;
         if((i-suffix_len >= 0 && x[i-suffix_len]==x[last-suffix_len]) || (i-suffix_len <  0)) {
            temx[last]=i;
            last=i;
         }
         i = temx[i];
      }
      if(bm_gs[m-suffix_len][x[0]] > m) bm_gs[m-suffix_len][x[0]] = m;
      temx[last]=-1;
   }
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, k, s, count, gs[XSIZE][SIGMA], bc[SIGMA];
   char ch = x[m-1];

   /* Preprocessing */
   BEGIN_PREPROCESSING
   Forward_Suffix_Function(x, m, gs, SIGMA);
   for (i=0; i < SIGMA; i++) bc[i]=m;
   for (j=0; j < m; j++) bc[x[j]]=m-j-1;
   for(i=0; i<=m; i++) y[n+i]=ch;
   y[n+m+1]='\0';
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   if( !memcmp(x,y,m) ) count++; 
   s=m;
   while(s<n) {
      while(k=bc[y[s]])   s += k;
      for(j=s-1, k=m-1; k>0 && x[k-1]==y[j]; k--, j--);
      if(!k && s<n) count++;
      s += gs[k][y[s+1]];
   }
   END_SEARCHING
   return count;
}



