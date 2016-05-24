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
 * This is an implementation of the Bit Parallel Length Invariant Matcher
 * in M. O. Kulekci. 
 * A method to overcome computer word size limitation in bit-parallel pattern matching. 
 * Proceedings of the 19th International Symposium on Algorithms and Computation, ISAAC 2008, 
 * Lecture Notes in Computer Science, vol.5369, pp.496--506, Springer-Verlag, Berlin, Gold Coast, Australia, (2008).
 */

#include "include/define.h"
#include "include/main.h"
#define MAXWSIZE (XSIZE + WORD)

int search(unsigned char* x, int m, unsigned char* y, int n)
{
   int i,j,k,count;
   unsigned int   wsize = WORD - 1 + m;
   unsigned long* M = (unsigned long*)malloc(sizeof(unsigned long)*SIGMA * MAXWSIZE);
   unsigned long  tmp, F;
   unsigned int   ScanOrder[XSIZE];
   unsigned int   MScanOrder[XSIZE];
   unsigned int*  so  = ScanOrder;
   unsigned int*  mso = MScanOrder;
   unsigned int   shift[SIGMA];

   /* Preprocessing */
   BEGIN_PREPROCESSING
   memset(M,0xff,sizeof(unsigned long)*SIGMA*wsize);
    for(i=0;i<WORD;i++){
      tmp = 1 << i;
      for(j=0;j<m;j++){
         for(k=0;k<SIGMA;k++) M[((i+j)*SIGMA) + k] &= ~tmp;
         M[ x[j] + ((i+j)*SIGMA) ]|= tmp;
      }
   }
   
   for(i=0;i<SIGMA;i++) shift[i] = wsize + 1;
   for(i=0;i<m;i++) shift[x[i]] = wsize - i;
   
   for(i=m-1;i>=0;i--){
      k=i;
      while (k<wsize){
         *so=k;
         *mso = SIGMA*k;
         so++;
         mso++;
         k+=m;
      }
   }
   END_PREPROCESSING
   
   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   i = 0;
   F = M[MScanOrder[0]+y[i+ScanOrder[0]]] & M[MScanOrder[1]+y[i+ScanOrder[1]]];
   while(i<n) {
      for(j=2;F && j<wsize;j++){ 
         F &= M[MScanOrder[j]+y[i+ScanOrder[j]]]; 
      }
      if (F) {
         for(j=0;j<WORD;j++) 
            if (F & (1<<j)) 
               if(i+j<=n-m) OUTPUT(i+j);
      }
      i+=shift[y[i+wsize]];
      F = M[MScanOrder[0]+y[i+ScanOrder[0]]] & M[MScanOrder[1]+y[i+ScanOrder[1]]];
   }
   END_SEARCHING
   return count;
}
