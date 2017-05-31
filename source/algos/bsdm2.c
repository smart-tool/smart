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

#define DSIGMA 65536
#define HS(x,i) (x[i]<<2) + x[i+1]
#define Q 2

int search(unsigned char *x, int m, unsigned char *y, int n) {
	unsigned int B[DSIGMA];
	int i, j, k, count;
    unsigned int s,d;
    if(m<Q) return -1;
    
    BEGIN_PREPROCESSING
    unsigned int occ[DSIGMA] = {0};
    int st = 0, len = 0;
    unsigned short c, dch;
    for(i=0, j=0; i<m-Q+1; i++) {
        c = HS(x,i);
        if(occ[c]) {
            dch = HS(x,j);
            while(dch!=c) {
                occ[dch]=0;
                j++;
                dch = HS(x,j);
            } 
            occ[dch]=0;
            j++;
        }
        occ[c]=1;
        if(len < i-j+1 ) {
            len = i-j+1;
            st = j;
        }
    }
    
	unsigned int pos[DSIGMA];
	for(i=0; i<DSIGMA; i++) pos[i]=-1;
	for(i=0; i<len; i++) {
	    c = HS(x,st+i);
	    pos[c]=i;
	}
    END_PREPROCESSING
	
    BEGIN_SEARCHING
    count = 0;
	for(i=0; i<m; i++) y[n+i]=x[i];
	unsigned char *xst = x+st;
	int offset = len+st-1;
	j = len-1;
	while(j<n) {
	    c = HS(y,j);
	    while ((i=pos[c])<0) {
	        j+=len;
   	        c = HS(y,j);
	    }
	    k=1;
	    while(k<=i && xst[i-k]==y[j-k]) k++;
	    if(k>i) {
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



