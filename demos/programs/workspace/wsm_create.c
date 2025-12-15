/**
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: wsm_create.c /main/7 1996/10/30 10:23:18 cde-osf $"
#endif
#endif

/*
 * Required Motif Include Files
 */
#include <Xm/Xm.h>
#include <Xm/TextF.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/ScrolledW.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/MenuShell.h>
#include <Xm/CascadeB.h>

/*
 * User supplied include files.
 */
#include "wsm_ui.h"
#include "wsm_cb.h"
#include "wsm.h"
#include "wsmStruct.h"
/*
 * Some Convenience Routines
 */
void MENU_POST(Widget, XtPointer, XEvent *, Boolean *);
extern Space *current_space;
/*
 * Literal Definitions
 */
/*
 * Global widget declarations.
 *        - first remove definition of EXTERNAL
 */
#ifdef EXTERNAL
#undef EXTERNAL
#endif
#define EXTERNAL
/* Start Global Widget Declarations */
/* End Global Widget Declarations */


/*
 * Function:
 *      MENU_POST(p, mw, ev, dispatch);
 * Description:
 *      A converter wrapper for convenience from BuilderXcessory.
 * Input:
 *      p - Widget : the widget to post
 *	mw - XtPointer : the menu widget
 *	ev - XEvent* : the event that caused the menu post
 *	dispatch - Boolean* : not used
 * Output:
 *      None
 */


#define ARGLIST(p)	(
#define ARG(a, b)	a b,
#define GRA(a, b)	a b)

#ifndef IGNORE_MENU_POST

void MENU_POST
ARGLIST((p, mw, ev, dispatch))
ARG(Widget, p)
ARG(XtPointer, mw)
ARG(XEvent *, ev)
GRA(Boolean *, dispatch)
{
    Arg	args[2];
    int	argcnt;
    int	button;
    Widget m = (Widget)mw;
    XButtonEvent *e = (XButtonEvent *)ev;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNwhichButton, &button);
    argcnt++;
    XtGetValues(m, args, argcnt);
    if( e->button != button) return;
    XmMenuPosition(m, e);
    XtManageChild(m);
}
#endif

Widget
CreateNameWorkspace(Widget parent, WSM_UI* wsm_ui)
{
    Arg    	args[32];
    Cardinal   	argcnt;
    /*SUPPRESS 591*/
    Widget 	retval;
    /*SUPPRESS 591*/
    Widget	cancelButton;
    Widget	separator;
    Widget	okButton;
    Widget	nameLabel;
    Widget 	pixmapLabel;
    Widget	nameTitleLabel;
    Widget	nameWindow;
    Widget      buttonForm;
    Widget      nameForm;
    Widget      pixForm;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    wsm_ui->name_form = XtCreateWidget("nameForm",
		xmFormWidgetClass,
		parent,
		args,
		argcnt);
    retval = wsm_ui->name_form;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    nameTitleLabel = XtCreateWidget("nameTitleLabel",
		xmLabelWidgetClass,
		wsm_ui->name_form,
		args,
		argcnt);
    XtManageChild(nameTitleLabel);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    buttonForm = XtCreateWidget("buttonForm",
		xmFormWidgetClass,
		wsm_ui->name_form,
		args,
		argcnt);
    XtManageChild(buttonForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    okButton = XtCreateWidget("applyButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);

    XtAddCallback(okButton, XmNactivateCallback, NameOkActivateCB,
		  (XtPointer)wsm_ui);
    XtManageChild(okButton);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    cancelButton = XtCreateWidget("dismissButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);

    XtAddCallback(cancelButton, XmNactivateCallback, DismissNameCB,
		  (XtPointer)wsm_ui);
    XtManageChild(cancelButton);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, buttonForm); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    separator = XtCreateWidget("separator",
		xmSeparatorWidgetClass,
		wsm_ui->name_form,
		args,
		argcnt);
    XtManageChild(separator);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, separator); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    pixForm = XtCreateWidget("pixForm",
		xmFormWidgetClass,
		wsm_ui->name_form,
		args,
		argcnt);
    XtManageChild(pixForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    pixmapLabel = XtCreateWidget("pixmapLabel",
		xmLabelWidgetClass,
		pixForm,
		args,
		argcnt);
#ifndef _NO_PIXMAP
    XtManageChild(pixmapLabel);
#endif

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNleftWidget, pixmapLabel); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    wsm_ui->pixmap_text = XtCreateWidget("textField",
		xmTextFieldWidgetClass,
		pixForm,
		args,
		argcnt);
    XtAddCallback(wsm_ui->pixmap_text, XmNactivateCallback,
		  NameOkActivateCB, (XtPointer)wsm_ui);
#ifndef _NO_PIXMAP
    XtManageChild(wsm_ui->pixmap_text);
#endif

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, pixForm); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    nameForm = XtCreateWidget("wnameForm",
		xmFormWidgetClass,
		wsm_ui->name_form,
		args,
		argcnt);
    XtManageChild(nameForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    nameLabel = XtCreateWidget("nameLabel",
		xmLabelWidgetClass,
		nameForm,
		args,
		argcnt);
    XtManageChild(nameLabel);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNleftWidget, nameLabel); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    wsm_ui->name_text = XtCreateWidget("textField",
		xmTextFieldWidgetClass,
		nameForm,
		args,
		argcnt);
    XtAddCallback(wsm_ui->name_text, XmNactivateCallback, NameActivateCB,
		  (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->name_text);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNscrollingPolicy, XmAPPLICATION_DEFINED); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, nameTitleLabel); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, nameForm); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    nameWindow = XtCreateWidget("nameWindow",
		xmScrolledWindowWidgetClass,
		wsm_ui->name_form,
		args,
		argcnt);
    XtManageChild(nameWindow);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNshadowThickness, 2); argcnt++;
    XtSetArg(args[argcnt], XmNitemCount, 0); argcnt++;
    XtSetArg(args[argcnt], XmNselectionPolicy, XmSINGLE_SELECT); argcnt++;
    XtSetArg(args[argcnt], XmNlistSizePolicy, XmCONSTANT); argcnt++;
    wsm_ui->name_list = XtCreateWidget("nameList",
		xmListWidgetClass,
		nameWindow,
		args,
		argcnt);

    XtAddCallback(wsm_ui->name_list, XmNsingleSelectionCallback,
		  SelectNameSpaceCB, (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->name_list);

    return( retval );
}



Widget
CreateBackgroundWorkspace(Widget parent, WSM_UI* wsm_ui)
{
    Arg    	args[32];
    Cardinal   	argcnt;
    Widget 	retval;
    Widget	cancelButton;
    Widget	separator;
    Widget	okButton;
    Widget	backgroundLabel;
    Widget	backgroundTitleLabel;
    Widget	backgroundWindow;
    Widget      buttonForm;
    Widget      bgForm;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    wsm_ui->background_form = XtCreateWidget("backgroundForm",
		xmFormWidgetClass,
		parent,
		args,
		argcnt);
    retval = wsm_ui->background_form;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    backgroundTitleLabel = XtCreateWidget("backgroundTitleLabel",
		xmLabelWidgetClass,
		wsm_ui->background_form,
		args,
		argcnt);
    XtManageChild(backgroundTitleLabel);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    buttonForm = XtCreateWidget("buttonForm",
		xmFormWidgetClass,
		wsm_ui->background_form,
		args,
		argcnt);
    XtManageChild(buttonForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    okButton = XtCreateWidget("applyButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);

    XtAddCallback(okButton, XmNactivateCallback, BackgroundActivateCB,
		  (XtPointer)wsm_ui);
    XtManageChild(okButton);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    cancelButton = XtCreateWidget("dismissButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);

    XtAddCallback(cancelButton, XmNactivateCallback, DismissBackgroundCB,
		  (XtPointer)wsm_ui);
    XtManageChild(cancelButton);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, buttonForm); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    separator = XtCreateWidget("separator",
		xmSeparatorWidgetClass,
		wsm_ui->background_form,
		args,
		argcnt);
    XtManageChild(separator);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, separator); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    bgForm = XtCreateWidget("bgForm",
		xmFormWidgetClass,
		wsm_ui->background_form,
		args,
		argcnt);
    XtManageChild(bgForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    backgroundLabel = XtCreateWidget("backgroundLabel",
		xmLabelWidgetClass,
		bgForm,
		args,
		argcnt);
    XtManageChild(backgroundLabel);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNleftWidget, backgroundLabel); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    wsm_ui->background_text = XtCreateWidget("textField",
		xmTextFieldWidgetClass,
		bgForm,
		args,
		argcnt);
    XtAddCallback(wsm_ui->background_text, XmNactivateCallback,
		  BackgroundActivateCB, (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->background_text);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNscrollingPolicy, XmAPPLICATION_DEFINED); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, backgroundTitleLabel); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, bgForm); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    backgroundWindow = XtCreateWidget("backgroundWindow",
		xmScrolledWindowWidgetClass,
		wsm_ui->background_form,
		args,
		argcnt);
    XtManageChild(backgroundWindow);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNshadowThickness, 2); argcnt++;
    XtSetArg(args[argcnt], XmNitemCount, 0); argcnt++;
    XtSetArg(args[argcnt], XmNselectionPolicy, XmSINGLE_SELECT); argcnt++;
    XtSetArg(args[argcnt], XmNlistSizePolicy, XmCONSTANT); argcnt++;
    wsm_ui->background_list = XtCreateWidget("backgroundList",
		xmListWidgetClass,
		backgroundWindow,
		args,
		argcnt);

    XtAddCallback(wsm_ui->background_list, XmNsingleSelectionCallback,
		  SelectBackgroundSpaceCB, (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->background_list);

    return( retval );
}



Widget
CreateDeleteWorkspace(Widget parent, WSM_UI* wsm_ui)
{
    Arg    	args[32];
    Cardinal   	argcnt;
    Widget 	retval;
    Widget	deleteWindow;
    Widget	deleteTitleLabel;
    Widget	cancelButton1;
    Widget	separator;
    Widget      buttonForm;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    wsm_ui->delete_form = XtCreateWidget("deleteForm",
		xmFormWidgetClass,
		parent,
		args,
		argcnt);
    retval = wsm_ui->delete_form;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    deleteTitleLabel = XtCreateWidget("deleteTitleLabel",
		xmLabelWidgetClass,
		wsm_ui->delete_form,
		args,
		argcnt);
    XtManageChild(deleteTitleLabel);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    buttonForm = XtCreateWidget("buttonForm",
		xmFormWidgetClass,
		wsm_ui->delete_form,
		args,
		argcnt);
    XtManageChild(buttonForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    wsm_ui->ok_button = XtCreateWidget("applyButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);
    XtAddCallback(wsm_ui->ok_button, XmNactivateCallback, DeleteActivateCB,
		  (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->ok_button);

    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    cancelButton1 = XtCreateWidget("dismissButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);
    XtAddCallback(cancelButton1, XmNactivateCallback, DismissDeleteCB,
		  (XtPointer)wsm_ui);
    XtManageChild(cancelButton1);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, buttonForm); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    separator = XtCreateWidget("separator",
		xmSeparatorWidgetClass,
		wsm_ui->delete_form,
		args,
		argcnt);
    XtManageChild(separator);


    argcnt = 0;
    XtSetArg(args[argcnt], XmNscrollingPolicy, XmAPPLICATION_DEFINED); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, deleteTitleLabel); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, separator); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    deleteWindow = XtCreateWidget("deleteWindow",
		xmScrolledWindowWidgetClass,
		wsm_ui->delete_form,
		args,
		argcnt);
    XtManageChild(deleteWindow);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNshadowThickness, 2); argcnt++;
    XtSetArg(args[argcnt], XmNitemCount, 0); argcnt++;
    XtSetArg(args[argcnt], XmNselectionPolicy, XmSINGLE_SELECT); argcnt++;
    XtSetArg(args[argcnt], XmNlistSizePolicy, XmCONSTANT); argcnt++;
    XtSetArg(args[argcnt], XmNvisibleItemCount, 10); argcnt++;
    wsm_ui->delete_list = XtCreateWidget("deleteList",
		xmListWidgetClass,
		deleteWindow,
		args,
		argcnt);

    XtAddCallback(wsm_ui->delete_list, XmNsingleSelectionCallback,
		  SelectDeleteCB, (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->delete_list);

    return( retval );
}

Widget
CreateConfigureWorkspace(Widget parent, WSM_UI* wsm_ui)
{
    Arg    	args[32];
    Cardinal   	argcnt;
    /*SUPPRESS 591*/
    Widget 	retval;
    /*SUPPRESS 591*/
    XmString	xmstr[32];
    Widget	fromScrolledWindow;
    Widget	toScrolledWindow;
    Widget	occupyButton;
    Widget	toggleRow;
    Widget	toggleRow2;
    Widget	titleLabel;
    Widget	dismissButton;
    Widget	separator2;
    Widget	menuShell1;
    Widget	pulldownMenu1;
    Widget	menuShell;
    Widget	pulldownMenu;
    Widget 	current_button =0;
    Widget      fromForm, buttonForm, toForm;
    int i;
    Space *s;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    wsm_ui->configure_form = XtCreateWidget("configureForm",
		xmFormWidgetClass,
		parent,
		args,
		argcnt);
    retval = wsm_ui->configure_form;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    titleLabel = XtCreateWidget("titleLabel",
		xmLabelWidgetClass,
		wsm_ui->configure_form,
		args,
		argcnt);
    XtManageChild(titleLabel);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNleftPosition, 45); argcnt++;
    dismissButton = XtCreateWidget("dismissButton",
		xmPushButtonWidgetClass,
		wsm_ui->configure_form,
		args,
		argcnt);

    XtAddCallback(dismissButton, XmNactivateCallback, DismissConfigureCB,
		  (XtPointer)wsm_ui);
    XtManageChild(dismissButton);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, dismissButton); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    separator2 = XtCreateWidget("separator2",
		xmSeparatorWidgetClass,
		wsm_ui->configure_form,
		args,
		argcnt);
    XtManageChild(separator2);

    /*
     * The window/client radio box
     */

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNradioBehavior, True); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, separator2); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrowColumnType, XmWORK_AREA); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, separator2); argcnt++;
    toggleRow2 = XtCreateWidget("toggleRow",
		xmRowColumnWidgetClass,
		wsm_ui->configure_form,
		args,
		argcnt);
    XtManageChild(toggleRow2);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNindicatorType, XmONE_OF_MANY); argcnt++;
    XtSetArg(args[argcnt], XmNset, True); argcnt++;
    wsm_ui->window_toggle = XtCreateWidget("windowButton",
		xmToggleButtonWidgetClass,
		toggleRow2,
		args,
		argcnt);
    XtAddCallback(wsm_ui->window_toggle, XmNvalueChangedCallback,
		  WindowModeCB, (XtPointer)wsm_ui);
#ifndef _NO_CLIENT_MODE
    XtManageChild(wsm_ui->window_toggle);
#endif

    argcnt = 0;
    XtSetArg(args[argcnt], XmNindicatorType, XmONE_OF_MANY); argcnt++;
    wsm_ui->client_toggle = XtCreateWidget("clientButton",
		xmToggleButtonWidgetClass,
		toggleRow2,
		args,
		argcnt);
    XtAddCallback(wsm_ui->client_toggle, XmNvalueChangedCallback,
		  ClientModeCB, (XtPointer)wsm_ui);
#ifndef _NO_CLIENT_MODE
    XtManageChild(wsm_ui->client_toggle);
#endif


    /*
     * The three container forms
     */

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, titleLabel); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNleftPosition, 50); argcnt++;
    buttonForm = XtCreateWidget("buttonForm",
		xmFormWidgetClass,
		wsm_ui->configure_form,
		args,
		argcnt);
    XtManageChild(buttonForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, titleLabel); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNrightWidget, buttonForm); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, toggleRow2); argcnt++;
    fromForm = XtCreateWidget("fromForm",
		xmFormWidgetClass,
		wsm_ui->configure_form,
		args,
		argcnt);
    XtManageChild(fromForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, titleLabel); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNleftWidget, buttonForm); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, toggleRow2); argcnt++;
    toForm = XtCreateWidget("toForm",
		xmFormWidgetClass,
		wsm_ui->configure_form,
		args,
		argcnt);
    XtManageChild(toForm);

    /*
     * The option menus
     */

    argcnt = 0;
    XtSetArg(args[argcnt], XmNwidth, 1); argcnt++;
    XtSetArg(args[argcnt], XmNheight, 1); argcnt++;
    menuShell = XtCreatePopupShell("menuShell",
         	xmMenuShellWidgetClass,
		toForm,
		args,
		argcnt);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNrowColumnType, XmMENU_PULLDOWN); argcnt++;
    pulldownMenu = XtCreateWidget("pulldownMenu",
		xmRowColumnWidgetClass,
		menuShell,
		args,
		argcnt);

    for (i = 0; i< wsm_ui->num_space_buttons; i++)
      {
	s = GetSpaceFromID(i);
	argcnt = 0;
	XtSetArg(args[argcnt], XmNuserData, i+1); argcnt++;
	XtSetArg(args[argcnt], XmNlabelString,
		 (xmstr[0]=XmStringLtoRCreate(s->name,
					      XmSTRING_DEFAULT_CHARSET)));
	argcnt++;
	wsm_ui->to_option_button[i] = XtCreateWidget("toWorkspace1Button",
						     xmPushButtonWidgetClass,
						     pulldownMenu,
						     args,
						     argcnt);

	XtAddCallback(wsm_ui->to_option_button[i], XmNactivateCallback,
		      ToWorkspaceCB, (XtPointer)wsm_ui);
	XtManageChild(wsm_ui->to_option_button[i]);

	XmStringFree(xmstr[0]);
	if (s == current_space)
	  current_button = wsm_ui->to_option_button[i];
      }


    argcnt = 0;
    XtSetArg(args[argcnt], XmNuserData, 0); argcnt++;
    XtSetArg(args[argcnt], XmNlabelString,
	     (xmstr[0]=XmStringLtoRCreate("All Workspaces",
					  XmSTRING_DEFAULT_CHARSET)));
    argcnt++;
    wsm_ui->to_option_button[i] = XtCreateWidget("toAllWorkspaceButton",
		xmPushButtonWidgetClass,
		pulldownMenu,
		args,
		argcnt);

    XtAddCallback(wsm_ui->to_option_button[i], XmNactivateCallback,
		  ToWorkspaceCB, (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->to_option_button[i]);

    XmStringFree(xmstr[0]);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNmarginWidth, 0); argcnt++;
    XtSetArg(args[argcnt], XmNspacing, 0); argcnt++;
    XtSetArg(args[argcnt], XmNlabelString,  (xmstr[0]=XmStringLtoRCreate(
		"", XmSTRING_DEFAULT_CHARSET))); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrowColumnType, XmMENU_OPTION); argcnt++;
    XtSetArg(args[argcnt], XmNsubMenuId, pulldownMenu); argcnt++;
    if (current_button)
      {XtSetArg(args[argcnt], XmNmenuHistory, current_button);argcnt++;}
    wsm_ui->to_option_menu = XtCreateWidget("toMenu",
		xmRowColumnWidgetClass,
		toForm,
		args,
		argcnt);
    XtManageChild(wsm_ui->to_option_menu);

    XmStringFree(xmstr[0]);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNwidth, 1); argcnt++;
    XtSetArg(args[argcnt], XmNheight, 1); argcnt++;
    menuShell1 = XtCreatePopupShell("menuShell1",
		xmMenuShellWidgetClass,
		fromForm,
		args,
		argcnt);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNrowColumnType, XmMENU_PULLDOWN); argcnt++;
    pulldownMenu1 = XtCreateWidget("pulldownMenu1",
		xmRowColumnWidgetClass,
		menuShell1,
		args,
		argcnt);

    current_button = 0;
    for (i = 0; i< wsm_ui->num_space_buttons; i++)
      {
	s = GetSpaceFromID(i);
	argcnt = 0;
	XtSetArg(args[argcnt], XmNuserData, i+1); argcnt++;
	XtSetArg(args[argcnt], XmNlabelString, (xmstr[0]=
	     XmStringLtoRCreate(s->name,XmSTRING_DEFAULT_CHARSET))); argcnt++;
	wsm_ui->from_option_button[i] = XtCreateWidget("fromWorkspace1Button",
						       xmPushButtonWidgetClass,
						       pulldownMenu1,
						       args,
						       argcnt);

	XtAddCallback(wsm_ui->from_option_button[i], XmNactivateCallback,
		      FromWorkspaceCB, (XtPointer)wsm_ui);
	XtManageChild(wsm_ui->from_option_button[i]);

	XmStringFree(xmstr[0]);

	if (s == current_space)
	  current_button = wsm_ui->to_option_button[i];
    }

    argcnt = 0;
    XtSetArg(args[argcnt], XmNmarginWidth, 0); argcnt++;
    XtSetArg(args[argcnt], XmNspacing, 0); argcnt++;
    XtSetArg(args[argcnt], XmNlabelString, (xmstr[0]=XmStringLtoRCreate(
		"", XmSTRING_DEFAULT_CHARSET))); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrowColumnType, XmMENU_OPTION); argcnt++;
    XtSetArg(args[argcnt], XmNsubMenuId, pulldownMenu1); argcnt++;
    if (current_button)
      {XtSetArg(args[argcnt], XmNmenuHistory, current_button);argcnt++;}
    wsm_ui->from_option_menu = XtCreateWidget("fromMenu",
		xmRowColumnWidgetClass,
		fromForm,
		args,
		argcnt);
    XtManageChild(wsm_ui->from_option_menu);

    XmStringFree(xmstr[0]);

    /*
     * The scrolled lists
     */

    argcnt = 0;
    XtSetArg(args[argcnt], XmNscrollingPolicy, XmAPPLICATION_DEFINED);
    argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, wsm_ui->from_option_menu); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    fromScrolledWindow = XtCreateWidget("fromScrolledWindow",
		xmScrolledWindowWidgetClass,
		fromForm,
		args,
		argcnt);
    XtManageChild(fromScrolledWindow);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNscrollingPolicy, XmAPPLICATION_DEFINED);
    argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, wsm_ui->to_option_menu); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    toScrolledWindow = XtCreateWidget("toScrolledWindow",
		xmScrolledWindowWidgetClass,
		toForm,
		args,
		argcnt);
    XtManageChild(toScrolledWindow);


    argcnt = 0;
    XtSetArg(args[argcnt], XmNshadowThickness, 2); argcnt++;
    XtSetArg(args[argcnt], XmNitemCount, 0); argcnt++;
    XtSetArg(args[argcnt], XmNselectionPolicy, XmSINGLE_SELECT); argcnt++;
    XtSetArg(args[argcnt], XmNlistSizePolicy, XmCONSTANT); argcnt++;
    wsm_ui->from_list = XtCreateWidget("fromList",
		xmListWidgetClass,
		fromScrolledWindow,
		args,
		argcnt);
    XtAddCallback(wsm_ui->from_list, XmNsingleSelectionCallback,
		  SelectFromListCB, (XtPointer)wsm_ui);

    XtAddCallback(wsm_ui->from_list, XmNmultipleSelectionCallback,
		  MultSelectFromListCB, (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->from_list);

    XtInitializeWidgetClass(xmListWidgetClass);
    argcnt = 0;
    XtSetArg(args[argcnt], XmNshadowThickness, 2); argcnt++;
    XtSetArg(args[argcnt], XmNitemCount, 0); argcnt++;
    XtSetArg(args[argcnt], XmNselectionPolicy, XmSINGLE_SELECT); argcnt++;
    XtSetArg(args[argcnt], XmNlistSizePolicy, XmCONSTANT); argcnt++;
    wsm_ui->to_list = XtCreateWidget("toList",
		xmListWidgetClass,
		toScrolledWindow,
		args,
		argcnt);

    XtAddCallback(wsm_ui->to_list, XmNsingleSelectionCallback,
		  SelectToListCB, (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->to_list);

    /*
     * The copy/link radio box
     */

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNradioBehavior, True); argcnt++;
    XtSetArg(args[argcnt], XmNrowColumnType, XmWORK_AREA); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    toggleRow = XtCreateWidget("toggleRow",
		xmRowColumnWidgetClass,
		buttonForm,
		args,
		argcnt);
    XtManageChild(toggleRow);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNindicatorType, XmONE_OF_MANY); argcnt++;
    XtSetArg(args[argcnt], XmNset, True); argcnt++;
    wsm_ui->copy_toggle = XtCreateWidget("copyButton",
		xmToggleButtonWidgetClass,
		toggleRow,
		args,
		argcnt);
    XtManageChild(wsm_ui->copy_toggle);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNindicatorType, XmONE_OF_MANY); argcnt++;
    wsm_ui->link_toggle = XtCreateWidget("linkButton",
		xmToggleButtonWidgetClass,
		toggleRow,
		args,
		argcnt);
    XtManageChild(wsm_ui->link_toggle);

    /*
     * The occupy/move/remove buttons
     */

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, toggleRow); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    occupyButton = XtCreateWidget("occupyButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);

    XtAddCallback(occupyButton, XmNactivateCallback, OccupyCB,
		  (XtPointer)wsm_ui);
    XtManageChild(occupyButton);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, occupyButton); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    wsm_ui->move_button = XtCreateWidget("moveButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);

    XtAddCallback(wsm_ui->move_button, XmNactivateCallback, MoveCB,
		  (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->move_button);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, wsm_ui->move_button); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    wsm_ui->delete_button = XtCreateWidget("deleteButton",
		xmPushButtonWidgetClass,
                buttonForm,
		args,
		argcnt);

    XtAddCallback(wsm_ui->delete_button, XmNactivateCallback, DeleteCB,
		  (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->delete_button);

    return( retval );
}



Widget
CreateWorkspacePanelBX(Widget parent, WSM_UI* wsm_ui, Boolean show_menu)
{
    Arg    	args[32];
    Cardinal   	argcnt;
    Widget 	retval;
    /*SUPPRESS 591*/
    XmString	xmstr[8];
    Widget	form;
    Widget	menuBar;
    Widget	fileCascade;
    Widget	pulldownMenu;
    Widget	saveButton;
    Widget	saveAsButton;
    Widget	separator;
    Widget	exitButton;
    Widget	workspaceCascade;
    Widget	pulldownMenu1;
    Widget	viewCascade;
    Widget	menuShell2;
    Widget	pulldownMenu2;
    Widget	popupMenu;
    Widget	backgroundButton;
    Widget	newButton;
    Widget	nameButton;
    Widget	hideButton;
    Widget	deleteButton;
    Widget	configureButton;
    int i;
    Space *s;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_ANY); argcnt++;
    form = XtCreateWidget("form",
		xmFormWidgetClass,
		parent,
		args,
		argcnt);
    retval = form;
    XtManageChild(form);

    if (show_menu)
    {
	argcnt = 0;
    	XtSetArg(args[argcnt], XmNrowColumnType, XmMENU_BAR); argcnt++;
	XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
	XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
	XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
	menuBar = XtCreateWidget("menuBar",
				 xmRowColumnWidgetClass,
				 form,
				 args,
				 argcnt);
	retval = menuBar;
	XtManageChild(menuBar);

	argcnt = 0;
	fileCascade = XtCreateWidget("fileCascade",
				     xmCascadeButtonWidgetClass,
				     menuBar,
				     args,
				     argcnt);

	XtManageChild(fileCascade);

	argcnt = 0;
	pulldownMenu = XmCreatePulldownMenu(fileCascade, "pulldownMenu",
					    args, argcnt);

	argcnt = 0;
	saveButton = XtCreateWidget("saveButton",
				    xmPushButtonWidgetClass,
				    pulldownMenu,
				    args,
				    argcnt);
	XtAddCallback(saveButton, XmNactivateCallback, SaveActivateCB,
		      (XtPointer)wsm_ui);
	XtManageChild(saveButton);

	argcnt = 0;
	saveAsButton = XtCreateWidget("saveAsButton",
				      xmPushButtonWidgetClass,
				      pulldownMenu,
				      args,
				      argcnt);
	XtAddCallback(saveAsButton, XmNactivateCallback, SaveAsCB,
		      (XtPointer)wsm_ui);
	XtManageChild(saveAsButton);

	argcnt = 0;
	separator = XtCreateWidget("separator",
				   xmSeparatorWidgetClass,
				   pulldownMenu,
				   args,
				   argcnt);
	XtManageChild(separator);

	argcnt = 0;
	exitButton = XtCreateWidget("exitButton",
				    xmPushButtonWidgetClass,
				    pulldownMenu,
				    args,
				    argcnt);
	XtAddCallback(exitButton, XmNactivateCallback, ExitCB,
		      (XtPointer)wsm_ui);
	XtManageChild(exitButton);

	argcnt = 0;
	XtSetArg(args[argcnt], XmNsubMenuId, pulldownMenu); argcnt++;
	XtSetValues(fileCascade, args, argcnt);

	argcnt = 0;
	workspaceCascade = XtCreateWidget("workspaceCascade",
					  xmCascadeButtonWidgetClass,
					  menuBar,
					  args,
					  argcnt);

	XtManageChild(workspaceCascade);

	argcnt = 0;
	pulldownMenu1 = XmCreatePulldownMenu(workspaceCascade, "pulldownMenu1",
					     args, argcnt);

	argcnt = 0;
	newButton = XtCreateWidget("newButton",
				   xmPushButtonWidgetClass,
				   pulldownMenu1,
				   args,
				   argcnt);
	XtAddCallback(newButton, XmNactivateCallback, NewActivateCB,
		      (XtPointer)wsm_ui);
	XtManageChild(newButton);

	argcnt = 0;
	nameButton = XtCreateWidget("nameButton",
				    xmPushButtonWidgetClass,
				    pulldownMenu1,
				    args,
				    argcnt);
	XtAddCallback(nameButton, XmNactivateCallback, CreateNameCB,
		      (XtPointer)wsm_ui);
	XtManageChild(nameButton);

	argcnt = 0;
	backgroundButton = XtCreateWidget("backgroundButton",
					  xmPushButtonWidgetClass,
					  pulldownMenu1,
					  args,
					  argcnt);
	XtAddCallback(backgroundButton, XmNactivateCallback,
		      CreateBackgroundCB, (XtPointer)wsm_ui);
	XtManageChild(backgroundButton);

	argcnt = 0;
	configureButton = XtCreateWidget("configureButton",
					 xmPushButtonWidgetClass,
					 pulldownMenu1,
					 args,
					 argcnt);
	XtAddCallback(configureButton, XmNactivateCallback, CreateConfigureCB,
		      (XtPointer)wsm_ui);
	XtManageChild(configureButton);

	argcnt = 0;
	deleteButton = XtCreateWidget("deleteButton",
				      xmPushButtonWidgetClass,
				      pulldownMenu1,
				      args,
				      argcnt);
	XtAddCallback(deleteButton, XmNactivateCallback, CreateDeleteCB,
		      (XtPointer)wsm_ui);
	XtManageChild(deleteButton);

	argcnt = 0;
	XtSetArg(args[argcnt], XmNsubMenuId, pulldownMenu1); argcnt++;
	XtSetValues(workspaceCascade, args, argcnt);

	argcnt = 0;
	viewCascade = XtCreateWidget("viewCascade",
				     xmCascadeButtonWidgetClass,
				     menuBar,
				     args,
				     argcnt);

	XtManageChild(viewCascade);

	argcnt = 0;
	pulldownMenu2 = XmCreatePulldownMenu(viewCascade, "pulldownMenu2",
					     args, argcnt);

	argcnt = 0;
	hideButton = XtCreateWidget("hideButton",
				    xmPushButtonWidgetClass,
				    pulldownMenu2,
				    args,
				    argcnt);
	XtAddCallback(hideButton, XmNactivateCallback, HideActivateCB,
		      (XtPointer)wsm_ui);
	XtManageChild(hideButton);

	argcnt = 0;
	XtSetArg(args[argcnt], XmNsubMenuId, pulldownMenu2); argcnt++;
	XtSetValues(viewCascade, args, argcnt);
    }

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizeWidth, True); argcnt++;
    XtSetArg(args[argcnt], XmNresizeHeight, True); argcnt++;
    XtSetArg(args[argcnt], XmNadjustLast, True); argcnt++;
    XtSetArg(args[argcnt], XmNradioAlwaysOne, True); argcnt++;
    XtSetArg(args[argcnt], XmNradioBehavior, True); argcnt++;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNspacing, 8); argcnt++;
    XtSetArg(args[argcnt], XmNpacking, XmPACK_TIGHT); argcnt++;
    if (show_menu)
    {
        XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
        XtSetArg(args[argcnt], XmNtopWidget, menuBar); argcnt++;
    }
    else
    {
        XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    }
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    wsm_ui->wsm_row_column = XtCreateWidget("buttonPanel",
		xmRowColumnWidgetClass,
		form,
		args,
		argcnt);
    XtManageChild(wsm_ui->wsm_row_column);

    for (i = 0; i< wsm_ui->num_space_buttons; i++)
      {
	s = GetSpaceFromID(i);
	argcnt = 0;
	XtSetArg(args[argcnt], XmNindicatorSize, 20); argcnt++;
	XtSetArg(args[argcnt], XmNspacing, 10); argcnt++;
	XtSetArg(args[argcnt], XmNuserData, i+1); argcnt++;
	XtSetArg(args[argcnt], XmNlabelString, (xmstr[0]=XmStringLtoRCreate(
	       s->name, XmSTRING_DEFAULT_CHARSET))); argcnt++;
	XtSetArg(args[argcnt], XmNrecomputeSize, True); argcnt++;
/* check between these lines...crashes in 2.0 not in 1.2.2 */
	if (SpacePixelSet(s))
	  {XtSetArg(args[argcnt],XmNbackground,s->pixel);argcnt++;}
	if (SpaceLabelPixmapSet(s))
	  {
	   XtSetArg(args[argcnt],XmNlabelPixmap,s->pixmap_label); argcnt++;
	   XtSetArg(args[argcnt],XmNlabelType,XmPIXMAP);argcnt++;
	 }
/*   above this line */
	if (s == current_space) {XtSetArg(args[argcnt], XmNset, True); argcnt++;}
	wsm_ui->space_button[i] = XtCreateWidget("pushButton",
						 xmToggleButtonWidgetClass,
						 wsm_ui->wsm_row_column,
						 args,
						 argcnt);

	XtAddCallback(wsm_ui->space_button[i], XmNvalueChangedCallback,
		      SendLeaveRoomCB, (XtPointer)wsm_ui);
	XtManageChild(wsm_ui->space_button[i]);

	XmStringFree(xmstr[0]);
      }

    /*
     * Build the popup menu
     */

    argcnt = 0;
    XtSetArg(args[argcnt], XmNwidth, 1); argcnt++;
    XtSetArg(args[argcnt], XmNheight, 1); argcnt++;
    menuShell2 = XtCreatePopupShell("menuShell2",
		xmMenuShellWidgetClass,
		wsm_ui->wsm_row_column,
		args,
		argcnt);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNrowColumnType, XmMENU_POPUP); argcnt++;
    popupMenu = XtCreateWidget("popupMenu",
		xmRowColumnWidgetClass,
		menuShell2,
		args,
		argcnt);
    XtAddEventHandler(XtParent(XtParent(popupMenu)),
        ButtonPressMask, False, MENU_POST, (XtPointer)popupMenu);

    argcnt = 0;
    newButton = XtCreateWidget("newButton",
		xmPushButtonWidgetClass,
		popupMenu,
		args,
		argcnt);

    XtAddCallback(newButton, XmNactivateCallback, NewActivateCB,
		  (XtPointer)wsm_ui);
    XtManageChild(newButton);

    argcnt = 0;
    nameButton = XtCreateWidget("nameButton",
		xmPushButtonWidgetClass,
		popupMenu,
		args,
		argcnt);

    XtAddCallback(nameButton, XmNactivateCallback, CreateNameCB,
		  (XtPointer)wsm_ui);
    XtManageChild(nameButton);

    argcnt = 0;
    backgroundButton = XtCreateWidget("backgroundButton",
		xmPushButtonWidgetClass,
		popupMenu,
		args,
		argcnt);

    XtAddCallback(backgroundButton, XmNactivateCallback, CreateBackgroundCB,
		  (XtPointer)wsm_ui);
    XtManageChild(backgroundButton);

    argcnt = 0;
    configureButton = XtCreateWidget("configureButton",
		xmPushButtonWidgetClass,
		popupMenu,
		args,
		argcnt);

    XtAddCallback(configureButton, XmNactivateCallback, CreateConfigureCB,
		  (XtPointer)wsm_ui);
    XtManageChild(configureButton);

    argcnt = 0;
    deleteButton = XtCreateWidget("deleteButton",
		xmPushButtonWidgetClass,
		popupMenu,
		args,
		argcnt);

    XtAddCallback(deleteButton, XmNactivateCallback, CreateDeleteCB,
		  (XtPointer)wsm_ui);
    XtManageChild(deleteButton);

    argcnt = 0;
    (void) XtCreateManagedWidget("separator",
				 xmSeparatorWidgetClass,
				 popupMenu,
				 args,
				 argcnt);

    argcnt = 0;
    hideButton = XtCreateWidget("hideButton",
		xmPushButtonWidgetClass,
		popupMenu,
		args,
		argcnt);

    XtAddCallback(hideButton, XmNactivateCallback, HideActivateCB,
		  (XtPointer)wsm_ui);
#ifndef _NO_ROOT
    XtManageChild(hideButton);
#endif

    argcnt = 0;
    (void) XtCreateManagedWidget("separator",
				 xmSeparatorWidgetClass,
				 popupMenu,
				 args,
				 argcnt);

    argcnt = 0;
    saveButton = XtCreateWidget("saveButton",
		xmPushButtonWidgetClass,
		popupMenu,
		args,
		argcnt);

    XtAddCallback(saveButton, XmNactivateCallback, SaveActivateCB,
		  (XtPointer)wsm_ui);
    XtManageChild(saveButton);

    argcnt = 0;
    saveAsButton = XtCreateWidget("saveAsButton",
		xmPushButtonWidgetClass,
		popupMenu,
		args,
		argcnt);

    XtAddCallback(saveAsButton, XmNactivateCallback, SaveAsCB,
		  (XtPointer)wsm_ui);
    XtManageChild(saveAsButton);

    argcnt = 0;
    (void) XtCreateManagedWidget("separator",
				 xmSeparatorWidgetClass,
				 popupMenu,
				 args,
				 argcnt);

    argcnt = 0;
    exitButton = XtCreateWidget("exitButton",
		xmPushButtonWidgetClass,
		popupMenu,
		args,
		argcnt);

    XtAddCallback(exitButton, XmNactivateCallback, ExitCB, (XtPointer)wsm_ui);
    XtManageChild(exitButton);

    return( retval );
}

Widget
CreateSaveAs(Widget parent, WSM_UI* wsm_ui)
{
    Arg    	args[32];
    Cardinal   	argcnt;
    Widget 	retval;
    Widget	cancelButton2;
    Widget	separator3;
    Widget	okButton2;
    Widget	saveAsLabel;
    Widget	saveAsTitleLabel;
    Widget      dbForm;
    Widget      buttonForm;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    wsm_ui->save_as_form = XtCreateWidget("saveAsForm",
		xmFormWidgetClass,
		parent,
		args,
		argcnt);
    retval = wsm_ui->save_as_form;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    saveAsTitleLabel = XtCreateWidget("saveAsTitleLabel",
		xmLabelWidgetClass,
		wsm_ui->save_as_form,
		args,
		argcnt);
    XtManageChild(saveAsTitleLabel);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, saveAsTitleLabel); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    dbForm = XtCreateWidget("dbForm",
		xmFormWidgetClass,
		wsm_ui->save_as_form,
		args,
		argcnt);
    XtManageChild(dbForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    saveAsLabel = XtCreateWidget("saveAsLabel",
		xmLabelWidgetClass,
		dbForm,
		args,
		argcnt);
    XtManageChild(saveAsLabel);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNleftWidget, saveAsLabel); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    wsm_ui->save_as_text = XtCreateWidget("saveAsText",
		xmTextFieldWidgetClass,
		dbForm,
		args,
		argcnt);
    XtAddCallback(wsm_ui->save_as_text, XmNactivateCallback, SaveAsOkCB,
		  (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->save_as_text);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, dbForm); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    separator3 = XtCreateWidget("separator3",
		xmSeparatorWidgetClass,
		wsm_ui->save_as_form,
		args,
		argcnt);
    XtManageChild(separator3);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, separator3); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    buttonForm = XtCreateWidget("buttonForm",
		xmFormWidgetClass,
		wsm_ui->save_as_form,
		args,
		argcnt);
    XtManageChild(buttonForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    okButton2 = XtCreateWidget("okButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);

    XtAddCallback(okButton2, XmNactivateCallback, SaveAsOkCB,
		  (XtPointer)wsm_ui);
    XtManageChild(okButton2);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    cancelButton2 = XtCreateWidget("cancelButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);

    XtAddCallback(cancelButton2, XmNactivateCallback, DismissSaveAsCB,
		  (XtPointer)wsm_ui);
    XtManageChild(cancelButton2);

    return( retval );
}



#ifndef _NO_OCCUPY_DIALOG


Widget
CreateOccupyWorkspace(Widget parent, WSM_UI* wsm_ui)
{
    Arg    	args[32];
    Cardinal   	argcnt;
    Widget 	retval;
    Widget	occupyWindow;
    Widget	occupyTitleLabel;
    Widget	cancelButton;
    Widget	separator;
    Widget	okButton;
    Widget 	toggleRow;
    Widget      buttonForm;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    wsm_ui->occupy_form = XtCreateWidget("occupyForm",
		xmFormWidgetClass,
		parent,
		args,
		argcnt);
    retval = wsm_ui->occupy_form;

    argcnt = 0;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    occupyTitleLabel = XtCreateWidget("occupyTitleLabel",
		xmLabelWidgetClass,
		wsm_ui->occupy_form,
		args,
		argcnt);
    XtManageChild(occupyTitleLabel);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNresizePolicy, XmRESIZE_GROW); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    buttonForm = XtCreateWidget("buttonForm",
		xmFormWidgetClass,
		wsm_ui->occupy_form,
		args,
		argcnt);
    XtManageChild(buttonForm);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    okButton = XtCreateWidget("okButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);
    XtAddCallback(okButton, XmNactivateCallback,OccupyActivateCB,
		  (XtPointer)wsm_ui);
    XtManageChild(okButton);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_POSITION); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    cancelButton = XtCreateWidget("cancelButton",
		xmPushButtonWidgetClass,
		buttonForm,
		args,
		argcnt);
    XtAddCallback(cancelButton, XmNactivateCallback, DismissOccupyCB,
		  (XtPointer)wsm_ui);
    XtManageChild(cancelButton);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, buttonForm); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    separator = XtCreateWidget("separator",
		xmSeparatorWidgetClass,
		wsm_ui->occupy_form,
		args,
		argcnt);
    XtManageChild(separator);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNorientation, XmHORIZONTAL); argcnt++;
    XtSetArg(args[argcnt], XmNradioBehavior, True); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_NONE); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget,separator); argcnt++;
    XtSetArg(args[argcnt], XmNrowColumnType, XmWORK_AREA); argcnt++;
    toggleRow = XtCreateWidget("toggleRow",
		xmRowColumnWidgetClass,
		wsm_ui->occupy_form,
		args,
		argcnt);
    XtManageChild(toggleRow);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNindicatorType, XmONE_OF_MANY); argcnt++;
    XtSetArg(args[argcnt], XmNset, True); argcnt++;
    wsm_ui->copy_occupy_toggle = XtCreateWidget("copyButton",
		xmToggleButtonWidgetClass,
		toggleRow,
		args,
		argcnt);
    XtAddCallback(wsm_ui->copy_occupy_toggle, XmNvalueChangedCallback,
		  CopyModeCB, (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->copy_occupy_toggle);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNindicatorType, XmONE_OF_MANY); argcnt++;
    wsm_ui->link_occupy_toggle = XtCreateWidget("linkButton",
		xmToggleButtonWidgetClass,
		toggleRow,
		args,
		argcnt);
    XtAddCallback(wsm_ui->link_occupy_toggle, XmNvalueChangedCallback,
		  LinkModeCB, (XtPointer)wsm_ui);
    XtManageChild(wsm_ui->link_occupy_toggle);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNscrollingPolicy, XmAPPLICATION_DEFINED); argcnt++;
    XtSetArg(args[argcnt], XmNtopAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNtopWidget, occupyTitleLabel); argcnt++;
    XtSetArg(args[argcnt], XmNbottomAttachment, XmATTACH_WIDGET); argcnt++;
    XtSetArg(args[argcnt], XmNbottomWidget, toggleRow); argcnt++;
    XtSetArg(args[argcnt], XmNleftAttachment, XmATTACH_FORM); argcnt++;
    XtSetArg(args[argcnt], XmNrightAttachment, XmATTACH_FORM); argcnt++;
    occupyWindow = XtCreateWidget("occupyWindow",
		xmScrolledWindowWidgetClass,
		wsm_ui->occupy_form,
		args,
		argcnt);
    XtManageChild(occupyWindow);

    argcnt = 0;
    XtSetArg(args[argcnt], XmNshadowThickness, 2); argcnt++;
    XtSetArg(args[argcnt], XmNitemCount, 0); argcnt++;
    XtSetArg(args[argcnt], XmNselectionPolicy, XmMULTIPLE_SELECT); argcnt++;
    XtSetArg(args[argcnt], XmNlistSizePolicy, XmCONSTANT); argcnt++;
    XtSetArg(args[argcnt], XmNvisibleItemCount, 10); argcnt++;
    wsm_ui->occupy_list = XtCreateWidget("occupyList",
		xmListWidgetClass,
		occupyWindow,
		args,
		argcnt);

    XtManageChild(wsm_ui->occupy_list);

    return( retval );
}

#endif
