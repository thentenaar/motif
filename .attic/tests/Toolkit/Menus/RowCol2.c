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
static char rcsid[] = "$XConsortium: RowCol2.c /main/9 1995/07/13 18:54:34 drk $"
#endif
#endif

#include <testlib.h>

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

void KillMe(Widget w, XtPointer closure, XtPointer call_data)
{
  XtDestroyWidget(w);
}

static XtCallbackRec killCallback[] = {
  {KillMe, (XtPointer) NULL },
  {NULL,   (XtPointer) NULL }};

Pixel CvtStringToPixel(char *src_string)
{ 
  XColor aColor, bColor;   
  XAllocNamedColor(XtDisplay(Shell1),
		   DefaultColormapOfScreen(XtScreen(Shell1)), 
		   src_string, &aColor, &bColor);
  return(aColor.pixel);
}

void ChangeFocusPolicy(Widget w, XtPointer closure, XtPointer call_data)
{
  Arg args[5];
  unsigned char policy;

  XtSetArg (args[0], XmNkeyboardFocusPolicy, &policy);
  XtGetValues(Shell1, args, 1);

  if (policy == XmEXPLICIT)
    XtSetArg (args[0], XmNkeyboardFocusPolicy, XmPOINTER);
  else
    XtSetArg (args[0], XmNkeyboardFocusPolicy, XmEXPLICIT);

  XtSetValues(Shell1, args, 1);
}

static XtCallbackRec focusCallback[] = {
  {ChangeFocusPolicy, (XtPointer) NULL },
  {NULL,   (XtPointer) NULL },};

void NoTraversal(Widget w, XtPointer closure, XtPointer call_data)
{
  Arg args[15];
  Boolean travOn;
  XmString tcs;

  XtSetArg(args[0], XmNtraversalOn, &travOn);
  XtGetValues(w, args, 1);

  if (travOn)
    {
      XtSetArg(args[0], XmNtraversalOn, False);
      XtSetValues(w, args, 1);
      tcs=XmStringCreateLtoR("TraversalOff",XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[0], XmNlabelString, tcs);
      XtSetValues(w, args, 1);
      XmStringFree(tcs);
    }
  else
    {
      XtSetArg(args[0], XmNtraversalOn, True);
      XtSetValues(w, args, 1);
      tcs=XmStringCreateLtoR("TraversalOn",XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[0], XmNlabelString, tcs);
      XtSetValues(w, args, 1);
      XmStringFree(tcs);
    }
}

static XtCallbackRec noTraversalCallback[] = {
  {NoTraversal, (XtPointer) NULL },
  {NULL, (XtPointer) NULL }};

void Sensitivity(Widget w, XtPointer closure, XtPointer call_data)
{
  Arg args[15];
  Boolean sensitive;
  XmString tcs;

  XtSetArg(args[0], XmNsensitive, &sensitive);
  XtGetValues(pButton3, args, 1);

  if (sensitive)
    {
      tcs = XmStringCreateLtoR("Make Blue Button Sensitive", 
			       XmSTRING_DEFAULT_CHARSET);
      XtSetSensitive(pButton3, False);
      XtSetArg(args[0], XmNlabelString, tcs);
      XtSetValues(w, args, 1);
    }
  else
    {
      tcs = XmStringCreateLtoR("Make Blue Button Insensitive", 
			       XmSTRING_DEFAULT_CHARSET);
      XtSetSensitive(pButton3, True);
      XtSetArg(args[0], XmNlabelString, tcs);
      XtSetValues(w, args, 1);
    }
}

static XtCallbackRec sensitivityCallback[] = {
  {Sensitivity, (XtPointer) NULL },
  {NULL, (XtPointer) NULL }};

void Grow(Widget w, XtPointer closure, XtPointer call_data)
{
   Arg args[2];
   Dimension height, width;

   XtSetArg(args[0], XmNheight, &height);
   XtSetArg(args[1], XmNwidth, &width);
   XtGetValues(Shell1, args, 2);
   height = height+50;
   width = width + 75;
   XtSetArg(args[0], XmNheight, height);
   XtSetArg(args[1], XmNwidth, width);
   XtSetValues(Shell1, args, 2);
}

static XtCallbackRec growCallback[] = {
   {Grow, (XtPointer) NULL },
   {NULL, (XtPointer) NULL }};

void Shrink(Widget w, XtPointer closure, XtPointer call_data)
{
    Arg args[2];
    Dimension height, width;

    XtSetArg(args[0], XmNheight, &height);
    XtSetArg(args[1], XmNwidth, &width);
    XtGetValues(Shell1, args, 2);
    height = height - 50;
    width = width - 75;
    XtSetArg(args[0], XmNheight, height);
    XtSetArg(args[1], XmNwidth, width);
    XtSetValues(Shell1, args, 2);
}

static XtCallbackRec shrinkCallback[] = {
  {Shrink, (XtPointer) NULL },
  {NULL,   (XtPointer) NULL }};

void UnMap(Widget w, XtPointer closure, XtPointer call_data)
{ 

  XtSetMappedWhenManaged (pButton3, FALSE);
}

static XtCallbackRec unmapCallback[] = {
   {UnMap, (XtPointer) NULL },
   {NULL,  (XtPointer) NULL }};

void Map(Widget w, XtPointer closure, XtPointer call_data)
{
  XtSetMappedWhenManaged (pButton3, TRUE);
}

static XtCallbackRec mapCallback[] = {
  {Map,  (XtPointer) NULL },
  {NULL, (XtPointer) NULL }};

void MgrTabGroup(Widget w, XtPointer closure, XtPointer call_data)
{
  Arg args[5];
  XmString tcs;

  if (R3MgrRegistered)
    {
      R3MgrRegistered = False;
      XmRemoveTabGroup(rc3);
      tcs = XmStringCreateLtoR ("Add TabGroup", XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[0], XmNlabelString, tcs);
      XtSetValues(w, args, 1);
    }
  else
    {
      R3MgrRegistered = True;
      tcs = XmStringCreateLtoR ("Remove TabGroup", XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[0], XmNlabelString, tcs);
      XtSetValues(w, args, 1);
      XmAddTabGroup(rc3);
    }
}

static XtCallbackRec mgrTabGroupCallback[] = {
  { MgrTabGroup,  (XtPointer) NULL },
  { NULL, (XtPointer) NULL }};

void PrimTabGroup(Widget w, XtPointer closure, XtPointer call_data)
{
   Arg args[5];
   if (primRegistered)
   {
      primRegistered = False;
      XmRemoveTabGroup(pButton6);
      XtSetArg(args[0], XmNlabelString, "Add Primitive TabGroup");
      XtSetValues(w, args, 1);
   }
   else
   {
      primRegistered = True;
      XtSetArg(args[0], XmNlabelString, "Remove Primitive TabGroup");
      XtSetValues(w, args, 1);
      XmAddTabGroup(pButton6);
   }
}

static XtCallbackRec primTabGroupCallback[] = {
   { PrimTabGroup,  (XtPointer) NULL },
   { NULL, (XtPointer) NULL }};

void DestroyMgr(Widget w, XtPointer closure, XtPointer call_data)
{
  Arg args[5];
  XmString tcs;
  int n;

  if (rc2)
    {
      XtDestroyWidget(rc2);
      rc2 = NULL;
      R2MgrRegistered = False;
      tcs=XmStringCreateLtoR("Create Paned Window  ",
			     XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[0], XmNlabelString, tcs);
      XtSetValues(w, args, 1);
      XmStringFree(tcs);
    }
  else
    {
      n = 0;
      XtSetArg(args[n], XmNspacing, 4); n++;
      rc2 = XmCreatePanedWindow(frame2, "rc2", (ArgList) args, n);
      /* Xt: a composite child of a realized widget needs to be explicitly
      ** realized before managed [as VTS does] in order to have its 
      ** ChangeManaged proc called correctly
      */
      XtRealizeWidget(rc2);
      R2MgrRegistered = True;

      n=0;
      XtSetArg(args[n], XmNtraversalOn, True); n++;
      XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
      XtSetArg(args[n], XmNhighlightThickness, 2); n++;
      XtSetArg(args[n], XmNresizeHeight, True); n++;
      XtManageChild(XmCreatePushButton(rc2, "misc1", (ArgList) args, n));
      XtManageChild(XmCreatePushButton(rc2, "misc2", (ArgList) args, n));
      XtManageChild(XmCreatePushButton(rc2, "misc3", (ArgList) args, n));
      XtManageChild(XmCreatePushButton(rc2, "misc4", (ArgList) args, n));
      XmAddTabGroup(rc2);
      XtManageChild(rc2);

      tcs=XmStringCreateLtoR("Destroy Paned Window", XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[0], XmNlabelString, tcs);
      XtSetValues(w, args, 1);
      XmStringFree(tcs);
    }
}

static XtCallbackRec destroyMgrCallback[] = {
  { DestroyMgr,  (XtPointer) NULL },
  { NULL, (XtPointer) NULL }};


int main(int argc, char **argv)
{
  int n;
  Arg args[15];

  /*  initialize toolkit  */
  CommonTestInit(argc, argv);

  /****************************************************************/
  /*  Create the of the top-level frame and row-column manager    */
  /****************************************************************/

  XtSetArg(args[0], XmNshadowThickness, 2);
  frame0 = (Widget)XmCreateFrame(Shell1, "frame0", args, 1);
  XtManageChild (frame0);

  XtSetArg(args[0], XmNpacking, XmPACK_COLUMN);
  XtSetArg(args[1], XmNnumColumns, 1);
  rc0 = XmCreateRowColumn(frame0, "outer", (ArgList) args, 2);
  XtManageChild(rc0);

  /****************************************************************/
  /*  Create the children of the top-level row-column manager     */
  /*  Each child is a frame widget containing a single row-column */
  /*     manager as its child.                                    */
  /****************************************************************/

  XtSetArg(args[0], XmNshadowThickness, 2);
  frame1 = (Widget)XmCreateFrame(rc0, "frame1", args, 1);
  XtManageChild (frame1);

  XtSetArg(args[0], XmNpacking, XmPACK_COLUMN);
  XtSetArg(args[1], XmNnumColumns, 2);
  XtSetArg(args[2], XmNadjustLast, False);
  rc1 = XmCreateRowColumn(frame1, "rc1", (ArgList) args, 2);
  XtManageChild(rc1);

  XtSetArg(args[0], XmNshadowThickness, 2);
  frame2 = (Widget)XmCreateFrame(rc0, "frame2", args, 1);
  XtManageChild(frame2);

  /* should match DestroyMgr */
  XtSetArg(args[0], XmNspacing, 4); 
  rc2 = XmCreatePanedWindow(frame2, "rc2", (ArgList) args, 1);
  XtManageChild(rc2);

  XtSetArg(args[0], XmNshadowThickness, 2);
  frame3 = (Widget)XmCreateFrame(rc0, "frame3", args, 1);
  XtManageChild(frame3);

  XtSetArg(args[0], XmNpacking, XmPACK_COLUMN);
  XtSetArg(args[1], XmNnumColumns, 2);
  rc3 = XmCreateRowColumn(frame3, "rc3", (ArgList) args, 2);
  XtManageChild(rc3);

  XtSetArg(args[0], XmNshadowThickness, 2);
  frame4 = (Widget)XmCreateFrame(rc0, "frame4", args, 1);
  XtManageChild(frame4);

  XtSetArg(args[0], XmNpacking, XmPACK_COLUMN);
  XtSetArg(args[1], XmNnumColumns, 2);
  rc4 = XmCreateRowColumn(frame4, "rc4", (ArgList) args, 2);
  XtManageChild(rc4);

  /****************************************************************/
  /*  Create the widget children of the first row-column manager  */
  /****************************************************************/

  n = 0;
  XtSetArg(args[n], XmNtraversalOn, True); n++;
  XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
  XtSetArg(args[n], XmNhighlightThickness, 2); n++;
  XtSetArg(args[n], XmNactivateCallback, (XtArgVal) growCallback); n++;
  XtManageChild(XmCreatePushButton(rc1, "grow", (ArgList) args, n)); n--;
  XtSetArg(args[n], XmNactivateCallback, (XtArgVal) shrinkCallback); n++;
  XtManageChild(XmCreatePushButton(rc1, "shrink", (ArgList) args, n)); n--;
  XtSetArg(args[n], XmNactivateCallback, (XtArgVal) unmapCallback); n++;
  XtManageChild(XmCreatePushButton(rc1, "unmap", (ArgList) args, n)); n--;
  XtSetArg(args[n], XmNactivateCallback, (XtArgVal) mapCallback); n++;
  XtManageChild(XmCreatePushButton(rc1, "map", (ArgList) args, n));

  /****************************************************************/
  /*  Create the widget children of the second manager */
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
  XtManageChild(pButton1);  n--;
  XtSetArg(args[n], XmNactivateCallback, (XtArgVal) killCallback); n++;
  pButton2 = XmCreatePushButton(rc3, "destroyMe", (ArgList) args, n);
  XtManageChild(pButton2); n--;
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
  pButton4 = XmCreatePushButton(rc3, "Change_Tab_Group",(ArgList) args, n);
  XtManageChild(pButton4); n--;

  XtSetArg(args[n], XmNactivateCallback, (XtArgVal) primTabGroupCallback);n++;
  pButton5 = XmCreatePushButton(rc3, "not Traversable",(ArgList) args, n);
  XtManageChild(pButton5);   n--;

  XtSetArg(args[n], XmNactivateCallback, (XtArgVal) sensitivityCallback);n++;
  pButton7 = XmCreatePushButton(rc3, "sensitivity",(ArgList) args, n);
  XtManageChild(pButton7);   n--;

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
  XtManageChild(text1); n--;

  XtSetArg(args[n], XmNvalue, (XtArgVal) "make my day"); n++;
  text2 = XmCreateText(rc4, "text2",(ArgList) args, n);
  XtManageChild(text2);

  /****************************************************************/
  /*  Create some more miscellaneous widget children              */
  /****************************************************************/

  XtSetArg(args[0], XmNvalue, (XtArgVal) "I can't take the focus");
  text3 = XmCreateText(rc0, "text3", (ArgList) args, 1);
  XtManageChild(text3);

  /****************************************************************/
  /*  Realize the widget hierarchy, and enter mainloop processing */
  /****************************************************************/

  AddAllTabGroups();
  XtRealizeWidget (Shell1);

  CommonPause();                               /* look around */
  CommonPause();                               /* first region */
    
  XtSetArg(args[0], XmNtraversalOn, False);
  XtSetValues(pButton5, args, 1);
  CommonPause();                               /* non-traversable buttons */

  CommonPause();			       /* destroy region 2 */
  CommonPause();                               /* exit */
  XtAppMainLoop(app_context);
}
