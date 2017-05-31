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

void TVSBSpreBrBc(unsigned char *x, int m, int brBc[SIGMA][SIGMA]) {
   int a, b, i;
   for (a = 0; a < SIGMA; ++a)
      for (b = 0; b < SIGMA; ++b)
         brBc[a][b] = m + 2;
   for (a = 0; a < SIGMA; ++a)
      brBc[a][x[0]] = m + 1;
   for (i = 0; i < m - 1; ++i)
      brBc[x[i]][x[i + 1]] = m - i;
   for (a = 0; a < SIGMA; ++a)
      brBc[x[m - 1]][a] = 1;
}

 int search(unsigned char *x, int m, unsigned char *y, int n){
   int count,i,s1,s2,s3,s4,s5,s6,s7,s8,j =0;
     int l1,l2,l3,l4,l5,l6,l7,l8;
   int BrBcR[SIGMA][SIGMA], BrBcL[SIGMA][SIGMA];
   unsigned char firstCh, lastCh;
   unsigned char xr[XSIZE];
   unsigned char c;
     if(n<m+2) return -1;
     if(m<2) return -1;

   BEGIN_PREPROCESSING
   for(i=0; i<m; i++) xr[i] = x[m-1-i];
   xr[m]='\0';
   count = 0;
   int mPlus1 = m+1;
   TVSBSpreBrBc(x, m,  BrBcR);
   TVSBSpreBrBc(xr, m, BrBcL);
   END_PREPROCESSING

   BEGIN_SEARCHING
   for(i=0; i<m; i++) y[n+i]=y[n+m+i]=x[i];
   int q = n/4;
   s1 = 0; s2 = q-1; s3 = q; s4 = 2*q-1; s5=2*q; s6=3*q-1; s7=3*q; s8=n-m;
     if(s2>n-m) s2=n-m;
     if(s4>n-m) s4=n-m;
     if(s6>n-m) s6=n-m;
     l1 = s1;
     l3 = s3;
     l2 = s2;
     l4 = s4;
     l5 = s5;
     l6 = s6;
     l7 = s7;
     l8 = s8;
   while(s1<=s2 || s3<=s4 || s5<=s6 || s7<=s8) {
		i=0; c = x[0];
		while(c==y[s1+i] | c==y[s2+i] | c==y[s3+i] | c==y[s4+i] | c==y[s5+i] | c==y[s6+i] | c==y[s7+i] | c==y[s8+i]) c=x[++i];
		if(i>=m) {
            if(s1<l2 && !memcmp(x,y+s1,m)) {
                l1=s1;
                count++;
            }
            if(s2>l1 && !memcmp(x,y+s2,m)) {
                l2=s2;
                count++;
            }
            if(s3<l4 && !memcmp(x,y+s3,m)) {
                l3=s3;
                count++;
            }
            if(s4>l3 && !memcmp(x,y+s4,m)) {
                l4=s4;
                count++;
            }
            if(s5<l6 && !memcmp(x,y+s5,m)) {
                l5=s5;
                count++;
            }
            if(s6>l5 && !memcmp(x,y+s6,m)) {
                l6=s6;
                count++;
            }
            if(s7<l8 && !memcmp(x,y+s7,m)) {
                l7=s7;
                count++;
            }
            if(s8>l7 && !memcmp(x,y+s8,m)) {
                l8=s8;
                count++;
            }
		}
    	s1 += BrBcR[y[s1+m]][y[s1+mPlus1]];
    	s2 -= BrBcL[y[s2-1]][y[s2-2]];
    	s3 += BrBcR[y[s3+m]][y[s3+mPlus1]];
    	s4 -= BrBcL[y[s4-1]][y[s4-2]];
    	s5 += BrBcR[y[s5+m]][y[s5+mPlus1]];
    	s6 -= BrBcL[y[s6-1]][y[s6-2]];
    	s7 += BrBcR[y[s7+m]][y[s7+mPlus1]];
    	s8 -= BrBcL[y[s8-1]][y[s8-2]];
    }
   END_SEARCHING
    return count;
 }
