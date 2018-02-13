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
 *  THIS IS AN IMPLEMENTATION OF:
 *	ALGORITHM Weak Factor Recognizer (WFR) Using Q-Grams
 *  appeared in: Simone Faro, Domenico Cantone and Arianna Pavone.
 *  Speeding Up String Matching by Weak Factor Recognition.
 *  Proceedings of the Pague Stringology Conference 2017: pp.42-50
 *
 * PREPROCESSING:
 *		an hash value is computed for al factors of the pattern with length in [1..16]
 *		the computed hash value is always a number in [0...256*256]
 *		if w is a factor of x, and hash(w) is its hash value, than F[hash(w)]=TRUE, otherwise F[hash(w)]=FALSE
 * SEARCHING
 *		The algorithm searches for factors of the pattern using a weak recognition method
 *		the search phase is very similar to BOM.
 *		The window is scanned right to left and for each character a new hash value of the suffix of the window is computed.
 *		Let w be the suffix we scanned. If F[hash(w)]=TRUE we continue scanning the next character of the window.
 *		Otherwise we stop scanning (w is not a factor of the pattern) and jump to the right, like in BOM.
 */

#include "include/define.h"
#include "include/main.h"
#include "include/GRAPH.h"
#define Q 4
#define HASH(j) (y[j]<<6) + (y[j-1]<<4) + (y[j-2]<<2) + y[j-3]

int preprocessing(unsigned char *x, int m, char *F) {
	int i,j;
	unsigned short h;
	int fact = m<16?m:16;
	for(i=0; i<256*256; i++) F[i] = FALSE;
	for(i=0; i<m; i++) {
		int stop = (i-fact+1)>0?(i-fact+1):0;
		h = 0;
		for(j=i; j>=stop; j--) {
			h = h<<2;
			h += x[j];
			F[h] = TRUE;
		}  
	}
}


int search(unsigned char *x, int m, unsigned char *y, int n) {
    int i, j, p, k, count, test;
    char F[256*256];
    unsigned short h;
    if(m<Q) return -1;
    
    BEGIN_PREPROCESSING
    /* Preprocessing */
    int plen = m;
    if(m%Q!=0) m = m-(m%Q);
    int mq = m-Q+1;
    preprocessing(x,m,F);
    for(i=0; i<m; i++) y[n+i]=x[i];
    END_PREPROCESSING
    
    BEGIN_SEARCHING
    /* Searching */
    count = 0;
    if( !memcmp(x,y,plen) ) count++;
    j=m;
    while (j < n) {
        h = HASH(j);
        i = j-m+1;
        while((test=F[h]) && j>i+Q-1) {
            j-=Q;
            h = (h<<8) + HASH(j);
        }
        if(j==i+Q-1 && test) {
            k=0;
            while(k<plen && x[k]==y[i+k]) k++;
            if(k==plen && i<=n-plen)
                count++;
        }
        j+=m-Q+1;
    }
    END_SEARCHING
    return count;
}

