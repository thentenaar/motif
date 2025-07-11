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
static char rcsid[] = "$XConsortium: DNDDrop3.c /main/8 1995/07/14 11:52:47 drk $"
#endif
#endif

/*
 *	file: DNDDrop3.c
 *
 *	This tests non-rectangular dropSites, i.e., dropSites that have to be
 *	represented by more that one rectangle.
 */

#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xm/TextP.h>
#include <Xm/TextFP.h>
#include <Xm/FormP.h>
#include <Xm/PushBP.h>
#include <Xm/RowColumn.h>
#include <Xm/LabelP.h>
#include <Xm/DropSMgrP.h>
#include <Xm/DragCP.h>
#include <Xm/AtomMgr.h>


static void StartMyDragProc();
static void RaiseProc();
static void LowerProc();


XRectangle rects0[] = {
	{  40,   0,  40,  40 },
	{   0,  40, 120,  40 },
	{  40,  80,  40,  40 },
};

XRectangle rects1[] = {
	{   0,   0,  40,  40 },
	{  80,   0,  40,  40 },
	{   0,  40, 120,  40 },
};

XRectangle rects2[] = {
	{  20,   0,  40,  40 },
	{   0,  40,  80,  20 },
};

XRectangle rects3[] = {
	{   0,   0,  40,  60 },
	{  40,  20,  40,  20 },
};

typedef struct _DropZoneRec {
	char	*name;
	XRectangle *rects;
	int		num_rects;
	Region	region;
	Widget	refW;
	Widget	icon;
} DropZoneRec, *DropZone;

static DropZoneRec DropZones[] = {
	{ "cross", rects0, XtNumber(rects0), NULL, NULL, NULL},
	{ "trough", rects1, XtNumber(rects1), NULL, NULL, NULL},
	{ "MwmIcon", rects2, XtNumber(rects2), NULL, NULL, NULL},
	{ "mallet", rects3, XtNumber(rects3), NULL, NULL, NULL},
};

static int numZones = XtNumber(DropZones);

static String myDNDTranslations = 
	" <Btn2Down> :  StartMyDrag() \n\
	 m <Btn1Up>  :  Raise(1)        \n\
	   <Btn1Up>  :  Raise(0)        \n\
	 m <Btn1Up>  :  Lower(1)        \n\
	   <Btn1Up>  :  Lower(0)";


static XtActionsRec myDNDActions[] = {
	{ "StartMyDrag", StartMyDragProc },
	{ "Raise", RaiseProc },
	{ "Lower", LowerProc },
};


Widget shell1;
Widget form1;
Widget p0, p1, p2, p3;
Widget text1;
int n = 0;
Arg args[30];
XtAppContext this_app;
Display *display;
int i;


Pixel GetPixel(dpy, name)
Display  *dpy;
char     *name;
{

	XColor color, exact_color;
	int           scr = DefaultScreen(dpy);
	Colormap      cmap = DefaultColormap(dpy, scr);

	XAllocNamedColor(dpy, cmap, name, &color, &exact_color);

	return(color.pixel);
}

void ExposeEventHandler(w, client_data, event, continue_to_dispatch)
Widget w;
XtPointer client_data;
XEvent *event;
Boolean *continue_to_dispatch;
{
	static Boolean first_time = True;
	XGCValues values;
	unsigned long  valueMask;
	static GC dzGC;
	Pixel zoneColor;
	int i, j;
	Region clipRegion = NULL;
	Region sibRegion = NULL;

	*continue_to_dispatch = True;

	if (first_time)
	{
		valueMask = GCGraphicsExposures;
		values.graphics_exposures = False;
		dzGC = XCreateGC(XtDisplay(w), XtWindow(w), valueMask, &values);

		clipRegion = XCreateRegion();
		sibRegion = XCreateRegion();
	}

	for (i=0; i < XtNumber(DropZones); i++)
	{
		valueMask = (GCForeground | GCBackground);
		values.background = DropZones[i].refW->core.background_pixel;
		values.foreground = DropZones[i].refW->core.background_pixel;
		XChangeGC(XtDisplay(w), dzGC, valueMask, &values);

		XUnionRegion(DropZones[i].region, DropZones[i].region,
			clipRegion);
		XOffsetRegion(clipRegion, XtX(w), XtY(w));

		for (j=0; j < i ; j++)
		{
			XUnionRegion(DropZones[j].region, DropZones[j].region,
				sibRegion);
			XOffsetRegion(sibRegion, XtX(DropZones[j].refW),
				XtY(DropZones[j].refW));
			XSubtractRegion(clipRegion, sibRegion, clipRegion);
		}

		XSetRegion(XtDisplay(w), dzGC, clipRegion);

		XFillRectangle(XtDisplay(w), XtWindow(w), dzGC, 
			XtX(w), XtY(w), XtWidth(w), XtHeight(w));
	}
}

static void RaiseProc(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{

	int	operation;
	if (*num_params)
	{
		/* We only expect one */
		operation = atoi(params[0]);
	}

	switch(operation)
	{
		case 1:
		{
			/* Raise to top of stack */
		}
		break;

		case 0:
		default:
		{
			/* Circulate the stack upwards */
		}
		break;
	}
}

static void LowerProc(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
	
}

static void StartMyDragProc(w, event, params, num_params)
Widget w;
XEvent *event;
String *params;
Cardinal *num_params;
{
	Arg args[30];
	int n, i;
	XButtonEvent *myEvent = (XButtonEvent *) event;
	Widget dc;

	/* detect which, if any drop zone the drag started in */
	for (i=0; i < numZones; i++)
		if (XPointInRegion(DropZones[i].region, myEvent->x, myEvent->y))
			break;
	
	/* if in a drop zone, load the appropriate cursor */

	if (i == numZones)
		return;
	
	n = 0;
	XtSetArg(args[n], XmNsourceCursorIcon, DropZones[i].icon); n++;
	dc = XmDragStart(w, event, args, n);
}

static void MyDropProc(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;
{
	XmDropProcCallbackStruct *cb =
		(XmDropProcCallbackStruct *) call_data;
	Position newX = cb->x - XtX(w);
	Position newY = cb->y - XtY(w);
	Arg args[5];
	int n;
	Boolean junk;

	n = 0;
	XtSetArg(args[n], XmNx, newX); n++;
	XtSetArg(args[n], XmNy, newY); n++;
	XtSetValues(w, args, n);

	ExposeEventHandler(w, NULL, NULL, &junk);
}

void CreateDropZone(w, targets, num_targets, dz)
Widget w;
Atom *targets;
Cardinal num_targets;
DropZone dz;
{
	Display *display = XtDisplay(w);
	XRectangle clipBox;
	unsigned long vMask;
	XGCValues values;
	Pixmap iconMap, maskMap;
	GC clipGC;
	static Boolean first_time = True;
	static Region r1, r2;
	unsigned long  valueMask;
	char buf[128];

	if (first_time)
	{
		r1 = XCreateRegion();
		r2 = XCreateRegion();
		first_time = False;
	}

	dz->refW = w;

	/* Create and set the region from the rectangle data */
	dz->region = XCreateRegion();
	for (i=0; i < dz->num_rects; i++)
		XUnionRectWithRegion(&(dz->rects[i]), dz->region, dz->region);
	
	/* Get the bounding box of the region */
	XClipBox(dz->region, &clipBox);

	n = 0;
	XtSetArg(args[n], XmNwidth, clipBox.width); n++;
	XtSetArg(args[n], XmNheight, clipBox.height); n++;
	XtSetValues(w, args, n);

	/* Create depth 1 pixmaps the size of bounding box */
	iconMap = XCreatePixmap(display, DefaultRootWindow(display),
		clipBox.width, clipBox.height, 1);
	maskMap = XCreatePixmap(display, DefaultRootWindow(display),
		clipBox.width, clipBox.height, 1);

	/* Fill the bitmaps with 0's */
	vMask = (GCForeground | GCBackground);
	values.background = 0;
	values.foreground = 0;
	clipGC = XCreateGC(display, iconMap, vMask, &values);

	XFillRectangle(display, iconMap, clipGC, 0, 0, clipBox.width,
		clipBox.height);
	XFillRectangle(display, maskMap, clipGC, 0, 0, clipBox.width,
		clipBox.height);

	/* Now we write with 1's */
	valueMask = (GCForeground | GCBackground);
	values.background = 1;
	values.foreground = 1;
	XChangeGC(display, clipGC, valueMask, &values);
	
	/* Fill the shape of the icon with 1's */
	XUnionRegion(dz->region, dz->region, r1);
	XOffsetRegion(r1, (-1 * clipBox.x), (-1 * clipBox.y));
	XSetRegion(display, clipGC, r1);

	XFillRectangle(display, iconMap, clipGC, 
		0, 0, clipBox.width, clipBox.height);

	/* Create a clip region to shoot the 1's thru into the mask */
	XUnionRegion(r1, r1, r2);
	XShrinkRegion(r2, 2, 2);
	XSubtractRegion(r1, r2, r1);

	XSetRegion(display, clipGC, r1);

	XFillRectangle(display, maskMap, clipGC, 
		0, 0, clipBox.width, clipBox.height);
	
	n = 0;
	XtSetArg(args[n], XmNpixmap, iconMap); n++;
	XtSetArg(args[n], XmNmask, maskMap); n++;
	XtSetArg(args[n], XmNdepth, 1); n++;
	XtSetArg(args[n], XmNwidth, clipBox.width); n++;
	XtSetArg(args[n], XmNheight, clipBox.height); n++;
	XtSetArg(args[n], XmNhotX, (clipBox.width / 2)); n++;
	XtSetArg(args[n], XmNhotY, (clipBox.height / 2)); n++;
	dz->icon = XmCreateDragIcon(w, dz->name, args, n);
	
	n = 0;
	XtSetArg(args[n], XmNimportTargets, targets); n++;
	XtSetArg(args[n], XmNnumImportTargets, num_targets); n++;
	XtSetArg(args[n], XmNdropRectangles, dz->rects); n++;
	XtSetArg(args[n], XmNnumDropRectangles, dz->num_rects); n++;
	XtSetArg(args[n], XmNdropProc, MyDropProc); n++;
	XmDropSiteRegister(w, args, n);
}

main(argc, argv)
int argc;
char **argv;
{
	int i;
	Atom targets[1];
	unsigned short *ID;
	XtTranslations myParsedTranslations;

	XtToolkitInitialize();
	this_app = XtCreateApplicationContext();
	display = XtOpenDisplay(this_app, NULL, NULL, "DNDNonRect",
		NULL, 0, &argc, argv);
	
	if (display == NULL) {
		fprintf(stderr, "%s:  Can't open display\n", argv[0]);
		exit(1);
	}

	targets[0] = XmInternAtom(display, "COMPOUND_TEXT", False);

	XtAppAddActions(this_app, myDNDActions, XtNumber(myDNDActions));

	myParsedTranslations = XtParseTranslationTable(myDNDTranslations);

	n = 0;
	XtSetArg(args[n], XmNallowShellResize, True);  n++;
	XtSetArg(args[n], XmNwidth, 400);  n++;
	XtSetArg(args[n], XmNheight, 300);  n++;
	shell1 = XtAppCreateShell(NULL, "XMclient",
		applicationShellWidgetClass, display, args, n);
	
	n = 0;
	form1 = XmCreateForm(shell1, "form1", args, n);
	XtManageChild(form1);
	XtOverrideTranslations(form1, myParsedTranslations);

	XtAddEventHandler(form1, ExposureMask, True, ExposeEventHandler,
		NULL);

	n = 0;
	XtSetArg(args[n], XmNx, 10); n++;
	XtSetArg(args[n], XmNy, 10); n++;
	XtSetArg(args[n], XmNbackground, GetPixel(display, "DarkGreen"));
		n++;
	p0 = XtCreateWidget("p0", xmPrimitiveWidgetClass, form1, args, n);

	CreateDropZone(p0, targets, XtNumber(targets), &DropZones[0]);

	n = 0;
	XtSetArg(args[n], XmNx, 50); n++;
	XtSetArg(args[n], XmNy, 50); n++;
	XtSetArg(args[n], XmNbackground, GetPixel(display, "Blue")); n++;
	p1 = XtCreateWidget("p1", xmPrimitiveWidgetClass, form1, args, n);

	CreateDropZone(p1, targets, XtNumber(targets), &DropZones[1]);

	n = 0;
	XtSetArg(args[n], XmNbackground, GetPixel(display, "Cyan")); n++;
	p2 = XtCreateWidget("p2", xmPrimitiveWidgetClass, form1, args, n);

	CreateDropZone(p2, targets, XtNumber(targets), &DropZones[2]);

	n = 0;
	XtSetArg(args[n], XmNx, 150); n++;
	XtSetArg(args[n], XmNy, 150); n++;
	XtSetArg(args[n], XmNbackground, GetPixel(display, "Orange")); n++;
	p3 = XtCreateWidget("p3", xmPrimitiveWidgetClass, form1, args, n);

	CreateDropZone(p3, targets, XtNumber(targets), &DropZones[3]);

	n = 0;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_NONE); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	text1 = XmCreateTextField( form1, "text1", args, n);
	XtManageChild(text1);

	XtRealizeWidget(shell1);

	/* MainLoop */
	XtAppMainLoop(this_app);
}

