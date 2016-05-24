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
 * This is an implementation of the Apostolico-Crochemore algorithm
 * in A. Apostolico and M. Crochemore. 
 * Optimal canonization of all substrings of a string. Inf. Comput., vol.95, n.1, pp.76--95, (1991).
 */

#include "include/define.h"
#include "include/main.h"

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


int search(unsigned char *x, int m, unsigned char *y, int n) {
    int i, j, k, ell, kmpNext[XSIZE], count;

    /* Preprocessing */
    BEGIN_PREPROCESSING
    preKmp(x, m, kmpNext);
    for (ell = 1; ell<m && x[ell - 1] == x[ell]; ell++);
    if (ell == m) ell = 0;
    END_PREPROCESSING

    /* Searching */
    BEGIN_SEARCHING
    count = 0;
    i = ell;
    j = k = 0;
    while (j <= n - m) {
        while (i < m && x[i] == y[i + j])   ++i;
        if (i >= m) {
            while (k < ell && x[k] == y[j + k])  ++k;
            if (k >= ell) OUTPUT(j);
        }
        j += (i - kmpNext[i]);
        if (i == ell) k = MAX(0, k - 1);
        else
            if (kmpNext[i] <= ell) {
                k = MAX(0, kmpNext[i]);
                i = ell;
            }
            else {
                k = ell;
                i = kmpNext[i];
            }
    }
    END_SEARCHING
    return count;
}

