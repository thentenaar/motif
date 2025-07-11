/* $TOG: XmeGetDefaultPixel.c /main/5 1997/03/04 17:41:05 dbl $ */
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
#include <Xm/PushB.h>
#include <Xm/XmP.h>

Widget button;
Pixel background, foreground, top_shadow, bottom_shadow, select_color;

void
doit(w, client_data, cbs)
Widget w;
XtPointer client_data, cbs;
{
  int offset=0;
  XrmValue value, get_value;
  char *str_type_return;

  XmeGetDefaultPixel(w, XmFOREGROUND, offset, &value);
  if (*(Pixel*)value.addr != foreground)
    printf("ERROR: foreground pixel value: %d  expected: %d\n",
	   (*(Pixel*)value.addr), foreground);

  XmeGetDefaultPixel(w, XmTOP_SHADOW, offset, &value);
  if (*(Pixel*)value.addr != top_shadow)
    printf("ERROR: top_shadow pixel value: %d  expected: %d\n",
	   (*(Pixel*)value.addr), top_shadow);

  XmeGetDefaultPixel(w, XmBOTTOM_SHADOW, offset, &value);
  if (*(Pixel*)value.addr != bottom_shadow)
    printf("ERROR: bottom_shadow pixel value: %d  expected: %d\n",
	   (*(Pixel*)value.addr), bottom_shadow);

  XmeGetDefaultPixel(w, XmSELECT, offset, &value);
  if (*(Pixel*)value.addr != select_color)
    printf("ERROR:  select pixel value: %d  expected: %d\n",
	   (*(Pixel*)value.addr), select_color);
}

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False;
  register int       n;
  Arg                args[MAX_ARGS];
  XColor             color, unused;
  Colormap           cmap;
  Display            *display;
  Screen             *screen;
  XmString           label;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetArg(args[n], XmNborderWidth, 20);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);

  display = XtDisplay(Shell1);
  screen = XtScreen(Shell1);
  label = XmStringCreateLocalized("Validate");

  /* get color data */
  XtVaGetValues(Shell1, XmNcolormap, &cmap, NULL);
  XAllocNamedColor(display, cmap, "red", &color, &unused);
  background = color.pixel;
  XmGetColors(screen, cmap, background,
	      &foreground, &top_shadow, &bottom_shadow, &select_color);
  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 20);  n++;
  XtSetArg(args[n], XmNbackground, background);  n++;
  XtSetArg(args[n], XmNlabelString, label);  n++;
  button = XmCreatePushButton(Shell1, "button", args, n);
  XtAddCallback(button, XmNactivateCallback, doit, (XtPointer)NULL);

  XtManageChild(button);
  CommonPause();

  XtDestroyWidget(button);
  /* get color data */
  XAllocNamedColor(display, cmap, "black", &color, &unused);
  background = color.pixel;
  XmGetColors(screen, cmap, background,
	      &foreground, &top_shadow, &bottom_shadow, &select_color);
  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 20);  n++;
  XtSetArg(args[n], XmNbackground, background);  n++;
  XtSetArg(args[n], XmNlabelString, label);  n++;
  button = XmCreatePushButton(Shell1, "button", args, n);
  XmStringFree(label);
  XtAddCallback(button, XmNactivateCallback, doit, (XtPointer)NULL);

  XtManageChild(button);
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
