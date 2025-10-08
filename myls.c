#include <sys/types.h>

#include <err.h>
#include <fts.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	FTS *ftsp;
	FTSENT *p;
	FTSENT *child;	
	int listdot = 0;

	if (argc < 2){
		fprintf(stderr, "usage: %s path....\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Open FTS on the provided path */
	ftsp = fts_open(argv + 1, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
	if (ftsp == NULL){
		perror("fts_open");
		exit(EXIT_FAILURE);
	}
	

	/* Traverse through the fts tree abstraction */
	while ((p = fts_read(ftsp)) != NULL)
		switch (p->fts_info){
			case FTS_DC:
				warnx("%s: directory causes a cycle", p->fts_name);
				break;
			case FTS_DNR:
			case FTS_ERR:
				warnx("%s: %s", p->fts_path, strerror(p->fts_errno));
				exit(EXIT_FAILURE);
				break;
			case FTS_D:
				if (p->fts_level != FTS_ROOTLEVEL &&
					p->fts_name[0] == '.' && !listdot){
					(void)fts_set(ftsp, p, FTS_SKIP);
					break;
				}
				//Print directory header
				printf("%s:\n", p->fts_path);
				child = fts_children(ftsp, 0);
				if (child == NULL){
					warn("%s", p->fts_path);
					continue;
				}
				/* Print children */
				for (FTSENT *c = child; c != NULL; c = c->fts_link){
					printf("	%s\n", c->fts_name);
				}
				fts_set(ftsp, p, FTS_SKIP);
				break;
		}
	fts_close(ftsp);
	exit(EXIT_SUCCESS);
}

	
