/*
 * Copyright (c) 2019 ... 2023 2024
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
#define RT_UNKNOWN 0
#define DELIM_DEFAULT ','
#define DEFAULT_MAX_COL 200

#ifndef JLIB2_CHAR_NULL
#define NO_JLIB 1
#define JLIB2_INT_NULL   ((int) '\0')
#define JLIB2_CHAR_NULL  ((char) '\0')
#define JLIB2_UCHAR_NULL ((unsigned char) '\0' )
#endif

#ifndef NULL
#define NULL '\0'
#endif
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef SSIZE_T
#define SSIZE_T ssize_t
#endif
#ifndef TAB_CHAR
#define TAB_CHAR 0x09
#endif

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
#define NULL_S_RAW ((struct s_raw *) NULL)

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


#ifdef NO_JLIB
#define ARG_DELM          'd'  /* Field Delimiter                    */
#define ARG_DELM_OUT      'D'  /* Output Field Delimiter             */
#define ARG_ERR           'e'  /* Output Error File                  */
#define ARG_FORCE         'f'  /* force create files                 */
#define ARG_HELP          'h'  /* Show Help                          */
#define ARG_MAX_COL       'm'  /* Maximum number of columns          */
#define ARG_OUT           'o'  /* Output File                        */
#define ARG_VERBOSE       'v'  /* Verbose                            */
#define ARG_VERSION       'V'  /* Show Version Information           */
#define FILE_NAME_STDIN   "-"
#define FILE_NAME_STDOUT  "-"
#define LIT_FILE          "File"
#define LIT_INFO_01       "\tBuild: %s %s\n"
#define LIT_REV           "Revision"
#define LIT_STDIN         "(standard input)"
#define MSG_ERR_E000      "Try '%s %c%c' for more information\n"
#define MSG_ERR_E002      "ERROR E002: Cannot open '%s' for write, processing aborted\n"
#define MSG_ERR_E006      "ERROR E006: '%s' is an invalid value for %c%c\n"
#define MSG_ERR_E008      "ERROR E008: '%s' is an invalid value for %c%c, must be numeric\n"
#define MSG_ERR_E025      "ERROR E025: File %s cannot be created, already exists\n"
#define MSG_ERR_E074      "ERROR E074: 'Too many Arguments specified for %c%c\n"
#define MSG_ERR_E080      "ERROR E080: cannot allocate initial memory : %s\n"
#define MSG_INFO_I043     "I043: Input  File: %s\n"
#define MSG_INFO_I045     "I045: File  Reads: %-ld\n"
#define MSG_WARN_W002     "W002: Open Error Bypass File '%s' : %s\n"
#define SWITCH_CHAR       '-'
#define USG_MSG_ARG_DELM_C      "\t\t\t  the decimal # representing the character.\n"
#define USG_MSG_ARG_DELM_D      "\t\t\t  Default '%c'\n"
#define USG_MSG_ARG_DELM_I      "\t%c%c c\t\t: Input File Field Delimiter, the actual character OR\n"
#define USG_MSG_ARG_DELM_O      "\t%c%c c\t\t: Output File Field Delimiter, the actual character OR\n"
#define USG_MSG_ARG_ERR         "\t%c%c file\t\t: Write errors to file 'file', default stderr\n"
#define USG_MSG_ARG_FORCE       "\t%c%c\t\t: force create of files when found\n"
#define USG_MSG_ARG_HELP        "\t%c%c\t\t: Show brief help and exit\n"
#define USG_MSG_ARG_MAX_COL     "\t%c%c max\t\t: maximum number of colums to process, default %d\n"
#define USG_MSG_ARG_OUT         "\t%c%c file\t\t: Write output to file 'file', default stdout\n"
#define USG_MSG_ARG_VERBOSE_8   "\t%c%c\t\t: verbose level, each time specified level increases\n"
#define USG_MSG_ARG_VERSION     "\t%c%c\t\t: Show revision information and exit\n"
#define USG_MSG_OPTIONS         "Options\n"
#define USG_MSG_USAGE           "usage:\t%s [OPTIONS] [FILES ...]\n"
#endif /* NO_JLIB */

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

#ifdef NO_JLIB
SSIZE_T j2_getline(char **buf, size_t *n, FILE *fp);
char *j2_get_prgname(char *argv_0, char *default_name);
int j2_f_exist(char *file_name);
int j2_is_numr(char *s);
long int j2_clr_str(char *s, char c, int size);
long int j2_fix_delm(char delm, char **fixed, char *buf);
long int j2_justleft(char *s);
long int j2_rtw(char *buffer);
#endif /* NO_JLIB */

#endif /*  JSIDE_H  */
