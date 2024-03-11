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

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define FALSE 0
#define TRUE 1
#define XSIZE 4200 // maximal length of the pattern
#define WSIZE 256  // greater int value fitting in a computer word
#define SIGMA 256  // constant alphabet size
//#define ALPHA       256				//constant alphabet size
//#define ASIZE		256				//constant alphabet size
#define UNDEFINED -1
#define HALFDEFINED -2
#define WORD 32 // computer word size (in bit)
#ifdef DEBUG
#define OUTPUT(j)                                                              \
  {                                                                            \
    fprintf(stderr, "found at y[%d] %.*s line:%u\n", (j), m, &y[(j)], __LINE__); \
    count++;                                                                   \
  }
#define OUTPUTP(j)                                                             \
  {                                                                            \
    fprintf(stderr, "found at y[%d]\n", (j));                                  \
    (*count)++;                                                                \
  }
#else
#define OUTPUT(j) count++
#define OUTPUTP(j) (*count)++
#endif
#define M_CUTOFF 32 // when to switch to malloc

#ifdef __GNUC__
#define ATTRIBUTE_MALLOC __attribute__((malloc))
#else
#define ATTRIBUTE_MALLOC
#endif

#if !(defined(_WIN32) || defined(__AVR__))
#define HAVE_SHM
#endif

#ifdef __GNUC__
# define HAVE_POPCOUNT
# if __SIZEOF_LONG_LONG__ >= 8
#  define HAVE_POPCOUNT64
# endif
#elif defined _MSC_VER
# define HAVE_POPCOUNT
# if _INTEGRAL_MAX_BITS >= 64
#  define HAVE_POPCOUNT64
# endif
#endif

#ifdef HAVE_POPCOUNT
# ifndef _MSC_VER
#  define POPCOUNT16(x) __builtin_popcount(x)
# else
#  define POPCOUNT16(x) __popcount16(x)
# endif
#else
# define POPCOUNT16(x) PopCount[x]
#endif
#ifdef HAVE_POPCOUNT64
# ifndef _MSC_VER
#  define POPCOUNT64(x) __builtin_popcountll(x)
# else
#  define POPCOUNT64(x) _popcount64(x)
# endif
#endif
