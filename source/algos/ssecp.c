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
#include "include/log2.h"
#include <assert.h>
#include <nmmintrin.h>

  // BEWARE: code sometimes has access to load bytes after end of string.

const int max_needle = 8;

  // pattern up to length max_needle using raw intel sse4.2 instructions: should max_needle be longer?
int search_rawsse(unsigned char *x, int m, unsigned char *y, int n) 
{
    if (m > max_needle)
	return -1;

    int occurences = 0;
    __m128i needle_reg = _mm_loadu_si128((__m128i *)x);

    int step = 16 - m + 1; // experiment with aligned text blocks: step = 8.
    unsigned int result_mask = (1 << step) - 1;

    int number_of_steps = (n + step - 16) / step;
    int steps_size = number_of_steps * step;
    unsigned char *steps_end = y + steps_size;

    while (y != steps_end) { // full 16 bytes 
        __m128i haystack_reg = _mm_loadu_si128((__m128i *)y);
        __m128i mask_reg = _mm_cmpestrm(needle_reg, m, haystack_reg, 16, SIDD_UBYTE_OPS | SIDD_CMP_EQUAL_ORDERED);
        occurences += _mm_popcnt_u32(*(int *)&mask_reg & result_mask);
        y += step;
    }

    n -= steps_size; // remainder under 16 bytes
    if (n >= m) {
        __m128i haystack_reg = _mm_loadu_si128((__m128i *)y);
        __m128i mask_reg = _mm_cmpestrm(needle_reg, m, haystack_reg, n, SIDD_UBYTE_OPS | SIDD_CMP_EQUAL_ORDERED);
        step = n - m + 1;
        result_mask = (1 << step) - 1;
        occurences += _mm_popcnt_u32(*(int *)&mask_reg & result_mask);
    }

    return occurences;
}

/* copied from tw.c */
/* Computing of the maximal suffix for <= */
int maxSuf(char *x, int m, int *p) {
   int ms, j, k;
   char a, b;

   ms = -1;
   j = 0;
   k = *p = 1;
   while (j + k < m) {
      a = x[j + k];
      b = x[ms + k];
      if (a < b) {
         j += k;
         k = 1;
         *p = j - ms;
      }
      else
         if (a == b)
            if (k != *p)
               ++k;
            else {
               j += *p;
               k = 1;
            }
         else { /* a > b */
            ms = j;
            j = ms + 1;
            k = *p = 1;
         }
   }
   return(ms);
}
 
/* copied from tw.c */
/* Computing of the maximal suffix for >= */
int maxSufTilde(char *x, int m, int *p) {
   int ms, j, k;
   char a, b;

   ms = -1;
   j = 0;
   k = *p = 1;
   while (j + k < m) {
      a = x[j + k];
      b = x[ms + k];
      if (a > b) {
         j += k;
         k = 1;
         *p = j - ms;
      }
      else
         if (a == b)
            if (k != *p)
               ++k;
            else {
               j += *p;
               k = 1;
            }
         else { /* a < b */
            ms = j;
            j = ms + 1;
            k = *p = 1;
         }
   }
   return(ms);
}

/* copied from kmp.c */
void preKmp(unsigned char *x, int m, int kmpNext[]) {
   int i, j;
   i = 0;
   j = kmpNext[0] = -1;
   while (i < m) {
      while (j > -1 && x[i] != x[j])
         j = kmpNext[j];
      i++;
      j++;
      if (i<m && x[i] == x[j])
         kmpNext[i] = kmpNext[j];
      else
         kmpNext[i] = j;
   }
}

/* compute period length and critical factorization */
/* if CP does not find exact period length then use KMP */
void  compute(unsigned char* x, int m, int *mu, int *pi)
{
   int i, j, ell, p, per, q, kmpNext[XSIZE];

   /* Preprocessing */
   i = maxSuf(x, m, &p);
   j = maxSufTilde(x, m, &q);
   if (i > j) {
      ell = i;
      per = p;
   } else {
      ell = j;
      per = q;
   }

   *mu = ell+1;

   if (!memcmp(x,x + per, m-per)) {
       *pi = per;
   } else {
       int kmpNext[XSIZE];
       preKmp(x, m, kmpNext);
       *pi = m-kmpNext[m];
   }
}

// algorithm is not exactly like paper- allow overflow anchor match.
// todo: double up loops to avoid the one comparison for the length 

int search(unsigned char *x, int m, unsigned char *y, int n) 
{
    if (m <= max_needle) // raw SSE instructions for short patterns
	return search_rawsse(x, m, y, n);

    int mu, pi, count = 0;
    compute(x, m, &mu, &pi);

	// safety - remove?
    assert( (mu > 0 && mu < pi && mu < m  && pi > 0 && pi <= m) );

      // SSE instruction search anchor block after critical factorization; maximum length needle_length
    __m128i needle_reg = _mm_loadu_si128((__m128i *)&x[mu]);
    int needle_length = m - mu;
    if (needle_length > 16)
	needle_length = 16;

loop:  // optimize further !
    while (n >= m) {
	  // search for first part immediately after critical factorization to anchor
        __m128i haystack_reg = _mm_loadu_si128((__m128i *)&y[mu]);

	int haystack_length = n - mu;
	if (haystack_length > 16)
		haystack_length = 16; 

        int idx = _mm_cmpestri(needle_reg, needle_length, haystack_reg, haystack_length, SIDD_UBYTE_OPS | SIDD_CMP_EQUAL_ORDERED);
	if (!_mm_cmpestrc(needle_reg, needle_length, haystack_reg, haystack_length, SIDD_UBYTE_OPS | SIDD_CMP_EQUAL_ORDERED)) {
		y += haystack_length;
		n -= haystack_length;

		goto loop; 
	}

	y += idx;
	n -= idx;
	if (n < m) // anchor at idx; break loop;
		break; 

	int head0 = 0, head;

	  // compare remainder after critical factorization and after anchor.
	if (needle_length <= haystack_length - idx) {
		  // full occurrence of needle
		head = mu + needle_length; 
	} else {
		  // overflowing occurrence of needle: idx < haystack_length
		head = mu + haystack_length - idx; 
	}

loop1:
	  // compare remainder as above; or after shift by period with head = max(mu,m-pi)
	  // this is memcmp with index of first mismatch.
	while (head < m) {
		__m128i b0 = _mm_loadu_si128((__m128i *)&x[head]), 
			b1 = _mm_loadu_si128((__m128i *)&y[head]);

		unsigned int b = m - head;

		unsigned int idx = _mm_cmpestri(b0, b, b1, b, SIDD_UBYTE_OPS | SIDD_CMP_EQUAL_EACH | SIDD_NEGATIVE_POLARITY);
		if (_mm_cmpestrc(b0, b, b1, b, SIDD_UBYTE_OPS | SIDD_CMP_EQUAL_EACH | SIDD_NEGATIVE_POLARITY)) {
 		          // mismatch 
			head += idx + 1;
			int step = head - mu;
			y += step;
			n -= step;

			goto loop; // continue main loop
		}

		head += idx;
/*
		if (b > 16) 
			head += 16;
		else
			head += b;
*/
	}

loop2:
	  // now compare prefix up to mu: this is just memcmp.
	while (head0 < mu) {
		__m128i b0 = _mm_loadu_si128((__m128i *)&x[head0]), 
			b1 = _mm_loadu_si128((__m128i *)&y[head0]);

		unsigned int b = mu - head0;

		unsigned int idx = _mm_cmpestri(b0, b, b1, b, SIDD_UBYTE_OPS | SIDD_CMP_EQUAL_EACH | SIDD_NEGATIVE_POLARITY);
		if (_mm_cmpestrc(b0, b, b1, b, SIDD_UBYTE_OPS | SIDD_CMP_EQUAL_EACH | SIDD_NEGATIVE_POLARITY)) {
 		          // mismatch 
			goto loop3;
		}

		head0 += idx;
/*
		if (b > 16) 
			head0 += 16;
		else
			head0 += b;
*/
	}

	  // first part match as well: got an OCCURRENCE !
	count++;

loop3:
	  // shift by pi
	y += pi;
	n -= pi;
	head = m - pi;
	if (head < mu) { // odd: can do better probably
		head0 = head;
		head = mu;
	}
	
	goto loop1;
    }

    return ++count;
}
