/*  
 Copyright (c) 2011 Simone Faro and Thierry Lecroq. All rights reserved.
 faro@dmi.unict.it and thierry.lecroq@univ-rouen.fr
 
 This code is intended to work using the smart tool ( http://www.dmi.unict.it/~faro/smart/ )  
 See the documentation at http://www.dmi.unict.it/~faro/smart/howto.php for
 instructions on how to add new codes. 
 
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
 */

#include "include/define.h"
#include "include/main.h"


int search(unsigned char *x, int m, unsigned char *y, int n) {
   int B[SIGMA], W[SIGMA], hbcr[SIGMA], hbcl[SIGMA];
   int i, j, s,s1,s2,s3,s4, f,d, z,first, count, stop;
   int plen = m;
   if(m>32) m=32;
   int m1 = m-1;
   int mp1 =  m+1;   

   /* Pre processing */
   BEGIN_PREPROCESSING
   for(i=0; i<SIGMA; i++) B[i]=W[i]=0;
   s=1;
   f=1<<(m-1);
   for (i=m-1; i>=0; i--) {
		B[x[i]] |= s;
		W[x[i]] |= f;
		s <<= 1;
      f >>= 1;
   }
   
	for (i=0;i<SIGMA;i++)   hbcr[i]=hbcl[i]=2*m;
   for (i=0;i<m;i++) {
       hbcr[x[i]]=(2*m)-i-1;
       hbcl[x[m-1-i]]=(2*m)-i-1;
   }
   END_PREPROCESSING

   /* Searching phase */
   BEGIN_SEARCHING
   int q = n/2;
   s1=m-1; s2=q-m; s3 = q; s4=n-m; count=0;
   while (s1<=s2+m1 || s3<=s4+m1){
         while ((d = (B[y[s1]]|W[y[s2]]|B[y[s3]]|W[y[s4]])) == 0) {
         s1 += hbcr[y[s1+m]];
         s2 -= hbcl[y[s2-m]];
         s3 += hbcr[y[s3+m]];
         s4 -= hbcl[y[s4-m]];
      }
      first=s1-m1; 
		do d = (d<<1) & (B[y[--s1]]|W[y[++s2]]|B[y[--s3]]|W[y[++s4]]);  while (d);
      if(s1<first) {
			s1++;
			s2--;
			s3++;
			s4--;
			i=0;
			while(i<plen && x[i]==y[s1+i]) i++;
			if(i==plen && s1+m1<s2) count++;
			i=0;
			while(i<plen && x[i]==y[s2-m1+i]) i++;
			if(i==plen && s1+m1<=s2) count++;
			i=0;
			while(i<plen && x[i]==y[s3+i]) i++;
			if(i==plen && s3+m1<s4) count++;
			i=0;
			while(i<plen && x[i]==y[s4-m1+i]) i++;
			if(i==plen && s3+m1<=s4) count++;
      }
      s1+=m;
      s2-=m;
      s3+=m;
      s4-=m;
   }
   END_SEARCHING
      return count;
}



