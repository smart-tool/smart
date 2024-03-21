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
 */

#define _GNU_SOURCE
#include "include/define.h"
#include "include/main.h"

int search(unsigned char *x, int m, unsigned char *y, int n) {
  //const unsigned char *orig_y = y;
  BEGIN_PREPROCESSING
  END_PREPROCESSING

  /* Searching */
  BEGIN_SEARCHING
  int count = 0;
  unsigned char *p;
  while ((p = memmem((char *)y, (size_t)n, (char *)x, (size_t)m))) {
    OUTPUT(p - orig_y);
    n -= (p + 1) - y;
    y = p + 1; // can be optimized
  }
  END_SEARCHING
  return count;
}
