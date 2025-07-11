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
static char rcsid[] = "$XConsortium: DNDIcon2.c /main/11 1996/10/07 14:51:28 drk $"
#endif
#endif

/*
 *	file: DNDIcon2.c
 *
 *	This tests the various Icons used during a Drag and Drop transaction.
 *	This test needs to be expanded to include changing of icons on the fly,
 *	and test that the new icons correctly override the default icons.
 *
 */

#include <signal.h>
#include <stdio.h>

#include <testlib.h>

#include <Xm/Screen.h>
#include <Xm/DragDrop.h>

#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>

#include "DNDIcons.h"


#define	MAX_DROPSITES	100


static Widget   CreatePushButton();
static void	 	Quit();
static void	 	StartDrag();


enum {DEFAULT_OP, OP_ICON1, OP_ICON2, OP_ICON3};
enum {DEFAULT_STATE, STATE_ICON1, STATE_ICON2, STATE_ICON3};
enum {DEFAULT_SRC, SRC_ICON1, SRC_ICON2, SRC_ICON3};

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

Widget	opIcon1, opIcon2, opIcon3;
Widget	stateIcon1, stateIcon2, stateIcon3;
Widget	srcIcon1, srcIcon2, srcIcon3;

Widget	opIcon, stateIcon, srcIcon;

int		opIcon_value, stateIcon_value, srcIcon_value;

Boolean	use_instruction_box;

unsigned char	initiatorProtocolStyle;
unsigned char	receiverProtocolStyle;

Boolean	use_small_icons;
Boolean	no_background;

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

Widget  WhichOpAttach_Pulldown;
Widget  WhichOpAttach_Option;
Widget	Op_northwest, Op_north, Op_northeast, Op_east, Op_southeast, 
		Op_south, Op_southwest, Op_west, Op_center;

Widget  WhichStateAttach_Pulldown;
Widget  WhichStateAttach_Option;
Widget	State_northwest, State_north, State_northeast, State_east, 
		State_southeast, State_south, State_southwest, State_west, State_center;

Widget  WhichForeground_Pulldown;
Widget  WhichForeground_Option;
Widget	redFG, blackFG, yellowFG, greenFG, whiteFG;

Widget  WhichBackground_Pulldown;
Widget  WhichBackground_Option;
Widget	redBG, blackBG, yellowBG, greenBG, whiteBG;

Widget  WhichOpIcon_Pulldown;
Widget  WhichOpIcon_Option;
Widget	OpIcon1, OpIcon2, OpIcon3, defOpIcon;

Widget  WhichStateIcon_Pulldown;
Widget  WhichStateIcon_Option;
Widget	StateIcon1, StateIcon2, StateIcon3, defStateIcon;

Widget  WhichSrcIcon_Pulldown;
Widget  WhichSrcIcon_Option;
Widget	SrcIcon1, SrcIcon2, SrcIcon3, defSrcIcon;

Widget  ApplyChanges;

unsigned char	animationStyle;
unsigned char	dropActivity;
unsigned char	dropOperations;
unsigned char	dragOperations;
unsigned char	blendModel;
unsigned char	Op_attachment, State_attachment;
Pixel			cursorForeground;
Pixel			cursorBackground;

Pixel	redPixel, blackPixel, yellowPixel, greenPixel, whitePixel;

Widget	dropSites[MAX_DROPSITES];
int		numDropSites;

#ifdef DEBUG
FILE	*outfp;
#endif

Boolean	change_op = False;


/*ARGSUSED*/
static void Quit (void )
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

	Widget					dc;
	Widget					opIcon, stateIcon;
	XmDragMotionCallback	DragMotion;
	Arg						args[MAX_ARGS];
	Cardinal				n;
	static int				count = 0;

	dc = w;
	DragMotion = (XmDragMotionCallback )call_data;

	if (change_op == True) {

		if (count == 0) {

			n = 0;
			XtSetArg(args[n], XmNoperationCursorIcon, &opIcon); n++;
			XtSetArg(args[n], XmNstateCursorIcon, &stateIcon); n++;
			XtGetValues(dc, args, n);

			n = 0;
			XtSetArg(args[n], XmNattachment, Op_attachment); n++;
			XtSetValues(opIcon, args, n);

			n = 0;
			XtSetArg(args[n], XmNattachment, State_attachment); n++;
			XtSetValues(stateIcon, args, n);

			count++;

		}

	}

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
	static int	count = 0;


	if (count == 0) {

		if (use_small_icons == False) {

			defCopyPix = XCreateBitmapFromData(display, XtWindow(w), 
											   defCopy_bits,
									   		   defCopy_width, defCopy_height);
			defMovePix = XCreateBitmapFromData(display, XtWindow(w), 
											   defMove_bits, defMove_width, 
											   defMove_height);
			defLinkPix = XCreateBitmapFromData(display, XtWindow(w), 
											   defLink_bits, defLink_width, 
											   defLink_height);
			defVPix = XCreateBitmapFromData(display, XtWindow(w), defV_bits,
											defV_width, defV_height);
			defInvPix = XCreateBitmapFromData(display, XtWindow(w), defInv_bits,
									  		  defInv_width, defInv_height);
			defNonePix = XCreateBitmapFromData(display, XtWindow(w), 
											   defNone_bits,
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
			XtSetArg(args[n], XmNattachment, Op_attachment); n++;
			defCopyIcon = XmCreateDragIcon(w, "defCopyIcon", args, n);
	
			n = 0;
			XtSetArg(args[n], XmNwidth, defMove_width); n++;
			XtSetArg(args[n], XmNheight, defMove_height); n++;
			XtSetArg(args[n], XmNhotX, defMove_x_hot); n++;
			XtSetArg(args[n], XmNhotY, defMove_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defMovePix); n++;
			XtSetArg(args[n], XmNattachment, Op_attachment); n++;
			defMoveIcon = XmCreateDragIcon(w, "defMoveIcon", args, n);
		
			n = 0;
			XtSetArg(args[n], XmNwidth, defLink_width); n++;
			XtSetArg(args[n], XmNheight, defLink_height); n++;
			XtSetArg(args[n], XmNhotX, defLink_x_hot); n++;
			XtSetArg(args[n], XmNhotY, defLink_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defLinkPix); n++;
			XtSetArg(args[n], XmNattachment, Op_attachment); n++;
			defLinkIcon = XmCreateDragIcon(w, "defLinkIcon", args, n);
	
			n = 0;
			XtSetArg(args[n], XmNwidth, defV_width); n++;
			XtSetArg(args[n], XmNheight, defV_height); n++;
			XtSetArg(args[n], XmNhotX, defV_x_hot); n++;
			XtSetArg(args[n], XmNhotY, defV_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defVPix); n++;
			XtSetArg(args[n], XmNattachment, State_attachment); n++;
			defVIcon = XmCreateDragIcon(w, "defVIcon", args, n);
	
			n = 0;
			XtSetArg(args[n], XmNwidth, defInv_width); n++;
			XtSetArg(args[n], XmNheight, defInv_height); n++;
			XtSetArg(args[n], XmNhotX, defInv_x_hot); n++;
			XtSetArg(args[n], XmNhotY, defInv_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defInvPix); n++;
			XtSetArg(args[n], XmNattachment, State_attachment); n++;
			defInvIcon = XmCreateDragIcon(w, "defInvIcon", args, n);
	
			n = 0;
			XtSetArg(args[n], XmNwidth, defNone_width); n++;
			XtSetArg(args[n], XmNheight, defNone_height); n++;
			XtSetArg(args[n], XmNhotX, defNone_x_hot); n++;
			XtSetArg(args[n], XmNhotY, defNone_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defNonePix); n++;
			XtSetArg(args[n], XmNattachment, State_attachment); n++;
			defNoneIcon = XmCreateDragIcon(w, "defNoneIcon", args, n);
	
			n = 0;
			XtSetArg(args[n], XmNwidth, defSrc_width); n++;
			XtSetArg(args[n], XmNheight, defSrc_height); n++;
			XtSetArg(args[n], XmNhotX, defSrc_x_hot); n++;
			XtSetArg(args[n], XmNhotY, defSrc_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defSrcPix); n++;
			defSrcIcon = XmCreateDragIcon(w, "defSrcIcon", args, n);
	
		}
		else {
			defCopyPix = XCreateBitmapFromData(display, XtWindow(w), 
											   copy_bits,
									   		   copy_width, copy_height);
			defMovePix = XCreateBitmapFromData(display, XtWindow(w), 
											   move_bits, move_width, 
											   move_height);
			defLinkPix = XCreateBitmapFromData(display, XtWindow(w), 
											   link_bits, link_width, 
											   link_height);
			defVPix = XCreateBitmapFromData(display, XtWindow(w), valid_bits,
											valid_width, valid_height);
			defInvPix = XCreateBitmapFromData(display, XtWindow(w), 
											  invalid_bits, invalid_width, 
											  invalid_height);
			defNonePix = XCreateBitmapFromData(display, XtWindow(w), 
											   none_bits, none_width, 
											   none_height);
			defSrcPix = XCreateBitmapFromData(display, XtWindow(w), src_bits,
									  		  src_width, src_height);

			n = 0;
			XtSetArg(args[n], XmNwidth, copy_width); n++;
			XtSetArg(args[n], XmNheight, copy_height); n++;
			XtSetArg(args[n], XmNhotX, copy_x_hot); n++;
			XtSetArg(args[n], XmNhotY, copy_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defCopyPix); n++;
			XtSetArg(args[n], XmNattachment, Op_attachment); n++;
			defCopyIcon = XmCreateDragIcon(w, "defCopyIcon", args, n);
	
			n = 0;
			XtSetArg(args[n], XmNwidth, move_width); n++;
			XtSetArg(args[n], XmNheight, move_height); n++;
			XtSetArg(args[n], XmNhotX, move_x_hot); n++;
			XtSetArg(args[n], XmNhotY, move_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defMovePix); n++;
			XtSetArg(args[n], XmNattachment, Op_attachment); n++;
			defMoveIcon = XmCreateDragIcon(w, "defMoveIcon", args, n);
		
			n = 0;
			XtSetArg(args[n], XmNwidth, link_width); n++;
			XtSetArg(args[n], XmNheight, link_height); n++;
			XtSetArg(args[n], XmNhotX, link_x_hot); n++;
			XtSetArg(args[n], XmNhotY, link_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defLinkPix); n++;
			XtSetArg(args[n], XmNattachment, Op_attachment); n++;
			defLinkIcon = XmCreateDragIcon(w, "defLinkIcon", args, n);
	
			n = 0;
			XtSetArg(args[n], XmNwidth, valid_width); n++;
			XtSetArg(args[n], XmNheight, valid_height); n++;
			XtSetArg(args[n], XmNhotX, valid_x_hot); n++;
			XtSetArg(args[n], XmNhotY, valid_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defVPix); n++;
			XtSetArg(args[n], XmNattachment, State_attachment); n++;
			defVIcon = XmCreateDragIcon(w, "defVIcon", args, n);
	
			n = 0;
			XtSetArg(args[n], XmNwidth, invalid_width); n++;
			XtSetArg(args[n], XmNheight, invalid_height); n++;
			XtSetArg(args[n], XmNhotX, invalid_x_hot); n++;
			XtSetArg(args[n], XmNhotY, invalid_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defInvPix); n++;
			XtSetArg(args[n], XmNattachment, State_attachment); n++;
			defInvIcon = XmCreateDragIcon(w, "defInvIcon", args, n);
	
			n = 0;
			XtSetArg(args[n], XmNwidth, none_width); n++;
			XtSetArg(args[n], XmNheight, none_height); n++;
			XtSetArg(args[n], XmNhotX, none_x_hot); n++;
			XtSetArg(args[n], XmNhotY, none_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defNonePix); n++;
			XtSetArg(args[n], XmNattachment, State_attachment); n++;
			defNoneIcon = XmCreateDragIcon(w, "defNoneIcon", args, n);
	
			n = 0;
			XtSetArg(args[n], XmNwidth, src_width); n++;
			XtSetArg(args[n], XmNheight, src_height); n++;
			XtSetArg(args[n], XmNhotX, src_x_hot); n++;
			XtSetArg(args[n], XmNhotY, src_y_hot); n++;
			XtSetArg(args[n], XmNdepth, 1); n++;
			XtSetArg(args[n], XmNpixmap, defSrcPix); n++;
			defSrcIcon = XmCreateDragIcon(w, "defSrcIcon", args, n);
	
		}
		count++;
		
	}
	else {

		n = 0;
		XtSetArg(args[n], XmNattachment, Op_attachment); n++;
		XtSetValues(defCopyIcon, args, n);
		XtSetValues(defMoveIcon, args, n);
		XtSetValues(defLinkIcon, args, n);

		n = 0;
		XtSetArg(args[n], XmNattachment, State_attachment); n++;
		XtSetValues(defVIcon, args, n);
		XtSetValues(defInvIcon, args, n);
		XtSetValues(defNoneIcon, args, n);

	}

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


static void CreateOtherDragIcons(w)
Widget	w;
{

	Pixmap	opPix, statePix, srcPix;
	int		n;
	Arg		args[MAX_ARGS];
	static int	count = 0;


	if (count == 0) {

		opPix = XCreateBitmapFromData(display, XtWindow(w), opIcon1_bits,
									   opIcon1_width, opIcon1_height);
		n = 0;
		XtSetArg(args[n], XmNwidth, opIcon1_width); n++;
		XtSetArg(args[n], XmNheight, opIcon1_height); n++;
		XtSetArg(args[n], XmNhotX, opIcon1_x_hot); n++;
		XtSetArg(args[n], XmNhotY, opIcon1_y_hot); n++;
		XtSetArg(args[n], XmNdepth, 1); n++;
		XtSetArg(args[n], XmNpixmap, opPix); n++;
		opIcon1 = XmCreateDragIcon(w, "opIcon1", args, n);

		opPix = XCreateBitmapFromData(display, XtWindow(w), opIcon2_bits,
										   opIcon2_width, opIcon2_height);
		n = 0;
		XtSetArg(args[n], XmNwidth, opIcon2_width); n++;
		XtSetArg(args[n], XmNheight, opIcon2_height); n++;
		XtSetArg(args[n], XmNhotX, opIcon2_x_hot); n++;
		XtSetArg(args[n], XmNhotY, opIcon2_y_hot); n++;
		XtSetArg(args[n], XmNdepth, 1); n++;
		XtSetArg(args[n], XmNpixmap, opPix); n++;
		opIcon2 = XmCreateDragIcon(w, "opIcon2", args, n);
	
		opPix = XCreateBitmapFromData(display, XtWindow(w), opIcon3_bits,
										   opIcon3_width, opIcon3_height);
		n = 0;
		XtSetArg(args[n], XmNwidth, opIcon3_width); n++;
		XtSetArg(args[n], XmNheight, opIcon3_height); n++;
		XtSetArg(args[n], XmNhotX, opIcon3_x_hot); n++;
		XtSetArg(args[n], XmNhotY, opIcon3_y_hot); n++;
		XtSetArg(args[n], XmNdepth, 1); n++;
		XtSetArg(args[n], XmNpixmap, opPix); n++;
		opIcon3 = XmCreateDragIcon(w, "opIcon3", args, n);

		statePix = XCreateBitmapFromData(display, XtWindow(w), stateIcon1_bits,
									   stateIcon1_width, stateIcon1_height);
		n = 0;
		XtSetArg(args[n], XmNwidth, stateIcon1_width); n++;
		XtSetArg(args[n], XmNheight, stateIcon1_height); n++;
		XtSetArg(args[n], XmNhotX, stateIcon1_x_hot); n++;
		XtSetArg(args[n], XmNhotY, stateIcon1_y_hot); n++;
		XtSetArg(args[n], XmNdepth, 1); n++;
		XtSetArg(args[n], XmNpixmap, statePix); n++;
		stateIcon1 = XmCreateDragIcon(w, "stateIcon1", args, n);

		statePix = XCreateBitmapFromData(display, XtWindow(w), stateIcon2_bits,
										   stateIcon2_width, stateIcon2_height);
		n = 0;
		XtSetArg(args[n], XmNwidth, stateIcon2_width); n++;
		XtSetArg(args[n], XmNheight, stateIcon2_height); n++;
		XtSetArg(args[n], XmNhotX, stateIcon2_x_hot); n++;
		XtSetArg(args[n], XmNhotY, stateIcon2_y_hot); n++;
		XtSetArg(args[n], XmNdepth, 1); n++;
		XtSetArg(args[n], XmNpixmap, statePix); n++;
		stateIcon2 = XmCreateDragIcon(w, "stateIcon2", args, n);
	
		statePix = XCreateBitmapFromData(display, XtWindow(w), stateIcon3_bits,
										   stateIcon3_width, stateIcon3_height);
		n = 0;
		XtSetArg(args[n], XmNwidth, stateIcon3_width); n++;
		XtSetArg(args[n], XmNheight, stateIcon3_height); n++;
		XtSetArg(args[n], XmNhotX, stateIcon3_x_hot); n++;
		XtSetArg(args[n], XmNhotY, stateIcon3_y_hot); n++;
		XtSetArg(args[n], XmNdepth, 1); n++;
		XtSetArg(args[n], XmNpixmap, statePix); n++;
		stateIcon3 = XmCreateDragIcon(w, "stateIcon3", args, n);

		srcPix = XCreateBitmapFromData(display, XtWindow(w), 
									   srcIcon1_bits,
									   srcIcon1_width, 
									   srcIcon1_height);

		n = 0;
		XtSetArg(args[n], XmNwidth, srcIcon1_width); n++;
		XtSetArg(args[n], XmNheight, srcIcon1_height); n++;
		XtSetArg(args[n], XmNhotX, srcIcon1_x_hot); n++;
		XtSetArg(args[n], XmNhotY, srcIcon1_y_hot); n++;
		XtSetArg(args[n], XmNdepth, 1); n++;
		XtSetArg(args[n], XmNpixmap, srcPix); n++;
		srcIcon1 = XmCreateDragIcon(w, "srcIcon1", args, n);

		srcPix = XCreateBitmapFromData(display, XtWindow(w), srcIcon2_bits,
										   srcIcon2_width, srcIcon2_height);
		n = 0;
		XtSetArg(args[n], XmNwidth, srcIcon2_width); n++;
		XtSetArg(args[n], XmNheight, srcIcon2_height); n++;
		XtSetArg(args[n], XmNhotX, srcIcon2_x_hot); n++;
		XtSetArg(args[n], XmNhotY, srcIcon2_y_hot); n++;
		XtSetArg(args[n], XmNdepth, 1); n++;
		XtSetArg(args[n], XmNpixmap, srcPix); n++;
		srcIcon2 = XmCreateDragIcon(w, "srcIcon2", args, n);
	
		srcPix = XCreateBitmapFromData(display, XtWindow(w), srcIcon3_bits,
										   srcIcon3_width, srcIcon3_height);
		n = 0;
		XtSetArg(args[n], XmNwidth, srcIcon3_width); n++;
		XtSetArg(args[n], XmNheight, srcIcon3_height); n++;
		XtSetArg(args[n], XmNhotX, srcIcon3_x_hot); n++;
		XtSetArg(args[n], XmNhotY, srcIcon3_y_hot); n++;
		XtSetArg(args[n], XmNdepth, 1); n++;
		XtSetArg(args[n], XmNpixmap, srcPix); n++;
		srcIcon3 = XmCreateDragIcon(w, "srcIcon3", args, n);
	
		count++;

	}
	else {

		n = 0;
		XtSetArg(args[n], XmNattachment, Op_attachment); n++;
		XtSetValues(opIcon1, args, n);
		XtSetValues(opIcon2, args, n);
		XtSetValues(opIcon3, args, n);

		n = 0;
		XtSetArg(args[n], XmNattachment, State_attachment); n++;
		XtSetValues(stateIcon1, args, n);
		XtSetValues(stateIcon2, args, n);
		XtSetValues(stateIcon3, args, n);

	}

}


static void SetDefaultDragIcons(w)
Widget	w;
{

	int		n;
	Arg		args[MAX_ARGS];


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

	static Boolean	first_drag = True;
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

	CreateOtherDragIcons(w);

	/*
	 * If it is the first drag initiated, get the current opIcon and
	 * stateIcon values.  This is to avoid calling SetValues on a 
	 * NULL opIcon or stateIcon.
	 */
	if (first_drag == True) {

		if (opIcon_value != DEFAULT_OP) {

			switch(opIcon_value) {

				case OP_ICON1:

					opIcon = opIcon1;
					break;

				case OP_ICON2:

					opIcon = opIcon2;
					break;

				case OP_ICON3:

					opIcon = opIcon3;
					break;

			}

		}

		if (stateIcon_value != DEFAULT_STATE) {

			switch(stateIcon_value) {

				case STATE_ICON1:

					stateIcon = stateIcon1;
					break;

				case STATE_ICON2:

					stateIcon = stateIcon2;
					break;

				case STATE_ICON3:

					stateIcon = stateIcon3;
					break;

			}

		}

		first_drag = False;

	}

	if (opIcon_value != DEFAULT_OP) {

		n = 0;
		XtSetArg(args[n], XmNattachment, Op_attachment); n++;
		XtSetValues(opIcon, args, n);

	}

	if (stateIcon_value != DEFAULT_STATE) {

		n = 0;
		XtSetArg(args[n], XmNattachment, State_attachment); n++;
		XtSetValues(stateIcon, args, n);

	}

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
	if (no_background == False)
		XtSetArg(args[n], XmNcursorBackground, cursorBackground); n++;
	XtSetArg(args[n], XmNdragOperations, dragOperations); n++;
	XtSetArg(args[n], XmNblendModel, blendModel); n++;

	if (opIcon_value != DEFAULT_OP) {
		XtSetArg(args[n], XmNoperationCursorIcon, opIcon); n++;
	}
	if (stateIcon_value != DEFAULT_STATE) {
		XtSetArg(args[n], XmNstateCursorIcon, stateIcon); n++;
	}
	if (srcIcon_value != DEFAULT_SRC) {
		XtSetArg(args[n], XmNsourceCursorIcon, srcIcon); n++;
	}

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

		} 
		else {
			if (DropProc->operation & XmDROP_LINK)
				fprintf(stderr, "Link Operation currently not supported\n");
			XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
			XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
		}

	}

	XtSetArg(args[n], XmNtransferProc, DropTransferCallback); n++;
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
static void Op_northwestCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	Op_attachment = XmATTACH_NORTH_WEST;
	return;

}


/*ARGSUSED*/
static void Op_northCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	Op_attachment = XmATTACH_NORTH;
	return;

}


/*ARGSUSED*/
static void Op_northeastCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	Op_attachment = XmATTACH_NORTH_EAST;
	return;

}


/*ARGSUSED*/
static void Op_eastCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	Op_attachment = XmATTACH_EAST;
	return;

}


/*ARGSUSED*/
static void Op_southeastCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	Op_attachment = XmATTACH_SOUTH_EAST;
	return;

}


/*ARGSUSED*/
static void Op_southCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	Op_attachment = XmATTACH_SOUTH;
	return;

}


/*ARGSUSED*/
static void Op_southwestCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	Op_attachment = XmATTACH_SOUTH_WEST;
	return;

}


/*ARGSUSED*/
static void Op_westCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	Op_attachment = XmATTACH_WEST;
	return;

}


/*ARGSUSED*/
static void Op_centerCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	Op_attachment = XmATTACH_CENTER;
	return;

}


/*ARGSUSED*/
static void State_northwestCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	State_attachment = XmATTACH_NORTH_WEST;
	return;

}


/*ARGSUSED*/
static void State_northCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	State_attachment = XmATTACH_NORTH;
	return;

}


/*ARGSUSED*/
static void State_northeastCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	State_attachment = XmATTACH_NORTH_EAST;
	return;

}


/*ARGSUSED*/
static void State_eastCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	State_attachment = XmATTACH_EAST;
	return;

}


/*ARGSUSED*/
static void State_southeastCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	State_attachment = XmATTACH_SOUTH_EAST;
	return;

}


/*ARGSUSED*/
static void State_southCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	State_attachment = XmATTACH_SOUTH;
	return;

}


/*ARGSUSED*/
static void State_southwestCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	State_attachment = XmATTACH_SOUTH_WEST;
	return;

}


/*ARGSUSED*/
static void State_westCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	State_attachment = XmATTACH_WEST;
	return;

}


/*ARGSUSED*/
static void State_centerCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	State_attachment = XmATTACH_CENTER;
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
static void OpIcon1CB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	opIcon_value = OP_ICON1;
	opIcon = opIcon1;
	return;

}


/*ARGSUSED*/
static void OpIcon2CB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	opIcon_value = OP_ICON2;
	opIcon = opIcon2;
	return;

}


/*ARGSUSED*/
static void OpIcon3CB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	opIcon_value = OP_ICON3;
	opIcon = opIcon3;
	return;

}


/*ARGSUSED*/
static void defOpIconCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	opIcon_value = DEFAULT_OP;
	return;

}


/*ARGSUSED*/
static void StateIcon1CB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	stateIcon_value = STATE_ICON1;
	stateIcon = stateIcon1;
	return;
}


/*ARGSUSED*/
static void StateIcon2CB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	stateIcon_value = STATE_ICON2;
	stateIcon = stateIcon2;
	return;

}


/*ARGSUSED*/
static void StateIcon3CB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	stateIcon_value = STATE_ICON3;
	stateIcon = stateIcon3;
	return;

}


/*ARGSUSED*/
static void defStateIconCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	stateIcon_value = DEFAULT_STATE;
	return;

}


/*ARGSUSED*/
static void SrcIcon1CB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	srcIcon_value = SRC_ICON1;
	srcIcon = srcIcon1;
	return;
}


/*ARGSUSED*/
static void SrcIcon2CB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	srcIcon_value = SRC_ICON2;
	srcIcon = srcIcon2;
	return;

}


/*ARGSUSED*/
static void SrcIcon3CB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	srcIcon_value = SRC_ICON3;
	srcIcon = srcIcon3;
	return;

}


/*ARGSUSED*/
static void defSrcIconCB(Widget w, XtPointer client_data, 
						   XtPointer call_data)
{

	srcIcon_value = DEFAULT_SRC;
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
	use_small_icons = False;
	no_background = False;

	initiatorProtocolStyle = XmDRAG_PREFER_RECEIVER;
	receiverProtocolStyle = XmDRAG_PREFER_PREREGISTER;
	animationStyle = XmDRAG_UNDER_HIGHLIGHT;
	dropActivity = XmDROP_SITE_ACTIVE;
	dropOperations = XmDROP_MOVE | XmDROP_COPY;
	dragOperations = XmDROP_COPY;
	blendModel = XmBLEND_ALL;
	Op_attachment = XmATTACH_SOUTH_EAST;
	State_attachment = XmATTACH_NORTH_EAST;

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

	xmScreen = XmGetXmScreen(screen);

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
	WhichOpAttach_Pulldown = XmCreatePulldownMenu(Shell1, 
												  "WhichOpAttach_Pulldown", 
												  args, n);

	Op_northwest = CreatePushButton(WhichOpAttach_Pulldown, "NorthWest");
	Op_north = CreatePushButton(WhichOpAttach_Pulldown, "North");
	Op_northeast = CreatePushButton(WhichOpAttach_Pulldown, "NorthEast");
	Op_east = CreatePushButton(WhichOpAttach_Pulldown, "East");
	Op_southeast = CreatePushButton(WhichOpAttach_Pulldown, "SouthEast");
	Op_south = CreatePushButton(WhichOpAttach_Pulldown, "South");
	Op_southwest = CreatePushButton(WhichOpAttach_Pulldown, "SouthWest");
	Op_west = CreatePushButton(WhichOpAttach_Pulldown, "West");
	Op_center = CreatePushButton(WhichOpAttach_Pulldown, "Center");

	XtAddCallback(Op_northwest, XmNactivateCallback, Op_northwestCB, NULL);
	XtAddCallback(Op_north, XmNactivateCallback, Op_northCB, NULL);
	XtAddCallback(Op_northeast, XmNactivateCallback, Op_northeastCB, NULL);
	XtAddCallback(Op_east, XmNactivateCallback, Op_eastCB, NULL);
	XtAddCallback(Op_southeast, XmNactivateCallback, Op_southeastCB, NULL);
	XtAddCallback(Op_south, XmNactivateCallback, Op_southCB, NULL);
	XtAddCallback(Op_southwest, XmNactivateCallback, Op_southwestCB, NULL);
	XtAddCallback(Op_west, XmNactivateCallback, Op_westCB, NULL);
	XtAddCallback(Op_center, XmNactivateCallback, Op_centerCB, NULL);

    tcs = XmStringLtoRCreate("OpIcon Attachment   ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, Op_southeast); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichOpAttach_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichBlend_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichOpAttach_Option = XmCreateOptionMenu(Form, "WhichOpAttach_Option", 
										   args, n);
	XtManageChild(WhichOpAttach_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichStateAttach_Pulldown = XmCreatePulldownMenu(Shell1, 
												  "WhichStateAttach_Pulldown", 
												  args, n);

	State_northwest = CreatePushButton(WhichStateAttach_Pulldown, "NorthWest");
	State_north = CreatePushButton(WhichStateAttach_Pulldown, "North");
	State_northeast = CreatePushButton(WhichStateAttach_Pulldown, "NorthEast");
	State_east = CreatePushButton(WhichStateAttach_Pulldown, "East");
	State_southeast = CreatePushButton(WhichStateAttach_Pulldown, "SouthEast");
	State_south = CreatePushButton(WhichStateAttach_Pulldown, "South");
	State_southwest = CreatePushButton(WhichStateAttach_Pulldown, "SouthWest");
	State_west = CreatePushButton(WhichStateAttach_Pulldown, "West");
	State_center = CreatePushButton(WhichStateAttach_Pulldown, "Center");

	XtAddCallback(State_northwest, XmNactivateCallback, State_northwestCB, 
				  NULL);
	XtAddCallback(State_north, XmNactivateCallback, State_northCB, NULL);
	XtAddCallback(State_northeast, XmNactivateCallback, State_northeastCB, 
				  NULL);
	XtAddCallback(State_east, XmNactivateCallback, State_eastCB, NULL);
	XtAddCallback(State_southeast, XmNactivateCallback, State_southeastCB, 
				  NULL);
	XtAddCallback(State_south, XmNactivateCallback, State_southCB, NULL);
	XtAddCallback(State_southwest, XmNactivateCallback, State_southwestCB, 
				  NULL);
	XtAddCallback(State_west, XmNactivateCallback, State_westCB, NULL);
	XtAddCallback(State_center, XmNactivateCallback, State_centerCB, NULL);

    tcs = XmStringLtoRCreate("StateIcon Attachment ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, State_northeast); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichStateAttach_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichOpAttach_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichStateAttach_Option = XmCreateOptionMenu(Form, 
												 "WhichStateAttach_Option", 
										   		 args, n);
	XtManageChild(WhichStateAttach_Option);

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
	XtSetArg(args[n], XmNtopWidget, WhichStateAttach_Option); n++;
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
	WhichOpIcon_Pulldown = XmCreatePulldownMenu(Shell1, "WhichOpIcon_Pulldown", 
												args, n);

	OpIcon1 = CreatePushButton(WhichOpIcon_Pulldown, "OpIcon1");
	OpIcon2 = CreatePushButton(WhichOpIcon_Pulldown, "OpIcon2");
	OpIcon3 = CreatePushButton(WhichOpIcon_Pulldown, "OpIcon3");
	defOpIcon = CreatePushButton(WhichOpIcon_Pulldown, "default");

	XtAddCallback(OpIcon1, XmNactivateCallback, OpIcon1CB, NULL);
	XtAddCallback(OpIcon2, XmNactivateCallback, OpIcon2CB, NULL);
	XtAddCallback(OpIcon3, XmNactivateCallback, OpIcon3CB, NULL);
	XtAddCallback(defOpIcon, XmNactivateCallback, defOpIconCB, NULL);

    tcs = XmStringLtoRCreate("Op Icon               ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, defOpIcon); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichOpIcon_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichBackground_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichOpIcon_Option = XmCreateOptionMenu(Form, "WhichOpIcon_Option", 
										   	args, n);
	XtManageChild(WhichOpIcon_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichStateIcon_Pulldown = XmCreatePulldownMenu(Shell1, 
												  "WhichStateIcon_Pulldown", 
												  args, n);

	StateIcon1 = CreatePushButton(WhichStateIcon_Pulldown, "StateIcon1");
	StateIcon2 = CreatePushButton(WhichStateIcon_Pulldown, "StateIcon2");
	StateIcon3 = CreatePushButton(WhichStateIcon_Pulldown, "StateIcon3");
	defStateIcon = CreatePushButton(WhichStateIcon_Pulldown, "default");

	XtAddCallback(StateIcon1, XmNactivateCallback, StateIcon1CB, NULL);
	XtAddCallback(StateIcon2, XmNactivateCallback, StateIcon2CB, NULL);
	XtAddCallback(StateIcon3, XmNactivateCallback, StateIcon3CB, NULL);
	XtAddCallback(defStateIcon, XmNactivateCallback, defStateIconCB, NULL);

    tcs = XmStringLtoRCreate("State Icon            ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, defStateIcon); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichStateIcon_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichOpIcon_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichStateIcon_Option = XmCreateOptionMenu(Form, "WhichStateIcon_Option", 
										   		args, n);
	XtManageChild(WhichStateIcon_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen"));	n++;
	WhichSrcIcon_Pulldown = XmCreatePulldownMenu(Shell1, 
												  "WhichSrcIcon_Pulldown", 
												  args, n);

	SrcIcon1 = CreatePushButton(WhichSrcIcon_Pulldown, "SrcIcon1");
	SrcIcon2 = CreatePushButton(WhichSrcIcon_Pulldown, "SrcIcon2");
	SrcIcon3 = CreatePushButton(WhichSrcIcon_Pulldown, "SrcIcon3");
	defSrcIcon = CreatePushButton(WhichSrcIcon_Pulldown, "default");

	XtAddCallback(SrcIcon1, XmNactivateCallback, SrcIcon1CB, NULL);
	XtAddCallback(SrcIcon2, XmNactivateCallback, SrcIcon2CB, NULL);
	XtAddCallback(SrcIcon3, XmNactivateCallback, SrcIcon3CB, NULL);
	XtAddCallback(defSrcIcon, XmNactivateCallback, defSrcIconCB, NULL);

    tcs = XmStringLtoRCreate("Src Icon              ",
							 XmSTRING_DEFAULT_CHARSET);

	n = 0;
	XtSetArg(args[n], XmNlabelString, tcs); n++;
	XtSetArg(args[n], XmNmenuHistory, defSrcIcon); n++;
	XtSetArg(args[n], XmNsubMenuId, WhichSrcIcon_Pulldown); n++;
	XtSetArg(args[n], XmNbackground, CommonGetColor("darkgreen")); n++;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopWidget, WhichStateIcon_Option); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
	WhichSrcIcon_Option = XmCreateOptionMenu(Form, "WhichSrcIcon_Option", 
										   		args, n);
	XtManageChild(WhichSrcIcon_Option);

	XmStringFree(tcs);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNtopWidget, WhichSrcIcon_Option); n++;
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

	opIcon_value = DEFAULT_OP;
	stateIcon_value = DEFAULT_STATE;
	srcIcon_value = DEFAULT_SRC;

	if (use_instruction_box == True) {

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

		CommonPause();

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

		if (strcmp(next_word, "-smallicons") == 0) {
			use_small_icons = True;
			continue;

		}

		if (strcmp(next_word, "-nobackground") == 0) {
			no_background = True;
			continue;

		}

		if (strcmp(next_word, "-changeop") == 0) {
			change_op = True;
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
