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
# endif

KUTE_DECLARE_TEST_CASE(icloak_test_monkey);

KUTE_DECLARE_TEST_CASE(icloak_ko_nullity_tests);

KUTE_DECLARE_TEST_CASE(icloak_mk_ko_perm_nullity_tests);

KUTE_DECLARE_TEST_CASE(icloak_mk_ko_perm_tests);

KUTE_DECLARE_TEST_CASE(icloak_ko_tests);

KUTE_DECLARE_TEST_CASE(icloak_filename_pattern_ctx_tests);

KUTE_MAIN(icloak_test_monkey);

KUTE_TEST_CASE(icloak_ko_nullity_tests)
    KUTE_ASSERT(icloak_ko(NULL) != 0);
    KUTE_ASSERT(icloak_ko("parangaricutirimirruru") != 0);
KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_mk_ko_perm_nullity_tests)
    void *exit = NULL;
    KUTE_ASSERT(icloak_mk_ko_perm(NULL, NULL) != 0);
    KUTE_ASSERT(icloak_mk_ko_perm(NULL, &exit) != 0);
    KUTE_ASSERT(icloak_mk_ko_perm("parangaricutirimirruaru", &exit) != 0);
    KUTE_ASSERT(icloak_mk_ko_perm("parangaricutirimirruaru", NULL) != 0);
KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_mk_ko_perm_tests)
    void *exit = NULL;
    KUTE_ASSERT(icloak_mk_ko_perm(ICLOAK_MODNAME, &exit) == 0);
    KUTE_ASSERT(icloak_mk_ko_nonperm(ICLOAK_MODNAME, exit) == 0);
KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_ko_tests)
    KUTE_ASSERT(icloak_ko(ICLOAK_MODNAME) == 0);
KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_filename_pattern_ctx_tests)
    icloak_filename_pattern_ctx *plist = NULL, *tail = NULL;

    plist = icloak_add_filename_pattern(plist, "abc", 3, &tail);
    plist = icloak_add_filename_pattern(plist, "foobar", 6, &tail);
    plist = icloak_add_filename_pattern(plist, "*m", 2, NULL);
    plist = icloak_add_filename_pattern(plist, "e*", 2, NULL);

    plist = icloak_del_filename_pattern(plist, "abcd");

    plist = icloak_del_filename_pattern(plist, "abc");
    plist = icloak_del_filename_pattern(plist, "*m");
    plist = icloak_del_filename_pattern(plist, "foobar");
    plist = icloak_del_filename_pattern(plist, "e*");

KUTE_TEST_CASE_END

KUTE_TEST_CASE(icloak_test_monkey)
    KUTE_RUN_TEST(icloak_ko_nullity_tests);
    KUTE_RUN_TEST(icloak_mk_ko_perm_nullity_tests);
    //KUTE_RUN_TEST(icloak_mk_ko_perm_tests);
    //KUTE_RUN_TEST(icloak_ko_tests);
KUTE_TEST_CASE_END

#undef ICLOAK_MODNAME

#endif
