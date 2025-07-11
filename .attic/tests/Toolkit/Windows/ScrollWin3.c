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
static char rcsid[] = "$XConsortium: ScrollWin3.c /main/8 1995/07/13 19:49:07 drk $"
#endif
#endif

#include <testlib.h>

#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>

Widget  DrawingArea1;
Widget  ScrolledWindow2;
Widget  vertSB, horizSB;
Widget	Text1;
Widget	Clip1;

static void  ResizeDrawAreaCB();
static void  ChangeCallback();

#define	SLIDER_SIZE	10


void CreateScrollBars(parent)
Widget parent;
{
    Arg args[10];
    int n;

    n = 0;
    XtSetArg (args[n], XmNorientation, XmVERTICAL);  n++;
    XtSetArg (args[n], XmNbackground, CommonGetColor("aquamarine"));  n++;
    XtSetArg (args[n], XmNmaximum, 60);  n++;
    XtSetArg (args[n], XmNminimum, 0);  n++;
    XtSetArg (args[n], XmNtraversalOn, True);  n++;
    XtSetArg (args[n], XmNhighlightOnEnter, True);  n++;
    XtSetArg (args[n], XmNhighlightThickness, 2);  n++;
    XtSetArg (args[n], XmNsliderSize, SLIDER_SIZE);  n++;
    vertSB = XmCreateScrollBar (parent, "vertSB", args, n);
    XtManageChild (vertSB);

    XtAddCallback (vertSB, XmNvalueChangedCallback, ChangeCallback , NULL);

    n = 0;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetArg (args[n], XmNbackground, CommonGetColor("aquamarine"));  n++;
    XtSetArg (args[n], XmNmaximum, 59);  n++;
    XtSetArg (args[n], XmNminimum, 0);  n++;
    XtSetArg (args[n], XmNtraversalOn, True);  n++;
    XtSetArg (args[n], XmNhighlightOnEnter, True);  n++;
    XtSetArg (args[n], XmNhighlightThickness, 2);  n++;
    XtSetArg (args[n], XmNsliderSize, SLIDER_SIZE);  n++;
    horizSB = XmCreateScrollBar (parent, "horizSB", args, n);

    XtAddCallback (horizSB, XmNvalueChangedCallback, ChangeCallback , NULL);

    XtManageChild (horizSB);
}

static void ChangeCallback(w, client_data, call_data)
Widget   w;
XtPointer client_data;
XtPointer call_data;
{
    int val_h, val_v, val;
    Arg args[5];
    int n = 0;

    n = 0;
    XtSetArg(args[n], XmNvalue, &val_h);  n++;
    XtGetValues (horizSB, args, n);

    n = 0;
    XtSetArg(args[n], XmNvalue, &val_v);  n++;
    XtGetValues (vertSB, args, n);

    val = (51 * (val_v / SLIDER_SIZE)) + val_h;

    n = 0;
    XtSetArg(args[n], XmNcursorPosition, val);  n++;
    XtSetValues (Text1, args, n);
}

void TranslationProc(w, ev, p, np)
Widget w;
XEvent *ev;
String *p;
Cardinal *np;
{

#ifndef BESTCASE
    XmProcessTraversal(ScrolledWindow2, XmTRAVERSE_CURRENT);
#endif  /* BESTCASE */

    printf("Translation successfully set.\n");
}

static XtActionsRec actions[] =
{
    {"TranslationProc", TranslationProc}
};


void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;
    Pixel         colorVal;
    Dimension	  height, width;
    char	  *value = "This is a long string which will test the         \nscrolling Features of an Application created      \nwith XmNscrollingPolicy set to XmAPPLICTION_      \nDEFINED. It should be contained on four lines     \nwith very long text horizontally. It will not     \nbe editable so dont try to modify any text!!!     ";
    static String translations = "<Key>t: TranslationProc()";
    XtTranslations new_table;

    CommonTestInit(argc, argv);
    
    /* test case for PIR 3180 */
    XtAppAddActions(app_context, actions, XtNumber(actions));

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);  n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC);  n++; 
    XtSetArg(args[n], XmNscrolledWindowMarginHeight, 20);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginWidth, 20);  n++;
    XtSetArg(args[n], XmNspacing, 10);  n++;
    XtSetArg(args[n], XmNscrollBarPlacement, XmTOP_RIGHT);  n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("aquamarine"));  n++;
    ScrolledWindow2 = XmCreateScrolledWindow(Shell1, "ScrolledWindow2", 
        args, n);
    XtManageChild(ScrolledWindow2);
    /* end of test case for PIR 3180 */
  
    /* test case for PIR 2945 */
    n = 0;
    XtSetArg(args[n], XmNclipWindow, &Clip1);  n++;
    XtGetValues(ScrolledWindow2, args, n);
  
    /* add some translations to that widget */
    new_table = XtParseTranslationTable(translations);
    XtOverrideTranslations(Clip1, new_table);
    /* end of test case for PIR 2945 */
    
    XtRealizeWidget(Shell1);

    CommonPause();

    XtDestroyWidget(ScrolledWindow2);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAPPLICATION_DEFINED);  n++;
    XtSetArg(args[n], XmNscrollBarPlacement, XmBOTTOM_RIGHT);  n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("aquamarine"));  n++;
    XtSetArg(args[n], XmNspacing, 10);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginHeight, 20);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginWidth, 20);  n++;
    ScrolledWindow2 = XmCreateScrolledWindow(Shell1, "ScrolledWindow2", 
	args, n);
    XtManageChild(ScrolledWindow2);

    n=0;
    XtSetArg (args[n], XmNbackground, CommonGetColor("green"));  n++;
    DrawingArea1 = XmCreateDrawingArea
		(ScrolledWindow2, "DrawingArea1", args , n);
    XtManageChild (DrawingArea1);

    n = 0;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;
    XtSetArg (args[n], XmNeditable, False);  n++;
    XtSetArg (args[n], XmNrows, 4);  n++;
    XtSetArg (args[n], XmNcolumns, 30);  n++;
    XtSetArg (args[n], XmNwordWrap, False);  n++;
    XtSetArg (args[n], XmNvalue, value);  n++;
    Text1 = XmCreateText(DrawingArea1, "Text1", args, n);
    XtManageChild (Text1);

    CreateScrollBars (ScrolledWindow2);
    XmScrolledWindowSetAreas (ScrolledWindow2, horizSB, vertSB, DrawingArea1);


    CommonPause();
    CommonPause();

    XtDestroyWidget(ScrolledWindow2);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAPPLICATION_DEFINED);  n++;
    XtSetArg(args[n], XmNvisualPolicy, XmCONSTANT);  n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED);  n++;
    XtSetArg(args[n], XmNscrollBarPlacement, XmTOP_RIGHT);  n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("aquamarine"));  n++;
    XtSetArg(args[n], XmNspacing, 10);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginHeight, 20);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginWidth, 20);  n++;
    ScrolledWindow2 = XmCreateScrolledWindow(Shell1, "ScrolledWindow2", 
	args, n);
    XtManageChild(ScrolledWindow2);

    n=0;
    XtSetArg (args[n], XmNbackground, CommonGetColor("green"));  n++;
    DrawingArea1 = XmCreateDrawingArea
		(ScrolledWindow2, "DrawingArea1", args , n);
    XtManageChild (DrawingArea1);

    n = 0;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;
    XtSetArg (args[n], XmNeditable, False);  n++;
    XtSetArg (args[n], XmNrows, 4);  n++;
    XtSetArg (args[n], XmNcolumns, 30);  n++;
    XtSetArg (args[n], XmNwordWrap, False);  n++;
    XtSetArg (args[n], XmNvalue, value);  n++;
    Text1 = XmCreateText(DrawingArea1, "Text1", args, n);
    XtManageChild (Text1);

    CreateScrollBars (ScrolledWindow2);
    XmScrolledWindowSetAreas (ScrolledWindow2, horizSB, vertSB, DrawingArea1);

    CommonPause();

    XtDestroyWidget(ScrolledWindow2);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAPPLICATION_DEFINED);  n++;
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC);  n++;
    XtSetArg(args[n], XmNscrollBarPlacement, XmBOTTOM_LEFT);  n++; 
    XtSetArg(args[n], XmNbackground, CommonGetColor("aquamarine"));  n++;
    XtSetArg(args[n], XmNspacing, 10);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginHeight, 20);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginWidth, 20);  n++;
    ScrolledWindow2 = XmCreateScrolledWindow(Shell1, "ScrolledWindow2", 
	args, n);
    XtManageChild(ScrolledWindow2);

    n=0;
    XtSetArg (args[n], XmNbackground, CommonGetColor("green"));  n++;
    DrawingArea1 = XmCreateDrawingArea
		(ScrolledWindow2, "DrawingArea1", args , n);
    XtManageChild (DrawingArea1);

    n = 0;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;
    XtSetArg (args[n], XmNeditable, False);  n++;
    XtSetArg (args[n], XmNrows, 4);  n++;
    XtSetArg (args[n], XmNcolumns, 30);  n++;
    XtSetArg (args[n], XmNwordWrap, False);  n++;
    XtSetArg (args[n], XmNvalue, value);  n++;
    Text1 = XmCreateText(DrawingArea1, "Text1", args, n);
    XtManageChild (Text1);

    CreateScrollBars (ScrolledWindow2);
    XmScrolledWindowSetAreas (ScrolledWindow2, horizSB, vertSB, DrawingArea1);

    CommonPause();

    XtDestroyWidget(ScrolledWindow2);

    n = 0;
    XtSetArg(args[n], XmNscrollingPolicy, XmAUTOMATIC);  n++; 
    XtSetArg(args[n], XmNscrollBarDisplayPolicy, XmSTATIC);  n++; 
    XtSetArg(args[n], XmNscrollBarPlacement, XmBOTTOM_LEFT);  n++;
    XtSetArg(args[n], XmNbackground, CommonGetColor("aquamarine"));  n++;
    XtSetArg(args[n], XmNspacing, 10);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginHeight, 20);  n++;
    XtSetArg(args[n], XmNscrolledWindowMarginWidth, 20);  n++;
    ScrolledWindow2 = XmCreateScrolledWindow(Shell1, "ScrolledWindow2", 
	args, n);
    XtManageChild(ScrolledWindow2);

    n=0;
    XtSetArg (args[n], XmNbackground, CommonGetColor("green"));  n++;
    DrawingArea1 = XmCreateDrawingArea
		(ScrolledWindow2, "DrawingArea1", args , n);
    XtManageChild (DrawingArea1);

    n = 0;
    XtSetArg (args[n], XmNeditMode, XmMULTI_LINE_EDIT);  n++;
    XtSetArg (args[n], XmNeditable, False);  n++;
    XtSetArg (args[n], XmNrows, 4);  n++;
    XtSetArg (args[n], XmNcolumns, 30);  n++;
    XtSetArg (args[n], XmNwordWrap, False);  n++;
    XtSetArg (args[n], XmNvalue, value);  n++;
    Text1 = XmCreateText(DrawingArea1, "Text1", args, n);
    XtManageChild (Text1);

/*    CreateScrollBars (ScrolledWindow2); */
    XmScrolledWindowSetAreas (ScrolledWindow2, NULL, NULL, DrawingArea1);

    CommonPause();

    XtAppMainLoop(app_context);
}
