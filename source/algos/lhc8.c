/*
 * Copyright 2022 Matt Palmer.  All rights reserved.
 *
 * This is an implementation of the LinearHashChain algorithm, (currently unpublished) by Matt Palmer.
 * It is a factor search similar to WFR or the QF family of algorithms, but with linear performance in the worst case.
 *
 * It builds a hash table containing entries for chains of hashes.  Hashes are chained together by
 * placing a fingerprint of the *next* hash into the entry for the *current* hash.  This enables
 * a check for the second hash value to be performed without requiring a second lookup in the hash table.
 *
 * It creates Q chains of hashes from the end of the pattern back to the start.  Two techniques are used to ensure
 * linear performance. (1) During the filtering phase scanning back, it will not scan back over bytes it has already
 * matched previously. (2) During the verification phase, a linear matching algorithm (KMP) is used to identify any
 * possible matches, which will not re-verify bytes which have already been matched.
 *
 * Performance is very similar to HashChain on average, but remains linear when given worst-case and low entropy data
 * (for example, a lengthy text and patterns with an alphabet of 1, e.g. the entire text is made up of the same character.)
 *
 * This implementation is written to integrate with the SMART string search benchmarking tool,
 * by Simone Faro, Matt Palmer, Stefano Stefano Scafiti and Thierry Lecroq.
*/

#include "include/define.h"
#include "include/main.h"

/*
 * Alpha - the number of bits in the hash table.
 */
#define ALPHA 12

/*
 * Number of bytes in a q-gram.
 * Chain hash functions defined below must be written to process this number of bytes.
 */
#define	Q     8

/*
 * Functions and calculated parameters.
 * Hash functions must be written to use the number of bytes defined in Q. They scan backwards from the initial position.
 */
#define S                 ((ALPHA) / (Q))                          // Bit shift for each of the chain hash byte components.
#define HASH(x, p, s)     ((((((((((((((x[p] << (s)) + x[p - 1]) << (s)) + x[p - 2]) << (s)) + x[p - 3]) << (s)) + x[p - 4]) << (s)) + x[p - 5]) << (s)) + x[p - 6]) << (s)) + x[p - 7])
#define CHAIN_HASH(x, p)  HASH((x), (p), (S))                      // Hash function for chain hashes, using the S3 bitshift.
#define LINK_HASH(H)      (1U << ((H) & 0x1F))                     // Hash fingerprint, taking low 5 bits of the hash to set one of 32 bits.
#define ASIZE             (1 << (ALPHA))                           // Hash table size.
#define TABLE_MASK        ((ASIZE) - 1)                            // Mask for table is one less than the power of two size.
#define Q2                (Q + Q)                                  // 2 Qs.
#define END_FIRST_QGRAM   (Q - 1)                                  // Position of the end of the first q-gram.
#define END_SECOND_QGRAM  (Q2 - 1)                                 // Position of the end of the second q-gram.

/*
 * Builds the KMP failure table, given a pattern x of length m and a list of integers with m + 1 elements.
 * It adds a failure function to the very end, at position m, to be able to continue searching.
 */
void pre_kmp(unsigned char *x, int m, int KMP[])
{
    int j = 0;
    int t = -1;
    KMP[0] = -1;
    while (j < m) {
        while (t > -1 && x[j] != x[t]) {
            t = KMP[t];
        }
        j++; t++;
        if (j < m && x[j] == x[t]) {
            KMP[j] = KMP[t];
        }
        else {
            KMP[j] = t;
        }
    }
}

/*
 * Builds the hash table B of size ASIZE for a string x of length m.
 * Returns the 32-bit hash value of matching the entire pattern.
 */
unsigned int preprocessing(const unsigned char *x, int m, unsigned int *B) {

    // 0. Zero out the hash table.
    for (int i = 0; i < ASIZE; i++) B[i] = 0;

    // 1. Calculate all the chain hashes, ending with processing the entire pattern so H has the cumulative value.
    unsigned int H = 0;
    int start = m < Q2 ? m - END_FIRST_QGRAM : Q;
    for (int chain_no = start; chain_no >= 1; chain_no--)
    {
        H = CHAIN_HASH(x, m - chain_no);
        for (int chain_pos = m - chain_no - Q; chain_pos >= END_FIRST_QGRAM; chain_pos -=Q)
        {
            unsigned int H_last = H;
            H = CHAIN_HASH(x, chain_pos);
            B[H_last & TABLE_MASK] |= LINK_HASH(H);
        }
    }

    // 2. Add in hashes for the first qgrams that have no preceding value.  Only set a value if there is nothing there already.
    unsigned int F;
    int stop = MIN(m, END_SECOND_QGRAM);
    for (int chain_pos = END_FIRST_QGRAM; chain_pos < stop; chain_pos++)
    {
        F = CHAIN_HASH(x, chain_pos);
        if (!B[F & TABLE_MASK]) B[F & TABLE_MASK] = LINK_HASH(~F);
    }

    return H; // Return 32-bit hash value for processing the entire pattern.
}

/*
 * Searches for a pattern x of length m in a text y of length n and reports the number of occurrences found.
 */
int search(unsigned char *x, int m, unsigned char *y, int n) {
    if (m < Q) return -1;  // have to be at least Q in length to search.
    unsigned int H, V, B[ASIZE];
    int KMP[m + 1];

    /* Preprocess;ing */
    BEGIN_PREPROCESSING
    const int MQ1 = m - Q + 1;
    preprocessing(x, m, B);
    pre_kmp(x, m, KMP);
    END_PREPROCESSING

    /* Searching */
    BEGIN_SEARCHING
    int count = 0;
    int pos = m - 1;
    int rightmost_match_pos = 0;
    int next_verify_pos = 0;
    int pattern_pos = 0;
    // While within the search text:
    while (pos < n) {

        // If there is a bit set for the hash:
        H = CHAIN_HASH(y, pos);
        V = B[H & TABLE_MASK];
        if (V) {
            // Calculate how far back to scan and update the right most match pos.
            const int end_first_qgram_pos = pos - m + Q;
            const int scan_back_pos = MAX(end_first_qgram_pos, rightmost_match_pos) + Q;
            rightmost_match_pos = pos;

            // Look at the chain of q-grams that precede it:
            while (pos >= scan_back_pos)
            {
                pos -= Q;
                H = CHAIN_HASH(y, pos);
                // If we have no match for this chain q-gram, break out and go around the main loop again:
                if (!(V & LINK_HASH(H))) goto shift;
                V = B[H & TABLE_MASK];
            }

            // Matched the chain all the way back to the start - verify the pattern :
            const int window_start_pos = end_first_qgram_pos - Q + 1;
            // Check if we need to re-start KMP if our window start is after last results.
            if (window_start_pos > next_verify_pos) {
                next_verify_pos = window_start_pos;
                pattern_pos = 0;
            }

            //TODO: what does this condition signify?  It is unclear even if theoretically sound...
            //      can it overflow next_verify_pos beyond n?
            while (pattern_pos >= next_verify_pos - window_start_pos) {

                // Naive string matching - how many characters do we match...
                while (pattern_pos < m && x[pattern_pos] == y[next_verify_pos]) {
                    pattern_pos++;
                    next_verify_pos++;
                }

                // If we matched the whole length of the pattern (and we're still inside the text), increase match count.
                if (pattern_pos == m) count++;

                // Get the next matching pattern position.
                pattern_pos = KMP[pattern_pos];
                if (pattern_pos < 0) {
                    pattern_pos++;
                    next_verify_pos++;
                }
            }

            pos = next_verify_pos + m - 1 - pattern_pos;
            continue;
            //pos = next_verify_pos + Q - pattern_pos; //TODO: this fails tests - shift calculation not correct?
        }

        // Go around the main loop looking for another hash, incrementing the pos by MQ1.
        shift:
        pos += MQ1;
    }
    END_SEARCHING

    return count;
}
