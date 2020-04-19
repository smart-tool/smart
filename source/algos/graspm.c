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
 * This is an implementation of the Genomic Rapid Algorithm for String Pattern Matching
 * in S. Deusdado and P. Carvalho. 
 * GRASPm: an efficient algorithm for exact pattern-matching in genomic sequences. 
 * Int. J. Bioinformatics Res. Appl., vol.5, n.4, pp.385--401, Inderscience Publishers, Inderscience Publishers, Geneva, SWITZERLAND, (2009).
 */

#include "include/define.h"
#include "include/main.h"

typedef struct GRASPmList {
   int k;
   struct GRASPmList *next;
} GList;

void ADD_LIST(GList **l, int e)  {
   GList *t = (GList *)malloc(sizeof(GList));
   t->k = e;
   t->next = *l;
   *l = t;
}

int search(unsigned char *p, int m, unsigned char *t, int n) {
   GList *pos, *z[SIGMA];
   int i,j,k,count,first, hbc[SIGMA];

   /* Preprocessing of the list */
   BEGIN_PREPROCESSING
   for(i=0; i<SIGMA; i++) z[i]=NULL;
   if (p[0] == p[m-1]) for(i=0; i<SIGMA; i++) ADD_LIST(&z[i], 0);
   for(i=0; i<m-1;i++) if (p[i+1] == p[m-1]) ADD_LIST(&z[p[i]],(i+1));
   /* Preprocessing of horspool bc */    
   for(i=0;i<SIGMA;i++) hbc[i]=m;
   for(i=0;i<m;i++) hbc[p[i]]=m-i-1;
   for(i=0;i<m;i++) t[n+i]=p[i];
   END_PREPROCESSING

   /* searching */
   BEGIN_SEARCHING
   count = 0;
   j = m-1;
   while (j<n) {
      while (k=hbc[t[j]]) j += k; {
         pos = z[t[j-1]];
         while(pos!=NULL) { 
            k = pos->k;
            i = 0; first = j-k;                 
            while(i<m && p[i]==t[first+i]) i++;
            if(i==m && first<=n-m) count++;
            pos = pos->next;
         }
      }
      j+=m;
   }
   END_SEARCHING
   return count;
}
