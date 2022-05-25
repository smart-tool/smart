//
// Created by matt on 29/04/2022.
//

#ifndef SMART_SEARCHINFO_H
#define SMART_SEARCHINFO_H

#include <stdio.h>
#include <string.h>

#define MAX_ALGO_VALUES 8
#define MAX_ALGO_VALUE_NAME_LEN 32

/*
 * A struct holding information about the operation of a search algorithm when searching a text.
 *
 * Notes on adapting search algorithms to gather searchInfo:
 * =========================================================
 *
 * Pattern validation with memcmp()
 * ================================
 * Many algorithms use memcmp() to efficiently test for a pattern match.
 * We usually want to count the number of bytes read for validation, so calls to memcmp should be replaced
 * by calls to matchTest(), defined in mainstats.h:
 *
 *   if(memcmp(y+k,x,m) == 0) count++;
 *
 * should be replaced with:
 *
 *   matchTest(&searchInfo, x, m, y, n, k);
 *
 * passing in the address of your searchInfo struct, along with the pattern, pattern length, text, text length and pos
 * to match at.  This will increment the appropriate measurements in the searchInfo, including the match count if a
 * match is found.  It returns the number of bytes that successfully matched, so if you want to do something else with
 * a match other than increment the match count, you can write:
 *
 *   if (matchTest(&searchInfo, x, m, y, n, k) == m) {
 *     ... do whatever ...
 *   }
 *
 *
 * while loop counting
 * ===================
 * Many algorithms use a while loop in which text bytes or index lookups are made in the main while condition, e.g.
 *
 *    while (F[y[i]] == 0) {
 *       ...
 *    }
 *
 * To accurately count the text bytes read and index lookups made, they should be pre-incremented in the while condition,
 * as the *first* conditions to appear.  If you count them *inside* the loop, you will fail to count reads made when the
 * main while test condition failed.
 *
 *    while (++results.textBytesRead && ++results.indexLookupCount && F[y[i]] == 0) {
 *       ...
 *    }
 *
 */
struct searchInfo {
    /*
     * Whether the struct contains any searchInfo.
     * 0 indicates no searchInfo, 1 indicates there is info.
     */
    char hasStats;

    /*
     * The length of the text searched.
     */
    long textLength;

    /*
     * The number of bytes used by any search indices.
     */
    long searchIndexBytes;

    /*
     * The number of entries available in the main search index.
     */
    long searchIndexEntries;

    /*
     * The number of entries available in any secondary search index, or zero if no such entries are used.
     */
    long searchIndex2Entries;

    /*
     * How many times the main search loop executes.
     */
    long mainLoopCount;

    /*
     * The total bytes read in the text by the search algorithm.
     * This should include bytes read in both the searching and validation phases of the algorithm.
     */
    long textBytesRead;

    /*
     * The number of times a search index is accessed.
     */
    long indexLookupCount;

    /*
     * How many times the algorithm follows its fast path.
     * If the fast path is taken repeatedly in a loop inside the main loop, each iteration of the fast path
     * loop should be counted to better understand how much work is being done (rather than just counting
     * that a fast path was entered once).
     * Not all algorithms have a special fast path, but many do.
     */
    long fastPathCount;

    /*
     * The sum of shifts obtained when the algorithm operates in its fast path.
     */
    long fastPathShifts;

    /*
     * How many times the algorithm follows its slow path.
     * If the slow path is taken repeatedly in a loop inside the main loop, each iteration of the slow path
     * loop should be counted to better understand how much work is being done (rather than just counting
     * that a slow path was entered once).
     * Not all algorithms have a slow path, but many do.
     */
    long slowPathCount;

    /*
     * The sum of shifts obtained when the algorithm operates in its slow path.
     */
    long slowPathShifts;

    /*
     * The number of times a match validation occurs, regardless of whether a match is found.
     */
    long validationCount;

    /*
     * The number of bytes read during pattern validation.
     * Note: a lot of algorithms use memcmp() to determine a match quickly, but this doesn't tell us how many
     * bytes have been read.
     * mainstats.h defines a utility function "matchTest()" which performs all match validation at a position
     * in the text and updates all of searchInfo fields appropriately.  This should always be used in preference
     * to memcmp(), but it's usually easier to use it even if memcmp isn't being used, just because it updates
     * all the relevant searchInfo fields itself.
     */
    long validationBytesRead;

    /*
     * The sum of shifts obtained after the algorithm has performed a validation.
     */
    long validationShifts;

    /*
     * The count of the number of times the algorithm shifts ahead (not the sum of the shifts themselves).
     * When the number of shifts made is low, then the average length of the shifts obtained must be large,
     * and vice versa.  Knowing the total shifts made is not interesting, as it always adds up to about the
     * text length by the end of the search.  Knowing how many times a shift was made lets us understand
     * the average shifts obtained in the run.
     *
     * Note: we only count a shift made when the algorithm makes use of it.
     * If an algorithm during its main loop adds up a few different shifts to get the final shift, we only count
     * them once.  It's not interesting that a final shift is determined over several calculations.
     * It is interesting how many times the algorithm gets to actually use a shift.
     */
    long numShifts;

    /*
     * The number of occurrences found by the algorithm.
     */
    int matchCount;

    /*
     * Per algorithm statistic values.
     * Set any of these values, and set a corresponding algorithm name in the getAlgoValueNames() function.
     * If a name is set, then the statistic will be processed.
     */
    long algoValues[MAX_ALGO_VALUES];
};

/*
 * An enum for each of the different stat types.
 */
enum searchInfoStats {
    PATTERN_LENGTH,
    TEXT_LENGTH,
    SEARCH_INDEX_BYTES,
    SEARCH_INDEX_ENTRIES,
    SEARCH_INDEX2_ENTRIES,
    MAIN_LOOP_COUNT,
    TEXT_BYTES_READ,
    INDEX_LOOKUP_COUNT,
    FAST_PATH_COUNT,
    FAST_PATH_SHIFTS,
    SLOW_PATH_COUNT,
    SLOW_PATH_SHIFTS,
    VALIDATION_COUNT,
    VALIDATION_BYTES_READ,
    VALIDATION_SHIFTS,
    NUM_SHIFTS,
    MATCH_COUNT,
    ALGO_VALUES
};

/*
 * Returns the name of a stat given the enum for the stat.
 * If you want the actual names of the per algorithm algo values, this must be obtained from the algoValueNames struct.
 */
const char* getStatName(enum searchInfoStats stat) {
    switch (stat) {
        case PATTERN_LENGTH : return "Pattern length";
        case TEXT_LENGTH: return "Text length";
        case SEARCH_INDEX_BYTES: return "Search index bytes";
        case SEARCH_INDEX_ENTRIES: return "Search index entries";
        case SEARCH_INDEX2_ENTRIES: return "Search index 2 entries";
        case MAIN_LOOP_COUNT: return "Main loop count";
        case TEXT_BYTES_READ: return "Text bytes read";
        case INDEX_LOOKUP_COUNT: return "Index lookup count";
        case FAST_PATH_COUNT: return "Fast path count";
        case FAST_PATH_SHIFTS: return "Fast path shifts";
        case SLOW_PATH_COUNT: return "Slow path count";
        case SLOW_PATH_SHIFTS: return "Slow path shifts";
        case VALIDATION_COUNT: return "Validation count";
        case VALIDATION_BYTES_READ: return "Validation bytes read";
        case VALIDATION_SHIFTS: return "Validation shifts";
        case NUM_SHIFTS: return "Number of shifts";
        case MATCH_COUNT: return "Number of matches";
        case ALGO_VALUES: return "Per algorithm stats";
    }
    return "{Statname not defined}";
}

/*
 * Returns the value of a stat in search info, given an enum for the stat.
 * If you want the per algorithm stat values for the ALGO_VALUES enum, you must get that from the appropriate
 * index in the searchStruct.algoValues[] array.  -1 will be returned if you ask for the ALGO_VALUES stat values.
 */
const long getStatValue(enum searchInfoStats stat, struct searchInfo* info) {
    switch (stat) {
        case TEXT_LENGTH: return info->textLength;
        case SEARCH_INDEX_BYTES: return info->searchIndexBytes;
        case SEARCH_INDEX_ENTRIES: return info->searchIndexEntries;
        case SEARCH_INDEX2_ENTRIES : return info->searchIndex2Entries;
        case MAIN_LOOP_COUNT: return info->mainLoopCount;
        case TEXT_BYTES_READ: return info->textBytesRead;
        case INDEX_LOOKUP_COUNT: return info->indexLookupCount;
        case FAST_PATH_COUNT: return info->fastPathCount;
        case FAST_PATH_SHIFTS: return info->fastPathShifts;
        case SLOW_PATH_COUNT: return info->slowPathCount;
        case SLOW_PATH_SHIFTS: return info->slowPathShifts;
        case VALIDATION_COUNT: return info->validationCount;
        case VALIDATION_BYTES_READ: return info->validationBytesRead;
        case VALIDATION_SHIFTS: return info->validationShifts;
        case NUM_SHIFTS: return info->numShifts;
        case MATCH_COUNT: return info->matchCount;
    }
    return -1;
}

/*
 * A struct holding an array of names for algorithm-specific stats.
 */
struct algoValueNames {
    /*
     * A short name for the statistic, which appears in the console output when benchmarking.
     * The short name controls whether a statistic is processed.
     * If it is empty, no stats will be reported, even if a long name is set.
     */
    char shortName[MAX_ALGO_VALUES][MAX_ALGO_VALUE_NAME_LEN];

    /*
     * A slightly longer name, which appears in the final report.
     */
    char longName[MAX_ALGO_VALUES][MAX_ALGO_VALUE_NAME_LEN];
};


/*
 * A default struct indicating no results are available.
 */
struct searchInfo NO_ALGO_RESULTS = {0};


/*
 * A default struct indicating no algorithm stat names.
 */
struct algoValueNames NO_ALGO_NAMES = {0};


/*
 * A convenience method to initialise the stats with a number of entries and the size of each entry.
 */
void initStats(struct searchInfo* info, int n, int entries, int entrySize) {
    info->hasStats = 1;
    info->textLength = n;
    info->searchIndexEntries = entries;
    info->searchIndexBytes = entries * entrySize;
}

/*
 * A convenience method to initialise the stats with a number of entries and the size of each entry,
 * for both a main search index and a secondary search index.
 */
void initStats2(struct searchInfo* info, int n, int entries, int entrySize, int entries2, int entry2Size) {
    info->hasStats = 1;
    info->textLength = n;
    info->searchIndexEntries = entries;
    info->searchIndex2Entries = entries2;
    info->searchIndexBytes = (entries * entrySize) + (entries2 * entry2Size);
}


/*
 * A convenience method to initialize stats with the number of entries and the total number of bytes.
 * This is for times when the memory size isn't computable by a single set of entries with a single size
 * (e.g. some algorithms have two tables using different entry sizes for each).
 */
void initStatsBytes(struct searchInfo* info, int n, int entries, int bytes) {
    info->hasStats = 1;
    info->textLength = n;
    info->searchIndexEntries = entries;
    info->searchIndexBytes = bytes;
}


/*
 * A convenience method to set an algorithm stat name.
 */
int setAlgoValueName(struct algoValueNames *names, int index, const char *shortName, const char *longName) {
    if (index >= 0 && index < MAX_ALGO_VALUES) {
        strncpy(names->shortName[index], shortName, MAX_ALGO_VALUE_NAME_LEN);
        strncpy(names->longName[index], longName, MAX_ALGO_VALUE_NAME_LEN);
        return 0;
    }
    printf("WARNING: cannot set algo name at index %d, valid range is 0 to %d", index, MAX_ALGO_VALUES - 1);
    return 1;
}


/*
 * A method to initialize a stat struct with default values.
 */
void clearSearchInfo(struct searchInfo *info) {
    info->hasStats = 0;
    info->textLength = 0;
    info->searchIndexBytes=0;
    info->searchIndexEntries=0;
    info->searchIndex2Entries=0;
    info->mainLoopCount=0;
    info->textBytesRead=0;
    info->indexLookupCount=0;
    info->fastPathCount=0;
    info->fastPathShifts=0;
    info->slowPathCount=0;
    info->slowPathShifts=0;
    info->validationCount=0;
    info->validationBytesRead=0;
    info->validationShifts=0;
    info->numShifts=0;
    info->matchCount=0;
    for (int i = 0; i < MAX_ALGO_VALUES; i++) {
        info->algoValues[i] = 0;
    }
}

/*
 * Calculates the average stat info given a set of totals and the number of samples.
 * The average results are placed in the averageResuts struct passed in.
 */
void buildAverageSearchInfo(struct searchInfo *infoTotals, int samples, struct searchInfo *averageResults) {
    averageResults->hasStats = 1;
    averageResults->textLength = infoTotals->textLength / samples;
    averageResults->searchIndexBytes = infoTotals->searchIndexBytes / samples;
    averageResults->searchIndexEntries = infoTotals->searchIndexEntries / samples;
    averageResults->searchIndex2Entries = infoTotals->searchIndex2Entries / samples;
    averageResults->mainLoopCount = infoTotals->mainLoopCount / samples;
    averageResults->textBytesRead = infoTotals->textBytesRead / samples;
    averageResults->indexLookupCount = infoTotals->indexLookupCount / samples;
    averageResults->fastPathCount = infoTotals->fastPathCount / samples;
    averageResults->fastPathShifts = infoTotals->fastPathShifts / samples;
    averageResults->slowPathCount = infoTotals->slowPathCount / samples;
    averageResults->slowPathShifts = infoTotals->slowPathShifts / samples;
    averageResults->validationCount = infoTotals->validationCount / samples;
    averageResults->validationBytesRead = infoTotals->validationBytesRead / samples;
    averageResults->validationShifts = infoTotals->validationShifts / samples;
    averageResults->numShifts = infoTotals->numShifts / samples;
    averageResults->matchCount = infoTotals->matchCount / samples;
    for (int i=0;i < MAX_ALGO_VALUES; i++) {
        averageResults->algoValues[i] = infoTotals->algoValues[i] / samples;
    }
}


/*
 * Adds the values in one stat struct to another.
 */
void addSearchInfoTo(struct searchInfo *toAdd, struct searchInfo *addTo) {
    addTo->hasStats = 1;
    addTo->textLength += toAdd->textLength;
    addTo->searchIndexBytes += toAdd->searchIndexBytes;
    addTo->searchIndexEntries += toAdd->searchIndexEntries;
    addTo->searchIndex2Entries += toAdd->searchIndex2Entries;
    addTo->mainLoopCount += toAdd->mainLoopCount;
    addTo->textBytesRead += toAdd->textBytesRead;
    addTo->indexLookupCount += toAdd->indexLookupCount;
    addTo->fastPathCount += toAdd->fastPathCount;
    addTo->fastPathShifts += toAdd->fastPathShifts;
    addTo->slowPathCount += toAdd->slowPathCount;
    addTo->slowPathShifts += toAdd->slowPathShifts;
    addTo->validationCount += toAdd->validationCount;
    addTo->validationBytesRead += toAdd->validationBytesRead;
    addTo->validationShifts += toAdd->validationShifts;
    addTo->numShifts += toAdd->numShifts;
    addTo->matchCount += toAdd->matchCount;
    for (int i = 0; i < MAX_ALGO_VALUES; i++) {
        addTo->algoValues[i] += toAdd->algoValues[i];
    }
}


/*
 * Prints the algo value names to the console.
 */
void printAlgoValueNames(const char *format, const struct algoValueNames *valueNames) {
    for (int i = 0; i < MAX_ALGO_VALUES; i++) {
        const char * valueName = valueNames->shortName[i];
        if (valueName[0] != '\0') {
            printf(format, valueName);
        }
    }
}


/*
 * Prints all search info to the console.
 */
void printSearchInfo(const char *format, const struct searchInfo *searchInfo, const struct algoValueNames *valueNames) {
    printf(format, "Text length", searchInfo->textLength);
    printf(format, "Index bytes", searchInfo->searchIndexBytes);
    printf(format, "Index entries", searchInfo->searchIndexEntries);
    printf(format, "Index2 entries", searchInfo->searchIndex2Entries);
    printf(format, "Main loop count", searchInfo->mainLoopCount);
    printf(format, "Text bytes read", searchInfo->textBytesRead);
    printf(format, "Index lookup count", searchInfo->indexLookupCount);
    printf(format, "Fast path count", searchInfo->fastPathCount);
    printf(format, "Fast path shifts", searchInfo->fastPathShifts);
    printf(format, "Slow path count", searchInfo->slowPathCount);
    printf(format, "Slow path shifts", searchInfo->slowPathShifts);
    printf(format, "Validation count", searchInfo->validationCount);
    printf(format, "Validation bytes read", searchInfo->validationBytesRead);
    printf(format, "Validation shifts", searchInfo->validationShifts);
    printf(format, "Num shifts", searchInfo->numShifts);
    printf(format, "Match count", searchInfo->matchCount);
    for (int i = 0; i < MAX_ALGO_VALUES; i++) {
        const char * shortName = valueNames->shortName[i];
        if (shortName[0] != '\0') {
            const char * longName = valueNames->longName[i];
            const char * nameToUse = longName[0] != '\0'? longName : shortName;
            printf(format, nameToUse, searchInfo->algoValues[i]);
        }
    }
}

#endif //SMART_SEARCHINFO_H
