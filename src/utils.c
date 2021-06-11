/* utils.c

Copyright 2021      Christopher Nelson <christopher.nelson@languidnights.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

#ifndef SCROT_HAVE_STRDUP
extern char * _strdup(const char *input)
{
  if (! input) return NULL;

  size_t length = strlen(input) + 1;
  if (length == 0) return NULL;

  char *output = (char *) malloc(length);

  if (output == NULL) {
    fprintf(stderr,"Copy of %s failed on allocate", input);
    exit(EXIT_FAILURE);
  }

  strcpy(output, input);
  output[length -1] = '\0';

  return output;
}
#endif
