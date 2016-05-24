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
 * This is an implementation of the Average Optimal Shift Or algorithm
 * in K. Fredriksson and S. Grabowski. 
 * Practical and Optimal String Matching. SPIRE, Lecture Notes in Computer Science, vol.3772, pp.376--387, Springer-Verlag, Berlin, (2005).
 */

#include "include/define.h"
#include "include/main.h"
#include "include/log2.h"

void verify(unsigned char *y, int j, int n, unsigned char *x, int m, int q, unsigned int D, unsigned int mm, int *count) {
    unsigned int s;
    int c, k, i;
    D = (D & mm)^mm;
    while (D != 0) {
        s = LOG2(D); 
        c = -(m/q-1)*q-s/(m/q);
        k = 0; i=j+c;
        if(i>=0 && i<=n-m) while(k<m && x[k]==y[i+k]) k++;
        if(k==m) (*count)++;
        D &= ~(1<<s);
    }
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
    unsigned int B[SIGMA], D, h, mm, tmp, not_mm;
    int i, j, count;
    int q=6;

    if(m<=q) return -1;   
    if(m>32) return search_large(x,m,y,n,q);

    /* Preprocessing */
    BEGIN_PREPROCESSING
    for (i = 0; i < SIGMA; ++i) B[i] = ~0; 
    h = mm = 0;
    for (j = 0; j < q; ++j) {
        for (i = 0; i < m/q; ++i) {
            tmp = ~(1<<h);
            B[x[i*q+j]] &= tmp;
            ++h;
        }
        mm |= (1<<(h-1));
    }
    END_PREPROCESSING

    BEGIN_SEARCHING
    /* Searching */
    count = 0;
    D = ~0;
    j = 0;
    while (j < n) {
        D = ((D & ~mm)<<1)|B[y[j]];
        if ((D & mm) != mm)
            verify(y, j, n, x, m, q, D, mm, &count);
        j += q;
    }
    END_SEARCHING
    return count;
}

/*
 * AOSO algorithm designed for large patterns
 * The present implementation searches for prefixes of the pattern of length 32.
 * When an occurrence is found the algorithm tests for the whole occurrence of the pattern
 */

void verify_large(unsigned char *y, int j, int n, unsigned char *x, int m, int q, unsigned int D, unsigned int mm, int *count, int p_len) {
    unsigned int s, tmp;
    int c, k, i;
    D = (D & mm)^mm;
    while (D != 0) {
        s = LOG2(D);
        c = -(m/q-1)*q-s/(m/q);
        k = 0; i=j+c;
        if(i>=0 && i<=n-p_len) while(k<p_len && x[k]==y[i+k]) k++;
        if(k==p_len) (*count)++;
        D &= ~(1<<s);
    }
}

int search_large(unsigned char *x, int m, unsigned char *y, int n, int q) {
    unsigned int B[SIGMA], D, h, mm, tmp;
    int i, j, count, p_len;
    p_len = m;
    m = 32;

    BEGIN_PREPROCESSING
    /* Preprocessing */
    for (i = 0; i < SIGMA; ++i) B[i] = ~0; 
    h = mm = 0;
    for (j = 0; j < q; ++j) {
        for (i = 0; i < m/q; ++i) {
            tmp = ~(1<<h);
            B[x[i*q+j]] &= tmp;
            ++h;
        }
        mm |= (1<<(h-1));
    }
    END_PREPROCESSING

    BEGIN_SEARCHING
    /* Searching */
    count = 0;
    D = ~0;
    j = 0;
    while (j < n) {
        D = ((D & ~mm)<<1)|B[y[j]];
        if ((D & mm) != mm)
            verify_large(y, j, n, x, m, q, D, mm, &count, p_len);
        j += q;
    }
    END_SEARCHING
    return count;
}
