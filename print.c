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
int
suffix(struct stat *st, char *path, char *suf, struct pflags *pf) 
{
    ssize_t         len;
    char            target[PATH_MAX - 4];
    
    if (pf->dashf){
        switch (st->st_mode) {
            case S_IFDIR:
                strncpy(suf, "/", sizeof(char)*2);
                break;
            case S_IFLNK:
                strncpy(suf, "@", sizeof(char)*2);
                break;
            case S_IFSOCK:
                strncpy(suf, "=", sizeof(char)*2);
                break;
            case S_IFIFO:
                strncpy(suf, "|", sizeof(char)*2);
                break;
            case S_IFWHT:
                strncpy(suf, "%", sizeof(char)*2);
                break;
            default:
                if (st->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
                    strncpy(suf, "*", sizeof(char)*2);
        }
    }

    if  ((S_ISLNK(st->st_mode)) && pf->dashl){
        if  ((len = readlink(path, target, sizeof(target) - 1)) != -1){
            target[len] = '\0';
            strncat(suf, " -> ", 5 * sizeof(char));
            strncat(suf, target, strlen(target));
        }

    } 
    return 0;
}

/* Stat-like printing function for handling -l flag */
int 
ft_print(FTSENT* ft, struct maxwidths *w, struct pflags *pf) 
{   
    struct stat         *st = ft->fts_statp;
    struct group        *gr;
    struct passwd       *pw;
    struct tm           *tm;
    char                timebuf[64];            /* Conservative choice for time buffer */
    char                uid[12];                /* Conservative choice for username id length */
    char                gid[12];
    char                modebuf[12];            /* Length of mode characters */
    char                suf[PATH_MAX] = "";    
    char                path[PATH_MAX];         /* Path to file in question */
    char                safename[PATH_MAX];     /* For printing unprintables */

    strmode(st->st_mode, modebuf);
    pw = getpwuid(st->st_uid);
    gr = getgrgid(st->st_gid);
    snprintf(uid, sizeof(uid), "%u", st->st_uid);
    snprintf(gid, sizeof(gid), "%u", st->st_gid);
    tm = localtime(&st->st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %e %H:%M %Y", tm);

    /* Build the path out of the data in the ft_sent struct */
    snprintf(path, sizeof(path), "%s/", ft->fts_path);
    strlcat(path, ft->fts_name, sizeof(path));
    suffix(st, path, suf, pf);                           
    unprintable(ft->fts_name, safename);

    /* Formatted print */
    printf("%s %*lu %-*s  %-*s  %*lld %s %s%s\n", 
        modebuf,
        w->max_links, (unsigned long)st->st_nlink,
        w->max_usrlen, pw ? pw->pw_name : uid,       /* Print uid if the get*id functions found no match */
        w->max_grplen, gr ? gr->gr_name : gid,
        w->max_sizlen, (long long)st->st_size,
        timebuf,
        safename,
        suf
    );

    return 0;
}

/* Normal, non-l printing */
int
regprint(FTSENT* ft, struct pflags *pf){
    char    fname[PATH_MAX];
    int     suf;

    snprintf(fname, sizeof(fname), "%s/", ft->fts_name);
    suf = strlen(fname);
    suffix(ft->fts_statp, NULL, fname + suf, pf);  

    printf("%s\n", fname);
    return 0;
}

/* Helper to print non-printables */
int
unprintable(char* name, char* safename)
{
    int len = strlen(name);

    for (int i = 0; i < len; i++){
        char c = name[i];
        if (isprint(c)){
            safename[i] = c;
        } else{
            safename[i] = '?';
        }
    }
    safename[len] = '\0';
    return 0;
}

/* Helper to call the correct printing method for single files/dirs */
int
callprint(FTSENT* ft, int longform, struct pflags *pf){
    if (longform == 1){
        struct maxwidths w = ft_widths(ft);
        (void)ft_print(ft, &w, pf);
    } else {
        regprint(ft, pf);
    }
    return 0;

}


