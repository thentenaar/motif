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
static char rcsid[] = "$XConsortium: Scale9.c /main/6 1995/07/13 19:13:08 drk $"
#endif
#endif

/************************************************************************
 *
 *  Create a scale with three labels in it.
 *
 ************************************************************************/
#include <testlib.h>

/*  Forward declarations  */

static void ButtonCallback1();
static void ButtonCallback2();
static void ButtonCallback3();

Widget scale;
int max;
int min;


void main (argc,argv)
unsigned int argc;
char **argv;
{
   Arg args[MAX_ARGS];
   register int	n;
   Widget label1, label2, label3;
   Widget b;


   /*  initialize toolkit  */
    CommonTestInit(argc, argv);

   /*  Set up arglist and create the scale  */

   n = 0;
   scale = XmCreateScale(Shell1, "scaleWidget", args, n); 
   XtManageChild (scale);

   /* Get Maximum value of slider */

   n = 0;
   XtSetArg (args[n], XmNmaximum, &max); n++;
   XtSetArg (args[n], XmNminimum, &min); n++;
   XtGetValues (scale, args, n);

   /*  Create the push button labels for the scale  */

   n = 0;
   XtSetArg (args[n], XmNborderWidth, 1);	n++;
   label1 = XmCreatePushButton (scale, "Push_5_More", args, 0);
   XtManageChild (label1);

   XtAddCallback (label1, XmNactivateCallback, ButtonCallback1, NULL);

   n = 0;
   XtSetArg (args[n], XmNborderWidth, 1);	n++;
   label2 = XmCreatePushButton (scale, "Push_Max", args, 0);
   XtManageChild (label2);

   XtAddCallback (label2, XmNactivateCallback, ButtonCallback2, NULL);

   n = 0;
   XtSetArg (args[n], XmNborderWidth, 1);	n++;
   label3 = XmCreatePushButton (scale, "Push_Min", args, 0);
   XtManageChild (label3);

   XtAddCallback (label3, XmNactivateCallback, ButtonCallback3, NULL);

   /*  realize widgets  */

   XtRealizeWidget (Shell1);

   CommonPause();

   /* Change orientation to HORIZONTAL and repeat testing */

   n = 0;
   XtSetArg (args[n], XmNorientation, XmHORIZONTAL); n++;
   XtSetArg (args[n], XmNscaleWidth, 250); n++;
   XtSetArg (args[n], XmNscaleHeight, 15); n++;
   XtSetValues (scale, args, n);

   CommonPause();

   /*  process events  */

   XtAppMainLoop(app_context);
}
                                   

static void ButtonCallback1 (sw, closure, call_data)
Widget sw;
caddr_t closure;
caddr_t call_data;
{
   int slider;

   XmScaleGetValue (scale, &slider);

   printf ("new slider = %d\n", slider + 5);

/* This used to be XmScaleSetSlider() */
   XmScaleSetValue (scale, slider + 5);
}


static void ButtonCallback2 (sw, closure, call_data)
Widget sw;
caddr_t closure;
caddr_t call_data;
{
/* This used to be XmScaleSetSlider() */

   XmScaleSetValue (scale, max);
}


static void ButtonCallback3 (sw, closure, call_data)
Widget sw;
caddr_t closure;
caddr_t call_data;
{
/* This used to be XmScaleSetSlider() */

   XmScaleSetValue (scale, min);
}
