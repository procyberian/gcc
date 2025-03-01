/* do not edit automatically generated by mc from StdIO.  */
/* StdIO.def provides general Read and Write procedures.

Copyright (C) 2001-2025 Free Software Foundation, Inc.
Contributed by Gaius Mulley <gaius.mulley@southwales.ac.uk>.

This file is part of GNU Modula-2.

GNU Modula-2 is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GNU Modula-2 is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */


#if !defined (_StdIO_H)
#   define _StdIO_H

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


#   if defined (_StdIO_C)
#      define EXTERN
#   else
#      define EXTERN extern
#   endif

typedef struct StdIO_ProcWrite_p StdIO_ProcWrite;

typedef struct StdIO_ProcRead_p StdIO_ProcRead;

typedef void (*StdIO_ProcWrite_t) (char);
struct StdIO_ProcWrite_p { StdIO_ProcWrite_t proc; };

typedef void (*StdIO_ProcRead_t) (char *);
struct StdIO_ProcRead_p { StdIO_ProcRead_t proc; };


/*
   Read - is the generic procedure that all higher application layers
          should use to receive a character.
*/

EXTERN void StdIO_Read (char *ch);

/*
   Write - is the generic procedure that all higher application layers
           should use to emit a character.
*/

EXTERN void StdIO_Write (char ch);

/*
   PushOutput - pushes the current Write procedure onto a stack,
                any future references to Write will actually invoke
                procedure, p.
*/

EXTERN void StdIO_PushOutput (StdIO_ProcWrite p);

/*
   PopOutput - restores Write to use the previous output procedure.
*/

EXTERN void StdIO_PopOutput (void);

/*
   GetCurrentOutput - returns the current output procedure.
*/

EXTERN StdIO_ProcWrite StdIO_GetCurrentOutput (void);

/*
   PushInput - pushes the current Read procedure onto a stack,
               any future references to Read will actually invoke
               procedure, p.
*/

EXTERN void StdIO_PushInput (StdIO_ProcRead p);

/*
   PopInput - restores Write to use the previous output procedure.
*/

EXTERN void StdIO_PopInput (void);

/*
   GetCurrentInput - returns the current input procedure.
*/

EXTERN StdIO_ProcRead StdIO_GetCurrentInput (void);
#   ifdef __cplusplus
}
#   endif

#   undef EXTERN
#endif
