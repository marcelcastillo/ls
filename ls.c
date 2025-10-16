/* Marcel Castillo
 * CS 631 Adv. Prog. in Unix
 * I pledge my honor that I have abided by the Stevens Honor System
 * Midterm Project - ls
 */

#include <sys/types.h>

#include <err.h>
#include <fts.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "cmp.h"

int
main(int argc, char *argv[])
{
	FTS		*ftsp;
	FTSENT	*p;
	FTSENT	*child;	
	int ch;
	int fts_options;
	int f_listdot = 	0;	
	int f_sort = 		1;			// Sort by default
	int f_recursive = 	0;
	char ** fts_args;
	char *default_argv[] = {".", '\0'};
	int (*cmp)(const FTSENT **, const FTSENT **);

	fts_options = FTS_PHYSICAL;		// Option bit-mask, start by not following symbolic links
									
	while ((ch = getopt(argc, argv, "-AacdFfhiklnqRrSstuw")) != -1){
		switch (ch){
			case 'R':
				f_recursive = 1;
				break;
			case 'f':
				f_sort = 0;
				/* FALLTHROUGH */
			case 'a':
				fts_options |= FTS_SEEDOT;	// Shows hidden directories
				/* FALLTHROUGH */
			case 'A':
				f_listdot = 1;				// Lists all entries except for '.' and '..'
				break;						// Logical flag that is used 
			case '?':
				fprintf(stdout, "usage: %s [-AacdFfhiklnqRrSstuw] [file ...]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	/* -A is always set for the superuser */
	if (getuid() == 0){
		f_listdot = 1;
	}

	/* Skip past the flag arguments */
	argc -= optind;
	argv += optind;

	if (argc == 0){
		fts_args = default_argv;
	} else {
		fts_args = argv;
	}

	/* Select appropriate sorting comparison */
	if (f_sort == 0)
		cmp = NULL;
	else
		cmp = compare;

	/* Open FTS on the provided path */
	if ((ftsp = fts_open(fts_args, fts_options, cmp)) == NULL){
			perror("fts_open");
			exit(EXIT_FAILURE);
	}
	
	/* Traverse through the fts tree abstraction */
	while ((p = fts_read(ftsp)) != NULL){						// Begins with the target of ls
		switch (p->fts_info){
			case FTS_DNR:
			case FTS_ERR:
				warn("%s: %s", p->fts_path, strerror(p->fts_errno));
				break;

			/* When p points to a directory FTSENT struct */
			case FTS_D:
				/* Skip '.' prefix files when -a or -A are set */
				if (p->fts_level != FTS_ROOTLEVEL &&
					p->fts_name[0] == '.' && f_listdot == 0)
					{
						(void)fts_set(ftsp, p, FTS_SKIP);
						break;
					}

				if (p->fts_info == FTS_ERR)
    				warn("%s: %s", p->fts_path, strerror(p->fts_errno));

				/* Print directory header */
				if (p->fts_level != FTS_ROOTLEVEL)
					printf("%s:\n", p->fts_path);
				child = fts_children(ftsp, 0);
				if (child == NULL){
					warn("%s", p->fts_path);
					continue;
				}
				/* Print children */
				/* If operating regularly (non -a or -A), hide hiddens */
				for (FTSENT *c = child; c != NULL; c = c->fts_link){
					if (f_listdot == 0 && c->fts_name[0] == '.'){
						continue;
					}
					printf("%s\n", c->fts_name);
				}
				if (f_recursive == 0){
					fts_set(ftsp, p, FTS_SKIP);
					break;
				}
		}
	}

	fts_close(ftsp);
	exit(EXIT_SUCCESS);
}

	
