/* $TOG: CommText11.c /main/7 1997/10/02 16:02:30 csn $ */
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

/*
 *  Global variables.
 */
Widget drawingAreaW, editorW;

char text_string[] = "\
As I was going to St. Ives\n\
I met a man with seven wives\n\
Each wife had seven sacks,\n\
Each sack had seven cats,\n\
Each cat had seven kits\n\
Kits, cats, sacks, and wives,\n\
How many were going to St. Ives?\n\
\n\
I don't know but...\n\
Jack Sprat could eat no fat\n\
His wife could eat no lean\n\
And so between them both\n\
They licked the platter clean";


void enlarge_it(Widget w, XtPointer client_data, XtPointer call_data)
{
	Dimension	get_width;
	int n;
	Arg args[MAX_ARGS];

	n=0;
	XtSetArg(args[n], XmNwidth, &get_width);  n++; 
	XtGetValues(w, args, n);

	n=0;
	if (get_width == 100)
	{
		XtSetArg(args[n], XmNwidth, 120);  n++; 
		XtSetArg(args[n], XmNheight, 120);  n++; 
	}
	else
	{
		XtSetArg(args[n], XmNwidth, 100);  n++;
		XtSetArg(args[n], XmNheight, 100);  n++;
	}
	XtSetValues(w, args, n);
}

void
QuitCB(widget, string, cb)
Widget widget;
char *string;
caddr_t *cb;
{
  exit(1);
}

void
wxEditorCallback(editorW, editor, editorCB)
     Widget editorW;
     caddr_t *editor;
     XmTextVerifyCallbackStruct *editorCB;
{
  XtRemoveCallback(editorW,XmNvalueChangedCallback,
                   wxEditorCallback,NULL);
}

void
keyPressCB(w,wxWidget,xevent,sendAgain)
     Widget w;
     XtPointer wxWidget;
     XEvent *xevent;
     Boolean *sendAgain;
{
  XtRemoveEventHandler(editorW,
                       KeyPressMask,
                       False,
                       keyPressCB,
                       NULL);
}

void
AddCB(widget, string, cb)
Widget widget;
char *string;
caddr_t *cb;
{
  printf("Add insert handler\n");

  XtInsertEventHandler(editorW,
                       KeyPressMask,
                       False,
                       keyPressCB,
                       NULL,
                       XtListHead);
}

Widget
createEditor(parent, x, y)
     Widget parent;
     Position x, y;
{
  Arg args[20];
  int n, i;
  Widget tmpw;

  n = 0;
  XtSetArg(args[n],XmNx, x); n++;
  XtSetArg(args[n],XmNy, y); n++;
  tmpw = XmCreateScrolledWindow(parent,"WINDOW", args, n);

  n = 0;
  XtSetArg(args[n],XmNwidth, 200); n++;
  XtSetArg(args[n],XmNheight, 200); n++;
  XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
  XtSetArg(args[n],XmNwordWrap, False); n++;
  XtSetArg(args[n],XmNscrollHorizontal, False); n++;
  XtSetArg(args[n],XmNscrollVertical, False); n++;
  XtSetArg(args[n],XmNverifyBell, False); n++;
  XtSetArg(args[n],XmNborderWidth, 0); n++;
  editorW = XmCreateText(tmpw,"EDITOR",args,n);

  XtVaSetValues(editorW,XmNwidth,200,XmNheight,200,NULL);
  XtManageChild(editorW);
  XtManageChild(tmpw);

  return editorW;
}

static char render_tag[] = "render1";

void main_text11 (argc, argv)
     unsigned int argc;
     char **argv;
{
	Widget bboard;
	Widget pushbutton;
	Widget pushbutton2;
	Widget text1;
	Arg args[MAX_ARGS];
	XmString string;
	int n;

	XtAccelerators new_accel;
	static char new_accelerators[] =
	"#override Meta<Key>Q:               ArmAndActivate()";
	static char new_accelerators2[] =
	"Meta<Key>Q:               next-page()";

	static XtCallbackRec activateCBs[] =
	{
		{ enlarge_it, NULL },
		{ NULL, NULL },
	};

	CommonTestInit(argc, argv);

	n=0;
	XtSetArg(args[n], XmNmarginWidth, 20);  n++; 
	XtSetArg(args[n], XmNmarginHeight, 20);  n++; 
	bboard = XmCreateBulletinBoard(Shell1, "bboard", args, n);

	n=0;
	XtSetArg(args[n], XmNwidth, 100);  n++; 
	XtSetArg(args[n], XmNheight, 100);  n++; 
	string = XmStringLtoRCreate("Push Button 1", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNactivateCallback, activateCBs);  n++; 
	XtSetArg(args[n], XmNlabelType, XmSTRING);  n++; 
	XtSetArg(args[n], XmNlabelString, string);  n++; 

	pushbutton = XmCreatePushButton(bboard, "pushbutton", args, n); 

	XmStringFree(string);

	new_accel = XtParseAcceleratorTable(new_accelerators);

	n=0;
	XtSetArg(args[n], XmNwidth, 100);  n++; 
	XtSetArg(args[n], XmNheight, 100);  n++; 
	XtSetArg(args[n], XmNy, 140);  n++; 
	string = XmStringLtoRCreate("Push Button 2", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNactivateCallback, activateCBs);  n++; 
	XtSetArg(args[n], XmNlabelType, XmSTRING);  n++; 
	XtSetArg(args[n], XmNlabelString, string);  n++; 
	XtSetArg(args[n], XmNaccelerators, new_accel);  n++; 

	pushbutton2 = XmCreatePushButton(bboard, "pushbutton2", args, n); 

	XmStringFree(string);

	XtManageChild (bboard);
	XtManageChild (pushbutton);
	XtManageChild (pushbutton2);

	XtRealizeWidget (Shell1);

	XtInstallAccelerators(pushbutton, pushbutton2);


	CommonPause();  

	n=0;
	XtSetArg(args[n], XmNwidth, 100);  n++; 
	XtSetArg(args[n], XmNheight, 100);  n++; 
	XtSetArg(args[n], XmNy, 300);  n++; 
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNvalue, text_string);  n++;
	text1 = XmCreateText(bboard, "text1", args, n);
	XtManageChild (text1);

	/* test case for PIR 2162, PIR 4275 */
	XtInstallAccelerators(text1, pushbutton2);
	/* end of test case for PIR 2162, PIR 4275 */

	CommonPause();  

	XtDestroyWidget(pushbutton);
	XtDestroyWidget(pushbutton2);
	XtDestroyWidget(text1);

	n=0;
	XtSetArg(args[n], XmNwidth, 100);  n++; 
	XtSetArg(args[n], XmNheight, 100);  n++; 
	string = XmStringLtoRCreate("Push Button 1", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNactivateCallback, activateCBs);  n++; 
	XtSetArg(args[n], XmNlabelType, XmSTRING);  n++; 
	XtSetArg(args[n], XmNlabelString, string);  n++; 

	pushbutton = XmCreatePushButton(bboard, "pushbutton", args, n); 

	XmStringFree(string);

	n=0;
	XtSetArg(args[n], XmNwidth, 100);  n++; 
	XtSetArg(args[n], XmNheight, 100);  n++; 
	XtSetArg(args[n], XmNy, 140);  n++; 
	string = XmStringLtoRCreate("Push Button 2", XmSTRING_DEFAULT_CHARSET);
	XtSetArg(args[n], XmNactivateCallback, activateCBs);  n++; 
	XtSetArg(args[n], XmNlabelType, XmSTRING);  n++; 
	XtSetArg(args[n], XmNlabelString, string);  n++; 

	pushbutton2 = XmCreatePushButton(bboard, "pushbutton2", args, n); 

	XmStringFree(string);

	new_accel = XtParseAcceleratorTable(new_accelerators2);

	n=0;
	XtSetArg(args[n], XmNwidth, 100);  n++; 
	XtSetArg(args[n], XmNheight, 100);  n++; 
	XtSetArg(args[n], XmNy, 300);  n++; 
	XtSetArg(args[n], XmNeditMode, XmMULTI_LINE_EDIT); n++;
	XtSetArg(args[n], XmNvalue, text_string);  n++;
	XtSetArg(args[n], XmNaccelerators, new_accel);  n++; 
	text1 = XmCreateText(bboard, "text1", args, n);

	XtManageChild (pushbutton);
	XtManageChild (pushbutton2);
	XtManageChild (text1);

	XtInstallAccelerators(pushbutton, text1);

	CommonPause();  

        XtDestroyWidget( pushbutton );
        XtDestroyWidget( pushbutton2 );
        XtDestroyWidget( text1 );

       /* Begin testing CR 6142 */
        n = 0;
        XtSetArg( args[n], XmNresizePolicy, XmRESIZE_NONE ); n++;
        XtSetArg( args[n], XmNwidth, 400 ); n++;
        XtSetArg( args[n], XmNheight, 400 ); n++;
        XtSetArg( args[n], XmNmarginHeight, 0 ); n++;
        XtSetArg( args[n], XmNmarginWidth,0 ); n++;
        XtSetArg( args[n], XmNshadowThickness, 0 ); n++;
        XtSetArg( args[n], XmNborderWidth,0 ); n++;
        drawingAreaW = XmCreateDrawingArea( bboard, "window", args, n );
        XtManageChild( drawingAreaW );

        editorW = createEditor(drawingAreaW, 0, 80);

        string = XmStringCreateLtoR( "Add Callback",
                   XmFONTLIST_DEFAULT_TAG );

        n = 0;
        XtSetArg( args[n], XmNx, 100 ); n++;
        XtSetArg( args[n], XmNy, 0 ); n++;
        XtSetArg( args[n], XmNlabelString, string ); n++;
        pushbutton =
          XmCreatePushButton( drawingAreaW, "pushbutton", args, n );
        XtManageChild( pushbutton );

        XtAddCallback(pushbutton, XmNactivateCallback, AddCB, NULL);

        XmStringFree( string );

        CommonPause();


       /*  Reset text in the editor text widget and set font. */
        n = 0;
	XtSetArg( args[n], XmNvalue, "" ); n++;
	XtSetArg( args[n], XmNfontList, CommonGetFontList("12x24")); n++;
        XtSetValues( editorW, args, n );

        CommonPause();
      /*  End testing CR 6142 */

        CommonPause();
	XtAppMainLoop(app_context);
}
