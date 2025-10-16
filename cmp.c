/* cmp.c */

#include <string.h>

#include "cmp.h"

int compare(const FTSENT **a, const FTSENT **b){
	return (strcmp((*a)->fts_name, (*b)->fts_name));
}
