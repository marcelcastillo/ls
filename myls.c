#include <sys/types.h>


#include <fts.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
	FTS *ftsp;
	FTSENT *ent;
	FTSENT *child;


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
	
	/* Move to the first (root) node */
	ent = fts_read(ftsp);
	if (ent == NULL){
		perror("fts_read");
		fts_close(ftsp);
		exit(EXIT_FAILURE);
	}

	// Make sure argument is a directory
	if (ent->fts_info != FTS_D){
		fprintf(stderr, "%s is not a directory\n", argv[1]);
		fts_close(ftsp);
		exit(EXIT_FAILURE);
	}

	// Get immediate children of this directory
	child = fts_children(ftsp, 0);
	if (child == NULL){
		perror("fts_children");
		fts_close(ftsp);
		exit(EXIT_FAILURE);
	}

	// Print only the child names
    for (FTSENT *p = child; p != NULL; p = p->fts_link) {
        printf("%s\n", p->fts_name);
    }


	fts_close(ftsp);
	exit(EXIT_SUCCESS);
}

	
