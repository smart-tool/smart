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
 * This is an implementation of the Tailed Substring algorithm
 * in D. Cantone and S. Faro. 
 * Searching for a substring with constant extra-space complexity. 
 * Proc. of Third International Conference on Fun with algorithms, pp.118--131, (2004).
 */

#include "include/define.h"
#include "include/main.h"

int PreBadPosBC(unsigned char *P, int m, int *hbc, int pos)
{
	
	int i, alpha=SIGMA;
	
	for(i=0;i<alpha;i++) hbc[i] = pos+1;
	
	for(i=0;i<pos;i++) hbc[P[i]] = pos-i;

}



int FindBadPos(unsigned char *P, int m)
{
   int i, j, alpha, pos, bc[SIGMA], temp, max, q, gbc;
   double last_value, values[SIGMA], best_value, adv;
   char c;
   alpha = SIGMA;
   //P[m]='1';
   //P[m+1]='\0';
   for(i=0; i<=alpha; i++) bc[i]=-1;
   adv = 1;
   max = 1;
   q = 0;
   for(i=1; i<=m; i++)
   {
      gbc = i-bc[P[i-1]]-1; 																				//last_value += alpha;
      adv = adv - (double)gbc * 0.5 + 1;
      bc[P[i-1]]=i-1;
      if(adv > max) {
      	max = adv;
      	q = i;
      }
   }
   //P[m]='\0';
   //printf("%d / %d\n",q,m);
   return q;
}

int search(unsigned char *P, int m, unsigned char *T, int n)
{
	int i, j, k, d, shift, s, count, bc[SIGMA], pos;
   BEGIN_PREPROCESSING
   	pos = FindBadPos(P, m);
	PreBadPosBC(P, m, bc, pos);
 
	for(i=0; i<m; i++) T[n+i] = P[i];  
   END_PREPROCESSING
   BEGIN_SEARCHING
	s = 0;
   
	count = 0;
	
	while(s<=n-m)
 {
			
		i=0;
			
		while(i<m && P[i]==T[s+i]) i++;
			
		if(i==m) count++;
			
		s += bc[T[s+pos]];
		
	}
	
   END_SEARCHING
	return count;

}
