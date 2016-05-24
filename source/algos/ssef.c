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
 * This is an implementation of the SSEF algorithm
 * in M. O. Kulekci. 
 * Filter Based Fast Matching of Long Patterns by Using SIMD Instructions. 
 * Proceedings of the Prague Stringology Conference 2009, pp.118--128, Czech Technical University in Prague, Czech Republic, (2009).
 */

#include <emmintrin.h>
#include "include/define.h"
#include "include/main.h"

typedef union{
   __m128i* data16;
   unsigned char* data;
} TEXT;

typedef struct list {
   struct list *next;
   int pos;
} LIST;


int  search(unsigned char* x, int Plen, unsigned char *y, int Tlen) {
   if(Plen<32) return -1;
   LIST *flist[65536];
   LIST *t;
   memset(flist,0,sizeof(LIST*)*65536);
   __m128i tmp128;
   TEXT T;
   T.data16 = (__m128i*) y;
   T.data = (unsigned char *) y;

   BEGIN_PREPROCESSING
   unsigned int K=7;
   unsigned int count=0;
   unsigned int i,last,j;
   __m128i *ptr16;
   __m128i *lastchunk = &T.data16[Tlen/16];
   unsigned int filter;
   unsigned char* f = malloc(Plen);

   last = (Plen/16) - 1;
   for(i=0;i<Plen;i++){
      f[i] = (x[i]&0x80)>>7;
   }
   count = 15;

   for(i=0;i<last*16;i++) {
      j = last*16-i;
      filter = f[j] + f[j+1]*2     + f[j+2]*4     + f[j+3]*8      + 
                      f[j+4]*16    + f[j+5]*32    + f[j+6]*64     + f[j+7]*128    +
                      f[j+8]*256   + f[j+9]*512   + f[j+10]*1024  + f[j+11]*2048  +
                      f[j+12]*4096 + f[j+13]*8192 + f[j+14]*16384 + f[j+15]*32768 ;
      if (flist[filter]==0){
         flist[filter] = (LIST*)malloc(sizeof(LIST));
         flist[filter]->next = NULL;
         flist[filter]->pos  = i;
      } else {
         t = flist[filter];
         while(t->next!=NULL) t = t->next;
         t->next = (LIST*)malloc(sizeof(LIST));
         t = t->next;
         t->next=NULL;
         t->pos = i;
      }
   }
   END_PREPROCESSING

   BEGIN_SEARCHING
   count=0;
   ptr16 = &T.data16[last];

   while(ptr16<lastchunk) {
      filter = _mm_movemask_epi8(*ptr16);

      if (flist[filter]) {
         i = ((ptr16 - &T.data16[0])-last)*16;
         t = flist[filter];
         while(t) {
            if (memcmp(x,&T.data[i+t->pos],Plen) == 0) count++;
            t=t->next;
         }
      }
      ptr16+=last;
   }
   END_SEARCHING
   return count;
}

