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

struct maxwidths ft_widths(FTSENT* head_ft);
int suffix(struct stat *st, char *path, char *suf);
int ft_print(FTSENT* ft, struct maxwidths *w);
int regprint(FTSENT* ft);
int unprintable(char* name, char* safename);



#endif /* PRINT_H */
