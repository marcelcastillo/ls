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


	if (argc < 2){
		fprintf(stderr, "usage: %s path....\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Open FTS non-recursively */
	ftsp = fts_open(argv + 1, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
	if (ftsp == NULL){
		perror("fts_open");
		exit(EXIT_FAILURE);
	}

    // iterate through all roots
    while ((ent = fts_read(ftsp)) != NULL) {
        if (ent->fts_info == FTS_D) {  // only act on directories
            printf("%s:\n", ent->fts_path);

            FTSENT *child = fts_children(ftsp, 0);
            if (!child) {
                perror("fts_children");
                continue;
            }

            for (FTSENT *p = child; p; p = p->fts_link)
                printf("  %s\n", p->fts_name);

            printf("\n");
        }
    }



	fts_close(ftsp);
	exit(EXIT_SUCCESS);
}

	
