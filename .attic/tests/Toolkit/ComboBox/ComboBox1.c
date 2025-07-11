/* $XConsortium: ComboBox1.c /main/6 1996/07/26 10:56:31 jakku $ */
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





/* This test takes all its resources from the resource file ComboBox.defaults
   in the ./tests/Toolkit/ComboBox directory. The test should be passed
   a flag (a number from 1-3), which will determine the XtName of the
   ComboBox widget created. This name then determines what resources
   are set for the ComboBox. The resource file contains 3 set of resources
   for this test */


#include <testlib.h>
#include <Xm/ComboBox.h>


#define match(a,b) strcmp(a,b)==0

void
main (int argc, char **argv)
{
	Widget Parent;
	Widget CB;
	int i,a;
	char buf[3];

	CommonTestInit(argc,argv);

	/* User specifies an integer from 1-3  which determines the Xt name of
	   the Notebook widget. */

	if (UserData == NULL)
	  {
	    fprintf (stderr, "Usage : ComboBox1 -u <1-3>\n");
	    exit(1);
	  }

	if (match (UserData,"1")) 
	  a=1;
	else if (match (UserData,"2")) 
	  a=2;
	else if (match (UserData,"3")) 
	  a=3;
	else if (match (UserData, "4"))
	  a=4;

	Parent = XmCreateForm(Shell1, "Parent", NULL, 0);
	XtManageChild (Parent);

	/* determine Xt Name of ComboBox and create ComboBox */

	sprintf (buf, "CB%d", a);
	CB  = XmCreateComboBox (Parent, buf, NULL, 0);
	XtManageChild (CB);

	XtRealizeWidget (Shell1);

	CommonPause();

	XtAppMainLoop (app_context);
      }







