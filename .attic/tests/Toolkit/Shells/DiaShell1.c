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
static char rcsid[] = "$TOG: DiaShell1.c /main/9 1999/03/26 14:47:46 jff $"
#endif
#endif



#include <testlib.h>
#include "warn.bmp"


/*  Global Variables  */
Pixmap	warnmap;
Widget 	MessageBox1;
Widget 	MessageBox2;
Widget 	MessageBox3;
Widget 	Dialog1, BulletinBoard, fontLabel1, fontLabel2, fontLabel3;

Widget  FormDialog1 = NULL;
Widget  FormDialog2 = NULL;


static void
PostSecondFormDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

    Widget      parent;
    Cardinal 	n;
    Arg 		args[10];

	if (FormDialog2 != NULL) {

		if (XtIsManaged(FormDialog2))
			fprintf(stderr, "FormDialog2 already managed\n");
		else
			fprintf(stderr, "FormDialog2 being managed\n");
		XtManageChild(FormDialog2);
		return;

	}

    parent = (Widget) client_data;

   	n = 0;
	XtSetArg(args[n], XmNwidth, 100); n++;
	XtSetArg(args[n], XmNheight, 100); n++;
   	FormDialog2 = XmCreateFormDialog(FormDialog1, "FormDialog2", args, n);
	XtManageChild(FormDialog2);

}


static void
CreateFirstFormDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

    Widget		parent, Post_FormDialog2;
    Cardinal	n;
    Arg 		args[MAX_ARGS];


	if (FormDialog1 != NULL)
		return;

    parent = (Widget) client_data;

    n = 0;
	XtSetArg(args[n], XmNautoUnmanage, False); n++;
    FormDialog1 = XmCreateFormDialog(parent, "FormDialog1", args, n);

	n = 0;
	XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNtopOffset, 5); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNbottomOffset, 5); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNleftOffset, 5); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightOffset, 5); n++;
   	Post_FormDialog2 = XmCreatePushButton(FormDialog1, "Post_FormDialog2", 
										  args, n);
	XtManageChild(Post_FormDialog2);
   	XtAddCallback (Post_FormDialog2, XmNactivateCallback, 
				   PostSecondFormDialog, FormDialog1);

}


static void
ManageFormDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

	if (FormDialog1 != NULL)
    	XtManageChild(FormDialog1);

}


static void
UnmanageFormDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

	if (FormDialog1 != NULL)
    	XtUnmanageChild(FormDialog1);

}


static void
UnmanageSystemDialog(Widget w, XtPointer client_data, XtPointer call_data)
{

    XtUnmanageChild(w);

}


static void
HelpCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  printf("If you want to overwrite the file click on OK\n");
  printf("If you DON'T want to overwrite it click CANCEL\n");
}


void
main(unsigned int argc, char **argv)
{

	XmString        tcs, tcs2;
    XmFontList		font0, font1, font2;
    Cardinal    	n;
    Arg         	args[MAX_ARGS];
    Widget      	BB1;
    Widget      	SystemDia1;			/* PIR4340 */
    Widget      	CreateForm;
    Widget      	ManageForm;
    Widget      	UnmanageForm;


    CommonTestInit(argc, argv);
    
    /*  Load some fonts */


    font0 = CommonGetFontList("fixed");
    font1 = CommonGetFontList("6x10");
    font2 = CommonGetFontList("8x13bold");

    n = 0;
    XtSetArg(args[n], XmNwidth,  100);  n++;
    XtSetArg(args[n], XmNheight, 100);  n++;
    XtSetValues(Shell1, args, n);

    tcs = XmStringCreateLtoR("DialogStyle Unmanaged Test", 
						 XmFONTLIST_DEFAULT_TAG);
    tcs2 = XmStringCreateLtoR("UnManage", XmFONTLIST_DEFAULT_TAG);

												/* Begin PIR4340 */
    n = 0;
    XtSetArg (args[n], XmNdialogStyle, XmDIALOG_SYSTEM_MODAL);    n++;
    XtSetArg (args[n], XmNmessageString, tcs);            n++;
    XtSetArg (args[n], XmNokLabelString, tcs2);           n++;
    XtSetArg (args[n], XmNautoUnmanage, False);                   n++;
    XtSetArg (args[n], XmNminimizeButtons, False);                n++;
    SystemDia1 = XmCreateMessageDialog (Shell1, "SystemDia1", args, n);
    XtManageChild (SystemDia1);
    XmStringFree(tcs);
    XmStringFree(tcs2);

	XtAddCallback(SystemDia1, XmNokCallback, UnmanageSystemDialog, NULL);

												/* End PIR4340 */

    XtRealizeWidget(Shell1);

	CommonPause();								/* PIR4340 */


    /* build the pixmap */
    warnmap = XCreatePixmapFromBitmapData(display, rootWindow,
		       warn_bits,warn_width,warn_height,
		       CommonGetColor("white"),
		       CommonGetColor("black"),
		       DefaultDepth(display,
				    DefaultScreen(display)));


    tcs = XmStringCreateLtoR("Warning!! File Exists! OK to OverWrite?",
       						 XmFONTLIST_DEFAULT_TAG);
    n = 0;
    XtSetArg(args[n], XmNmessageString, tcs );  n++;
    XtSetArg(args[n], XmNsymbolPixmap, warnmap);  n++;
    XtSetArg(args[n], XmNwidth, 370); n++;
    XtSetArg(args[n], XmNheight, 120); n++;
    MessageBox1 = XmCreateWarningDialog(Shell1, "MessageBox1", args, n);
    XtManageChild(MessageBox1);
    XmStringFree(tcs);

    /* add callbacks, change label */
    XtAddCallback(MessageBox1,XmNhelpCallback, HelpCB, NULL);

    tcs = XmStringCreateLtoR("No Overwrite", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNcancelLabelString, tcs); n++;
    XtSetValues(MessageBox1,args,n);
    XmStringFree (tcs);

    tcs = XmStringCreateLtoR("Error!! File Exists!", XmFONTLIST_DEFAULT_TAG);
        
    n = 0;
    XtSetArg(args[n], XmNmessageString, tcs );  n++;
    XtSetArg(args[n], XmNwidth, 270); n++;
    XtSetArg(args[n], XmNheight, 120); n++;
    MessageBox2 = XmCreateErrorDialog(Shell1, "MessageBox2", args, n);
    XtManageChild(MessageBox2);
    XmStringFree(tcs);

    /* add callbacks, change label */
    XtAddCallback(MessageBox2,XmNhelpCallback, HelpCB, NULL);

    tcs = XmStringCreateLtoR("Giveup", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNcancelLabelString, tcs); n++;
    XtSetValues(MessageBox2,args,n);
    XmStringFree (tcs);
    
    tcs = XmStringCreateLtoR("Do you want to go on?", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNmessageString, tcs );  n++;
    XtSetArg(args[n], XmNwidth, 270); n++;
    XtSetArg(args[n], XmNheight, 120); n++;
    MessageBox3 = XmCreateQuestionDialog(Shell1, "MessageBox3", args, n);
    XtManageChild(MessageBox3);
    XmStringFree(tcs);

    /* add callbacks, change label */
    XtAddCallback(MessageBox3,XmNhelpCallback, HelpCB, NULL);

    tcs = XmStringCreateLtoR("No", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNcancelLabelString, tcs); n++;
    XtSetValues(MessageBox2,args,n);
    XmStringFree (tcs);

    CommonPause();

    CommonPause();

    CommonPause();

											/* Begin PIR3228 */
    n = 0;
    Dialog1 = XmCreateDialogShell(Shell1, "Dialog1", args, n); 
    BulletinBoard = XmCreateBulletinBoard(Dialog1, "BulletinBoard", args, n); 

    tcs = XmStringCreateLtoR("fixed", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNx, 10); n++;
    XtSetArg(args[n], XmNy, 10); n++;
    XtSetArg(args[n], XmNfontList, font0);      n++;
    XtSetArg(args[n], XmNlabelString, tcs);	n++;
    fontLabel1 = XmCreateLabel (BulletinBoard, "fontLabel1", args, n);
    XtManageChild(fontLabel1);
    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("6x10", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNx, 60); n++;
    XtSetArg(args[n], XmNy, 10); n++;
    XtSetArg(args[n], XmNfontList, font1);      n++;
    XtSetArg(args[n], XmNlabelString, tcs);	n++;
    fontLabel2 = XmCreateLabel (BulletinBoard, "fontLabel2", args, n);
    XtManageChild(fontLabel2);
    XmStringFree(tcs);

    tcs = XmStringCreateLtoR("8x13bold", XmFONTLIST_DEFAULT_TAG);

    n = 0;
    XtSetArg(args[n], XmNx, 110); n++;
    XtSetArg(args[n], XmNy, 10); n++;
    XtSetArg(args[n], XmNfontList, font2);      n++;
    XtSetArg(args[n], XmNlabelString, tcs);	n++;
    fontLabel3 = XmCreateLabel(BulletinBoard, "fontLabel3", args, n);
    XtManageChild(fontLabel3);
    XmStringFree(tcs);

    XtManageChild(BulletinBoard);
    XtManageChild(Dialog1); 
											/* End PIR3228 */

    CommonPause();

	XtDestroyWidget(fontLabel1);
	XtDestroyWidget(fontLabel2);
	XtDestroyWidget(fontLabel3);
	XtDestroyWidget(BulletinBoard);
	XtDestroyWidget(Dialog1);

											/* Begin PIR3006 */
    n = 0;
    BB1 = XmCreateBulletinBoard (Shell1, "BB1", args, n);
    XtManageChild (BB1);

    tcs = XmStringCreateSimple ("Create FormDialog1"); n++;
    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 100); n++;
    XtSetArg (args[n], XmNlabelString, tcs); n++;
    CreateForm = XmCreatePushButton (BB1, "CreateForm", args, n);
    XtManageChild (CreateForm);
    XtAddCallback (CreateForm, XmNactivateCallback, CreateFirstFormDialog,
                   Shell1);
    XmStringFree (tcs);

    tcs = XmStringCreateSimple ("Manage FormDialog1");
    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 150); n++;
    XtSetArg (args[n], XmNlabelString, tcs); n++;
    ManageForm = XmCreatePushButton (BB1, "ManageForm", args, n);
    XtManageChild (ManageForm);
    XtAddCallback (ManageForm, XmNactivateCallback, ManageFormDialog, NULL);
    XmStringFree (tcs);

    tcs = XmStringCreateSimple ("Unmanage FormDialog1");
    n = 0;
    XtSetArg (args[n], XmNx, 10); n++;
    XtSetArg (args[n], XmNy, 200); n++;
    XtSetArg (args[n], XmNlabelString, tcs); n++;
    UnmanageForm = XmCreatePushButton (BB1, "UnmanageForm", args, n);
    XtManageChild (UnmanageForm);
    XtAddCallback (UnmanageForm, XmNactivateCallback, UnmanageFormDialog, NULL);
   XmStringFree(tcs);


    CommonPause();

    CommonPause();

    CommonPause();
											/* End PIR3006 */

    CommonPause();

    XtAppMainLoop(app_context);

}
