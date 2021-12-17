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

#ifndef JSIDE_H

#define JSIDE_H "INCLUDED"

#define PROG_NAME  "jside"

/*** structures ***/
struct s_raw
{
  long int rec_num;
  int real_cols;
  char *fname;
  char *rec;
  char **field_col;
  struct s_raw *next;
} ;

struct s_file_info
{
  FILE *fp;
  char *fname;
} ;

struct s_work
{
  struct s_file_info out;         /* default stdout        */
  struct s_file_info err;         /* default stderr        */
  char *ifname;
  char delim_in[2];
  char delim_out[2];
  char *prog_name;                /* real program name     */
  int num_files;                  /* # of files to process */
  int verbose;                    /* TRUE or FALSE         */
  int force;                      /* TRUE or FALSE         */
  int max_cols;                   /* Maximum # of Columns  */
} ;

#define NULL_S_RAW ((struct s_raw *) NULL)
#define RT_UNKNOWN 0
#define DELIM_DEFAULT ','
#define DEFAULT_MAX_COL 200

/*** prototypes ***/
void init(int, char **, struct s_work *);
void init_finfo(struct s_file_info *);
void show_brief_help(FILE *, char *);
void show_rev(FILE *, char *);
void process_arg(int, char **, struct s_work *);
int  open_out(FILE *, struct s_file_info *, char *, int);
void close_out(struct s_file_info *);
int  open_in(FILE **, char *, FILE *);
void close_in(FILE **, char *);
struct s_raw *raw_load(FILE *, char *, char *, int, int);
void raw_write_side(FILE *, struct s_raw *, char *, int);
void raw_free(struct s_raw **, int);

#endif /*  JSIDE_H  */
