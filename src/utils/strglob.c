/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <utils/strglob.h>

#if defined(__linux__)
# include <linux/kernel.h>
# include <linux/string.h>
#elif defined(__FreeBSD__)
# include <sys/malloc.h>
# include <sys/systm.h>
#elif defined(__NetBSD__)
# include <sys/malloc.h>
# include <sys/systm.h>
#endif

int strglob(const char *str, const char *pattern) {
    const char *sp, *sp_end;
    const char *p, *p_end;
    int matches = 1;

    if (str == NULL || pattern == NULL) {
        return 0;
    }

    if (strcmp(pattern, "*") == 0 || *pattern == 0) {
        return 1;
    }

    sp = str;
    sp_end = sp + strlen(sp);
    p = pattern;
    p_end = p + strlen(p);

    while (matches && p != p_end && sp != sp_end) {
        switch (*p) {
            case '*':
                matches = (*(p + 1) == 0) && (*(sp + 1) == 0);

                while (!matches && sp != sp_end) {
                    matches = strglob(sp, p + 1);
                    sp++;
                }

                if (matches) {
                    sp = sp_end;
                    p = p_end;
                }

                goto strglob_epilogue;

            case '?':
                matches = (*sp != 0);
                break;

            case '[':
                matches = 0;
                p++;

                while (!matches && sp != sp_end && *p != ']') {
                    matches = (*sp == *p);
                    p++;
                }

                if (matches && *p != ']') {
                    while (*p != ']' && p != p_end) {
                        p++;
                    }
                }
                break;

            default:
                matches = (*sp == *p);
                break;
        }
        p++;
        sp++;
    }

strglob_epilogue:
    if (matches && sp == sp_end && p != p_end && *p == '*') {
        p++;
    }

    matches = (matches && (p == p_end && sp == sp_end));

    return matches;
}
