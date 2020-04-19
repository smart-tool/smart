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
 * This is an implementation of the Jumping Occurrence Matcher (JOM)
 * in Simone Faro and Domenico Cantone, Improved and Self-Tuned Occurrence Heuristics,
 * Proceedings of the Prague Stringology Conference 2013, PSC 2013, pp.92-106 (2013)
 */

#include "include/define.h"
#include "include/main.h"

/* we suppose an alphabet of SIGMA characters with a distribution equal to the first 100 characters of the pattern */
void computeFreq(unsigned char* T, double freq[]) {
   for(int i=0; i<SIGMA; i++) freq[i] = 0;
   for(int i=0; i<100; i++) freq[T[i]]++;
   for(int i=0; i<SIGMA; i++) freq[i] /= 100.0;
}

int FindJumpDistance(unsigned char *p, int m, int i, double freq[], double bound) {
   int v[SIGMA];
   for(int c=0; c<SIGMA; c++) v[c]=1;
   double frq = 1.0;
   int j = 1;
   while( frq >= bound && j<=i+1 ) {
      if(v[p[i+1-j]]==1) {
         v[p[i+1-j]] = 0;
         frq -= freq[p[i+1-j]];
      }
      j = j+1;
   }
   return j-1;
}

void PrecomputeJOH(unsigned char *p, int m, int i, int j, int jbc[SIGMA][SIGMA]) {
	for(int a=0; a<SIGMA; a++)
	   for(int b=0; b<SIGMA; b++)
	      jbc[a][b] = i + j + 1;
	      
	for(int a=0; a<SIGMA; a++) 
	   for(int k=0; k<j; k++)
	      jbc[a][p[k]] = i + j - k;
	      
    for(int k=0; k<i-j; k++)
	   jbc[p[k]][p[k+j]] = i - k;
	   
    for(int k=i-j; k<=i-1; k++)
	   for(int a=0; a<SIGMA; a++) 
	      jbc[p[k]][a] = i - k;

}

int FindWorstOccurrence(unsigned char *p, int m, double freq[]) {
	int lp[SIGMA], gbc;
	for(int i=0; i<SIGMA; i++) lp[i] = -1;
	double adv = 1.0;
	double max = 1.0;
	int q = 0;
	for(int i=1; i<=m; i++) {
		gbc = i-lp[p[i-1]]-1;
		adv = adv-freq[p[i-1]]*gbc+1.0;
		lp[p[i-1]] = i-1;
		if(adv>max) {
			max = adv;
			q = i;
		}
	}
	return q;
}

int search(unsigned char *P, int m, unsigned char *T, int n) {
   int i, j, k, s, count, jbc[SIGMA][SIGMA];
   double freq[SIGMA];
   if(m<2) return -1;
    
   BEGIN_PREPROCESSING
   computeFreq(T, freq); 
   i = FindWorstOccurrence(P,m,freq);
   j = FindJumpDistance(P,m,i,freq,0.9);
   PrecomputeJOH(P,m,i,j,jbc);
   END_PREPROCESSING

   BEGIN_SEARCHING
   s = 0;
   count = 0;
   while(s<=n-m) {
      if(!memcmp(P,T+s,m)) count++;
      s+=jbc[T[s+i]][T[s+i+j]];
   }
   END_SEARCHING
   return count;
}
