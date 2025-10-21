/* cmp.h */
#ifndef PRINT_H
#define PRINT_H

#include <sys/types.h>

#include <fts.h>
#include <stdint.h>
#include <string.h>

int compare(const FTSENT **a, const FTSENT **b);

#endif /* PRINT_H */


