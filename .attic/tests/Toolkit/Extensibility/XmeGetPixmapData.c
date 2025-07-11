/* $TOG: XmeGetPixmapData.c /main/5 1998/09/24 13:20:52 samborn $ */
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
#include <Xm/LabelG.h>
#include <Xm/RowColumn.h>
#include <Xm/XmosP.h>
#include <Xm/XmP.h>

Widget rowcol;
Pixmap pixmap;
Pixel fg, bg;

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False;
  register int       n;
  Arg                args[MAX_ARGS];
  String             dirname;
  char               *image_name;
  int                depth;
  Pixel              foreground, background;
  int                hot_x = -1, hot_y = -1;
  unsigned int       width, height;

  CommonTestInit(argc, argv);

  n = 0;
  XtSetArg(args[n], XmNwidth,  200);  n++;
  XtSetArg(args[n], XmNheight, 200);  n++;
  XtSetValues(Shell1, args, n);

  XtRealizeWidget(Shell1);
  rowcol = XtVaCreateWidget("rowcol",xmRowColumnWidgetClass, Shell1,
			    XmNpacking, XmPACK_COLUMN,
			    NULL);

  /* get the values for the foreground and background colors */
  XtVaGetValues(rowcol,
		XmNforeground, &fg,
		XmNbackground, &bg,
		NULL);

  pixmap = XmGetPixmap(XtScreen(Shell1),"../bitmaps/xlogo64.xbm",fg,bg);
  if (pixmap == XmUNSPECIFIED_PIXMAP) {
    printf("ERROR - couldn't load image - xlogo64.xbm\n");
    exit(1);
  } else
    XtVaCreateManagedWidget("xlogo64", xmLabelGadgetClass, rowcol,
			    XmNlabelType, XmPIXMAP,
			    XmNlabelPixmap, pixmap,
			    NULL);
  XtManageChild(rowcol);

  /* call the function */
  if (!XmeGetPixmapData(XtScreen(Shell1), pixmap,
			&image_name, &depth, &foreground,
			&background, &hot_x, &hot_y,
			&width, &height))
      printf("ERROR: image not in the image cache\n");

  /* Verify the data returned */
  if (strcmp(image_name, "../bitmaps/xlogo64.xbm"))
    printf("ERROR - image name: %s\n",image_name);
  if (depth != 8)
    printf("ERROR - incorrect depth: %d\n",depth);
  if (hot_x != -1)
    printf("ERROR - incorrect hot_x: %d\n",hot_x);
  if (hot_y != -1)
    printf("ERROR - incorrect hot_y: %d\n",hot_y);
  if (width != 64)
    printf("ERROR - incorrect depth: %d\n",width);
  if (height != 64)
    printf("ERROR - incorrect depth: %d\n",height);
  if (foreground != fg)
    printf("ERROR - foreground color doesn't match\n");
  if (background != bg)
    printf("ERROR - background color doesn't match\n");
  CommonPause();

  CommonPause();	/* quit */
  XtAppMainLoop(app_context);
  XtDestroyWidget(Shell1);
}
