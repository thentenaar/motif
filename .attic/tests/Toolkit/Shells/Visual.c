/* $XConsortium: Visual.c /main/5 1995/07/15 21:09:39 drk $ */
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

/* Test created to check the setting of the resource XmNvisual in the Shell
   widget
*/

#include <testlib.h>
#include <string.h>

void
doit()
{
    Screen * screen;
    XrmValue args[10];
    Cardinal num_args = 0;
    Boolean result;
    static char string[100];
    static char * visual_class[] = { "StaticGray",
			      "StaticColor",
			      "TrueColor",
			      "GrayScale",
			      "PseudoColor",
			      "DirectColor" };
    int visual_found[6];
    int i = 0;
    static Visual * visual[6];
    Display * display;
    XrmValue fromVal;
    XrmValue toVal;
    Arg shell_args[10];
    Cardinal num_shell_args = 0;
    Visual * ret_visual;
    int depth;
    int size;
    char buf[512];
    char tmp[256];

    screen = XtScreen(Shell1);
    display = DisplayOfScreen(screen);
    depth = DefaultDepthOfScreen(screen);
    args[0].addr = (XPointer) &screen;
    args[0].size = sizeof(Screen *);
    num_args++;
    args[1].addr = (XPointer) &depth;
    args[1].size = sizeof(int);
    num_args++;
    sprintf(buf, 
	    "\nVisual Class\tVisual Found\n------------\t------------\n\n");
    for (i = 0; i < 6; i++) {
	strcpy(string, XrmQuarkToString(XrmStringToQuark(visual_class[i])));
	size = strlen((char *)string);
	fromVal.addr = (XPointer)string;
	fromVal.size = sizeof(size);
	toVal.addr = (XPointer) &visual[i];
	toVal.size = sizeof(Visual);
	result = XtCallConverter(display, XtCvtStringToVisual, args, num_args,
				 &fromVal, &toVal, NULL);
	if (result == False) {
	    sprintf(tmp, "%s\tNo\n", visual_class[i]);
	    strcat(buf, tmp);
	    visual_found[i] = False;
	}
	else {
	    sprintf(tmp, "%s\tYes\n", visual_class[i]);
	    strcat(buf, tmp);
	    visual_found[i] = True;
	}
    }
    printf("%s\n", buf);
    for (i = 0; i < 6 ; i++) {
	if (!visual_found[i])
	    continue;
	num_shell_args = 0;
	printf("For visual class %s...\n\n", visual_class[i]);
	XtSetArg(shell_args[0], XmNvisual, &ret_visual); num_shell_args++;
	XtGetValues(Shell1, shell_args, num_shell_args);
	printf("original visual: %x\n", ret_visual);
	printf("replacement visual: %x\n", visual[i]);
	num_shell_args = 0;
	XtSetArg(shell_args[0], XmNvisual, visual[i]); num_shell_args++;
	XtSetValues(Shell1, shell_args, num_shell_args);
	num_shell_args = 0;
	XtSetArg(shell_args[0], XmNvisual, &ret_visual); num_shell_args++;
	XtGetValues(Shell1, shell_args, num_shell_args);
	if (visual[i] != ret_visual) {
	    printf("Error. Incorrect visual value returned. Visual class: %s\n\n",
		   visual_class[i]);
	}
	else {
	    printf("Visual successfully modified\n\n");
	}
    }

  }

void
main (int argc, char **argv)
{

  Arg args[10];
  int n;

  CommonTestInit (argc,argv);

  n=0;
  XtSetArg (args[n], XmNiconName, "TESTING"); n++;
  XtSetArg (args[n], XmNwidth,100); n++;
  XtSetArg (args[n], XmNheight,100); n++;
  XtSetValues (Shell1, args, n);

  XtRealizeWidget (Shell1);

  CommonPause();
  
  doit();

  CommonPause();
  CommonPause();

  XtAppMainLoop (app_context);

}







