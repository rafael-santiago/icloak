/*
 *                                Copyright (C) 2018 by Rafael Santiago
 *
 * This is a free software. You can redistribute it and/or modify under
 * the terms of the GNU General Public License version 2.
 *
 */
// INFO(Rafael): NetBSD actually needs another kernel module.
//               Because the module is only inserted in the module list
//               after a well-succeeded `MODULE_CMD_INIT`.
#include "../cutest/src/kutest.h"

KUTE_DECLARE_TEST_CASE(icloak_stub_kmod);

KUTE_TEST_CASE(icloak_stub_kmod)
KUTE_TEST_CASE_END

KUTE_MAIN(icloak_stub_kmod)
