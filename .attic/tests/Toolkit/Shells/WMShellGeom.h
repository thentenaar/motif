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
/*   $XConsortium: WMShellGeom.h /main/8 1995/07/13 19:21:38 drk $ */

/*
 *	file: WMShellGeometry.h
 */

#define	NUM_SCALES		14
#define	MAX_NAME_LEN	100

/* The order should be maintained always */
#define	BASEHEIGHT	0
#define	BASEWIDTH	BASEHEIGHT+1
#define	HEIGHTINC	BASEWIDTH+1
#define	WIDTHINC	HEIGHTINC+1
#define	ICONX		WIDTHINC+1
#define	ICONY		ICONX+1
#define	MAXASPECTX	ICONY+1
#define	MAXASPECTY	MAXASPECTX+1
#define	MINASPECTX	MAXASPECTY+1
#define	MINASPECTY	MINASPECTX+1
#define	MAXHEIGHT	MINASPECTY+1
#define	MAXWIDTH	MAXHEIGHT+1
#define	MINHEIGHT	MAXWIDTH+1
#define	MINWIDTH	MINHEIGHT+1


struct	scale_info {

	Widget	widget;
	char	titleString[MAX_NAME_LEN + 1];
	int		minValue;
	int		maxValue;

};

struct scale_info	Scales[NUM_SCALES] = {

	{ NULL, "baseHeight", 0, -1 },
	{ NULL, "baseWidth",  0, -1 },
	{ NULL, "heightInc",  1, -1 },
	{ NULL, "widthInc",   1, -1 },
	{ NULL, "iconX",      0, -1 },
	{ NULL, "iconY",      0, -1 },
	{ NULL, "maxAspectX", 1, -1 },
	{ NULL, "maxAspectY", 1, -1 },
	{ NULL, "minAspectX", 1, -1 },
	{ NULL, "minAspectY", 1, -1 },
	{ NULL, "maxHeight",  0, -1 },
	{ NULL, "maxWidth",   0, -1 },
	{ NULL, "minHeight",  0, -1 },
	{ NULL, "minWidth",   0, -1 }

};

Arg	scale_values[NUM_SCALES] = {

	{ XmNbaseHeight, -1 },
	{ XmNbaseWidth,  -1 },
	{ XmNheightInc,  -1 },
	{ XmNwidthInc,   -1 },
	{ XmNiconX,      -1 },
	{ XmNiconY,      -1 },
	{ XmNmaxAspectX, -1 },
	{ XmNmaxAspectY, -1 },
	{ XmNminAspectX, -1 },
	{ XmNminAspectY, -1 },
	{ XmNmaxHeight,  -1 },
	{ XmNmaxWidth,   -1 },
	{ XmNminHeight,  -1 },
	{ XmNminWidth,   -1 }

};

int			screenHeight, screenWidth;
Widget	  	Form;
Widget		WMShell_Resources;
Widget		Frame1;
Widget	  	RowCol1;
Widget		WhichShell_Pulldown;
Widget		WhichShell_Option;
Widget		Application_Shell, Top_Shell, Dialog_Shell, All_Shells;
Widget		applicationShell, topLevelShell, dialogShell;
Widget		application_btn, topLevel_btn;

XmFontList 	font_list;
char	fontName[MAX_NAME_LEN + 1];
Bool	no_adjust = False;

/*  Private Functions  */


static void  	Application_ShellCB(Widget w, XtPointer client_data, 
									XtPointer call_data);
static void  	Top_ShellCB(Widget w, XtPointer client_data, 
						   XtPointer call_data);
static void  	Dialog_ShellCB(Widget w, XtPointer client_data, 
							   XtPointer call_data);
static void  	All_ShellsCB(Widget w, XtPointer client_data, 
							 XtPointer call_data);
	
static void 	CreateWMShellResources(Widget w);

static void 	GetWMShellResources();
static void 	SetWMShellResources(Widget widget);

static Widget  	CreatePushButton(Widget parent, char *label);

