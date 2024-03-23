/*
 Copyright (c) 2011 Simone Faro and Thierry Lecroq. All rights reserved.
 faro@dmi.unict.it and thierry.lecroq@univ-rouen.fr

 This code is intended to work using the smart tool (
 http://www.dmi.unict.it/~faro/smart/ ) See the documentation at
 http://www.dmi.unict.it/~faro/smart/howto.php for instructions on how to add
 new codes.

 Redistribution and use of the source code, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions must reproduce the above copyright notice, this
 list of conditions and the following disclaimer in the  documentation
 and/or other materials provided with the distribution.  Redistributions of
 source code must also reproduce this information in the source code itself.

 2. If the program is modified, redistributions must include a notice
 (in the same places as above) indicating that the redistributed program is
 not identical to the version distributed by the original creator.

 3. The name of the original creator may not be used to endorse or
 promote products derived from this software without specific prior written
 permission.

 We also request that use of this software be cited in publications as

 Simone Faro and Thierry Lecroq,
 "Multiple Sliding Windows Algorithms for Searching Texts on Large Alphabets"
 SEA 2012 - 11th International Symposium on Experimental Algorithms

 THIS SOFTWARE IS PROVIDED BY THE ORIGINAL AUTHOR ``AS IS'' AND  ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE  IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  ARE
 DISCLAIMED. IN NO EVENT SHALL THE ORIGINAL AUTHOR BE LIABLE  FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL  DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS  OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)  HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.
 *
 * Constraints: requires m>=6
 * Buffer overflows: bin/asan/fsbndm-w6 aaaaaa 6 aaaaaaaaaa 10
 */

#include <assert.h>
#include "include/define.h"
#include "include/main.h"
//#include "include/search_large.h"
#include "include/search_small.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
  unsigned int B[SIGMA], W[SIGMA], d, set, hbcr[SIGMA], hbcl[SIGMA];
  int i, j, s1, s2, s3, s4, s5, s6, pos, mm1, count;

  /* Preprocessing */
  int plen = m;
  if (m > 31)
    m = 31;
    //return search_large(x, m, y, n);;
  if (m < 6)
    return search_small(x, m, y, n);;
  BEGIN_PREPROCESSING
  count = 0;
  mm1 = m - 1;
  //mp1 = m + 1;
  set = 1;
  for (i = 0; i < SIGMA; i++)
    B[i] = W[i] = set;
  for (i = 0; i < m; ++i) {
    B[x[i]] |= (1U << (m - i));
    W[x[m - 1 - i]] |= (1U << (m - i));
  }

  for (i = 0; i < SIGMA; i++)
    hbcr[i] = hbcl[i] = 2 * m;
  for (i = 0; i < m; i++) {
    hbcr[x[i]] = (2 * m) - i - 1;
    hbcl[x[m - 1 - i]] = (2 * m) - i - 1;
  }
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  int q = n / 3;
  s1 = mm1;
  s2 = q - m;
  s3 = q;
  s4 = 2 * q - m;
  s5 = 2 * q;
  s6 = n - plen;
  while (s1 <= s2 + mm1 || s3 <= s4 + mm1 || s5 <= s6 + mm1) {
    assert(s1 + 1 <= n);
    assert(s2 <= n);
    assert(s3 + 1 <= n);
    assert(s4 <= n);
    assert(s5 + 1 <= n);
    assert(s6 <= n);
    assert(s2 > 0);
    assert(s4 > 0);
    assert(s6 > 0);
    while ((d = (((B[y[s1 + 1]] << 1) & B[y[s1]]) |
                 ((W[y[s2 - 1]] << 1) & W[y[s2]]) |
                 ((B[y[s3 + 1]] << 1) & B[y[s3]]) |
                 ((W[y[s4 - 1]] << 1) & W[y[s4]]) |
                 ((B[y[s5 + 1]] << 1) & B[y[s5]]) |
                 ((W[y[s6 - 1]] << 1) & W[y[s6]]))) == 0) {
      assert(s1 + m <= n);
      assert(s2 - m >= 0);
      s1 += hbcr[y[s1 + m]];
      s2 -= hbcl[y[s2 - m]];
      s3 += hbcr[y[s3 + m]];
      s4 -= hbcl[y[s4 - m]];
      s5 += hbcr[y[s5 + m]];
      s6 -= hbcl[y[s6 - m]];
      assert(s1 + 1 <= n);
      assert(s3 + 1 <= n);
      assert(s5 + 1 <= n);
      assert(s2 > 0);
      assert(s4 > 0);
      assert(s6 > 0);
    }
    pos = s1;
    while ((d = (d + d) & (B[y[s1 - 1]] | W[y[s2 + 1]] | B[y[s3 - 1]] |
                           W[y[s4 + 1]] | B[y[s5 - 1]] | W[y[s6 + 1]]))) {
      --s1;
      ++s2;
      --s3;
      ++s4;
      --s5;
      ++s6;
      assert(s1 > 0);
      assert(s2 + 1 <= n);
      assert(s3 > 0);
      assert(s4 + 1 <= n);
      assert(s5 > 0);
      assert(s6 + 1 <= n);
    }
    s1 += mm1;
    s2 -= mm1;
    s3 += mm1;
    s4 -= mm1;
    s5 += mm1;
    s6 -= mm1;
    if (s1 == pos) {
      i = 0;
      j = s1 - mm1;
      assert(j + plen <= n);
      while (i < plen && x[i] == y[j + i])
        i++;
      if (i == plen && s1 <= s2 + mm1)
        OUTPUT(j);
      i = 0;
      assert(s2 + plen <= n);
      while (i < plen && x[i] == y[s2 + i])
        i++;
      if (i == plen && s1 < s2 + mm1)
        OUTPUT(s2);
      i = 0;
      j = s3 - mm1;
      assert(j + plen <= n);
      while (i < plen && x[i] == y[j + i])
        i++;
      if (i == plen && s3 <= s4 + mm1)
        OUTPUT(j);
      i = 0;
      assert(s4 + plen <= n);
      while (i < plen && x[i] == y[s4 + i])
        i++;
      if (i == plen && s3 < s4 + mm1)
        OUTPUT(s4);
      i = 0;
      j = s5 - mm1;
      assert(j + plen <= n);
      while (i < plen && x[i] == y[j + i])
        i++;
      if (i == plen && s5 <= s6 + mm1)
        OUTPUT(j);
      i = 0;
      assert(s6 + plen <= n);
      while (i < plen && x[i] == y[s6 + i])
        i++;
      if (i == plen && s5 < s6 + mm1)
        OUTPUT(s6);
      ++s1;
      --s2;
      ++s3;
      --s4;
      ++s5;
      --s6;
    }
  }
  END_SEARCHING
  return count;
}
