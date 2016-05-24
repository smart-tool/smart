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
 * This is an implementation of the Horspool algorithm using probabilities * in M. E. Nebel. 
 * Fast string matching by using probabilities: on an optimal mismatch variant of Horspool's algorithm. 
 * Theor. Comput. Sci., vol.359, n.1, pp.329--343, Elsevier Science Publishers Ltd., Essex, UK, (2006).
 * 
 * Note: probability of characters in the text are computed dynamically testing the first 100 chars of the text
 */

#include "include/define.h"
#include "include/main.h"

int search(unsigned char *x, int m, unsigned char *y, int n)
{
   int i, j, s, tmp, count, hbc[SIGMA], v[XSIZE], FREQ[SIGMA];
   /* Computing the frequency of characters */
   for(i=0; i<SIGMA; i++)	FREQ[i] = 0;
   for(i=0; i<100; i++) FREQ[y[i]]++;
   
   /* Preprocessing */
   BEGIN_PREPROCESSING
   for(i=0; i<m; i++) v[i]=i;
   for(i=m-1; i>0; i--)
      for(j=0; j<i; j++)
         if(FREQ[x[v[j]]]>FREQ[x[v[j+1]]]) {   
            tmp = v[j+1];
            v[j+1] = v[j];
            v[j] = tmp;
         }
   for(i=0;i<SIGMA;i++)   hbc[i]=m;
   for(i=0;i<m-1;i++) hbc[x[i]]=m-i-1;
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   s = 0;
   count = 0;
   while(s<=n-m) {
      i=0;
      while(i<m && x[v[i]]==y[s+v[i]]) i++;
      if(i==m) count++;
      s+=hbc[y[s+m-1]];
   }
   END_SEARCHING
   return count;
}
