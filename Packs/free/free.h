/*
 * free.h
 * Memory usage utility for Darwin & MacOS X (similar to 'free' under Linux)
 *
 * by:  David Cantrell <david.l.cantrell@gmail.com>
 * Copyright (C) 2008, David Cantrell, Honolulu, HI, USA.
 *
 * Licensed under the GNU Lesser General Public License version 2.1 or later.
 * See COPYING.LIB for licensing details.
 */

#define FREE_USAGE "Usage: %s [-b|-k|-m] [-s delay] [-V] [-h|-?]\n"
#define COMBINED_UNIT_OPTIONS "You may only use one of the unit options:  -b, -k, or -m\n"

enum { BYTES, KILOBYTES, MEGABYTES };

typedef struct mem {
    uint64_t total;
    uint64_t used;
    uint64_t free;
    uint64_t active;
    uint64_t inactive;
    uint64_t wired;
} mem_t;
