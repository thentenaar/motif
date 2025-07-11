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
static char rcsid[] = "$XConsortium: Form17.c /main/4 1995/07/13 18:03:31 drk $"
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
extern char *optarg;
static char *color_name[] = {
  "Red",       "Green",       "Blue", 
  "Orange",    "Yellow",      "Pink",
  "Magenta",   "Brown",       "Violet",
};

XColor exact_def;
Colormap cmap;
int ncolors = 9;
unsigned long color[9];

enum { unmap, unmanage, destroy } remove_mode;

void main (argc, argv)
unsigned int argc;
char **argv;
{
  int screen;
  Arg args[25];
  int n;
  char c;


   
       CommonTestInit(argc, argv);

       if (UserData == NULL)
	 {
	   printf ("Usage : Form17 -u [destroy unmap unmanage]\n");
	   printf ("remove_mode being set to destroy\n");
	   remove_mode = destroy;
	 }



      if (UserData && (strcmp (UserData, "destroy")  == 0))
	remove_mode = destroy;
      else
      if (UserData && (strcmp (UserData, "unmap")  == 0))
	remove_mode = unmap;
      else
      if (UserData && (strcmp (UserData, "unmanage")  == 0))
	remove_mode = unmanage;
    
  n = 0;
  XtSetArg (args[n], XmNwidth, 1);               n++;
  XtSetArg (args[n], XmNheight, 1);              n++;
  XtSetValues(Shell1, args, n);
    
  XtRealizeWidget (Shell1);

  CreateForm(Shell1);
    
  n = 0;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	         n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		 n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	         n++;
  PushButton1 = XmCreatePushButton(Form, "PushButton1", args, n);
  XtAddCallback(PushButton1, XmNactivateCallback, PushButtonCallback, NULL);
    
  n = 0;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);	         n++;
  XtSetArg (args[n], XmNleftWidget, PushButton1);		         n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		 n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	         n++;
  PushButton2 = XmCreatePushButton(Form, "PushButton2", args, n);
  XtAddCallback(PushButton2, XmNactivateCallback, PushButtonCallback, NULL);
    
  n = 0;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);	         n++;
  XtSetArg (args[n], XmNleftWidget, PushButton2); 	                 n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);               n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		 n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	         n++;
  PushButton3 = XmCreatePushButton(Form, "PushButton3", args, n);
  XtAddCallback(PushButton3, XmNactivateCallback, PushButtonCallback, NULL);

  XtManageChild(Form);
  XtManageChild(PushButton1);
  XtManageChild(PushButton2);
  XtManageChild(PushButton3);


  CommonPause();
  CommonPause();
  CommonPause();

  DestroyForm();
  CreateForm(Shell1);
    

  XtManageChild(Form);
  CreateMatrix(Form, color[0]);

    
  CommonPause();

  DestroyForm();
  CreateForm(Shell1);
    
  n = 0;
  XtSetArg (args[n], XmNallowOverlap, False);                         n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	        n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  Form1 = XmCreateForm(Form, "Form1", args, n);
    
  n = 0;
  XtSetArg (args[n], XmNallowOverlap, False);                         n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNrightWidget, Form1);		                n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  Form2 = XmCreateForm(Form, "Form2", args, n);
    
  n = 0;
  XtSetArg (args[n], XmNallowOverlap, False);                         n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNrightWidget, Form2); 	                        n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);               n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  Form3 = XmCreateForm(Form, "Form3", args, n);
    
  n = 0;
  XtSetArg (args[n], XmNallowOverlap, False);                         n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNtopWidget, Form3);	                        n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	        n++;
  Form4 = XmCreateForm(Form, "Form4", args, n);
    
  n = 0;
  XtSetArg (args[n], XmNallowOverlap, False);                         n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNrightWidget, Form4);		                n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNtopWidget, Form2);        	                n++;
  Form5 = XmCreateForm(Form, "Form5", args, n);
    
  n = 0;
  XtSetArg (args[n], XmNallowOverlap, False);                         n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNrightWidget, Form5); 	                        n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);               n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNtopWidget, Form1);    	                n++;
  Form6 = XmCreateForm(Form, "Form6", args, n);
    
  n = 0;
  XtSetArg (args[n], XmNallowOverlap, False);                         n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	        n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNtopWidget, Form4);    	                n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	        n++;
  Form7 = XmCreateForm(Form, "Form7", args, n);
    
  n = 0;
  XtSetArg (args[n], XmNallowOverlap, False);                         n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNrightWidget, Form7);		                n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNtopWidget, Form5);        	                n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	        n++;
  Form8 = XmCreateForm(Form, "Form8", args, n);
    
  n = 0;
  XtSetArg (args[n], XmNallowOverlap, False);                         n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNrightWidget, Form8); 	                        n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);               n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	        n++;
  XtSetArg (args[n], XmNtopWidget, Form6);         	                n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	        n++;
  Form9 = XmCreateForm(Form, "Form9", args, n);


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

  CreateMatrix(Form1, color[0]);
  CreateMatrix(Form2, color[1]);
  CreateMatrix(Form3, color[2]);
  CreateMatrix(Form4, color[3]);
  CreateMatrix(Form5, color[4]);
  CreateMatrix(Form6, color[5]);
  CreateMatrix(Form7, color[6]);
  CreateMatrix(Form8, color[7]);
  CreateMatrix(Form9, color[8]);


    
  CommonPause();


/*  DestroyForm(); */

  XtAppMainLoop(app_context);      
}

static void CreateForm(Shell)
Widget Shell;
{
  unsigned int n;
  Arg args[MAX_ARGS];
  
  n = 0;
  XtSetArg (args[n], XmNallowOverlap, False);                          n++;  
  Form = XmCreateForm (Shell, "Form", args, n);
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
  switch (remove_mode) {
  case unmap:
    XtUnmapWidget(pb);
    break;
  case unmanage:
    XtUnmanageChild(pb);
    break;
  case destroy:
    XtDestroyWidget(pb);
    break;
  }

}

static void CreateMatrix(form, color)
     Widget form;
     unsigned long color;
{
  int n;
  Arg args[MAX_ARGS];
  char buf[32];
  static short button_num = 1;
  Widget PushButton1;
  Widget PushButton2;
  Widget PushButton3;
  Widget PushButton4;
  Widget PushButton5;
  Widget PushButton6;
  Widget PushButton7;
  Widget PushButton8;
  Widget PushButton9;
  extern void PushButtonCallback();

  n = 0;
  XtSetArg (args[n], XmNbackground, color);                     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  sprintf (buf, "PushButton%d", button_num++);
  PushButton1 = XmCreatePushButton(form, buf, args, n);
       
  n = 0;
  XtSetArg (args[n], XmNbackground, color);                     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton1);		n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  sprintf (buf, "PushButton%d", button_num++);
  PushButton2 = XmCreatePushButton(form, buf, args, n);
  
  n = 0;
  XtSetArg (args[n], XmNbackground, color);                     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton2); 	        n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);        n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
  sprintf (buf, "PushButton%d", button_num++);
  PushButton3 = XmCreatePushButton(form, buf, args, n);

  n = 0;
  XtSetArg (args[n], XmNbackground, color);                     n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton1);	        n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
  sprintf (buf, "PushButton%d", button_num++);
  PushButton4 = XmCreatePushButton(form, buf, args, n);
       
  n = 0;
  XtSetArg (args[n], XmNbackground, color);                     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton4);		n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton2);        	n++;
  sprintf (buf, "PushButton%d", button_num++);
  PushButton5 = XmCreatePushButton(form, buf, args, n);

  n = 0;
  XtSetArg (args[n], XmNbackground, color);                     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton5); 	        n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);         n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton3);    	n++;
  sprintf (buf, "PushButton%d", button_num++);
  PushButton6 = XmCreatePushButton(form, buf, args, n);
  
  n = 0;
  XtSetArg (args[n], XmNbackground, color);                     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton4);    	n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  sprintf (buf, "PushButton%d", button_num++);
  PushButton7 = XmCreatePushButton(form, buf, args, n);
       
  n = 0;
  XtSetArg (args[n], XmNbackground, color);                     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton7);		n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton5);        	n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  sprintf (buf, "PushButton%d", button_num++);
  PushButton8 = XmCreatePushButton(form, buf, args, n);
  
  n = 0;
  XtSetArg (args[n], XmNbackground, color);                     n++;
  XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNrightWidget, PushButton8); 	        n++;
  XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);         n++;
  XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);	n++;
  XtSetArg (args[n], XmNtopWidget, PushButton6);         	n++;
  XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
  sprintf (buf, "PushButton%d", button_num++);
  PushButton9 = XmCreatePushButton(form, buf, args, n);

  XtAddCallback(PushButton1, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton2, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton3, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton4, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton5, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton6, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton7, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton8, XmNactivateCallback, PushButtonCallback, NULL);
  XtAddCallback(PushButton9, XmNactivateCallback, PushButtonCallback, NULL);

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
