/* $XConsortium: MwmUI5.c /main/4 1995/07/15 21:14:21 drk $ */
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



#include <testlib.h>

typedef struct _test_dialog_rec *test_dialog;
typedef struct _test_dialog_rec {
    Widget          form_dialog;
    Widget          test_button;
    Widget          ok_button;
    test_dialog     the_test_dialog;
} test_dialog_rec;

Widget              toplevel;
test_dialog         the_test_dialog;
static int          dialog_id = 0;

static test_dialog  test_dialog_create ();
static void         test_dialog_manage ();
static void         test_dialog_unmanage ();


unsigned int main (argc, argv)
    unsigned int argc;
    char         *argv[];
{
    Widget       TestShell1;
    XmString     label_string;
    Arg          args[10];
    int          n;

    CommonTestInit (argc,argv);


    n = 0;
    XtSetArg (args[n], XmNallowShellResize, True); n++;
    XtSetArg (args[n], XmNwidth, 100); n++;
    XtSetArg (args[n], XmNheight, 100); n++;
    the_test_dialog = test_dialog_create (Shell1);

    n=0;
    XtSetArg (args[n], XmNwidth, 100); n++;
    XtSetArg (args[n], XmNheight, 100); n++;
    TestShell1 = XtAppCreateShell ("TestWindow1", "TestWindow1",
                                   topLevelShellWidgetClass,
                                   display, args, n);

    XtRealizeWidget (TestShell1);
    CommonPause();
    CommonPause();
    XtAppMainLoop (app_context);
}


static void activate_proc (widget, the_dialog)
    Widget      widget;
    test_dialog the_dialog;
{
    if (widget == the_dialog->test_button)
    {
        if (the_dialog->the_test_dialog == NULL)
        {
            the_dialog->the_test_dialog = test_dialog_create
(the_dialog->form_dialog);
        }
        test_dialog_manage (the_dialog->the_test_dialog);
    }
    else if (widget == the_dialog->ok_button)
    {
        test_dialog_unmanage (the_dialog);
    }
}


static test_dialog test_dialog_create (parent)
  Widget parent;
{
    Arg         args[20];
    int         n;
    test_dialog the_dialog;
    the_dialog = (test_dialog) XtMalloc(sizeof(test_dialog_rec));
    the_dialog->the_test_dialog = NULL;
    dialog_id++;
    {
        char     id_string[20];
        XmString dialog_title;
        sprintf(id_string, "Form%d", dialog_id);
        dialog_title = XmStringCreate(id_string, XmSTRING_DEFAULT_CHARSET);
        n = 0;
        XtSetArg (args[n], XmNautoUnmanage, False); n++;
        XtSetArg (args[n], XmNdialogTitle, dialog_title); n++;
        the_dialog->form_dialog = (Widget) XmCreateFormDialog (
            parent,
            id_string,
            args, n);
        XtManageChild (the_dialog->form_dialog);
    }
    n = 0;
    the_dialog->test_button = XmCreatePushButtonGadget (
        the_dialog->form_dialog,
        "Make Child",
        args, n);
    XtManageChild (the_dialog->test_button);
    XtAddCallback (the_dialog->test_button, XmNactivateCallback, activate_proc,
the_dialog);
    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNtopWidget, the_dialog->test_button); n++;
    the_dialog->ok_button = XmCreatePushButtonGadget (
        the_dialog->form_dialog,
        "Destroy Me",
        args, n);
    XtManageChild (the_dialog->ok_button);
    XtAddCallback (the_dialog->ok_button, XmNactivateCallback, activate_proc,
the_dialog);
    return (the_dialog);
}
static void test_dialog_manage (the_dialog)
    test_dialog the_dialog;
{
    XtManageChild(the_dialog->form_dialog);
}
static void test_dialog_unmanage (the_dialog)
    test_dialog the_dialog;
{
    /*
    **  Now unmanage this form.
    */
    XtUnmanageChild(the_dialog->form_dialog);
}


