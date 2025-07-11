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
static char rcsid[] = "$XConsortium: ResPartOffM1.c /main/6 1995/07/13 18:37:26 drk $"
#endif
#endif

/*
 *  Create a foo widget in a form.  Foo uses XmResolvePartOffsets.
 */

#include <signal.h>
#include <X11/StringDefs.h>
#include <Xm/ScrollBarP.h>

#include <testlib.h>

/*  Foo class structure  */

typedef struct _FooClassPart
{
   int foo;	/*  No new fields needed  */
} FooClassPart;


/*  Full class record declaration for CheckBox class  */

typedef struct _FooClassRec
{
   CoreClassPart        core_class;
   XmPrimitiveClassPart primitive_class;
   XmScrollBarClassPart scrollbar_class;
   FooClassPart         foo_class;
} FooClassRec;

extern FooClassRec fooClassRec;


/*  The Foo resources  */

#define XmNbar		"bar"
#define XmCBar		"Bar"


/*  The Foo instance record  */

typedef struct _FooPart
{
   int bar;
} FooPart;


/*  Full instance record declaration  */

typedef struct _FooRec
{
   CorePart	   core;
   XmPrimitivePart primitive;
   XmScrollBarPart scrollBar;
   FooPart         foo;
} FooRec;


extern WidgetClass fooWidgetClass;

typedef struct _FooClassRec * FooWidgetClass;
typedef struct _FooRec      * FooWidget;



/*  Resource list for Foo  */

#define FooIndex (XmScrollBarIndex + 1)

static XmPartResource resources[] = 
{
   {
      XmNbar, XmCBar, XmRInt, sizeof (int),
      XmPartOffset(Foo, bar), XmRImmediate, (caddr_t) 0
   },
};

/*  Class functions routine definitions  */

static void    ClassInitialize();

static XmOffsetPtr offset;	/* ptr to the offset record  */

/*  The Foo class record definition  */

FooClassRec fooClassRec =
{
   {
      (WidgetClass) &xmScrollBarClassRec, /* superclass	         */
      "Foo",                            /* class_name	         */
      sizeof(FooPart),                  /* widget_size	         */
      ClassInitialize,	                /* class_initialize      */
      NULL,                             /* class_part_initialize */
      FALSE,                            /* class_inited          */
      NULL,                             /* initialize	         */
      NULL,                             /* initialize_hook       */
      XtInheritRealize,                 /* realize	         */	
      NULL,                             /* actions               */	
      0,                                /* num_actions	         */	
      (XtResource *) resources,         /* resources	         */	
      XtNumber (resources),             /* num_resources         */	
      NULLQUARK,                        /* xrm_class	         */	
      TRUE,                             /* compress_motion       */	
      TRUE,                             /* compress_exposure     */	
      TRUE,                             /* compress_enterleave   */
      FALSE,                            /* visible_interest      */	
      NULL,                             /* destroy               */	
      XtInheritResize,                  /* resize                */	
      XtInheritExpose,                  /* expose                */	
      NULL,                             /* set_values    	 */	
      NULL,                             /* set_values_hook       */
      XtInheritSetValuesAlmost,         /* set_values_almost     */
      NULL,				/* get_values_hook       */
      XtInheritAcceptFocus,             /* accept_focus	         */	
      XtVersionDontCheck,               /* version               */
      NULL,                             /* callback private      */
      XtInheritTranslations,            /* tm_table              */
      NULL,                             /* query_geometry        */
      NULL,                             /* display_accelerator   */
      NULL,                             /* extension             */
   },

   {
      XmInheritWidgetProc,		/* border_highlight   */
      XmInheritWidgetProc,		/* border_unhighlight */
      XtInheritTranslations,	        /* translations       */
      NULL,	    		     	/* arm_and_activate   */
      NULL,            			/* get resources      */
      0,                		/* num get_resources  */
      NULL,				/* extension          */
   },

   {
      NULL,				/* extension          */
   },

   {
      0,				/* extension	      */
   }
};



WidgetClass fooWidgetClass = (WidgetClass) &fooClassRec;


/************************************************************************
 *
 *  ClassInitialize
 *     Initialize the getvalue resources.
 *
 ************************************************************************/

static void ClassInitialize ()

{
   XmResolvePartOffsets (fooWidgetClass, &offset);
}


/*  Forward declarations  */

extern void FormCallback();


/*  main  */

void main (argc, argv)
unsigned int argc;
char **argv;

{
   Widget form;
   Widget scrollbar;
   Arg args[10];
   int n;



   /*  initialize toolkit  */
    CommonTestInit(argc, argv);
    

   /*  The form creation.  */

   n = 0;
   XtSetArg (args[n], XmNhorizontalSpacing, 4);		n++;
   XtSetArg (args[n], XmNverticalSpacing, 4);		n++;
   XtSetArg (args[n], XmNwidth, 50);			n++;
   XtSetArg (args[n], XmNheight, 500);			n++;
   form = XmCreateForm (Shell1, "form", args, n);
   XtManageChild (form);
   XtAddCallback (form, XmNhelpCallback, FormCallback, NULL);

   XtRealizeWidget (Shell1);

   /*  Arguments to the frame.  */

   n = 0;
   XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);		n++;
   XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);	n++;
   XtSetArg (args[n], XmNwidth, 50);				n++;
   XtSetArg (args[n], XmNheight, 500);				n++;
   scrollbar = XtCreateManagedWidget("foo", fooWidgetClass, form, args, n);

   CommonPause();

   XtAppMainLoop(app_context);
}



void myQuit()
{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
}



void FormCallback (fw, closure, call_data)
XmFormWidget fw;
caddr_t closure;
caddr_t call_data;

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
         printf ("Hit the default, incorrect reason sent!!\n");
      break;
   }
}












