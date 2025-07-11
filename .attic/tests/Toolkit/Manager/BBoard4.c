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
static char rcsid[] = "$XConsortium: BBoard4.c /main/7 1995/07/13 18:26:11 drk $"
#endif
#endif




#include <testlib.h>
 
Widget       Bb, Sel,Button, Dial;

 
void act (Widget, XtPointer, XtPointer);
int SelExists;



main(int argc, char *argv[])
{
    Arg          args[10];
    int         n;


    CommonTestInit (argc, argv); 

    n = 0;
    XtSetArg(args[n], XmNwidth, 400); n++;
    XtSetArg(args[n], XmNheight, 400); n++;
    Bb = XmCreateBulletinBoard(Shell1, "Bb", args, n);
    XtManageChild(Bb);
 
    n = 0;
    Sel = XmCreateSelectionBox(Bb, "Sel", args, n);
    XtManageChild(Sel);
    SelExists = 1;
 
    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    Dial = XmCreateFormDialog(Shell1, "Dial", args, n);
    
    n = 0;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNlabelString, XmStringCreateSimple("Destroy")); n++;
    Button = XmCreatePushButton(Dial, "Button", args, n);
    XtAddCallback(Button, XmNactivateCallback, act, NULL);

    XtManageChild(Button);
    XtManageChild(Dial);

    XtRealizeWidget(Shell1);
    XtAppMainLoop(app_context);
}


void act(Widget w,XtPointer client_data, XtPointer call_data)

{
  char *new_button;
  XmString xms;
  Arg args[10];
  int n;

  if (SelExists) 
    {
	printf("Destroying selection box\n");
	XtDestroyWidget(Sel);
	new_button = "Create";
	SelExists = 0;
    }
  else 
    {
	printf("Creating selection box\n");
	n = 0;
	Sel = XmCreateSelectionBox(Bb, "Sel", args, n);
	XtManageChild(Sel);
	new_button = "Destroy";
	SelExists = 1;
    }
    
   xms = XmStringCreateSimple(new_button);
  
   n=0;
   XtSetArg (args[n], XmNlabelString, xms); n++;
   XtSetValues(w, args, n);
   XmStringFree(xms);
}

