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
static char rcsid[] = "$XConsortium: SelectBox1.c /main/9 1995/07/13 19:19:18 drk $"
#endif
#endif

#include <testlib.h>

#define NITEMS 62
#define VITEMS 20

char *Istrings[] = {
    "btest1a", "btest1b", "btest2", "btest3",
    "decor00", "decor01", "decor02", "decor03", "decor04", "decor05",
    "decor06", "decor07", "decor08", "decor09",
    "func00", "func01", "func02", "func03", "func04",
    "func05", "func06", "func07", "func08", "func09",
    "func10", "func11", "func12", "func13", "func14",
    "iplac00", "iplac01", "iplac02", "iplac03", "iplac04",
    "iplac05", "iplac06", "iplac07", "iplac08",
    "menu00", "menu01", "menu02", "menu03",
    "menu04", "menu05", "menu06", "menu07",
    "mtest1", "mtest2", "mvrz00", "mvrz01",
    "parse00", "parse01", "parse02", "parse03", "pitest1",
    "prop00", "prop01", "prop02", "prop03", "prop04", "prop05",
    "rstest1"
};

Widget ScaleDialog;
Widget ScaleWidth, ScaleColumns;

void 		      UpdateScale();
static void ApplyToSelectionBox();
static void KillYourself();
static void PostDialog();
void 		      GetResources ();

Widget    	SelectionBox1;

static void  noMatchCB();
static void  okCB();
static void  applyCB();
static void  cancelCB();

void  main(argc, argv)
    int     argc;
    char  **argv;
{
    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs, tcs2, tcs3, tcs4, ItemList[NITEMS];
    int 	  i;
    Widget	  help_child;

    CommonTestInit(argc, argv);
    
    n = 0;
    XtSetArg(args[n], XmNwidth,  10);  			n++;
    XtSetArg(args[n], XmNheight, 10);  			n++;
    XtSetValues(Shell1, args, n);
  
    XtRealizeWidget(Shell1);

    tcs = XmStringLtoRCreate("test: ", XmSTRING_DEFAULT_CHARSET);
    tcs2 = XmStringLtoRCreate("Pick a Test", XmSTRING_DEFAULT_CHARSET);

    tcs3 = XmStringCreate("MyDialogTitle", "iso8859-1");

    n = 0;
    tcs4 = XmStringCreateLtoR ("prop00", XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[n], XmNselectionLabelString, tcs);  		n++;
    XtSetArg(args[n], XmNautoUnmanage, False);  		n++;
    XtSetArg(args[n], XmNlistLabelString, tcs2); 		n++;
    XtSetArg(args[n], XmNmustMatch, True);  			n++;
    XtSetArg(args[n], XmNdialogTitle, tcs3);			n++;
    XtSetArg(args[n], XmNtextString, tcs4 );                    n++;
    SelectionBox1 = XmCreateSelectionDialog(Shell1, "SelectionBox1", args, n);

    XtManageChild(XmSelectionBoxGetChild(SelectionBox1,XmDIALOG_APPLY_BUTTON));

    /* Convert the items to Xm strings */
    for (n = 0; n < NITEMS; n++)
        ItemList[n] = XmStringCreateLtoR((char *)Istrings[n],
					XmSTRING_DEFAULT_CHARSET);

    /* Add 'em to the SelectionBox */
    n = 0;
    XtSetArg(args[n], XmNlistItems, ItemList); 			n++;
    XtSetArg(args[n], XmNlistItemCount, NITEMS); 		n++;
    XtSetArg(args[n], XmNlistVisibleItemCount, VITEMS); 	n++;
    XtSetValues(SelectionBox1, args, n);

    XtManageChild(SelectionBox1);

    XmStringFree(tcs);
    XmStringFree(tcs2);
    XmStringFree(tcs4);

    XtAddCallback(SelectionBox1, XmNnoMatchCallback, noMatchCB, NULL);
    XtAddCallback(SelectionBox1, XmNokCallback, okCB, NULL);
    XtAddCallback(SelectionBox1, XmNapplyCallback, applyCB, NULL);
    XtAddCallback(SelectionBox1, XmNcancelCallback, cancelCB, NULL);

    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();
    CommonPause();

    help_child = XmMessageBoxGetChild(InstructionBox, XmDIALOG_HELP_BUTTON);

    tcs = XmStringCreateSimple("Measure");

    n = 0; 
    XtSetArg(args[n], XmNsensitive, True);			n++;
    XtSetArg(args[n], XmNlabelString, tcs);			n++;
    XtSetValues(help_child, args, n);

    GetResources();

    XtAddCallback(help_child, XmNactivateCallback, PostDialog, NULL);

    XmStringFree(tcs);
 
    CommonPause(); /* measure */

    /* unmanage text and make all button children disappear on activate */

    XtUnmanageChild(XmSelectionBoxGetChild(SelectionBox1, XmDIALOG_TEXT));

    XtAddCallback(XmSelectionBoxGetChild(SelectionBox1, XmDIALOG_APPLY_BUTTON),
		  XmNactivateCallback, KillYourself, NULL);
    XtAddCallback(XmSelectionBoxGetChild(SelectionBox1, XmDIALOG_CANCEL_BUTTON),
		  XmNactivateCallback, KillYourself, NULL);
    XtAddCallback(XmSelectionBoxGetChild(SelectionBox1, XmDIALOG_DEFAULT_BUTTON)
		  , XmNactivateCallback, KillYourself, NULL);
    XtAddCallback(XmSelectionBoxGetChild(SelectionBox1, XmDIALOG_HELP_BUTTON),
		  XmNactivateCallback, KillYourself, NULL);

    CommonPause(); /* more measure without Text */

    XtAppMainLoop(app_context);
}


static void KillYourself (w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;

{
    XtUnmanageChild (w);
}


void GetResources ()
{
    short 	col;
    Dimension 	width;
    int 	n;
    Arg 	args[10];
    XmString    tcs;

    Widget ScaleOK;

    static char *scaleWidgets[] = { "ScaleWidth",
                    		    "ScaleColumns",
		  		    (char *) NULL
		  		   };

    n = 0;
    XtSetArg (args[n], XmNautoUnmanage, False);				n++;
    XtSetArg (args[n], XmNdefaultPosition, False);			n++;
    XtSetArg (args[n], XmNx, 350);					n++;
    XtSetArg (args[n], XmNy, 350);					n++;
    ScaleDialog = XmCreateBulletinBoardDialog(SelectionBox1, "ScaleDialog",
					      args, n);

    n = 0;
    tcs = XmStringCreateSimple ("Width"); 
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL); 			n++;
    XtSetArg (args[n], XmNshowValue, True); 				n++;
    XtSetArg (args[n], XmNminimum, 1); 					n++;
    XtSetArg (args[n], XmNmaximum, 600); 				n++;
    XtSetArg (args[n], XmNtitleString, tcs ); 				n++;
    XtSetArg (args[n], XmNy, 10); 					n++;
    XtSetArg (args[n], XmNx, 10); 					n++;
    ScaleWidth = XmCreateScale (ScaleDialog, "ScaleWidth", args, n);
    XmStringFree(tcs);

    n = 0;
    tcs = XmStringCreateSimple("Columns");
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL); 			n++;
    XtSetArg (args[n], XmNshowValue, True); 			 	n++;
    XtSetArg (args[n], XmNminimum, 1); 					n++;
    XtSetArg (args[n], XmNmaximum, 132); 				n++;
    XtSetArg (args[n], XmNtitleString, tcs );				n++; 
    XtSetArg (args[n], XmNy, 100); 					n++;
    XtSetArg (args[n], XmNx, 10); 					n++;
    ScaleColumns = XmCreateScale (ScaleDialog, "ScaleColumns", args, n);
    XmStringFree(tcs);

    n = 0;
    tcs = XmStringCreateSimple ("OK"); 	
    XtSetArg (args[n], XmNx, 100); 					n++;
    XtSetArg (args[n], XmNy, 200); 					n++;
    XtSetArg (args[n], XmNlabelString, tcs );			 	n++;
    ScaleOK = XmCreatePushButtonGadget (ScaleDialog, "ScaleOK", args, n);
    XmStringFree(tcs);

    XtAddCallback 
        (ScaleOK, XmNactivateCallback, ApplyToSelectionBox, scaleWidgets);
    
    XtManageChild (ScaleOK);
    XtManageChild (ScaleWidth);
    XtManageChild (ScaleColumns);
}


void UpdateScale (scale, value)
    Widget scale;
    int value;
{
    Arg args[2];
    int min, max;
    int n;
 
    n = 0;
    XtSetArg (args[n], XmNminimum, &min); 		n++;
    XtSetArg (args[n], XmNmaximum, &max); 		n++;
    XtGetValues (scale, args, n);

    /* fix up end cases first */
    if (value < min) value = min;
    if (value > max) value = max;

    XtSetArg (args[0], XmNvalue, value);
    XtSetValues (scale, args, 1);

}

static void PostDialog(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    register int n;
    Arg args[10];
    short 	col;
    Dimension 	width;

    /* set the scales to the current values of the selection box */

    n = 0;
    XtSetArg (args[n], XmNtextColumns, &col) ; 				n++;
    XtSetArg (args[n], XmNwidth, &width); 				n++;
    XtGetValues (SelectionBox1, args, n);
    
    UpdateScale (ScaleColumns, col);
    UpdateScale (ScaleWidth, width);

    XtManageChild(ScaleDialog);

}

static void ApplyToSelectionBox(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    Arg args[1];
    int n;
    int value;
    Widget whichScale;

    char **widgetList;

   widgetList = (char **) client_data;


   while (*widgetList != NULL) {
    	whichScale = XtNameToWidget(ScaleDialog, *widgetList);
    	if (whichScale) {
      		n = 0;
      		XtSetArg (args[n], XmNvalue, &value); 		n++;
      		XtGetValues (whichScale, args, n);
      		if (strcmp(*widgetList, "ScaleWidth") == 0) {
        		n = 0;
        		XtSetArg (args[n], XmNwidth, value); 	n++;
        		XtSetValues (SelectionBox1, args, n);
      		} 
		else 
			if (strcmp(*widgetList, "ScaleColumns") == 0) {
        			n = 0;
        			XtSetArg (args[n], XmNtextColumns, value); n++;
        			XtSetValues (SelectionBox1, args, n);
      			} 
    	} /*if whichScale */
   	widgetList++;
    } /* while */

    XtUnmanageChild(XtParent(w));

} /* end routine */


static void  noMatchCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    char  *text;


    XmStringGetLtoR(((XmSelectionBoxCallbackStruct *)call_data)->value,
                    XmSTRING_DEFAULT_CHARSET, &text);
    printf("noMatch:  %s\n", text);
    XtFree(text);
}



static void  okCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    char  *text;


    XmStringGetLtoR(((XmSelectionBoxCallbackStruct *)call_data)->value,
                    XmSTRING_DEFAULT_CHARSET, &text);
    printf("ok:  %s\n", text);
    XtFree(text);
}



static void  applyCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    char  *text;


    XmStringGetLtoR(((XmSelectionBoxCallbackStruct *)call_data)->value,
                    XmSTRING_DEFAULT_CHARSET, &text);
    printf("apply:  %s\n", text);
    XtFree(text);
}


static void  cancelCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    char  *text;


    XmStringGetLtoR(((XmSelectionBoxCallbackStruct *)call_data)->value,
                    XmSTRING_DEFAULT_CHARSET, &text);
    printf("cancel:  %s\n", text);
    XtFree(text);
}
