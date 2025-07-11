/* $XConsortium: SpinBox1.c /main/5 1995/07/15 21:09:44 drk $ */
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


#include "testlib.h"



#define match(a,b) strcmp(a,b)==0
#define NUM_ITEM 5

void
main(int argc, char **argv)
{

  Arg args[10];
  int n,a,i;
  char buf[16];

  Widget TextF1,TextF2;
  Widget Parent;
  Widget SpinBox;

		     
  

  CommonTestInit(argc,argv);

   if (UserData == NULL)
     a=1;
	
    else
      {
    if (match (UserData,"1")) 
	  a=1;
	else if (match (UserData,"2")) 
	  a=2;
	else if (match (UserData,"3")) 
	  a=3;
	else if (match (UserData,"4")) 
	  a=4;
  }

	Parent = XmCreateFrame(Shell1, "Parent", NULL, 0);
	XtManageChild (Parent);


        sprintf (buf, "SpinBox%d", a);
	SpinBox  = XmCreateSpinBox(Parent, buf, NULL,0);
        XtManageChild (SpinBox);

        
        TextF1 = XmCreateTextField (SpinBox, "TextF1", NULL, 0);
        XtManageChild (TextF1);

        TextF2 = XmCreateTextField (SpinBox, "TextF2", NULL, 0);
        XtManageChild (TextF2);


        XtRealizeWidget (Shell1);

        CommonPause();
        CommonPause();
	XtAppMainLoop (app_context);
}
