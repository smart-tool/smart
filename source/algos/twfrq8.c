/*
 *	ALGORITHM Weak Factor Recognizer, Tuned Version, (TWFR)
 *  By Simone Faro, Domenico Cantone and Arianna Pavone
 *
 * This tuned variant has two optimisations:
 *   1.  An initial fast loop which does not require a position test, as the pattern is copied to the end of the data.
 *   2.  Pre-processing is done on q-grams rather than bytes, as for the WFRQ variants.
 *
 * PREPROCESSING:
 *		an hash value is computed for al factors of the pattern with length in [1..8]
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
#define Q 8
#define HASH(j) (y[j]<<14) + (y[j-1]<<12) + (y[j-2]<<10) + (y[j-3]<<8) + (y[j-4]<<6) + (y[j-5]<<4) + (y[j-6]<<2) + y[j-7]

/*
 * Pre-process q-gram factors of the pattern.
 */
int preprocessingQ(unsigned char *x, int m, char *F) {
    int i,j;
    unsigned short h;
    int fact = m < 8 ? m : 8;
    for(i=0; i<256*256; i++) F[i] = FALSE;
    for(i=Q-1; i<m; i++) {
        int stop = (i-fact+1)>=Q-1?(i-fact+1):Q-1;
        h = (x[i]<<14) + (x[i-1]<<12) + (x[i-2]<<10) + (x[i-3]<<8) + (x[i-4]<<6) + (x[i-5]<<4) + (x[i-6]<<2) + x[i-7];
        F[h] = TRUE;
        for(j=i-Q; j>=stop; j-=Q) {
            h = (h<<16) + (x[j]<<14) + (x[j-1]<<12) + (x[j-2]<<10) + (x[j-3]<<8) + (x[j-4]<<6) + (x[j-5]<<4) + (x[j-6]<<2) + x[j-7];
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
    preprocessingQ(x,m,F);
    for(i=0; i<m; i++) y[n+i]=x[i];
    END_PREPROCESSING

    BEGIN_SEARCHING
    /* Searching */
    count = 0;
    if( !memcmp(x,y,plen) ) count++;
    j=m;
    while (j < n) {
        h = HASH(j);
        while(!F[h]) {
            j += mq;
            h = HASH(j);
        }
        i = j-m+1;
        while((test=F[h]) && j>i+Q-1) {
            j-=Q;
            h = (h<<16) + HASH(j);
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

