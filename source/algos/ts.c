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
 * This is an implementation of the Tailed Substring algorithm
 * in D. Cantone and S. Faro. 
 * Searching for a substring with constant extra-space complexity. 
 * Proc. of Third International Conference on Fun with algorithms, pp.118--131, (2004).
 */

#include "include/define.h"
#include "include/main.h"

int search(unsigned char* x, int m, unsigned char * y, int n) {
   int s, j, i, k, h, dim, count;

   BEGIN_PREPROCESSING
   END_PREPROCESSING
   BEGIN_SEARCHING
   /* Searching */
   count = 0;
   /* phase n.1*/
   s = 0; i=m-1; k=m-1; dim=1;
   while (s<=n-m && i-dim>=0) {
   	  if(x[i]!=y[s+i]) s++;
      else {
      	 for(j=0; j<m && x[j]==y[s+j]; j++);
         if(j==m) count++;
         for(h=i-1; h>=0 && x[h]!=x[i]; h--);
         if(dim<i-h) {k=i; dim=i-h;}
         s+=i-h;
         i--;
      }
   }

   /* phase n.2 */
   while (s <= n - m) {
   	  if(x[k]!=y[s+k]) s++;
      else {
      	 j=0;
         while(j<m && x[j]==y[s+j]) j++;
         if(j==m) count++;
         s+=dim;
      }
   }
   END_SEARCHING
   return count;
}
