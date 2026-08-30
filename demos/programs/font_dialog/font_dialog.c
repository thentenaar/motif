/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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

#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/FontDialog.h>

/**
 * Update our main window when a font is selected
 */
static void font_selected(Widget w, XtPointer client, XtPointer call)
{
	Cardinal i, n_children, ac = 0;
	XmString tmp;
	XmFontDialogCallbackStruct *fd_cb;
	XmRenderTable rt;
	Widget label;
	WidgetList children;

	label = (Widget)client;
	fd_cb = (XmFontDialogCallbackStruct *)call;
	if (!label| !fd_cb)
		return;

	/**
	 * Display a new string in the chosen font.
	 */
	tmp = XmStringCreateLocalized("Gaza frequens Libycum duxit Karthago triumphum.");
	rt  = XmRenderTableAddRenditions(NULL, &fd_cb->rendition, 1, 0);
	XtVaSetValues(label, XmNrenderTable, rt, XmNlabelString, tmp, NULL);
	XmStringFree(tmp);
	XmRenderTableFree(rt);

	/**
	 * The rendition we get in the callback data is a copy, so we must
	 * free it.
	 */
	XmRenditionFree(fd_cb->rendition);
}

/**
 * Create a FontDialog with a title and source
 */
static Widget mkdialog(Widget parent, XmString title,
                       XmFontDialogSource source, Widget frame)
{
	Arg arg[2];
	Widget dialog;

	XtSetArg(arg[0], XmNtitleString, title);
	XtSetArg(arg[1], XmNfontSource, source);
	dialog = XmCreateFontDialog(parent, "Font Dialog", arg, 2);
	XtAddCallback(dialog, XmNokCallback, font_selected, frame);
	XmStringFree(title);
	return dialog;
}

/**
 * Popup a font dialog
 */
static void popup_fontdlg(Widget w, XtPointer client, XtPointer call)
{
	(void)call;
	XtManageChild((Widget)client);
	XtPopup(XtParent((Widget)client), XtGrabNonexclusive);
}

/**
 * Callback for our quit button
 */
static void quit(Widget w, XtPointer client, XtPointer call)
{
	(void)client;
	(void)call;
	XtAppSetExitFlag(XtWidgetToApplicationContext(w));
}

int main(int argc, char *argv[])
{
	Widget top, form, frame, label, x_button, x_dialog, quit_button;
	XmString s;
	XtAppContext app;
	Widget xft_button, xft_dialog;

	XtSetLanguageProc(NULL, NULL, NULL);
	top = XtVaOpenApplication(&app, "XmdFontDialog", NULL, 0, &argc, argv,
	                          NULL, sessionShellWidgetClass,
	                          XtNallowShellResize, True, NULL);

	/**
	 * Main app form
	 */
	form  = XtCreateManagedWidget("form", xmFormWidgetClass, top, NULL, 0);
	frame = XtVaCreateManagedWidget("frame",
	                                xmFrameWidgetClass, form,
	                                XmNtopAttachment, XmATTACH_FORM,
	                                XmNleftAttachment, XmATTACH_FORM, NULL);

	s = XmStringCreateLocalized("No font selected.");
	label = XtVaCreateManagedWidget("label", xmLabelWidgetClass, frame,
	                                XmNlabelString, s, NULL);
	XmStringFree(s);

	xft_button = XtVaCreateManagedWidget("Xft Fonts",
	                                     xmPushButtonWidgetClass, form,
	                                     XmNtopAttachment, XmATTACH_WIDGET,
	                                     XmNtopWidget, frame,
	                                     XmNbottomAttachment, XmATTACH_FORM,
	                                     XmNleftAttachment, XmATTACH_FORM, NULL);
	x_button = XtVaCreateManagedWidget("X Fonts",
	                                   xmPushButtonWidgetClass, form,
	                                   XmNtopAttachment, XmATTACH_WIDGET,
	                                   XmNtopWidget, frame,
	                                   XmNbottomAttachment, XmATTACH_FORM,
	                                   XmNleftAttachment, XmATTACH_WIDGET,
	                                   XmNleftWidget, xft_button, NULL);
	quit_button = XtVaCreateManagedWidget("Quit",
	                                      xmPushButtonWidgetClass, form,
	                                      XmNtopAttachment, XmATTACH_WIDGET,
	                                      XmNtopWidget, frame,
	                                      XmNbottomAttachment, XmATTACH_FORM,
	                                      XmNleftAttachment, XmATTACH_WIDGET,
	                                      XmNleftWidget, x_button, NULL);

	/* Create our dialogs */
	xft_dialog = mkdialog(top, XmStringCreateLocalized("Pick a Xft Font"),
	                      XmFONT_SOURCE_XFT, label);
	x_dialog   = mkdialog(top, XmStringCreateLocalized("Pick a X Font"),
	                      XmFONT_SOURCE_X, label);

	/* Finally, hook up our button callbacks */
	XtAddCallback(xft_button,  XmNactivateCallback, popup_fontdlg, xft_dialog);
	XtAddCallback(x_button,    XmNactivateCallback, popup_fontdlg, x_dialog);
	XtAddCallback(quit_button, XmNactivateCallback, quit, NULL);

	/* Show our window and enter Xt's main loop */
	XtRealizeWidget(top);
	XtAppMainLoop(app);
	return 0;
}

