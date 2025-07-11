/* $XConsortium: ToggleBtn3.c /main/5 1995/07/15 21:02:42 drk $ */
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
/*
 *	File: ToggleBtn3.c
 *
 *	Program to compare between 1.1 and 1.2 various togglebuttons within 
 *	different managers.
 *
 */

#include <testlib.h>
#include <X11/Xresource.h>
#include <X11/StringDefs.h>
#include "smiley.bmp"

#define XBMs_BITS        smiley_bits
#define XBMs_WIDTH       smiley_width
#define XBMs_HEIGHT      smiley_height
Pixmap px_select;

#include "circles1.bmp"
#define XBMu_BITS        circles1_bits
#define XBMu_WIDTH       circles1_width
#define XBMu_HEIGHT      circles1_height


Pixmap px_unselect;

enum { BULLETINBOARD, FORM, PANEDWINDOW, ROWCOLUMN };

unsigned char   manager_needed;

Widget	Manager1;

Widget	ToggleButton1, 
		ToggleButton2, 
		ToggleButton3, 
		ToggleButton4,
		ToggleButton5,
		ToggleButton6;

Widget	ToggleBGad1, 
		ToggleBGad2, 
		ToggleBGad3, 
		ToggleBGad4,
		ToggleBGad5,
		ToggleBGad6;


void
main(unsigned int argc, char **argv)
{

	Arg			args[MAX_ARGS];
	Cardinal	n;
	XmFontList fontlist;
	

	manager_needed = BULLETINBOARD;

	CommonTestInit(argc, argv);

	if (UserData != NULL)
		ProcessCommandArgs();

	fontlist = CommonGetFontList("9x15");

	n = 0;
    if (manager_needed == BULLETINBOARD)
        Manager1 = XmCreateBulletinBoard(Shell1, "Manager1", args, n);
    else if (manager_needed == FORM)
        Manager1 = XmCreateForm(Shell1, "Manager1", args, n);
    else if (manager_needed == PANEDWINDOW)
        Manager1 = XmCreatePanedWindow(Shell1, "Manager1", args, n);
    else if (manager_needed == ROWCOLUMN) {
	
		XtSetArg(args[n], XmNcolumns, 1); n++;
		XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
        Manager1 = XmCreateRowColumn(Shell1, "Manager1", args, n);

	}

	XtManageChild(Manager1);

	n = 0;
	XtSetArg(args[n], XmNindicatorOn, True);  n++;
	XtSetArg(args[n], XmNfontList, fontlist); n++;
	if (manager_needed == FORM) {

		XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

	}
	ToggleButton1 = XmCreateToggleButton(Manager1, "ToggleButton1", args, n);
	XtManageChild(ToggleButton1);

	n = 0;
	XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY);  n++;
	XtSetArg(args[n], XmNspacing, 10);  n++;
	XtSetArg(args[n], XmNfillOnSelect, False);  n++;
	if (manager_needed == FORM) {

		XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
		XtSetArg(args[n], XmNtopWidget, ToggleButton1); n++;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

	}
	if (manager_needed == BULLETINBOARD) {
		XtSetArg(args[n], XmNy, 50); n++;
	}
	ToggleButton2 = XmCreateToggleButton(Manager1, "ToggleButton2", args, n);
	XtManageChild(ToggleButton2);

	n = 0;
	XtSetArg(args[n], XmNvisibleWhenOff, False);  n++;
	if (manager_needed == FORM) {

		XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
		XtSetArg(args[n], XmNtopWidget, ToggleButton2); n++;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

	}
	if (manager_needed == BULLETINBOARD) {
		XtSetArg(args[n], XmNy, 100); n++;
	}
	ToggleButton3 = XmCreateToggleButton(Manager1, "ToggleButton3", args, n);
	XtManageChild(ToggleButton3);

	n = 0;
	XtSetArg(args[n], XmNset, True);  n++;
	XtSetArg(args[n], XmNfillOnSelect, True);  n++;
	if (manager_needed == FORM) {

		XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
		XtSetArg(args[n], XmNtopWidget, ToggleButton3); n++;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

	}
	if (manager_needed == BULLETINBOARD) {
		XtSetArg(args[n], XmNy, 150); n++;
	}
	ToggleButton4 = XmCreateToggleButton(Manager1, "ToggleButton4", args, n);
	XtManageChild(ToggleButton4);

	px_unselect = XCreatePixmapFromBitmapData(display,
				  DefaultRootWindow(display),
				  XBMu_BITS, 
				  XBMu_WIDTH,XBMu_HEIGHT,
				  WhitePixel(display,DefaultScreen(display)),
				  BlackPixel(display,DefaultScreen(display)),
				  DefaultDepth(display,DefaultScreen(display)));
	px_select = XCreatePixmapFromBitmapData(display, 
				DefaultRootWindow(display),
				XBMs_BITS, XBMs_WIDTH,XBMs_HEIGHT,
				WhitePixel(display,DefaultScreen(display)),
				BlackPixel(display,DefaultScreen(display)),
				DefaultDepth(display,DefaultScreen(display)));

	n = 0;
	XtSetArg(args[n], XmNlabelType, XmPIXMAP);  n++;
	XtSetArg(args[n], XmNselectPixmap, px_select);  n++;
	XtSetArg(args[n], XmNlabelPixmap, px_unselect);  n++;
	XtSetArg(args[n], XmNindicatorOn, False); n++;
	if (manager_needed == FORM) {

		XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
		XtSetArg(args[n], XmNtopWidget, ToggleButton4); n++;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;

	}
	if (manager_needed == BULLETINBOARD) {
		XtSetArg(args[n], XmNy, 200); n++;
	}
	ToggleButton5 = XmCreateToggleButton(Manager1, "ToggleButton5", args, n);
	XtManageChild(ToggleButton5);
	
	n = 0;
	XtSetArg(args[n], XmNindicatorOn, False);  n++;
	XtSetArg(args[n], XmNshadowThickness, 4); n++;
	if (manager_needed == FORM) {

		XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
		XtSetArg(args[n], XmNtopWidget, ToggleButton5); n++;
		XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
		XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;

	}
	if (manager_needed == BULLETINBOARD) {
		XtSetArg(args[n], XmNy, 300); n++;
	}
	ToggleButton6 = XmCreateToggleButton(Manager1, "ToggleButton6", args, n);
	XtManageChild(ToggleButton6);

	XtRealizeWidget(Shell1);

  	for (n = 0; n < 5; n++)
		CommonPause();                  

	XmToggleButtonSetState(ToggleButton5, TRUE, FALSE);

	CommonPause();

	XmToggleButtonSetState(ToggleButton5, FALSE, FALSE);

	CommonPause();

	n = 0;
	XtSetArg (args[n], XmNbackgroundPixmap, px_select);	n++;
	XtSetValues (ToggleButton5, args, n);

	CommonPause();

	n = 0;
	XtSetArg (args[n], XmNbackgroundPixmap, XmUNSPECIFIED_PIXMAP);   n++;
	XtSetValues (ToggleButton5, args, n);

	CommonPause();

											/* Begin PIR3850 */
	n = 0;
	XtSetArg (args[n], XmNbackgroundPixmap, None); n++;
	XtSetValues (Manager1, args, n);

	CommonPause();
											/* End PIR3850 */

	XtDestroyWidget(Manager1);

	CommonPause();

	XtAppMainLoop(app_context);

}


#define	WORD_LEN	32

ProcessCommandArgs()
{

	char	*user_data;
	char	next_word[WORD_LEN + 1];
	int     num_spaces;

	user_data = UserData;

	while (*user_data != '\0') {

		get_next_word(user_data, next_word, &num_spaces);
		user_data += (strlen(next_word) + num_spaces);

		if (strcmp(next_word, "-bb") == 0) {
			manager_needed = BULLETINBOARD;
			continue;

		}

		if (strcmp(next_word, "-fo") == 0) {
			manager_needed = FORM;
			continue;

		}

		if (strcmp(next_word, "-pw") == 0) {
			manager_needed = PANEDWINDOW;
			continue;

		}

		if (strcmp(next_word, "-rc") == 0) {
			manager_needed = ROWCOLUMN;
			continue;

		}

	}

	free(UserData);

}


get_next_word(source, dest, spaces)
char	*source;
char	*dest;
int		*spaces;
{

	int	n;
	int	space_count;

	space_count = 0;

	while (isspace(*source)) {
		source++;
		space_count++;
	}

	n = 0;
	while (!isspace(*source) && *source != '\0' && n < WORD_LEN) {
		*dest++ = *source++;
		n++;
	}
	*dest = '\0';
	*spaces = space_count;

}
