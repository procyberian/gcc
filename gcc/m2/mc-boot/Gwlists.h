/* do not edit automatically generated by mc from wlists.  */
/* wlists.def word lists module.

Copyright (C) 2015-2025 Free Software Foundation, Inc.
Contributed by Gaius Mulley <gaius@glam.ac.uk>.

This file is part of GNU Modula-2.

GNU Modula-2 is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GNU Modula-2 is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Modula-2; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */


#if !defined (_wlists_H)
#   define _wlists_H

#define INCLUDE_MEMORY
#include "config.h"
#include "system.h"
#   ifdef __cplusplus
extern "C" {
#   endif
#include <stdbool.h>
#   if !defined (PROC_D)
#      define PROC_D
       typedef void (*PROC_t) (void);
       typedef struct { PROC_t proc; } PROC;
#   endif

#   include "GSYSTEM.h"

#   if defined (_wlists_C)
#      define EXTERN
#   else
#      define EXTERN extern
#   endif

#if !defined (wlists_wlist_D)
#  define wlists_wlist_D
   typedef void *wlists_wlist;
#endif

typedef struct wlists_performOperation_p wlists_performOperation;

typedef void (*wlists_performOperation_t) (unsigned int);
struct wlists_performOperation_p { wlists_performOperation_t proc; };


/*
   initList - creates a new wlist, l.
*/

EXTERN wlists_wlist wlists_initList (void);

/*
   killList - deletes the complete wlist, l.
*/

EXTERN void wlists_killList (wlists_wlist *l);

/*
   putItemIntoList - places an WORD, c, into wlist, l.
*/

EXTERN void wlists_putItemIntoList (wlists_wlist l, unsigned int c);

/*
   getItemFromList - retrieves the nth WORD from wlist, l.
*/

EXTERN unsigned int wlists_getItemFromList (wlists_wlist l, unsigned int n);

/*
   getIndexOfList - returns the index for WORD, c, in wlist, l.
                    If more than one CARDINAL, c, exists the index
                    for the first is returned.
*/

EXTERN unsigned int wlists_getIndexOfList (wlists_wlist l, unsigned int c);

/*
   noOfItemsInList - returns the number of items in wlist, l.
*/

EXTERN unsigned int wlists_noOfItemsInList (wlists_wlist l);

/*
   includeItemIntoList - adds an WORD, c, into a wlist providing
                         the value does not already exist.
*/

EXTERN void wlists_includeItemIntoList (wlists_wlist l, unsigned int c);

/*
   removeItemFromList - removes an WORD, c, from a wlist.
                        It assumes that this value only appears once.
*/

EXTERN void wlists_removeItemFromList (wlists_wlist l, unsigned int c);

/*
   replaceItemInList - replace the nth WORD in wlist, l.
                       The first item in a wlists is at index, 1.
                       If the index, n, is out of range nothing is changed.
*/

EXTERN void wlists_replaceItemInList (wlists_wlist l, unsigned int n, unsigned int w);

/*
   isItemInList - returns true if a WORD, c, was found in wlist, l.
*/

EXTERN bool wlists_isItemInList (wlists_wlist l, unsigned int c);

/*
   foreachItemInListDo - calls procedure, P, foreach item in wlist, l.
*/

EXTERN void wlists_foreachItemInListDo (wlists_wlist l, wlists_performOperation p);

/*
   duplicateList - returns a duplicate wlist derived from, l.
*/

EXTERN wlists_wlist wlists_duplicateList (wlists_wlist l);
#   ifdef __cplusplus
}
#   endif

#   undef EXTERN
#endif
