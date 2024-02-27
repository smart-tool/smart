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
 * This is an implementation of the BNDM for Long patterns
 * in G. Navarro and M. Raffinot. 
 * Fast and Flexible String Matching by Combining Bit-Parallelism and Suffix Automata. ACM J. Experimental Algorithmics, vol.5, pp.4, (2000).
 */

#include "include/define.h"
#include "include/main.h"

#define CHAR_BIT 8
#define WORD_TYPE unsigned int
#define WORD_BITS (sizeof(WORD_TYPE)*CHAR_BIT)
#define bit_size(bits) (((bits)+WORD_BITS-1)/WORD_BITS)
#define   bit_byte(bit) ((bit) / WORD_BITS)
#define   bit_mask(bit) (1L << ((bit) % WORD_BITS))
#define bit_alloc(bits) calloc(bit_size(bits), sizeof(WORD_TYPE))
#define   bit_set(name, bit) ((name)[bit_byte(bit)] |= bit_mask(bit))
#define   bit_clear(name, bit) ((name)[bit_byte(bit)] &= ~bit_mask(bit))
#define   bit_test(name, bit) ((name)[bit_byte(bit)] & bit_mask(bit))
#define bit_zero(name, bits) memset(name, 0, bit_size(bits) * sizeof(WORD_TYPE))
#define SHIFT_BNDM(a, b, n) ((a << (n)) | (b >> (WORD_BITS-(n))))

static void bit_alloc_n(WORD_TYPE **name, int n, int bits) {
   int i;
   name[0] = calloc(n * bit_size(bits), sizeof(WORD_TYPE));
   for (i = 1; i < n; i++) name[i] = name[0] + i*bit_size(bits);
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int B[SIGMA];
   int i, j, s, D, last, count;

   if (m > 32) return search_large(x,m,y,n);

   /* Preprocessing */
    BEGIN_PREPROCESSING
   for(i=0; i<SIGMA; i++) B[i]=0;
   s=1;
   for (i=m-1; i>=0; i--) {
      B[x[i]] |= s;
      s <<= 1;
   }
    END_PREPROCESSING

   /* Searching */
    BEGIN_SEARCHING
   j=0; count=0;
   while (j <= n-m){
      i=m-1; last=m;
      D = ~0;
      while (i>=0 && D!=0) {
         D &= B[y[j+i]];
         i--;
         if (D != 0) {
            if (i >= 0) last = i+1;
            else count ++;
          }
          D <<= 1;
      }
      j += last;
   }
   END_SEARCHING
   return count;
}

/*
 * Backward Nondeterministic DAWG Matching Long patterns
 * The present implementation uses the multiword implementation of the BNDM algorithm
 */

int search_large(unsigned char *x, int m, unsigned char *y, int n)
{
   int i, j;
   WORD_TYPE *D, H, M;
   WORD_TYPE *B[SIGMA];
   int count = 0;

    BEGIN_PREPROCESSING
   bit_alloc_n(B, SIGMA, m);
   for (i = 0; i < m; i++) bit_set(B[x[m-1-i]], i);
    END_PREPROCESSING

    BEGIN_SEARCHING
   D = bit_alloc(m);
   j = m-1;
   while (j < n) {
      int k = 1;
      int l = 0;
      int x = 0;
      for (i = 0; i < bit_size(m); i++) {
         D[i] = B[y[j]][i];
         if (D[i]) x = 1;
      }
      while (k < m && x) {
         x = 0;
         if (bit_test(D, m-1)) l = k;
         H = 0;
         for (i = 0; i < bit_size(m); i++) {
            M = D[i];
            D[i] = SHIFT_BNDM(D[i], H, 1) & B[y[j-k]][i];
            if (D[i]) x = 1;
            H = M;
         }
         k++;
      }
      if (x) OUTPUT(j-m+1);
      j += m-l;
   }
   free(D);
   free(B[0]);
   END_SEARCHING
   return count;
}


