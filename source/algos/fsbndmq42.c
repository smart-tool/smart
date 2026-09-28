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
 * download the tool at:  https://github.com/rurban/smart/
 *
 * This is an implementation of the Forward Semplified BNDM algorithm using
 * q-grams in Hannu Peltola and Jorma Tarhio Variations of Forward-SBNDM
 * Proceedings of the Prague Stringology Conference 2011, pp.3--14, Czech
 * Technical University in Prague, Czech Republic, (2008). Q is the dimension of
 * q-grams F is the number of forward characters
 *
 * Note: Broken!
 * Constraints: requires m>=4
 */

#define MIN_M 4
#define MAX_M 30
#include "include/define.h"
#include "include/main.h"
#include "include/search_small.h"
#include "include/search_large.h"

#define Q 4
#define F 2

int search(unsigned char *x, int m, unsigned char *y, int n) {
  return search_safe(x, m, y, n);
}
