/*
 * Copyright (c) 2019 2020 2021 2022
 *     John McCue <jmccue@jmcunx.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _MSDOS
#include <sys/param.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#ifdef OpenBSD
#include <err.h>
#endif
#ifdef __FreeBSD_version
#include <err.h>
#endif
#ifdef __NetBSD_Version__
#include <err.h>
#endif

#include <j_lib2.h>
#include <j_lib2m.h>

#include "jside.h"

/*
 * main()
 */
int main(int argc, char **argv)

{
  struct s_work w;
  struct s_raw *raw1 = NULL_S_RAW;
  int i = 0;

#ifdef OpenBSD
  if(pledge("stdio rpath wpath cpath",NULL) == -1)
    err(1,"pledge\n");
#endif

  init(argc, argv, &w);

  for (i = optind; i < argc; i++)
    {
      raw1 = raw_load(w.err.fp, argv[i], w.delim_in, w.max_cols, w.verbose);
      raw_write_side(w.out.fp, raw1, w.delim_out, w.verbose);
      raw_free(&raw1, w.max_cols);
      raw1 = NULL_S_RAW;
    }

  if (i == optind)
    {
      raw1 = raw_load(w.err.fp, FILE_NAME_STDIN, w.delim_in, w.max_cols, w.verbose);
      raw_write_side(w.out.fp, raw1, w.delim_out, w.verbose);
      raw_free(&raw1, w.max_cols);
      raw1 = NULL_S_RAW;
    }
  
  close_out(&(w.out));
  close_out(&(w.err));

  if (w.prog_name != (char *) NULL)
    free(w.prog_name);
  exit(EXIT_SUCCESS);

}  /* main() */
