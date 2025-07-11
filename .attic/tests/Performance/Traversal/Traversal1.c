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
static char rcsid[] = "$XConsortium: Traversal1.c /main/7 1995/07/13 17:26:55 drk $"
#endif
#endif
/*
 *	file: Traversal1.c
 */

#include <testlib.h>

/*  Private Functions  */
static void  quitCB();

/*************Some Globals***********************/

Widget rc0, rc1, rc2, rc3, rc4;
Widget frame0, frame1, frame2, frame3, frame4;
Widget pButton1, pButton2, pButton3, pButton4;
Widget pButton5, pButton6, pButton7, pButton8;
Widget text1, text2, text3;
Pixel color, fgPixel, bgPixel;

Boolean R3MgrRegistered = True;
Boolean R2MgrRegistered = True;
Boolean primRegistered = True;

/************************************************/

AddAllTabGroups()
{
   /* Add in correct order */
   XmAddTabGroup(rc1);
   XmAddTabGroup(rc2);
   XmAddTabGroup(rc3);
/* XmAddTabGroup(pButton3); */
   XmAddTabGroup(rc4);
   XmAddTabGroup(pButton6); 
}

/*************CALLBACK PROCEDURES****************/

void KillMe(w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
  XtDestroyWidget(w);
}

static XtCallbackRec killCallback[] =
{
   { KillMe, NULL },
   { NULL,   NULL },
};



Pixel CvtStringToPixel(src_string)
    char *  src_string;
{ 
    XColor aColor, bColor;   

    XAllocNamedColor(XtDisplay(Shell1),
                     DefaultColormapOfScreen(XtScreen(Shell1)), 
		     src_string, &aColor, &bColor);

    return(aColor.pixel);
}



void ChangeFocusPolicy(w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
   int	n;
   Arg args[5];
   unsigned char policy;

   n = 0;
   XtSetArg (args[n], XmNkeyboardFocusPolicy, &policy); n++;
   XtGetValues(Shell1, args, n);

   n = 0;
   if (policy == XmEXPLICIT) {
      XtSetArg (args[n], XmNkeyboardFocusPolicy, XmPOINTER); n++;
   }
   else {
      XtSetArg (args[n], XmNkeyboardFocusPolicy, XmEXPLICIT); n++;
   }

   XtSetValues(Shell1, args, n);
}

static XtCallbackRec focusCallback[] =
{
   { ChangeFocusPolicy, NULL },
   { NULL,   NULL },
};


void NoTraversal(w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
    Arg args[15];
	int	n;
    Boolean travOn;
    XmString tcs;

	n = 0;
    XtSetArg(args[n], XmNtraversalOn, &travOn); n++;
    XtGetValues(w, args, n);

    if (travOn)
    {
	   n = 0;
       XtSetArg(args[n], XmNtraversalOn, False); n++;
       XtSetValues(w, args, n);
       tcs=XmStringCreateLtoR("TraversalOff",XmSTRING_DEFAULT_CHARSET);
	   n = 0;
       XtSetArg(args[n], XmNlabelString, tcs); n++;
       XtSetValues(w, args, n);
    }
    else
    {
	   n = 0;
       XtSetArg(args[n], XmNtraversalOn, True); n++;
       XtSetValues(w, args, n);
       tcs=XmStringCreateLtoR("TraversalOn",XmSTRING_DEFAULT_CHARSET);
	   n = 0;
       XtSetArg(args[n], XmNlabelString, tcs); n++;
       XtSetValues(w, args, n);
    }
}

static XtCallbackRec noTraversalCallback[] =
{
   { NoTraversal, NULL },
   { NULL,        NULL },
};


void Sensitivity(w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
    Arg args[15];
	int	n;
    Boolean sensitive;
    XmString tcs;

	n = 0;
    XtSetArg(args[n], XmNsensitive, &sensitive); n++;
    XtGetValues(pButton3, args, n);

    if (sensitive)
    {
       tcs = XmStringCreateLtoR(
		"Make Blue Button Sensitive", XmSTRING_DEFAULT_CHARSET);
       XtSetSensitive(pButton3, False);
	   n = 0;
       XtSetArg(args[n], XmNlabelString, tcs); n++;
       XtSetValues(w, args, n);
    }
    else
    {
       tcs = XmStringCreateLtoR(
		"Make Blue Button Insensitive", XmSTRING_DEFAULT_CHARSET);
       XtSetSensitive(pButton3, True);
	   n = 0;
       XtSetArg(args[n], XmNlabelString, tcs); n++;
       XtSetValues(w, args, n);
    }
}

static XtCallbackRec sensitivityCallback[] =
{
   { Sensitivity, NULL },
   { NULL,        NULL },
};


void Grow(w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
   Arg args[2];
   int	n;
   Dimension height, width;

   n = 0;
   XtSetArg(args[n], XmNheight, &height); n++;
   XtSetArg(args[n], XmNwidth, &width); n++;
   XtGetValues(Shell1, args, n);
   height = height+50;
   width = width + 75;
   n = 0;
   XtSetArg(args[n], XmNheight, height); n++;
   XtSetArg(args[n], XmNwidth, width); n++;
   XtSetValues(Shell1, args, n);
}

static XtCallbackRec growCallback[] =
{
   { Grow, NULL },
   { NULL, NULL },
};


void Shrink(w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
    Arg args[2];
	int	n;
    Dimension height, width;

	n = 0;
    XtSetArg(args[n], XmNheight, &height); n++;
    XtSetArg(args[n], XmNwidth, &width); n++;
    XtGetValues(Shell1, args, n);
    height = height - 50;
    width = width - 75;
	n = 0;
    XtSetArg(args[n], XmNheight, height); n++;
    XtSetArg(args[n], XmNwidth, width); n++;
    XtSetValues(Shell1, args, n);
}

static XtCallbackRec shrinkCallback[] =
{
   { Shrink, NULL },
   { NULL,   NULL },
};


void UnMap(w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
   XtUnmapWidget(pButton3);
}

static XtCallbackRec unmapCallback[] =
{
   { UnMap, NULL },
   { NULL,  NULL },
};


void Map(w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
   XtMapWidget(pButton3);
}

static XtCallbackRec mapCallback[] =
{
   { Map,  NULL },
   { NULL, NULL },
};


void MgrTabGroup(w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
   Arg args[5];
   int	n;
   XmString tcs;

   if (R3MgrRegistered)
   {
      R3MgrRegistered = False;
      XmRemoveTabGroup(rc3);
      tcs = XmStringCreateLtoR ("Add TabGroup", XmSTRING_DEFAULT_CHARSET);
	  n = 0;
      XtSetArg(args[n], XmNlabelString, tcs); n++;
      XtSetValues(w, args, n);
   }
   else
   {
      R3MgrRegistered = True;
      tcs = XmStringCreateLtoR ("Remove TabGroup", XmSTRING_DEFAULT_CHARSET);
	  n = 0;
      XtSetArg(args[n], XmNlabelString, tcs); n++;
      XtSetValues(w, args, n);
      XmAddTabGroup(rc3);
   }
}

static XtCallbackRec mgrTabGroupCallback[] =
{
   { MgrTabGroup,  NULL },
   { NULL, NULL },
};


void PrimTabGroup(w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
   Arg args[5];
   int	n;
   XmString	label_string;

   if (primRegistered)
   {
      primRegistered = False;
      XmRemoveTabGroup(pButton6);
	  n = 0;
     label_string = 
		XmStringCreateLtoR("Add Primitive TabGroup", XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[n], XmNlabelString, label_string); n++;
      XtSetValues(w, args, n);
   }
   else
   {
      primRegistered = True;
	  n = 0;
     label_string = 
		XmStringCreateLtoR("Remove Primitive TabGroup", XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[n], XmNlabelString, label_string); n++;
      XtSetValues(w, args, n);
      XmAddTabGroup(pButton6);
   }
   XmStringFree(label_string);
}

static XtCallbackRec primTabGroupCallback[] =
{
   { PrimTabGroup,  NULL },
   { NULL, NULL },
};


void DestroyMgr (w, closure, call_data)
    Widget      w;
    XtPointer     closure;
    XtPointer     call_data;
{
   Arg args[5];
   XmString tcs;
   int n;

   if (rc2)
   {
      XtDestroyWidget(rc2);
      rc2 = NULL;
      R2MgrRegistered = False;
      tcs=XmStringCreateLtoR("Create Paned Window  ", XmSTRING_DEFAULT_CHARSET);
	  n = 0;
      XtSetArg(args[n], XmNlabelString, tcs); n++;
      XtSetValues(w, args, n);
   }
   else
   {
      n = 0;
      XtSetArg(args[n], XmNspacing, 4); n++;
      rc2 = XmCreatePanedWindow(frame2, "rc2", (ArgList) args, n);
      XtManageChild(rc2);
      R2MgrRegistered = True;

      n=0;
      XtSetArg(args[n], XmNtraversalOn, True); n++;
      XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
      XtSetArg(args[n], XmNhighlightThickness, 2); n++;
      XtManageChild(XmCreatePushButton(rc2, "misc1", (ArgList) args, n));
      XtManageChild(XmCreatePushButton(rc2, "misc2", (ArgList) args, n));
      XtManageChild(XmCreatePushButton(rc2, "misc3", (ArgList) args, n));
      XtManageChild(XmCreatePushButton(rc2, "misc4", (ArgList) args, n));
      XmAddTabGroup(rc2);

      tcs=XmStringCreateLtoR("Destroy Paned Window", XmSTRING_DEFAULT_CHARSET);
	  n = 0;
      XtSetArg(args[n], XmNlabelString, tcs); n++;
      XtSetValues(w, args, n);
   }
}

static XtCallbackRec destroyMgrCallback[] =
{
   { DestroyMgr,  NULL },
   { NULL, NULL },
};


void main (argc, argv)
unsigned int argc;
char **argv;

{
    int n;
    Arg args[15];
	Widget	quit_button;

  

    /*  initialize toolkit  */
    CommonTestInit(argc, argv);
    

   /****************************************************************/
   /*  Create the of the top-level frame and row-column manager    */
   /****************************************************************/

	n = 0;
    XtSetArg(args[n], XmNshadowThickness, 2); n++;
    frame0 = (Widget)XmCreateFrame(Shell1, "frame0", args, n);
    XtManageChild (frame0);

	n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    rc0 = XmCreateRowColumn(frame0, "outer", (ArgList) args, n);
    XtManageChild(rc0);


   /****************************************************************/
   /*  Create the children of the top-level row-column manager     */
   /*  Each child is a frame widget containing a single row-column */
   /*     manager as its child.                                    */
   /****************************************************************/

	n = 0;
    XtSetArg(args[n], XmNshadowThickness, 2); n++;
    frame1 = (Widget)XmCreateFrame(rc0, "frame1", args, n);
    XtManageChild (frame1);

	n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    XtSetArg(args[n], XmNadjustLast, False); n++;
    rc1 = XmCreateRowColumn(frame1, "rc1", (ArgList) args, n);
    XtManageChild(rc1);

	n = 0;
    XtSetArg(args[n], XmNshadowThickness, 2); n++;
    frame2 = (Widget)XmCreateFrame(rc0, "frame2", args, n);
    XtManageChild (frame2);

	n = 0;
    rc2 = XmCreatePanedWindow(frame2, "rc2", (ArgList) args, n);
    XtManageChild(rc2);

	n = 0;
    XtSetArg(args[n], XmNshadowThickness, 2); n++;
    frame3 = (Widget)XmCreateFrame(rc0, "frame3", args, n);
    XtManageChild (frame3);

	n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    rc3 = XmCreateRowColumn(frame3, "rc3", (ArgList) args, n);
    XtManageChild(rc3);

	n = 0;
    XtSetArg(args[n], XmNshadowThickness, 2); n++;
    frame4 = (Widget)XmCreateFrame(rc0, "frame4", args, n);
    XtManageChild (frame4);

	n = 0;
    XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
    XtSetArg(args[n], XmNnumColumns, 2); n++;
    rc4 = XmCreateRowColumn(frame4, "rc4", (ArgList) args, n);
    XtManageChild(rc4);


   /****************************************************************/
   /*  Create the widget children of the first row-column manager  */
   /****************************************************************/
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) growCallback); n++;
   XtManageChild(XmCreatePushButton(rc1, "grow", (ArgList) args, n));
   n--;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) shrinkCallback); n++;
   XtManageChild(XmCreatePushButton(rc1, "shrink", (ArgList) args, n));
   n--;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) unmapCallback); n++;
   XtManageChild(XmCreatePushButton(rc1, "unmap", (ArgList) args, n));
   n--;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) mapCallback); n++;
   XtManageChild(XmCreatePushButton(rc1, "map", (ArgList) args, n));


   /****************************************************************/
   /*  Create the widget children of the second row-column manager */
   /****************************************************************/
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   
   XtManageChild(XmCreatePushButton(rc2, "misc1", (ArgList) args, n));

   XtManageChild(XmCreatePushButton(rc2, "misc2", (ArgList) args, n));

   XtManageChild(XmCreatePushButton(rc2, "misc3", (ArgList) args, n));

   XtManageChild(XmCreatePushButton(rc2, "misc4", (ArgList) args, n));


   /****************************************************************/
   /*  Create the widget children of the third row-column manager  */
   /****************************************************************/
   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;

   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) noTraversalCallback);n++;
   pButton1 = XmCreatePushButton(rc3, "toggleTraversal", (ArgList) args, n);
   XtManageChild(pButton1);
   n--;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) killCallback); n++;
   pButton2 = XmCreatePushButton(rc3, "destroyMe", (ArgList) args, n);
   XtManageChild(pButton2);
   n--;
   fgPixel = CvtStringToPixel("White");
   bgPixel = CvtStringToPixel("SteelBlue");

   XtSetArg(args[n], XmNforeground, (XtArgVal) fgPixel);n++;
   XtSetArg(args[n], XmNbackground, (XtArgVal) bgPixel);n++;
   pButton3 = XmCreatePushButton(rc3, "blueButton", (ArgList) args, n);
   XtManageChild(pButton3);

   n = 0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;

   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) mgrTabGroupCallback); n++;
   pButton4 = XmCreatePushButton(rc3, "Change Tab Group",(ArgList) args, n);
   XtManageChild(pButton4);
   n--;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) primTabGroupCallback);n++;
   pButton5 = XmCreatePushButton(rc3, "not Traversable",(ArgList) args, n);
   XtManageChild(pButton5);
   n--;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) sensitivityCallback);n++;
   pButton7 = XmCreatePushButton(rc3, "sensitivity",(ArgList) args, n);
   XtManageChild(pButton7);
   n--;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) focusCallback); n++;
   pButton8 = XmCreatePushButton(rc3, "focus",(ArgList) args, n);
   XtManageChild(pButton8);


   /****************************************************************/
   /*  Create some more miscellaneous widget children              */
   /****************************************************************/
   n=0;
   XtSetArg(args[n], XmNactivateCallback, (XtArgVal) destroyMgrCallback);n++;
   XtSetArg(args[n], XmNtraversalOn, True);n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++; 
   pButton6 = XmCreatePushButton(rc0, "destroyMgr", (ArgList) args, n); 
   XtManageChild(pButton6);


   /****************************************************************/
   /*  Create the widget children of the fourth row-column manager */
   /****************************************************************/
   n=0;
   XtSetArg(args[n], XmNtraversalOn, True); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
   XtSetArg(args[n], XmNvalue, (XtArgVal) "edit my string"); n++;
   text1 = XmCreateText(rc4, "text1",(ArgList) args, n);
   XtManageChild(text1);
   n--;
   XtSetArg(args[n], XmNvalue, (XtArgVal) "make my day"); n++;
   text2 = XmCreateText(rc4, "text2",(ArgList) args, n);
   XtManageChild(text2);


   /****************************************************************/
   /*  Create some more miscellaneous widget children              */
   /****************************************************************/

   n = 0;
   XtSetArg(args[n], XmNvalue, (XtArgVal) "I can't take the focus"); n++;
   XtSetArg(args[n], XmNtraversalOn, True);n++;
   text3 = XmCreateText(rc0, "text3", (ArgList) args, n);
   XtManageChild(text3);

   /****************************************************************/
   /*  Create quit button
   /****************************************************************/
   n=0;
   XtSetArg(args[n], XmNtraversalOn, True);n++;
   XtSetArg(args[n], XmNhighlightThickness, 2); n++;
   XtSetArg(args[n], XmNhighlightOnEnter, True); n++; 
   XtSetArg(args[n], XmNshowAsDefault, 2); n++; 
   quit_button = XmCreatePushButton(rc0, "Quit", (ArgList) args, n); 
	XtManageChild(quit_button);
	XtAddCallback(quit_button, XmNactivateCallback, quitCB, NULL);

   /****************************************************************/
   /*  Realize the widget hierarchy, and enter mainloop processing */
   /****************************************************************/

    AddAllTabGroups();
	n = 0;
    XtSetArg(args[n], XmNtraversalOn, False); n++;
    XtSetValues(pButton5, args, n);

    XtRealizeWidget (Shell1);

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


