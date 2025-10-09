/* Marcel Castillo
 * CS 631 Adv. Prog. in Unix
 * I pledge my honor that I have abided by the Stevens Honor System
 */

#include <sys/types.h>

#include <err.h>
#include <fts.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	FTS *ftsp;
	FTSENT *p;
	FTSENT *child;	
	int ch;
	int fts_options;
	//static char dot[] = ".";

	int f_listdot = 0;		/* */
	int f_nosort = 0;

	fts_options = FTS_PHYSICAL;		// Option bit-mask
	while ((ch = getopt(argc, argv, "-AacdFfhiklnqRrSstuw")) != -1){
		switch (ch){
			case 'f':
				f_nosort = 1;
				/* FALLTHROUGH */
			case 'a':
				fts_options |= FTS_SEEDOT;	// Shows hidden directories
				/* FALLTHROUGH */
			case 'A':
				f_listdot = 1;				// Lists all entries except for '.' and '..'
				break;
		}
	}

	/* -A is always set for the superuser */
	if (!getuid()){
		f_listdot = 1;
	}

	if (argc < 2){
		fprintf(stderr, "usage: %s [-AacdFfhiklnqRrSstuw] [file ...]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Open FTS on the provided path */
	/*						*/
	ftsp = fts_open(argv + 1, fts_options | FTS_NOCHDIR, NULL);
	if (ftsp == NULL){
		perror("fts_open");
		exit(EXIT_FAILURE);
	}
	
	/* Traverse through the fts tree abstraction */
	while ((p = fts_read(ftsp)) != NULL)
		switch (p->fts_info){
			case FTS_D:
				/* Do not traverse into hidden directories (-a or -A are set) */
				if (p->fts_level != FTS_ROOTLEVEL &&
					p->fts_name[0] == '.' &&
					!f_listdot)
					{
						(void)fts_set(ftsp, p, FTS_SKIP);
						break;
					}
				//Print directory header
				printf("%s :\n", p->fts_path);
				child = fts_children(ftsp, 0);
				if (child == NULL){
					warn("%s", p->fts_path);
					continue;
				}
				/* Print children */
				/* If operating regularly (non -a or -A), hide hiddens */
				for (FTSENT *c = child; c != NULL; c = c->fts_link){
					if (!f_listdot && c->fts_name[0] == '.'){
						continue;
					}
					printf("	%s\n", c->fts_name);
				}
				fts_set(ftsp, p, FTS_SKIP);
				break;
		}
	if (f_nosort){
		printf("lemons\n");
	}
	fts_close(ftsp);
	exit(EXIT_SUCCESS);
}

	
