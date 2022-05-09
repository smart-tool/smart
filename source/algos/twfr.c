/*
 *	ALGORITHM Weak Factor Recognizer, Tuned Version, (TWFR)
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
 */

#include "include/define.h"
#include "include/mainstats.h" // defines the search interface for time and statistics.
#include "include/bitstats.h"  // implements the standard set of bit algorithm statistic names.


int preprocessing(unsigned char *x, int m, char *F) {
	int i,j;
	unsigned short h;
    int fact = m < 8 ? m : 8;
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
   int i, j, p, k, count, test;
	char F[256*256];
	unsigned short h;
	int mMinus1 = m-1;
    //if(m<2) return -1;
	
   BEGIN_PREPROCESSING
   /* Preprocessing */
	preprocessing(x,m,F);
	for(i=0; i<m; i++) y[n+i]=x[i];
   END_PREPROCESSING

   BEGIN_SEARCHING
   /* Searching */
   count = 0;
   j = m-1;
   while (j < n) {
		while(!F[y[j]]) j+=m;
		i = j-mMinus1;
        h = y[j];
		while((test=F[h]) && j>i) {
			j--;
			h = (h<<2) + y[j];
		}
		if(j==i && test) {
			k=0;
			while(k<m && x[k]==y[i+k]) k++;
			if(k==m && j<=n-m)
                count++;
		}
		j+=m;
	}
   END_SEARCHING
   return count;
}

struct searchInfo searchStats(unsigned char *x, int m, unsigned char *y, int n) {
    int i, j, p, k, test;
    char F[256*256];
    unsigned short h;
    int mMinus1 = m-1;
    //if(m<2) return -1;

    /* Preprocessing */
    preprocessing(x,m,F);
    for(i=0; i<m; i++) y[n+i]=x[i];

    /* Basic search info */
    struct searchInfo results = {0};
    initStats(&results, n, 256*256, sizeof(char));

    /* Table stats */
    countEmptySlotsAndBitsSetC(0, 1, &results, F, 256*256);

    /* Searching */
    j = m-1;
    int lastPos;
    while (j < n) {
        results.mainLoopCount++;
        lastPos = j;

        while(++results.indexLookupCount && ++results.textBytesRead  && !F[y[j]]) {
            j += m;
            results.fastPathCount++;
            results.fastPathShifts += m;
            results.numShifts++;
        }

        i = j-mMinus1;
        h = y[j];
        results.textBytesRead++;

        while(++results.indexLookupCount  && (test=F[h]) && j>i) {
            j--;
            h = (h<<2) + y[j];
            results.textBytesRead++;
        }

        if(j==i && test) {
            results.validationCount++;
            k=0;
            while(k<m && x[k]==y[i+k]) {
                k++;
                results.validationBytesRead++;
                results.textBytesRead++;
            }
            if(k==m && j<=n-m) {
                results.matchCount++;
            }
            results.validationShifts += (j+m-lastPos);
        }
        j+=m;
        results.slowPathShifts += (j - lastPos);
        results.numShifts++;
    }
    return results;
}
