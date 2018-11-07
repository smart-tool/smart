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
    //for(i=0; i<65536; i++) F[i] = FALSE;
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
    int i, j, jp, p, b, lf, k, count, test, kmpNext[XSIZE];
    int tp, st;
    char F[256*256];
    unsigned short h;
    int mm1 = m-1;
    
    BEGIN_PREPROCESSING
    /* Preprocessing */
    preKmp(x, m, kmpNext);
    preprocessing(x,m,F);
    for(i=0; i<m; i++) y[n+i]=x[i];
    END_PREPROCESSING
    
    BEGIN_SEARCHING
    /* Searching */
    count = 0;
    tp = 0;
    st = 0;
    j=m-1;
    jp = -1;
    while (j < n) {
        while(!F[y[j]]) j+=m;
        lf = j-mm1;
        b = jp;
        h = y[j];
        if(b<tp) b=tp-1;  //b is the maximum between lf and tp-1
        while((test=F[h]) && j>b) {
            j--;
            h = (h<<2) + y[j];
        }
        //verification
        if(j<=b && test) {
            if(lf>tp) {
                //restart kmp aut
                tp = lf;
                st = 0;
            }
            while(st >= tp-lf) {
                while(st<m && x[st]==y[tp]) {
                    st++;
                    tp++;
                }
                if (st == m && lf<=n-m) count++;
                st = kmpNext[st];
                if(st<0) {
                    st++;
                    tp++;
                }
            }
            j = tp+mm1-st;
        }
        else {
            
            j+=m;
        }
    }
    END_SEARCHING
    return count;
}

