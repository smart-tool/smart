/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012  Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * contact the authors at: faro@dmi.unict.it, thierry.lecroq@univ-rouen.fr
 * download the tool at:  https://github.com/rurban/smart/
 *
 * Backward Set Oracle Matching (BSOM).  The SMART search interface supplies
 * one pattern, so this builds the factor oracle for its singleton word set.
 * The search and shift rules are those of Set Backward Oracle Matching.
 */

#include "include/define.h"
#include "include/main.h"

struct bsom_transition {
  unsigned char c;
  int target;
  int next;
};

static int transition(const int *head, const struct bsom_transition *trans,
                      int state, unsigned char c) {
  int edge;

  for (edge = head[state]; edge != UNDEFINED; edge = trans[edge].next)
    if (trans[edge].c == c)
      return trans[edge].target;
  return UNDEFINED;
}

static void add_transition(int *head, struct bsom_transition *trans,
                           int *edge_count, int state, unsigned char c,
                           int target) {
  trans[*edge_count].c = c;
  trans[*edge_count].target = target;
  trans[*edge_count].next = head[state];
  head[state] = (*edge_count)++;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  int *head, *supply;
  struct bsom_transition *trans;
  int count = 0;
  int edge_count = 0;
  int i, j, q = UNDEFINED, down;

  if (m <= 0 || n < m)
    return 0;

  BEGIN_PREPROCESSING
  head = (int *)malloc(((size_t)m + 1) * sizeof(*head));
  supply = (int *)malloc(((size_t)m + 1) * sizeof(*supply));
  trans = (struct bsom_transition *)malloc((size_t)m * 2 * sizeof(*trans));
  if (head == NULL || supply == NULL || trans == NULL) {
    free(trans);
    free(supply);
    free(head);
    END_PREPROCESSING
    return 0;
  }

  for (i = 0; i <= m; i++)
    head[i] = UNDEFINED;

  /* Build the trie for the reversed singleton set. */
  for (i = 0; i < m; i++)
    add_transition(head, trans, &edge_count, i, x[m - 1 - i], i + 1);

  /* Add factor-oracle transitions using supply links. */
  supply[0] = UNDEFINED;
  for (i = 0; i < m; i++) {
    const int current = i + 1;
    const unsigned char c = x[m - 1 - i];

    down = supply[i];
    while (down != UNDEFINED) {
      q = transition(head, trans, down, c);
      if (q != UNDEFINED)
        break;
      add_transition(head, trans, &edge_count, down, c, current);
      down = supply[down];
    }
    supply[current] = down == UNDEFINED ? 0 : q;
  }
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (i = 0; i <= n - m;) {
    q = 0;
    j = m - 1;
    while (j >= 0 && (q = transition(head, trans, q, y[i + j])) != UNDEFINED)
      j--;

    if (j < 0 && !memcmp(x, y + i, (size_t)m))
      OUTPUT(i);

    if (j <= 0)
      i++;
    else
      i += j + 1;
  }
  END_SEARCHING

  free(trans);
  free(supply);
  free(head);
  return count;
}
