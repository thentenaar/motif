/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar.
 * Copyright (c) 1987 - 2012 The Open Group.
 * Licensed under the LGPL 2.1 license.
 */
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <check.h>
#include "suites.h"

XtAppContext app;

/**
 * Xt fixture
 */
Widget init_xt(const char *klass)
{
	Widget shell;
	int argc = 0;

	XtSetLanguageProc(NULL, NULL, NULL);
	XtToolkitInitialize();
	shell = XtAppInitialize(&app, klass, NULL, 0, &argc, NULL, NULL, NULL, 0);
	ck_assert_msg(app, "Failed to initialize app context");
	return shell;
}

void uninit_xt(void)
{
	if (app)
		XtDestroyApplicationContext(app);
	app = NULL;
}

int main(int argc, char *argv[])
{
	int failed;
	SRunner *runner;

	(void)argc;
	(void)argv;

	runner = srunner_create(NULL);
	srunner_set_tap(runner, "-");
	xmfontlistentry_suite(runner);
	xmfontlist_suite(runner);
	png_suite(runner);

	/**
	 * Given that some things in Motif / Xt rely on static initialization
	 * at runtime, the tests NEED to fork.
	 */
	srunner_set_fork_status(runner, CK_FORK);
	srunner_run_all(runner, CK_SILENT);
	failed = srunner_ntests_failed(runner);
	srunner_free(runner);
	return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}

