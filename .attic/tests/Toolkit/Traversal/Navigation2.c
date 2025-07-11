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
static char rcsid[] = "$XConsortium: Navigation2.c /main/6 1995/07/13 19:42:18 drk $"
#endif
#endif


#include <signal.h>
#include <X11/StringDefs.h>
#include <testlib.h>


Widget Form, Form1, Form2, Form3, Form4, Form5, Form6, Form7, Form8, Form9;
Widget PushButton1, PushButton2, PushButton3;
static void RunTest();
static void CreateForm();
static void DestroyForm();
static void PushButtonCallback();
static void CreateMatrix();
static char *color_name[] = {
  "Red",       "Green",       "Blue", 
  "Orange",    "Yellow",      "Pink",
  "Magenta",   "Brown",       "Violet",
};

enum { unmap, unmanage, insensitive, destroy } remove_mode;
extern char *UserData;
Widget frame;

/*  main  */

/*  main  */

void main (argc, argv)
unsigned int argc;
char **argv;
     
{
  Display *display;
  int screen;
  Arg args[25];
  int n;
  char c;

  /*  initialize toolkit  */
    CommonTestInit(argc, argv);

  if (UserData != NULL) {
    if (strcmp (UserData, "unmap") == 0) 
	remove_mode = unmap;
    else if (strcmp (UserData, "unmanage") == 0)
	remove_mode = unmanage;
    else if (strcmp (UserData, "insensitive") == 0)
 	remove_mode = insensitive;
    else 
        remove_mode = destroy;
  }
  else {
    printf ("Test error: no mode specified\n", UserData);
    exit (1);
  }
    
  n = 0;
  XtSetArg (args[n], XmNwidth, 1);               n++;
  XtSetArg (args[n], XmNheight, 1);              n++;
    XtSetValues(Shell1, args, n);
    
  XtRealizeWidget (Shell1);

  RunTest(Shell1);

  XtAppMainLoop(app_context);      
}


static void RunTest(Shell)
Widget Shell;
{
  Arg      args[MAX_ARGS];
  unsigned int n;
 
    n = 0;
    frame = XmCreateFrame(Shell, "frame", args, n);

		/* Part 1 of Test */

    CreateForm(frame);
    
    /*  Create the first Form.  */
    n = 0;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	        n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);			n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);                      n++;
    XtSetArg (args[n], XmNnumColumns, 3);				n++;
    Form1 = XmCreateRowColumn(Form, "Form1", args, n);
    
    /*  Create the second Form.  */
    n = 0;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNrightWidget, Form1);		                n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);                   n++;
    XtSetArg (args[n], XmNnumColumns, 3);                                  n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);                      n++;
    Form2 = XmCreateRowColumn(Form, "Form2", args, n);
    
    /*  Create the third Form.  */
    n = 0;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNrightWidget, Form2); 	                        n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);               n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);                   n++;
    XtSetArg (args[n], XmNnumColumns, 3);                                  n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);                      n++;
    Form3 = XmCreateRowColumn(Form, "Form3", args, n);
    
    /*  Create the fourth Form.  */
    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNtopWidget, Form1);	                        n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	        n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);                   n++;
    XtSetArg (args[n], XmNnumColumns, 3);                                  n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);                      n++;
    Form4 = XmCreateRowColumn(Form, "Form4", args, n);
    
    /*  Create the fifth Form.  */
    n = 0;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNrightWidget, Form4);		                n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNtopWidget, Form2);        	                n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);                   n++;
    XtSetArg (args[n], XmNnumColumns, 3);                                  n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);                      n++;
    Form5 = XmCreateRowColumn(Form, "Form5", args, n);
    
    /*  Create the sixth Form.  */
    n = 0;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNrightWidget, Form5); 	                        n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);               n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNtopWidget, Form3);    	                n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);                   n++;
    XtSetArg (args[n], XmNnumColumns, 3);                                  n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);                      n++;
    Form6 = XmCreateRowColumn(Form, "Form6", args, n);
    
    /*  Create the seventh Form.  */
    n = 0;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	        n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNtopWidget, Form4);    	                n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	        n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);                   n++;
    XtSetArg (args[n], XmNnumColumns, 3);                                  n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);                      n++;
    Form7 = XmCreateRowColumn(Form, "Form7", args, n);
    
    /*  Create the eigth Form.  */
    n = 0;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNrightWidget, Form7);		                n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNtopWidget, Form5);        	                n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	        n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);                   n++;
    XtSetArg (args[n], XmNnumColumns, 3);                                  n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);                      n++;
    Form8 = XmCreateRowColumn(Form, "Form8", args, n);
    
    /*  Create the ninth Form.  */
    n = 0;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNrightWidget, Form8); 	                        n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);               n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
    XtSetArg (args[n], XmNtopWidget, Form6);         	                n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	        n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);                   n++;
    XtSetArg (args[n], XmNnumColumns, 3);                                  n++;
    XtSetArg (args[n], XmNpacking, XmPACK_COLUMN);                      n++;
    Form9 = XmCreateRowColumn(Form, "Form9", args, n);

    /* Create Form Matrices. */
    CreateMatrix(Form1, color_name[0]);
    CreateMatrix(Form2, color_name[1]);
    CreateMatrix(Form3, color_name[2]);
    CreateMatrix(Form4, color_name[3]);
    CreateMatrix(Form5, color_name[4]);
    CreateMatrix(Form6, color_name[5]);
    CreateMatrix(Form7, color_name[6]);
    CreateMatrix(Form8, color_name[7]);
    CreateMatrix(Form9, color_name[8]);

    /* Manage children. */
    XtManageChild(Form1);
    XtManageChild(Form2);
    XtManageChild(Form3);
    XtManageChild(Form4);
    XtManageChild(Form5);
    XtManageChild(Form6);
    XtManageChild(Form7);
    XtManageChild(Form8);
    XtManageChild(Form9);
    XtManageChild(Form);
    XtManageChild (frame);
 
    CommonPause();

    DestroyForm();


}

static void CreateForm(Shell)
Widget Shell;
{
  unsigned int n;
  Arg args[MAX_ARGS];
  
  /*  The form creation. */
  n = 0;
  XtSetArg (args[n], XmNpacking, XmPACK_COLUMN); n++;
  XtSetArg (args[n], XmNnumColumns, 3); n++;
  XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
  Form = XmCreateRowColumn (Shell, "Form", args, n);

}


static void DestroyForm()
{
  XtDestroyWidget(Form);
}

static void PushButtonCallback (pb, client_data, call_data)
     Widget pb;
     XtPointer client_data;
     XtPointer call_data;
{
  Arg args[1];

  switch (remove_mode) {
  case unmap:
    XtSetMappedWhenManaged (pb, False);
    XtUnmapWidget(pb);
#ifndef MOTIF1_1
    printf ("Focus on %s\n", XtName(XmGetFocusWidget(frame)));
    fflush (stdout);
#endif
    break;
  case unmanage:
    XtUnmanageChild(pb);
    break;
  case insensitive:
    XtSetArg (args[0], XmNsensitive, False); 
    XtSetValues (pb, args, 1);
    break;
  case destroy:
    XtDestroyWidget(pb);
    break;
  }

}

static void CreateMatrix(form, color)
     Widget form;
     char* color;
{
  int n;
  Arg args[MAX_ARGS];
  Widget PushButton1;
  Widget PushButton2;
  Widget PushButton3;
  Widget PushButton4;
  Widget PushButton5;
  Widget PushButton6;
  Widget PushButton7;
  Widget PushButton8;
  Widget PushButton9;
  char buf[256];
  
  extern void PushButtonCallback();


  /*  Create the first PushButton.  */
  strcpy (buf, color);
  strcat (buf, "1");
  n = 0;
  XtSetArg (args[n], XmNbackground, CommonGetColor(color));     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  PushButton1 = XmCreatePushButton(form, buf, args, n);
       
  /*  Create the second PushButton.  */
  strcpy (buf, color);
  strcat (buf, "2");
  n = 0;
  XtSetArg (args[n], XmNbackground, CommonGetColor(color));     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton1);		n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  PushButton2 = XmCreatePushButton(form, buf, args, n);
  
  /*  Create the third PushButton.  */
  strcpy (buf, color);
  strcat (buf, "3");
  n = 0;
  XtSetArg (args[n], XmNbackground, CommonGetColor(color));     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton2); 	        n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);         n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  PushButton3 = XmCreatePushButton(form, buf, args, n);

  /*  Create the fourth PushButton.  */
  strcpy (buf, color);
  strcat (buf, "4");
  n = 0;
  XtSetArg (args[n], XmNbackground, CommonGetColor(color));     n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton1);	        n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
  PushButton4 = XmCreatePushButton(form, buf, args, n);
       
  /*  Create the fifth PushButton.  */
  strcpy (buf, color);
  strcat (buf, "5");
  n = 0;
  XtSetArg (args[n], XmNbackground, CommonGetColor(color));     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton4);		n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton2);        	n++;
  PushButton5 = XmCreatePushButton(form, buf, args, n);

  /*  Create the sixth PushButton.  */
  strcpy (buf, color);
  strcat (buf, "6");
  n = 0;
  XtSetArg (args[n], XmNbackground, CommonGetColor(color));     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton5); 	        n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);         n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton3);    		n++;
  PushButton6 = XmCreatePushButton(form, buf, args, n);
  
  /*  Create the seventh PushButton.  */
  strcpy (buf, color);
  strcat (buf, "7");
  n = 0;
  XtSetArg (args[n], XmNbackground, CommonGetColor(color));     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton4);    		n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  PushButton7 = XmCreatePushButton(form, buf, args, n);
       
  /*  Create the eigth PushButton.  */
  strcpy (buf, color);
  strcat (buf, "8");
  n = 0;
  XtSetArg (args[n], XmNbackground, CommonGetColor(color));     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton7);		n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton5);        	n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  PushButton8 = XmCreatePushButton(form, buf, args, n);
  
  /*  Create the ninth PushButton.  */
  strcpy (buf, color);
  strcat (buf, "9");
  n = 0;
  XtSetArg (args[n], XmNbackground, CommonGetColor(color));     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton8); 	        n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);         n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton6);         	n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  PushButton9 = XmCreatePushButton(form, buf, args, n);

  /* Add Callbacks. */
  XtAddCallback(PushButton1, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton2, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton3, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton4, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton5, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton6, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton7, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton8, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton9, XmNactivateCallback, PushButtonCallback, NULL);

  /* Manage children. */
  XtManageChild(PushButton1);
  XtManageChild(PushButton2);
  XtManageChild(PushButton3);
  XtManageChild(PushButton4);
  XtManageChild(PushButton5);
  XtManageChild(PushButton6);
  XtManageChild(PushButton7);
  XtManageChild(PushButton8);
  XtManageChild(PushButton9);

}
