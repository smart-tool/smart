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
 * This is an implementation of the Apostolico-Giancarlo algorithm
 * in A. Apostolico and R. Giancarlo. 
 * The Boyer-Moore-Galil string searching strategies revisited. SIAM J. Comput., vol.15, n.1, pp.98--105, (1986).
 */

#include "include/define.h"
#include "include/main.h"

void preBmBc(unsigned char *x, int m, int bmBc[]) {
    int i;
    for (i = 0; i < SIGMA; ++i) bmBc[i] = m;
    for (i = 0; i < m - 1; ++i) bmBc[x[i]] = m - i - 1;
}
 
 
void suffixes(unsigned char *x, int m, int *suff) {
    int f, g, i;
    suff[m - 1] = m;
    g = m - 1;
    for (i = m - 2; i >= 0; --i) {
        if (i > g && suff[i + m - 1 - f] < i - g)
            suff[i] = suff[i + m - 1 - f];
        else {
            if (i < g) g = i;
            f = i;
            while (g >= 0 && x[g] == x[g + m - 1 - f]) --g;
            suff[i] = f - g;
        }
    }
}

void preBmGsAG(unsigned char *x, int m, int bmGs[], int suff[]) {
    int i, j;
    suffixes(x, m, suff);
    for (i = 0; i < m; ++i) bmGs[i] = m;
    j = 0;
    for (i = m - 1; i >= 0; --i)
        if (suff[i] == i + 1)
            for (; j < m - 1 - i; ++j)
                if (bmGs[j] == m)
                   bmGs[j] = m - 1 - i;
    for (i = 0; i <= m - 2; ++i)
        bmGs[m - 1 - suff[i]] = m - 1 - i;
}



int search(unsigned char *x, int m, unsigned char *y, int n) {
    int i, j, k, s, shift, count;
    int bmGs[XSIZE], skip[XSIZE], suff[XSIZE], bmBc[SIGMA];
  
    /* Preprocessing */
    BEGIN_PREPROCESSING
    preBmGsAG(x, m, bmGs, suff);
    preBmBc(x, m, bmBc);
    memset(skip, 0, m*sizeof(int));
    END_PREPROCESSING
  
    /* Searching */
    BEGIN_SEARCHING
    count = 0;
    j = 0;
   	while (j <= n - m) {
        i = m - 1;
        while (i >= 0) {
            k = skip[i];
            s = suff[i];
            if (k > 0)
                if (k > s) {
                   if (i + 1 == s)
                      i = (-1);
                   else i -= s;
                   break;
                }
                else {
                    i -= k;
                    if (k < s) break;
                }
            else {
                if (x[i] == y[i + j]) --i;
                else break;
            }
        }
        if (i < 0) {
            OUTPUT(j);
            skip[m - 1] = m;
            shift = bmGs[0];
        }
        else {
            skip[m - 1] = m - 1 - i;
            shift = MAX(bmGs[i], bmBc[y[i + j]] - m + 1 + i);
        }
        j += shift;
        memcpy(skip, skip + shift, (m - shift)*sizeof(int));
        memset(skip + m - shift, 0, shift*sizeof(int));
    }
    END_SEARCHING    
    return count;
}


