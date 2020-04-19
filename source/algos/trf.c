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
 * This is an implementation of the Turbo Reverse Factor algorithm
 * in M. Crochemore and A. Czumaj and L. Gcasieniec and S. Jarominek and T. Lecroq and W. Plandowski and W. Rytter. 
 * Speeding up two string matching algorithms. Algorithmica, vol.12, n.4/5, pp.247--267, (1994).
 */

#include "include/define.h"
#include "include/main.h"
#include "include/AUTOMATON.h"

void preMpforTRF(unsigned char *x, int m, int mpNext[]) {
   int i, j;
   i = 0;
   j = mpNext[0] = -1;
   while (i < m) {
      while (j > -1 && x[i] != x[j])
         j = mpNext[j];
      mpNext[++i] = ++j;
   }
}

void buildSuffixAutomaton4TRF(unsigned char *x, int m, 
   int *ttrans, int *tlength, int *tposition, int *tsuffix, unsigned char *tterminal, int *tshift) {
   int i, art, init, last, p, q, r, counter, tmp;
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
      setPosition(q, getPosition(p) + 1);
      while (p != init &&
             getTarget(p, c) == UNDEFINED) {
         setTarget(p, c, q);
         setShift(p, c, getPosition(q) - getPosition(p) - 1);
         p = getSuffixLink(p);
      }
      if (getTarget(p, c) == UNDEFINED) {
         setTarget(init, c, q);
         setShift(init, c, getPosition(q) - getPosition(init) - 1);
         setSuffixLink(q, init);
      }
      else
         if (getLength(p) + 1 == getLength(getTarget(p, c)))
            setSuffixLink(q, getTarget(p, c));
         else {
            r = newState();
            tmp = getTarget(p, c);
            memcpy(ttrans+r*SIGMA, ttrans+tmp*SIGMA, SIGMA*sizeof(int));
            memcpy(tshift+r*SIGMA, tshift+tmp*SIGMA, SIGMA*sizeof(int));
            setPosition(r, getPosition(tmp));
            setSuffixLink(r, getSuffixLink(tmp));
            setLength(r, getLength(p) + 1);
            setSuffixLink(tmp, r);
            setSuffixLink(q, r);
            while (p != art && getLength(getTarget(p, c)) >= getLength(r)) {
               setShift(p, c, getPosition(getTarget(p, c)) - getPosition(p) - 1);
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
   int period, i, j, shift, u, periodOfU, disp, init, count, state, mu, *mpNext;
   int *ttrans, *tlength, *tposition, *tsuffix, *tshift;
   unsigned char *tterminal;
   int mMinus1, nMinusm, size;
  
   /* Preprocessing */
   BEGIN_PREPROCESSING
   nMinusm = n-m;
   mMinus1 = m-1;
   size = 2*m+3;
   mpNext = (int *)malloc((m+1)*sizeof(int));
   ttrans = (int *)malloc(size*SIGMA*sizeof(int));
   tshift = (int *)malloc(size*SIGMA*sizeof(int));
   tlength = (int *)calloc(size, sizeof(int));
   tposition = (int *)calloc(size, sizeof(int));
   tsuffix = (int *)calloc(size, sizeof(int));
   tterminal = (unsigned char *)calloc(size, sizeof(unsigned char));
   memset(ttrans, -1, (2*m+3)*SIGMA*sizeof(int));
   buildSuffixAutomaton4TRF(x, m, ttrans, tlength, tposition, tsuffix, tterminal, tshift);
   init = 0;
   preMpforTRF(x, m, mpNext);
   period = m - mpNext[m];
   i = 0;
   shift = m;
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
      u = mMinus1 - shift;
      periodOfU = (shift != m ?  m - shift - mpNext[m - shift] : 0);
      shift = m;
      disp = 0;
      while (i > u && getTarget(state, y[i + j]) != UNDEFINED) {
         disp += getShift(state, y[i + j]);
         state = getTarget(state, y[i + j]);
         if (isTerminal(state))
            shift = i;
         --i;
      }
      if (i <= u)
         if (disp == 0) {
            OUTPUT(j);
            shift = period;
         }
         else {
            mu = (u + 1)/2;
            if (periodOfU <= mu) {
               u -= periodOfU;
               while (i > u && getTarget(state, y[i + j]) != UNDEFINED) {
                  disp += getShift(state, y[i + j]);
                  state = getTarget(state, y[i + j]);
                  if (isTerminal(state))
                     shift = i;
                  --i;
               }
               if (i <= u)
                  shift = disp;
            }
            else {
               u = u - mu - 1;
               while (i > u && getTarget(state, y[i + j]) != UNDEFINED) {
                  disp += getShift(state, y[i + j]);
                  state = getTarget(state, y[i + j]);
                  if (isTerminal(state))
                     shift = i;
                  --i;
               }
            }
         }
      j += shift;
   }
   free(mpNext);
   free(ttrans);
   free(tshift);
   free(tlength);
   free(tposition);
   free(tsuffix);
   free(tterminal);
   END_SEARCHING
   return count;
}
