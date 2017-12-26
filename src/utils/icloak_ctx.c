/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <utils/icloak_ctx.h>
#include <utils/memory.h>

static icloak_filename_pattern_ctx *icloak_get_filename_pattern_tail(icloak_filename_pattern_ctx *head);

#define icloak_new_filename_pattern_entry(e) {\
    (e) = icloak_alloc(sizeof(icloak_filename_pattern_ctx));\
    (e)->next = NULL;\
    (e)->pattern = NULL;\
    (e)->pattern_size = 0;\
}

icloak_filename_pattern_ctx *icloak_add_filename_pattern(icloak_filename_pattern_ctx *head,
                                                         const char *pattern, size_t pattern_size,
                                                         icloak_filename_pattern_ctx **tail) {
    icloak_filename_pattern_ctx *hp, *np;

    if (pattern == NULL || pattern_size == 0) {
        return head;
    }

    if (head == NULL) {
        icloak_new_filename_pattern_entry(hp);

        if (hp == NULL) {
            return head;
        }

        np = hp;
    } else {
        if (tail == NULL || (*tail) == NULL) {
            np = icloak_get_filename_pattern_tail(head);
        } else {
            np = (*tail);
        }

        icloak_new_filename_pattern_entry(np->next);

        if (np->next == NULL) {
            return head;
        }

        np = np->next;
        hp = head;
    }

    np->pattern = (char *) icloak_alloc(pattern_size + 1);

    if (np->pattern == NULL) {
        return head;
    }

    memset(np->pattern, 0, pattern_size + 1);

    np->pattern_size = pattern_size;
    memcpy(np->pattern, pattern, pattern_size);

    if (tail != NULL) {
        (*tail) = np;
    }

    return hp;
}

icloak_filename_pattern_ctx *icloak_del_filename_pattern(icloak_filename_pattern_ctx *head, const char *pattern) {
    icloak_filename_pattern_ctx *hp, *lp, *del;

    if (head == NULL) {
        return NULL;
    }

    lp = del = NULL;

    for (hp = head; hp != NULL; hp = hp->next) {
        if (strcmp(hp->pattern, pattern) == 0) {
            if (hp == head) {
                hp = hp->next;
                head->next = NULL;
                del = head;
                goto icloak_del_filename_pattern_epilogue;
            } else {
                lp->next = hp->next;
                hp->next = NULL;
                del = hp;
                hp = head;
                goto icloak_del_filename_pattern_epilogue;
            }
        }
        lp = hp;
    }

    return head;

icloak_del_filename_pattern_epilogue:

    if (del != NULL) {
        icloak_free_icloak_filename_pattern(del);
        del = NULL;
    }

    lp = NULL;

    return hp;
}

void icloak_free_icloak_filename_pattern(icloak_filename_pattern_ctx *head) {
    icloak_filename_pattern_ctx *hp, *tp;

    for (hp = tp = head; tp != NULL; hp = tp) {
        tp = hp->next;
        if (hp->pattern != NULL) {
            icloak_free(hp->pattern);
        }
        icloak_free(hp);
    }
}

static icloak_filename_pattern_ctx *icloak_get_filename_pattern_tail(icloak_filename_pattern_ctx *head) {
    icloak_filename_pattern_ctx *hp;

    if (head == NULL) {
        return NULL;
    }

    for (hp = head; hp->next != NULL; hp = hp->next)
        ;

    return hp;
}
