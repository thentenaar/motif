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
static char rcsid[] = "$XConsortium: Browse.c /main/9 1995/07/14 10:45:31 drk $"
#endif
#endif
 
/*******************************************
 * Browse.c: Source code for Browse Widget *
 *******************************************/
 
/*
 * Include files
 */
 
#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#include <X11/Composite.h>
 
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
 
#include "BrowseP.h"
 
#include <stdio.h>

extern void _XmBulletinBoardCancel();
extern void _XmBulletinBoardReturn();
extern void _XmGadgetActivate();
extern void _XmGadgetArm();
extern void _XmManagerEnter();
extern void _XmManagerFocusIn();
extern void _XmManagerHelp();
 
/*
 * Callback Routines
 */
 
static void     ChangeImage();
static void     ButtonCallback();
 
/*
 * Convenience constants and macros
 */
 
#define PREV    0
#define NEXT    1
 
/*
 * Methods
 */
 
static void     Initialize();
static Boolean  SetValues();
 
/*
 * Convenience routines
 */
 
void    CreateChildren();
 
/*
 * Action Table
 */
 
static XtActionsRec actionsList[] =
{
   {"Enter",			(XtActionProc)	_XmManagerEnter},
   {"FocusIn",			(XtActionProc)	_XmManagerFocusIn},
   {"Arm",			(XtActionProc)	_XmGadgetArm},
   {"Activate",			(XtActionProc)	_XmGadgetActivate},
   {"Help",			(XtActionProc)	_XmManagerHelp},
   {"Return",			(XtActionProc)	_XmBulletinBoardReturn},
   {"BulletinBoardReturn",	(XtActionProc)	_XmBulletinBoardReturn},
   {"BulletinBoardCancel",	(XtActionProc)	_XmBulletinBoardCancel},
};
 
/*
 * Resource List
 */
 
static XtResource       resources[] =
{
    {
        XmNimages,
        XmCImages,
        XmRPointer,
        sizeof(caddr_t),
        XtOffset(BrowseWidget, browse.images),
        XmRString,
        (caddr_t) NULL,
    },
 
    {
        XmNnumImages,
        XmCNumImages,
        XmRInt,
        sizeof(int),
        XtOffset(BrowseWidget, browse.num_images),
        XmRString,
        "0",
    },
 
    {
        XmNimageBackground,
        XmCBackground,
        XmRPixel,
        sizeof(Pixel),
        XtOffset(BrowseWidget, browse.image_background),
        XmRString,
        "White",
    },
 
    {
        XmNimageForeground,
        XmCForeground,
        XmRPixel,
        sizeof(Pixel),
        XtOffset(BrowseWidget, browse.image_foreground),
        XmRString,
        "Black",
    },
 
    {
        XmNbuttonBackground,
        XmCBackground,
        XmRPixel,
        sizeof(Pixel),
        XtOffset(BrowseWidget, browse.button_background),
        XmRString,
        "White",
    },
 
    {
        XmNbuttonForeground,
        XmCForeground,
        XmRPixel,
        sizeof(Pixel),
        XtOffset(BrowseWidget, browse.button_foreground),
        XmRString,
        "Black",
    },
 
    {
        XmNquitCallback,
        XmCCallback,
        XmRCallback,
        sizeof(XtCallbackList),
        XtOffset (BrowseWidget, browse.quit_callback),
        XmRPointer,
        (caddr_t) NULL,
    },
};
 
/*
 * Initialize Class Record
 */
 
BrowseClassRec  browseClassRec =
{
 
/*
 * CoreClassPart
 */
 
    {
        (WidgetClass) &xmFormClassRec,  /* superclass                   */
        "Browse",                       /* class_name                   */
        sizeof(BrowseRec),              /* widget_size                  */
        NULL,                           /* class_initialize             */
        NULL,                           /* class_part_initialize        */
        FALSE,                          /* class_inited                 */
        Initialize,                     /* initialize                   */
        NULL,                           /* initialize_hook              */
        XtInheritRealize,               /* realize                      */
        actionsList,                    /* actions                      */
        XtNumber(actionsList),          /* num_actions                  */
        resources,                      /* resources                    */
        XtNumber(resources),            /* num_resources                */
        NULLQUARK,                      /* xrm_class                    */
        FALSE,                          /* compress_motion              */
        TRUE,                           /* compress_exposure            */
        FALSE,                          /* compress_enterleave          */
        FALSE,                          /* visible_interest             */
        NULL,                           /* destroy                      */
        XtInheritResize,                /* resize                       */
        XtInheritExpose,                /* expose                       */
        SetValues,                      /* set_values                   */
        NULL,                           /* set_values_hook              */
        XtInheritSetValuesAlmost,       /* set_values_almost            */
        NULL,                           /* get_values_hook              */
        NULL,                           /* accept_focus                 */
        XtVersion,                      /* version                      */
        NULL,                           /* callback private             */
        XtInheritTranslations,          /* tm_table                     */
        NULL,                           /* query_geometry               */
        NULL,                           /* display_accelerator          */
        NULL,                           /* extension                    */
    },
 
/*
 * CompositeClassPart
 */
 
    {
        XtInheritGeometryManager,       /* geometry_manager             */
        XtInheritChangeManaged,         /* change_managed               */
        XtInheritInsertChild,           /* insert_child                 */
        XtInheritDeleteChild,           /* delete_child                 */
        NULL,                           /* extension                    */
    },
 
/*
 * ConstraintClassPart
 */
 
    {
        NULL,                           /* constraint resource          */
        0,                              /* number of constraints        */
        sizeof(XmFormConstraintRec),    /* size of constraint           */
        NULL,                           /* initialization               */
        NULL,                           /* destroy proc                 */
        NULL,                           /* set_values proc              */
        NULL,                           /* extension                    */
    },
 
/*
 * ManagerClassPart
 */
 
    {
	XtInheritTranslations,          /* default translations         */
	NULL,                           /* syn_resources                */
        0,                              /* num_syn_resources            */
        NULL,                           /* syn_cont_resources           */
        0,                              /* num_syn_cont_resources       */
        NULL,                           /* extension                    */
    },
 
/*
 * BulletinBoardClassPart
 */
 
   {
       FALSE,                           /* always_install_accelerators  */
       NULL,                            /* geo_matrix_create            */
       NULL				/* extension                    */
   },
 
/*
 * FormClassPart
 */
 
    {
        NULL,                           /* extension                    */
    },
 
/*
 * BrowseClassPart
 */
 
    {
        0,                              /* extension                    */
    }
};
 
WidgetClass browseWidgetClass = (WidgetClass) &browseClassRec;
 
 
/*********************************
 ***** Browse Widget Methods *****
 *********************************/
 
/**************
 * Initialize *
 **************/
 
static void Initialize(request, new)
     BrowseWidget       request,
                        new;
{
 
/*
 * Window size must be > 0.  This is not done by Core Initialize() Method
 */
 
    if(request->core.width == 0)
    {
        new->core.width = 5;
    }
 
    if(request->core.height == 0)
    {
        new->core.height = 5;
    }
 
/*
 * Set default spacing
 */
 
    new->form.horizontal_spacing = 10;
    new->form.vertical_spacing = 10;
 
/*
 * Start with first image
 */
 
    new->browse.current_image = -1;
 
/*
 * Create child widgets
 */
 
    CreateChildren(new);
}
 
/*************
 * SetValues *
 *************/
 
static Boolean SetValues(current, request, new)
     BrowseWidget       current,
                        request,
                        new;
{
    Arg args[2];
    int nargs;
 
/*
 * If image or button colors have changed, call XtSetValues to make the
change
 */
 
    if((new->browse.image_background != current->browse.image_background)
||
       (new->browse.image_foreground != current->browse.image_foreground))
    {
        if(new->browse.image)
        {
            nargs = 0;
            XtSetArg(args[nargs], XmNbackground,
new->browse.image_background);
            nargs++;
            XtSetArg(args[nargs], XmNforeground,
new->browse.image_foreground);
            nargs++;
 
            XtSetValues(new->browse.image, args, nargs);
        }
    }
 
    if((new->browse.button_background !=
current->browse.button_background) ||
       (new->browse.button_foreground !=
current->browse.button_foreground))
    {
        nargs = 0;
        XtSetArg(args[nargs], XmNbackground,
new->browse.button_background);
        nargs++;
        XtSetArg(args[nargs], XmNforeground,
new->browse.button_foreground);
        nargs++;
 
        if(new->browse.prev)
        {
            XtSetValues(new->browse.prev, args, nargs);
        }
 
        if(new->browse.next)
        {
            XtSetValues(new->browse.next, args, nargs);
        }
 
        if(new->browse.help)
        {
            XtSetValues(new->browse.help, args, nargs);
        }
 
        if(new->browse.quit)
        {
            XtSetValues(new->browse.quit, args, nargs);
        }
    }
 
/*
 * If images have changed, place first image in image label widget
 */
 
    if(new->browse.images != current->browse.images)
    {
        new->browse.current_image = -1;
 
        if(new->browse.image)
        {
            ChangeImage(new->browse.image, NEXT, NULL);
        }
    }
 
    return(FALSE);
}
 
 
/********************************
 ***** Convenience routines *****
 ********************************/
 
void
CreateChildren(parent)
     BrowseWidget       parent;
{
    Arg         args[6];
    int         nargs;
    Widget      left_att;
 
/*
 * Create child widgets
 */
 
/*
 * Image: Label
 */
 
    nargs = 0;
    XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_FORM); nargs++;
    XtSetArg(args[nargs], XmNleftAttachment, XmATTACH_FORM); nargs++;
    XtSetArg(args[nargs], XmNlabelType, XmPIXMAP); nargs++;
 
    parent->browse.image = XmCreateLabelGadget((Widget)parent,  /* parent    */
                                               "Image", /* name      */
                                               args,    /* arguments */
                                               nargs);  /* num_args  */
 
    if(parent->browse.images)
    {
        ChangeImage(parent->browse.image, NEXT, NULL);
    }
 
 
/*
 * Prev: PushButton
 */
 
    nargs = 0;
    XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_WIDGET); nargs++;
    XtSetArg(args[nargs], XmNtopWidget, parent->browse.image); nargs++;
    XtSetArg(args[nargs], XmNleftAttachment, XmATTACH_FORM); nargs++;
 
    parent->browse.prev = XmCreatePushButtonGadget((Widget)parent,      /* parent 
  */
                                                   "Prev",      /* name   
  */
                                                   args,        /*
arguments */
                                                   nargs);      /*
num_args  */
 
    XtAddCallback(parent->browse.prev,
                  XmNactivateCallback,
                  ChangeImage,
                  PREV);
 
/*
 * Next: PushButton
 */
 
    nargs = 0;
    XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_WIDGET); nargs++;
    XtSetArg(args[nargs], XmNtopWidget, parent->browse.image); nargs++;
    XtSetArg(args[nargs], XmNleftAttachment, XmATTACH_WIDGET); nargs++;
    XtSetArg(args[nargs], XmNleftWidget, parent->browse.prev); nargs++;
 
    parent->browse.next = XmCreatePushButtonGadget((Widget)parent,      /* parent 
  */
                                                   "Next",      /* name   
  */
                                                   args,        /*
arguments */
                                                   nargs);      /*
num_args  */
 
    XtAddCallback(parent->browse.next,
                  XmNactivateCallback,
                  ChangeImage,
                  (XtPointer) NEXT);
 
/*
 * Attachments for Help and Quit widgets depend on width
 * of Image widget versus widths of Prev and Next widgets
 */
 
    if(parent->browse.image->core.width >=
       (parent->browse.prev->core.width +
parent->browse.next->core.width))
    {
        left_att = parent->browse.image;
    }
    else
    {
        left_att = parent->browse.next;
    }
 
 
/*
 * Help: PushButton
 */
 
    nargs = 0;
    XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_FORM); nargs++;
    XtSetArg(args[nargs], XmNleftAttachment, XmATTACH_WIDGET); nargs++;
    XtSetArg(args[nargs], XmNleftWidget, left_att); nargs++;
 
    parent->browse.help = XmCreatePushButtonGadget((Widget)parent,      /* parent 
  */
                                                   "Help",      /* name   
  */
                                                   args,        /*
arguments */
                                                   nargs);      /*
num_args  */
 
    XtAddCallback(parent->browse.help,
                  XmNactivateCallback,
                  ButtonCallback,
                  (caddr_t) parent);
 
/*
 * Quit: PushButton
 */
 
    nargs = 0;
    XtSetArg(args[nargs], XmNtopAttachment, XmATTACH_WIDGET); nargs++;
    XtSetArg(args[nargs], XmNtopWidget, parent->browse.help); nargs++;
    XtSetArg(args[nargs], XmNleftAttachment, XmATTACH_WIDGET); nargs++;
    XtSetArg(args[nargs], XmNleftWidget, left_att); nargs++;
 
    parent->browse.quit = XmCreatePushButtonGadget((Widget)parent,      /* parent 
  */
                                                   "Quit",      /* name   
  */
                                                   args,        /*
arguments */
                                                   nargs);      /*
num_args  */
 
    XtAddCallback(parent->browse.quit,
                  XmNactivateCallback,
                  ButtonCallback,
                  (caddr_t) parent);
 
/*
 * Manage children
 */
 
    XtManageChildren(parent->composite.children,
                     parent->composite.num_children);
}
 
static void ChangeImage(w, which, call_data)
     Widget     w;
     int        which;
     XtPointer  call_data;
{
    Arg                 args[1];
    int                 nargs;
    BrowseWidget        parent = (BrowseWidget) w->core.parent;
    XmLabelGadget       image = (XmLabelGadget) parent->browse.image;
    int                 *current_image = &parent->browse.current_image,
                        num_images = parent->browse.num_images;
 
    if(which == NEXT)
    {
        if(*current_image < num_images - 1)
        {
            (*current_image)++;
 
            nargs = 0;
            XtSetArg(args[nargs], XmNlabelPixmap,
                     parent->browse.images[*current_image]); nargs++;
            XtSetValues((Widget)image, args, nargs);
        }
    }
    else if(which == PREV)
    {
        if(*current_image > 0)
        {
            (*current_image)--;
 
            nargs = 0;
            XtSetArg(args[nargs], XmNlabelPixmap,
                     parent->browse.images[*current_image]); nargs++;
            XtSetValues((Widget)image, args, nargs);
        }
    }
}
 
/******************
 * ButtonCallback *
 ******************/
 
static void ButtonCallback (w, parent, call_data)
     Widget             w;
     BrowseWidget       parent;
     caddr_t            call_data;
{
    if(w == parent->browse.help)
    {
        XtCallCallbacks((Widget)parent,
                        XmNhelpCallback,
                        NULL);
    }
    else if(w == parent->browse.quit)
    {
        XtCallCallbacks((Widget)parent,
                        XmNquitCallback,
                        NULL);
    }
 
}
 
/*******************
 *  XmCreateBrowse *
 *******************/
 
Widget XmCreateBrowse (parent, name, args, nargs)
     Widget     parent;
     char       *name;
     Arg        *args;
     int        nargs;
{
    return (XtCreateWidget(name,
                           browseWidgetClass,
                           parent,
                           args,
                           nargs));
}
