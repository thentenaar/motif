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
static char rcsid[] = "$XConsortium: DNDIcon1.c /main/10 1996/10/07 14:52:58 drk $"
#endif
#endif

/*
 *	file: DNDIcon1.c
 *
 *	This tests the various Icons used during a Drag and Drop transaction.
 *	This tests all the default icons that can be specified as XmScreen 
 *	resources.
 *
 */

#include <signal.h>
#include <stdio.h>

#include <testlib.h>

#include <Xm/Screen.h>
#include <Xm/DragDrop.h>

#include <X11/keysym.h>
#include <X11/Xatom.h>

#include "DNDIcons.h"


#define	MAX_DROPSITES	100


static Widget   CreatePushButton();
static void	 	Quit();
static void	 	StartDrag();


static char translations[] = 
"#override \
  Ctrl<Key>t: XtDisplayTranslations()\n\
  Ctrl<Key>a: XtDisplayAccelerators()\n\
  Ctrl<Key>i: XtDisplayInstalledAccelerators()\n\
  Ctrl<Key>q: Quit()";

static char dragTranslations[] =
"#override \
  Ctrl<Key>t: XtDisplayTranslations()\n\
  Ctrl<Key>a: XtDisplayAccelerators()\n\
  Ctrl<Key>i: XtDisplayInstalledAccelerators()\n\
  <Btn2Down>: StartDrag()\n\
  Ctrl<Key>q: Quit()";

static XtActionsRec myactions[] = {
	{"Quit", (XtActionProc)Quit},
	{"StartDrag", (XtActionProc)StartDrag},
};


typedef struct _XlationRepRec{
	String		str;
	XtTranslations	xlation;
}XlationRepRec, *XlationRep;

typedef struct _AppDataRec{
	XlationRepRec	xlation1;
	XlationRepRec	xlation2;
	Cardinal		numShells, numRows, numCols;
	Boolean		applyAccelerators;
	Boolean		applyOverrides;
}AppDataRec, *AppData;

XtResource	appResources[] = {
	{
	"xlations1", XtCTranslations, XtRString, 
	sizeof(String), XtOffset(AppData, xlation1.str),
	XtRString, (XtPointer)translations,
	},
	{
	"xlations1", XtCTranslations, XtRTranslationTable, 
	sizeof(XtTranslations), XtOffset(AppData, xlation1.xlation),
	XtRString, (XtPointer)translations,
	},
	{
	"xlations2", XtCTranslations, XtRString, 
	sizeof(String), XtOffset(AppData, xlation2.str),
	XtRString, (XtPointer)dragTranslations,
	},
	{
	"xlations2", XtCTranslations, XtRTranslationTable, 
	sizeof(XtTranslations), XtOffset(AppData, xlation2.xlation),
	XtRString, (XtPointer)dragTranslations,
	},
	{
	"applyAccelerators", "ApplyAccelerators", XtRBoolean,
	sizeof(Boolean), XtOffset(AppData, applyAccelerators),
	XtRImmediate, (XtPointer) True,
	},
	{
	"applyOverrides", "ApplyOverrides", XtRBoolean,
	sizeof(Boolean), XtOffset(AppData, applyOverrides),
	XtRImmediate, (XtPointer) True,
	},
	{
	"numShells", "NumShells", XtRCardinal,
	sizeof(Cardinal), XtOffset(AppData, numShells),
	XtRImmediate, (XtPointer) 1,
	},
	{
	"numRows", "NumRows", XtRInt,
	sizeof(Cardinal), XtOffset(AppData, numRows),
	XtRImmediate, (XtPointer) 2,
	},
	{
	"numCols", "NumCols", XtRInt,
	sizeof(Cardinal), XtOffset(AppData, numCols),
	XtRImmediate, (XtPointer) 3,
	},
};


Widget	myDC;
Atom	COMPOUND_TEXT;
Atom	DELETE;
Atom	NULL_ATOM;

Widget	xmDisplay;
Widget	xmScreen;
Widget	defCopyIcon, defMoveIcon, defLinkIcon;
Widget	defVIcon, defInvIcon, defNoneIcon, defSrcIcon;

Boolean	use_instruction_box;

unsigned char	initiatorProtocolStyle;
unsigned char	receiverProtocolStyle;

Widget	Form;

Widget  Whichanim_Pulldown;
Widget  Whichanim_Option;
Widget  DragUnderHighLight, DragUnderShadowOut, DragUnderShadowIn, 
		DragUnderNone, DragUnderPixmap;

Widget  WhichdropAct_Pulldown;
Widget  WhichdropAct_Option;
Widget	dropActive, dropInactive;

Widget  WhichdropOp_Pulldown;
Widget  WhichdropOp_Option;
Widget	dropMoveCopy, dropMoveOnly, dropCopyOnly, dropLinkOnly,
		dropMoveLink, dropCopyLink, dropAll, dropNone;

Widget  WhichdragOp_Pulldown;
Widget  WhichdragOp_Option;
Widget	dragMoveCopy, dragMoveOnly, dragCopyOnly, dragLinkOnly,
		dragMoveLink, dragCopyLink, dragAll, dragNone;

Widget  WhichBlend_Pulldown;
Widget  WhichBlend_Option;
Widget	blendAll, blendStateSource, blendJustSource, blendNone;

Widget  WhichForeground_Pulldown;
Widget  WhichForeground_Option;
Widget	redFG, blackFG, yellowFG, greenFG, whiteFG;

Widget  WhichBackground_Pulldown;
Widget  WhichBackground_Option;
Widget	redBG, blackBG, yellowBG, greenBG, whiteBG;

Widget  WhichValidForeground_Pulldown;
Widget  WhichValidForeground_Option;
Widget	redValid, blackValid, yellowValid, greenValid, whiteValid;

Widget  WhichInvalidForeground_Pulldown;
Widget  WhichInvalidForeground_Option;
Widget	redInvalid, blackInvalid, yellowInvalid, greenInvalid, whiteInvalid;

Widget  WhichNoneForeground_Pulldown;
Widget  WhichNoneForeground_Option;
Widget	redNone, blackNone, yellowNone, greenNone, whiteNone;

Widget  ApplyChanges;

unsigned char	animationStyle;
unsigned char	dropActivity;
unsigned char	dropOperations;
unsigned char	dragOperations;
unsigned char	blendModel;
Pixel			cursorForeground;
Pixel			cursorBackground;
Pixel			ValidcursorForeground;
Pixel			InvalidcursorForeground;
Pixel			NonecursorForeground;

Pixel	redPixel, blackPixel, yellowPixel, greenPixel, whitePixel;

Widget	dropSites[MAX_DROPSITES];
int		numDropSites;

#ifdef DEBUG
FILE	*outfp;
#endif


static void Quit ()
{

	printf("Begin exiting ... please standby ... \n");
#ifdef DEBUG
	fflush(outfp);
	fclose(outfp);
#endif
	exit(0);

}


/*ARGSUSED*/
static Boolean DragConvertProc(w, selection, target, typeRtn, valueRtn, 
							  lengthRtn, formatRtn, max_lengthRtn, 
							  client_data, request_id)
Widget              w;
Atom                *selection;
Atom                *target;
Atom                *typeRtn;
XtPointer           *valueRtn;
unsigned long       *lengthRtn;
int                 *formatRtn;
unsigned long       *max_lengthRtn;
XtPointer			client_data;
XtRequestId			*request_id;
{

	char	*string = "return value";
	int		ok;
	XTextProperty tmp_prop;

	if (*target != COMPOUND_TEXT && *target != DELETE)
		return(False);

	if (*target == COMPOUND_TEXT) {

		ok = 0;
		ok = XmbTextListToTextProperty(display, &string, 1, 
									   XCompoundTextStyle, &tmp_prop);
		if (ok != Success)
			return(False);
		
		*valueRtn = (char *) tmp_prop.value;
		*lengthRtn = tmp_prop.nitems;
		*typeRtn = COMPOUND_TEXT;
		*formatRtn = 8;
		return(True);

	}
	if (*target == DELETE) {
#ifdef DEBUG
		fprintf(outfp, "DELETE target encountered\n");
#endif
		*typeRtn = NULL_ATOM;
		*valueRtn = NULL;
		*lengthRtn = 0;
		*formatRtn = 8;
		return(True);
	}

}


static void DragMotionCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragContext			dc;
	XmDragMotionCallback	DragMotion;

	dc = (XmDragContext)w;
	DragMotion = (XmDragMotionCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "Drag Motion! \n");
	fprintf(outfp, "operation = %d\n", DragMotion->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DragMotion->dropSiteStatus);
	fprintf(outfp, "x = %d\n", DragMotion->x);
	fprintf(outfp, "y = %d\n", DragMotion->y);
#endif

}


static void DnDFinishCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragContext				dc;
	XmDragDropFinishCallback	DndFinish;
	Widget						source_icon, state_icon, op_icon;
	Arg							args[MAX_ARGS];
	int							n;

	dc = (XmDragContext)w;
	DndFinish = (XmDragDropFinishCallback )call_data;
	source_icon = state_icon = op_icon = NULL;

	n = 0;
	XtSetArg(args[n], XmNsourceCursorIcon, &source_icon); n++;
	XtSetArg(args[n], XmNstateCursorIcon, &state_icon); n++;
	XtSetArg(args[n], XmNoperationCursorIcon, &op_icon); n++;
	XtGetValues((Widget) dc, args, n);

	if (source_icon != NULL)
		XtDestroyWidget(source_icon);
	if (state_icon != NULL)
		XtDestroyWidget(state_icon);
	if (op_icon != NULL)
		XtDestroyWidget(op_icon);

#ifdef DEBUG
	fprintf(outfp, "DND Finish! \n");
#endif

}


static void DropFinishCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragContext			dc;
	XmDropFinishCallback	DropFinish;
	Widget					source_icon, state_icon, op_icon;
	Arg						args[MAX_ARGS];
	int						n;

	dc = (XmDragContext)w;
	DropFinish = (XmDropFinishCallback )call_data;
	source_icon = state_icon = op_icon = NULL;

	n = 0;
	XtSetArg(args[n], XmNsourceCursorIcon, &source_icon); n++;
	XtSetArg(args[n], XmNstateCursorIcon, &state_icon); n++;
	XtSetArg(args[n], XmNoperationCursorIcon, &op_icon); n++;
	XtGetValues((Widget) dc, args, n);

#ifdef DEBUG
	fprintf(outfp, "Drop Finish! \n");
	fprintf(outfp, "operation = %d\n", DropFinish->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DropFinish->dropSiteStatus);
	fprintf(outfp, "dropAction = %d\n", DropFinish->dropAction);
	fprintf(outfp, "completionStatus = %d\n", DropFinish->completionStatus);
#endif

}


static void DropSiteEnterCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragContext			dc;
	XmDropSiteEnterCallback	DropSiteEnter;

	dc = (XmDragContext)w;
	DropSiteEnter = (XmDropSiteEnterCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "Drop Site Enter! \n");
	fprintf(outfp, "operation = %d\n", DropSiteEnter->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DropSiteEnter->dropSiteStatus);
	fprintf(outfp, "x = %d\n", DropSiteEnter->x);
	fprintf(outfp, "y = %d\n", DropSiteEnter->y);
#endif

}


static void DropSiteLeaveCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragContext			dc;
	XmDropSiteLeaveCallback	DropSiteLeave;
	Arg						args[MAX_ARGS];
	int						n;

	dc = (XmDragContext)w;
	DropSiteLeave = (XmDropSiteLeaveCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "Drop Site Leave! \n");
#endif

}


static void DropStartCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragContext		dc;
	XmDropStartCallback	DropStart;

	dc = (XmDragContext)w;
	DropStart = (XmDropStartCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "Drop Start! \n");
	fprintf(outfp, "operation = %d\n", DropStart->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DropStart->dropSiteStatus);
	fprintf(outfp, "dropAction = %d\n", DropStart->dropAction);
	fprintf(outfp, "x = %d\n", DropStart->x);
	fprintf(outfp, "y = %d\n", DropStart->y);
#endif

}


static void TopLEnterCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragContext			dc;
	XmTopLevelEnterCallback	TopLEnter;

	dc = (XmDragContext)w;
	TopLEnter = (XmTopLevelEnterCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "TopLevel Enter! \n");
	fprintf(outfp, "window = %ld\n", TopLEnter->window);
	fprintf(outfp, "x = %d\n", TopLEnter->x);
	fprintf(outfp, "y = %d\n", TopLEnter->y);
	fprintf(outfp, "dragProtocolStyle = %d\n", TopLEnter->dragProtocolStyle);
#endif

}


static void TopLLeaveCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragContext			dc;
	XmTopLevelLeaveCallback	TopLLeave;

	dc = (XmDragContext)w;
	TopLLeave = (XmTopLevelLeaveCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "TopLevel Leave! \n");
	fprintf(outfp, "window = %ld\n", TopLLeave->window);
#endif

}


static void CreateDefaultDragIcons(w)
Widget	w;
{

	Pixmap	defCopyPix, defMovePix, defLinkPix;
	Pixmap	defVPix, defInvPix, defNonePix, defSrcPix;
	int		n;
	Arg		args[MAX_ARGS];


	xmScreen = XmGetXmScreen(screen);

	defCopyPix = XCreateBitmapFromData(display, XtWindow(w), defCopy_bits,
									   defCopy_width, defCopy_height);
	defMovePix = XCreateBitmapFromData(display, XtWindow(w), defMove_bits,
									   defMove_width, defMove_height);
	defLinkPix = XCreateBitmapFromData(display, XtWindow(w), defLink_bits,
									   defLink_width, defLink_height);
	defVPix = XCreateBitmapFromData(display, XtWindow(w), defV_bits,
									defV_width, defV_height);
	defInvPix = XCreateBitmapFromData(display, XtWindow(w), defInv_bits,
									  defInv_width, defInv_height);
	defNonePix = XCreateBitmapFromData(display, XtWindow(w), defNone_bits,
									   defNone_width, defNone_height);
	defSrcPix = XCreateBitmapFromData(display, XtWindow(w), defSrc_bits,
									  defSrc_width, defSrc_height);

	n = 0;
	XtSetArg(args[n], XmNwidth, defCopy_width); n++;
	XtSetArg(args[n], XmNheight, defCopy_height); n++;
	XtSetArg(args[n], XmNhotX, defCopy_x_hot); n++;
	XtSetArg(args[n], XmNhotY, defCopy_y_hot); n++;
	XtSetArg(args[n], XmNdepth, 1); n++;
	XtSetArg(args[n], XmNpixmap, defCopyPix); n++;
	XtSetArg(args[n], XmNattachment, XmATTACH_SOUTH_EAST); n++;
	defCopyIcon = XmCreateDragIcon(w, "defCopyIcon", args, n);

	n = 0;
	XtSetArg(args[n], XmNwidth, defMove_width); n++;
	XtSetArg(args[n], XmNheight, defMove_height); n++;
	XtSetArg(args[n], XmNhotX, defMove_x_hot); n++;
	XtSetArg(args[n], XmNhotY, defMove_y_hot); n++;
	XtSetArg(args[n], XmNdepth, 1); n++;
	XtSetArg(args[n], XmNpixmap, defMovePix); n++;
	XtSetArg(args[n], XmNattachment, XmATTACH_SOUTH_EAST); n++;
	defMoveIcon = XmCreateDragIcon(w, "defMoveIcon", args, n);

	n = 0;
	XtSetArg(args[n], XmNwidth, defLink_width); n++;
	XtSetArg(args[n], XmNheight, defLink_height); n++;
	XtSetArg(args[n], XmNhotX, defLink_x_hot); n++;
	XtSetArg(args[n], XmNhotY, defLink_y_hot); n++;
	XtSetArg(args[n], XmNdepth, 1); n++;
	XtSetArg(args[n], XmNpixmap, defLinkPix); n++;
	XtSetArg(args[n], XmNattachment, XmATTACH_SOUTH_EAST); n++;
	defLinkIcon = XmCreateDragIcon(w, "defLinkIcon", args, n);

	n = 0;
	XtSetArg(args[n], XmNwidth, defV_width); n++;
	XtSetArg(args[n], XmNheight, defV_height); n++;
	XtSetArg(args[n], XmNhotX, defV_x_hot); n++;
	XtSetArg(args[n], XmNhotY, defV_y_hot); n++;
	XtSetArg(args[n], XmNdepth, 1); n++;
	XtSetArg(args[n], XmNpixmap, defVPix); n++;
	XtSetArg(args[n], XmNattachment, XmATTACH_NORTH_EAST); n++;
	defVIcon = XmCreateDragIcon(w, "defVIcon", args, n);

	n = 0;
	XtSetArg(args[n], XmNwidth, defInv_width); n++;
	XtSetArg(args[n], XmNheight, defInv_height); n++;
	XtSetArg(args[n], XmNhotX, defInv_x_hot); n++;
	XtSetArg(args[n], XmNhotY, defInv_y_hot); n++;
	XtSetArg(args[n], XmNdepth, 1); n++;
	XtSetArg(args[n], XmNpixmap, defInvPix); n++;
	XtSetArg(args[n], XmNattachment, XmATTACH_NORTH_EAST); n++;
	defInvIcon = XmCreateDragIcon(w, "defInvIcon", args, n);

	n = 0;
	XtSetArg(args[n], XmNwidth, defNone_width); n++;
	XtSetArg(args[n], XmNheight, defNone_height); n++;
	XtSetArg(args[n], XmNhotX, defNone_x_hot); n++;
	XtSetArg(args[n], XmNhotY, defNone_y_hot); n++;
	XtSetArg(args[n], XmNdepth, 1); n++;
	XtSetArg(args[n], XmNpixmap, defNonePix); n++;
	XtSetArg(args[n], XmNattachment, XmATTACH_NORTH_EAST); n++;
	defNoneIcon = XmCreateDragIcon(w, "defNoneIcon", args, n);

	n = 0;
	XtSetArg(args[n], XmNwidth, defSrc_width); n++;
	XtSetArg(args[n], XmNheight, defSrc_height); n++;
	XtSetArg(args[n], XmNhotX, defSrc_x_hot); n++;
	XtSetArg(args[n], XmNhotY, defSrc_y_hot); n++;
	XtSetArg(args[n], XmNdepth, 1); n++;
	XtSetArg(args[n], XmNpixmap, defSrcPix); n++;
	defSrcIcon = XmCreateDragIcon(w, "defSrcIcon", args, n);

	n = 0;
	XtSetArg(args[n], XmNdefaultCopyCursorIcon, defCopyIcon); n++;
	XtSetArg(args[n], XmNdefaultMoveCursorIcon, defMoveIcon); n++;
	XtSetArg(args[n], XmNdefaultLinkCursorIcon, defLinkIcon); n++;
	XtSetArg(args[n], XmNdefaultValidCursorIcon, defVIcon); n++;
	XtSetArg(args[n], XmNdefaultInvalidCursorIcon, defInvIcon); n++;
	XtSetArg(args[n], XmNdefaultNoneCursorIcon, defNoneIcon); n++;
	XtSetArg(args[n], XmNdefaultSourceCursorIcon, defSrcIcon); n++;
	XtSetValues(xmScreen, args, n);

}


static void StartDrag(w, event)
Widget	w;
XEvent	*event;
{

	Pixmap		sourceIcon, opIcon, stateIcon;
	Arg			args[MAX_ARGS];
	Cardinal	n;
	Atom		exportList[1];
	static XtCallbackRec DragMotionCB[2] = 
	{
		{DragMotionCallback,NULL},
		{NULL, NULL}
	};

	static XtCallbackRec DnDFinishCB[2] = 
	{
		{DnDFinishCallback,NULL},
		{NULL, NULL}
	};

	static XtCallbackRec DropFinishCB[2] = 
	{
		{DropFinishCallback,NULL},
		{NULL, NULL}
	};

	static XtCallbackRec DropSiteEnterCB[2] = 
	{
		{DropSiteEnterCallback,NULL},
		{NULL, NULL}
	};

	static XtCallbackRec DropSiteLeaveCB[2] = 
	{
		{DropSiteLeaveCallback,NULL},
		{NULL, NULL}
	};

	static XtCallbackRec DropStartCB[2] = 
	{
		{DropStartCallback,NULL},
		{NULL, NULL}
	};

	static XtCallbackRec TopLEnterCB[2] = 
	{
		{TopLEnterCallback,NULL},
		{NULL, NULL}
	};

	static XtCallbackRec TopLLeaveCB[2] = 
	{
		{TopLLeaveCallback,NULL},
		{NULL, NULL}
	};


	CreateDefaultDragIcons(w);

	exportList[0] = COMPOUND_TEXT;
	n = 0;
	XtSetArg(args[n], XmNconvertProc, DragConvertProc); n++;
	XtSetArg(args[n], XmNdragDropFinishCallback, DnDFinishCB); n++;
	XtSetArg(args[n], XmNdragMotionCallback, DragMotionCB); n++;
	XtSetArg(args[n], XmNdropFinishCallback, DropFinishCB); n++;
	XtSetArg(args[n], XmNdropSiteEnterCallback, DropSiteEnterCB); n++;
	XtSetArg(args[n], XmNdropSiteLeaveCallback, DropSiteLeaveCB); n++;
	XtSetArg(args[n], XmNdropStartCallback, DropStartCB); n++;
	XtSetArg(args[n], XmNtopLevelEnterCallback, TopLEnterCB); n++;
	XtSetArg(args[n], XmNtopLevelLeaveCallback, TopLLeaveCB); n++;

	XtSetArg(args[n], XmNexportTargets, exportList); n++;
	XtSetArg(args[n], XmNnumExportTargets, 1); n++;

	XtSetArg(args[n], XmNcursorForeground, cursorForeground); n++;
	XtSetArg(args[n], XmNcursorBackground, cursorBackground); n++;
	XtSetArg(args[n], XmNinvalidCursorForeground, InvalidcursorForeground); n++;
	XtSetArg(args[n], XmNvalidCursorForeground, ValidcursorForeground); n++;
	XtSetArg(args[n], XmNnoneCursorForeground, NonecursorForeground); n++;
	XtSetArg(args[n], XmNdragOperations, dragOperations); n++;
	XtSetArg(args[n], XmNblendModel, blendModel); n++;

	myDC = XmDragStart(w, event, args, n);

}


static void HandleDrag(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragProcCallback	DragProc;


	DragProc = (XmDragProcCallback)call_data;

#ifdef DEBUG
	fprintf(outfp, "DragProc !\n");
	fprintf(outfp, "x = %d\n", DragProc->x);
	fprintf(outfp, "y = %d\n", DragProc->y);
	fprintf(outfp, "dropSiteStatus = %d\n", DragProc->dropSiteStatus);
	fprintf(outfp, "operations = %d\n", DragProc->operations);
#endif

}


static void DropTransferCallback(w, closure, seltype, type, value, 
								 length, format)
Widget 			w;
XtPointer 		closure ;
Atom 			*seltype ;
Atom 			*type ;
XtPointer 		value ;
unsigned long	*length ;
int 			*format ;
{

	XTextProperty	tmp_prop;
	int 			num_vals;
	int				ok;
	char 			*total_tmp_value = NULL;
	char 			**tmp_value;
	int 			malloc_size = 0;
	char 			*actual_value;
	int				actual_length = 0;
	Cardinal		i;



	if (*length)
		if (*type == COMPOUND_TEXT) {

       		tmp_prop.value = (unsigned char *) value;
	   		tmp_prop.encoding = *type;
			tmp_prop.format = 8;
			tmp_prop.nitems = *length;
			num_vals = 0;

			ok = 0;
			ok = XmbTextPropertyToTextList(display, &tmp_prop, &tmp_value,
										   &num_vals);

			if (num_vals && (ok == Success || ok > 0)) {

				for (i = 0; i < num_vals; i++)
					malloc_size += strlen(*tmp_value + i);
				total_tmp_value = XtMalloc ((unsigned) malloc_size + 1);
				total_tmp_value[0] = '\0';
				for (i = 0; i < num_vals ; i++)
					strcat(total_tmp_value, *tmp_value + i);
				actual_value = total_tmp_value;
				actual_length = strlen(total_tmp_value);
#ifdef DEBUG
				fprintf(stderr, "the transfer value is : %s\n", actual_value);
				fprintf(stderr, "the length of transfer is : %d\n", 
						actual_length);
#endif
				XFreeStringList(tmp_value);

			}
			else {
				if (value) {
					XtFree((char *)value);
					value = NULL;
				}
				actual_length = -1;
			}

		}

#ifdef DEBUG
	if (actual_length != -1) {
		if (*type == COMPOUND_TEXT)
			fprintf(outfp, "the transfer value is : %s\n", actual_value);
		else if (*type == XA_PIXMAP)
			fprintf(outfp, "the transfer value is of type pixmap\n");
		else if (*type == XA_BITMAP)
			fprintf(outfp, "the transfer value is of type bitmap\n");
		else if (*type == NULL_ATOM)
			fprintf(outfp, "the transfer value is of type NULL\n");
	}
#endif

	/* You no longer need it, so free it */
	if (total_tmp_value != NULL)
		XtFree(total_tmp_value);

}


static void HandleDrop(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDropProcCallback		DropProc;
	Widget 					dc;
	XmDropTransferEntryRec	transferEntries[2];
	XmDropTransferEntry		transferList;
	Cardinal 				numTransfers = 0;
	Cardinal 				numExportTargets;
	Atom 					*exportTargets;
	Cardinal 				numImportTargets;
	Atom 					*importTargets;
	Arg 					args[MAX_ARGS];
	int						n;
	char					*string = "TestString";


	DropProc = (XmDropProcCallback)call_data;
	dc = DropProc->dragContext;
	n = 0;
	XtSetArg(args[n], XmNexportTargets, &exportTargets); n++;
	XtSetArg(args[n], XmNnumExportTargets, &numExportTargets); n++;
	XtGetValues(dc, args, n);
	n = 0;
	XtSetArg(args[n], XmNimportTargets, &importTargets); n++;
	XtSetArg(args[n], XmNnumImportTargets, &numImportTargets); n++;
	XmDropSiteRetrieve(w, args, n);


#ifdef DEBUG
	fprintf(outfp, "DropProc !\n");
	fprintf(outfp, "x = %d\n", DropProc->x);
	fprintf(outfp, "y = %d\n", DropProc->y);
	fprintf(outfp, "dropSiteStatus = %d\n", DropProc->dropSiteStatus);
	fprintf(outfp, "operation = %d\n", DropProc->operation);
	fprintf(outfp, "dropAction = %d\n", DropProc->dropAction);
#endif

	transferEntries[0].target = COMPOUND_TEXT;
	transferEntries[0].client_data = string;
	n = 0;
	if (XmTargetsAreCompatible(display, exportTargets, numExportTargets,
						   importTargets, numImportTargets) == False) {
		XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
		XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
	}
	else {

		if (DropProc->operation & XmDROP_MOVE || 
			DropProc->operation & XmDROP_COPY) {
		
			if (DropProc->operation & XmDROP_MOVE) {
				transferEntries[1].target = DELETE;
				transferEntries[1].client_data = NULL;
				numTransfers = 2;
			}
			else
				numTransfers = 1;
			transferList = transferEntries;
			XtSetArg(args[n], XmNdropTransfers, transferList); n++;
			XtSetArg(args[n], XmNnumDropTransfers, numTransfers); n++;
			XtSetArg(args[n], XmNtransferProc, DropTransferCallback); n++;

		} 
		else {
			if (DropProc->operation & XmDROP_LINK)
				fprintf(stderr, "Link Operation currently not supported\n");
			XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
			XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
		}

	}

	XmDropTransferStart(dc, args, n);

}


static int ProtoError(dpy, ev)
Display		*dpy;
XErrorEvent	*ev;
{

	fprintf(stderr, "Proto Error\n");
#ifdef DEBUG
	fflush(outfp);
	fclose(outfp);
#endif
	exit(0);

}


/*ARGSUSED*/
static void DragUnderHighLightCB(Widget w, XtPointer client_data, 
								 XtPointer call_data)
{

	animationStyle = XmDRAG_UNDER_HIGHLIGHT;
	return;

}


/*ARGSUSED*/
static void DragUnderShadowOutCB(Widget w, XtPointer client_data, 
									 XtPointer call_data)
{

	animationStyle = XmDRAG_UNDER_SHADOW_OUT;
	return;

}


/*ARGSUSED*/
static void DragUnderShadowInCB(Widget w, XtPointer client_data, 
									 XtPointer call_data)
{

	animationStyle = XmDRAG_UNDER_SHADOW_IN;
	return;

}


/*ARGSUSED*/
static void DragUnderPixmapCB(Widget w, XtPointer client_data, 
									 XtPointer call_data)
{

	animationStyle = XmDRAG_UNDER_PIXMAP;
	return;

}


/*ARGSUSED*/
static void DragUnderNoneCB(Widget w, XtPointer client_data, 
									 XtPointer call_data)
{

	animationStyle = XmDRAG_UNDER_SHADOW_OUT;
	return;

}


/*ARGSUSED*/
static void dropActiveCB(Widget w, XtPointer client_data, 
							 XtPointer call_data)
{

	dropActivity = XmDROP_SITE_ACTIVE;
	return;

}


/*ARGSUSED*/
static void dropInactiveCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dropActivity = XmDROP_SITE_INACTIVE;
	return;

}


/*ARGSUSED*/
static void dropMoveCopyCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dropOperations = XmDROP_MOVE | XmDROP_COPY;
	return;

}


/*ARGSUSED*/
static void dropMoveOnlyCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dropOperations = XmDROP_MOVE;
	return;

}


/*ARGSUSED*/
static void dropCopyOnlyCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dropOperations = XmDROP_COPY;
	return;

}


/*ARGSUSED*/
static void dropLinkOnlyCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dropOperations = XmDROP_LINK;
	return;

}


/*ARGSUSED*/
static void dropMoveLinkCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dropOperations = XmDROP_MOVE | XmDROP_LINK;
	return;

}


/*ARGSUSED*/
static void dropCopyLinkCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dropOperations = XmDROP_COPY | XmDROP_LINK;
	return;

}


/*ARGSUSED*/
static void dropAllCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dropOperations = XmDROP_MOVE | XmDROP_COPY | XmDROP_LINK;
	return;

}


/*ARGSUSED*/
static void dropNoneCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dropOperations = XmDROP_NOOP;
	return;

}


/*ARGSUSED*/
static void dragMoveCopyCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dragOperations = XmDROP_MOVE | XmDROP_COPY;
	return;

}


/*ARGSUSED*/
static void dragMoveOnlyCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dragOperations = XmDROP_MOVE;
	return;

}


/*ARGSUSED*/
static void dragCopyOnlyCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dragOperations = XmDROP_COPY;
	return;

}


/*ARGSUSED*/
static void dragLinkOnlyCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dragOperations = XmDROP_LINK;
	return;

}


/*ARGSUSED*/
static void dragMoveLinkCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dragOperations = XmDROP_MOVE | XmDROP_LINK;
	return;

}


/*ARGSUSED*/
static void dragCopyLinkCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dragOperations = XmDROP_COPY | XmDROP_LINK;
	return;

}


/*ARGSUSED*/
static void dragAllCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dragOperations = XmDROP_MOVE | XmDROP_COPY | XmDROP_LINK;
	return;

}


/*ARGSUSED*/
static void dragNoneCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	dragOperations = XmDROP_NOOP;
	return;

}


/*ARGSUSED*/
static void blendStateSourceCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	blendModel = XmBLEND_STATE_SOURCE;
	return;

}


/*ARGSUSED*/
static void blendJustSourceCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	blendModel = XmBLEND_JUST_SOURCE;
	return;

}


/*ARGSUSED*/
static void blendAllCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	blendModel = XmBLEND_ALL;
	return;

}


/*ARGSUSED*/
static void blendNoneCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	blendModel = XmBLEND_NONE;
	return;

}


/*ARGSUSED*/
static void redFGCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	cursorForeground = redPixel;
	return;

}


/*ARGSUSED*/
static void blackFGCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	cursorForeground = blackPixel;
	return;

}


/*ARGSUSED*/
static void yellowFGCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	cursorForeground = yellowPixel;
	return;

}


/*ARGSUSED*/
static void greenFGCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	cursorForeground = greenPixel;
	return;

}


/*ARGSUSED*/
static void whiteFGCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	cursorForeground = whitePixel;
	return;

}


/*ARGSUSED*/
static void redBGCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	cursorBackground = redPixel;
	return;

}


/*ARGSUSED*/
static void blackBGCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	cursorBackground = blackPixel;
	return;

}


/*ARGSUSED*/
static void yellowBGCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	cursorBackground = yellowPixel;
	return;

}


/*ARGSUSED*/
static void greenBGCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	cursorBackground = greenPixel;
	return;

}


/*ARGSUSED*/
static void whiteBGCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	cursorBackground = whitePixel;
	return;

}


/*ARGSUSED*/
static void redValidCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	ValidcursorForeground = redPixel;
	return;

}


/*ARGSUSED*/
static void blackValidCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	ValidcursorForeground = blackPixel;
	return;

}


/*ARGSUSED*/
static void yellowValidCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	ValidcursorForeground = yellowPixel;
	return;

}


/*ARGSUSED*/
static void greenValidCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	ValidcursorForeground = greenPixel;
	return;

}


/*ARGSUSED*/
static void whiteValidCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	ValidcursorForeground = whitePixel;
	return;

}


/*ARGSUSED*/
static void redInvalidCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	InvalidcursorForeground = redPixel;
	return;

}


/*ARGSUSED*/
static void blackInvalidCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	InvalidcursorForeground = blackPixel;
	return;

}


/*ARGSUSED*/
static void yellowInvalidCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	InvalidcursorForeground = yellowPixel;
	return;

}


/*ARGSUSED*/
static void greenInvalidCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	InvalidcursorForeground = greenPixel;
	return;

}


/*ARGSUSED*/
static void whiteInvalidCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	InvalidcursorForeground = whitePixel;
	return;

}


/*ARGSUSED*/
static void redNoneCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	NonecursorForeground = redPixel;
	return;

}


/*ARGSUSED*/
static void blackNoneCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	NonecursorForeground = blackPixel;
	return;

}


/*ARGSUSED*/
static void yellowNoneCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	NonecursorForeground = yellowPixel;
	return;

}


/*ARGSUSED*/
static void greenNoneCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	NonecursorForeground = greenPixel;
	return;

}


/*ARGSUSED*/
static void whiteNoneCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	NonecursorForeground = whitePixel;
	return;

}


/*ARGSUSED*/
static void ApplyChangesCB(Widget w, XtPointer client_data, 
									 XtPointer call_data)
{

	int	i, n;
	Arg	args[MAX_ARGS];


	n = 0;
	XtSetArg(args[n], XmNanimationStyle, animationStyle); n++;
	XtSetArg(args[n], XmNdropSiteActivity, dropActivity); n++;
	XtSetArg(args[n], XmNdropSiteOperations, dropOperations); n++;

	for (i = 0; i < numDropSites; i++)
		XmDropSiteUpdate(dropSites[i], args, n);

}


void main (argc, argv)
unsigned int argc;
char **argv;

{

	Arg 		args[MAX_ARGS];
	int 		n, i, j, k, l;
	AppDataRec	appDataRec, *appData;
	Widget	 	topLevel, BulletinB, PushBtn, Text;
	XEvent		event;
	char		name[80];
	Atom		importList[1];
	Widget		*buttons;
	Cardinal	numButtons;
	XmString	tcs;



	appData = &appDataRec;

	use_instruction_box = True;

	initiatorProtocolStyle = XmDRAG_PREFER_RECEIVER;
	receiverProtocolStyle = XmDRAG_PREFER_PREREGISTER;
	animationStyle = XmDRAG_UNDER_HIGHLIGHT;
	dropActivity = XmDROP_SITE_ACTIVE;
	dropOperations = XmDROP_MOVE | XmDROP_COPY;
	dragOperations = XmDROP_COPY;
	blendModel = XmBLEND_ALL;

#ifdef DEBUG
	sprintf(name, "%s.out", argv[0]);

	if ((outfp = fopen(name, "w+")) == NULL) {

		fprintf(stderr, "Can not open output file %s.out. Trying in /tmp\n",
				argv[0]);
		sprintf(name, "/tmp/%s.out", argv[0]);
		if ((outfp = fopen(name, "w+")) == NULL) {
			fprintf(stderr, "Can not open output file /tmp/%s.out. Exiting\n",
					argv[0]);
			exit(0);
		}

	}
#endif

	XSetErrorHandler(ProtoError);

	CommonTestInit(argc, argv);

	if (UserData != NULL)
		ProcessCommandArgs();

	XtAppAddActions(app_context, (XtActionList)myactions, XtNumber(myactions));

	XtGetApplicationResources(Shell1, (XtPointer)appData, 
							  appResources, XtNumber(appResources), NULL, 0);

	/* try out stuffing the translations directly in */
	n = 0;
	XtSetArg(args[n], XmNtranslations, appData->xlation1.xlation); n++;
	/* 
	 * Set the initiatorProtocolStyle and receiverProtocolStyle resources
	 * if asked to from the command line.
	 */
	xmDisplay = XmGetXmDisplay(display);
	XtSetArg(args[n], XmNdragInitiatorProtocolStyle, 
			 initiatorProtocolStyle); n++;
	XtSetArg(args[n], XmNdragReceiverProtocolStyle, 
			 receiverProtocolStyle); n++;
	XtSetValues(xmDisplay, args, n);

	XtRealizeWidget(Shell1);

	redPixel = CommonGetColor("red");
	blackPixel = CommonGetColor("black");
	yellowPixel = CommonGetColor("yellow");
	greenPixel = CommonGetColor("green");
	whitePixel = CommonGetColor("white");
	cursorForeground = redPixel;
	cursorBackground = whitePixel;
	ValidcursorForeground = redPixel;
	InvalidcursorForeground = redPixel;
	NonecursorForeground = redPixel;

	COMPOUND_TEXT = XmInternAtom(XtDisplay(Shell1), "COMPOUND_TEXT", False);
	DELETE = XmInternAtom(XtDisplay(Shell1), "DELETE", False);
	NULL_ATOM = XmInternAtom(XtDisplay(Shell1), "NULL", False);

	/* Create Outer Form */

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	Form = XmCreateForm(Shell1, "Form", args, n);
	XtManageChild(Form);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	Whichanim_Pulldown = XmCreatePulldownMenu(Shell1, "Whichanim_Pulldown", 
											  args, n);

	DragUnderHighLight = CreatePushButton(Whichanim_Pulldown, "HighLight");
	DragUnderShadowOut = CreatePushButton(Whichanim_Pulldown, "ShadowOut");
	DragUnderShadowIn = CreatePushButton(Whichanim_Pulldown, "ShadowIn");
	DragUnderNone = CreatePushButton(Whichanim_Pulldown, "None");
	DragUnderPixmap = CreatePushButton(Whichanim_Pulldown, "Pixmap");

	XtAddCallback(DragUnderHighLight, XmNactivateCallback, 
				  DragUnderHighLightCB, NULL);
	XtAddCallback(DragUnderShadowOut, XmNactivateCallback, 
				  DragUnderShadowOutCB, NULL);
	XtAddCallback(DragUnderShadowIn, XmNactivateCallback, 
				  DragUnderShadowInCB, NULL);
	XtAddCallback(DragUnderNone, XmNactivateCallback, 
				  DragUnderNoneCB, NULL);
	XtAddCallback(DragUnderPixmap, XmNactivateCallback, 
				  DragUnderPixmapCB, NULL);

	n = 0;
	XtSetArg(args[n], XmNsensitive, False); n++;
	XtSetValues(DragUnderPixmap, args, n);

    tcs = XmStringLtoRCreate("Animation Style         ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, DragUnderHighLight); n++;
	XtSetArg(args[n], XmNsubMenuId, Whichanim_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	Whichanim_Option = XmCreateOptionMenu(Form, "Whichanim_Option", 
										   args, n);
	XtManageChild(Whichanim_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichdropAct_Pulldown = XmCreatePulldownMenu(Shell1, 
												 "WhichdropAct_Pulldown", 
												 args, n);

	dropActive = CreatePushButton(WhichdropAct_Pulldown, "Active");
	dropInactive = CreatePushButton(WhichdropAct_Pulldown, "Inactive");

	XtAddCallback(dropActive, XmNactivateCallback, dropActiveCB, NULL);
	XtAddCallback(dropInactive, XmNactivateCallback, dropInactiveCB, NULL);

    tcs = XmStringLtoRCreate("Drop Activity         ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, dropActive); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichdropAct_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, Whichanim_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichdropAct_Option = XmCreateOptionMenu(Form, "WhichdropAct_Option", 
											 args, n);
	XtManageChild(WhichdropAct_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichdropOp_Pulldown = XmCreatePulldownMenu(Shell1, "WhichdropOp_Pulldown", 
												  args, n);

	dropMoveCopy = CreatePushButton(WhichdropOp_Pulldown, "Move_n_Copy");
	dropMoveOnly = CreatePushButton(WhichdropOp_Pulldown, "MoveOnly");
	dropCopyOnly = CreatePushButton(WhichdropOp_Pulldown, "CopyOnly");
	dropLinkOnly = CreatePushButton(WhichdropOp_Pulldown, "LinkOnly");
	dropMoveLink = CreatePushButton(WhichdropOp_Pulldown, "Move_n_Link");
	dropCopyLink = CreatePushButton(WhichdropOp_Pulldown, "Copy_n_Link");
	dropAll = CreatePushButton(WhichdropOp_Pulldown, "All");
	dropNone = CreatePushButton(WhichdropOp_Pulldown, "None");

	XtAddCallback(dropMoveCopy, XmNactivateCallback, dropMoveCopyCB, NULL);
	XtAddCallback(dropMoveOnly, XmNactivateCallback, dropMoveOnlyCB, NULL);
	XtAddCallback(dropCopyOnly, XmNactivateCallback, dropCopyOnlyCB, NULL);
	XtAddCallback(dropLinkOnly, XmNactivateCallback, dropLinkOnlyCB, NULL);
	XtAddCallback(dropMoveLink, XmNactivateCallback, dropMoveLinkCB, NULL);
	XtAddCallback(dropCopyLink, XmNactivateCallback, dropCopyLinkCB, NULL);
	XtAddCallback(dropAll, XmNactivateCallback, dropAllCB, NULL);
	XtAddCallback(dropNone, XmNactivateCallback, dropNoneCB, NULL);

    tcs = XmStringLtoRCreate("Drop Operations         ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, dropMoveCopy); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichdropOp_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichdropAct_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichdropOp_Option = XmCreateOptionMenu(Form, "WhichdropOp_Option", 
										   args, n);
	XtManageChild(WhichdropOp_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichdragOp_Pulldown = XmCreatePulldownMenu(Shell1, "WhichdragOp_Pulldown", 
												  args, n);

	dragMoveCopy = CreatePushButton(WhichdragOp_Pulldown, "Move_n_Copy");
	dragMoveOnly = CreatePushButton(WhichdragOp_Pulldown, "MoveOnly");
	dragCopyOnly = CreatePushButton(WhichdragOp_Pulldown, "CopyOnly");
	dragLinkOnly = CreatePushButton(WhichdragOp_Pulldown, "LinkOnly");
	dragMoveLink = CreatePushButton(WhichdragOp_Pulldown, "Move_n_Link");
	dragCopyLink = CreatePushButton(WhichdragOp_Pulldown, "Copy_n_Link");
	dragAll = CreatePushButton(WhichdragOp_Pulldown, "All");
	dragNone = CreatePushButton(WhichdragOp_Pulldown, "None");

	XtAddCallback(dragMoveCopy, XmNactivateCallback, dragMoveCopyCB, NULL);
	XtAddCallback(dragMoveOnly, XmNactivateCallback, dragMoveOnlyCB, NULL);
	XtAddCallback(dragCopyOnly, XmNactivateCallback, dragCopyOnlyCB, NULL);
	XtAddCallback(dragLinkOnly, XmNactivateCallback, dragLinkOnlyCB, NULL);
	XtAddCallback(dragMoveLink, XmNactivateCallback, dragMoveLinkCB, NULL);
	XtAddCallback(dragCopyLink, XmNactivateCallback, dragCopyLinkCB, NULL);
	XtAddCallback(dragAll, XmNactivateCallback, dragAllCB, NULL);
	XtAddCallback(dragNone, XmNactivateCallback, dragNoneCB, NULL);

    tcs = XmStringLtoRCreate("Drag Operations         ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, dragCopyOnly); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichdragOp_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichdropOp_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichdragOp_Option = XmCreateOptionMenu(Form, "WhichdragOp_Option", 
										   args, n);
	XtManageChild(WhichdragOp_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichBlend_Pulldown = XmCreatePulldownMenu(Shell1, "WhichBlend_Pulldown", 
												  args, n);

	blendAll = CreatePushButton(WhichBlend_Pulldown, "All");
	blendStateSource = CreatePushButton(WhichBlend_Pulldown, "State_n_Source");
	blendJustSource = CreatePushButton(WhichBlend_Pulldown, "JustSource");
	blendNone = CreatePushButton(WhichBlend_Pulldown, "None");

	XtAddCallback(blendAll, XmNactivateCallback, blendAllCB, NULL);
	XtAddCallback(blendStateSource, XmNactivateCallback, 
				  blendStateSourceCB, NULL);
	XtAddCallback(blendJustSource, XmNactivateCallback, 
				  blendJustSourceCB, NULL);
	XtAddCallback(blendNone, XmNactivateCallback, blendNoneCB, NULL);


    tcs = XmStringLtoRCreate("Blend Model         ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, blendAll); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichBlend_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichdragOp_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichBlend_Option = XmCreateOptionMenu(Form, "WhichBlend_Option", 
										   args, n);
	XtManageChild(WhichBlend_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichForeground_Pulldown = XmCreatePulldownMenu(Shell1, 
													"WhichForeground_Pulldown", 
												  	args, n);

	redFG = CreatePushButton(WhichForeground_Pulldown, "Red");
	blackFG = CreatePushButton(WhichForeground_Pulldown, "Black");
	yellowFG = CreatePushButton(WhichForeground_Pulldown, "Yellow");
	greenFG = CreatePushButton(WhichForeground_Pulldown, "Green");
	whiteFG = CreatePushButton(WhichForeground_Pulldown, "White");

	XtAddCallback(redFG, XmNactivateCallback, redFGCB, NULL);
	XtAddCallback(blackFG, XmNactivateCallback, blackFGCB, NULL);
	XtAddCallback(yellowFG, XmNactivateCallback, yellowFGCB, NULL);
	XtAddCallback(greenFG, XmNactivateCallback, greenFGCB, NULL);
	XtAddCallback(whiteFG, XmNactivateCallback, whiteFGCB, NULL);

    tcs = XmStringLtoRCreate("Cursor Foreground     ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, redFG); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichForeground_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichBlend_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichForeground_Option = XmCreateOptionMenu(Form, "WhichForeground_Option", 
										   		args, n);
	XtManageChild(WhichForeground_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichBackground_Pulldown = XmCreatePulldownMenu(Shell1, 
													"WhichBackground_Pulldown", 
												  	args, n);

	redBG = CreatePushButton(WhichBackground_Pulldown, "Red");
	blackBG = CreatePushButton(WhichBackground_Pulldown, "Black");
	yellowBG = CreatePushButton(WhichBackground_Pulldown, "Yellow");
	greenBG = CreatePushButton(WhichBackground_Pulldown, "Green");
	whiteBG = CreatePushButton(WhichBackground_Pulldown, "White");

	XtAddCallback(redBG, XmNactivateCallback, redBGCB, NULL);
	XtAddCallback(blackBG, XmNactivateCallback, blackBGCB, NULL);
	XtAddCallback(yellowBG, XmNactivateCallback, yellowBGCB, NULL);
	XtAddCallback(greenBG, XmNactivateCallback, greenBGCB, NULL);
	XtAddCallback(whiteBG, XmNactivateCallback, whiteBGCB, NULL);

    tcs = XmStringLtoRCreate("Cursor Background     ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, whiteBG); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichBackground_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichForeground_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichBackground_Option = XmCreateOptionMenu(Form, "WhichBackground_Option", 
										   		args, n);
	XtManageChild(WhichBackground_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichValidForeground_Pulldown = XmCreatePulldownMenu(Shell1, 
											"WhichValidForeground_Pulldown", 
											args, n);

	redValid = CreatePushButton(WhichValidForeground_Pulldown, "Red");
	blackValid = CreatePushButton(WhichValidForeground_Pulldown, "Black");
	yellowValid = CreatePushButton(WhichValidForeground_Pulldown, "Yellow");
	greenValid = CreatePushButton(WhichValidForeground_Pulldown, "Green");
	whiteValid = CreatePushButton(WhichValidForeground_Pulldown, "White");

	XtAddCallback(redValid, XmNactivateCallback, redValidCB, NULL);
	XtAddCallback(blackValid, XmNactivateCallback, blackValidCB, NULL);
	XtAddCallback(yellowValid, XmNactivateCallback, yellowValidCB, NULL);
	XtAddCallback(greenValid, XmNactivateCallback, greenValidCB, NULL);
	XtAddCallback(whiteValid, XmNactivateCallback, whiteValidCB, NULL);

    tcs = XmStringLtoRCreate("Valid Cursor Foreground ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, redValid); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichValidForeground_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichBackground_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichValidForeground_Option = XmCreateOptionMenu(Form, 
												"WhichValidForeground_Option", 
										   		args, n);
	XtManageChild(WhichValidForeground_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichInvalidForeground_Pulldown = XmCreatePulldownMenu(Shell1, 
											"WhichInvalidForeground_Pulldown", 
											args, n);

	redInvalid = CreatePushButton(WhichInvalidForeground_Pulldown, "Red");
	blackInvalid = CreatePushButton(WhichInvalidForeground_Pulldown, "Black");
	yellowInvalid = CreatePushButton(WhichInvalidForeground_Pulldown, "Yellow");
	greenInvalid = CreatePushButton(WhichInvalidForeground_Pulldown, "Green");
	whiteInvalid = CreatePushButton(WhichInvalidForeground_Pulldown, "White");

	XtAddCallback(redInvalid, XmNactivateCallback, redInvalidCB, NULL);
	XtAddCallback(blackInvalid, XmNactivateCallback, blackInvalidCB, NULL);
	XtAddCallback(yellowInvalid, XmNactivateCallback, yellowInvalidCB, NULL);
	XtAddCallback(greenInvalid, XmNactivateCallback, greenInvalidCB, NULL);
	XtAddCallback(whiteInvalid, XmNactivateCallback, whiteInvalidCB, NULL);

    tcs = XmStringLtoRCreate("Invalid Cursor Foreground ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, redInvalid); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichInvalidForeground_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichValidForeground_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichInvalidForeground_Option = XmCreateOptionMenu(Form, 
												"WhichInvalidForeground_Option",
										   		args, n);
	XtManageChild(WhichInvalidForeground_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichNoneForeground_Pulldown = XmCreatePulldownMenu(Shell1, 
											"WhichNoneForeground_Pulldown", 
											args, n);

	redNone = CreatePushButton(WhichNoneForeground_Pulldown, "Red");
	blackNone = CreatePushButton(WhichNoneForeground_Pulldown, "Black");
	yellowNone = CreatePushButton(WhichNoneForeground_Pulldown, "Yellow");
	greenNone = CreatePushButton(WhichNoneForeground_Pulldown, "Green");
	whiteNone = CreatePushButton(WhichNoneForeground_Pulldown, "White");

	XtAddCallback(redNone, XmNactivateCallback, redNoneCB, NULL);
	XtAddCallback(blackNone, XmNactivateCallback, blackNoneCB, NULL);
	XtAddCallback(yellowNone, XmNactivateCallback, yellowNoneCB, NULL);
	XtAddCallback(greenNone, XmNactivateCallback, greenNoneCB, NULL);
	XtAddCallback(whiteNone, XmNactivateCallback, whiteNoneCB, NULL);

    tcs = XmStringLtoRCreate("None Cursor Foreground ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, redNone); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichNoneForeground_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichInvalidForeground_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichNoneForeground_Option = XmCreateOptionMenu(Form, 
												"WhichNoneForeground_Option",
										   		args, n);
	XtManageChild(WhichNoneForeground_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNtopWidget, WhichNoneForeground_Option); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNbottomOffset, 5); n++;
	ApplyChanges = XmCreatePushButton(Form, "ApplyChanges", args, n);
	XtManageChild(ApplyChanges);

	XtAddCallback(ApplyChanges, XmNactivateCallback, 
				  ApplyChangesCB, NULL);


	for (i = 0; i < appData->numShells; i++) {

		sprintf(name, "topLevel%d",i);
		n = 0;
		topLevel = XtCreatePopupShell(name, topLevelShellWidgetClass, Shell1, 
								   	  args, n);

		sprintf(name, "BulletinB%d",i);
		n = 0;
		BulletinB = XmCreateBulletinBoard(topLevel, name, args, n);
		XtManageChild(BulletinB);

		for (j = 0; j < appData->numCols; j++)
			for (k = 0; k < appData->numRows; k++) {

				n = 0;
				XtSetArg(args[n], XmNx, 10 + j * 100); n++;
				XtSetArg(args[n], XmNy, 10 + k * 100); n++;
				XtSetArg(args[n], XmNwidth, 90); n++;
				XtSetArg(args[n], XmNheight, 90); n++;
				if (appData->applyOverrides) {
					XtSetArg(args[n], XmNtranslations, 
							 appData->xlation2.xlation); n++;
				}
				sprintf(name, "PushBtn%d", (j * appData->numRows) + k);
				PushBtn = XmCreatePushButton(BulletinB, name, args, n);
				XtManageChild(PushBtn);

				if (appData->applyAccelerators)
					XtInstallAccelerators(PushBtn, BulletinB);

			}

		/* Register even numbered buttons as dropSites */
		n = 0;
		XtSetArg(args[n], XmNchildren, &buttons); n++;
		XtSetArg(args[n], XmNnumChildren, &numButtons); n++;
		XtGetValues(BulletinB, args, n);
		for (l = 0; l < numButtons; l++) {

			if (l % 2 != 0)
				continue;

			importList[0] = COMPOUND_TEXT;
			n = 0;
			XtSetArg(args[n], XmNdragProc, HandleDrag); n++;
			XtSetArg(args[n], XmNdropProc, HandleDrop); n++;
			if (l != 2) {
				XtSetArg(args[n], XmNimportTargets, importList); n++;
				XtSetArg(args[n], XmNnumImportTargets, 1); n++;
			}

			if (numDropSites >= MAX_DROPSITES) {

				fprintf(stderr, "Trying to register too many dropSites\n");
#ifdef DEBUG
				fflush(outfp);
				fclose(outfp);
#endif
				exit(0);

			}

			XmDropSiteRegister(buttons[l], args, n);
			dropSites[numDropSites] = buttons[l];
			numDropSites++;

		}

		n = 0;
		XtSetArg(args[n], XmNx, 10); n++;
		XtSetArg(args[n], XmNy, 10 + k * 100); n++;
		XtSetArg(args[n], XmNwidth, k * 100); n++;
		XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
		sprintf(name, "Text%d",i);
		Text = XmCreateText(BulletinB, name, args, n);
		XtManageChild(Text);

		XtPopup(topLevel, XtGrabNone);

	}

	if (use_instruction_box == True) {

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

 	/* Test for CR 5937 */
 		n = 0;
 		XtSetArg(args[n], XmNchildren, &buttons); n++;
 		XtSetArg(args[n], XmNnumChildren, &numButtons); n++;
 		XtGetValues(BulletinB, args, n);
 
 		n = 0;
 		XtSetArg(args[n], XmNwidth, 120); n++;
 		XtSetArg(args[n], XmNheight, 120); n++;
 
 		for (l = 0; l < numButtons; l++) {
 		  XtSetValues(buttons[l], args, n);
 		}
 
 		n = 0;
 		XtSetArg(args[n], XmNdropSiteOperations, XmDROP_NOOP); n++;
 		XmDropSiteUpdate(buttons[2], args, n);
 
 		CommonPause();
 	/* End test for CR 5937 */
		CommonPause();

	}

	XtAppMainLoop(app_context);

}


static Widget  CreatePushButton(Widget parent, char *label)

{

	Widget			widget;
	register int	n;
	Arg				args[3];
	XmString		tcs;

	tcs = XmStringLtoRCreate(label, XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	widget = XmCreatePushButton(parent, label, args, n);
	XtManageChild(widget);

	XmStringFree(tcs);

	return(widget);

}


#define	WORD_LEN	32

ProcessCommandArgs()
{

	char	*user_data;
	char	next_word[WORD_LEN + 1];
	int     num_spaces;

	user_data = UserData;

	while (*user_data != '\0') {

		get_next_word(user_data, next_word, &num_spaces);
		user_data += (strlen(next_word) + num_spaces);

		if (strcmp(next_word, "-I") == 0) {

			get_next_word(user_data, next_word, &num_spaces);

			if (strcmp(next_word, "P") == 0)
				initiatorProtocolStyle = XmDRAG_PREREGISTER;
			else if (strcmp(next_word, "D") == 0)
				initiatorProtocolStyle = XmDRAG_DYNAMIC;
			else if (strcmp(next_word, "N") == 0)
				initiatorProtocolStyle = XmDRAG_NONE;
			else if (strcmp(next_word, "DO") == 0)
				initiatorProtocolStyle = XmDRAG_DROP_ONLY;
			else if (strcmp(next_word, "PD") == 0)
				initiatorProtocolStyle = XmDRAG_PREFER_DYNAMIC;
			else if (strcmp(next_word, "PP") == 0)
				initiatorProtocolStyle = XmDRAG_PREFER_PREREGISTER;
			else if (strcmp(next_word, "PR") == 0)
				initiatorProtocolStyle = XmDRAG_PREFER_RECEIVER;
			else
				initiatorProtocolStyle = XmDRAG_PREFER_RECEIVER;

			user_data += (strlen(next_word) + num_spaces);
			continue;

		}

		if (strcmp(next_word, "-R") == 0) {

			get_next_word(user_data, next_word, &num_spaces);

			if (strcmp(next_word, "P") == 0)
				receiverProtocolStyle = XmDRAG_PREREGISTER;
			else if (strcmp(next_word, "D") == 0)
				receiverProtocolStyle = XmDRAG_DYNAMIC;
			else if (strcmp(next_word, "N") == 0)
				receiverProtocolStyle = XmDRAG_NONE;
			else if (strcmp(next_word, "DO") == 0)
				receiverProtocolStyle = XmDRAG_DROP_ONLY;
			else if (strcmp(next_word, "PD") == 0)
				receiverProtocolStyle = XmDRAG_PREFER_DYNAMIC;
			else if (strcmp(next_word, "PP") == 0)
				receiverProtocolStyle = XmDRAG_PREFER_PREREGISTER;
			/* Prefer Receiver is not allowed if the client is a receiver */
			else
				receiverProtocolStyle = XmDRAG_PREFER_PREREGISTER;

			user_data += (strlen(next_word) + num_spaces);
			continue;

		}

		if (strcmp(next_word, "-noinstruct") == 0) {
			use_instruction_box = False;
			continue;

		}

	}

	free(UserData);

}


get_next_word(source, dest, spaces)
char	*source;
char	*dest;
int		*spaces;
{

	int	n;
	int	space_count;

	space_count = 0;

	while (isspace(*source)) {
		source++;
		space_count++;
	}

	n = 0;
	while (!isspace(*source) && *source != '\0' && n < WORD_LEN) {
		*dest++ = *source++;
		n++;
	}
	*dest = '\0';
	*spaces = space_count;

}
