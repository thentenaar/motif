/* $XConsortium: RIDrawArea1.c /main/5 1995/07/15 21:08:40 drk $ */
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
 * Original History:
 *
 * Revision 1.7.4.4  1993/05/20  15:58:47  msmith
 * 	Cleaned up memory leaks due to XmString's being created but
 * 	not freed.  Added calls to XmStringFree() where necessary.
 * 	[1993/05/20  15:58:17  msmith]
 *
 * Revision 1.7.4.3  1993/04/08  17:28:15  yak
 * 	Replaced #ifdef MOTIF1_2 with #ifndef MOTIF1_1.
 * 	[1993/04/08  16:37:44  yak]
 * 
 * Revision 1.7.4.2  1993/03/15  20:01:01  rec
 * 	Test was recreating a widget that was not previously destroyed.
 * 	This caused pixmap comparison problems (looked like it was comparing
 * 	against a previous image).  Resolved by using XtSetValues instead
 * 	of XmCreate.  This work was done to resolve pixmap comparison
 * 	problems (T 34).
 * 	[1993/03/15  20:00:35  rec]
 * 
 * Revision 1.7.2.5  1992/04/06  20:53:38  libby
 * 	Can't use FlushEvents under Automation.
 * 	[1992/04/06  20:52:40  libby]
 * 
 * Revision 1.7.2.4  1992/04/02  22:01:13  tomm
 * 	Ansi changes from HP.
 * 	[1992/04/02  21:58:35  tomm]
 * 
 * Revision 1.7.2.3  1992/03/30  13:04:43  libby
 * 	Force Panel12 to come up 300 x 300.
 * 	[1992/03/28  21:16:32  libby]
 * 
 * Revision 1.7.2.2  1992/03/26  14:58:57  libby
 * 	Add test to correct working of P4502 (operator error as reported).
 * 	[1992/03/26  14:56:29  libby]
 * 
 * Revision 1.7  1992/03/13  17:05:45  devsrc
 * 	Converted to ODE
 * 
*/ 


#include <signal.h>
#include <testlib.h>


/*  Global Variables  */
Widget    parent;
Widget    DrawingArea1;
Widget    ArrowButton1;
Widget    Label1;
Widget    ArrowButton2;
Widget    DrawnButton1;
Widget    ChildToggle;
Widget    ParentToggle;
Widget    rowcol;
Widget    PB;

static Widget    resize_target;

/* forward references */
void ResizeMessage();
void ForceResize();
void ForceMove();
void ChangeResizeTarget();
void PBsize();
void ResizeChildtoFit( Widget w, XtPointer client_data, XtPointer call_data);
extern void FlushEvents();

void ExposeProc( w, d1, d2)
Widget w;
XtPointer d1;
XtPointer d2;
{
static int counter = 0;

  printf ("%2d: An expose event has been generated\n", ++counter);

}


enum {BBParent, FormParent} parentMode;

void  main (argc, argv)
int     argc;
char  **argv;
{
    Boolean            trace = False;
    register int       n;
    Arg                args[MAX_ARGS];
    XmString           tcs;
    int		       u_data;

    CommonTestInit(argc, argv);

    /* 
     *  If UserData is "xmBulletinBoardWidgetClass" or
     *  is not specified, we will create a
     *  Bulletin Board parent, which illustrates P2721. If it's
     *  xmFormWidgetClass,
     *  we'll create the Form parent, which lets us test ForceResize 
     *  on the parent, an uninteresting case unless the parent is
     *  a Form. Hence, when we're in BBParent mode, we'll also ignore 
     *  the ResizeParent cases and insensitize that toggle.
     */
   
    if (UserData) { 
       if (strcmp (UserData, "xmFormWidgetClass") == 0) {
           parentMode = FormParent;
	   printf ("Using Form Parent.\n");
       }
       else {
		printf ("Using BulletinBoard Parent.\n");
        	parentMode = BBParent;
       }
    }
    else {
        printf ("Using BulletinBoard Parent.\n");
        parentMode = BBParent;
    }
      
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  40);  n++;
    XtSetArg(args[n], XmNheight, 30);  n++;
    XtSetValues(Shell1, args, n);
    XtRealizeWidget(Shell1);

    n = 0;
    if (parentMode == BBParent) {
       parent = XmCreateBulletinBoard (Shell1, "parent", args, n);
#ifdef DEBUG
       printf ("Creating BulletinBoardParent.\n");
#endif
     }
    else {
       parent = XmCreateForm (Shell1, "parent", args, n);
#ifdef DEBUG
       printf ("Creating Form Parent\n");
#endif
     }
    XtManageChild (parent);

    n = 0;
    if (parentMode == BBParent) {
       XtSetArg (args[n], XmNx, 0); n++;
       XtSetArg (args[n], XmNy, 0); n++;
#ifdef DEBUG
       printf ("Setting Drawing Area child position to 0,0\n");
#endif
     }
    else {
       XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
       XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
       XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
       XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
#ifdef DEBUG
       printf ("Attaching DrawingArea child to edges\n");
#endif
     }
    DrawingArea1 = XmCreateDrawingArea(parent, "DrawingArea1", args, n);
    XtManageChild(DrawingArea1);
    XtAddCallback (DrawingArea1, XmNresizeCallback, ResizeMessage, NULL);

    n = 0;
    XtSetArg(args[n], XmNx, 4);  n++;
    XtSetArg(args[n], XmNy, 4);  n++;
    XtSetArg(args[n], XmNarrowDirection, XmARROW_UP);  n++;
    ArrowButton1 = XmCreateArrowButton(DrawingArea1, "ArrowButton1", args, n);
    XtManageChild(ArrowButton1);
    XtAddCallback (ArrowButton1, XmNactivateCallback, 
		   ForceResize, (XtPointer) 1); 

    n = 0;
    tcs = XmStringCreateSimple ("Resize Drawn Button");
    XtSetArg(args[n], XmNx, 50);  n++;
    XtSetArg(args[n], XmNy, 4);  n++;
    XtSetArg(args[n], XmNlabelString, tcs ); n++; 
    Label1 = XmCreateLabel(DrawingArea1, "Label1", args, n);
    XtManageChild(Label1);
    XmStringFree(tcs);

    n = 0;
    XtSetArg(args[n], XmNx, 200);  n++;
    XtSetArg(args[n], XmNy, 4);  n++;
    XtSetArg(args[n], XmNarrowDirection, XmARROW_DOWN);  n++;
    ArrowButton2 = XmCreateArrowButton(DrawingArea1, "ArrowButton2", args, n);
    XtManageChild(ArrowButton2);
    XtAddCallback (ArrowButton2, XmNactivateCallback, 
		   ForceResize, (XtPointer) -1); 

    n = 0;
    XtSetArg(args[n], XmNx, 4);  n++;
    XtSetArg(args[n], XmNy, 45);  n++;
    XtSetArg(args[n], XmNwidth, 135);  n++;
    XtSetArg(args[n], XmNheight, 135);  n++;
    DrawnButton1 = XmCreateDrawnButton(DrawingArea1, "DrawnButton1", args, n);
    XtManageChild(DrawnButton1);

    n = 0;
    XtSetArg(args[n], XmNx, 4);  n++;
    XtSetArg(args[n], XmNy, 195);  n++;
    XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg (args[n], XmNradioBehavior, True); n++;
    rowcol = XmCreateRowColumn(DrawingArea1, "rowcol", args, n);

    n = 0;
    XtSetArg(args[n], XmNwidth, 135);  n++;
    XtSetArg(args[n], XmNheight, 25);  n++;
    XtSetArg(args[n], XmNset, True); n++;
    tcs = XmStringLtoRCreate("Resize Child", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    ChildToggle = XmCreateToggleButton(rowcol, "ChildToggle", args, n);
    XtManageChild(ChildToggle);
    XmStringFree(tcs);
    XtAddCallback (ChildToggle, XmNvalueChangedCallback, 
		   ChangeResizeTarget, (XtPointer) DrawnButton1);

    resize_target = DrawnButton1;


    /* add a pushbutton gadget at bottom */
    n = 0;
    XtSetArg(args[n], XmNwidth, 135);  n++;
    XtSetArg(args[n], XmNheight, 25);  n++;
    XtSetArg(args[n], XmNset, False); n++;
    tcs = XmStringLtoRCreate("Resize Parent", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNlabelString, tcs);  n++;
    ParentToggle = XmCreateToggleButtonGadget(rowcol, "ParentToggle", args, n);
    XtManageChild(ParentToggle);
    XmStringFree(tcs);
    XtAddCallback (ParentToggle, XmNvalueChangedCallback, ChangeResizeTarget, 
		   (XtPointer) parent);

    /* if the parentMode is BB, this isn't an interesting case, so make
       it insensitive right off the bat. Since resize_target is already
       child, we're okay there.
    */

    if (parentMode == BBParent) {
      XtSetSensitive (ParentToggle, False);
    }

    XtManageChild (rowcol);
    
    /* reset the margins, change margin units */
    n = 0;
    XtSetArg(args[n],XmNmarginHeight,5); n++;
    XtSetArg(args[n],XmNmarginWidth,5); n++;
    XtSetArg(args[n],XmNunitType,Xm100TH_MILLIMETERS); n++;
    XtSetValues(DrawingArea1,args,n);

    /* reset the margins in 100ths of millimeters */
    n = 0;
    XtSetArg(args[n],XmNmarginHeight,500); n++;
    XtSetArg(args[n],XmNmarginWidth,500); n++;
    XtSetValues(DrawingArea1,args,n);

    CommonPause();           /* resize callback tests*/
    CommonPause();
    CommonPause();
    CommonPause();

    /* move the child instead of growing it */
    /* try to reproduce P2721*/

    /* change the label of the toggle to "move child" and force it on */

    n = 0;
    tcs = XmStringCreateSimple ("Move Child");
    XtSetArg (args[n], XmNlabelString, tcs );       n++;
    XtSetValues (ChildToggle, args, n);
    XmStringFree(tcs);

    if (XmToggleButtonGetState(ChildToggle) == False)
       XmToggleButtonSetState(ChildToggle, 
			      True,         /* set it ON */
			      False);       /* no notify */

    if (XmToggleButtonGetState(ParentToggle) == True)
       XmToggleButtonSetState (ParentToggle, False, False);

    if (parentMode == FormParent)
      XtSetSensitive (ParentToggle, False);    /* else it's already been done! */

    /* switch over to move callbacks */

    XtRemoveCallback (ArrowButton1, XmNactivateCallback, ForceResize, 
		      (XtPointer) 1);
    XtRemoveCallback (ArrowButton2, XmNactivateCallback, ForceResize, 
		      (XtPointer) -1);
    XtAddCallback (ArrowButton1, XmNactivateCallback, ForceMove, (XtPointer) 1);
    XtAddCallback (ArrowButton2, XmNactivateCallback, ForceMove, (XtPointer) -1);

    /* and force the resize_target to the child */

    resize_target = DrawnButton1;
    printf ("Resize target is Drawn button\n");

    CommonPause();

    XtAddCallback (DrawingArea1, XmNexposeCallback, ExposeProc, NULL);

    XtUnmanageChild (ParentToggle);
    XtManageChild (ParentToggle);

    CommonPause();

    XtUnmanageChild (DrawingArea1);
    printf ("Unmanaging ChildToggle\n");
    XtUnmanageChild(ChildToggle);
    printf ("Unmanaging DrawnButton1\n");
    XtUnmanageChild(DrawnButton1);
    printf ("Unmanaging ArrowButton2\n");
    XtUnmanageChild(ArrowButton2);
    printf ("Unmanaging Label1\n");
    XtUnmanageChild(Label1);
    printf ("Unmanaging ArrowButton1\n");
    XtUnmanageChild(ArrowButton1);
    printf ("Unmanaging ParentToggle\n");
    XtUnmanageChild (ParentToggle);
    XtUnmanageChild (rowcol);


/* this test shows P1138. Without the BESTCASE flag, you currently get 16
   expose events. With BESTCASE,  i.e., when you do manage and unmanage
   the parent, you get 4. I believe there is a workaround -- I haven't found
   it yet.
*/

#ifndef BESTCASE
    XtManageChild (DrawingArea1);
#endif
    XtManageChild (ParentToggle);

    CommonPause();

    /* Begin code for PIR 3791 */

    XtDestroyWidget(parent);

    n = 0;
    DrawingArea1 = XmCreateDrawingArea (Shell1, "DrawingArea1", args, n);
    XtManageChild (DrawingArea1);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 10); n++;
    XtSetArg(args[n], XmNwidth, 200); n++;
    XtSetArg(args[n], XmNheight, 20); n++;
    DrawnButton1 = XmCreateDrawnButton (DrawingArea1, "DrawnButton1", args, n);
    XtManageChild(DrawnButton1);

    n = 0;
    tcs = XmStringCreateSimple ("Push me to change Size"); 
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 30); n++;
    XtSetArg(args[n], XmNlabelString, tcs ); n++;
    PB = XmCreatePushButton(DrawingArea1, "PB", args, n);
    XtManageChild(PB);
    XmStringFree(tcs);
    XtAddCallback(PB, XmNactivateCallback, PBsize, (XtPointer) DrawnButton1);
  

    CommonPause();


    /* 
     *  Begin code for PIR 2775
     */

    XtDestroyWidget(DrawingArea1);

    n = 0;
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_GROW); n++;
    XtSetArg(args[n], XmNwidth, 500); n++;
    XtSetArg(args[n], XmNheight, 500); n++;
    DrawingArea1 = XmCreateDrawingArea(Shell1, "DrawingArea1", args, n);
    XtManageChild(DrawingArea1);
    
    n = 0;
    XtSetArg(args[n], XmNwidth, 100);  n++;
    XtSetArg(args[n], XmNheight, 100);  n++;
    XtSetArg(args[n], XmNarrowDirection, XmARROW_RIGHT);  n++;
    ArrowButton1 = XmCreateArrowButton(DrawingArea1, "ArrowButton1", args, n);
    XtManageChild(ArrowButton1);

    n = 0;
    XtSetArg(args[n], XmNheight, 600); n++;
    XtSetValues(ArrowButton1, args, n);

    CommonPause();

    /* 
     *  End code for PIR 2775
     */

#ifndef MOTIF1_1

    /*  Begin Test for PIR 3166  */
    n = 0;
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_GROW); n++;
    XtSetArg(args[n], XmNwidth, 100); n++;
    XtSetArg(args[n], XmNheight, 150); n++;
    XtSetValues(DrawingArea1, args, n);
    
    n = 0;
    XtSetArg(args[n], XmNx, 100);  n++;
    XtSetArg(args[n], XmNy, 100);  n++;
    XtSetArg(args[n], XmNwidth, 50); n++;
    XtSetArg(args[n], XmNheight, 50); n++;
    XtSetArg(args[n], XmNarrowDirection, XmARROW_RIGHT);  n++;
    XtSetValues(ArrowButton1, args, n);

    CommonPause();

    /* 
     *  Now move the child, drawing area should be (100, 200)
     *   not (50, 200)
     */

    n = 0;
    XtSetArg(args[n], XmNx, 50); n++;
    XtSetArg(args[n], XmNy, 200); n++;
    XtSetValues(ArrowButton1, args, n);

    /* End test for PIR 3166 */

    CommonPause();

    /*
     *  Now kill it all, and create a drawing area with a specified
     *   size.  It should not be ignored.
     */

    XtDestroyWidget(ArrowButton1);
    XtDestroyWidget(DrawingArea1);

    n = 0;
    XtSetArg(args[n], XmNwidth,  300);  n++;
    XtSetArg(args[n], XmNheight, 300);  n++;
    XtSetValues(Shell1, args, n);

    /* can't use under Automation */
    /* FlushEvents(); */

    n = 0;
    XtSetArg(args[n], XmNwidth, 300); n++;
    XtSetArg(args[n], XmNheight, 300); n++;
    XtSetArg(args[n], XmNresizePolicy, XmRESIZE_NONE); n++;
    DrawingArea1 = XmCreateDrawingArea(Shell1, "DrawingArea1", args, n);
    XtManageChild(DrawingArea1);
    
    n = 0;
    XtSetArg(args[n], XmNx, 50);  n++;
    XtSetArg(args[n], XmNy, 50);  n++;
    XtSetArg(args[n], XmNwidth, 30); n++;
    XtSetArg(args[n], XmNheight, 30); n++;
    XtSetArg(args[n], XmNarrowDirection, XmARROW_RIGHT);  n++;
    ArrowButton1 = XmCreateArrowButton(DrawingArea1, "ArrowButton1", args, n);
    XtManageChild(ArrowButton1);

    CommonPause();

    /* Begin test for PIR 4502 */
    /* in this panel, the arrow will resize itself when the parent is */
    /* resized. This is done by adding a resizeCallback to the Drawing */
    /* area that calls XtConfigureWidget on the arrow button. Note that */
    /* XtConfigureWidget, XtResizeWidget, and XtMoveWidget, which are */
    /* normally restricted to widget internals, are legal within resize */
    /* Callback procs. In fact, their use is required because simply */
    /* setValue'ing the child would cause another resizeCallback to be */
    /* issued from within the DrawingArea's Resize routine, thus setting */
    /* up an infinite loop of resize events */

    n = 0;
    XtSetArg (args[n], XmNresizePolicy, XmRESIZE_ANY); n++;
    XtSetValues (DrawingArea1, args, n);

    XtAddCallback(DrawingArea1, XmNresizeCallback, ResizeChildtoFit, 
		  (XtPointer) ArrowButton1);

    /* End test for PIR 4052 */
#endif  /* MOTIF1_1  */

    CommonPause();
    CommonPause();
    XtAppMainLoop(app_context);
}

void ResizeMessage (widget, client_data, call_data)
Widget widget;
XtPointer client_data, call_data;

{
  static int counter;
  printf ("%2d: resizeCallback for the Drawing Area called.\n", ++counter);

}

void ForceResize (widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;

{
/* check whether we are going to force the parent or the child */
Dimension height, width;
Arg args[2];

  XtSetArg (args[0], XmNheight, &height);
  XtSetArg (args[1], XmNwidth, &width);
  XtGetValues (resize_target, args, 2);

  if ((int)(long)client_data > 0) {  /* increase them */
    height += 10;
    width  += 10;
  }  
  else /* client_data < 1 */ {
    if (height <= 10)
      height = 1;
    else  height -= 10;
    if (width <= 10)
      width = 1;
    else width  -= 10;
  }
  
  XtSetArg (args[0], XmNheight, height);
  XtSetArg (args[1], XmNwidth, width);
  XtSetValues (resize_target, args, 2);

}

void ForceMove (widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;

{
/* check whether we are going to force the parent or the child */
Dimension xpos, ypos;
Arg args[2];

  XtSetArg (args[0], XmNx, &xpos);
  XtSetArg (args[1], XmNy, &ypos);
  XtGetValues (resize_target, args, 2);

  if ((int)(long)client_data > 0) {  /* increase them */
    xpos += 10;
    ypos  += 10;
  }  
  else /* client_data < 1 */ {
    if (xpos <= 10)
      xpos = 1;
    else  xpos -= 10;
    if (ypos <= 10)
      ypos = 1;
    else ypos  -= 10;
  }
  
  XtSetArg (args[0], XmNx, xpos);
  XtSetArg (args[1], XmNy, ypos);
  XtSetValues (resize_target, args, 2);

}
  
void ChangeResizeTarget (widget, client_data, call_data)
Widget widget;
Widget client_data;
XtPointer call_data;

{

/* only if the change is a 'being set' change; otherwise, we'll handle it
   when its partner's event arrives */

  if ( ((XmToggleButtonCallbackStruct *)call_data)->set == 0)
    return;
  else  {
  /* since we're being set, change the global that says which thing should
     be resized. */
     resize_target = (Widget) client_data;
     printf ("resize target is %s\n", XtName (client_data));

  }
}

void PBsize(widget, client_data, call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;

{
  Dimension width; int tptr;
  Arg args[1];

  XtSetArg(args[0], XmNwidth, &width); 
  XtGetValues((Widget) client_data, args, 1);
  printf("Setting width from %d TO %d\n",width,width+20);
  XtSetArg(args[0], XmNwidth, width+20); 
  XtSetValues((Widget) client_data, args, 1);

}

void ResizeChildtoFit (Widget w, XtPointer client_data, XtPointer call_data)
{
  Dimension manager_width, manager_height;
  int n;
  Arg args[10];
  
  n=0;
  XtSetArg(args[n], XmNwidth, &manager_width); n++;
  XtSetArg(args[n], XmNheight, &manager_height); n++;
  XtGetValues(w, args, n);

  printf("manager_width =%d  manager_height = %d\n", 
	 manager_width, manager_height);

  XtConfigureWidget ((Widget) client_data, 
		     0, 0,
		     manager_width - 1, 		
		     manager_height - 1, 
		     0);
}
