/*
 *	ALGORITHM Weak Factor Recognizer, Linear Version, (LWFR)
 *  By Simone Faro, Domenico Cantone and Arianna Pavone
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
 *      Linearity is obtained by combaining the veification phase of the KMP algorithm
 */


#include "include/define.h"
#include "include/main.h"
#define Q 2
#define HASH(j) (y[j]<<2) + y[j-1]

void preKmp(unsigned char *x, int m, int kmpNext[]) {
    int i, j;
    i = 0;
    j = kmpNext[0] = -1;
    while (i < m) {
        while (j > -1 && x[i] != x[j])
            j = kmpNext[j];
        i++;
        j++;
        if (i<m && x[i] == x[j])
            kmpNext[i] = kmpNext[j];
        else
            kmpNext[i] = j;
    }
}

void preprocessing(unsigned char *x, int m, char *F) {
    int i,j;
    unsigned short h;
    int fact = m<16?m:16;
    //for(i=0; i<256*256; i++) F[i] = FALSE;
    long long int *G = (long long int *) F;
    int K = 65536 / sizeof(long long int);
    for(i=0; i<K; i++) G[i] = FALSE;
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
    int i, j, p, b, lf, count, test, kmpNext[XSIZE];
    int tp, st;
    char F[256*256];
    unsigned short h;
    if(m<2) return -1;
    
    BEGIN_PREPROCESSING
    /* Preprocessing */
    preKmp(x, m, kmpNext);
    int plen = m;
    if(m%2==1) m = m-1;
    int mm1 = m-1;
    int mq = m-Q+1;
    preprocessing(x,m,F);
    for(i=0; i<m; i++) y[n+i]=x[i];
    END_PREPROCESSING
    
    BEGIN_SEARCHING
    /* Searching */
    count = 0;
    tp = 0;
    st = 0;
    if( !memcmp(x,y,plen) ) count++;
    j=m;
    while (j < n) {
        h = HASH(j);
        while(!F[h]) {
            j += mq;
            h = HASH(j);
        }
        lf = b = j-m+Q;
        if(b<tp) b=tp-1;  //b is the maximum between lf and tp-1
        while((test=F[h]) && j>b) {
            j-=Q;
            h = (h<<4) + HASH(j);
        }
        
        // verification
        if(j<=b && test) {
            lf -= Q-1;
            if(lf>tp) {
                //restart kmp aut
                tp = lf;
                st = 0;
            }
            while(st >= tp-lf) {
                while(st<plen && x[st]==y[tp]) {
                    st++;
                    tp++;
                }
                if (st == plen && lf<=n-plen) count++;
                st = kmpNext[st];
                if(st<0) {
                    st++;
                    tp++;
                }
            }
            j = tp+mm1-st;
        }
        else j+=m-Q+1;
    }
    END_SEARCHING
    return count;
}

