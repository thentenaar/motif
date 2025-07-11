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
static char rcsid[] = "$XConsortium: DNDProt1.c /main/10 1996/10/07 14:49:40 drk $"
#endif
#endif

/*
 *	file: DNDProt1.c
 *
 *	Program to test the different protocols being used in DND transactions.
 *	The resources:
 *		initiatorProtocolStyle
 *		receiverProtocolStyle
 *
 *	This basically tests the fact that unless the client application
 *	specifies its own animationStyle, there should be no user-perceptible
 *	difference during a DND transaction, irrespective of whether the
 *	active Protocol style is DYNAMIC or PRE-REGISTER.
 *
 */

#include <signal.h>
#include <stdio.h>

#include <testlib.h>

#include <Xm/DragDrop.h>

#include <X11/keysym.h>
#include <X11/Xatom.h>


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

Boolean	use_instruction_box;

unsigned char	initiatorProtocolStyle;
unsigned char	receiverProtocolStyle;

Widget	  Form;

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

Widget  ApplyChanges;

unsigned char	animationStyle;
unsigned char	dropActivity;
unsigned char	dropOperations;
unsigned char	dragOperations;

Widget	dropSites[MAX_DROPSITES];
int		numDropSites;

#ifdef DEBUG
FILE	*outfp;
#endif


static void Quit()
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


/*ARGSUSED*/
static void DragMotionCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragMotionCallback	DragMotion;


	DragMotion = (XmDragMotionCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "Drag Motion! \n");
	fprintf(outfp, "operation = %d\n", DragMotion->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DragMotion->dropSiteStatus);
	fprintf(outfp, "x = %d\n", DragMotion->x);
	fprintf(outfp, "y = %d\n", DragMotion->y);
#endif

}


/*ARGSUSED*/
static void DnDFinishCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	Widget						dc;
	XmDragDropFinishCallback	DndFinish;
	Widget						source_icon, state_icon, op_icon;
	Arg							args[MAX_ARGS];
	int							n;

	dc = w;
	DndFinish = (XmDragDropFinishCallback )call_data;
	source_icon = state_icon = op_icon = NULL;

	n = 0;
	XtSetArg(args[n], XmNsourceCursorIcon, &source_icon); n++;
	XtSetArg(args[n], XmNstateCursorIcon, &state_icon); n++;
	XtSetArg(args[n], XmNoperationCursorIcon, &op_icon); n++;
	XtGetValues(dc, args, n);

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


/*ARGSUSED*/
static void DropFinishCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDropFinishCallback	DropFinish;


	DropFinish = (XmDropFinishCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "Drop Finish! \n");
	fprintf(outfp, "operation = %d\n", DropFinish->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DropFinish->dropSiteStatus);
	fprintf(outfp, "dropAction = %d\n", DropFinish->dropAction);
	fprintf(outfp, "completionStatus = %d\n", DropFinish->completionStatus);
#endif

}


/*ARGSUSED*/
static void DropSiteEnterCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDropSiteEnterCallback	DropSiteEnter;


	DropSiteEnter = (XmDropSiteEnterCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "Drop Site Enter! \n");
	fprintf(outfp, "operation = %d\n", DropSiteEnter->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DropSiteEnter->dropSiteStatus);
	fprintf(outfp, "x = %d\n", DropSiteEnter->x);
	fprintf(outfp, "y = %d\n", DropSiteEnter->y);
#endif

}


/*ARGSUSED*/
static void DropSiteLeaveCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDropSiteLeaveCallback	DropSiteLeave;


	DropSiteLeave = (XmDropSiteLeaveCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "Drop Site Leave! \n");
#endif

}


/*ARGSUSED*/
static void DropStartCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDropStartCallback	DropStart;


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


/*ARGSUSED*/
static void TopLEnterCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmTopLevelEnterCallback	TopLEnter;


	TopLEnter = (XmTopLevelEnterCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "TopLevel Enter! \n");
	fprintf(outfp, "window = %ld\n", TopLEnter->window);
	fprintf(outfp, "x = %d\n", TopLEnter->x);
	fprintf(outfp, "y = %d\n", TopLEnter->y);
	fprintf(outfp, "dragProtocolStyle = %d\n", TopLEnter->dragProtocolStyle);
#endif

}


/*ARGSUSED*/
static void TopLLeaveCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmTopLevelLeaveCallback	TopLLeave;


	TopLLeave = (XmTopLevelLeaveCallback )call_data;

#ifdef DEBUG
	fprintf(outfp, "TopLevel Leave! \n");
	fprintf(outfp, "window = %ld\n", TopLLeave->window);
#endif

}


/*ARGSUSED*/
static void StartDrag(w, event)
Widget	w;
XEvent	*event;
{

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


	exportList[0] = COMPOUND_TEXT;
	n = 0;
	XtSetArg(args[n], XmNconvertProc, DragConvertProc); n++;
	XtSetArg(args[n], XmNdragDropFinishCallback, DnDFinishCB); n++;
	XtSetArg(args[n], XmNdropFinishCallback, DropFinishCB); n++;
	XtSetArg(args[n], XmNdropSiteEnterCallback, DropSiteEnterCB); n++;
	XtSetArg(args[n], XmNdropSiteLeaveCallback, DropSiteLeaveCB); n++;
	XtSetArg(args[n], XmNdropStartCallback, DropStartCB); n++;
	XtSetArg(args[n], XmNtopLevelEnterCallback, TopLEnterCB); n++;
	XtSetArg(args[n], XmNtopLevelLeaveCallback, TopLLeaveCB); n++;

	XtSetArg(args[n], XmNexportTargets, exportList); n++;
	XtSetArg(args[n], XmNnumExportTargets, 1); n++;

	XtSetArg(args[n], XmNdragOperations, dragOperations); n++;
	myDC = XmDragStart(w, event, args, n);

}


/*ARGSUSED*/
static void HandleDrag(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragProcCallback	DragProc;


	DragProc = (XmDragProcCallback)call_data;

#ifdef DEBUG
	fprintf(outfp, "DragProc !\n");
	fprintf(outfp, "reason = %d\n", DragProc->reason);
#endif

}


/*ARGSUSED*/
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


/*ARGSUSED*/
static void HandleDrop(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDropProcCallback		DropProc;
	Widget				 	dc;
	XmDropTransferEntryRec	transferEntries[2];
	XmDropTransferEntry		transferList;
	Cardinal			 	numTransfers = 0;
	Cardinal			 	numExportTargets;
	Atom				 	*exportTargets;
	Cardinal			 	numImportTargets;
	Atom				 	*importTargets;
	Arg					 	args[MAX_ARGS];
	int						n;
	char					*string = "TestString";
#ifdef DEBUG
	unsigned char			dragOp, dropSiteOp;
#endif


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
	fprintf(outfp, "operations = %d\n", DropProc->operations);
	fprintf(outfp, "operation = %d\n", DropProc->operation);
	fprintf(outfp, "dropAction = %d\n", DropProc->dropAction);
#endif

	transferEntries[0].target = COMPOUND_TEXT;
	transferEntries[0].client_data = string;
#ifdef DEBUG
	n = 0;
	XtSetArg(args[n], XmNdropSiteOperations, &dropSiteOp); n++;
	XmDropSiteRetrieve(w, args, n);
	fprintf(outfp, "operations = %d\n", DropProc->operations);
	fprintf(outfp, "operation = %d\n", DropProc->operation);
	fprintf(outfp, "dragOp = %d\n", dragOp);
	fprintf(outfp, "dropSiteOp = %d\n", dropSiteOp);
#endif
	n = 0;
	if (XmTargetsAreCompatible(display, exportTargets, numExportTargets,
							   importTargets, numImportTargets) == False) {
		XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
		XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
	}
	else {

		if (DropProc->operation & XmDROP_MOVE || 
			DropProc->operation & XmDROP_COPY) {
		
			transferList = transferEntries;
			numTransfers = 1;
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


/*************End of CALLBACK PROCEDURES****************/

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

	Arg		 	args[MAX_ARGS];
	int		 	n, i, j, k, l;
	AppDataRec	appDataRec, *appData;
	Widget		topLevel, BulletinB, PushBtn, Text;
	XEvent		event;
	Widget		*buttons;
	Cardinal	numButtons;
	char		name[80];
	Atom		importList[1];
	XmString	tcs;


	use_instruction_box = True;

	appData = &appDataRec;

	initiatorProtocolStyle = XmDRAG_PREFER_RECEIVER;
	receiverProtocolStyle = XmDRAG_PREFER_PREREGISTER;
	animationStyle = XmDRAG_UNDER_HIGHLIGHT;
	dropActivity = XmDROP_SITE_ACTIVE;
	dropOperations = XmDROP_MOVE | XmDROP_COPY;
	dragOperations = XmDROP_MOVE | XmDROP_COPY;

	sprintf(name, "%s.out", argv[0]);

#ifdef DEBUG
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
	XtSetArg(args[n], XmNmenuHistory, dragMoveCopy); n++;
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
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNtopWidget, WhichdragOp_Option); n++;
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
				XtSetArg(args[n], XtNx, 10 + j * 100); n++;
				XtSetArg(args[n], XtNy, 10 + k * 100); n++;
				XtSetArg(args[n], XtNwidth, 90); n++;
				XtSetArg(args[n], XtNheight, 90); n++;
				if (appData->applyOverrides) {
					XtSetArg(args[n], XtNtranslations, 
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
			n = 0;
			XtSetArg(args[n], XmNdragProc, HandleDrag); n++;
			XtSetArg(args[n], XmNdropProc, HandleDrop); n++;
			if (l == 2 || (l % 4 == 0)) {
				importList[0] = COMPOUND_TEXT;
				XtSetArg(args[n], XmNimportTargets, importList); n++;
				XtSetArg(args[n], XmNnumImportTargets, 1); n++;
				if (receiverProtocolStyle == XmDRAG_DYNAMIC) {
					XtSetArg(args[n], XmNanimationStyle, 
							 XmDRAG_UNDER_SHADOW_OUT); n++;
				}
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
		XtSetArg(args[n], XtNx, 10); n++;
		XtSetArg(args[n], XtNy, 10 + k * 100); n++;
		XtSetArg(args[n], XtNwidth, k * 100); n++;
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
