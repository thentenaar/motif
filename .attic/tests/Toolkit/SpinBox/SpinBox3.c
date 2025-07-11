/* $XConsortium: SpinBox3.c /main/7 1996/04/30 13:56:24 schuldy $ */
/*
 *  (c) Copyright 1989, 1990, 1991, 1992, 1993 OPEN SOFTWARE FOUNDATION, INC. 
 *  ALL RIGHTS RESERVED 
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

#define NUM_ITEMS 12

void
ValueChangedCallback(Widget,XtPointer,XtPointer);


void
ModifyVerifyCallback(Widget,XtPointer,XtPointer);


void
main (int argc, char **argv)

{

    Widget Parent;
    Widget SpinBox1;
    Widget FocusChild[2];
    Widget NonFocusChild1;
    Arg args[32];
    int n,i;
    
    char *Child, *Manager;


    static char *months[] = {
                    "January",
                    "February",
                    "March",
                    "April",
                    "May",
                    "June",
                    "July",
                    "August",
                    "September",
                    "October",
                    "November",
                    "December"};

    XmString CSmonths[NUM_ITEMS], TempLabelString;


    CommonTestInit (argc,argv);

    if (UserData == NULL)

        {
            printf ("Usage: SpinBox3 -u XmParent,XmChildWidget\n");
            exit (0);
        };



    Manager = strtok (UserData,",");
    Child = strtok (NULL,",");


    n=0;
    if (strcmp (Manager, "XmForm") == 0)
        Parent = XmCreateForm (Shell1, "Parent",args,n);
    else
    if (strcmp (Manager, "XmFrame") == 0)
        Parent = XmCreateFrame (Shell1, "Parent",args,n);
    else
    if (strcmp (Manager, "XmBulletinBoard") == 0)
        Parent = XmCreateBulletinBoard(Shell1, "Parent",args,n);
    else
    if (strcmp (Manager, "XmRowColumn") == 0)
        Parent = XmCreateRowColumn(Shell1, "Parent",args,n);
    else
      {
	printf ("Not a manager!\n");
	exit(0);
      }


    XtManageChild (Parent);

   /* create array of compund strings. */

    for (i = 0; i < NUM_ITEMS; i++)
	{
	 TempLabelString = XmStringCreate (months[i], 
				    XmFONTLIST_DEFAULT_TAG);
	 CSmonths[i] = TempLabelString;

}
    n=0;
    XtSetArg (args[n], XmNdefaultArrowSensitivity, XmARROWS_SENSITIVE); n++;
    XtSetArg (args[n], XmNarrowLayout, XmARROWS_FLAT_BEGINNING); n++;
    XtSetArg (args[n], XmNarrowSize, 25); n++;
    XtSetArg (args[n], XmNarrowOrientation, XmHORIZONTAL); n++;
    XtSetArg (args[n], XmNinitialDelay,0); n++;
    XtSetArg (args[n], XmNrepeatDelay,0); n++;
    SpinBox1 = XmCreateSpinBox (Parent, "SpinBox1", args, n);
    XtManageChild (SpinBox1);
    
    XtAddCallback (SpinBox1, XmNvalueChangedCallback, ValueChangedCallback,
		   (XtPointer)NULL);
    XtAddCallback (SpinBox1, XmNmodifyVerifyCallback, ModifyVerifyCallback,
		   (XtPointer)NULL);

    if (strcmp (Child, "XmText") == 0)

        {
	    n=0;
	    XtSetArg (args[n], XmNspinBoxChildType, XmSTRING); n++;
            XtSetArg (args[n], XmNposition,1); n++;
            XtSetArg (args[n], XmNnumValues, NUM_ITEMS); n++;
	    XtSetArg (args[n], XmNvalues, CSmonths); n++;
	    FocusChild[0] = XmCreateText(SpinBox1, "Focus1", args, n);
	    XtManageChild (FocusChild[0]);

	    n=0;
	    XtSetArg (args[n], XmNorientation, XmVERTICAL); n++;
	    NonFocusChild1 = XmCreateSeparator (SpinBox1, "NonFocus1", args, n);
	    XtManageChild (NonFocusChild1);


            n=0;
	    XtSetArg (args[n], XmNspinBoxChildType, XmNUMERIC); n++;
	    XtSetArg (args[n], XmNposition, 1); n++;
	    XtSetArg (args[n], XmNminimumValue, 1); n++;
	    XtSetArg (args[n], XmNmaximumValue, 31); n++;
	    XtSetArg (args[n], XmNincrementValue, 1); n++;
            FocusChild[1] = XmCreateText (SpinBox1, "Focus2", args, n);
            XtManageChild (FocusChild[1]);

        }


    if (strcmp (Child, "XmTextField") == 0)

        {
	    n=0;
	    XtSetArg (args[n], XmNspinBoxChildType, XmSTRING); n++;
            XtSetArg (args[n], XmNposition,1); n++;
            XtSetArg (args[n], XmNpositionType,XmPOSITION_INDEX); n++;
            XtSetArg (args[n], XmNnumValues, NUM_ITEMS); n++;
	    XtSetArg (args[n], XmNvalues, CSmonths); n++;
	    FocusChild[0] = XmCreateTextField(SpinBox1, "Focus1", args, n);
	    XtManageChild (FocusChild[0]);


            n=0;
	    XtSetArg (args[n], XmNspinBoxChildType, XmNUMERIC); n++;
	    XtSetArg (args[n], XmNposition, 1); n++;
            XtSetArg (args[n], XmNpositionType,XmPOSITION_INDEX); n++;
	    XtSetArg (args[n], XmNminimumValue, 1); n++;
	    XtSetArg (args[n], XmNmaximumValue, 31); n++;
	    XtSetArg (args[n], XmNincrementValue, 1); n++;
	    FocusChild[1] = XmCreateTextField (SpinBox1, "Focus2", args, n);
	    XtManageChild (FocusChild[1]);


        }


        XtRealizeWidget (Shell1);


        CommonPause();	/* TP 0 */
       
        n=0;
        XtSetArg (args[n], XmNminimumValue, 3); n++;
        XtSetArg (args[n], XmNmaximumValue, 15); n++;
        XtSetArg (args[n], XmNincrementValue, 3); n++;
        XtSetValues (FocusChild[1], args, n);

        CommonPause();	/* TP 1 */

        n=0;
	XtSetArg (args[n], XmNinitialDelay, 10000); n++;
	XtSetArg (args[n], XmNrepeatDelay,10000); n++;
	XtSetValues (SpinBox1, args, n);

        CommonPause();	/* TP 2 */

	n=0;
	XtSetArg (args[n], XmNarrowLayout, XmARROWS_FLAT_END); n++;
	XtSetArg (args[n], XmNarrowSize, 50); n++;
	XtSetArg (args[n], XmNinitialDelay, 0); n++;
	XtSetArg (args[n], XmNrepeatDelay,10000); n++;
	XtSetValues (SpinBox1, args, n);

	
    	n=0;
	XtSetArg (args[n], XmNarrowSensitivity, 
                           XmARROWS_DECREMENT_SENSITIVE); n++;
        XtSetArg (args[n], XmNposition,5); n++;
	XtSetValues (FocusChild[0], args, n);
	
 
        n=0;
	XtSetArg (args[n], XmNarrowSensitivity, 
                           XmARROWS_INCREMENT_SENSITIVE); n++;
        XtSetArg (args[n], XmNposition, 6); n++;
	XtSetArg (args[n], XmNminimumValue, 1); n++;
	XtSetArg (args[n], XmNmaximumValue, 31); n++;
	XtSetArg (args[n], XmNincrementValue, 1); n++;
	XtSetArg (args[n], XmNdecimalPoints, 1); n++;
        XtSetValues (FocusChild[1], args, n);


        CommonPause();	/* TP 3 */


    	n=0;
	XtSetArg (args[n], XmNarrowSensitivity, 
                           XmARROWS_DECREMENT_SENSITIVE); n++;
        XtSetValues (FocusChild[0], args, n);
	
 
        n=0;
	XtSetArg (args[n], XmNarrowSensitivity, 
                           XmARROWS_INCREMENT_SENSITIVE); n++;
	XtSetArg (args[n], XmNincrementValue, 10); n++;
	XtSetArg (args[n], XmNposition, 0); n++;
        XtSetValues (FocusChild[1], args, n);


	n=0;
	XtSetArg (args[n], XmNarrowLayout, XmARROWS_SPLIT); n++;
	XtSetArg (args[n], XmNinitialDelay, 0); n++;
	XtSetArg (args[n], XmNrepeatDelay,0); n++;
	XtSetValues (SpinBox1, args, n);

        CommonPause();	/* TP 4 */


    	n=0;
	XtSetArg (args[n], XmNarrowSensitivity, 
                           XmARROWS_DEFAULT_SENSITIVITY); n++;
        XtSetValues (FocusChild[0], args, n);
	
 
        n=0;
	XtSetArg (args[n], XmNarrowSensitivity, 
                           XmARROWS_DEFAULT_SENSITIVITY); n++;
        XtSetValues (FocusChild[1], args, n);

    	n=0;
	XtSetArg (args[n], XmNdefaultArrowSensitivity, XmARROWS_INSENSITIVE); n++;
	XtSetArg (args[n], XmNarrowLayout, XmARROWS_FLAT_BEGINNING); n++;
	XtSetValues (SpinBox1, args, n);
	
    	
        CommonPause();	/* TP 5 */

        n=0;
        XtSetArg (args[n], XmNarrowLayout, XmARROWS_FLAT_END); n++;
        XtSetValues (SpinBox1, args, n);

        CommonPause();	/* TP 6 */
        CommonPause();	/* TP 7 */

	n=0;
	XtSetArg ( args[n], XmNarrowLayout, XmARROWS_BEGINNING); n++;
        XtSetValues (SpinBox1, args, n);
	CommonPause();  /* TP 8, Motif/CDE Convergence */

	n=0;
	XtSetArg ( args[n], XmNarrowLayout, XmARROWS_END); n++;
        XtSetValues (SpinBox1, args, n);
	CommonPause();  /* TP 9, Motif/CDE Convergence */

	n=0;
	XtSetArg ( args[n], XmNarrowOrientation, XmARROWS_VERTICAL); n++;
        XtSetValues (SpinBox1, args, n);
	CommonPause();  /* TP 10, Motif/CDE Convergence */

	n=0;
	XtSetArg ( args[n], XmNarrowLayout, XmARROWS_BEGINNING); n++;
	XtSetArg ( args[n], XmNarrowOrientation, XmARROWS_DECREMENT_SENSITIVE);
		n++;
        XtSetValues (SpinBox1, args, n);
	CommonPause();  /* TP 11, Motif/CDE Convergence */

	n=0;
	XtSetArg ( args[n], XmNarrowLayout, XmARROWS_END); n++;
	XtSetArg ( args[n], XmNarrowOrientation, XmARROWS_INCREMENT_SENSITIVE);
		n++;
        XtSetValues (SpinBox1, args, n);
	CommonPause();  /* TP 12, Motif/CDE Convergence */

	n=0;
	XtSetArg ( args[n], XmNarrowLayout, XmARROWS_FLAT_BEGINNING); n++;
	XtSetArg ( args[n], XmNdefaultArrowSensitivity, XmARROWS_SENSITIVE); n++;
        XtSetValues (SpinBox1, args, n);
        n=0;
        XtSetArg ( args[n], XmNarrowSensitivity, XmARROWS_DECREMENT_SENSITIVE);
		n++;
	XtSetValues( FocusChild[0], args, n );
        n=0;
        XtSetArg ( args[n], XmNarrowSensitivity, XmARROWS_INCREMENT_SENSITIVE);
		n++;
	XtSetValues( FocusChild[1], args, n );
	CommonPause();  /* TP 13, Motif/CDE Convergence */

	CommonPause();  /* TP 14, Motif/CDE Convergence */

	CommonPause();  /* TP 15, Motif/CDE Convergence */

	n=0;
	XtSetArg ( args[n], XmNarrowLayout, XmARROWS_FLAT_END); n++;
        XtSetValues (SpinBox1, args, n);
	CommonPause();  /* TP 16, Motif/CDE Convergence */

	CommonPause();  /* TP 17, Motif/CDE Convergence */

	CommonPause();  /* TP 18, Motif/CDE Convergence */

	n=0;
	XtSetArg ( args[n], XmNarrowLayout, XmARROWS_SPLIT); n++;
        XtSetValues (SpinBox1, args, n);
	CommonPause();  /* TP 19, Motif/CDE Convergence */

	CommonPause();  /* TP 20, Motif/CDE Convergence */

	CommonPause();  /* TP 21, Motif/CDE Convergence */

    XtAppMainLoop (app_context);

  }



void
ValueChangedCallback(Widget w,XtPointer client_data,XtPointer call_data)
{

  int reason = ((XmSpinBoxCallbackStruct *) call_data)->reason;

  printf ("valueChanged callback invoked\n");
  
  switch (reason)
    {
    case XmCR_OK:
      printf ("Reason is XmCR_OK\n\n");
      break;

    case XmCR_SPIN_NEXT:
      printf ("Reason is XmCR_SPIN_NEXT\n\n");
      break;

    case XmCR_SPIN_PRIOR:
      printf ("Reason is XmCR_SPIN_PRIOR\n\n");
      break;

    default:
      break;
    }

}




void
ModifyVerifyCallback(Widget w,XtPointer client_data,XtPointer call_data)
{

  int reason = ((XmSpinBoxCallbackStruct *)call_data)->reason;

  printf ("modifyVerify callback invoked\n");
  
  switch (reason)
    {
    case XmCR_OK:
      printf ("Reason is XmCR_OK\n\n");
      break;

    case XmCR_SPIN_NEXT:
      printf ("Reason is XmCR_SPIN_NEXT\n\n");
      break;

    case XmCR_SPIN_PRIOR:
      printf ("Reason is XmCR_SPIN_PRIOR\n\n");
      break;

    default:
      break;
    }

}





