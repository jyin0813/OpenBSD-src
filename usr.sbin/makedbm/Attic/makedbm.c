/*
 * Copyright (c) 1994 Mats O Jansson <moj@stacken.kth.se>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef LINT
static char rcsid[] = "$Id: makedbm.c,v 1.1 1995/10/23 07:46:14 deraadt Exp deraadt $";
#endif

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <strings.h>
#include "ypdb.h"
#include "ypdef.h"

char *ProgramName = "makedbm";

/*
 * Read one line
 */

static int read_line(fp, buf, size)
	FILE	*fp;
	char	*buf;
	int	size;
{
	int	done;

	done = 0;

	do {
		while (fgets(buf, size, fp)) {
			int len = strlen(buf);
			done += len;
			if (len > 1 && buf[len-2] == '\\' &&
					buf[len-1] == '\n') {
				int ch;
				buf += len - 2;
				size -= len - 2;
				*buf = '\n'; buf[1] = '\0';
				
				/* Skip leading white space on next line */
				while ((ch = getc(fp)) != EOF &&
					isascii(ch) && isspace(ch))
						;
				(void) ungetc(ch, fp);
			} else {
				return done;
			}
		}
	} while (size > 0 && !feof(fp));

	return done;
}

void 
add_record(db, str1, str2, check)
	DBM	*db;
	char	*str1, *str2;
	int     check;
{
        datum   key,val;
        int     status;

	key.dptr = str1;
	key.dsize = strlen(str1) + 1;
	
	if (check) {
	        val = ypdb_fetch(db,key);

		if (val.dptr == NULL) {
			val.dptr  = str2;
			val.dsize = strlen(str2) + 1;
			status = ypdb_store(db, key, val, YPDB_INSERT);
		} else {
			status = 0;
		}
	} else {
	        val.dptr  = str2;
		val.dsize = strlen(str2) + 1;
		status = ypdb_store(db, key, val, YPDB_INSERT);
	}
	
	if (status != 0) {
		printf("makedbm: problem storing %s %s\n",str1,str2);
		exit(1);
	}
}

static char *
file_date(filename)
	char	*filename;
{
	struct	stat finfo;
	static	char datestr[10];
	int	status;
	
	if (strcmp(filename,"-") == 0) {
		sprintf(datestr, "%010d", time(0));
	} else {
		status = stat(filename, &finfo);
		if (status < 0) {
			fprintf(stderr, "makedbm: can't stat %s\n", filename);
			exit(1);
		}	
		sprintf(datestr, "%010d", finfo.st_mtime);
	}
  	
	return datestr;
}

void
list_database(database)
	char	*database;
{
	DBM	*db;
	datum	key,val;
  	
	db = ypdb_open(database, O_RDONLY, 0444);
	
	if (db == NULL) {
		fprintf(stderr, "makedbm: can't open database %s\n", database);
		exit(1);
	}
	
	key = ypdb_firstkey(db);
	
	while (key.dptr != NULL) {
	        val = ypdb_fetch(db,key);
		printf("%*.*s %*.*s\n",
		       key.dsize - 1,key.dsize - 1,key.dptr,
		       val.dsize - 1,val.dsize - 1,val.dptr);
		key = ypdb_nextkey(db);
	}
	  
	ypdb_close(db);
	
}

void

create_database(infile,database,
		yp_input_file,yp_output_file,
		yp_master_name,yp_domain_name,
		bflag, lflag, sflag)
	char	*infile, *database;
	char	*yp_input_file, *yp_output_file;
	char	*yp_master_name, *yp_domain_name;
	int	bflag, lflag, sflag;        
{
	FILE	*data_file;
	char	data_line[4096];
	char	myname[255];
	int	line_no = 0;
	int	len;
	int	i,j;
	char	*p,*k,*v;
	DBM	*new_db;
	static	char mapname[] = "ypdbXXXXXX";
	char	db_mapname[255],db_outfile[255],db_tempname[255];
	char	empty_str[] = "";
	
	if (strcmp(infile,"-") == 0) {
		data_file = stdin;
	} else {
		data_file = fopen(infile, "r");
	}
	
	j = 0;
	for (i=0; i<strlen(database); i++) {
		if (database[i] == '/') {
			j = i;
		}
	};
	
	for (i=0; i<j; i++) 
		db_tempname[i] = database[i];
	
	if (i != 0)
		db_tempname[i++] = '/';
	
	for (j=0; j<strlen(mapname); j++) {
		db_tempname[i+j] = mapname[j];
		db_tempname[i+j+1] = '\0';
	}
	
	mktemp(db_tempname);
	new_db = ypdb_open(db_tempname, O_RDWR|O_CREAT, 0444);
	
	while (read_line(data_file,data_line,sizeof(data_line))) {
		
		line_no++;
		len =  strlen(data_line);
		
		/* Check if we have the whole line */ 
		
		if (data_line[len-1] != '\n') {
			fprintf(stderr, "line %d in \"%s\" is too long",
				line_no, infile);
		} else {
			data_line[len-1] = '\0';
		}
		
		p = (char *) &data_line;
		
		k  = p;				     /* save start of key */
		while (!isspace(*p)) {		    /* find first "space" */
			if (lflag && isupper(*p))   /* if force lower case */
				*p = tolower(*p);   /* fix it */
			p++;
		};
		while (isspace(*p)) {		/* replace space with <NUL> */
			*p = '\0';
			p++;
		};
		
		v = p;				/* save start of value */
		while(*p != '\0') { p++; };	/* find end of string */
		
		add_record(new_db, k, v, TRUE);	/* save record */
		
	}
	
	if (strcmp(infile,"-") != 0) {
		(void) fclose(data_file);
	}
	
	add_record(new_db, YP_LAST_KEY, file_date(infile), FALSE);
	
	if (yp_input_file) {
		add_record(new_db, YP_INPUT_KEY, yp_input_file, FALSE);
	}
	
	if (yp_output_file) {
		add_record(new_db, YP_OUTPUT_KEY, yp_output_file, FALSE);
	}
	
	if (yp_master_name) {
		add_record(new_db, YP_MASTER_KEY, yp_master_name, FALSE);
	} else {
		gethostname(myname, sizeof(myname) - 1);
		add_record(new_db, YP_MASTER_KEY, myname, FALSE);
	}
	
	if (yp_domain_name) {
		add_record(new_db, YP_DOMAIN_KEY, yp_domain_name, FALSE);
	}
	
	if (bflag) {
		add_record(new_db, YP_INTERDOMAIN_KEY, empty_str, FALSE);
	}

	if (sflag) {
		add_record(new_db, YP_SECURE_KEY, empty_str, FALSE);
	}

	ypdb_close(new_db);
	sprintf(db_mapname,"%s%s",db_tempname,YPDB_SUFFIX);
	sprintf(db_outfile,"%s%s",database,YPDB_SUFFIX);
	rename(db_mapname,db_outfile);
	
}

int
main (argc,argv)
	int	argc;
	char	*argv[];
{
	int	aflag, uflag, bflag, lflag, sflag;
	char	*yp_input_file, *yp_output_file;
	char	*yp_master_name,*yp_domain_name;
	char	*infile,*outfile;
	int	usage = 0;
	int	ch;
	
	extern int optind;
	
	yp_input_file = yp_output_file = NULL;
	yp_master_name = yp_domain_name = NULL;
	aflag = uflag = bflag = lflag = sflag = 0;
	infile = outfile = NULL;
	
	while ((ch = getopt(argc, argv, "blsui:o:m:d:")) != EOF)
		switch (ch) {
			case 'b':
		  		bflag++;
				aflag++;
				break;
			case 'l':
				lflag++;
				aflag++;
				break;
			case 's':
				sflag++;
				aflag++;
				break;
			case 'i':
				yp_input_file = argv[optind];
				aflag++;
				break;
			case 'o':
				yp_output_file = argv[optind];
				aflag++;
				break;
			case 'm':
				yp_master_name = argv[optind];
				aflag++;
				break;
			case 'd':
				yp_domain_name = argv[optind];
				aflag++;
				break;
			case 'u':
				uflag++;
				break;
			default:
				usage++;
				break;
		}
	
	if ((uflag != 0) && (aflag != 0)) {
		usage++;
	} else {
		
		if (uflag != 0) {
			if (argc == (optind + 1)) {
				infile = argv[optind];
			} else {
				usage++;
			}
		} else {
			if (argc == (optind + 2)) {
				infile = argv[optind];
				outfile = argv[optind+1];
			} else {
				usage++;
			}
		}
	}
	
	if (usage) {
		fprintf(stderr,"%s%s%s",
			"usage:\tmakedbm -u file\n\tmakedbm [-bls]",
			" [-i YP_INPUT_FILE] [-o YP_OUTPUT_FILE]\n\t\t",
			"[-d YP_DOMAIN_NAME] [-m YP_MASTER_NAME] infile outfile\n");
		exit(1);
	}
	
	if (uflag != 0) {
		list_database(infile);
	} else {
		create_database(infile,outfile,
				yp_input_file,yp_output_file,
				yp_master_name,yp_domain_name,
				bflag, lflag, sflag);
	}
	
	return(0);
	
}


