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
static char rcsid[] = "$XConsortium: Scale7.c /main/8 1995/07/13 19:12:34 drk $"
#endif
#endif

/************************************************************************
 *
 *  Create a scale with 10 labels in it.  Reverse the processing
 *  direction set the border width on the labels.  Realize the scale
 *  after inserting the children.
 *
 ************************************************************************/

#include <testlib.h>

static int first_drag = 1;

static void 
ScaleValueChangedCB (Widget sw, XtPointer client_data, XtPointer data)
{
   XmScaleCallbackStruct *call_data = (XmScaleCallbackStruct *) data ;
   
   printf ("Test valueChangedCallback called\n");
   printf ("    Value = %d\n", call_data->value);

   if (first_drag == 0)
       first_drag = 1;
}

static void 
ScaleDragCB (Widget sw, XtPointer client_data, XtPointer data)
{
   if (first_drag == 1) {
       printf ("\nTest dragCallback called\n\n");
       first_drag = 0;
   }
}


void 
main (unsigned int argc, char **argv)
{
   Widget Scale1;
   Arg args[MAX_ARGS];
   register int	n;
   Widget label;
   Widget label1, label2, label3, label4, label5, label6, label7,
          label8, label9, label10, label11;

   /*  initialize toolkit  */
    CommonTestInit(argc, argv);
    
   /*  Set up arglist and create the scale  */

   n = 0;
   XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_BOTTOM);		n++;
   XtSetArg (args[n], XmNshowValue, True); n++;
   Scale1 = XmCreateScale(Shell1, "Scale1", args, n); 
   XtManageChild (Scale1);
   XtAddCallback (Scale1, XmNvalueChangedCallback, CommonGenericCB, NULL);
   XtAddCallback (Scale1, XmNvalueChangedCallback, ScaleValueChangedCB, NULL);
   XtAddCallback (Scale1, XmNdragCallback,         ScaleDragCB, NULL);
   XtAddCallback (Scale1, XmNhelpCallback,         CommonGenericCB, NULL);

   /*  realize widgets  */

   n = 0;
   XtSetArg (args[n], XmNborderWidth, 1);	n++;
   label1 = XmCreateLabel (Scale1, "0", args, n);
   XtManageChild (label1);
   label2 = XmCreateLabel (Scale1, "10", args, n);
   XtManageChild (label2);
   label3 = XmCreateLabel (Scale1, "20", args, n);
   XtManageChild (label3);
   label4 = XmCreateLabel (Scale1, "30", args, n);
   XtManageChild (label4);
   label5 = XmCreateLabel (Scale1, "40", args, n);
   XtManageChild (label5);
   label6 = XmCreateLabel (Scale1, "50", args, n);
   XtManageChild (label6);
   label7 = XmCreateLabel (Scale1, "60", args, n);
   XtManageChild (label7);
   label8 = XmCreateLabel (Scale1, "70", args, n);
   XtManageChild (label8);
   label9 = XmCreateLabel (Scale1, "80", args, n);
   XtManageChild (label9);
   label10 = XmCreateLabel (Scale1, "90", args, n);
   XtManageChild (label10);
   label11 = XmCreateLabel (Scale1, "100", args, n);
   XtManageChild (label11);


   /*  process events  */
   XtRealizeWidget (Shell1);

   CommonPause();

   /* Change orientation to HORIZONTAL and repeat testing */

   n = 0;
   XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg (args[n], XmNwidth, 250); n++;
   XtSetArg (args[n], XmNscaleHeight, 15); n++;
   XtSetValues (Scale1, args, n);

   CommonPause();

   XtAppMainLoop(app_context);
}
