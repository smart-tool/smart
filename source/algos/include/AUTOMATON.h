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

#define setTarget(p, c, q) ttrans[(p)*SIGMA+(c)] = (q)
#define getTarget(p, c) ttrans[(p)*SIGMA+(c)]
#define setLength(p, q) tlength[(p)] = (q)
#define getLength(p) tlength[(p)]
#define setSuffixLink(p, q) tsuffix[(p)] = (q)
#define getSuffixLink(p) tsuffix[(p)]
#define setShift(p,c,q) tshift[(p)*SIGMA + c] = (q)
#define getShift(p,c) tshift[(p)*SIGMA + c]
#define setPosition(p,q) tposition[(p)]=(q)
#define getPosition(p) tposition[(p)]
#define setTerminal(p) tterminal[(p)] = 1
#define isTerminal(p) tterminal[(p)]
#define newState() counter++
#define error(string) exit(1)

#define setSMA(p, c, q) ttransSMA[(p)*SIGMA+(c)] = (q)
#define getSMA(p, c) ttransSMA[(p)*SIGMA+(c)]
#define isSMATerminal(p) p==m

#define UNDEFINED -1
#define FALSE      0
#define TRUE       1

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

  struct _cell{
    int element;
    struct _cell *next;
  };

  typedef struct _cell *List;

char *reverse(char *x, int m) {
   char *xR;
   int i;
 
   xR = (char *)malloc((m + 1)*sizeof(char));
   for (i = 0; i < m; ++i)
      xR[i] = x[m - 1 - i];
   xR[m] = '\0';
   return(xR);
}

/*void copyVertex(int target, int source) {
	  for(i=0; i<SIGMA; i++) setTarget(target, i, getTarget(source,i) );      
	  for(i=0; i<SIGMA; i++) setShift(target, i, getShift(source,i) );      
      tterminal[target] = tterminal[source];
      tsuffix[target] = tsuffix[source];
      tlength[target] = tlength[source];
      tposition[target] = tposition[source];
}*/

/*void buildSuffixAutomaton(unsigned char *x, int m, 
   int *ttrans, int *tlength, int *tsuffix, unsigned char *tterminal, int *tshift, int *position) {
   int i, art, init, last, p, q, r, counter,j, source, target;
   unsigned char c;
  
   init = 0;
   art = 1;
   counter = 2;
   setSuffixLink(init, art);
   last = init;
   for (i = 0; i < m; ++i) {
      c = x[i];
      p = last;
      q = newState();
      setLength(q, getLength(p) + 1);
	  setPosition(q, getPosition(p) + 1);
      while (p != init && getTarget(p, c) == UNDEFINED) {
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
         if (getLength(p) + 1 == getLength(getTarget(p, c))) {
            setSuffixLink(q, getTarget(p, c));
         }
         else {
            r = newState();
            //copyVertex(r, getTarget(p, c));
			target = r;
			source = getTarget(p,c);
		  	for(j=0; j<SIGMA; j++) setTarget(target, j, getTarget(source,j) );      
		  	for(j=0; j<SIGMA; j++) setShift(target, j, getShift(source,j) );      
      		tterminal[target] = tterminal[source];
      		tsuffix[target] = tsuffix[source];
      		tlength[target] = tlength[source];
      		position[target] = position[source];

            memcpy(ttrans+r*SIGMA, ttrans+getTarget(p, c)*SIGMA, SIGMA*sizeof(int));
            setLength(r, getLength(p) + 1);
            setSuffixLink(r, getSuffixLink(getTarget(p, c)));
            setSuffixLink(getTarget(p, c), r);
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
}*/

void buildSimpleSuffixAutomaton(unsigned char *x, int m, 
   int *ttrans, int *tlength, int *tsuffix, unsigned char *tterminal) {
   int i, art, init, last, p, q, r, counter;
   unsigned char c;
  
   init = 0;
   art = 1;
   counter = 2;
   setSuffixLink(init, art);
   last = init;
   for (i = 0; i < m; ++i) {
      c = x[i];
      p = last;
      q = newState();
      setLength(q, getLength(p) + 1);
      while (p != init && getTarget(p, c) == UNDEFINED) {
         setTarget(p, c, q);
         p = getSuffixLink(p);
      }
      if (getTarget(p, c) == UNDEFINED) {
         setTarget(init, c, q);
         setSuffixLink(q, init);
      }
      else
         if (getLength(p) + 1 == getLength(getTarget(p, c))) {
            setSuffixLink(q, getTarget(p, c));
         }
         else {
            r = newState();
            //copyVertex(r, getTarget(p, c));
            memcpy(ttrans+r*SIGMA, ttrans+getTarget(p, c)*SIGMA, SIGMA*sizeof(int));
            setLength(r, getLength(p) + 1);
            setSuffixLink(r, getSuffixLink(getTarget(p, c)));
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


void preSMA(unsigned char *x, int m, int *ttransSMA) {
   int i, j,state, target, oldTarget;
   unsigned char c;

   memset(ttransSMA, 0, SIGMA*sizeof(int));
   for (state = 0, i = 0; i < m; ++i) {
      oldTarget = getSMA(state, x[i]);
      target = state+1;
      setSMA(state, x[i], target);
      for (j=0, c=0; j < SIGMA; ++c, ++j)
         setSMA(target, c, getSMA(oldTarget, c));
      state = target;
   }
}
