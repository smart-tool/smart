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

#include "timer.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../searchinfo.h"

#define BEGIN_PREPROCESSING	{timer_start(_timer);start = clock();}
#define BEGIN_SEARCHING		{timer_start(_timer);start = clock();}
#define END_PREPROCESSING	{timer_stop(_timer);end = clock();(*pre_time) = timer_elapsed(_timer)*1000;}
#define END_SEARCHING		{timer_stop(_timer);end = clock();(*run_time) = timer_elapsed(_timer)*1000;}

/* global variables used for computing preprocessing and searching times */
double *run_time, 		// searching time
*pre_time;	// preprocessing time
clock_t start, end;
TIMER * _timer;


/*********************************************************************************************************
 * Main interface to implement in each search algorithm supporting runtime statistics.
 */

/*
 * The normal search function, which is timed.
 * Use timing macros BEGIN_PREPROCESSING, END_PREPROCESSING, BEGIN_SEARCHING and END_SEARCHING.
 * Don't measure any searchInfo stats in this function, or the timings will be negatively affected.
 */
int search(unsigned char* p, int m, unsigned char* t, int n);

/*
 * An instrumented version of the search function that is not timed, but returns running statistics.
 * Take a copy of the normal search method and modify it, taking out the timing macros, and
 * recording the runtime statistics of the algorithm instead, returning searchInfo rather than int.
 */
struct searchInfo searchStats(unsigned char* p, int m, unsigned char* t, int n);

/*
 * A function that returns any special algorithm statistic names used in the searchStats function.
 * If only the standard stats are returned, all the names should be empty.
 * If any special stats are defined, return names for those statistics here.
 * A standard set of algo value stat names for many bit oriented algorithms is provided in "bitstats.h",
 * (a count of empty table slots and a count of total bits set in the table), so if you need to provide
 * those stats, just include the header - you don't need to implement this method.
 */
struct algoValueNames getAlgoValueNames();


/*********************************************************************************************************
 * Convenience function for testing a pattern against a position in the text an updating searchInfo
 * to reflect how many bytes were read and to increment the match count if a match was found.
 *
 * This should replace calls to memcmp() when testing for a pattern match in algorithms that use it.
 *
 * It returns the number of matching bytes found when matching the pattern.
 */
int matchTest(struct searchInfo *info, unsigned char *p, int m, unsigned char *t, int n, int pos) {
    info->validationCount++;
    if (pos + m > n) {
        return 0;
    }
    for (int i = 0; i < m; i++) {
        info->textBytesRead++;
        info->validationBytesRead++;
        if (p[i] != t[pos + i]) {
            return i;
        }
    }
    info->matchCount++;
    return m;
}


/*********************************************************************************************************
 * Convenience functions to calculate various table statistics.
 */

/*
 * Counts the bits set in an unsigned integer.
 * This is Brian Kernighan's method for counting bits,
 * from https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetKernighan
 */
int countBitsSet(unsigned int value) {
    unsigned int c; // c accumulates the total bits set in v
    for (c = 0; value; c++)
    {
        value &= value - 1; // clear the least significant bit set
    }
    return c;
}

/*
 * Counts the number of empty table slots and the total bits set in a table of integers,
 * and places the results in algorithm statistics with the given indices.
 */
void countEmptySlotsAndBitsSet(int bitValueIndex, int emptyValueIndex, struct searchInfo * searchInfo, int *table, int numEntries) {
    for (int i = 0; i < numEntries; i++) {
        if (table[i]) {
            searchInfo->algoValues[bitValueIndex] += countBitsSet(table[i]);
        } else {
            searchInfo->algoValues[emptyValueIndex] += 1;
        }
    }
}

/*
 * Counts the number of empty table slots and the total bits set in a table of unsigned integers,
 * and places the results in algorithm statistics with the given indices.
 */
void countEmptySlotsAndBitsSetU(int bitValueIndex, int emptyValueIndex, struct searchInfo * searchInfo, unsigned int *table, int numEntries) {
    for (int i = 0; i < numEntries; i++) {
        if (table[i]) {
            searchInfo->algoValues[bitValueIndex] += countBitsSet(table[i]);
        } else {
            searchInfo->algoValues[emptyValueIndex] += 1;
        }
    }
}

/*
 * Counts the number of empty table slots and the total bits set in a table of long,
 * and places the results in algorithm statistics with the given indices.
 */
void countEmptySlotsAndBitsSetL(int bitValueIndex, int emptyValueIndex, struct searchInfo * searchInfo, long *table, int numEntries) {
    for (int i = 0; i < numEntries; i++) {
        if (table[i]) {
            searchInfo->algoValues[bitValueIndex] += countBitsSet(table[i]);
        } else {
            searchInfo->algoValues[emptyValueIndex] += 1;
        }
    }
}

/*
 * Counts the number of empty table slots and the total bits set in a table of unsigned longs,
 * and places the results in algorithm statistics with the given indices.
 */
void countEmptySlotsAndBitsSetUL(int bitValueIndex, int emptyValueIndex, struct searchInfo * searchInfo, unsigned long *table, int numEntries) {
    for (int i = 0; i < numEntries; i++) {
        if (table[i]) {
            searchInfo->algoValues[bitValueIndex] += countBitsSet(table[i]);
        } else {
            searchInfo->algoValues[emptyValueIndex] += 1;
        }
    }
}

/*
 * Counts the number of empty table slots and the total bits set in a table of char,
 * and places the results in algorithm statistics with the given indices.
 */
void countEmptySlotsAndBitsSetC(int bitValueIndex, int emptyValueIndex, struct searchInfo * searchInfo, char *table, int numEntries) {
    for (int i = 0; i < numEntries; i++) {
        if (table[i]) {
            searchInfo->algoValues[bitValueIndex] += countBitsSet(table[i]);
        } else {
            searchInfo->algoValues[emptyValueIndex] += 1;
        }
    }
}

/*
 * Adds up all the integers in a table of integers, and places the result in an algorithm statistic with the given index.
 */
void sumTable(int sumValueIndex, struct searchInfo* searchInfo, int *table, int numEntries) {
    int sum = 0;
    for (int i = 0; i < numEntries; i++) {
        sum += table[i];
    }
    searchInfo->algoValues[sumValueIndex] = sum;
}


/*********************************************************************************************************
 * Main
 */

int main(int argc, char *argv[])
{
    _timer = (TIMER*) malloc (sizeof(TIMER));
    unsigned char *p, *t;
    int m, n;
    if(argc==2 && !strcmp("-stats", argv[1])) {
        return 0;
    }
    if(!strcmp("-statnames", argv[1])) { // invoke with just -statnames to print a list of the names of any algo stats provided.
        struct algoValueNames names = getAlgoValueNames();
        printAlgoValueNames("%s\n", &names);
    } else  if(!strcmp("shared", argv[1])) {
        if (argc == 4 && !strcmp("-statnames", argv[2])) { //{progname} shared -statnames {shared mem for algo names struct}
            struct algoValueNames *algoNames;
            int snshmid;
            key_t snkey = atoi(argv[3]);
            if ((snshmid = shmget(snkey, sizeof(struct algoValueNames), 0666)) < 0) {
                perror("shmget");
                return 1;
            }
            /* Now we attach the segment to our data space. */
            if ((algoNames = shmat(snshmid, NULL, 0)) == (struct algoValueNames *) -1) {
                perror("shmat");
                return 1;
            }
            struct algoValueNames names = getAlgoValueNames();
           // printf("\nGot names:\n");
          //  printAlgoValueNames("%s\n", &names);

            (*algoNames) = names;
            return 0;
        }
        if(argc != 8 && argc != 9) {
            printf("error in input parameter:\nfour, six or seven parameters needed when used with shared memory\n");
            return 1;
        }
        int pshmid, tshmid, eshmid, preshmid, sishmid;
        /* Locate the pattern. */
        key_t pkey = atoi(argv[2]); //segment name for the pattern
        m = atoi(argv[3]); //segment size for the pattern
        if ((pshmid = shmget(pkey, m, 0666)) < 0) {
            perror("shmget");
            return 1;
        }
        /* Now we attach the segment to our data space. */
        if ((p = shmat(pshmid, NULL, 0)) == (unsigned char *) -1) {
            perror("shmat");
            return 1;
        }

        /* Locate the text. */
        key_t tkey = atoi(argv[4]); //segment name for the text
        n = atoi(argv[5]); //segment size for the text
        if ((tshmid = shmget(tkey, n, 0666)) < 0) {
            perror("shmget");
            return 1;
        }
        /* Now we attach the segment to our data space. */
        if ((t = shmat(tshmid, NULL, 0)) == (unsigned char *) -1) {
            perror("shmat");
            return 1;
        }

        /* 8 arguments are for statistics: {progname}, 'shared', pattern, pattern_length, text, text_length, '-stats', searchInfo */
        if (argc == 8) {
            if (strcmp("-stats", argv[6])) {
                printf("error in input parameter:\nthe sixth parameter should be '-stats' with seven parameters.\n");
                return 1;
            }

            struct searchInfo *searchInfo;
            key_t skey = atoi(argv[7]); // segment name for search info.
            if ((sishmid = shmget(skey, sizeof(struct searchInfo), 0666)) < 0) {
                perror("shmget");
                return 1;
            }
            /* Now we attach the segment to our search info variable space. */
            if ((searchInfo = shmat(sishmid, NULL, 0)) == (struct searchInfo *) -1) {
                perror("shmat");
                return 1;
            }

            (*searchInfo) = searchStats(p, m, t, n);
            return 0;

        } else { // 9 arguments are for benchmarking: {progname}, 'shared', pattern, pattern_length, text, text_length, result, search time, pre processing time.
            /* Locate the running time variable */
            key_t ekey = atoi(argv[7]); //segment name for the running time
            if ((eshmid = shmget(ekey, 8, 0666)) < 0) {
                perror("shmget");
                return 1;
            }
            /* Now we attach the segment to our time variable space. */
            if ((run_time = shmat(eshmid, NULL, 0)) == (double *) -1) {
                perror("shmat");
                return 1;
            }

            /* Locate the preprocessing running time variable */
            key_t prekey = atoi(argv[8]); //segment name for the preprocessing running time
            if ((preshmid = shmget(prekey, 8, 0666)) < 0) {
                perror("shmget");
                return 1;
            }
            /* Now we attach the segment to our time variable space. */
            if ((pre_time = shmat(preshmid, NULL, 0)) == (double *) -1) {
                perror("shmat");
                return 1;
            }

            //timer_start(_timer);
            //start = clock();
            int count = search(p,m,t,n);
            //timer_stop(_timer);
            //end = clock();
            //(*run_time) = timer_elapsed(_timer)*1000;

            int rshmid, *result;
            key_t rkey = atoi(argv[6]); //segment name for the occurrences
            // Locate the int value.
            if ((rshmid = shmget(rkey, 4, 0666)) < 0) {
                perror("shmget");
                return 1;
            }
            // Now we attach the segment to our data space.
            if ((result = shmat(rshmid, NULL, 0)) == (int *) -1) {
                perror("shmat");
                return 1;
            }
            *result = count;
            return 0;
        }
    }
    else {
        if(argc < 5) {
            printf("error in input parameter\nfour parameters needed in standard mode\n");
            return 1;
        }
        p = (unsigned char*) argv[1];
        m = atoi(argv[2]);
        t = (unsigned char*) argv[3];
        n = atoi(argv[4]);
        if (argc == 6 && !strcmp("-stats", argv[5])) {
            const struct searchInfo searchInfo = searchStats(p, m, t, n);
            const struct algoValueNames names = getAlgoValueNames();
            printSearchInfo("%-32s\t%d\n", &searchInfo, &names);
        } else {
            int occ = search(p, m, t, n);
            printf("found %d occurrences\n", occ);
            return 0;
        }
    }
}



