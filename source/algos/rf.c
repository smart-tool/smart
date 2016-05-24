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
 * This is an implementation of the Reverse Factor algorithm
 * in T. Lecroq. A variation on the Boyer-Moore algorithm. Theor. Comput. Sci., vol.92, n.1, pp.119--144, (1992).
 */

#include "include/define.h"
#include "include/main.h"
#include "include/AUTOMATON.h"

void buildSuffixAutomaton(unsigned char *x, int m, int *ttrans, int *tlength, int *tsuffix, unsigned char *tterminal) {
   int i, art, init, last, p, q, r, counter;
   unsigned char c;

   init = 0;
   art = 1;
   counter = 2;
   setSuffixLink(init, art);
   last = init;
   for (i = m-1; i >= 0; --i) {
      c = x[i];
      p = last;
      q = newState();
      setLength(q, getLength(p) + 1);
      while (p != init &&
             getTarget(p, c) == UNDEFINED) {
         setTarget(p, c, q);
         p = getSuffixLink(p);
      }
      if (getTarget(p, c) == UNDEFINED) {
         setTarget(init, c, q);
         setSuffixLink(q, init);
      }
      else
         if (getLength(p) + 1 == getLength(getTarget(p, c)))
            setSuffixLink(q, getTarget(p, c));
         else {
            r = newState();
            //copyVertex(r, getTarget(p, c));
            memcpy(ttrans+r*SIGMA, ttrans+getTarget(p, c)*SIGMA, SIGMA*sizeof(int));
            setSuffixLink(r, getSuffixLink(getTarget(p, c)));
            setLength(r, getLength(p) + 1);
            setSuffixLink(getTarget(p, c), r);
            setSuffixLink(q, r);
            while (p != art && getLength(getTarget(p, c)) >= getLength(r)) {
               setTarget(p, c, r);
               p = getSuffixLink(p);
            }
         }
      last = q;
   }
   setTerminal(last);
   while (last != init) {
      last = getSuffixLink(last);
      setTerminal(last);
   }
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
   int i, j, shift, period, init, state, count;
   int *ttrans, *tlength, *tsuffix;
   unsigned char *tterminal;
   int mMinus1, nMinusm, size;
 
   /* Preprocessing */
   BEGIN_PREPROCESSING
   mMinus1 = m-1;
   nMinusm = n-m;
   size = 2*m+3;
   ttrans = (int *)malloc(size*SIGMA*sizeof(int));
   tlength = (int *)calloc(size, sizeof(int));
   tsuffix = (int *)calloc(size, sizeof(int));
   tterminal = (unsigned char *)calloc(size, sizeof(unsigned char));
   memset(ttrans, -1, (2*m+3)*SIGMA*sizeof(int));
   buildSuffixAutomaton(x, m, ttrans, tlength, tsuffix, tterminal);
   init = 0;
   period = m;
   END_PREPROCESSING

   /* Searching */
   BEGIN_SEARCHING
   count = 0;
   if (strncmp(x, y, m) == 0)
      OUTPUT(0);
   j = 1;
   while (j <= nMinusm) {
      i = mMinus1;
      state = init;
      shift = m;
      while (getTarget(state, y[i + j]) != UNDEFINED) {
         state = getTarget(state, y[i + j]);
         if (isTerminal(state)) {
            period = shift;
            shift = i;
         }
         --i;
      }
      if (i < 0) {
         OUTPUT(j);
         shift = period;
      }
      j += shift;
   }
   free(ttrans);
   free(tlength);
   free(tsuffix);
   free(tterminal);
   END_SEARCHING
   return count;
}
