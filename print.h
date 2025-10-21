/* Marcel Castillo */
/* Header file for print.c */

#ifndef PRINT_H
#define PRINT_H

#include <sys/stat.h>

#include <fts.h>
#include <grp.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

char *suffix(const struct stat *st, const char *path);
int ft_print(const FTSENT* ft);
int regprint(const FTSENT* ft);

#endif /* PRINT_H */
