/* $XConsortium: i18ninput.c /main/5 1995/07/15 20:45:37 drk $ */
/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
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
/* 
 * HISTORY
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

static XtAppContext app_context;

static String fallback_reslist[] = {
  "*font.label.labelString:	Select font",
  "*font*button1.labelString:	Small",
  "*font*button1.renderTable:	*medium*-r-*--14*:",
  "*font*button2.labelString:	Medium",
  "*font*button2.renderTable:	*medium*-r-*--18*:",
  "*font*button3.labelString:	Large",
  "*font*button3.renderTable:	*medium*-r-*--24*:",
  "*color.label.labelString:	Select color",
  "*color*button1.labelString:	Red",
  "*color*button1.foreground:	#fe5151",
  "*color*button2.labelString:	Green",
  "*color*button2.foreground:	#00be51",
  "*color*button3.labelString:	Blue",
  "*color*button3.foreground:	#2aa1fd",
  "*XmText.renderTable:		*medium*-r-*--18*:",
  "*XmTextField.renderTable:	*medium*-r-*--18*:",
  NULL
  };

extern int dialog_init(int *argc, char **argv, Display *dpy);

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
  Display *display;

  /* Initialize the X Intrinsics */
  XtToolkitInitialize();

  /* Set up language environment for X */
  XtSetLanguageProc(NULL, (XtLanguageProc)NULL, NULL);

  /* Create application context */
  app_context = XtCreateApplicationContext();
  XtAppSetFallbackResources(app_context, fallback_reslist);

  /* open up one display */
  display = XtOpenDisplay(app_context,
                          NULL,
                          "XmdI18nInput", "XmdI18nInput",
                          (XrmOptionDescRec *)NULL, 0,
                          &argc, argv);
  if (!display) {
    (void) fprintf(stderr, "Unable to open display\n");
    exit(0);
  }

  /* Call init functions. This creates the user interface 
     (placed in input.c) */
  if (dialog_init(&argc, argv, display) != 0)
    exit(1);

  XtAppMainLoop(app_context);

  return 0;    /* make compiler happy */
}

