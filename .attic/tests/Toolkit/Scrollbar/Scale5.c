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
static char rcsid[] = "$XConsortium: Scale5.c /main/7 1995/07/13 19:11:58 drk $"
#endif
#endif

/************************************************************************
 *
 *  Create a scale with one label in it.  Reverse the processing
 *  direction set the border width on the labels.
 *
 ************************************************************************/

#include <testlib.h>

void ScaleCallback (sw, client_data, data)
Widget sw;
XtPointer client_data;
XtPointer data ;
{
XmScaleCallbackStruct *call_data = (XmScaleCallbackStruct *) data;
   printf ("	Value = %d\n", call_data->value);
}


void main (argc,argv)
unsigned int argc;
char **argv;

{
   Widget Scale1;
   Arg args[MAX_ARGS];
   register int	n;
   Widget label1, label2;


   /*  initialize toolkit  */
    CommonTestInit(argc, argv);

   n = 0;
   XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_BOTTOM);  n++;
   Scale1 = XmCreateScale(Shell1, "Scale1", args, n); 

   XtManageChild (Scale1);
   XtAddCallback (Scale1, XmNvalueChangedCallback, CommonGenericCB, NULL);
   XtAddCallback (Scale1, XmNvalueChangedCallback, ScaleCallback, NULL);
   XtAddCallback (Scale1, XmNdragCallback,         CommonGenericCB, NULL);
   XtAddCallback (Scale1, XmNhelpCallback,         CommonGenericCB, NULL);
   XtAddCallback (Scale1, XmNhelpCallback,         ScaleCallback, NULL);

   /*  realize widgets  */

   XtRealizeWidget (Shell1);

   n = 0;
   XtSetArg (args[n], XmNborderWidth, 1);  n++;
   label1 = XmCreateLabel (Scale1, "0", args, n);
   label2 = XmCreateLabel (Scale1, "100", args, n);

   XtManageChild (label1);
   XtManageChild (label2);

   CommonPause();

   /* Change orientation to HORIZONTAL and continue testing */

   n = 0;
   XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg (args[n], XmNprocessingDirection, XmMAX_ON_RIGHT); n++;
   XtSetArg (args[n], XmNwidth, 250); n++;
   XtSetArg (args[n], XmNscaleHeight, 15); n++;
   XtSetValues (Scale1, args, n);

   CommonPause();

   /*  process events  */

   XtAppMainLoop(app_context);
}
