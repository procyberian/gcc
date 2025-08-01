/* Basic error reporting routines.
   Copyright (C) 1999-2025 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* warning, error, and fatal.  These definitions are suitable for use
   in the generator programs; the compiler has a more elaborate suite
   of diagnostic printers, found in diagnostic-global-context.cc using
   the diagnostics/ subdirectory.  */

#ifdef HOST_GENERATOR_FILE
#include "config.h"
#define GENERATOR_FILE 1
#else
#include "bconfig.h"
#endif
#include "system.h"
#include "errors.h"

/* Set this to argv[0] at the beginning of main.  */

const char *progname;

/* Starts out 0, set to 1 if error is called.  */

int have_error = 0;

/* Print a warning message - output produced, but there may be problems.  */

void
warning (const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "%s: warning: ", progname);
  vfprintf (stderr, format, ap);
  va_end (ap);
  fputc ('\n', stderr);
}


/* Print an error message - we keep going but the output is unusable.  */

void
error (const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "%s: ", progname);
  vfprintf (stderr, format, ap);
  va_end (ap);
  fputc ('\n', stderr);

  have_error = 1;
}


/* Fatal error - terminate execution immediately.  Does not return.  */

void
fatal (const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "%s: ", progname);
  vfprintf (stderr, format, ap);
  va_end (ap);
  fputc ('\n', stderr);
  exit (FATAL_EXIT_CODE);
}

/* Similar, but say we got an internal error.  */

void
internal_error (const char *format, ...)
{
  va_list ap;

  va_start (ap, format);
  fprintf (stderr, "%s: Internal error: ", progname);
  vfprintf (stderr, format, ap);
  va_end (ap);
  fputc ('\n', stderr);
  exit (FATAL_EXIT_CODE);
}

/* Given a partial pathname as input, return another pathname that
   shares no directory elements with the pathname of __FILE__.  This
   is used by fancy_abort() to print `Internal compiler error in expr.cc'
   instead of `Internal compiler error in ../../GCC/gcc/expr.cc'.  This
   version is meant to be used for the gen* programs and therefor need not
   handle subdirectories.  */

const char *
trim_filename (const char *name)
{
  static const char this_file[] = __FILE__;
  const char *p = name, *q = this_file;

  /* Skip any parts the two filenames have in common.  */
  while (*p == *q && *p != 0 && *q != 0)
    p++, q++;

  /* Now go backwards until the previous directory separator.  */
  while (p > name && !IS_DIR_SEPARATOR (p[-1]))
    p--;

  return p;
}

/* "Fancy" abort.  Reports where in the compiler someone gave up.
   This file is used only by build programs, so we're not as polite as
   the version in diagnostics/context.cc.  */
void
fancy_abort (const char *file, int line, const char *func)
{
  internal_error ("abort in %s, at %s:%d", func, trim_filename (file), line);
}
