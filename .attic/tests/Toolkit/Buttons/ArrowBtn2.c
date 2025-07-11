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
static char rcsid[] = "$XConsortium: ArrowBtn2.c /main/9 1995/07/13 17:28:29 drk $"
#endif
#endif


/****************************************
*   Automatic test program generation   *
*              Version 1.2              *
*       Tue Apr  4 15:41:13 1989        *
****************************************/

#include <testlib.h>

Widget ArrowButton2, ArrowButton1;
Widget parent1, parent2;

void  main (argc, argv)
int argc;
char **argv;
{
  Boolean            trace = False;
  register int       n;
  Arg                args[MAX_ARGS];
  XmString           tcs;

  CommonTestInit(argc, argv);
    
  n = 0;
  XtSetArg(args[n], XmNwidth,  400);  n++;
  XtSetArg(args[n], XmNheight, 300);  n++;
  XtSetArg(args[n], XtNgeometry, "+0+0");  n++;
  XtSetArg(args[n],  XmNkeyboardFocusPolicy, XmPOINTER);  n++;
  XtSetValues(Shell1, args, n);
  
  XtRealizeWidget(Shell1);

  n = 0;
  ArrowButton2 = XmCreateArrowButton(Shell1, "ArrowButton2", args, n);
  XtManageChild(ArrowButton2);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
  XtSetArg(args[n], XmNhighlightThickness, 5);  n++;
  XtSetArg(args[n], XmNhighlightColor, CommonGetColor("red"));  n++;
  XtSetArg(args[n], XmNforeground, CommonGetColor("red"));  n++;
  XtSetValues(ArrowButton2, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNhighlightThickness, 10);  n++;
  XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_LEFT);  n++;
  XtSetValues(ArrowButton2, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNshadowThickness, 4);  n++;
  XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_RIGHT);  n++;
  XtSetArg(args[n], XmNtopShadowColor, CommonGetColor("pink"));  n++;
  XtSetValues(ArrowButton2, args, n);

  CommonPause();

  n = 0;
  XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN);  n++;
  XtSetArg(args[n], XmNhighlightOnEnter, TRUE);  n++;
  XtSetArg(args[n], XmNbottomShadowColor, CommonGetColor("green"));  n++;
  XtSetValues(ArrowButton2, args, n);

  CommonPause();

  XtDestroyWidget (ArrowButton2);

  n = 0;
  XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
  parent1 = XmCreateRowColumn (Shell1, "parent1", args, n);
  
  n = 0;
  ArrowButton1 = XmCreateArrowButton (parent1, "ArrowButton1", args, n);
  XtAddCallback (ArrowButton1, XmNactivateCallback, CommonGenericCB, NULL);

  n = 0;
  parent2 = XmCreateBulletinBoard (parent1, "parent2", args, n);
 
  n = 0;
  XtSetArg (args[n], XmNwidth, 200); n++;
  ArrowButton2 = XmCreateArrowButton (parent2, "ArrowButton2", args, n);
  XtAddCallback (ArrowButton2, XmNactivateCallback, CommonGenericCB, NULL);

  XtManageChild (ArrowButton1);
  XtManageChild (ArrowButton2);
  XtManageChild (parent2);
  XtManageChild (parent1);

  CommonPause();

  XtSetSensitive (ArrowButton1, False);
  XtSetSensitive (parent2, False);

  CommonPause();

  XtAppMainLoop(app_context);
  XtDestroyWidget(ArrowButton2);
}















char * concat_args(n, words)
int n;
char *words[];
{
	char *buffer;
	int i, len = 0;

	if (n <= 1)
		return("");

	for (i = 1; i < n; i++)
		len += strlen(words[i]);

	len += (n - 1);

	buffer = (char *) malloc(len + 1);
	if (buffer == NULL) 
		{
		fprintf(stderr, "Out of Memory in concat_args()\n");
		exit(1);
		}
	buffer[0] = '\0';

	for (i = 1; i < n; i++)
		{
		if (i > 1)
			strcat(buffer," ");
		strcat(buffer, words[i]);
		}
	return(buffer);
}

void quit(w, client_data, event)
Widget 	w;
caddr_t	client_data;
XEvent	*event;
{
exit(0);
}
	
void myQuit()
{
	printf("Completed\n");
	exit(0);
}









