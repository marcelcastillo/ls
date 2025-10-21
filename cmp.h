/* cmp.h */
#ifndef CMP_H
#define CMP_H

#include <sys/types.h>

#include <fts.h>
#include <stdint.h>
#include <string.h>

int compare(const FTSENT **a, const FTSENT **b);

#endif /* CMP_H */


