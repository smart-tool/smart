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
 * This is an implementation of the Wu Manber algorithm for Single Pattern Matching
 * in T. Lecroq. Fast exact string matching algorithms. 
 * Information Processing Letters, vol.102, n.6, pp.229--235, Elsevier North-Holland, (2007).
 */

#include "include/define.h"
#include "include/main.h"
#define RANK8 8

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, sh, shift[WSIZE], sh1, mMinus1, mMinus7, count;
   unsigned int h;
   if(m<8) return -1;

   /* Preprocessing */
   BEGIN_PREPROCESSING   
   count = 0;
   mMinus1 = m-1;
   mMinus7 = m-7;
   for (i = 0; i < WSIZE; ++i)
      shift[i] = mMinus7;

   h = x[0];
   h = ((h<<1) + x[1]);
   h = ((h<<1) + x[2]);
   h = ((h<<1) + x[3]);
   h = ((h<<1) + x[4]);
   h = ((h<<1) + x[5]);
   h = ((h<<1) + x[6]);
   h = ((h<<1) + x[7]);
   shift[h%WSIZE] = m-RANK8;
   for (i=RANK8; i < mMinus1; ++i) {
      h = x[i-7];
      h = ((h<<1) + x[i-6]);
      h = ((h<<1) + x[i-5]);
      h = ((h<<1) + x[i-4]);
      h = ((h<<1) + x[i-3]);
      h = ((h<<1) + x[i-2]);
      h = ((h<<1) + x[i-1]);
      h = ((h<<1) + x[i]);
      shift[h%WSIZE] = mMinus1-i;
   }
   h = x[i-7];
   h = ((h<<1) + x[i-6]);
   h = ((h<<1) + x[i-5]);
   h = ((h<<1) + x[i-4]);
   h = ((h<<1) + x[i-3]);
   h = ((h<<1) + x[i-2]);
   h = ((h<<1) + x[i-1]);
   h = ((h<<1) + x[i]);
   sh1 = shift[h%WSIZE];
   shift[h%WSIZE] = 0;
   if(sh1==0) sh1=1;
   END_PREPROCESSING   

   /* Searching */
   BEGIN_SEARCHING
   i = mMinus1;
   memcpy(y+n, x, m);
   while (1) {
      sh = 1;
      while (sh != 0) {
         h = y[i-7];
         h = ((h<<1) + y[i-6]);
         h = ((h<<1) + y[i-5]);
         h = ((h<<1) + y[i-4]);
         h = ((h<<1) + y[i-3]);
         h = ((h<<1) + y[i-2]);
         h = ((h<<1) + y[i-1]);
         h = ((h<<1) + y[i]);
         sh = shift[h%WSIZE];
         i+=sh;
      }
      if (i < n) {
         j=0;
         while(j<m && x[j]==y[i-mMinus1+j]) j++;
         if (j>=m) {
            OUTPUT(i-mMinus1);
         }
         i+=sh1;
      }
      else {
   		END_SEARCHING
      	return count;
      }
   }
}
