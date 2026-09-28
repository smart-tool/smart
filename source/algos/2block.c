/*
 * SMART: string matching algorithms research tool.
 * Copyright (C) 2012 Simone Faro and Thierry Lecroq
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * 2BLOCK from M. A. Sustik and J. S. Moore, "String Searching over Small
 * Alphabets", TR-07-62, University of Texas at Austin, 2007.
 * https://www.cs.utexas.edu/~moore/publications/sustik-moore.pdf
 */

#include "include/define.h"
#include "include/main.h"

#include <stdint.h>

#define MAX_M 128

typedef struct {
  uint16_t left, first, last;
  uint8_t smart;
} two_block_state;

typedef struct {
  uint16_t next;
  uint8_t shift, ready;
} two_block_transition;

typedef struct {
  unsigned char *pattern;
  int m, states, capacity, smart;
  two_block_state *state;
  two_block_transition *transition;
} two_block_machine;

static int two_block_read_pos(const two_block_machine *machine, int state) {
  const two_block_state *blocks = &machine->state[state];

  if (blocks->first == machine->m)
    return machine->m - 1;
  if (blocks->last < machine->m - 1)
    return blocks->last + 1;
  return blocks->first - 1;
}

static two_block_state two_block_normalize(two_block_state blocks, int m) {
  if (blocks.first >= m || blocks.first > blocks.last) {
    blocks.first = blocks.last = m;
    return blocks;
  }
  if (blocks.first < blocks.left)
    blocks.first = blocks.left;
  if (blocks.first == blocks.left) {
    blocks.left = blocks.last + 1;
    blocks.first = blocks.last = m;
  }
  return blocks;
}

static int two_block_state_id(two_block_machine *machine,
                              two_block_state blocks) {
  int i;

  blocks = two_block_normalize(blocks, machine->m);
  if (machine->smart && blocks.first != machine->m &&
      blocks.last < machine->m / 2)
    return 0;

  for (i = 0; i < machine->states; ++i) {
    if (machine->state[i].left == blocks.left &&
        machine->state[i].first == blocks.first &&
        machine->state[i].last == blocks.last &&
        machine->state[i].smart == blocks.smart)
      return i;
  }
  if (machine->states == machine->capacity)
    return 0;
  machine->state[machine->states] = blocks;
  return machine->states++;
}

static int two_block_shift_valid(const two_block_machine *machine,
                                 const two_block_state *blocks, int read_pos,
                                 unsigned char character, int shift) {
  int i;

  if (read_pos >= shift && machine->pattern[read_pos - shift] != character)
    return 0;
  for (i = shift; i < blocks->left; ++i) {
    if (machine->pattern[i] != machine->pattern[i - shift])
      return 0;
  }
  if (blocks->first != machine->m) {
    for (i = MAX(blocks->first, shift); i <= blocks->last; ++i) {
      if (machine->pattern[i] != machine->pattern[i - shift])
        return 0;
    }
  }
  return 1;
}

static two_block_transition two_block_mismatch(two_block_machine *machine,
                                               int state,
                                               unsigned char character) {
  const two_block_state *old = &machine->state[state];
  two_block_state blocks;
  two_block_transition result;
  int read_pos = two_block_read_pos(machine, state);
  int shift;
  for (shift = 1; shift <= machine->m; ++shift) {
    if (two_block_shift_valid(machine, old, read_pos, character, shift))
      break;
  }
  blocks.left = old->left > shift ? old->left - shift : 0;
  if (old->first == machine->m) {
    blocks.first = blocks.last = machine->m;
  } else {
    blocks.first = old->first > shift ? old->first - shift : machine->m;
    blocks.last = old->last >= shift ? old->last - shift : machine->m;
  }
  blocks.smart = machine->smart && shift >= machine->m / 2;
  if (read_pos >= shift) {
    int observed = read_pos - shift;

    if (observed == blocks.left) {
      ++blocks.left;
    } else if (blocks.first == machine->m) {
      blocks.first = blocks.last = observed;
    } else if (observed == blocks.first - 1) {
      --blocks.first;
    } else if (observed == blocks.last + 1) {
      ++blocks.last;
    }
  }
  blocks = two_block_normalize(blocks, machine->m);
  result.next = two_block_state_id(machine, blocks);
  result.shift = shift;
  result.ready = 1;
  return result;
}

static int two_block_fallback(unsigned char *x, int m, unsigned char *y,
                              int n) {
  int count = 0, i, j, shift[SIGMA];

  BEGIN_PREPROCESSING
  for (i = 0; i < SIGMA; ++i)
    shift[i] = m;
  for (i = 0; i < m - 1; ++i)
    shift[x[i]] = m - 1 - i;
  END_PREPROCESSING

  BEGIN_SEARCHING
  for (j = 0; j <= n - m;) {
    for (i = m - 1; i >= 0 && x[i] == y[j + i]; --i)
      ;
    if (i < 0) {
      OUTPUT(j);
      ++j;
    } else {
      int amount = shift[y[j + m - 1]];

      j += amount > 0 ? amount : 1;
    }
  }
  END_SEARCHING
  return count;
}

int search(unsigned char *x, int m, unsigned char *y, int n) {
  two_block_machine machine;
  int count = 0, state = 0, align = 0;

  if (m <= 0 || n < m)
    return 0;
  if (m > MAX_M)
    return two_block_fallback(x, m, y, n);
  BEGIN_PREPROCESSING
  machine.pattern = x;
  machine.m = m;
  machine.smart = 0;
  machine.states = 1;
  machine.capacity = 2 * m * m;
  machine.state = calloc(machine.capacity, sizeof(*machine.state));
  machine.transition =
      calloc((size_t)machine.capacity * SIGMA, sizeof(*machine.transition));
  if (!machine.state || !machine.transition) {
    free(machine.state);
    free(machine.transition);
    return two_block_fallback(x, m, y, n);
  }
  machine.state[0].first = machine.state[0].last = m;
  END_PREPROCESSING

  BEGIN_SEARCHING
  while (align <= n - m) {
    const int read_pos = two_block_read_pos(&machine, state);
    const unsigned char character = y[align + read_pos];
    two_block_state blocks;

    if (character == x[read_pos]) {
      blocks = machine.state[state];
      if (blocks.smart) {
        blocks.smart = 0;
        if (memcmp(x, y + align, m) == 0) {
          OUTPUT(align);
          ++align;
          state = 0;
          continue;
        }
      }
      if (blocks.first == m) {
        blocks.first = blocks.last = read_pos;
      } else if (read_pos == blocks.last + 1) {
        ++blocks.last;
      } else {
        --blocks.first;
      }
      blocks = two_block_normalize(blocks, m);
      if (blocks.left == m) {
        OUTPUT(align);
        ++align;
        state = 0;
        continue;
      }
      state = two_block_state_id(&machine, blocks);
      continue;
    }

    two_block_transition *transition =
        &machine.transition[state * SIGMA + character];
    if (!transition->ready)
      *transition = two_block_mismatch(&machine, state, character);
    align += transition->shift;
    state = transition->next;
  }
  END_SEARCHING
  free(machine.transition);
  free(machine.state);
  return count;
}
