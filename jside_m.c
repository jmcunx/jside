/*
 * Copyright (c) 2019 2020 ... 2023 2024
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_JLIB
#include <j_lib2.h>
#include <j_lib2m.h>
#endif

#include "jside.h"

#define REC_SIZE_INIT 400

/*
 * raw_write_side() -- write data sideways
 */
void raw_write_side(FILE *fp, struct s_raw *raw, char *delm, int verbose)
{
  struct s_raw *now = raw;
  int i;

  if (verbose > 0)
    fprintf(fp, "%s: %s\n", LIT_FILE, raw->fname);

  for (i = 0; i < raw->real_cols; i++)
    {
      now = raw;
      while (now != NULL_S_RAW)
	{
	  if (now->field_col[i] != (char *) NULL)
	    {
	      fprintf(fp, "%s%s", now->field_col[i], delm);
	    }
	  now = now->next;
	}
      fprintf(fp, "\n");
    }

} /* raw_write_side() */

/*
 * raw_init() -- Initialize rec info
 */
void raw_init(struct s_raw *r, int max_cols)
{

  r->rec_num      = 0L;
  r->real_cols    = 0;
  r->rec          = (char *) NULL;
  r->fname        = (char *) NULL;
  r->next         = NULL_S_RAW;
  r->field_col    = calloc((max_cols + 1), sizeof (char **));

  if (r->field_col == (char **) NULL)
    {
      fprintf(stderr, MSG_ERR_E080, "field columns");
      exit(EXIT_FAILURE);
    }

} /* raw_init() */

/*
 * raw_free_col() -- free column memory
 */
void raw_free_col(struct s_raw *r, int max_cols)
{
  int i;

  if (r->field_col == (char **) NULL)
    return;

  for (i = 0; i < max_cols; i++)
    {
      if (r->field_col[i] == (char *) NULL)
	break;
      free(r->field_col[i]);
      r->field_col[i] = (char *) NULL;
    }

  free(r->field_col);
  r->field_col = (char **) NULL;

} /* raw_free_col() */

/*
 * raw_free() -- free memory
 *               this logic avoids a recursion stack overflow
 *               Freeing some large lists caused a stack overflow
 */
void raw_free(struct s_raw **raw, int max_cols)
{
  struct s_raw *now   = (*raw);
  struct s_raw *prior = (*raw);

  if ((*raw)->fname != (char *) NULL)
    {
      free((*raw)->fname);
      (*raw)->fname = (char *) NULL;
    }

  while (now != NULL_S_RAW)
    {
      prior = now;
      now = now->next;
      if (prior->rec != (char *) NULL)
	free(prior->rec);
      raw_free_col(prior, max_cols);
      free(prior);
    }

  (*raw) = NULL_S_RAW;

} /* raw_free() */

/*
 * raw_save() -- get memory and save data
 */
struct s_raw *raw_save(char *buf, long int rcount, int max_cols)
{
  struct s_raw *raw = NULL_S_RAW;

  raw = calloc(1, sizeof (struct s_raw));
  raw_init(raw, max_cols);
  
  raw->rec_num  = rcount;
  raw->rec      = strdup(buf);

  return(raw);

} /* raw_save() */

/*
 * raw_split() -- split raw record into columns
 */
int raw_split(struct s_raw *raw, char *delim, int max_cols)
{
  char *r = (char *) NULL;
  char *f = (char *) NULL;
  int i;
  int cols = 0;

  if (raw->rec == (char *) NULL)
    return(0);
  if (raw->field_col == (char **) NULL)
    return(0);

  j2_fix_delm(delim[0], &r, raw->rec);

  f = strtok(r, delim);

  if (f != (char *) NULL)
    {
      raw->field_col[0] = strdup(f);
      j2_rtw(raw->field_col[0]);
      j2_justleft(raw->field_col[0]);
      for (i = 1; i < max_cols; i++)
	{
	  f = strtok((char *) NULL, delim);
	  if (f == (char *) NULL)
	    break;
	  raw->field_col[i] = strdup(f);	  
	  j2_rtw(raw->field_col[i]);
	  j2_justleft(raw->field_col[i]);
	  cols++;
	}      
    }

  if (r != (char *) NULL)
    free(r);

  return(cols + 1);

} /* raw_split() */

/*
 * raw_load() -- load text data into a linked list
 */
struct s_raw *raw_load(FILE *fperr, char *fname, char *delim, 
                       int max_cols, int verbose)
{
  size_t bsize = (size_t) 0;
  size_t rcount = (size_t) 0;
  ssize_t creads = (size_t) 0;
  char *buf = (char *) NULL;
  int is_stdin = (int) FALSE;
  int errsave, cols;
  FILE *fp;
  struct s_raw *raw, *now, *new;

  raw = now = new = NULL_S_RAW;

  if (fname == (char *) NULL)
    {
      fp = stdin;
      is_stdin = (int) TRUE;
    }
  else
    {
      if (strcmp(fname, FILE_NAME_STDIN) == 0)
	{
	  fp = stdin;
	  is_stdin = (int) TRUE;
	}
      else
	{
	  fp = fopen(fname, "r");
	  errsave = errno;
	  if (fp == (FILE *) NULL)
	    {
	      fprintf(stderr, "ERROR E01: Open in: %s\n", fname);
	      fprintf(stderr, "\t%s\n", strerror(errsave));
	      exit(EXIT_FAILURE);
	    }
	}
    }

  /* allocate initial memory (optional) */
  bsize = REC_SIZE_INIT;
  buf = (char *) calloc(REC_SIZE_INIT, sizeof(char));
  errsave = errno;
  if (buf == (char *) NULL)
    {
      fprintf(stderr, "ERROR E02: cannot allocate initial memory\n");
      fprintf(stderr, "\t%s\n", strerror(errsave));
      exit(EXIT_FAILURE);
    }

  while ((creads = j2_getline(&buf, &bsize, fp)) >= 0)
    {
      rcount++;
      j2_rtw(buf);
      new = raw_save(buf, rcount, max_cols);
      if (raw == NULL_S_RAW)
	raw = new;
      else
	now->next = new;
      cols = raw_split(new, delim, max_cols);
      now = new;
      memset(buf, '\0', bsize);
      if (cols > raw->real_cols)
	raw->real_cols = cols;
    }

#ifdef OpenBSD
  freezero(buf, bsize);
#else
  if (buf != (char *) NULL)
    free(buf);
#endif

  if (verbose > 0)
    {
      fprintf(fperr, MSG_INFO_I043, fname);
      fprintf(fperr, MSG_INFO_I045, (long int) rcount);
      fflush(fperr);
    }
  
  if (is_stdin == TRUE)
    raw->fname = strdup(LIT_STDIN);
  else
    {
      raw->fname = strdup(fname);
      fclose(fp);
    }
  return(raw);

} /* raw_load() */
