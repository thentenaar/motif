/* $XConsortium: DNDMenu1.c /main/5 1995/07/15 21:00:10 drk $ */

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

#include <gnu.bmp>

#define XBMgnu_BITS        gnu_bits
#define XBMgnu_WIDTH       gnu_width
#define XBMgnu_HEIGHT      gnu_height

#include <testlib.h>

void
PostIt(Widget w, XtPointer p, XEvent *ev, Boolean *c);

void 
main (int argc, char **argv)
{

    Widget RC1;
    Widget Frame[3];
    Widget MenuBar1, Pulldown1, Pulldown2, Option1, Cascade1, Popup1, Label1;
    Widget OptionPB1, OptionPB2, PopupPB1, PulldownPB1;
    Widget Text1, TopLevel1;
    Arg args[10];
    int n,i;
    XmString LabelString;
    char name[32];

    CommonTestInit(argc, argv);

  
    n = 0;
    RC1 = XmCreateRowColumn( Shell1, "RC1", args, n );
    XtManageChild(RC1);

    for (i=0; i<3; i++)
      {
	sprintf (name, "Frame%d", i+1);
	Frame[i] = XmCreateFrame (RC1,name,args,n);
	XtManageChild (Frame[i]);
      }

    

    /* 
     * Create a tear-off Pulldown menu. This will also enable testing dragging
     * from a MenuBar.
     */

    n=0;
    MenuBar1 = XmCreateMenuBar (Frame[0], "MenuBar1",args,n);
    XtManageChild (MenuBar1);

    n=0;
    XtSetArg (args[n], XmNtearOffModel, XmTEAR_OFF_ENABLED); n++;
    Pulldown1 =XmCreatePulldownMenu(Frame[1],"Pulldown1",args, n);


    n=0;
    LabelString = XmStringCreate ("MenuBar", XmFONTLIST_DEFAULT_TAG);
    XtSetArg(args[n], XmNsubMenuId,Pulldown1); n++;
    XtSetArg (args[n], XmNlabelString, LabelString); n++;
    Cascade1 =XmCreateCascadeButton(MenuBar1, "Cascade1", args, n);
    XtManageChild (Cascade1);

    XmStringFree(LabelString);



    LabelString = XmStringCreate ("PulldownMenu", XmFONTLIST_DEFAULT_TAG);
    n=0;
    XtSetArg (args[n], XmNlabelString, LabelString); n++;
    PulldownPB1 = XmCreatePushButton (Pulldown1,"PulldownPB1", args, n);
    XtManageChild (PulldownPB1);
    XmStringFree (LabelString);

    
    


    /* 
     * Create an option menu 
     */

    n=0;
    Pulldown2 =XmCreatePulldownMenu(Frame[1],"Pulldown2",args, n);

    n=0;
    XtSetArg(args[n], XmNsubMenuId,Pulldown2); n++;
    Option1 =XmCreateOptionMenu(Frame[1],"Option1", args, n);
    XtManageChild (Option1);


    LabelString = XmStringCreate ("OptionItem1", XmFONTLIST_DEFAULT_TAG);
    n=0;
    XtSetArg (args[n], XmNlabelString, LabelString); n++;
    OptionPB1 = XmCreatePushButton (Pulldown2,"OptionPB1", args, n);
    XtManageChild (OptionPB1);
    XmStringFree (LabelString);

    LabelString = XmStringCreate ("OptionItem2", XmFONTLIST_DEFAULT_TAG);
    n=0;
    XtSetArg (args[n], XmNlabelString, LabelString); n++;
    OptionPB2 = XmCreatePushButton (Pulldown2,"OptionPB2", args, n);
    XtManageChild (OptionPB2);
    XmStringFree (LabelString);

    
    
    /* 
     * Create a popup menu 
     */


    n = 0; 
    LabelString = XmStringCreate("Popup Menu on MB3", XmFONTLIST_DEFAULT_TAG);
    XtSetArg(args[n], XmNlabelString, LabelString); n++;
    Label1 = XmCreateLabel(Frame[2], "Label1", args, n);
    XtManageChild(Label1);
    XmStringFree(LabelString);


    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNmenuPost, "<Btn3Down>"); n++; 
    XtSetArg (args[n], XmNtearOffModel, XmTEAR_OFF_ENABLED); n++;
    Popup1 = XmCreatePopupMenu(Label1, "popup", args, n);
    XtAddEventHandler(Label1, ButtonPressMask, False, PostIt, (XtPointer)Popup1);

    LabelString = XmStringCreate ("PopupMenu", XmFONTLIST_DEFAULT_TAG);
    n=0;
    XtSetArg (args[n], XmNlabelString, LabelString); n++;
    PopupPB1 = XmCreatePushButton (Popup1,"PopupPB1", args, n);
    XtManageChild (PopupPB1);

    
    
    /* 
     * Create a Text widget for dropping into 
     */

    n=0;   
    TopLevel1 = XtCreatePopupShell("TopLevel1", topLevelShellWidgetClass, 
				 Shell1,args,n);


    n=0;
    XtSetArg (args[n], XmNwidth, 200); n++;
    XtSetArg (args[n], XmNheight, 100); n++;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    Text1 = XmCreateText (TopLevel1, "Text1", args, n);
    XtManageChild (Text1);

  
    XtPopup (TopLevel1,XtGrabNone);


    XtRealizeWidget(Shell1);

    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    XtAppMainLoop (app_context);
  }



void
PostIt(Widget w, XtPointer p, XEvent *event, Boolean *c)
{
  Widget popup = (Widget) p;
  if(event->xbutton.button != Button3)
    {return; }
  XmMenuPosition(popup, (XButtonPressedEvent *)event);
  XtManageChild(popup);
}







