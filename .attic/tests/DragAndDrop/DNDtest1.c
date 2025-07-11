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
static char rcsid[] = "$XConsortium: DNDtest1.c /main/9 1996/10/07 15:17:06 drk $"
#endif
#endif
/*
 *	file: DNDtest1.c
 *
 *	This is a basic test for DND.
 *
 */

#include <signal.h>
#include <stdio.h>

#include <testlib.h>

#include <Xm/DragDrop.h>

#include <X11/keysym.h>
#include <X11/Xatom.h>

/* global variables */
Widget	myDC;
Atom	COMPOUND_TEXT;
Atom	DELETE;
Atom	NULL_ATOM;

Boolean	use_instruction_box;

FILE	*outfp;


Quit ()
{

	printf("Begin exiting ... please standby ... \n");
	fflush(outfp);
	fclose(outfp);
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

	fprintf(outfp, "Drag Motion! \n");
	fprintf(outfp, "operation = %d\n", DragMotion->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DragMotion->dropSiteStatus);
	fprintf(outfp, "x = %d\n", DragMotion->x);
	fprintf(outfp, "y = %d\n", DragMotion->y);

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
	XtGetValues((Widget)dc, args, n);

	if (source_icon != NULL)
		XtDestroyWidget(source_icon);
	if (state_icon != NULL)
		XtDestroyWidget(state_icon);
	if (op_icon != NULL)
		XtDestroyWidget(op_icon);

	fprintf(outfp, "DND Finish! \n");

}


static void DropFinishCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDropFinishCallback	DropFinish;
	Arg					args[MAX_ARGS];
	Cardinal			n;


	DropFinish = (XmDropFinishCallback )call_data;


	fprintf(outfp, "Drop Finish! \n");
	fprintf(outfp, "operation = %d\n", DropFinish->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DropFinish->dropSiteStatus);
	fprintf(outfp, "dropAction = %d\n", DropFinish->dropAction);
	fprintf(outfp, "completionStatus = %d\n", DropFinish->completionStatus);

}


static void DropSiteEnterCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDropSiteEnterCallback	DropSiteEnter;

	DropSiteEnter = (XmDropSiteEnterCallback )call_data;

	fprintf(outfp, "Drop Site Enter! \n");
	fprintf(outfp, "operation = %d\n", DropSiteEnter->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DropSiteEnter->dropSiteStatus);
	fprintf(outfp, "x = %d\n", DropSiteEnter->x);
	fprintf(outfp, "y = %d\n", DropSiteEnter->y);

}


static void DropSiteLeaveCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDropSiteLeaveCallback	DropSiteLeave;


	DropSiteLeave = (XmDropSiteLeaveCallback )call_data;

	fprintf(outfp, "Drop Site Leave! \n");

}


static void DropStartCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDropStartCallback	DropStart;

	DropStart = (XmDropStartCallback )call_data;


	fprintf(outfp, "Drop Start! \n");
	fprintf(outfp, "operation = %d\n", DropStart->operation);
	fprintf(outfp, "dropSiteStatus = %d\n", DropStart->dropSiteStatus);
	fprintf(outfp, "dropAction = %d\n", DropStart->dropAction);
	fprintf(outfp, "x = %d\n", DropStart->x);
	fprintf(outfp, "y = %d\n", DropStart->y);

}


static void TopLEnterCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmTopLevelEnterCallback	TopLEnter;


	TopLEnter = (XmTopLevelEnterCallback )call_data;

	fprintf(outfp, "TopLevel Enter! \n");
	fprintf(outfp, "window = %ld\n", TopLEnter->window);
	fprintf(outfp, "x = %d\n", TopLEnter->x);
	fprintf(outfp, "y = %d\n", TopLEnter->y);
	fprintf(outfp, "dragProtocolStyle = %d\n", TopLEnter->dragProtocolStyle);

}


static void TopLLeaveCallback(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmTopLevelLeaveCallback	TopLLeave;


	TopLLeave = (XmTopLevelLeaveCallback )call_data;

	fprintf(outfp, "TopLevel Leave! \n");
	fprintf(outfp, "window = %ld\n", TopLLeave->window);

}


static void StartDrag(w, event)
Widget	w;
XEvent	*event;
{

	Widget	dragSource;
	Arg		args[MAX_ARGS];
	Cardinal	n;
	Atom	exportList[1];
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

	XtSetArg(args[n], XmNdragOperations, XmDROP_COPY); n++;

	myDC = XmDragStart(w, event, args, n);

}


static void HandleDrag(w, client_data, call_data)
Widget		w;
XtPointer	client_data, call_data;
{

	XmDragProcCallback	DragProc;

	DragProc = (XmDragProcCallback)call_data;

	fprintf(outfp, "DragProc !\n");
	fprintf(outfp, "x = %d\n", DragProc->x);
	fprintf(outfp, "y = %d\n", DragProc->y);
	fprintf(outfp, "dropSiteStatus = %d\n", DragProc->dropSiteStatus);
	fprintf(outfp, "operations = %d\n", DragProc->operations);

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
	Cardinal			 	numImportTargets;
	Atom				 	*importTargets;
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

	fprintf(outfp, "DropProc !\n");
	fprintf(outfp, "x = %d\n", DropProc->x);
	fprintf(outfp, "y = %d\n", DropProc->y);
	fprintf(outfp, "dropSiteStatus = %d\n", DropProc->dropSiteStatus);
	fprintf(outfp, "operation = %d\n", DropProc->operation);
	fprintf(outfp, "dropAction = %d\n", DropProc->dropAction);

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
		
			transferList = transferEntries;
			numTransfers = 1;
			XtSetArg(args[n], XmNdropTransfers, transferList); n++;
			XtSetArg(args[n], XmNnumDropTransfers, numTransfers); n++;

		} 
		else {
			XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
			XtSetArg(args[n], XmNnumDropTransfers, 0); n++;
		}
	}

	XtSetArg(args[n], XmNtransferProc, DropTransferCallback); n++;
	XmDropTransferStart(dc, args, n);

}


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


/*************End of CALLBACK PROCEDURES****************/

static int ProtoError(dpy, ev)
Display	*dpy;
XErrorEvent	*ev;
{
	fprintf(stderr, "Proto Error\n");
	fflush(outfp);
	fclose(outfp);
	exit(0);
}


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


void main (argc, argv)
unsigned int argc;
char **argv;

{

	Arg 		args[MAX_ARGS];
	int 		n, i, j, k, l;
	AppDataRec	appDataRec, *appData;
	Widget	 	topLevel, BulletinB, PushBtn, Text;
	XEvent		event;
	Widget		*buttons;
	Cardinal	numButtons;
	char		name[80];
	Atom		importList[1];


	use_instruction_box = True;

	appData = &appDataRec;

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
	XtSetValues(Shell1, args, n);

	XtRealizeWidget(Shell1);

	COMPOUND_TEXT = XmInternAtom(XtDisplay(Shell1), "COMPOUND_TEXT", False);

	DELETE = XmInternAtom(XtDisplay(Shell1), "DELETE", False);
	NULL_ATOM = XmInternAtom(XtDisplay(Shell1), "NULL", False);

	for (i = 0; i < appData->numShells; i++) {

		sprintf(name, "topLevel%d",i);
		n = 0;
		topLevel = XtCreatePopupShell(name, topLevelShellWidgetClass, Shell1, 
								   args, n);

		sprintf(name, "BulletinB%d",i);
		n = 0;
		BulletinB = XmCreateBulletinBoard(topLevel, name, args, n);
		XtManageChild(BulletinB);

		/* Create numRows x numColumns number of pushButtons */
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
			importList[0] = COMPOUND_TEXT;
			if (l != 2) {
				XtSetArg(args[n], XmNimportTargets, importList); n++;
				XtSetArg(args[n], XmNnumImportTargets, 1); n++;
			}
			XmDropSiteRegister(buttons[l], args, n);

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

	}

	XtAppMainLoop(app_context);

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
