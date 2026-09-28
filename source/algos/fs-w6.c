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

 Simone Faro ad Thierry Lecroq,
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
 * Constraints: requires n>=8
 * Constraints: needs m space at the end of T
 */

#include "include/define.h"
#include "include/main.h"
#include "include/search_small.h"
#include "include/search_large.h"

void Pre_GS(unsigned char *x, int m, int bm_gs[]) {
  int i, j, p, f[XSIZE];
  for (i = 0; i < XSIZE; i++)
    bm_gs[i] = 0;
  f[m] = j = m + 1;
  for (i = m; i > 0; i--) {
    while (j <= m && x[i - 1] != x[j - 1]) {
      if (bm_gs[j] == 0)
        bm_gs[j] = j - i;
      j = f[j];
    }
    f[i - 1] = --j;
  }
  p = f[0];
  for (j = 0; j <= m; ++j) {
    if (bm_gs[j] == 0)
      bm_gs[j] = p;
    if (j == p)
      p = f[p];
  }
}

int search(unsigned char *P, int m, unsigned char *T, int n) {
  return search_safe(P, m, T, n);
}
