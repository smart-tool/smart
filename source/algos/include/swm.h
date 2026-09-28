/*
 * Simplified Wu-Manber multi-pattern API.
 *
 * The matcher groups patterns so no group contains two equal two-byte
 * suffixes.  Each callback receives the original pattern index and byte
 * position of a verified match.  One-byte patterns are matched directly.
 */
#ifndef SMART_SWM_H
#define SMART_SWM_H

typedef void (*swm_match_callback)(int pattern_index, int position,
                                   void *context);

int swm_search_set(const unsigned char *const *patterns, const int *lengths,
                   int pattern_count, const unsigned char *text, int n,
                   swm_match_callback callback, void *context);

#endif
