/* $XConsortium: IconGadget1.c /main/5 1995/07/15 21:03:51 drk $ */
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
 * 
 */
/*
 * HISTORY
 */

#include <testlib.h>

typedef XtPointer (*ConvertProc)(String value);
static Widget	        CreateOptionMenu(Widget parent, Cardinal j);
static Widget	        CreateResLine(Widget parent, Cardinal j);
static Widget	        CreatePushButton(Widget parent, Cardinal j, 
					 Cardinal k);
static void		CreateControlPanel(void );
static void		ApplyCB(Widget wid, XtPointer client_data, 
				XtPointer call_data);
static XtPointer        LabelStringConvert(String  value);
static XtPointer        IconPixmapConvert(String  value);
static XtPointer        IconMaskConvert(String  value);
static XtPointer        DetailConvert(String  value);
static XtPointer        CardinalConvert(String  value);
static XtPointer        ViewTypeConvert(String  value);
static XtPointer        VisualEmphasisConvert(String  value);
static void             ProcessIcon(Cardinal i);
static void             ProcessRes(Cardinal i, Cardinal j);

#define NUM_ICONG 3
static Widget IconGad[NUM_ICONG];
static Widget *IconRes;
static Widget IconDA;
static Widget IconTog[NUM_ICONG];

static char *DetailString[3] = {
    "First detail ",
    "Second detail ",
    "Last detail"
};

typedef struct _IconResInfo {
    String name ;
    String * values ;
    Cardinal num_values ;
    ConvertProc convert ;
} IconResInfo ;

static String LabelStringValues[] = {
    "A",
    "Short",
    "Longer label",
    "How about something longer but useless"
};

static String IconPixmapValues[] = {
    "large.xpm",
    "small.xpm",
    "large.xbm",
    "small.xbm",
    "UNSPECIFIED"
};

static String IconMaskValues[] = {
    "large_m.xpm",
    "small_m.xpm",
    "round.xbm",
    "none"
};

static String DetailValues[] = {
    "1 detail",
    "2 details",
    "3 details"
};

static String DetailCountValues[] = {
    "0",
    "1",
    "2",
    "3"
};

static String ShadowThicknessValues[] = {
    "0",
    "1",
    "2",
    "5",
    "10"
};

static String ViewTypeValues[] = {
    "XmLARGE_ICON",
    "XmSMALL_ICON"
};

static String VisualEmphasisValues[] = {
    "XmNOT_SELECTED",
    "XmSELECTED"
};

static IconResInfo icon_res_info[] = {
    {
	XmNlabelString,
	LabelStringValues,
	XtNumber(LabelStringValues),
	LabelStringConvert
    },
    {
	XmNlargeIconPixmap,
	IconPixmapValues,
	XtNumber(IconPixmapValues),
	IconPixmapConvert
    },
    {
	XmNlargeIconMask,
	IconMaskValues,
	XtNumber(IconMaskValues),
	IconMaskConvert
    },
    {
	XmNsmallIconPixmap,
	IconPixmapValues,
	XtNumber(IconPixmapValues),
	IconPixmapConvert
    },
    {
	XmNsmallIconMask,
	IconMaskValues,
	XtNumber(IconMaskValues),
	IconMaskConvert
    },
    {
	XmNdetail,
	DetailValues,
	XtNumber(DetailValues),
	DetailConvert
    },
    {
	XmNdetailCount,
	DetailCountValues,
	XtNumber(DetailCountValues),
	CardinalConvert
    },
    {
	XmNshadowThickness,
	ShadowThicknessValues,
	XtNumber(ShadowThicknessValues),
	CardinalConvert
    },
    {
	XmNviewType,
	ViewTypeValues,
	XtNumber(ViewTypeValues),
	ViewTypeConvert
    },
    {	
	XmNvisualEmphasis,
	VisualEmphasisValues,
	XtNumber(VisualEmphasisValues),
	VisualEmphasisConvert
    }
};

static Cardinal NumIconRes = XtNumber(icon_res_info);

void
main(int argc, char **argv)
{

	int		i;
	Cardinal	n;
	Arg		args[MAX_ARGS];
	Widget          icon_gadget;

	CommonTestInit(argc, argv);

	n = 0 ;
	IconDA = XmCreateDrawingArea(Shell1, "IconGadget1", args, n);
	XtManageChild(IconDA);

	for (i=0; i < NUM_ICONG; i++) {
	    char icon_name[20];

	    sprintf(icon_name, "IconGad%d", i+1);
	    IconGad[i] = XmCreateIconGadget(IconDA, icon_name, NULL, 0);
	}

	XtManageChildren(IconGad, NUM_ICONG);

	CreateControlPanel();

	XtRealizeWidget(Shell1);

	CommonPause();
	CommonPause();
	CommonPause();
	CommonPause();
	CommonPause();

	icon_gadget = XmObjectAtPoint(IconDA, 50, 200);

	if (icon_gadget != NULL) {
	    printf("\nThe following Icon Gadget is located at 50 200: ");
	    printf("%s \n\n", XtName(icon_gadget));
	}
	else
	    printf("\nNo Icon Gadget is located at 50 200\n\n");

	CommonPause();

	XtAppMainLoop(app_context);

}


static void
CreateControlPanel(void )
{

   Arg		args[MAX_ARGS];
   Cardinal	n, i, j;
   Widget	PopupShell, FormDialog,
                RcRes, RcCommand, Apply, RcButtons, frame;


   n = 0;
   XtSetArg(args[n], XtNgeometry, "=+50+300"); n++;
   XtSetArg(args[n], XtNallowShellResize, True); n++;
   PopupShell = XtCreatePopupShell("Icon Gadget Resources", 
                                    topLevelShellWidgetClass, Shell1, 
                                    args, n);

   n = 0;
   FormDialog = XmCreateForm(PopupShell, "FormDialog", args, n);
   XtManageChild(FormDialog);

   n = 0;
   XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
   XtSetArg(args[n], XmNpacking, XmPACK_COLUMN); n++;
   RcRes = XmCreateRowColumn(FormDialog, "RcRes", args, n);
   XtManageChild(RcRes);

   IconRes = (Widget *) XtMalloc(NumIconRes * sizeof(Widget));
   for (j=0; j< NumIconRes; j++) {
       IconRes[j] = CreateResLine(RcRes, j);
   }

   n = 0;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
   XtSetArg(args[n], XmNtopWidget, RcRes); n++;
   RcCommand = XmCreateRowColumn(FormDialog, "RcCommand", args, n);
   XtManageChild(RcCommand);

   n = 0;
   Apply = XmCreatePushButton(RcCommand, "Apply", args, n);
   XtManageChild(Apply);
   XtAddCallback(Apply, XmNactivateCallback, ApplyCB, NULL);

   n = 0;
   XtSetArg(args[n], XmNshadowType, XmSHADOW_IN); n++;
   frame = XmCreateFrame(RcCommand, "frame", args, n);
   XtManageChild(frame);

   n = 0;
   XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
   RcButtons = XmCreateRowColumn(frame, "RcButtons", args, n);
   XtManageChild(RcButtons);

   for (i=0; i< NUM_ICONG; i++) {
       char     widget_name[20];
       char     name[20];
       XmString tcs;

       sprintf(widget_name, "TB_IconGad%d", i+1);
       sprintf(name, "IconGad%d", i+1);
       tcs = XmStringGenerate(name, XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, 
			      NULL);

       n = 0;
       XtSetArg(args[n], XmNlabelString, tcs); n++;
       IconTog[i] = XmCreateToggleButton(RcButtons, widget_name, args, n);
       XmStringFree(tcs);
   }
   XtManageChildren(IconTog, NUM_ICONG);

   XtPopup(PopupShell, XtGrabNone);
}

static Widget
CreateResLine(Widget parent, Cardinal j)

{
    Widget      RcResLine, TogSet, ResPulldown, OptRes;
    Arg		args[MAX_ARGS];
    Cardinal	n, k;
    char        toggle_name[30];
    XmString    tcs;
    
    n = 0;
    XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
    RcResLine = XmCreateRowColumn(parent, "RcResLine", args, n);
    XtManageChild(RcResLine);

    tcs = XmStringGenerate("", XmFONTLIST_DEFAULT_TAG, XmCHARSET_TEXT, 
			   NULL);
    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    XtSetArg(args[n], XmNindicatorOn, XmINDICATOR_CHECK_BOX); n++;
    XtSetArg(args[n], XmNindicatorSize, 20); n++;

    /* Name of the toggle button has the form "TogSet_resourceName" */

    sprintf(toggle_name, "TogSet_%s", icon_res_info[j].name);
    TogSet = XmCreateToggleButton(RcResLine, toggle_name, args, n);
    XtManageChild(TogSet);
    XmStringFree(tcs);

    n = 0;
    ResPulldown = XmCreatePulldownMenu(RcResLine, "ResPulldown", args, n);

    for (k=0; k< icon_res_info[j].num_values; k++) {
	Widget pushb ;
	pushb = CreatePushButton(ResPulldown, j, k);
	XtSetArg(args[0], XmNuserData, k);
	XtSetValues(pushb, args, 1);
   }

    OptRes = CreateOptionMenu(RcResLine, j);

    n = 0;
    XtSetArg(args[n], XmNsubMenuId, ResPulldown); n++;
    XtSetValues(OptRes, args, n);
    
    return RcResLine ;
}


static void
ApplyCB(Widget wid, XtPointer client_data, XtPointer call_data)
{
    Cardinal i ;

    for (i=0; i < NUM_ICONG; i++) {
	if (XmToggleButtonGetState(IconTog[i]))
	    ProcessIcon(i);
    }    
}

static void
ProcessIcon(Cardinal i)

{
    Cardinal j ;

    for (j=0; j < NumIconRes; j++) {
	ProcessRes(i, j);
    }

}

static void
ProcessRes(Cardinal i, Cardinal j)

{
    Widget TogSet, OptRes, cur_pushb ;
    XtPointer user_data, cur_val ;
    int cur_k ;
    Arg args[1] ;
    char name[30];

    /* if the TogSet is On, process */
    sprintf(name, "TogSet_%s", icon_res_info[j].name);
    TogSet = XtNameToWidget(IconRes[j], name);
    
    if (!XmToggleButtonGetState(TogSet)) return ;

    /* find the current value in the option */
    sprintf(name, "OM_%s", icon_res_info[j].name);
    OptRes = XtNameToWidget(IconRes[j], name);
    
    XtVaGetValues(OptRes, XmNmenuHistory, &cur_pushb, NULL);
    
    XtVaGetValues(cur_pushb, XmNuserData, &user_data, NULL);
    cur_k = (int) user_data ;

    cur_val = icon_res_info[j].convert(icon_res_info[j].values[cur_k]) ;

    XtSetArg(args[0], icon_res_info[j].name, cur_val);
    XtSetValues(IconGad[i], args, 1);
}

static Widget
CreatePushButton(Widget parent, Cardinal j, Cardinal k)

{

    Widget          widget;
    register int    n;
    Arg             args[MAX_ARGS];
    XmString        tcs;
    char            name[25];

    /* Name of a push button in the pulldown has the form "resourcePBnumber" 
       where resource is the resource name, and number is the rang in the
       resource value array */

    sprintf(name, "%sPB%d", icon_res_info[j].name, k);

    tcs = XmStringGenerate(icon_res_info[j].values[k], XmFONTLIST_DEFAULT_TAG,
			   XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    widget = XmCreatePushButton(parent, name, args, n);
    XtManageChild(widget);

    XmStringFree(tcs);

    return(widget);

}

static Widget
CreateOptionMenu(Widget parent, Cardinal j)

{

    Widget          widget;
    register int    n;
    Arg             args[MAX_ARGS];
    XmString        tcs;
    char            name[25];

    /* Name of the Option Menu has the form "OM_resourceName" */

    sprintf(name, "OM_%s", icon_res_info[j].name);

    tcs = XmStringGenerate(icon_res_info[j].name, XmFONTLIST_DEFAULT_TAG, 
			   XmCHARSET_TEXT, NULL);

    n = 0;
    XtSetArg(args[n], XmNlabelString, tcs); n++;
    widget = XmCreateOptionMenu(parent, name, args, n);
    XtManageChild(widget);

    XmStringFree(tcs);

    return(widget);

}

static XtPointer 
LabelStringConvert(String value)

{
    XmString s ;

    s = XmStringGenerate(value, NULL, XmCHARSET_TEXT, NULL);

    return (XtPointer) s ;
}


static XtPointer 
IconPixmapConvert(String value)

{
    Pixmap    p;
    Pixel     foreground, background;
    Arg       args[3];
    int       n;

    if (strcmp(value, "UNSPECIFIED") == 0)
	return (XtPointer) XmUNSPECIFIED_PIXMAP;

    n = 0;
    XtSetArg (args[n], XmNforeground, &foreground);       n++;
    XtSetArg (args[n], XmNbackground, &background);       n++;
    XtGetValues (IconDA, args, n);

    p = XmGetPixmap(XtScreen(Shell1), value, foreground, background);

    return (XtPointer) p ;
}


static XtPointer 
IconMaskConvert(String value)

{
    Pixmap    p;

    if (strcmp(value, "none") == 0)
	return (XtPointer) 0;

    p = XmGetPixmapByDepth(XtScreen(Shell1), value, 1, 0, 1);

    return (XtPointer) p ;
}


static XtPointer 
DetailConvert(String value)

{
    XmStringTable st;
    int           num_details, i;
    

    if (strcmp(value, DetailValues[0]) == 0)
	num_details = 1;
    if (strcmp(value, DetailValues[1]) == 0)
	num_details = 2;
    if (strcmp(value, DetailValues[2]) == 0)
	num_details = 3;

    st = (XmStringTable) XtMalloc (num_details * sizeof(XmString));

    for (i = 0; i < num_details; i++)
	st[i] = XmStringGenerate(DetailString[i], NULL, XmCHARSET_TEXT, NULL);
    
    return (XtPointer)st ;
}


static XtPointer 
CardinalConvert(String value)

{
    Dimension d ;
    XtPointer return_value;

    d = atoi(value);

    return_value = (XtPointer) ((int) d);

    return (return_value);
}


static XtPointer 
ViewTypeConvert(String value)

{
    unsigned char c;
    XtPointer return_value;

    if (strcmp(value, "XmLARGE_ICON") == 0)
	c = XmLARGE_ICON;
    if (strcmp(value, "XmSMALL_ICON") == 0)
	c = XmSMALL_ICON;

    return_value = (XtPointer) ((int) c);

    return (return_value);
}

static XtPointer 
VisualEmphasisConvert(String value)

{
    unsigned char c;
    XtPointer return_value;

    if (strcmp(value, "XmNOT_SELECTED") == 0)
	c = XmNOT_SELECTED;
    if (strcmp(value, "XmSELECTED") == 0)
	c = XmSELECTED;

    return_value = (XtPointer) ((int) c);

    return (return_value);
}
