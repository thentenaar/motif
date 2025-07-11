/* $TOG: TransFamilyM1.c /main/7 1999/08/02 13:56:31 mgreess $ */
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
/*
 *	file: TransFamilyM1.c
 *
 *	This tests some of the 1.2 enhancements for mwm.
 *
 */

#include <testlib.h>
#include <X11/Shell.h>
#include <Xm/MwmUtil.h>

#ifdef NO_TEST
#define CommonPause() /**/
Widget Shell1;
XtAppContext app_context;
#define CommonTestInit(argc,argv) {Shell1 = XtAppInitialize(&app_context, "Testing", NULL, 0, &argc,argv, NULL, NULL, 0); }
#endif

#define	MAX_DIALOGS	50
#define	NAME_LEN	255

char	*ModelessMsg = 
	"While this box is up, you should be able to use all other shells";
char	*PrimaryMsg = 
	"While this box is up, you should not be able to use its parent";
char	*FullAppMsg = 
	"While this box is up, you should not be able to use either TopShell1 \
or TopShell2 or any other dialog box.";
char	*SystemMsg =
	"While this box is up, you should not be able to use any other \
application nor any other part of this client.";

char	*MenuStrDia1 = "Beep f.beep";


typedef struct {
	Widget	Dialog;
	int		DialogIndex;
	Widget	Parent;
} DialogStruct;

DialogStruct	ModelessDia[MAX_DIALOGS];
DialogStruct	PrimaryDia[MAX_DIALOGS];
DialogStruct	FullAppDia[MAX_DIALOGS];
DialogStruct	SystemDia[MAX_DIALOGS];

int		ModelessDialogCount = 0;
int		PrimaryDialogCount = 0;
int		FullAppDialogCount = 0;
int		SystemDialogCount = 0;

#ifndef MOTIF1_1

Bool Test1_2 = False;

char	*MeuStrDia2 = "LowerWithin f.lower within\n\
						LowerFree f.lower freeFamily\n\
						RaiseWithin f.raise within\n\
						RaiseFree f.raise freeFamily";

#endif


static void
RemoveModelessChildren(int curr_index, Widget curr_dialog)
{

	int	count;

	for (count = curr_index; count < MAX_DIALOGS; count++) {

		if (ModelessDia[count].Parent == curr_dialog) {
			if (ModelessDia[count].DialogIndex != -1) {
				RemoveModelessChildren(count, ModelessDia[count].Dialog);
				ModelessDia[count].DialogIndex = -1;
			}
		}

	}

}


/* callback procedures */

static void
DestroyModelessDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

	DialogStruct	*Dialog;
	int		DialogIndex;
	int		i;


	Dialog = (DialogStruct *)client_data;
	DialogIndex = Dialog->DialogIndex;


	RemoveModelessChildren(DialogIndex, ModelessDia[DialogIndex].Dialog);

	XtDestroyWidget(ModelessDia[DialogIndex].Dialog);

	for (i = 0; i < MAX_DIALOGS; i++) {

		if (ModelessDia[i].DialogIndex == -1) {
			if (ModelessDia[i].Dialog != NULL)
				XtDestroyWidget((char *)ModelessDia[i].Dialog);
			ModelessDia[i].Dialog = NULL;
		}

	}

	ModelessDia[DialogIndex].Dialog = NULL;
	ModelessDia[DialogIndex].DialogIndex = -1;
	ModelessDialogCount--;

}


static void
PostModelessDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

    Widget	parent, spawnButton, okButton, cancelButton;
    int 	n;
    Arg 	args[MAX_ARGS];
    XmString tcs;
	char	title[NAME_LEN + 1];
	int		DialogCount;

	title[0] = '\0';

	if (ModelessDialogCount >= MAX_DIALOGS) {
		fprintf(stderr, "Too many modeless dialogs\n");
		return;
	}

	for (DialogCount = 0; DialogCount < MAX_DIALOGS; DialogCount++)
		if (ModelessDia[DialogCount].DialogIndex == -1)
			break;
	if (DialogCount == MAX_DIALOGS) {
		fprintf(stderr, "No more modeless dialogs\n");
		return;
	}
	sprintf(title, "Modeless%d", DialogCount);
    parent = (Widget) client_data;

#ifndef MOTIF1_1
	if (Test1_2) 
		{ 
		while (!XtIsTopLevelShell(parent))
			parent = XtParent(parent);
		}
#endif

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, 
			  XmStringCreateSimple (ModelessMsg)); n++;
    XtSetArg (args[n], XmNhelpLabelString, 
			  XmStringCreateSimple ("Spawn Dialog Child")); n++;

    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, title); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_MODELESS); n++;

	/* VendorShell resources */
#ifndef MOTIF1_1

	if (Test1_2 == True) {
		XtSetArg(args[n], XmNmwmMenu, MeuStrDia2); n++;
	}
	else {
		XtSetArg(args[n], XmNmwmMenu, MenuStrDia1); n++;
	}

#else

	XtSetArg(args[n], XmNmwmMenu, MenuStrDia1); n++;

#endif /* MOTIF1_1 */

    ModelessDia[DialogCount].Dialog= XmCreateMessageDialog(parent, title, 
															 args, n);

/* make HELP button spawn a child */
    spawnButton = XmMessageBoxGetChild (ModelessDia[DialogCount].Dialog, 
										XmDIALOG_HELP_BUTTON);
    XtAddCallback (spawnButton, XmNactivateCallback, 
				   PostModelessDialog, ModelessDia[DialogCount].Dialog);

    okButton = XmMessageBoxGetChild (ModelessDia[DialogCount].Dialog, 
									 XmDIALOG_OK_BUTTON);
    cancelButton = XmMessageBoxGetChild (ModelessDia[DialogCount].Dialog, 
									 	 XmDIALOG_CANCEL_BUTTON);
    XtAddCallback (okButton, XmNactivateCallback, 
				   DestroyModelessDialog, 
				   (XtPointer) &ModelessDia[DialogCount]);
#if 1
    XtAddCallback (cancelButton, XmNactivateCallback, 
				   DestroyModelessDialog, 
				   (XtPointer) &ModelessDia[DialogCount]);
#endif

    XtManageChild (ModelessDia[DialogCount].Dialog);
	ModelessDia[DialogCount].DialogIndex = DialogCount;
	ModelessDia[DialogCount].Parent = parent;
	ModelessDialogCount++;

}


static void
RemovePrimaryChildren(int curr_index, Widget curr_dialog)
{

	int	count;

	for (count = curr_index; count < MAX_DIALOGS; count++) {

		if (PrimaryDia[count].Parent == curr_dialog) {
			if (PrimaryDia[count].DialogIndex != -1) {
				RemovePrimaryChildren(count, PrimaryDia[count].Dialog);
				PrimaryDia[count].DialogIndex = -1;
			}
		}

	}

}


/* callback procedures */

static void
DestroyPrimaryDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

	DialogStruct	*Dialog;
	int		DialogIndex;
	int		i;


	Dialog = (DialogStruct *)client_data;
	DialogIndex = Dialog->DialogIndex;


	RemovePrimaryChildren(DialogIndex, PrimaryDia[DialogIndex].Dialog);

	XtDestroyWidget(PrimaryDia[DialogIndex].Dialog);

	for (i = 0; i < MAX_DIALOGS; i++) {

		if (PrimaryDia[i].DialogIndex == -1) {
			if (PrimaryDia[i].Dialog != NULL)
				XtDestroyWidget((char *)PrimaryDia[i].Dialog);
			PrimaryDia[i].Dialog = NULL;
		}

	}

	PrimaryDia[DialogIndex].Dialog = NULL;
	PrimaryDia[DialogIndex].DialogIndex = -1;
	PrimaryDialogCount--;

}


static void
PostPrimaryModalDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

    Widget	parent, spawnButton, okButton, cancelButton;
    int 	n;
    Arg 	args[MAX_ARGS];
    XmString tcs;
	char	title[NAME_LEN + 1];
	int		DialogCount;

	char	get_menu[1000];

	title[0] = '\0';

	if (PrimaryDialogCount >= MAX_DIALOGS) {
		fprintf(stderr, "Too many primary dialogs\n");
		return;
	}

	for (DialogCount = 0; DialogCount < MAX_DIALOGS; DialogCount++)
		if (PrimaryDia[DialogCount].DialogIndex == -1)
			break;
	if (DialogCount == MAX_DIALOGS) {
		fprintf(stderr, "No more primary dialogs\n");
		return;
	}
	sprintf(title, "Primary%d", DialogCount);
    parent = (Widget) client_data;

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, 
			  XmStringCreateSimple (PrimaryMsg)); n++;
    XtSetArg (args[n], XmNhelpLabelString, 
			  XmStringCreateSimple ("Spawn Dialog Child")); n++;

    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, title); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL);
			  n++;

	/* VendorShell resources */
#ifndef MOTIF1_1

	if (Test1_2 == True) {
		XtSetArg(args[n], XmNmwmMenu, MeuStrDia2); n++;
	}
	else {
		XtSetArg(args[n], XmNmwmMenu, MenuStrDia1); n++;
	}

#else

	XtSetArg(args[n], XmNmwmMenu, MenuStrDia1); n++;

#endif /* MOTIF1_1 */

    PrimaryDia[DialogCount].Dialog= XmCreateMessageDialog(parent, title, 
															 args, n);

/* make HELP button spawn a child */
    spawnButton = XmMessageBoxGetChild (PrimaryDia[DialogCount].Dialog, 
										XmDIALOG_HELP_BUTTON);
    XtAddCallback (spawnButton, XmNactivateCallback, 
				   PostPrimaryModalDialog, PrimaryDia[DialogCount].Dialog);

    okButton = XmMessageBoxGetChild (PrimaryDia[DialogCount].Dialog, 
									 XmDIALOG_OK_BUTTON);
    cancelButton = XmMessageBoxGetChild (PrimaryDia[DialogCount].Dialog, 
									 	 XmDIALOG_CANCEL_BUTTON);
    XtAddCallback (okButton, XmNactivateCallback, 
				   DestroyPrimaryDialog, 
				   (XtPointer) &PrimaryDia[DialogCount]);
    XtAddCallback (cancelButton, XmNactivateCallback, 
				   DestroyPrimaryDialog, 
				   (XtPointer) &PrimaryDia[DialogCount]);

    XtManageChild (PrimaryDia[DialogCount].Dialog);
	PrimaryDia[DialogCount].DialogIndex = DialogCount;
	PrimaryDia[DialogCount].Parent = parent;
	PrimaryDialogCount++;

	n = 0;
	XtSetArg(args[n], XmNmwmMenu, get_menu); n++;
	XtGetValues(PrimaryDia[DialogCount].Dialog, args, n);
	fprintf(stderr, "Menu of %d Prim dialog = %s\n", DialogCount, get_menu);

}


static void
RemoveFullAppChildren(int curr_index, Widget curr_dialog)
{

	int	count;

	for (count = curr_index; count < MAX_DIALOGS; count++) {

		if (FullAppDia[count].Parent == curr_dialog) {
			if (FullAppDia[count].DialogIndex != -1) {
				RemoveFullAppChildren(count, FullAppDia[count].Dialog);
				FullAppDia[count].DialogIndex = -1;
			}
		}

	}

}


/* callback procedures */

static void
DestroyFullAppDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

	DialogStruct	*Dialog;
	int		DialogIndex;
	int		i;


	Dialog = (DialogStruct *)client_data;
	DialogIndex = Dialog->DialogIndex;


	RemoveFullAppChildren(DialogIndex, FullAppDia[DialogIndex].Dialog);

	XtDestroyWidget(FullAppDia[DialogIndex].Dialog);

	for (i = 0; i < MAX_DIALOGS; i++) {

		if (FullAppDia[i].DialogIndex == -1) {
			if (FullAppDia[i].Dialog != NULL)
				XtDestroyWidget((char *)FullAppDia[i].Dialog);
			FullAppDia[i].Dialog = NULL;
		}

	}

	FullAppDia[DialogIndex].Dialog = NULL;
	FullAppDia[DialogIndex].DialogIndex = -1;
	FullAppDialogCount--;

}


static void
PostFullAppModalDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

    Widget	parent, spawnButton, okButton, cancelButton;
    int 	n;
    Arg 	args[MAX_ARGS];
    XmString tcs;
	char	title[NAME_LEN + 1];
	int		DialogCount;

	title[0] = '\0';

	if (FullAppDialogCount >= MAX_DIALOGS) {
		fprintf(stderr, "Too many fullapp dialogs\n");
		return;
	}

	for (DialogCount = 0; DialogCount < MAX_DIALOGS; DialogCount++)
		if (FullAppDia[DialogCount].DialogIndex == -1)
			break;
	if (DialogCount == MAX_DIALOGS) {
		fprintf(stderr, "No more fullapp dialogs\n");
		return;
	}
	sprintf(title, "FullApp%d", DialogCount);
    parent = (Widget) client_data;

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, 
			  XmStringCreateSimple (FullAppMsg)); n++;
    XtSetArg (args[n], XmNhelpLabelString, 
			  XmStringCreateSimple ("Spawn Dialog Child")); n++;

    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, title); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL);
			  n++;

	/* VendorShell resources */
#ifndef MOTIF1_1

	if (Test1_2 == True) {
		XtSetArg(args[n], XmNmwmMenu, MeuStrDia2); n++;
	}

#endif /* MOTIF1_1 */

    FullAppDia[DialogCount].Dialog= XmCreateMessageDialog(parent, title, 
															 args, n);

/* make HELP button spawn a child */
    spawnButton = XmMessageBoxGetChild (FullAppDia[DialogCount].Dialog, 
										XmDIALOG_HELP_BUTTON);
    XtAddCallback (spawnButton, XmNactivateCallback, 
				   PostFullAppModalDialog, FullAppDia[DialogCount].Dialog);

    okButton = XmMessageBoxGetChild (FullAppDia[DialogCount].Dialog, 
									 XmDIALOG_OK_BUTTON);
    cancelButton = XmMessageBoxGetChild (FullAppDia[DialogCount].Dialog, 
									 	 XmDIALOG_CANCEL_BUTTON);
    XtAddCallback (okButton, XmNactivateCallback, 
				   DestroyFullAppDialog, 
				   (XtPointer) &FullAppDia[DialogCount]);
    XtAddCallback (cancelButton, XmNactivateCallback, 
				   DestroyFullAppDialog, 
				   (XtPointer) &FullAppDia[DialogCount]);

    XtManageChild (FullAppDia[DialogCount].Dialog);
	FullAppDia[DialogCount].DialogIndex = DialogCount;
	FullAppDia[DialogCount].Parent = parent;
	FullAppDialogCount++;

}


static void
RemoveSystemChildren(int curr_index, Widget curr_dialog)
{

	int	count;

	for (count = curr_index; count < MAX_DIALOGS; count++) {

		if (SystemDia[count].Parent == curr_dialog) {
			if (SystemDia[count].DialogIndex != -1) {
				RemoveSystemChildren(count, SystemDia[count].Dialog);
				SystemDia[count].DialogIndex = -1;
			}
		}

	}

}


/* callback procedures */

static void
DestroySystemDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

	DialogStruct	*Dialog;
	int		DialogIndex;
	int		i;


	Dialog = (DialogStruct *)client_data;
	DialogIndex = Dialog->DialogIndex;


	RemoveSystemChildren(DialogIndex, SystemDia[DialogIndex].Dialog);

	XtDestroyWidget(SystemDia[DialogIndex].Dialog);

	for (i = 0; i < MAX_DIALOGS; i++) {

		if (SystemDia[i].DialogIndex == -1) {
			if (SystemDia[i].Dialog != NULL)
				XtDestroyWidget((char *)SystemDia[i].Dialog);
			SystemDia[i].Dialog = NULL;
		}

	}

	SystemDia[DialogIndex].Dialog = NULL;
	SystemDia[DialogIndex].DialogIndex = -1;
	SystemDialogCount--;

}


static void
PostSystemModalDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

    Widget	parent, spawnButton, okButton, cancelButton;
    int 	n;
    Arg 	args[MAX_ARGS];
    XmString tcs;
	char	title[NAME_LEN + 1];
	int		DialogCount;

	title[0] = '\0';

	if (SystemDialogCount >= MAX_DIALOGS) {
		fprintf(stderr, "Too many system dialogs\n");
		return;
	}

	for (DialogCount = 0; DialogCount < MAX_DIALOGS; DialogCount++)
		if (SystemDia[DialogCount].DialogIndex == -1)
			break;
	if (DialogCount == MAX_DIALOGS) {
		fprintf(stderr, "No more system dialogs\n");
		return;
	}
	sprintf(title, "System%d", DialogCount);
    parent = (Widget) client_data;

    n = 0;
    /* message box resources */
    XtSetArg (args[n], XmNmessageString, 
			  XmStringCreateSimple (SystemMsg)); n++;
    XtSetArg (args[n], XmNhelpLabelString, 
			  XmStringCreateSimple ("Spawn Dialog Child")); n++;

    /* dialog shell resources */
    XtSetArg (args[n], XmNtitle, title); n++;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL);
			  n++;

	/* VendorShell resources */
#ifndef MOTIF1_1

	if (Test1_2 == True) {
		XtSetArg(args[n], XmNmwmMenu, MeuStrDia2); n++;
	}

#endif /* MOTIF1_1 */

    SystemDia[DialogCount].Dialog= XmCreateMessageDialog(parent, title, 
															 args, n);

/* make HELP button spawn a child */
    spawnButton = XmMessageBoxGetChild (SystemDia[DialogCount].Dialog, 
										XmDIALOG_HELP_BUTTON);
    XtAddCallback (spawnButton, XmNactivateCallback, 
				   PostSystemModalDialog, SystemDia[DialogCount].Dialog);

    okButton = XmMessageBoxGetChild (SystemDia[DialogCount].Dialog, 
									 XmDIALOG_OK_BUTTON);
    cancelButton = XmMessageBoxGetChild (SystemDia[DialogCount].Dialog, 
									 	 XmDIALOG_CANCEL_BUTTON);
    XtAddCallback (okButton, XmNactivateCallback, 
				   DestroySystemDialog, 
				   (XtPointer) &SystemDia[DialogCount]);
    XtAddCallback (cancelButton, XmNactivateCallback, 
				   DestroySystemDialog, 
				   (XtPointer) &SystemDia[DialogCount]);

    XtManageChild (SystemDia[DialogCount].Dialog);
	SystemDia[DialogCount].DialogIndex = DialogCount;
	SystemDia[DialogCount].Parent = parent;
	SystemDialogCount++;

}


void
main(unsigned int argc, char  **argv)
{

    register int  n;
    Arg           args[MAX_ARGS];
    XmString      tcs;
    Widget        TopShell1, TopShell2;
    Widget        BB1, BB2;
    Widget        PrimaryButton1, PrimaryButton2;
    Widget        FullAppButton1, FullAppButton2;
    Widget        SystemButton1, SystemButton2;
    Widget        ModelessButton1, ModelessButton2;

	for (n = 0; n < MAX_DIALOGS; n++) {

		ModelessDia[n].Dialog = NULL;
		ModelessDia[n].Parent = NULL;
		ModelessDia[n].DialogIndex = -1;
		PrimaryDia[n].Dialog = NULL;
		PrimaryDia[n].Parent = NULL;
		PrimaryDia[n].DialogIndex = -1;
		FullAppDia[n].Dialog = NULL;
		FullAppDia[n].Parent = NULL;
		FullAppDia[n].DialogIndex = -1;
		SystemDia[n].Dialog = NULL;
		SystemDia[n].Parent = NULL;
		SystemDia[n].DialogIndex = -1;

	}

    CommonTestInit(argc, argv);

    n = 0;
    XtSetArg(args[n], XmNmappedWhenManaged, True);  n++;
    XtSetArg(args[n], XmNallowShellResize, True);  n++;
    XtSetArg(args[n], XmNtitle, "TopShell1"); n++;
    TopShell1 = XtCreatePopupShell("TopShell1", topLevelShellWidgetClass, 
								   Shell1, args, n);

    n = 0;
    XtSetArg(args[n], XmNmappedWhenManaged, True);  n++;
    XtSetArg(args[n], XmNallowShellResize, True);  n++;
    XtSetArg(args[n], XmNtitle, "TopShell2");         n++;
    XtSetArg(args[n], XtNgeometry, "+10+500");       n++;
    TopShell2 = XtCreatePopupShell("TopShell2", topLevelShellWidgetClass, 
								   Shell1, args, n);
   
	/* The following is for while running under automation */
	if (XtIsRealized(TopShell1)) {
		XtUnmapWidget(TopShell1);
    	XtMapWidget(TopShell1);
	}

    /* Shell 1 children */

    n = 0;
    BB1 = XmCreateBulletinBoard (TopShell1, "BB1", args, n);
    XtManageChild (BB1);
    
    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 100); n++;
    XtSetArg (args[n], XmNlabelString,
	      	  XmStringCreateSimple ("Create Primary Modal from TopShell1"));
			  n++;
    PrimaryButton1 = XmCreatePushButton (BB1, "PrimaryButton1", args, n);	  
    XtManageChild (PrimaryButton1);
    XtAddCallback (PrimaryButton1, XmNactivateCallback, 
				   PostPrimaryModalDialog, TopShell1);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 150); n++;
    XtSetArg (args[n], XmNlabelString,
	      	  XmStringCreateSimple ("Create Full App Modal from TopShell1"));
			  n++;
    FullAppButton1 = XmCreatePushButton (BB1, "FullAppButton1", args, n);	  
    XtManageChild (FullAppButton1);
    XtAddCallback (FullAppButton1,  XmNactivateCallback, 
				   PostFullAppModalDialog, TopShell1);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 200); n++;
    XtSetArg (args[n], XmNlabelString,
	      	  XmStringCreateSimple ("Create System Modal from TopShell1"));
			  n++;
    SystemButton1 = XmCreatePushButton (BB1, "SystemButton1", args, n);	  
    XtManageChild (SystemButton1);
    XtAddCallback (SystemButton1, XmNactivateCallback, PostSystemModalDialog, 
				   TopShell1);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 250); n++;
    XtSetArg (args[n], XmNlabelString,
	      	  XmStringCreateSimple ("Create Modeless Dialog from TopShell1"));
			  n++;
    ModelessButton1 = XmCreatePushButton (BB1, "ModelessButton1", args, n);	  
    XtManageChild (ModelessButton1);
    XtAddCallback (ModelessButton1, XmNactivateCallback, PostModelessDialog, 
				   TopShell1);

    /* Shell 2 children */

    n = 0;
    BB2 = XmCreateBulletinBoard (TopShell2, "BB2", args, n);    
    XtManageChild (BB2);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 100); n++;
    XtSetArg (args[n], XmNlabelString,
	      	  XmStringCreateSimple ("Create Primary Modal from TopShell2"));
			  n++;
    PrimaryButton2 = XmCreatePushButton (BB2, "PrimaryButton2", args, n);	  
    XtManageChild (PrimaryButton2);
    XtAddCallback (PrimaryButton2, XmNactivateCallback, PostPrimaryModalDialog, 
				   TopShell2);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 150); n++;
    XtSetArg (args[n], XmNlabelString,
	      	  XmStringCreateSimple ("Create Full App Modal from TopShell2"));
			  n++;
    FullAppButton2 = XmCreatePushButton (BB2, "FullAppButton2", args, n);	  
    XtManageChild (FullAppButton2);
    XtAddCallback (FullAppButton2,  XmNactivateCallback, 
				   PostFullAppModalDialog, TopShell2);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 200); n++;
    XtSetArg (args[n], XmNlabelString,
	      	  XmStringCreateSimple ("Create System Modal from TopShell2"));
			  n++;
    SystemButton2 = XmCreatePushButton (BB2, "SystemButton2", args, n);	  
    XtManageChild (SystemButton2);
    XtAddCallback (SystemButton2,  XmNactivateCallback, PostSystemModalDialog, 
				   TopShell2);

    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 250); n++;
    XtSetArg (args[n], XmNlabelString,
	      	  XmStringCreateSimple ("Create Modeless Dialog from TopShell2"));
			  n++;
    ModelessButton2 = XmCreatePushButton (BB2, "ModelessButton2", args, n);	  
    XtManageChild (ModelessButton2);
    XtAddCallback (ModelessButton2, XmNactivateCallback, PostModelessDialog, 
				   TopShell2);

    XtRealizeWidget(TopShell1);
    XtPopup(TopShell1, XtGrabNone);
    XtRealizeWidget(TopShell2);
    XtPopup(TopShell2, XtGrabNone);

	CommonPause();

	CommonPause();

	CommonPause();

	CommonPause();

#ifndef MOTIF1_1

	CommonPause();

	Test1_2 = True;

	CommonPause();

	CommonPause();

	CommonPause();

#endif /* MOTIF1_1 */

	CommonPause();

    XtAppMainLoop(app_context);

}
