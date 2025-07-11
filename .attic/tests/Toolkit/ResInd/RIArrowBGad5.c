/* $XConsortium: RIArrowBGad5.c /main/4 1995/07/15 21:08:17 drk $ */
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
 *  Create a drawing area with an arrow gadget within it.
 *  Initialize it with some invalid data.
 *  SetValues for the unit type, and x and y.
 */

#include <signal.h>
#include <X11/StringDefs.h>
#include <testlib.h>


Boolean	show_geom_bug;				/*PIR4250*/

void Destroy();

void main (argc, argv)
unsigned int argc;
char **argv;

{
  Widget drawarea;
  Widget arrow;
  Arg args[10];
  int n;

  show_geom_bug = False;				/*PIR4250*/

  CommonTestInit(argc, argv);

  if (UserData != NULL)
    ProcessCommandArgs();
    
  n = 0;
  XtSetArg (args[n], XmNwidth, 400);	n++;
  XtSetArg (args[n], XmNheight, 400);	n++;
  XtSetArg (args[n], XmNallowShellResize, False);	n++; 
  XtSetValues (Shell1, args, n);

  if (show_geom_bug == True)				/*PIR4250*/
    XtRealizeWidget (Shell1);

  /*  The drawing area creation.  Contained in an outer frame.  */
  n = 0;
  XtSetArg (args[n], XmNwidth, 400);	n++;
  XtSetArg (args[n], XmNheight, 400);	n++;
  drawarea = XmCreateDrawingArea (Shell1, "drawarea", args, n);

  if (show_geom_bug == True)				/*PIR4250*/
    XtRealizeWidget (drawarea);
  XtManageChild (drawarea);

  /*  Create the arrow */

  n = 0;
  XtSetArg (args[n], XmNx, 40);	n++;
  XtSetArg (args[n], XmNy, 40);	n++;
  XtSetArg (args[n], XmNunitType, 18);			n++;
  arrow = XmCreateArrowButtonGadget (drawarea, "arrow", args, n);

  if (show_geom_bug == True)				/*PIR4250*/
    XtRealizeWidget (arrow);
  XtManageChild (arrow);
  XtAddCallback (arrow, XmNactivateCallback, Destroy, NULL);

  n = 0;
  XtSetArg (args[n], XmNx, 100);			n++;
  XtSetArg (args[n], XmNy, 250);			n++;
  XtSetArg (args[n], XmNunitType, Xm1000TH_INCHES);	n++;
  XtSetValues (arrow, args, n);

  XtRealizeWidget (Shell1);
  CommonPause();

  XtAppMainLoop(app_context);
  XtDestroyWidget(drawarea);
}

void Destroy (w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;

{
   XtDestroyWidget (w);
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

		if (strcmp(next_word, "-show_geom_bug") == 0) {

			show_geom_bug = True;
			user_data += (strlen(next_word) + num_spaces);
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
