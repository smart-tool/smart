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


int *PATT_SIZE;
#define NumPatt 17    // maximal number of pattern lengths
int PATT_CUSTOM_SIZE[NumPatt];
int PATT_VERY_SHORT[NumPatt] = {1,  2,  3,  4,  5,  6,  7,  8, 9,
                                10, 11, 12, 13, 14, 15, 16, 0};
int PATT_SHORT_SIZE[NumPatt] = {2,  4,  6,  8,  10, 12, 14, 16, 18,
                                20, 22, 24, 26, 28, 30, 32, 0};
int PATT_LARGE_SIZE[NumPatt] = {2,    4,    8,    16, 32, 64, 128, 256, 512,
                                1024, 2048, 4096, 0,  0,  0,  0,   0};
#define NumSetting 15 // number of text buffers
char *SETTING_BUFFER[NumSetting] = {
    "rand2",       "rand4",        "rand8",     "rand16",       "rand32",
    "rand64",      "rand128",      "rand250",   "italianTexts", "englishTexts",
    "frenchTexts", "chineseTexts", "midimusic", "genome",       "protein"};
int SETTING_ALPHA_SIZE[NumSetting] = {2,   4,   8,   16,  32,  64, 128, 250,
                                      128, 128, 128, 128, 128, 64, 64};

#include "algorithms.h"

int EXECUTE[NumAlgo];
const char *ALGO_NAME[NumAlgo];
const char *ALGO_DESCRIPTION[NumAlgo];

#include "function.h"
