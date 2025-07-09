/* $XConsortium: dainput.c /main/5 1995/07/15 20:47:20 drk $ */
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
 * 
 */
/*
 * HISTORY
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <locale.h>

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>          /* to get _NO_PROTO */

#include "dainput.h"


/****************************************************************
 * base:
 ****************************************************************/
static char *base(char *str)
{
  static char basename[300];
  char *p;

  if ((p = strrchr(str, '/')))
    (void) strcpy(basename, p+1);
  else
    (void) strcpy(basename, str);
  if ((p = strrchr(basename, '.')))
    *p ='\0';
  return basename;
}


/**************************************************************
 * main:
 **************************************************************/
int main(int argc, char **argv)
{
  XtAppContext  app_context;
  Display     * display;
  Widget        top_level;
  char        * aclass = "XmdDaInput";
  char          name[512];
  char       ** temp_argv = argv;


  (void) strncpy(name, base(argv[0]), sizeof(name) - 1);

  /* handle the '-name' option */
  while (*temp_argv) {
    if (strcmp(*temp_argv, "-name") == 0) {
      (void) strncpy(name, *++temp_argv, sizeof(name) - 1);
      break;
    }
    temp_argv++;
  }

  /* Initialize the X Intrinsics */
  XtToolkitInitialize();

  /* Set up language environment */
  XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);

  /* create application context */
  app_context = XtCreateApplicationContext();

  /* open up one display */
  display = XtOpenDisplay(app_context,
                          NULL,
                          name, aclass,
                          (XrmOptionDescRec *)NULL, 0,
                          &argc, argv);
  if (!display) {
    (void) fprintf(stderr, "Unable to open display\n");
    exit(0);
  }

  /* Create the user interface */
  top_level = DaCreateGui(name, aclass, argc, argv, display);
  if (top_level == NULL) {
    (void) fprintf(stderr, "Unable to create user interface\n");
    exit(1);
  }

  /* Realize and start event dispatching */
  XtRealizeWidget(top_level);
  XtAppMainLoop(app_context);

  return 0;    /* make compiler happy */
}

