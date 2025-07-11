/* $XConsortium: SpinBox2.c /main/6 1996/04/30 13:56:08 schuldy $ */
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

/*
 *  Global variables.
 */
Widget Parent, SpinBox1, TextF1;

void
ValueChanged(Widget w,XtPointer client_data,XtPointer call_data)
{

  int reason = ((XmSpinBoxCallbackStruct *) call_data)->reason;

  printf ("XmNvalueChangedCallback invoked ");
  
  switch (reason)
    {
    case XmCR_OK:
      printf ("reason = XmCR_OK\n\n");
      break;

    case XmCR_SPIN_NEXT:
      printf ("reason = XmCR_SPIN_NEXT\n");
      break;

    case XmCR_SPIN_PRIOR:
      printf ("reason = XmCR_SPIN_PRIOR\n");
      break;

    case XmCR_SPIN_FIRST:
      printf ("reason = XmCR_SPIN_FIRST\n");
      break;

    case XmCR_SPIN_LAST:
      printf ("reason = XmCR_SPIN_LAST\n");
      break;

    default:
      break;
   }

}

void
CrossedBound(Widget w,XtPointer client_data,XtPointer call_data)
{
  XmSpinBoxCallbackStruct *cb_struct = (XmSpinBoxCallbackStruct *)call_data;
  static  int  doit = True;

  printf("crossed_boundary = ");
  if ( cb_struct->crossed_boundary )
  {
     printf(" True\n");
     doit = (doit) ? False : True;
     cb_struct->doit = doit;
  }
  else
     printf(" False\n");
}

void
SetPosition(Widget w, XtPointer client_data, XtPointer call_data)
{
  XmSpinBoxCallbackStruct *cb_struct = (XmSpinBoxCallbackStruct *)call_data;
  Arg   args[5];
  int   n, min, max, incr, max_pos;

    n = 0;
    XtSetArg (args[n], XmNminimumValue, &min); n++;
    XtSetArg (args[n], XmNmaximumValue, &max); n++;
    XtSetArg (args[n], XmNincrementValue, &incr); n++;
    XtGetValues( TextF1, args, n );

    max_pos = ( incr > 0 ) ? ( max - min ) / incr : ( max - min ); 

    switch( cb_struct->reason )
    {
       case XmCR_SPIN_NEXT:
       case XmCR_SPIN_LAST:
            cb_struct->position = max_pos - 1;
            break;

       case XmCR_SPIN_PRIOR:
       case XmCR_SPIN_FIRST:
            cb_struct->position = max_pos;
            break;
    }
    cb_struct->doit = True;
}

void
ModifyVerify(Widget w,XtPointer client_data,XtPointer call_data)
{
  XmSpinBoxCallbackStruct *cb_struct = (XmSpinBoxCallbackStruct *)call_data;

  printf ("XmNmodifyVerifyCallback invoked, ");

  switch ( cb_struct->reason )
  {
    case XmCR_OK:
      printf ("reason = XmCR_OK\n");
      break;

    case XmCR_SPIN_NEXT:
      printf ("reason = XmCR_SPIN_NEXT\n");
      break;

    case XmCR_SPIN_PRIOR:
      printf ("reason = XmCR_SPIN_PRIOR\n");
      break;

    case XmCR_SPIN_FIRST:
      printf ("reason = XmCR_SPIN_FIRST\n");
      break;

    case XmCR_SPIN_LAST:
      printf ("reason = XmCR_SPIN_LAST\n");
      break;

    default:
      break;
    }
}

void
main (int argc, char **argv)
{
    Arg args[10];
    int n;

    CommonTestInit (argc, argv);

    n=0;
    Parent = XmCreateFrame (Shell1, "Frame", args, n);
    XtManageChild (Parent);

    n=0;
    XtSetArg (args[n], XmNarrowLayout, XmARROWS_FLAT_END); n++;
    XtSetArg (args[n], XmNrepeatDelay, 0); n++;
    XtSetArg (args[n], XmNarrowOrientation, XmHORIZONTAL); n++;
    SpinBox1 = XmCreateSpinBox (Parent, "SpinBox1", args, n);
    XtManageChild (SpinBox1);
    XtAddCallback (SpinBox1, XmNvalueChangedCallback, ValueChanged,
    (XtPointer)NULL);
    XtAddCallback (SpinBox1, XmNmodifyVerifyCallback, ModifyVerify,
    (XtPointer)0);


    n=0;
    XtSetArg (args[n], XmNspinBoxChildType, XmNUMERIC); n++;
    XtSetArg (args[n], XmNminimumValue, 1); n++;
    XtSetArg (args[n], XmNmaximumValue, 10); n++;
    XtSetArg (args[n], XmNincrementValue, 3); n++;
    XtSetArg (args[n], XmNposition, 2); n++;
    XtSetArg (args[n], XmNpositionType, XmPOSITION_INDEX); n++;
    TextF1 = XmCreateTextField (SpinBox1, "TextF1", args,n);
    XtManageChild (TextF1);

    XtRealizeWidget (Shell1);

    CommonPause();
    CommonPause();
   
    XtAddCallback( SpinBox1, XmNmodifyVerifyCallback, CrossedBound,
                   (XtPointer)0 );
    CommonPause();
    XtRemoveCallback( SpinBox1, XmNmodifyVerifyCallback, CrossedBound,
                      (XtPointer)0 );
    XtAddCallback( SpinBox1, XmNmodifyVerifyCallback, SetPosition,
                   (XtPointer)0 );
    CommonPause();
    XtRemoveCallback( SpinBox1, XmNmodifyVerifyCallback, SetPosition,
                      (XtPointer)0 );
    CommonPause();
    CommonPause();

    XtAppMainLoop (app_context);

    
}


