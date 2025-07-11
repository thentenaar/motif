/* $XConsortium: DNDdropSiteD.c /main/5 1995/07/15 21:00:25 drk $ */
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
 * 
 */
/*
 * HISTORY
 */

#include <testlib.h>
#include "../Toolkit/bitmaps/gnu.bmp"
#include "DNDdropSiteD.h"

/*
 * Client data structure
 */

typedef struct {
		
		Pixel TestPixel;
		Pixmap TestPixmap;
		XmString TestString;
		Widget	TestWidget[2];
		
		
		int    TestInt;
		char *TestChar;
	        
	      } DropClientData;		


/*
 * Local functions
 */

static void
DragProc (Widget, XtPointer, XtPointer);

static void
DropProc (Widget, XtPointer, XtPointer);

/*
 * This is the drop site being  tested
 */

Widget DropSite;

void
main (int argc, char **argv)
{


  	Arg args[10];
  	int n;
	Widget Frame1;

	int num_widgets = XtNumber (WidgetStruct);  /* Number of widgets. */
	int w;
	

	/*
	 * dropSiteData related variables
	 */

	DropClientData MyClientData;
	Widget FormD1;
	
	Widget MyWidget[2];
	XmString MyString;
	Pixel MyPixel;
	Pixmap MyPixmap;

	int MyInt;
	char *MyChar;


	CommonTestInit (argc, argv);
	n=0;
	Frame1 = XmCreateFrame (Shell1, "Frame1", args, n);
	XtManageChild (Frame1);

	/*
	 * Create PushButton as default
	 */

	if (UserData == NULL)
	  {
	    n=0;
	    DropSite = XmCreatePushButton (Frame1, "DropSite", args, n);
	  }
	
	else
	  {
	    /*
	     * Create widget which user wants
	     */

	    w=0;
	    while (strcmp (UserData, WidgetStruct[w].name) != 0 && 
	       (w < num_widgets))
	      w++;

	    if (w==num_widgets) 
	      {
           	printf ("Not a valid widget!\n");
      		exit(0);
	      }
  
	    DropSite = (*WidgetStruct[w].CreateFunction)(Frame1,"DropSite"
							 ,args,n);
	   
	  }
	
	XtManageChild (DropSite);
   
   	/*
    	 * Initialize client data structure
    	 */
	
	n=0;
	FormD1 = XmCreateFormDialog (Shell1, "FormD1", args, n);
	XtManageChild (FormD1);

	
	n=0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	MyWidget[0]=
	XmCreatePushButton (FormD1, "PushB", args, n);
	XtManageChild (MyWidget[0]);

	n=0;
	XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET); n++;
	XtSetArg (args[n], XmNtopWidget, MyWidget[0]); n++;
	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	MyWidget[1]=
	XmCreatePushButton (FormD1, "PushB", args, n);
	XtManageChild (MyWidget[1]);	

	MyPixel = CommonGetColor ("Red");

	MyPixmap =  XCreatePixmapFromBitmapData(display,
                                DefaultRootWindow(display),
                                gnu_bits, gnu_width, gnu_height,
                                WhitePixel(display,DefaultScreen(display)),
                                BlackPixel(display,DefaultScreen(display)),
                                DefaultDepth(display,DefaultScreen(display)));
	MyString = XmStringCreate ("MyString", XmSTRING_DEFAULT_CHARSET);
	
	MyInt = 2;
	MyChar = ("The sixth sick sheikh's sixth sheep's sick\n");


	MyClientData.TestWidget[0] = MyWidget[0];
	MyClientData.TestWidget[1] = MyWidget[1];
	MyClientData.TestPixel = MyPixel;
	MyClientData.TestPixmap = MyPixmap;

	MyClientData.TestInt = MyInt;
	MyClientData.TestChar = MyChar;
	
	
	/*
	 * Create the drop site
	 */

      	n=0;
      	XtSetArg (args[n], XmNdragProc, DragProc); n++;
      	XtSetArg (args[n], XmNdropProc, DropProc); n++;
      	XtSetArg (args[n], XmNdropSiteData, (XtPointer)MyClientData); n++;
      	XmDropSiteRegister (DropSite, args, n);

      	XtRealizeWidget (Shell1);

	CommonPause();

	XtAppMainLoop(app_context);
  
}


static void
DragProc (Widget w, XtPointer call_data, XtPointer client_data)	
{
        int n;
	Arg args[10];
	
	DropClientData GetData;

	n=0;
	XtSetArg (args[n], XmNdropSiteData, &GetData);
	XmDropSiteRetrieve (w, args, n);

	if (call_data.reason == XmCR_DROP_SITE_ENTER_MESSAGE)
	   {
	   	printf ("%s\n", GetData.TestChar);
		printf ("The magic number is %d\n", GetData.TestInt);
	        XtSetArg (args[n], XmNbackground, GetData.TestPixel); n++;
	        XtSetArg (args[n], XmNlabelString, GetData.TestString); n++;
	        XtSetArg (args[n], XmNlabelPixmap, GetData.TestPixmap); n++;
		XtSetValues (GetData.TestWidget[0], args, n);
	   } 
}



static void
DropProc (Widget w, XtPointer call_data, XtPointer client_data)	
{
	
	DropClientData GetData;
	Arg args[10];
	int n;

	n=0;
	XtSetArg (args[n], XmNdropSiteData, &GetData);
	XmDropSiteRetrieve (w, args, n);

	if (call_data.reason == XmCR_DROP_SITE_ENTER_MESSAGE)
	   {
	   	printf ("%s\n", GetData.TestChar);
		printf ("The magic number is %d\n", GetData.TestInt);
	        XtSetArg (args[n], XmNbackground, GetData.TestPixel); n++;
	        XtSetArg (args[n], XmNlabelString, GetData.TestString); n++;
	        XtSetArg (args[n], XmNlabelPixmap, GetData.TestPixmap); n++;
		XtSetValues (GetData.TestWidget[1], args, n);
	   }

}

















