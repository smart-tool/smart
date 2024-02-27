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
 * This is an implementation of the Backward SNR DAWG Matching algorithm
 * in S. Faro and T. Lecroq. 
 * A Fast Suffix Automata Based Algorithm for Exact Online String Matching. 
 * 17-th International Conference on Implementation and Application of Automata - CIAA 2012, pp.146--160 (2012).
 */


#include "include/define.h"
#include "include/main.h"


int search(unsigned char *x, int m, unsigned char *y, int n) {
	unsigned int B[SIGMA];
	int i, j, k, count;
    unsigned int s,d;

    BEGIN_PREPROCESSING
    unsigned int occ[SIGMA] = {0};
    int st = 0, len = 0;
    for(i=0, j=0; i<m; i++) {
        if(occ[x[i]]) {
            while(x[j]!=x[i]) {
                occ[x[j]]=0;
                j++;
            } 
            occ[x[j]]=0;
            j++;
        }
        occ[x[i]]=1;
        if(len < i-j+1 ) {
            len = i-j+1;
            st = j;
        }
    }
	unsigned int pos[SIGMA];
	for(i=0; i<SIGMA; i++) pos[i]=-1;
	for(i=0; i<len; i++) pos[x[st+i]]=i;
    END_PREPROCESSING
	
    BEGIN_SEARCHING
    count = 0;
	for(i=0; i<m; i++) y[n+i]=x[i];
	unsigned char *xst = x+st;
	int offset = st+len-1;
	j = st+len-1;
	while(j<n) {
	    while ((i=pos[y[j]])<0) j+=len; //fast loop
	    //printf("%c - %c\n",y[j],x[st+i]);
	    k=1;
	    while(k<=i && xst[i-k]==y[j-k]) k++;
	    if(k>i) { // a suffix has been found
	    	if(k==len) {
	    		if(!memcmp(x,y+j-offset,m)) if(j-offset<=n-m) count++;
	    	}
	    	else j-=k;
	    }
	    j+=len;
	}
    END_SEARCHING
   	return count;
}



