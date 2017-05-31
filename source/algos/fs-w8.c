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

void Pre_GS(unsigned char *x, int m, int bm_gs[]) {
   int i, j, p, f[XSIZE];
   for(i=0;i<XSIZE;i++) bm_gs[i]=0;
   f[m]=j=m+1;
   for (i=m; i > 0; i--) {
      while (j <= m && x[i-1] != x[j-1]) {
         if (bm_gs[j] == 0) bm_gs[j]=j-i;
         j=f[j];
      }
      f[i-1]=--j;
   }
   p=f[0];
   for (j=0; j <= m; ++j) {
      if (bm_gs[j] == 0) bm_gs[j]=p;
      if (j == p) p=f[p];
   }
}

int search(unsigned char *P, int m, unsigned char *T, int n)
{
	int i, j,s1,s2,s3,s4,s5,s6,s7,s8,k1,k2,k3,k4,k5,k6,k7,k8,i1,i2,i3,i4,h, count;
    int l1,l2,l3,l4,l5,l6,l7,l8;
	int hbcr[SIGMA], hbcl[SIGMA], gsR[XSIZE], gsL[XSIZE];
   unsigned char c, Pr[XSIZE];

   /* proprocessing */
   BEGIN_PREPROCESSING
	for(i=0;i<SIGMA;i++) hbcr[i]=m;
   for(i=0;i<m;i++) hbcr[P[i]] = (m-i-1);

   for(i=0;i<SIGMA;i++) hbcl[i]=m;
   for(i=0;i<m;i++) hbcl[P[m-1-i]] = (m-i-1);

   for(i=0; i<m; i++) Pr[i]=P[m-1-i]; Pr[m]='\0';
	Pre_GS(P, m, gsR);
	Pre_GS(Pr, m, gsL);

   for(i=0; i<m; i++) T[n+i]=P[i];
   int mm1 = m-1;
   END_PREPROCESSING
	
   /* searching */
   BEGIN_SEARCHING
   int q = n/4;
	s1 = mm1; s2 = q-m; s3=q; s4=(2*q)-m; s5=2*q; s6=(3*q)-m; s7=3*q; s8=n-m;
    if(s2==s1-mm1) s2++;
    s3=s2+m;
    if(s4<=s3-mm1) s4=s3-mm1+1;
    s5=s4+m;
    if(s6<=s5-mm1) s6=s5-mm1+1;
    s7=s6+m;
	count = 0;
   k1=hbcr[T[s1]];
   k2=hbcl[T[s2]];
   k3=hbcr[T[s3]];
   k4=hbcl[T[s4]];
   k5=hbcr[T[s5]];
   k6=hbcl[T[s6]];
   k7=hbcr[T[s7]];
   k8=hbcl[T[s8]];
    l1 = s1-mm1;
    l3 = s3-mm1;
    l5 = s5-mm1;
    l7 = s7-mm1;
    l2 = s2;
    l4 = s4;
    l6 = s6;
    l8 = s8;
   while(s1<=s2+mm1 || s3<=s4+mm1 || s5<=s6+mm1 || s7<=s8+mm1) {
      if(!k1) { 
         j = mm1; k1=s1-mm1;
         while(j>=0 && P[j]==T[k1+j]) j--;
          if(j<0 && k1<l2) {
              l1=k1;
              count++;
          }
         s1+=gsR[j+1];
      }
      if(!k2) { 
         i = 0; 
         while(i<m && P[i]==T[s2+i]) i++;
          if(i==m && s2>l1) {
              l2=k2;
              count++;
          }
         s2-=gsL[m-i];
      }
      if(!k3) { 
         j = mm1; k3=s3-mm1;
         while(j>=0 && P[j]==T[k3+j]) j--;
          if(j<0 && k3<l4) {
              l3=k3;
              count++;
          }
         s3+=gsR[j+1];
      }
      if(!k4) { 
         i = 0; 
         while(i<m && P[i]==T[s4+i]) i++;
          if(i==m && s4>l3) {
              l4=k4;
              count++;
          }
         s4-=gsL[m-i];
      }
      if(!k5) { 
         j = mm1; k5=s5-mm1;
         while(j>=0 && P[j]==T[k5+j]) j--;
          if(j<0 && k5<l6) {
              l5=k5;
              count++;
          }
         s5+=gsR[j+1];
      }
      if(!k6) {
         i = 0; 
         while(i<m && P[i]==T[s6+i]) i++;
          if(i==m && s6>l5) {
              l6=k6;
              count++;
          }
         s6-=gsL[m-i];
      }
      if(!k7) { 
         j = mm1; k7=s7-mm1;
         while(j>=0 && P[j]==T[k7+j]) j--;
          if(j<0 && k7<l8) {
              l7=k7;
              count++;
          }
         s7+=gsR[j+1];
      }
      if(!k8) { 
         i = 0; 
         while(i<m && P[i]==T[s8+i]) i++;
          if(i==m && s8>l7) {
              l8=k8;
              count++;
          }
         s8-=gsL[m-i];
      }
      while( (k1=hbcr[T[s1]]) && (k2=hbcl[T[s2]])  && (k3=hbcr[T[s3]]) && (k4=hbcl[T[s4]])
          && (k5=hbcr[T[s5]]) && (k6=hbcl[T[s6]]) && (k7=hbcr[T[s7]]) && (k8=hbcl[T[s8]]) ) {
         s1+=k1; 
         s2-=k2; 
         s3+=k3; 
         s4-=k4; 
         s5+=k5;
         s6-=k6;
         s7+=k7; 
         s8-=k8; 
      }
   }
   END_SEARCHING
    T[n] = '\0';
   return count;
}
