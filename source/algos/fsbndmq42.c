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
 * This is an implementation of the Forward Semplified BNDM algorithm using q-grams
 * in Hannu Peltola and Jorma Tarhio 	
 * Variations of Forward-SBNDM
 * Proceedings of the Prague Stringology Conference 2011, pp.3--14, Czech Technical University in Prague, Czech Republic, (2008).
 * Q is the dimension of q-grams
 * F is the number of forward characters
 */

#include "include/define.h"
#include "include/main.h"
#define Q 4 
#define F 2 

int search(unsigned char *x, int m, unsigned char *y, int n) {
   unsigned int B[SIGMA], D, set;
   int i, j, pos, mm, sh, m1, count;
   if(m<Q) return -1;
	int plen = m;
	int larger = m+F>WORD? 1:0;
   if(larger) m = WORD-F;
		
   BEGIN_PREPROCESSING
   /* Preprocessing */
   count = 0;
   set = 0;
	for(j=0; j<F; j++) set = (set << 1) | 1;
	for(i=0; i<SIGMA; i++) B[i]=set;
   for (i = 0; i < m; ++i) B[x[i]] |= (1<<(m-i-1+F));
   mm = m-Q+F;
	sh = m-Q+F+1;
	m1 = m-1; 
   END_PREPROCESSING
	
   BEGIN_SEARCHING
   /* Searching */
   if(!memcmp(x,y,m)) OUTPUT(0);
	int end = n-plen+m;
   j = m;
   while (j < end) {
      D = B[y[j+2]];
		D = (D<<1) & B[y[j+1]];
		D = (D<<1) & B[y[j]];
		D = (D<<1) & B[y[j-1]];
      if (D != 0) {
         pos = j;
         while (D = (D<<1) & B[y[j-2]]) --j;
         j += mm;
         if (j == pos) {
				if(larger) {
					i=m;
					while(i<plen && x[i]==y[j-m1+i]) i++;
					if(i==plen) OUTPUT(j);
				}
            else OUTPUT(j);
            ++j;
         }
      }
      else j += sh;
   }
   END_SEARCHING
   return count;
}