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
#include "print.h"



int
main(int argc, char *argv[])
{
    FTS     *ftsp;
    FTSENT  *dir;
    FTSENT  *child;
    int ch;
    int fts_options;
    int dashd =     0;
    int dashf =     0;
    int dashl =     0;
    int dashi =     0;
    int dashs =     0;
    int listdirs =  0;
    int longform =  0;
    int listdot =   0;  
    int sort =      1;          // Sort by default
    int recursive =     0;
    char ** fts_args;
    char *default_argv[] = {".", '\0'};
    int (*cmp)(const FTSENT **, const FTSENT **);
    struct pflags pf;

    
    fts_options = FTS_PHYSICAL;     // Option bit-mask, start by not following symbolic links

    while ((ch = getopt(argc, argv, "-AacdFfhiklnqRrSstuw")) != -1){
        switch (ch){
            case 'd':
                recursive = 0;
                listdirs = 1;
                dashd = 1;
                break;
            case 'i':
                dashi = 1;
                break;
            case 's':
                dashs = 1;
                break;
            case 'F':
                dashf = 1;
                break;
            case 'l':
                longform = 1;
                dashl = 1;
                break;
            case 'R':
                recursive = 1;
                dashd = 0;
                break;
            case 'f':
                sort = 0;
                break;  
            case 'a':
                fts_options |= FTS_SEEDOT;  // Shows hidden directories
                listdot = 1;
                break;
            case 'A':
                listdot = 1;                // Lists all entries except for '.' and '..'
                break;                      // Logical flag that is used 
            case '?':
                fprintf(stdout, "usage: %s [-AacdFfhiklnqRrSstuw] [file ...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    pf.dashd = dashd;
    pf.dashf = dashf;
    pf.dashl = dashl;
    pf.dashi = dashi;
    pf.dashs = dashs;

    /* -A is always set for the superuser */
    if (getuid() == 0){
        listdot = 1;
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
    if (sort == 0)
        cmp = NULL;
    else
        cmp = compare;

    /* Open FTS on the provided path. Returns a handle on the file heirarchy
     * Later, supplied to the other fts functions */
    if ((ftsp = fts_open(fts_args, fts_options, cmp)) == NULL){
            perror("fts_open");
            exit(EXIT_FAILURE);
    }
  
    /* Traverse through the fts tree abstraction. fts_read returns a pointer to the FTSENT
     * struct describing one of the files in the file heirarchy */
    while ((dir = fts_read(ftsp)) != NULL){   // Begins with the target of ls
        switch (dir->fts_info){
            case FTS_F:
                /* Only print files here that were provided as program arguments */
                if (dir->fts_level == FTS_ROOTLEVEL){
                    callprint(dir, longform, &pf);
                }
                break;
 
            case FTS_DNR:
            case FTS_ERR:
                warn("FTS_ERR %s: %s", dir->fts_path, strerror(dir->fts_errno));
                break;

            /* When p points to a directory FTSENT struct */
            case FTS_D:
                /* Skip '.' prefix files when -a or -A are set */
                if (dir->fts_level != 0 &&
                    dir->fts_name[0] == '.'
                    && listdot == 0){
                        (void)fts_set(ftsp, dir, FTS_SKIP);
                        break;
                }

                if (dir->fts_info == FTS_ERR)
                    warn("%s: %s", dir->fts_path, strerror(dir->fts_errno));

                /* Print directory header */
                if ((dir->fts_level != FTS_ROOTLEVEL || argc > 1) && listdirs == 0)
                    printf("%s:\n", dir->fts_path);

                /* -d flag */
                if (listdirs){
                    callprint(dir, longform, &pf);
                    break;
                }
               
                /* Get the fts linked list of children of the current directory */
                if ((child = fts_children(ftsp, 0)) == NULL){
                    printf("\n");
                    continue;
                }

                /* If -l stat() style printing */
                if (longform == 1){
                    struct maxwidths w = ft_widths(child);
                    for (FTSENT *c = child; c != NULL; c = c->fts_link){
                        /* If operating regularly (non -a or -A), hide hiddens */
                        if (listdot == 0 && c->fts_name[0] == '.'){
                            continue;
                        }
                        (void)ft_print(c, &w, &pf);
                     }

                /* Regular printing */
                } else {
                    for (FTSENT *c = child; c != NULL; c = c->fts_link){

                        /* If operating regularly (non -a or -A), hide hiddens */
                        if (listdot == 0 && c->fts_name[0] == '.'){
                            continue;
                        }
                        regprint(c, &pf);
                    }
                }
                printf("\n");
        } 
        if (recursive == 0){
            fts_set(ftsp, dir, FTS_SKIP);
        }
    }

    fts_close(ftsp);
    exit(EXIT_SUCCESS);
}

    
