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

  struct _cell{
    int element; 
    struct _cell *next;
  };
 
  typedef struct _cell *List;

//Another important structures are automata and specifically suffix automata (see chapter 22). Basically automata are directed graphs. We will use the following interface to manipulate automata:
/*
  Graph newGraph(int v, int e); 
  Graph newAutomaton(int v, int e); 
  Graph newSuffixAutomaton(int v, int e); 
  int newVertex(Graph g); 
  int getInitial(Graph g); 
  boolean isTerminal(Graph g, int v); 
  void setTerminal(Graph g, int v); 
  int getTarget(Graph g, int v, unsigned char c); 
  void setTarget(Graph g, int v, unsigned char c, int t); 
  int getSuffixLink(Graph g, int v); 
  void setSuffixLink(Graph g, int v, int s); 
  int getLength(Graph g, int v); 
  void setLength(Graph g, int v, int ell); 
  int getPosition(Graph g, int v); 
  void setPosition(Graph g, int v, int p); 
  int getShift(Graph g, int v, unsigned char c); 
  void setShift(Graph g, int v, unsigned char c, int s); 
  void copyVertex(Graph g, int target, int source);
*/
//A possible implementation of this interface follows.
#define error(string) exit(1)
  
  struct _graph {
    int vertexNumber, 
        edgeNumber, 
        vertexCounter, 
        initial, 
        *terminal, 
        *target, 
        *suffixLink,
        *length, 
        *position, 
        *shift; 
  };
  
  typedef struct _graph *Graph; 
  typedef int boolean;
   
#define UNDEFINED -1

/* returns a new data structure for
   a graph with v vertices and e edges */
Graph newGraph(int v, int e) {
   Graph g;

   g = (Graph)calloc(1, sizeof(struct _graph));
   if (g == NULL)
      error("newGraph");
   g->vertexNumber  = v;
   g->edgeNumber    = e;
   g->initial       = 0;
   g->vertexCounter = 1;
   return(g);
}


/* returns a new data structure for
   a automaton with v vertices and e edges */
Graph newAutomaton(int v, int e) {
   Graph aut;

   aut = newGraph(v, e);
   aut->target = (int *)calloc(e, sizeof(int));
   if (aut->target == NULL)
      error("newAutomaton");
   aut->terminal = (int *)calloc(v, sizeof(int));
   if (aut->terminal == NULL)
      error("newAutomaton");
   return(aut);
}


/* returns a new data structure for
   a suffix automaton with v vertices and e edges */
Graph newSuffixAutomaton(int v, int e) {
   Graph aut;

   aut = newAutomaton(v, e);
   memset(aut->target, UNDEFINED, e*sizeof(int));
   aut->suffixLink = (int *)calloc(v, sizeof(int));
   if (aut->suffixLink == NULL)
      error("newSuffixAutomaton");
   aut->length = (int *)calloc(v, sizeof(int));
   if (aut->length == NULL)
      error("newSuffixAutomaton");
   aut->position = (int *)calloc(v, sizeof(int));
   if (aut->position == NULL)
      error("newSuffixAutomaton");
   aut->shift = (int *)calloc(e, sizeof(int));
   if (aut->shift == NULL)
      error("newSuffixAutomaton");
   return(aut);
}
 
 
/* returns a new data structure for
   a trie with v vertices and e edges */
Graph newTrie(int v, int e) {
   Graph aut;
 
   aut = newAutomaton(v, e);
   memset(aut->target, UNDEFINED, e*sizeof(int));
   aut->suffixLink = (int *)calloc(v, sizeof(int));
   if (aut->suffixLink == NULL)
      error("newTrie");
   aut->length = (int *)calloc(v, sizeof(int));
   if (aut->length == NULL)
      error("newTrie");
   aut->position = (int *)calloc(v, sizeof(int));
   if (aut->position == NULL)
      error("newTrie");
   aut->shift = (int *)calloc(e, sizeof(int));
   if (aut->shift == NULL)
      error("newTrie");
   return(aut);
}


/* returns a new vertex for graph g */
int newVertex(Graph g) {
   if (g != NULL && g->vertexCounter <= g->vertexNumber)
      return(g->vertexCounter++);
   error("newVertex");
}


/* returns the initial vertex of graph g */
int getInitial(Graph g) {
   //if (g != NULL)
      return(g->initial);
   //error("getInitial");
}


/* returns true if vertex v is terminal in graph g */
boolean isTerminal(Graph g, int v) {
   //if (g != NULL && g->terminal != NULL && v < g->vertexNumber)
      return(g->terminal[v]);
   //error("isTerminal");
}


/* set vertex v to be terminal in graph g */
void setTerminal(Graph g, int v) {
   if (g != NULL && g->terminal != NULL &&   v < g->vertexNumber)
      g->terminal[v] = 1;
   else
      error("isTerminal");
}


/* returns the target of edge from vertex v
   labelled by character c in graph g */
int getTarget(Graph g, int v, unsigned char c) {
   //if (g != NULL && g->target != NULL && v < g->vertexNumber && v*c < g->edgeNumber)
      return(g->target[v*(g->edgeNumber/g->vertexNumber) +
                       c]);
   //error("getTarget");
}


/* add the edge from vertex v to vertex t
   labelled by character c in graph g */
void setTarget(Graph g, int v, unsigned char c, int t) {
   if (g != NULL && g->target != NULL &&
       v < g->vertexNumber &&
       v*c <= g->edgeNumber && t < g->vertexNumber)
      g->target[v*(g->edgeNumber/g->vertexNumber) + c] = t;
   else
      error("setTarget");
}


/* returns the suffix link of vertex v in graph g */
int getSuffixLink(Graph g, int v) {
   //if (g != NULL && g->suffixLink != NULL && v < g->vertexNumber)
      return(g->suffixLink[v]);
   //error("getSuffixLink");
}


/* set the suffix link of vertex v
   to vertex s in graph g */
void setSuffixLink(Graph g, int v, int s) {
   if (g != NULL && g->suffixLink != NULL &&
       v < g->vertexNumber && s < g->vertexNumber)
      g->suffixLink[v] = s;
   else
      error("setSuffixLink");
}


/* returns the length of vertex v in graph g */
int getLength(Graph g, int v) {
   //if (g != NULL && g->length != NULL &&  v < g->vertexNumber)
      return(g->length[v]);
   //error("getLength");
}


/* set the length of vertex v to integer ell in graph g */
void setLength(Graph g, int v, int ell) {
   if (g != NULL && g->length != NULL &&
       v < g->vertexNumber)
      g->length[v] = ell;
   else
      error("setLength");
}


/* returns the position of vertex v in graph g */
int getPosition(Graph g, int v) {
   if (g != NULL && g->position != NULL &&
       v < g->vertexNumber)
      return(g->position[v]);
   error("getPosition");
}


/* set the length of vertex v to integer ell in graph g */
void setPosition(Graph g, int v, int p) {
   if (g != NULL && g->position != NULL &&
       v < g->vertexNumber)
      g->position[v] = p;
   else
      error("setPosition");
}


/* returns the shift of the edge from vertex v
   labelled by character c in graph g */
int getShift(Graph g, int v, unsigned char c) {
   //if (g != NULL && g->shift != NULL && v < g->vertexNumber && v*c < g->edgeNumber)
      return(g->shift[v*(g->edgeNumber/g->vertexNumber) +
             c]);
   //error("getShift");
}


/* set the shift of the edge from vertex v
   labelled by character c to integer s in graph g */
void setShift(Graph g, int v, unsigned char c, int s) {
   if (g != NULL && g->shift != NULL &&
       v < g->vertexNumber && v*c <= g->edgeNumber)
      g->shift[v*(g->edgeNumber/g->vertexNumber) + c] = s;
   else
      error("setShift");
}


/* copies all the characteristics of vertex source
   to vertex target in graph g */
void copyVertex(Graph g, int target, int source) {
   if (g != NULL && target < g->vertexNumber &&
       source < g->vertexNumber) {
      if (g->target != NULL)
         memcpy(g->target +
                target*(g->edgeNumber/g->vertexNumber),
                g->target +
                source*(g->edgeNumber/g->vertexNumber),
                (g->edgeNumber/g->vertexNumber)*
                sizeof(int));
      if (g->shift != NULL)
         memcpy(g->shift + target*(g->edgeNumber/g->vertexNumber),
                g->shift + source*(g->edgeNumber/g->vertexNumber),
                (g->edgeNumber/g->vertexNumber)* sizeof(int));
      if (g->terminal != NULL)
         g->terminal[target] = g->terminal[source];
      if (g->suffixLink != NULL)
         g->suffixLink[target] = g->suffixLink[source];
      if (g->length != NULL)
         g->length[target] = g->length[source];
      if (g->position != NULL)
         g->position[target] = g->position[source];
   }
   else
      error("copyVertex");
}

/******************************************/

int TableGraph(int **trans, Graph g) {
	int i;
	unsigned char c;
	for(i=0; i<g->vertexNumber; i++) 
		for(c=0; c<g->edgeNumber; c++)
			trans[i][c] = getTarget(g,i,c);	
	return 1;
  };

