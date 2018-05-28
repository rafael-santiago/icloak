/*
 *                                Copyright (C) 2017 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
#include <cutest/src/kutest.h>
#include <icloak.h>

#if defined(ICLOAK_TESTS)

# if defined(__linux__)
#  define ICLOAK_MODNAME "icloak_test"
# elif defined(__FreeBSD__)
#  define ICLOAK_MODNAME "icloak-test"
# elif defined(__NetBSD__)
#  define ICLOAK_MODNAME "icloak_stub_kmod"
# endif

KUTE_DECLARE_TEST_CASE(icloak_test_monkey);

KUTE_DECLARE_TEST_CASE(icloak_ko_nullity_tests);

KUTE_DECLARE_TEST_CASE(icloak_mk_ko_perm_nullity_tests);

KUTE_DECLARE_TEST_CASE(icloak_mk_ko_perm_tests);

KUTE_DECLARE_TEST_CASE(icloak_ko_tests);

KUTE_DECLARE_TEST_CASE(icloak_filename_pattern_ctx_tests);

KUTE_DECLARE_TEST_CASE(strglob_tests);

KUTE_DECLARE_TEST_CASE(icloak_file_hiding_feature_tests);

KUTE_MAIN(icloak_test_monkey);

KUTE_TEST_CASE(icloak_ko_nullity_tests)
    KUTE_ASSERT(icloak_ko(NULL) != 0);
    KUTE_ASSERT(icloak_ko("parangaricutirimirruru") != 0);
KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_mk_ko_perm_nullity_tests)
#if !defined(__NetBSD__)
    void *exit = NULL;
    KUTE_ASSERT(icloak_mk_ko_perm(NULL, NULL) != 0);
    KUTE_ASSERT(icloak_mk_ko_perm(NULL, &exit) != 0);
    KUTE_ASSERT(icloak_mk_ko_perm("parangaricutirimirruaru", &exit) != 0);
    KUTE_ASSERT(icloak_mk_ko_perm("parangaricutirimirruaru", NULL) != 0);
#else
    uprintf("== Feature not available for NetBSD. Just skipping...\n");
#endif
KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_mk_ko_perm_tests)
#if !defined(__NetBSD__)
    void *exit = NULL;
    KUTE_ASSERT(icloak_mk_ko_perm(ICLOAK_MODNAME, &exit) == 0);
    KUTE_ASSERT(icloak_mk_ko_nonperm(ICLOAK_MODNAME, exit) == 0);
#else
    uprintf("== Feature not available for NetBSD. Just skipping...\n");
#endif
KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_ko_tests)
    KUTE_ASSERT(icloak_ko(ICLOAK_MODNAME) == 0);
KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_filename_pattern_ctx_tests)
    icloak_filename_pattern_ctx *head = NULL, *tail = NULL;
    struct expected_sequence {
        char *pattern;
        size_t pattern_size;
    };
    struct expected_sequence expected[] = {
        { "abc", 3 },
        { "foobar", 6 },
        { "*m", 2 },
        { "e*", 2 }
    };
    size_t expected_nr = sizeof(expected) / sizeof(expected[0]), e;

    for (e = 0; e < expected_nr; e++) {
        if (e < (expected_nr / 2)) {
            head = icloak_add_filename_pattern(head, expected[e].pattern, expected[e].pattern_size, &tail);
        } else {
            head = icloak_add_filename_pattern(head, expected[e].pattern, expected[e].pattern_size, NULL);
        }
        KUTE_ASSERT(head != NULL);
    }

    for (e = 0, tail = head; e < expected_nr; e++, tail = tail->next) {
        KUTE_ASSERT(tail->pattern_size == expected[e].pattern_size);
        KUTE_ASSERT(memcmp(tail->pattern, expected[e].pattern, tail->pattern_size) == 0);
    }

    head = icloak_del_filename_pattern(head, "abcd");
    KUTE_ASSERT(head != NULL);
    head = icloak_del_filename_pattern(head, "abc");
    KUTE_ASSERT(head != NULL);
    head = icloak_del_filename_pattern(head, "*m");
    KUTE_ASSERT(head != NULL);
    head = icloak_del_filename_pattern(head, "foobar");
    KUTE_ASSERT(head != NULL);
    head = icloak_del_filename_pattern(head, "e*");
    KUTE_ASSERT(head == NULL);

    head = icloak_add_filename_pattern(head, "boo*", 4, NULL);
    head = icloak_add_filename_pattern(head, "tests.*", 7, NULL);
    head = icloak_add_filename_pattern(head, "onomatopaico[123]", 17, NULL);

    KUTE_ASSERT(head != NULL);

    KUTE_ASSERT(icloak_match_filename("tests.c", head) == 1);
    KUTE_ASSERT(icloak_match_filename("tests.h", head) == 1);
    KUTE_ASSERT(icloak_match_filename("icloak.c", head) == 0);
    KUTE_ASSERT(icloak_match_filename("onomatopaico3", head) == 1);

    icloak_free_icloak_filename_pattern(head);

KUTE_TEST_CASE_END

KUTE_TEST_CASE(strglob_tests)
    struct strglob_test {
        char *str;
        char *pattern;
        int result;
    };
    struct strglob_test tests[] = {
        {                       "abcdef",          "*cde*", 1 },
        {                          "abc",            "a?c", 1 },
        {                          "a.c",            "a?c", 1 },
        {                           "a0",  "a[1234567890]", 1 },
        {                           "a1",  "a[1234567890]", 1 },
        {                           "a2",  "a[1234567890]", 1 },
        {                           "a3",  "a[1234567890]", 1 },
        {                           "a4",  "a[1234567890]", 1 },
        {                           "a5",  "a[1234567890]", 1 },
        {                           "a6",  "a[1234567890]", 1 },
        {                           "a7",  "a[1234567890]", 1 },
        {                           "a8",  "a[1234567890]", 1 },
        {                           "a9",  "a[1234567890]", 1 },
        { "abcdefghijhklmnopqrstuvwxyz1", " a[1234567890]", 0 },
        { "abcdefghijhklmnopqrstuvwxyz0", "a*[1234567890]", 1 },
        {                            "x",             "a*", 0 }
    };
    size_t tests_nr = sizeof(tests) / sizeof(tests[0]), t;

    for (t = 0; t < tests_nr; t++) {
        KUTE_ASSERT(strglob(tests[t].str, tests[t].pattern) == tests[t].result);
    }
KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_file_hiding_feature_tests)
#if !defined(__NetBSD__)
    KUTE_ASSERT(icloak_hide_file("icloak.h") == 0);
    KUTE_ASSERT(icloak_hide_file("tests.c") == 0);
    KUTE_ASSERT(icloak_show_file("icloak.h") == 0);
#else
    uprintf("== Feature not available for NetBSD. Just skipping.\n");
#endif
KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_test_monkey)
    KUTE_RUN_TEST(strglob_tests);
    KUTE_RUN_TEST(icloak_filename_pattern_ctx_tests);
    KUTE_RUN_TEST(icloak_ko_nullity_tests);
    KUTE_RUN_TEST(icloak_mk_ko_perm_nullity_tests);
    KUTE_RUN_TEST(icloak_file_hiding_feature_tests);
    KUTE_RUN_TEST(icloak_mk_ko_perm_tests);
    KUTE_RUN_TEST(icloak_ko_tests);
KUTE_TEST_CASE_END

#undef ICLOAK_MODNAME

#endif
