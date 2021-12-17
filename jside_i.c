/*
 * Copyright (c) 2019 2020 2021
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

#include <sys/param.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>  /*** for access() ***/
#include <j_lib2.h>
#include <j_lib2m.h>

#include "jside.h"

#define SCKARG 80

char *jside_i_c="$Id: jside_i.c,v 1.4 2021/02/21 20:48:22 jmccue Exp $";

/*
 * get_delm() -- translate a number or string into a delimiter
 */
char get_delm(FILE *fp, char *s, char *pname)
{
  int d;
  int c = JLIB2_CHAR_NULL;

  if (strlen(s) == 1)
    {
      if ( ((*s) > 47)  && ((*s) < 58) ) /* 0 -- 9 */
	c = (*s) - 48;
      else
	c = (*s);
    }
  else
    {
      if (j2_is_numr(s) == (int) TRUE)
	{
	  d = atoi(s);
	  if ((d < 256) && (d > 0))
	    c = (char) d;
	  else
	    {
	      fprintf(fp, MSG_ERR_E006, s, SWITCH_CHAR, ARG_DELM);
	      fprintf(fp, MSG_ERR_E000, pname, SWITCH_CHAR, ARG_HELP);
	      exit(EXIT_FAILURE);
	    }
	}
      else
	{
	  fprintf(fp, MSG_ERR_E006, s, SWITCH_CHAR, ARG_DELM);
	  fprintf(fp, MSG_ERR_E000, pname, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	}
    }

  return(c);

} /* get_delm() */

/*
 * save_fname() -- Check and Save File Name
 */
void save_fname(struct s_file_info *f, char *afname, 
                char *pname, char arg_val)
{
  if (f->fname == (char *) NULL)
    f->fname = strdup(afname);
  else
    {
      fprintf(stderr, MSG_ERR_E074, SWITCH_CHAR, arg_val);
      fprintf(stderr, MSG_ERR_E000, pname, SWITCH_CHAR, ARG_HELP);
      exit(EXIT_FAILURE);
    }

} /* save_fname() */

/*
 * init_w() -- initialize work area
 */
void init_w(struct s_work *w, char *a)

{
  init_finfo(&(w->out));
  init_finfo(&(w->err));
  w->err.fp    = stderr;
  w->out.fp    = stdout;

  w->prog_name        = j2_get_prgname(a, PROG_NAME);
  w->num_files        = 0;
  w->verbose          = 0;
  w->force            = (int) FALSE;
  w->delim_in[0]      = DELIM_DEFAULT;
  w->delim_in[1]      = JLIB2_CHAR_NULL;
  w->delim_out[0]     = DELIM_DEFAULT;
  w->delim_out[1]     = JLIB2_CHAR_NULL;
  w->ifname           = (char *) NULL;
  w->max_cols         = DEFAULT_MAX_COL;

}  /* init_w() */

/*
 * process_arg() -- process arguments
 */
void process_arg(int argc, char **argv, struct s_work *w)

{
  char ckarg[SCKARG];
  int opt;
  int i;

  snprintf(ckarg, SCKARG, "%c%c%c%c%c:%c:%c:%c:%c:", 
	  ARG_FORCE, ARG_HELP, ARG_VERBOSE, ARG_VERSION,
	  ARG_MAX_COL, ARG_DELM, ARG_DELM_OUT, ARG_ERR, ARG_OUT);

  while ((opt = getopt(argc, argv, ckarg)) != -1)
    {
      switch (opt)
	{
	case ARG_MAX_COL:
	  if (j2_is_numr(optarg) == FALSE)
	    {
	      fprintf(w->err.fp, MSG_ERR_E008, optarg, SWITCH_CHAR, ARG_MAX_COL);
	      fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
	      exit(EXIT_FAILURE);
	    }
	  w->max_cols = (size_t) atoi(optarg);
	  break;
	case ARG_DELM:
	  w->delim_in[1] = JLIB2_CHAR_NULL;
	  w->delim_in[0] = get_delm(stderr, optarg, w->prog_name);
	  break;
	case ARG_DELM_OUT:
	  w->delim_out[1] = JLIB2_CHAR_NULL;
	  w->delim_out[0] = get_delm(stderr, optarg, w->prog_name);
	  break;
	case ARG_FORCE:
	  w->force = (int) TRUE;
	  break;
	case ARG_HELP:
	  show_brief_help(stderr, w->prog_name);
	  break;
	case ARG_VERBOSE:
	  w->verbose++;
	  break;
	case ARG_VERSION:
	  show_rev(stderr, w->prog_name);
	  break;
	case ARG_ERR:
	  save_fname(&(w->err), optarg, w->prog_name, ARG_ERR);
	  break;
	case ARG_OUT:
	  save_fname(&(w->out), optarg, w->prog_name, ARG_ERR);
	  break;
	default:
	  fprintf(w->err.fp, MSG_ERR_E000, w->prog_name, SWITCH_CHAR, ARG_HELP);
	  exit(EXIT_FAILURE);
	  break;
	}
    }

  /*** open 'out' files ***/
  if ( ! open_out(stderr, &(w->err), w->prog_name, w->force))
    w->err.fp = stderr;
  if ( ! open_out(w->err.fp, &(w->out), w->prog_name, w->force) )
    w->out.fp = stdout;

  /*** Count number of files to process */
  for (i = optind; i < argc; i++)
    {
      if (w->ifname == (char *) NULL)
	w->ifname = argv[i];
      (w->num_files)++;
    }
  if (w->num_files == 0)
    (w->num_files)++;  /* stdin when no files */

} /* process_arg() */

/*
 * init() -- initialize
 */
void init(int argc, char **argv, struct s_work *w)

{

  init_w(w, argv[0]);

  process_arg(argc, argv, w);

}  /* init() */

/* END: jside_i.c */
