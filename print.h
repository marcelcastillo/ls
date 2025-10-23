/* Marcel Castillo */
/* Header file for print.c */

#ifndef PRINT_H
#define PRINT_H

#include <sys/stat.h>

#include <ctype.h>
#include <fts.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

struct maxwidths{
    int max_links;
    int max_sizlen;
    int max_usrlen;
    int max_grplen;
};

struct pflags{
    int dashf;
    int dashl;
};

struct maxwidths ft_widths(FTSENT* head_ft);
int suffix(struct stat *st, char *path, char *suf, struct pflags *pf);
int ft_print(FTSENT* ft, struct maxwidths *w, struct pflags *pf);
int regprint(FTSENT* ft, struct pflags *pf);
int unprintable(char* name, char* safename);
int callprint(FTSENT* ft, int longform, struct pflags *pf);



#endif /* PRINT_H */
