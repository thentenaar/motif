 
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
static char rcsid[] = "$XConsortium: Form1.c /main/4 1995/07/13 18:00:28 drk $"
#endif
#endif


/*
 *  Create two PushButtons in a form.  The first being attached to the right
 *  and top sides of the form.  The second being attached to the left
 *  and bottom of the form and the left edge of the first PushButton.
 */


#include <signal.h>
#include <X11/StringDefs.h>
#include "ClassTables.h"

#define NUM_REPORT_LABELS 12

Widget        sides[4], attaches[7];
Widget        positionScale;
Widget        offsetScale;
Widget        target[2];
Widget        reportLabels[NUM_REPORT_LABELS];
Widget        form;  /* this is the form under test */

Dimension     child1_h, child1_w, child2_h, child2_w, child1_b, child2_b,
              child1_x, child1_y, child2_x, child2_y;
                                                     /* starting sizes */

/* these are used by several routines */

char *side_names[] =  { "Top", "Bottom", "Left", "Right"};
char *attaches_names[] = {
    "ATTACH_NONE",
    "ATTACH_FORM",
    "ATTACH_OPPOSITE_FORM",
    "ATTACH_WIDGET",
    "ATTACH_OPPOSITE_WIDGET",
    "ATTACH_POSITION",
    "ATTACH_SELF" };


/*  Forward declarations  */
/*extern char *UserData;
extern char *instruct_file_name; */
void CreateButtonChildren();
void FormCallback();
void CreateControlPanel();
void RefreshAttachmentsReport();
void ReportFormSize();
WidgetClass ChildClass();
void InitializeClassLookupTables();
Widget CreateChild();
void CreateKidControls();
void SetTargetWidth();
void SetTargetHeight();
void SetTargetxPosition();
void SetTargetyPosition();
void SetTargetBorderWidth();
void SetTargetLabelString();
void SetTargetFont();
void FormSizeControlCallback();
void GetChildWidthHeight();
void GetChildxyPosition();
void GetChildWidthHeight();
void GetChildBorderWidth();
void PositionScaleModifyQuery();
void ChangePositionScaleMax();

/*  main  */

void main (argc, argv)
unsigned int argc;
char **argv;

{
   Widget frame;
   Widget PushButton1;
   Widget PushButton2;
   Arg args[10];
   int n, i;

   WidgetClass child_class;

   /*  initialize toolkit  */
    CommonTestInit(argc, argv);

    XtRealizeWidget (Shell1);

   n=0;
   frame = XmCreateFrame(Shell1,"frame", args, n);
   XtManageChild(frame);

   /*  The form creation.  Contained in an outer frame.  */

   n = 0;
   XtSetArg (args[n], XmNfractionBase, 10); n++;
   form = XmCreateForm (frame, "form", args, n);
   XtManageChild (form);
   XtAddCallback (form, XmNhelpCallback, FormCallback, NULL);

   if (UserData != NULL) {
      InitializeClassLookupTables(); 
      child_class = ChildClass (UserData); 
      if (child_class != NULL) {
	CreateButtonChildren (form, child_class, 1);
	CreateButtonChildren (form, child_class, 0);   
      }
      else {
	printf ("Child class %s not a legal class. Exiting.\n", UserData);
	exit (1);
      }
    }
   else {
      printf ("Usage: %s -u xm<Child>WidgetClass\n",
       argv[0]);
      fflush (stdout);
      exit (1);
     }

   
  


   CreateControlPanel();



   if (instruct_file_name != NULL) {
     if (strncmp (instruct_file_name, "Form1a", 6) == 0)
       /* if you look carefully, you can spot P2253 
	  in this test in panel #3. */
       /* the circular dependency message shows up early & twice */
       for (i = 0; i <= 5; i++)
	 CommonPause();
     else if (strncmp (instruct_file_name, "Form1b", 6) == 0)
         CommonPause();
     else if (strncmp (instruct_file_name, "Form1c", 6) == 0)
         CommonPause();
     else if (strncmp (instruct_file_name, "Form1d", 6) == 0)
         CommonPause();
     else if (strncmp (instruct_file_name, "Form1e", 6) == 0)
         CommonPause();
     else if (strncmp (instruct_file_name, "Form1f", 6) == 0)
         CommonPause();
     else if (strncmp (instruct_file_name, "Form1g", 6) == 0)
       for (i = 0; i <= 2; i++) CommonPause();
     else if (strncmp (instruct_file_name, "Form1h", 6) == 0)
       for (i = 0; i <= 2; i++) CommonPause();
     else if (strncmp (instruct_file_name, "Form1k", 6) == 0)
       for (i = 0; i <= 2; i++) CommonPause();
   }
   else printf ("Test error: file %s not opened\n", instruct_file_name);

   XtAppMainLoop(app_context);
}

WidgetClass ChildClass (child_class_name)
char * child_class_name;

{
  int i;

  for (i = 0; i <= NUM_LEGAL_CHILDREN - 1; i++) 
    if (strcmp (child_class_name, child_table[i].string) == 0)
      return child_table[i].class;

  return ( (WidgetClass) NULL);

}

void InitializeClassLookupTables ()

{

/* 
   this grotesque hack is the simplest way I could think of to deal
   with the fact that the class pointer variables cannot be used
   in my lovely static arrays. A change to this array also requires changing
   both NUM_LEGAL_CHILDREN and the child_class array initialization in
   ClassTables.h.

*/

child_table[0].class = xmScaleWidgetClass;
child_table[1].class = xmScrollBarWidgetClass;
child_table[2].class = xmMessageBoxWidgetClass;
child_table[3].class = xmSelectionBoxWidgetClass;
child_table[4].class = xmCommandWidgetClass;
child_table[5].class = xmFileSelectionBoxWidgetClass;
child_table[6].class = xmArrowButtonWidgetClass;
child_table[7].class = xmListWidgetClass;
child_table[8].class = xmSeparatorWidgetClass;
child_table[9].class = xmTextWidgetClass;
child_table[10].class = xmTextFieldWidgetClass;
child_table[11].class = xmLabelWidgetClass;
child_table[12].class = xmDrawnButtonWidgetClass;
child_table[13].class = xmPushButtonWidgetClass;
child_table[14].class = xmToggleButtonWidgetClass;
child_table[15].class = xmArrowButtonGadgetClass;
child_table[16].class = xmSeparatorGadgetClass;
child_table[17].class = xmLabelGadgetClass;
child_table[18].class = xmPushButtonGadgetClass;
child_table[19].class = xmToggleButtonGadgetClass;
child_table[20].class = xmBulletinBoardWidgetClass;
child_table[21].class = xmFormWidgetClass;

}


void CreateButtonChildren (parent, child_class, whichchild)
Widget parent;
WidgetClass child_class;
int whichchild;

{

int n;
Arg args[5];

   /*  Arguments to the first child.  */

if (whichchild == 0) {
   n = 0;
   XtSetArg (args[n], XmNbackground, CommonGetColor ("blue"));  n++;
   target[0]=CreateChild (child_class, parent, "Child1", args, n);
   XtManageChild (target[0]);

   
   /* store starting size for Clear */
   XtSetArg (args[0], XmNheight, &child1_h);
   XtSetArg (args[1], XmNwidth, &child1_w);
   XtSetArg (args[2], XmNborderWidth, &child1_b);
   XtSetArg (args[3], XmNx, &child1_x);
   XtSetArg (args[4], XmNy, &child1_y);
   XtGetValues (target[0], args, 5);
   return;
 }

if (whichchild == 1) {

   /*  Arguments to the second child.  */

    n = 0;
    XtSetArg (args[n], XmNbackground, CommonGetColor ("yellow")); n++;
    target[1]=CreateChild (child_class, parent, "Child2", args, n);
    XtManageChild (target[1]);

   /* store starting size for Clear */
   XtSetArg (args[0], XmNheight, &child2_h);
   XtSetArg (args[1], XmNwidth, &child2_w);
   XtSetArg (args[2], XmNborderWidth, &child2_b);
   XtSetArg (args[3], XmNx, &child2_x);
   XtSetArg (args[4], XmNy, &child2_y);
   XtGetValues (target[1], args, 5);
   return;
 }

}

/* modification for automation */
Widget CreateChild (child_class, parent, name, args, n)
WidgetClass child_class;
Widget parent;
char name[6];
Arg args[10];
int n;

{
    Widget wid;

    if (child_class == xmScaleWidgetClass)
    wid = XmCreateScale (parent, name, args, n); 

    if (child_class == xmScrollBarWidgetClass)
    wid = XmCreateScrollBar (parent, name, args, n); 

    if (child_class == xmMessageBoxWidgetClass)
    wid = XmCreateMessageBox (parent, name, args, n); 

    if (child_class == xmSelectionBoxWidgetClass)
    wid = XmCreateSelectionBox (parent, name, args, n); 

    if (child_class == xmCommandWidgetClass)
    wid = XmCreateCommand (parent, name, args, n); 

    if (child_class == xmFileSelectionBoxWidgetClass)
    wid = XmCreateFileSelectionBox (parent, name, args, n); 

    if (child_class == xmArrowButtonWidgetClass)
    wid = XmCreateArrowButton (parent, name, args, n); 

    if (child_class == xmListWidgetClass)
    wid = XmCreateList (parent, name, args, n); 

    if (child_class == xmSeparatorWidgetClass)
    wid = XmCreateSeparator (parent, name, args, n); 

    if (child_class == xmTextWidgetClass)
    wid = XmCreateText (parent, name, args, n); 

    if (child_class == xmTextFieldWidgetClass)
    wid = XmCreateTextField (parent, name, args, n); 

    if (child_class == xmLabelWidgetClass)
    wid = XmCreateLabel (parent, name, args, n); 

    if (child_class == xmDrawnButtonWidgetClass)
    wid = XmCreateDrawnButton (parent, name, args, n); 

    if (child_class == xmPushButtonWidgetClass)
    wid = XmCreatePushButton (parent, name, args, n); 

    if (child_class == xmArrowButtonGadgetClass)
    wid = XmCreateArrowButtonGadget (parent, name, args, n); 

    if (child_class == xmSeparatorGadgetClass)
    wid = XmCreateSeparatorGadget (parent, name, args, n); 

    if (child_class == xmLabelGadgetClass)
    wid = XmCreateLabelGadget (parent, name, args, n); 

    if (child_class == xmPushButtonGadgetClass)
    wid = XmCreatePushButtonGadget (parent, name, args, n); 

    if (child_class == xmToggleButtonGadgetClass)
    wid = XmCreateToggleButtonGadget (parent, name, args, n); 

    if (child_class == xmBulletinBoardWidgetClass)
    wid = XmCreateBulletinBoard(parent, name, args, n); 

    if (child_class == xmFormWidgetClass)
    wid = XmCreateForm(parent, name, args, n); 

    return (wid);

  
}


    


void  QuitCB(w, client_data, call_data)
    Widget   w;
    XtPointer  client_data;
    XtPointer  call_data;

{
	printf("Exiting...Please Standby...\n");
	exit(0);
}

void FormCallback (fw, closure, call_data)
XmFormWidget fw;
XtPointer closure;
XtPointer call_data;
{
   int reason;

   XmAnyCallbackStruct * call_value = (XmAnyCallbackStruct *) call_data;

   reason = call_value -> reason;

   switch (reason)
   {
      case XmCR_HELP:
         printf ("Reason = XmCR_HELP\n");
      break;

      default:
         printf ("Test Warning: incorrect reason sent!!\n");
      break;
   }
}

void ChangeAttachments(w, client_data, call_data)
Widget w;
XtPointer client_data;
XtPointer call_data;

{
  static char *whichside[] = {
    "topAttachment",
    "bottomAttachment",
    "leftAttachment",
    "rightAttachment"
    };

  static int whichattach[] = {
    XmATTACH_NONE,
    XmATTACH_FORM,
    XmATTACH_OPPOSITE_FORM,
    XmATTACH_WIDGET,
    XmATTACH_OPPOSITE_WIDGET,
    XmATTACH_POSITION,
    XmATTACH_SELF };

  Arg args[8];
  int i = 0;
  int j = 0;
  int n = 0;

  int value; /* for scale value */
  int offset; /* for offset scale */
  
  int target_number = 0;
  if ( (int)(long)client_data == 0 ) target_number = 1;
  
  while (!(XmToggleButtonGadgetGetState(sides[i])))
    i++;

  while (!(XmToggleButtonGadgetGetState(attaches[j])))
    j++;

   XmScaleGetValue (offsetScale, &offset);

  if ((j == 1) || (j == 2)) { /* ATTACH_FORM or ATTACH_OPPOSITE_FORM */
    switch (i) {
    case 0:  
      XtSetArg (args[n], XmNtopOffset, offset); 
      n++;     /* we use !((int)client_data because you are setting the */
      break;   /* OTHER widget to be the top, bottom, left, rightWidget!!! */

    case 1:  
      XtSetArg (args[n], XmNbottomOffset, offset);
       n++; 
      break;

    case 2:  
       XtSetArg (args[n], XmNleftOffset, offset); 
        n++; 
      break;

    case 3:  
        XtSetArg (args[n], XmNrightOffset, offset);
        n++; 
      break;
    }
  }

  if ((j == 3) || (j == 4)) {  /* ATTACH_WIDGET or ATTACH_OPPOSITE_WIDGET */
    switch (i) {
    case 0:  
      XtSetArg (args[n], XmNtopWidget, target[target_number]);
      n++;     /* we use !((int)client_data because you are setting the */
      XtSetArg (args[n], XmNtopOffset, offset);
      n++;
      break;   /* OTHER widget to be the top, bottom, left, rightWidget!!! */

    case 1:  
      XtSetArg (args[n], XmNbottomWidget, target[target_number]);
      n++;
      XtSetArg (args[n], XmNbottomOffset, offset); 
      n++;
      break;

    case 2:  
      XtSetArg (args[n], XmNleftWidget, target[target_number]);
      n++;
      XtSetArg (args[n], XmNleftOffset, offset);
      n++;
      break;

    case 3:  
      XtSetArg (args[n], XmNrightWidget, target[target_number]);
      n++;
      XtSetArg (args[n], XmNrightOffset, offset);
      n++;
      break;
    }
  }
 
  if (j == 5) {  /* ATTACH_POSITION */
    XmScaleGetValue (positionScale, &value);
    switch (i) {
    case 0:  
      XtSetArg (args[n], XmNtopPosition, value);
      n++;
      break;

    case 1:  
      XtSetArg (args[n], XmNbottomPosition, value);
      n++;
      break;

    case 2:  
      XtSetArg (args[n], XmNleftPosition, value);
      n++;
      break;

    case 3:  
      XtSetArg (args[n], XmNrightPosition, value);
      n++;
      break;
    }
  }      
  
  XtSetArg(args[n], whichside[i], whichattach[j]); n++;
  XtSetValues(target[!target_number], args, n);

  RefreshAttachmentsReport(reportLabels);
}

void ClearAttachments(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  XmString  tcs, tcs2;
  Arg args[12];
  int n;

/*  XtDestroyWidget (target[(int)client_data]); */

  if ((int)(long)client_data == 1) {   /* reset button 2 top left corner */
    n = 0;
    tcs = XmStringCreateSimple ("Child2");
    tcs2 = XmStringCreateSimple (""); 
    XtSetArg (args[n], XmNheight, child1_h); n++;
    XtSetArg (args[n], XmNwidth, child1_w); n++;
    XtSetArg (args[n], XmNborderWidth, child1_b); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNlabelString, tcs ); n++;
    XtSetArg (args[n], XmNtitleString, tcs2 ); n++;
    XtSetValues (target[(int)(long)client_data], args, n);
    XmStringFree(tcs);
    XmStringFree(tcs2);
  }
  else {  /* reset button 1 */
    n = 0;
    tcs = XmStringCreateSimple ("Child1");
    tcs2 = XmStringCreateSimple ("");
    XtSetArg (args[n], XmNheight, child2_h); n++;
    XtSetArg (args[n], XmNwidth, child2_w); n++;
    XtSetArg (args[n], XmNborderWidth, child2_b); n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_NONE); n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_NONE); n++;
    XtSetArg (args[n], XmNlabelString, tcs ); n++;
    XtSetArg (args[n], XmNtitleString, tcs2 ); n++;
    XtSetValues (target[(int)(long)client_data], args, n);
    XmStringFree(tcs);
    XmStringFree(tcs2);
  }

  RefreshAttachmentsReport(reportLabels);
}

void CreateControlPanel()
{
  Widget        popup;
  Widget        controlpanel;
  Widget        mainParent;
  Widget        actionsBoard, reportPanel, formPanel, kidPanel;
  Widget        rbox1, rbox2;

  Widget        button1, button2;
  Widget        clear1, clear2;


  Widget        form_rc;
  Widget        formSizeControl[10];
  Widget        resizeButton;

  Arg           args[MAX_ARGS];
  register int  n, i;
  char buf[32];
  XmString      tcs;


  n=0;
  XtSetArg (args[n], XmNx, 10); n++;
  XtSetArg (args[n], XmNy, 165); n++;
  XtSetArg(args[n], XtNallowShellResize, True); n++;
  popup = XtCreatePopupShell ("popup", topLevelShellWidgetClass, Shell1, args,
			      n);


  n = 0;
  XtSetArg (args[n], XmNwidth, 200); n++;
  XtSetArg (args[n], XmNallowResize, True); n++;
  controlpanel = XmCreateBulletinBoard(popup, "controlpanel", args, n);



  n = 0;
  XtSetArg (args[n],XmNwidth, 200); n++;
  mainParent = XmCreatePanedWindow (controlpanel, "mainParent", args, n);


  n = 0;
  XtSetArg (args[n], XmNallowResize, True); n++;
  actionsBoard = XmCreateBulletinBoard (mainParent, "actions", args, n);


  n = 0;
  XtSetArg (args[n], XmNisHomogeneous, True); n++;
  XtSetArg (args[n], XmNradioAlwaysOne, True); n++;
  XtSetArg (args[n], XmNx, 20); n++;
  XtSetArg (args[n], XmNy, 0); n++;
  rbox1 = XmCreateRadioBox (actionsBoard, "rbox1", args, n);


  n = 0;
  XtSetArg (args[n], XmNisHomogeneous, True); n++;
  XtSetArg (args[n], XmNradioAlwaysOne, True); n++;
  XtSetArg (args[n], XmNx, 200); n++;
  XtSetArg (args[n], XmNy, 0); n++;
  rbox2 = XmCreateRadioBox (actionsBoard, "rbox1", args, n);

    
  for (i = 0; i <= 3; i++)
    { 
      n = 0;
      tcs = XmStringCreate(side_names[i], XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[n], XmNlabelString, tcs ); n++;
      sprintf(buf, "sides%d", i);
      sides[i] = XmCreateToggleButtonGadget(rbox1, buf, args, n);
      XtManageChild(sides[i]);
      XmStringFree(tcs);
    }
 
 /* set one side on by default */

  XmToggleButtonGadgetSetState (sides[0], True, False);

  for (i = 0; i <= 6; i++)
    { 
      n = 0;
      tcs = XmStringCreate(attaches_names[i], XmSTRING_DEFAULT_CHARSET);
      XtSetArg(args[n], XmNlabelString, tcs ); n++;
      sprintf(buf, "attaches%d", i);
      attaches[i] = XmCreateToggleButtonGadget(rbox2, buf, args, n);
      XtManageChild(attaches[i]);
      XmStringFree(tcs);
    }

  XmToggleButtonGadgetSetState (attaches[1], True, False);

  n = 0;
  tcs = XmStringCreateSimple ("Move Child1");
  XtSetArg (args[n], XmNbackground, CommonGetColor ("blue")); n++;
  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  XtSetArg (args[n], XmNx, 20); n++;
  XtSetArg (args[n], XmNy, 250); n++;
  button1 = XmCreatePushButton(actionsBoard, "button1", args, n);
  XtManageChild (button1);
  XtAddCallback(button1, XmNactivateCallback, ChangeAttachments, 0);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreateSimple ("Clear Child1");
  XtSetArg (args[n], XmNbackground, CommonGetColor ("blue")); n++;
  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  XtSetArg (args[n], XmNx, 20); n++;
  XtSetArg (args[n], XmNy, 300); n++;
  clear1 = XmCreatePushButton(actionsBoard, "clear1", args, n);
  XtManageChild (clear1);
  XtAddCallback(clear1, XmNactivateCallback, ClearAttachments, 0);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreateSimple ("Position Attachment"); 
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNminimum, 1); n++;
  XtSetArg (args[n], XmNmaximum, 10); n++;
  XtSetArg (args[n], XmNshowValue, True); n++;
  XtSetArg (args[n], XmNtitleString, tcs ); n++; 
  XtSetArg (args[n], XmNx, 20); n++;
  XtSetArg (args[n], XmNy, 125); n++;
  XtSetArg (args[n], XmNscaleMultiple, 1); n++;
  positionScale = XmCreateScale (actionsBoard, "positionScale", args, n);
  XtManageChild (positionScale);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreateSimple ("Offset");
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNminimum, 0); n++;
  XtSetArg (args[n], XmNmaximum, 200); n++;
  XtSetArg (args[n], XmNshowValue, True); n++;
  XtSetArg (args[n], XmNtitleString, tcs ); n++; 
  XtSetArg (args[n], XmNx, 20); n++;
  XtSetArg (args[n], XmNy, 180); n++;
  XtSetArg (args[n], XmNscaleMultiple, 1); n++;
  offsetScale = XmCreateScale (actionsBoard, "offsetScale", args, n);
  XtManageChild (offsetScale);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreateSimple ("Move Child2");
  XtSetArg (args[n], XmNbackground, CommonGetColor ("yellow")); n++;
  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  XtSetArg (args[n], XmNx, 200); n++;
  XtSetArg (args[n], XmNy, 250); n++;
  button2 = XmCreatePushButton(actionsBoard, "button2", args, n);
  XtManageChild (button2);
  XtAddCallback(button2, XmNactivateCallback, ChangeAttachments, (XtPointer) 1);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreateSimple ("Clear Child2");
  XtSetArg (args[n], XmNbackground, CommonGetColor ("yellow")); n++;
  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  XtSetArg (args[n], XmNx, 200); n++;
  XtSetArg (args[n], XmNy, 300); n++;
  clear2 = XmCreatePushButton(actionsBoard, "clear2", args, n);
  XtManageChild (clear2);
  XtAddCallback(clear2, XmNactivateCallback, ClearAttachments, (XtPointer) 1);
  XmStringFree(tcs);

/* the second panel of PanedWindow contains the status of the attachments */
/* for buttons1 and 2 at all times */

  n = 0;
  XtSetArg (args[n], XmNnumColumns, 4); n++;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg (args[n], XmNallowResize, True); n++;
  reportPanel = XmCreateRowColumn (mainParent, "report", args, n);


  for (i = 0; i <= NUM_REPORT_LABELS - 1; i++) {
    n = 0;
    if (i%3 == 1) 
      XtSetArg (args[n], XmNalignment, XmALIGNMENT_CENTER); n++;
    sprintf(buf, "labelchildlabelchildi%d", i);
    reportLabels[i] = XmCreateLabelGadget (reportPanel, buf, args, n);
    XtManageChild (reportLabels[i]);
  }
  
  RefreshAttachmentsReport(reportLabels);

  n = 0;
  XtSetArg (args[n], XmNallowResize, True); n++;
  formPanel = XmCreateForm (mainParent, "formPanel", args, n);


  n = 0;
  XtSetArg (args[n], XmNisHomogeneous, False); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNnumColumns, 2); n++;
  form_rc = XmCreateRowColumn (formPanel, "form_rc", args, n);


  n = 0;
  tcs = XmStringCreateSimple ("Report Form Size");

  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNtopWidget, form_rc); n++;
  resizeButton = XmCreatePushButton (formPanel, "formsize", args, n);
  XtAddCallback (resizeButton, XmNactivateCallback, 
		 ReportFormSize, NULL);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreateSimple ("fraction base");
  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  formSizeControl[0] = XmCreateLabel (form_rc, "fractionBase", args, n);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreateSimple ("horizontal spacing");
  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  formSizeControl[1] = XmCreateLabel (form_rc, "horizontalSpacing", args, n);
  XmStringFree(tcs);

  n = 0;
  XtSetArg (args[n], XmNlabelString,
	    XmStringCreateSimple ("vertical spacing")); n++;
  formSizeControl[2] = XmCreateLabel (form_rc, "verticalSpacing", args, n);

  n = 0;
  XtSetArg (args[n], XmNlabelString,
	    XmStringCreateSimple ("form height")); n++;
  formSizeControl[3] = XmCreateLabel (form_rc, "formheight", args, n);

  n = 0;
  XtSetArg (args[n], XmNlabelString,
	    XmStringCreateSimple ("form width")); n++;
  formSizeControl[4] = XmCreateLabel (form_rc, "formwidth", args, n);

  for (i = 5; i <= 9; i++) {
    n = 0;
    sprintf (buf, "formSizeText%d", i);
    XtSetArg (args[n], XmNcolumns, 6);
    n++;
    formSizeControl[i] = XmCreateTextField (form_rc, buf, args, n);
    XtAddCallback (formSizeControl[i], XmNactivateCallback,
		   FormSizeControlCallback, (XtPointer) (long)i);
  }
    

  /* a panel of controls for controlling the children of Form */
  
   n=0;
    XtSetArg (args[n], XmNallowResize, True); n++;
  kidPanel = XmCreateForm (mainParent, "kidPanel", args, n);

  
  CreateKidControls(kidPanel);

  XtManageChild (kidPanel);
  XtManageChildren (formSizeControl, 10);
  XtManageChild (form_rc);
  XtManageChild (resizeButton);
  XtManageChild (formPanel);
  XtManageChild (rbox2);
  XtManageChild (rbox1);
  XtManageChild (actionsBoard);
  XtManageChild (reportPanel);
  XtManageChild (mainParent);
  XtManageChild (controlpanel);

  XtPopup (popup,XtGrabNone);


}

void ReportFormSize (widget, client_data, call_data)
Widget widget;
XtPointer client_data, call_data;

{
  Dimension h, w;
  Arg args[2];
  
  XtSetArg (args[0], XmNheight, &h);
  XtSetArg (args[1], XmNwidth, &w);
  XtGetValues (form, args, 2);

  printf ("Form height %d, width %d\n", (int) h, (int) w);

}

void FormSizeControlCallback (w, client_data, call_data)
Widget w;
int client_data;
XtPointer call_data;

{

  Arg args[1];
  char * value;

  value = XmTextFieldGetString (w);

  switch (client_data) {
        case 5: /* fractionBase, an int */
          XtSetArg (args[0], XmNfractionBase, atoi (value));
	  XtSetValues (form, args, 1);
	  /* ask user if they want to reset max for positionScale's */
	  PositionScaleModifyQuery();
	  break;
	case 6: /* horizontal spacing, a Dimension */
	  XtSetArg (args[0], XmNhorizontalSpacing, (Dimension) atoi (value));
	  XtSetValues (form, args, 1);
	  break;
        case 7: /* vertical spacing, a Dimension */
	  XtSetArg (args[0], XmNverticalSpacing, (Dimension) atoi (value));
	  XtSetValues (form, args, 1);
	  break;
	case 8: /* form height */
	  XtSetArg (args[0], XmNheight, (Dimension) atoi (value));
	  XtSetValues (form, args, 1);
	  break;
        case 9: /* form width */
	  XtSetArg (args[0], XmNwidth, (Dimension) atoi (value));
	  XtSetValues (form, args, 1);
	  break;
	default:
	  printf ("Error: wrong value in FormSizeControlCallback\n");
	  fflush (stdout);
	}

  XtFree (value);

}

void PositionScaleModifyQuery()
 
{
  /* ask the user if s/he wants to redo the max for positionScale
     when fractionBase is changed */

  static Widget popup, message, max_text, apply, cancel;
  int n;
  Arg args[4];

  n = 0;
  XtSetArg (args[n], XmNautoUnmanage, True); n++;
  XtSetArg (args[n], XmNdialogType, XmDIALOG_FULL_APPLICATION_MODAL); n++;
  popup = XmCreateBulletinBoardDialog (Shell1, "popup", args, n);
  XtPopup (popup,XtGrabNone);

  
  n = 0;
  XtSetArg (args[n], XmNx, 20); n++;
  XtSetArg (args[n], XmNy, 25); n++;
  XtSetArg (args[n], XmNlabelString,
	    XmStringCreateLtoR ("Enter new value for positionScale\nmax and Apply, or Cancel to leave as is.", "foo")); n++;
  message = XmCreateLabel (popup, "message", args, n);

  n = 0;
  XtSetArg (args[n], XmNx, 20); n++;
  XtSetArg (args[n], XmNy, 100); n++;
  XtSetArg (args[n], XmNwidth, 50); n++;
  max_text = XmCreateTextField (popup, "max_text", args, n);

  n = 0;
  XtSetArg (args[n], XmNlabelString,
	    XmStringCreateSimple ("Apply")); n++;
  XtSetArg (args[n], XmNx, 20); n++;
  XtSetArg (args[n], XmNy, 150); n++;
  apply = XmCreatePushButton (popup, "apply", args, n);
  XtAddCallback (apply, XmNactivateCallback, ChangePositionScaleMax, 
		 max_text);

  n = 0;
  XtSetArg (args[n], XmNlabelString,
	    XmStringCreateSimple ("Cancel")); n++;
  XtSetArg (args[n], XmNx, 100); n++;
  XtSetArg (args[n], XmNy, 150); n++;
  cancel = XmCreatePushButton (popup, "cancel", args, n);

  XtManageChild (cancel);
  XtManageChild (apply);
  XtManageChild (max_text);
  XtManageChild (message);
  XtManageChild (popup);

}

void ChangePositionScaleMax (w, client_data, call_data)
Widget w, client_data;
XtPointer call_data;

{
  char * string;
  Arg args[1];
  int cur_max, new_max;
  
  string = XmTextFieldGetString (client_data);

  XtSetArg (args[0], XmNmaximum, &cur_max);
  XtGetValues (positionScale, args, 1);

  if ((new_max = atoi (string)) != cur_max) {
    XtSetArg (args[0], XmNmaximum, new_max);
    XtSetValues (positionScale, args, 1);
  }

  XtFree (string);

}

void RefreshAttachmentsReport(reportLabels)
Widget *reportLabels;

{
int i, n;
XmString cs;
static int created = 0;
Arg args[6];
unsigned char top, bottom, left, right;

/* what we are building is this: 
 *  Button1attachment         TOP        Button2attachment
 *        ditto             BOTTOM             ditto
 *        ditto              LEFT              ditto
 *        ditto              RIGHT             ditto
 */

/* labels 1, 4, 7, and 10 are always the same */
/* only create them once! */
  if (!created) {
    n = 0;
    cs = XmStringCreateSimple ("          Top         ");
    XtSetArg (args[n], XmNlabelString, cs); n++;
    XtSetValues (reportLabels[1], args, n);
    XmStringFree(cs);

    n = 0;
    cs = XmStringCreateSimple ("         Bottom       ");
    XtSetArg (args[n], XmNlabelString, cs); n++;
    XtSetValues (reportLabels[4], args, n);
    XmStringFree(cs);

    n = 0;
    cs = XmStringCreateSimple ("          Left        ");
    XtSetArg (args[n], XmNlabelString, cs); n++;
    XtSetValues (reportLabels[7], args, n);
    XmStringFree(cs);

    n = 0;
    cs = XmStringCreateSimple ("         Right        ");
    XtSetArg (args[n], XmNlabelString, cs); n++;
    XtSetValues (reportLabels[10], args, n);
    XmStringFree(cs);

    created = 1;
    }

/* 
   labels 0, 3, 6, 9 contain button 1's attachments. Go get 'em, then
   use that value as the index into attaches_names[] and print out
   the string.
 */

  n = 0;
  XtSetArg (args[n], XmNtopAttachment, &top); n++;
  XtSetArg (args[n], XmNbottomAttachment, &bottom); n++;
  XtSetArg (args[n], XmNleftAttachment, &left); n++;
  XtSetArg (args[n], XmNrightAttachment, &right); n++;
  XtGetValues (target[0], args, n);

#ifdef DEBUG
  printf ("Button1:\n  top: %d\n,  bottom: %d\n,  left:  %d\n  right %d\n", 
	  (int) top, (int) bottom, (int) left, (int) right);
#endif

  n = 0;
  cs = XmStringCreateSimple (attaches_names[top]);
  XtSetArg (args[n], XmNlabelString, cs); n++;
  XtSetValues (reportLabels[0], args, n);
  XmStringFree(cs);

  n = 0;
  cs = XmStringCreateSimple (attaches_names[bottom]);
  XtSetArg (args[n], XmNlabelString, cs); n++;
  XtSetValues (reportLabels[3], args, n);
  XmStringFree(cs);

  n = 0;
  cs = XmStringCreateSimple (attaches_names[left]);
  XtSetArg (args[n], XmNlabelString, cs); n++;
  XtSetValues (reportLabels[6], args, n);
  XmStringFree(cs);

  n = 0;
  cs = XmStringCreateSimple (attaches_names[right]);
  XtSetArg (args[n], XmNlabelString, cs); n++;
  XtSetValues (reportLabels[9], args, n);
  XmStringFree(cs);


/*
 and now the same exact thing, for button2 (target[1]),
 labels 2, 5, 8, 11
*/

  n = 0;
  XtSetArg (args[n], XmNtopAttachment, &top); n++;
  XtSetArg (args[n], XmNbottomAttachment, &bottom); n++;
  XtSetArg (args[n], XmNleftAttachment, &left); n++;
  XtSetArg (args[n], XmNrightAttachment, &right); n++;
  XtGetValues (target[1], args, n);

#ifdef DEBUG
  printf ("Button1:\n  top: %d\n,  bottom: %d\n,  left:  %d\n  right %d\n", 
	  (int) top, (int) bottom, (int) left, (int) right);
#endif

  n = 0;
  cs = XmStringCreateSimple (attaches_names[top]);
  XtSetArg (args[n], XmNlabelString, cs); n++;
  XtSetValues (reportLabels[2], args, n);
  XmStringFree(cs);

  n = 0;
  cs = XmStringCreateSimple (attaches_names[bottom]);
  XtSetArg (args[n], XmNlabelString, cs); n++;
  XtSetValues (reportLabels[5], args, n);
  XmStringFree(cs);

  n = 0;
  cs = XmStringCreateSimple (attaches_names[left]);
  XtSetArg (args[n], XmNlabelString, cs); n++;
  XtSetValues (reportLabels[8], args, n);
  XmStringFree(cs);

  n = 0;
  cs = XmStringCreateSimple (attaches_names[right]);
  XtSetArg (args[n], XmNlabelString, cs); n++;
  XtSetValues (reportLabels[11], args, n);
  XmStringFree (cs);

}

void CreateKidControls(parent)
Widget parent;
{ 
  Widget measure_rc, target_rc;
  static Widget target_toggle[2];
  Widget width_rc, height_rc, border_rc, label_rc, font_rc, x_rc, y_rc;
  Widget width_pair[2], height_pair[2], border_pair[2], x_pair[2], y_pair[2],
         label_pair[2], font_pair[2];
  Widget WHButton, xyButton, BorderButton;
  XmString tcs;
  Arg args[10];
  int n;
 
  /* parent is a Form */

  n = 0;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg (args[n], XmNradioBehavior, True); n++;
  target_rc = XmCreateRowColumn (parent, "target_rc", args, n);

  n = 0;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg (args[n], XmNradioBehavior, True); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNtopWidget, target_rc); n++;
  measure_rc = XmCreateRowColumn (parent, "measure_rc", args, n);

  n = 0;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNtopWidget, measure_rc); n++;
  width_rc = XmCreateRowColumn (parent, "width_rc", args, n);

  n = 0;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNtopWidget, width_rc); n++;
  height_rc = XmCreateRowColumn (parent, "height_rc", args, n);


  n = 0;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNtopWidget, measure_rc); n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget, width_rc); n++;
  x_rc = XmCreateRowColumn (parent, "x_rc", args, n);


  n = 0;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNtopWidget, x_rc); n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget, x_rc); n++;
  y_rc = XmCreateRowColumn (parent, "y_rc", args, n);





  n = 0;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNtopWidget, measure_rc); n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget, x_rc); n++;
  border_rc = XmCreateRowColumn (parent, "border_rc", args, n);

  n = 0;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget, border_rc); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg (args[n], XmNtopWidget, width_rc); n++;
  label_rc = XmCreateRowColumn (parent, "label_rc", args, n);

  n = 0;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  XtSetArg (args[n], XmNnumColumns, 1); n++;
  XtSetArg (args[n], XmNpacking, XmPACK_TIGHT); n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
  XtSetArg (args[n], XmNtopWidget, label_rc); n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg (args[n], XmNleftWidget, label_rc); n++;
  font_rc = XmCreateRowColumn (parent, "font_rc", args, n);

  /* create the target toggles */

  n = 0;
  tcs = XmStringCreateSimple ("Set child 1"); 
  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  XtSetArg (args[n], XmNset, True); n++;
  XtSetArg (args[n], XmNselectColor, CommonGetColor ("blue")); n++;
  target_toggle[0] = XmCreateToggleButtonGadget (target_rc, "target0", 
						 args, n);
  XmStringFree(tcs);
  
  n = 0;
  tcs = XmStringCreateSimple ("Set child 2");
  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  XtSetArg (args[n], XmNselectColor, CommonGetColor ("yellow")); n++;
  target_toggle[1] = XmCreateToggleButtonGadget (target_rc, "target1", 
						 args, n);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreateSimple ("Get Child Width & Height");
  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  WHButton = XmCreatePushButton (measure_rc, "WHButton", args, n);
  XtAddCallback (WHButton, XmNactivateCallback, 
		 GetChildWidthHeight, target_toggle[0]);
  XmStringFree(tcs);

  n = 0;
  tcs = XmStringCreateSimple ("Get Child x & y Position"); 
  XtSetArg (args[n], XmNlabelString, tcs ); n++;
  xyButton = XmCreatePushButton (measure_rc, "xyButton", args, n);
  XtAddCallback (xyButton, XmNactivateCallback, 
		 GetChildxyPosition, target_toggle[0]);
  XmStringFree(tcs);

  n = 0;
  XtSetArg (args[n], XmNlabelString,
	    XmStringCreateSimple ("Get Child Border Width")); n++;
  BorderButton = XmCreatePushButton (measure_rc, "BorderButton", args, n);
  XtAddCallback (BorderButton, XmNactivateCallback, 
		 GetChildBorderWidth, target_toggle[0]);







  /* all kids will get width, height, value/label, font controls */
  /* other controls to be added as needed */

  n = 0;
  width_pair[0] = XmCreateLabelGadget (width_rc, "Width", args, n);
  height_pair[0] = XmCreateLabelGadget (height_rc, "Height", args, n);
  border_pair[0] = XmCreateLabelGadget (border_rc, "Border Width", args, n);
  label_pair[0] = XmCreateLabelGadget (label_rc, "LabelValue", args, n);
  font_pair[0] = XmCreateLabelGadget (font_rc, "Font", args, n);
  x_pair[0] = XmCreateLabelGadget (x_rc, "xPosition", args, n);
  y_pair[0] = XmCreateLabelGadget (y_rc, "yPosition", args, n);

  n = 0;
  XtSetArg (args[n], XmNcolumns, 5); n++;
  width_pair[1] = XmCreateTextField (width_rc, "widthText", args, n);
  height_pair[1] = XmCreateTextField (height_rc, "heightText", args, n);
  border_pair[1] = XmCreateTextField (border_rc, "BorderText", args, n);
  x_pair[1] = XmCreateTextField (x_rc, "xText", args, n);
  y_pair[1] = XmCreateTextField (y_rc, "yText", args, n);

  n = 0;
  XtSetArg (args[n], XmNcolumns, 25); n++;
  label_pair[1] = XmCreateTextField (label_rc, "labelText", args, n);
  font_pair[1] = XmCreateTextField (font_rc, "fontText", args, n);

  XtAddCallback (width_pair[1], XmNactivateCallback, 
		 SetTargetWidth, target_toggle[0]);

  XtAddCallback (height_pair[1],  XmNactivateCallback, 
		 SetTargetHeight, target_toggle[0]);
	       
  XtAddCallback (label_pair[1], XmNactivateCallback, 
		 SetTargetLabelString, target_toggle[0]);

  XtAddCallback (border_pair[1], XmNactivateCallback, 
		 SetTargetBorderWidth, target_toggle[0]);

  XtAddCallback (x_pair[1], XmNactivateCallback, 
		 SetTargetxPosition, target_toggle[0]);

  XtAddCallback (y_pair[1], XmNactivateCallback, 
		 SetTargetyPosition, target_toggle[0]);

  XtAddCallback (font_pair[1], XmNactivateCallback, 
		 SetTargetFont, target_toggle[0]);

  /* manage everyone */
  XtManageChildren (label_pair, 2);
  XtManageChildren (font_pair, 2);
  XtManageChildren (width_pair, 2);
  XtManageChildren (height_pair, 2);
  XtManageChildren (border_pair, 2);
  XtManageChildren (x_pair, 2);
  XtManageChildren (y_pair, 2);
  XtManageChildren (target_toggle, 2);
  XtManageChild (WHButton);
  XtManageChild (xyButton);
  XtManageChild (BorderButton);

  XtManageChild (width_rc);
  XtManageChild (height_rc);
  XtManageChild (x_rc);
  XtManageChild (y_rc);
  XtManageChild (border_rc);  
  XtManageChild (label_rc);
  XtManageChild (font_rc);
  XtManageChild (measure_rc);
  XtManageChild (target_rc);

  return; 

}

void SetTargetHeight (w, client_data, call_data)
Widget w;
Widget client_data;
XtPointer call_data;

{
  Arg args[1];
  char *text_value;

  text_value = XmTextFieldGetString (w);
  XtSetArg (args[0], XmNheight, (Dimension) atoi (text_value)); 

  /* which target to set */

  if (XmToggleButtonGetState (client_data))
    XtSetValues (target[0], args, 1);
  else
    XtSetValues (target[1], args, 1);

  XtFree (text_value);

}

void SetTargetWidth (w, client_data, call_data)
Widget w;
Widget client_data;
XtPointer call_data;

{
  Arg args[1];
  char *text_value;

  text_value = XmTextFieldGetString (w);
  XtSetArg (args[0], XmNwidth, (Dimension) atoi (text_value)); 

  /* which target to set */

  if (XmToggleButtonGetState ((Widget) client_data))
    XtSetValues (target[0], args, 1);
  else
    XtSetValues (target[1], args, 1);

  XtFree (text_value);

}

void SetTargetxPosition (w, client_data, call_data)
Widget w;
Widget client_data;
XtPointer call_data;

{
  Arg args[1];
  char *text_value;

  text_value = XmTextFieldGetString (w);
  XtSetArg (args[0], XmNx, (Dimension) atoi (text_value)); 

  /* which target to set */

  if (XmToggleButtonGetState ((Widget) client_data))
    XtSetValues (target[0], args, 1);
  else
    XtSetValues (target[1], args, 1);

  XtFree (text_value);

}

void SetTargetyPosition (w, client_data, call_data)
Widget w;
Widget client_data;
XtPointer call_data;

{
  Arg args[1];
  char *text_value;

  text_value = XmTextFieldGetString (w);
  XtSetArg (args[0], XmNy, (Dimension) atoi (text_value)); 

  /* which target to set */

  if (XmToggleButtonGetState ((Widget) client_data))
    XtSetValues (target[0], args, 1);
  else
    XtSetValues (target[1], args, 1);

  XtFree (text_value);

}

void SetTargetBorderWidth (w, client_data, call_data)
Widget w;
Widget client_data;
XtPointer call_data;

{
  Arg args[1];
  char *text_value;

  text_value = XmTextFieldGetString (w);
  XtSetArg (args[0], XmNborderWidth, (Dimension) atoi (text_value)); 

  /* which target to set */

  if (XmToggleButtonGetState ((Widget) client_data))
    XtSetValues (target[0], args, 1);
  else
    XtSetValues (target[1], args, 1);

  XtFree (text_value);

}




void SetTargetLabelString (w, client_data, call_data)
Widget w;
Widget client_data;
XtPointer call_data;

{
  XmString tcs;
  char * s;
  Arg args[2];

  s = XmTextFieldGetString (w);
  tcs = XmStringCreateSimple (s);


  XtSetArg (args[0], XmNlabelString, tcs);
  XtSetArg (args[1], XmNtitleString, tcs); 

  /* which target to set */

  if (XmToggleButtonGetState ((Widget) client_data))
    XtSetValues (target[0], args, 2);
  else
    XtSetValues (target[1], args, 2);

  XtFree (s);
  XmStringFree (tcs);

}




void SetTargetFont (w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;

{

}

void GetChildWidthHeight (widget, client_data, call_data)
Widget widget;
Widget client_data;
XtPointer call_data;

{
  Dimension h, w;
  Arg args[2];

  XtSetArg (args[0], XmNheight, &h);
  XtSetArg (args[1], XmNwidth, &w);
  
  if (XmToggleButtonGetState (client_data))  { /* get for target[0] */
    XtGetValues (target[0], args, 2);
    printf ("Child 1:\n");
  }
  else {
    XtGetValues (target[1], args, 2);
    printf ("Child 2:\n");
  }
  printf ("Width %d, Height %d\n", (int) w, (int) h);
  fflush (stdout);

}
 
void GetChildxyPosition (widget, client_data, call_data)
Widget widget;
Widget client_data;
XtPointer call_data;

{
  Dimension x, y;
  Arg args[2];

  XtSetArg (args[0], XmNx, &x);
  XtSetArg (args[1], XmNy, &y);
  
  if (XmToggleButtonGetState (client_data))  { /* get for target[0] */
    XtGetValues (target[0], args, 2);
    printf ("Child 1:\n");
  }
  else {
    XtGetValues (target[1], args, 2);
    printf ("Child 2:\n");
  }
  printf ("x %d, y %d\n", (int) x, (int) y);
  fflush (stdout);

} 

void GetChildBorderWidth (widget, client_data, call_data)
Widget widget;
Widget client_data;
XtPointer call_data;

{
  Dimension b;
  Arg args[1];

  XtSetArg (args[0], XmNborderWidth, &b);
  
  if (XmToggleButtonGetState (client_data))  { /* get for target[0] */
    XtGetValues (target[0], args, 1);
    printf ("Child 1:\n");
  }
  else {
    XtGetValues (target[1], args, 1);
    printf ("Child 2:\n");
  }
  printf ("Border Width %d\n ", (int) b);
  fflush (stdout);

} 









