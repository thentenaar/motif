/* $XConsortium: XmeUserMessages.c /main/4 1995/07/15 21:05:31 drk $ */
/*
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
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
/*
 * HISTORY
 */


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = ""
#endif
#endif

#include <testlib.h>
#include <Xm/XmosP.h>
#include <Xm/XmP.h>

Widget Btn1;

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False, test_passed;
  register int       n;
  Arg                args[MAX_ARGS];
  String             dirname;

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  /* The following test checks XmeGetHomeDirName by printing the
     returned value and manually checking it at runtime */
  dirname = XmeGetHomeDirName();
  printf("home directory name: %s\n",dirname);
  CommonPause();

  /* The following test checks XmeWarning by printing a warning 
     message and verifying it at runtime */
  n = 0;
  Btn1 = XmCreateArrowButton(Shell1,"button1", args, n);
  XmeWarning(Btn1,"Sample message text.");
  CommonPause();

  /* The following tests check the results of XmeNamesAreEqual */
  printf("Begin testing XmeNamesAreEqual\n");
  test_passed = True;
  /* test if Xm prefix is ignored */
  if (!XmeNamesAreEqual("Xm1000TH_INCHES","1000th_inches")) {
    printf("ERROR - comparing \"Xm1000TH_INCHES\" and \"1000th_inches\"\n");
    test_passed = False;
  }
  /* test if case is ignored */
  if (!XmeNamesAreEqual("MixedCase","mixedcase")) {
    printf("ERROR - comparing \"MixedCase\" and \"mixedcase\"\n");
    test_passed = False;
  }
  /* test if uppercase fails for test_str */
  if (XmeNamesAreEqual("Capitalized","Capitalized")) {
    printf("ERROR - comparing \"Capitalized\" and \"Capitalized\"\n");
    test_passed = False;
  }
  if (test_passed)
    printf("\tAll tests passed\n");
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Btn1);
}
