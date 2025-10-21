/* Marcel Castillo
 * CS-631-A
 * Adv. Prog. in the Unix Env.
 * Midterm
 * I pledge my honor that I have abided by the Stevens Honor System
 *
 * The functions below are the various printing methods for ls.c
 */

#include "print.h"

/* Stat style file-type suffix */
char *
suffix(const struct stat *st, const char *path) 
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
ft_print(const FTSENT* ft) 
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

    snprintf(suf, sizeof(suf), "%s", suffix(st, ft->fts_path));

    printf("%s %lu %s %s %lld %s %s", 
        modebuf,
        (unsigned long)st->st_nlink,
        pw ? pw->pw_name : uid,
        gr ? gr->gr_name : gid,
        (long long)st->st_size,
        timebuf,
        ft->fts_name
    );
    printf("%s", suf);
    printf("\n");

    return 0;
}

/* Normal, non-l printing */
int
regprint(const FTSENT* ft){
    printf("%s\n", ft->fts_name);
    return 0;
}
