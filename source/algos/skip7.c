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
 */

#include "include/define.h"
#include "include/main.h"
#include "include/AUTOMATON.h"

#define DSIGMA 65536
#define HS(x,i) (x[i]<<6) + (x[i+1]<<5) + (x[i+2]<<4) + (x[i+3]<<3) + (x[i+4]<<2) + (x[i+5]<<1) + x[i+6]
#define Q 7

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, count, h, k;
   List ptr, z[DSIGMA];
   if(m<Q) return -1;

   BEGIN_PREPROCESSING	
   memset(z, 0, DSIGMA*sizeof(List));
   for (i = 0; i < m-Q+1; ++i) {
      ptr = (List)malloc(sizeof(struct _cell));
      if (ptr == NULL)
         error("SKIP");
      ptr->element = i;
      ptr->next = z[HS(x,i)];
      z[HS(x,i)] = ptr;
   }
   END_PREPROCESSING	
  
   BEGIN_SEARCHING
   count = 0;
   int mq = m-Q+1;
   for (j = mq - 1; j < n; j += mq)
      for (ptr = z[HS(y,j)]; ptr != NULL; ptr = ptr->next) 
			if((j-ptr->element) <= n-m) {
				k = 0;
				h = j-ptr->element;
				while(k<m && x[k]==y[h+k]) k++;
				if(k>=m) count++;
    	     }
   END_SEARCHING
   return count;
}
