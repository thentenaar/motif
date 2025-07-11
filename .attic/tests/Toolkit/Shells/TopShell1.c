/* $XConsortium: TopShell1.c /main/5 1995/07/15 21:09:34 drk $ */
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


#include <testlib.h>

int c = 0;


void
doit()
{

  Arg args[10];
  int n;

  switch(c++)
  {
    case 0:
        n=0;
        XtSetArg (args[n], XmNiconName, NULL); n++;
	XtSetValues (Shell1,args,n);
	printf("setting NULL (NULL)\n");
	break;
    case 1:
        n=0;
        XtSetArg (args[n], XmNiconName, ""); n++;
	XtSetValues (Shell1,args,n);
	printf("setting empty string ()\n");
	break;
    case 2:
        n=0;
        XtSetArg (args[n], XmNiconName, " "); n++;
	XtSetValues (Shell1,args,n);
	printf("setting space ( )\n");
	break;
    defaults: exit(0);
  }
}

void
main (int argc, char *argv[])
{

  Arg args[10];
  int n;

  CommonTestInit (argc,argv);

  CommonPause();

  n=0;
  XtSetArg (args[n], XmNiconName, "TESTING"); n++;
  XtSetArg (args[n], XmNinitialState,IconicState); n++;
  XtSetArg (args[n], XmNwidth,100); n++;
  XtSetArg (args[n], XmNheight,100); n++;
  XtSetValues (Shell1, args, n);

  XtRealizeWidget (Shell1);

  CommonPause();
  
  doit();

  CommonPause();

  doit();

  CommonPause();

  doit();

  CommonPause();
  CommonPause();
  

  XtAppMainLoop (app_context);

}








