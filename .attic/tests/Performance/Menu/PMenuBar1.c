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
static char rcsid[] = "$XConsortium: PMenuBar1.c /main/8 1995/07/14 12:00:52 drk $"
#endif
#endif
/*  Standard C headers  */

/*	X headers */
#include <X11/Xlib.h>
#include <X11/StringDefs.h>

#include <testlib.h>

/*  Private Functions  */
static void  quitCB();

#define BUFSIZE 128
#define MAXBUTTONS 12

Widget	menubar = NULL;
Widget  BB,
    	menu1 = NULL,
    	submenu1 = NULL,
        submenu2 = NULL,
        pb1 = NULL,
        pb2 = NULL,
    	toggle_RC = NULL,
    	toggle_MS = NULL,
    	toggle_CB = NULL,
	toggle_CBG = NULL;
Widget  Text;

Widget  LabelScale, BorderScale, ChildrenScale;
Widget  BBD, defBtn, quitBtn;
   
Boolean	ownms;   

XmStringCharSet	CharSet = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;

void PrintMe (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

printf ("Accelerator activated this button...\n");

}

void DoItJoe (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{
int border, num_chars, num_children;
char buf[BUFSIZE];
char buf2[32];
static int created = 0;
int n, i;
Arg args[30];
Widget cascadeButtons[MAXBUTTONS];

if (created)
   XtDestroyWidget (menubar);

/* get some info */

    n = 0;
    XtSetArg(args[n], XmNvalue, &border); n++;
    XtGetValues (BorderScale, args, n);

    n = 0;
    XtSetArg(args[n], XmNvalue, &num_chars); n++;
    XtGetValues (LabelScale, args, n);

    n = 0;
    XtSetArg(args[n], XmNvalue, &num_children); n++;
    XtGetValues(ChildrenScale, args, n);
   
    n = 0;
    XtSetArg (args[n], XmNborderWidth, border); n++;
    menubar = XmCreateMenuBar(BB, "menubar", args, n);

/*
 * XmPulldownMenu: submenu1
 */
   
    n = 0; 
    submenu1 = XmCreatePulldownMenu(menubar, "submenu1", args, n);

/*
 * XmPulldownMenu: submenu2
 */
   
    n = 0; 
    submenu2 = XmCreatePulldownMenu(menubar, "submenu2", args, n);



/*
 * XmPushButton: toggle_MS
 */

    n = 0; 

    XtSetArg(args[n], XmNlabelString,
	     XmStringLtoRCreate("Toggle MenuShell", CharSet)); n++;
    XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>M"); n++;
    XtSetArg(args[n], XmNacceleratorText,
             XmStringCreate("Ctrl-M", XmSTRING_DEFAULT_CHARSET)); n++;
    toggle_MS = XmCreatePushButton(submenu1, "toggle_MS", args, n);
    XtAddCallback (toggle_MS, XmNactivateCallback, PrintMe, NULL);

    XtManageChild(toggle_MS);
   
/*
 * XmPushButton: toggle_RC
 */

    n = 0; 
    XtSetArg(args[n], XmNlabelString, 
	     XmStringLtoRCreate("Toggle RowColumn", CharSet)); n++;
    XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>R"); n++;
    XtSetArg(args[n], XmNacceleratorText,
             XmStringCreate("Ctrl-R", XmSTRING_DEFAULT_CHARSET)); n++;
    toggle_RC = XmCreatePushButton(submenu1, "toggle_RC", args, n);

    XtManageChild(toggle_RC);

/*
 * XmPushButton: toggle_CB
 */

    n = 0; 
    toggle_CB = XmCreatePushButton(submenu1, "toggle_CB", args, n);

    XtManageChild(toggle_CB);
   
/*
 * XmPushButton: toggle_CBG
 */

    n = 0; 
    XtSetArg(args[n], XmNlabelString, 
	     XmStringLtoRCreate("Toggle CascadeButtonGadget", CharSet)); n++;
    XtSetArg(args[n], XmNaccelerator, "Ctrl<Key>G"); n++;
    XtSetArg(args[n], XmNacceleratorText,
             XmStringCreate("Ctrl-G", XmSTRING_DEFAULT_CHARSET)); n++;
    toggle_CBG = XmCreatePushButton(submenu1, "toggle_CBG", args, n);


    XtManageChild(toggle_CBG);
/*
 * XmCascadeButton: menu1
 */


/* some children of submenu2 */

n = 0;
pb1 = XmCreatePushButtonGadget (submenu2, "pb1", args, n);
pb2 = XmCreatePushButtonGadget (submenu2, "pb2", args, n);
XtManageChild (pb1);
XtManageChild (pb2);
 
 /***/
 printf ("num_children is %d\n", num_children); 

/* We need several button children, let's try this */
    for (i = 0; i <= (num_children - 1); i++)
      { n = 0;
        for (n = 0; n <= (num_chars - 1); n++)
            buf[n] = '+';
        buf[n] = '\0';
        n = 0;
        XtSetArg(args[n], XmNlabelString, 
	     XmStringCreate (&buf[0], XmSTRING_DEFAULT_CHARSET)); n++;
	if (i%2)
	  {
           XtSetArg(args[n], XmNsubMenuId, submenu1); n++;
	 }
	else
	  {
	   XtSetArg(args[n], XmNsubMenuId, submenu2); n++;
	 }
        sprintf(buf2, "CascadeButton%d", i);
	printf ("I am creating child %d\n", i);
/*	cascadeButtons[i] = XmCreateCascadeButtonGadget(menubar,
							 buf2,
							 args, n);
*/
	cascadeButtons[i] = XmCreateCascadeButton(menubar,
							 buf2,
							 args, n);

        XtManageChild(cascadeButtons[i]);
      }

    XtManageChild(menubar);

    created = 1;

}


void main(argc, argv)
     int	argc;
     char	*argv[];
{
    Arg args[20];
    XmString tcs, tcs1, tcs2;
    int n;
    int border;

    CommonTestInit(argc, argv);
    

    /* create a dialog shell to run things */
    /* a scale for border width, a scale for number of chars in label */
    /* and an okay button to make things go! */

    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    BBD = XmCreateFormDialog (Shell1, "BBD", args, n);

    n = 0;
/*    XtSetArg (args[n], XmNgeometry, "+300+300"); n++; */
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple("Set 'em up, Joe")); n++;
    defBtn = XmCreatePushButtonGadget (BBD, "defBtn", args, n);
    XtManageChild (defBtn);
    XtAddCallback (defBtn, XmNactivateCallback, DoItJoe, NULL);

    n = 0;
    XtSetArg (args[n], XmNdefaultButton, defBtn);
    XtSetValues (BBD, args, n);


    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNtitleString,
	     XmStringCreateSimple("Border Width")); n++;
    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNmaximum, 100); n++;
    XtSetArg(args[n], XmNvalue, 0); n++;
/*    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 50); n++; */
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, defBtn); n++;
    XtSetArg (args[n], XmNtopOffset, 10); n++;
    BorderScale = XmCreateScale (BBD, "BorderScale", args, n);
    XtManageChild (BorderScale);
   
    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNtitleString,
	     XmStringCreateSimple("# Chars in Label")); n++;
    XtSetArg(args[n], XmNminimum, 1); n++;
    XtSetArg(args[n], XmNmaximum, BUFSIZE - 1); n++;
    XtSetArg(args[n], XmNvalue, 1); n++;
/*    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 150); n++; */
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, BorderScale); n++;
    XtSetArg (args[n], XmNtopOffset, 10); n++;
    LabelScale = XmCreateScale (BBD, "LabelScale", args, n);
    XtManageChild (LabelScale);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    XtSetArg(args[n], XmNshowValue, True); n++;
    XtSetArg(args[n], XmNtitleString,
	     XmStringCreateSimple("# Children")); n++;
    XtSetArg(args[n], XmNminimum, 1); n++;
    XtSetArg(args[n], XmNmaximum, MAXBUTTONS - 1); n++;
    XtSetArg(args[n], XmNvalue, 1); n++;
/*    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 200); n++; */
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, LabelScale); n++;
    XtSetArg (args[n], XmNtopOffset, 10); n++;
    ChildrenScale = XmCreateScale (BBD, "ChildrenScale", args, n);
    XtManageChild (ChildrenScale);

    n = 0;
    XtSetArg (args[n], XmNlabelString,
	      XmStringCreateSimple ("Quit")); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, ChildrenScale); n++;
    XtSetArg (args[n], XmNtopOffset, 10); n++;
    quitBtn = XmCreatePushButtonGadget (BBD, "quitBtn", args, n); 
    XtAddCallback (quitBtn, XmNactivateCallback, quitCB, NULL);
    XtManageChild(quitBtn);

    n = 0;
    XtSetArg (args[n], XmNeditable, False); n++;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
    XtSetArg (args[n], XmNvalue, "Use the sliders\nto select the\nnumber of children,\nthe border width,\nand how many\ncharacters in\neach child\'s\nlabel. These will\nbe displayed\nwhen you create\nthe menubar using\nthe Set Up button."); n++;
    XtSetArg (args[n], XmNrows, 13); n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNtopWidget, quitBtn); n++;
    XtSetArg (args[n], XmNtopOffset, 10); n++;
    Text = XmCreateText (BBD, "Text", args, n);
    XtManageChild (Text);

    XtManageChild (BBD);

/*
 * XmMenuBar: menubar
 */
   n = 0;
   XtSetArg (args[n], XmNheight, 400); n++;
   XtSetArg (args[n], XmNwidth, 300); n++;
   BB = XmCreateBulletinBoard (Shell1, "BB", args, n);

/* call the callback */

    XtManageChild (BB);
   XtRealizeWidget(Shell1);
 
    XtAppMainLoop(app_context);
}


static void  quitCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;

{
	printf("Exiting...Please Standby...\n");
	exit(0);
}
