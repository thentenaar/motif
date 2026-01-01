/**
 * Motif
 *
 * Copyright (c) 2025 Tim Hentenaar
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
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

	cursor_suite(runner);
	jpeg_suite(runner);
	png_suite(runner);
	svg_suite(runner);
	xmdesktopobject_suite(runner);
	xmfontlistentry_suite(runner);
	xmfontlist_suite(runner);
	xmscreen_suite(runner);

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

