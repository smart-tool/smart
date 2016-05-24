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
 */

#include "include/define.h"
#include "include/main.h"

void preQsBc(unsigned char *P, int m, int qbc[])
{
	int i;
	for(i=0;i<SIGMA;i++)	qbc[i]=m+1;
	for(i=0;i<m;i++) qbc[P[i]]=m-i;
}

 ////////////Searching Phase///////////////////////////////////////
 int search(unsigned char *x, int m, unsigned char *y, int n){
   int count,i,j =0;
   int qsBc[SIGMA];
   unsigned char firstCh, lastCh;
   count = 0;
   BEGIN_PREPROCESSING
   preQsBc(x, m, qsBc);
   firstCh = x[0];
   lastCh = x[m -1];
   for(i=0; i<m; i++) y[n+i]=lastCh;
   END_PREPROCESSING
   BEGIN_SEARCHING
   while(j <= n - m){
     // Stage 1
     if(lastCh == y[j + m - 1] && firstCh == y[j])
     {
        //Stage 2
        for(i = m-2; i > 0 && x[i] == y[j + i]; i--);
        if(i <= 0) count++;
     }
     // Stage 3
     j += qsBc[y[j + m]];
    }
    END_SEARCHING
   return count;
 }
