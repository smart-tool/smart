/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012  Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Simplified Wu-Manber (SWM) uses a direct two-byte SHIFT table and one
 * candidate per terminal block.  SMART supplies one pattern, making its
 * singleton group distinct only by the direct candidate lookup contract.
 *
 * L. Vespa and N. Weng, SWM: Simplified Wu-Manber for GPU-based Deep Packet
 * Inspection, Proceedings of the 2012 International Conference on Security
 * and Management, 2012.
 */

#include "include/define.h"
#include "include/main.h"
#include "include/swm.h"
#include <stdint.h>

#define SWM_TABLE_SIZE (1 << 16)
int swm_search_set(const unsigned char *const *patterns, const int *lengths,
                   int pattern_count, const unsigned char *text, int n,
                   swm_match_callback callback, void *context) {
  int *groups, *shift, *candidate;
  int count = 0;
  int group_count = 0;
  int group, i, j, k, min_length, position;

  if (patterns == NULL || lengths == NULL || text == NULL ||
      pattern_count <= 0 || n <= 0)
    return 0;

  groups = (int *)malloc((size_t)pattern_count * sizeof(*groups));
  shift = (int *)malloc(SWM_TABLE_SIZE * sizeof(*shift));
  candidate = (int *)malloc(SWM_TABLE_SIZE * sizeof(*candidate));
  if (groups == NULL || shift == NULL || candidate == NULL) {
    free(candidate);
    free(shift);
    free(groups);
    return 0;
  }

  for (i = 0; i < pattern_count; i++) {
    if (patterns[i] == NULL || lengths[i] <= 0) {
      groups[i] = UNDEFINED;
      continue;
    }
    if (lengths[i] == 1) {
      groups[i] = UNDEFINED;
      for (position = 0; position < n; position++)
        if (text[position] == patterns[i][0]) {
          if (callback != NULL)
            callback(i, position, context);
          count++;
        }
      continue;
    }

    group = 0;
    for (j = 0; j < i; j++)
      if (groups[j] != UNDEFINED && lengths[j] >= 2 &&
          patterns[j][lengths[j] - 2] == patterns[i][lengths[i] - 2] &&
          patterns[j][lengths[j] - 1] == patterns[i][lengths[i] - 1])
        group++;
    groups[i] = group;
    if (group >= group_count)
      group_count = group + 1;
  }

  for (group = 0; group < group_count; group++) {
    min_length = 0;
    for (i = 0; i < pattern_count; i++)
      if (groups[i] == group && (min_length == 0 || lengths[i] < min_length))
        min_length = lengths[i];

    for (i = 0; i < SWM_TABLE_SIZE; i++) {
      shift[i] = min_length - 1;
      candidate[i] = UNDEFINED;
    }
    for (i = 0; i < pattern_count; i++) {
      if (groups[i] != group)
        continue;
      for (k = 0; k < lengths[i] - 1; k++) {
        const unsigned key =
            ((unsigned)patterns[i][k] << 8) | patterns[i][k + 1];
        const int distance = lengths[i] - 2 - k;

        if (distance < shift[key])
          shift[key] = distance;
      }
      k = lengths[i] - 2;
      candidate[((unsigned)patterns[i][k] << 8) | patterns[i][k + 1]] = i;
    }

    for (position = 0; position <= n - min_length;) {
      const unsigned key = ((unsigned)text[position + min_length - 2] << 8) |
                           text[position + min_length - 1];
      const int amount = shift[key];

      if (amount != 0) {
        position += amount;
        continue;
      }
      i = candidate[key];
      if (i != UNDEFINED && lengths[i] <= position + min_length) {
        const int start = position + min_length - lengths[i];

        if (!memcmp(patterns[i], text + start, (size_t)lengths[i])) {
          if (callback != NULL)
            callback(i, start, context);
          count++;
        }
      }
      position++;
    }
  }

  free(candidate);
  free(shift);
  free(groups);
  return count;
}

static int fallback_search(unsigned char *x, int m, unsigned char *y, int n) {
  int count = 0;
  int i, j;

  BEGIN_PREPROCESSING
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (i = 0; i <= n - m; i++) {
    for (j = 0; j < m && x[j] == y[i + j]; j++)
      ;
    if (j == m)
      OUTPUT(i);
  }
  END_SEARCHING
  return count;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  uint16_t shift[SWM_TABLE_SIZE];
  int count = 0;
  int i, j, amount;

  if (m <= 0 || n < m)
    return 0;
  if (m < 2)
    return fallback_search(x, m, y, n);

  BEGIN_PREPROCESSING
  for (i = 0; i < SWM_TABLE_SIZE; i++)
    shift[i] = (uint16_t)(m - 1);
  for (i = 0; i < m - 1; i++)
    shift[((unsigned)x[i] << 8) | x[i + 1]] = (uint16_t)(m - 2 - i);
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (i = 0; i <= n - m;) {
    const unsigned key = ((unsigned)y[i + m - 2] << 8) | y[i + m - 1];

    amount = shift[key];
    if (amount != 0) {
      i += amount;
      continue;
    }

    for (j = 0; j < m && x[j] == y[i + j]; j++)
      ;
    if (j == m)
      OUTPUT(i);
    i++;
  }
  END_SEARCHING
  return count;
}
