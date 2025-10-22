/* Marcel Castillo
 * CS-631-A
 * Adv. Prog. in the Unix Env.
 * Midterm
 * I pledge my honor that I have abided by the Stevens Honor System
 *
 * The functions below are the various printing methods for ls.c
 */

#include "print.h"

/* Scan through the child fts_stat structs for max values */
struct maxwidths
ft_widths(FTSENT* child)
{
    /* Initialize the width struct to 10 */
    struct maxwidths w = {1, 1, 1, 1};
    int len_links;
    int len_size;
    int len_uname;
    int len_gname;
    struct passwd *pw;
    struct group *gr;

    for (FTSENT *c = child; c != NULL; c = c->fts_link){

        struct stat *st = c->fts_statp;
       
        /* Find the length of the longest string necessary to print link count and file sizes */
        len_links = snprintf(NULL, 0, "%lld", (long long)st->st_nlink);
        if (len_links > w.max_links)
            w.max_links = len_links;

        len_size = snprintf(NULL, 0, "%lld", (long long)st->st_size);
        if (len_size > w.max_sizlen)
            w.max_sizlen = len_size;
        
        /* Find the length of the longest pw_name in this directory */
        pw = getpwuid(st->st_uid);
        if (pw != NULL){
            len_uname = strlen(pw->pw_name);
        } else {
            len_uname = snprintf(NULL, 0, "%u", st->st_uid);
        }
        if (len_uname > w.max_usrlen)
            w.max_usrlen = len_uname;

        /* Find the length of the longest gr_name in this directory */
        gr = getgrgid(st->st_gid);
        if (gr != NULL){
            len_gname = strlen(gr->gr_name);
        } else {
            len_gname = snprintf(NULL, 0, "%u", st->st_gid);
        }
        if (len_gname > w.max_grplen)
            w.max_grplen = len_gname;

    }

    return w;
}

/* Stat style file-type suffix */
char *
suffix(struct stat *st, char *path) 
{
    ssize_t         len;
    static char     buf[PATH_MAX];
    char            target[PATH_MAX - 4];

    if  (S_ISLNK(st->st_mode)){
        if  ((len = readlink(path, target, sizeof(target) - 1)) != -1){
            target[len] = '\0';
            snprintf(buf, sizeof(buf), " -> %s", target);
        } else {
            buf[0] = '\0';
        }
    } 
    return buf;
}

/* Stat-like printing function for handling -l flag */
int 
ft_print(FTSENT* ft, struct maxwidths *w) 
{   
    struct stat         *st = ft->fts_statp;
    struct group        *gr;
    struct passwd       *pw;
    struct tm           *tm;
    char                timebuf[64];    /* Conservative choice for time buffer */
    char                uid[12];
    char                gid[12];
    char                modebuf[12];        /* Length of mode characters */
    char                suf[PATH_MAX];


    strmode(st->st_mode, modebuf);
    pw = getpwuid(st->st_uid);
    gr = getgrgid(st->st_gid);
    snprintf(uid, sizeof(uid), "%u", st->st_uid);
    snprintf(gid, sizeof(gid), "%u", st->st_gid);
    tm = localtime(&st->st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %e %H:%M %Y", tm);
    snprintf(suf, sizeof(suf), "%s", suffix(st, ft->fts_accpath)); //TODO: Handle the readlink not working
    //printf("%s\n", ft->fts_accpath);
    printf("%s %*lu %-*s  %-*s  %*lld %s %s%s\n", 
        modebuf,
        w->max_links, (unsigned long)st->st_nlink,
        w->max_usrlen, pw ? pw->pw_name : uid,
        w->max_grplen, gr ? gr->gr_name : gid,
        w->max_sizlen, (long long)st->st_size,
        timebuf,
        ft->fts_name,
        suf
    );

    return 0;
}

/* Normal, non-l printing */
int
regprint(FTSENT* ft){
    printf("%s\n", ft->fts_name);
    return 0;
}
