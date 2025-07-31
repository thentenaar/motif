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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: MessageB.c /main/18 1999/10/13 16:17:09 mgreess $"
#endif
#endif

#include <string.h>
#include <Xm/AccColorT.h>	/* for XmAccessColorDataRec */
#include <Xm/ActivatableT.h>
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/DialogS.h>
#include <Xm/DrawnB.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumnP.h>
#include <Xm/SeparatoG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/TraitP.h>
#include <Xm/VendorSP.h>	/* for the default display */
#include <Xm/VaSimpleP.h>
#include "BulletinBI.h"
#include "GeoUtilsI.h"
#include "ImageCachI.h"
#include "MessageBI.h"
#include "MessagesI.h"
#include "RepTypeI.h"
#include "TraversalI.h"
#include "XmI.h"
#include "ScreenI.h"

/* convenience macros */
#define Double(d)        ((d) << 1)
#define TotalWidth(w)    ((w)->core.width + Double ((w)->core.border_width))
#define TotalHeight(w)   ((w)->core.height + Double ((w)->core.border_width))
#define BottomEdge(w)    ((w)->core.y + TotalHeight (w))

#define IsButton(w) \
(((XtPointer) XmeTraitGet((XtPointer) XtClass(w), XmQTactivatable) != NULL))

#define IsAutoChild(mb, w) ( \
      (w) == mb->message_box.symbol_wid || \
      (w) == mb->message_box.message_wid || \
      (w) == mb->message_box.separator || \
      (w) == mb->message_box.ok_button || \
      (w) == mb->bulletin_board.cancel_button || \
      (w) == mb->message_box.help_button)

#define ARG_LIST_CNT    25

#define PIXMAP_LIST_MOD 6

/* defines for warning message */
#define WARNING4	_XmMMsgMessageB_0003
#define WARNING5	_XmMMsgMessageB_0004


/********    Static Function Declarations    ********/

static XImage * CreateDefaultImage(
                        char *bits,
                        unsigned int width,
                        unsigned int height) ;
static void ClassInitialize( void ) ;
static void ClassPartInitialize(
                        WidgetClass wc) ;
static void Initialize(
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy(
                        Widget wid) ;
static void DeleteChild(
                        Widget child) ;
static void MessageCallback(
                        Widget w,
                        XtPointer closure,
                        XtPointer call_data) ;
static Boolean SetValues(
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void SetUpSymbol(
                        XmMessageBoxWidget w) ;
static void SetUpMessage(
                        XmMessageBoxWidget w) ;
static void CreateWidgets(
                        XmMessageBoxWidget w) ;
static Widget CreateDialog(
                        Widget parent,
                        char *name,
                        ArgList al,
                        int ac,
                        unsigned char type) ;
static void GetMessageString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
static void GetSymbolPixmap(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
static void GetOkLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
static void GetCancelLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
static void GetHelpLabelString(
                        Widget wid,
                        int resource_offset,
                        XtArgVal *value) ;
static void GetMsgBoxPixmap(
                        XmMessageBoxWidget mBox) ;

/********    End Static Function Declarations    ********/


/*  Resource definitions, "get resources" first */

static XmSyntheticResource syn_resources[] = {

    {   XmNmessageString,
        sizeof (XmString),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.message_string),
        GetMessageString,
		NULL},

    {   XmNsymbolPixmap,
        sizeof (Pixmap),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.symbol_pixmap),
        GetSymbolPixmap,
		NULL},

    {   XmNokLabelString,
        sizeof (XmString),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.ok_label_string),
        GetOkLabelString,
		NULL},

    {   XmNcancelLabelString,
        sizeof (XmString),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.cancel_label_string),
        GetCancelLabelString,
		NULL},

    {   XmNhelpLabelString,
        sizeof (XmString),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.help_label_string),
        GetHelpLabelString,
		NULL},
};


static XtResource resources[] = {

    {   XmNdialogType,
        XmCDialogType,
        XmRDialogType,
        sizeof(unsigned char),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.dialog_type),
        XmRImmediate,
        (XtPointer) XmDIALOG_MESSAGE},

    {   XmNminimizeButtons,
        XmCMinimizeButtons,
        XmRBoolean,
        sizeof(Boolean),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.minimize_buttons),
        XmRImmediate,
        (XtPointer) False},

    {   XmNdefaultButtonType,
        XmCDefaultButtonType,
        XmRDefaultButtonType,
        sizeof(unsigned char),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.default_type),
        XmRImmediate,
        (XtPointer) XmDIALOG_OK_BUTTON},

    {   XmNmessageString,
        XmCXmString,
        XmRXmString,
        sizeof (XmString),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.message_string),
        XmRXmString,
        NULL},

    {   XmNmessageAlignment,
        XmCAlignment,
        XmRAlignment,
        sizeof(unsigned char),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.message_alignment),
        XmRImmediate,
        (XtPointer) XmALIGNMENT_BEGINNING},

    {   XmNsymbolPixmap,
        XmCPixmap,
        XmRDynamicPixmap,
        sizeof (Pixmap),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.symbol_pixmap),
        XmRImmediate,
        (XtPointer) XmUNSPECIFIED_PIXMAP},

    {   XmNokLabelString,
        XmCXmString,
        XmRXmString,
        sizeof (XmString),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.ok_label_string),
        XmRXmString,
        NULL},             /* "OK" default dynamically set from label name */

    {   XmNokCallback,
        XmCCallback,
        XmRCallback,
        sizeof (XtCallbackList),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.ok_callback),
        XmRCallback,
        NULL},

    {   XmNcancelLabelString,
        XmCXmString,
        XmRXmString,
        sizeof (XmString),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.cancel_label_string),
        XmRXmString,
        NULL},            /* "Cancel" default dynamically set from label name */

    {   XmNcancelCallback,
        XmCCallback,
        XmRCallback,
        sizeof (XtCallbackList),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.cancel_callback),
        XmRCallback,
        NULL},

    {   XmNhelpLabelString,
        XmCXmString,
        XmRXmString,
        sizeof (XmString),
        XtOffsetOf( struct _XmMessageBoxRec, message_box.help_label_string),
        XmRXmString,
        NULL},             /* "Help" default dynamically set from label name */

};



/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

externaldef( xmmessageboxclassrec) XmMessageBoxClassRec xmMessageBoxClassRec =
{
   {                                            /* core_class fields  */
      (WidgetClass) &xmBulletinBoardClassRec,   /* superclass         */
      "XmMessageBox",                           /* class_name         */
      sizeof(XmMessageBoxRec),                  /* widget_size        */
      ClassInitialize,                          /* class_initialize   */
      ClassPartInitialize,                      /* class_part_init    */
      FALSE,                                    /* class_inited       */
      Initialize,                               /* initialize         */
      NULL,                                     /* initialize_hook    */
      XtInheritRealize,                         /* realize            */
      NULL,                                     /* actions            */
      0,                                        /* num_actions        */
      resources,                                /* resources          */
      XtNumber(resources),                      /* num_resources      */
      NULLQUARK,                                /* xrm_class          */
      TRUE,                                     /* compress_motion    */
      XtExposeCompressMaximal,                  /* compress_exposure  */
      FALSE,                                    /* compress_enterlv   */
      FALSE,                                    /* visible_interest   */
      Destroy,                                  /* destroy            */
      XtInheritResize,                          /* resize             */
      XtInheritExpose,                          /* expose             */
      SetValues,                                /* set_values         */
      NULL,                                     /* set_values_hook    */
      XtInheritSetValuesAlmost,                 /* set_values_almost  */
      NULL,                                     /* get_values_hook    */
      XtInheritAcceptFocus,                     /* enter_focus        */
      XtVersion,                                /* version            */
      NULL,                                     /* callback_private   */
      XtInheritTranslations,                    /* tm_table           */
      XtInheritQueryGeometry,                   /* query_geometry     */
      NULL,                                     /* display_accelerator*/
      NULL,                                     /* extension          */
   },

   {                                            /* composite_class fields */
      XtInheritGeometryManager,                 /* geometry_manager   */
      XtInheritChangeManaged,                   /* change_managed     */
      XtInheritInsertChild,                     /* insert_child       */
      DeleteChild,                              /* delete_child       */
      NULL,                                     /* extension          */
   },

   {                                            /* constraint_class fields */
      NULL,                                     /* resource list        */
      0,                                        /* num resources        */
      sizeof(XmManagerConstraintRec),           /* constraint size      */
      NULL,                                     /* init proc            */
      NULL,                                     /* destroy proc         */
      NULL,                                     /* set values proc      */
      NULL,                                     /* extension            */
   },

   {                                            /* manager_class fields   */
      XmInheritTranslations,                    /* translations           */
      syn_resources,                            /* syn_resources          */
      XtNumber(syn_resources),                  /* num_syn_resources      */
      NULL,                                     /* syn_cont_resources     */
      0,                                        /* num_syn_cont_resources */
      XmInheritParentProcess,                   /* parent_process         */
      NULL,                                     /* extension              */
   },

   {                                            /* bulletinBoard class  */
      TRUE,                                     /*always_install_accelerators*/
      _XmMessageBoxGeoMatrixCreate,             /* geo__matrix_create */
      XmInheritFocusMovedProc,                  /* focus_moved_proc */
      NULL                                      /* extension */
   },

   {                                            /* messageBox class */
      (XtPointer) NULL                          /* extension */
   }
};

externaldef( xmmessageboxwidgetclass) WidgetClass xmMessageBoxWidgetClass
                                        = (WidgetClass) &xmMessageBoxClassRec ;


/************************************************************************
 *  Bitmap Data for Default Symbol
 **********************************<->***********************************/

static XmConst unsigned char errorBits[] = {
   0x00, 0x00, 0x00, 0xc0, 0x0f, 0x00, 0xf0, 0x3a, 0x00, 0x58, 0x55, 0x00,
   0x2c, 0xa0, 0x00, 0x56, 0x40, 0x01, 0xaa, 0x80, 0x02, 0x46, 0x81, 0x01,
   0x8a, 0x82, 0x02, 0x06, 0x85, 0x01, 0x0a, 0x8a, 0x02, 0x06, 0x94, 0x01,
   0x0a, 0xe8, 0x02, 0x14, 0x50, 0x01, 0x28, 0xb0, 0x00, 0xd0, 0x5f, 0x00,
   0xa0, 0x2a, 0x00, 0x40, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static XmConst unsigned char infoBits[] = {
   0x00, 0x00, 0x78, 0x00, 0x54, 0x00, 0x2c, 0x00, 0x54, 0x00, 0x28, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x2a, 0x00, 0x5c, 0x00, 0x28, 0x00,
   0x58, 0x00, 0x28, 0x00, 0x58, 0x00, 0x28, 0x00, 0x58, 0x00, 0x28, 0x00,
   0x58, 0x00, 0xae, 0x01, 0x56, 0x01, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00};

static XmConst unsigned char questionBits[] = {
   0xf0, 0x3f, 0x00, 0x58, 0x55, 0x00, 0xac, 0xaa, 0x00, 0xd6, 0x5f, 0x01,
   0xea, 0xbf, 0x02, 0xf6, 0x7f, 0x01, 0xea, 0xba, 0x02, 0xf6, 0x7d, 0x05,
   0xea, 0xba, 0x0a, 0x56, 0x7d, 0x15, 0xaa, 0xbe, 0x1e, 0x56, 0x5f, 0x01,
   0xac, 0xaf, 0x02, 0x58, 0x57, 0x01, 0xb0, 0xaf, 0x00, 0x60, 0x55, 0x01,
   0xa0, 0xaa, 0x00, 0x60, 0x17, 0x00, 0xa0, 0x2f, 0x00, 0x60, 0x17, 0x00,
   0xb0, 0x2a, 0x00, 0x50, 0x55, 0x00};

static XmConst unsigned char warningBits[] = {
   0x00, 0x00, 0x18, 0x00, 0x2c, 0x00, 0x56, 0x00, 0x2a, 0x00, 0x56, 0x00,
   0x2a, 0x00, 0x56, 0x00, 0x2c, 0x00, 0x14, 0x00, 0x2c, 0x00, 0x14, 0x00,
   0x2c, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x14, 0x00,
   0x2c, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00};

static XmConst unsigned char workingBits[] = {
   0x00, 0x00, 0x00, 0xfe, 0xff, 0x0f, 0xaa, 0xaa, 0x0a, 0x44, 0x55, 0x06,
   0xcc, 0x2a, 0x02, 0x44, 0x55, 0x06, 0xcc, 0x2a, 0x02, 0x84, 0x15, 0x06,
   0x8c, 0x2a, 0x02, 0x04, 0x15, 0x06, 0x0c, 0x0a, 0x02, 0x04, 0x06, 0x06,
   0x0c, 0x0b, 0x02, 0x84, 0x15, 0x06, 0xcc, 0x2a, 0x02, 0x44, 0x55, 0x06,
   0xcc, 0x2a, 0x02, 0x44, 0x55, 0x06, 0xcc, 0x2a, 0x02, 0x44, 0x55, 0x06,
   0xfe, 0xff, 0x0f, 0x56, 0x55, 0x05, 0x00, 0x00, 0x00};



/****************************************************************
 * Create a default images for symbol... used in ClassInitialize.
 ****************/
static XImage *
CreateDefaultImage(
        char *bits,
        unsigned int width,
        unsigned int height )
{
    XImage *        image ;
    Display * display = _XmGetDefaultDisplay() ; /* we don't have one here */

    _XmCreateImage(image, display, bits, width, height, LSBFirst);

    return( image) ;
}
/****************************************************************/
static void
ClassInitialize( void )
{
    XImage *image;
/****************/

    /* create and install the default images for the symbol */

    image = CreateDefaultImage ((char *)errorBits, 20, 20);
    XmInstallImage (image, "default_xm_error");

    image = CreateDefaultImage ((char *)infoBits, 11, 24);
    XmInstallImage (image, "default_xm_information");

    image = CreateDefaultImage ((char *)questionBits, 22, 22);
    XmInstallImage (image, "default_xm_question");

    image = CreateDefaultImage ((char *)warningBits, 9, 22);
    XmInstallImage (image, "default_xm_warning");

    image = CreateDefaultImage ((char *)workingBits, 21, 23);
    XmInstallImage (image, "default_xm_working");

    return ;
}
/****************************************************************/
static void
ClassPartInitialize(
        WidgetClass wc )
{
/****************/

    _XmFastSubclassInit (wc, XmMESSAGE_BOX_BIT);

    return ;
}
/****************************************************************
 * MessageBox widget specific initialization
 ****************/
static void
Initialize(
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
            XmMessageBoxWidget new_w = (XmMessageBoxWidget) nw ;
            Widget          defaultButton ;
/****************/

    new_w->message_box.message_wid = NULL;
    new_w->message_box.symbol_wid = NULL;
    new_w->message_box.separator = NULL;
    new_w->message_box.ok_button = NULL;
    new_w->bulletin_board.cancel_button = NULL;
    new_w->message_box.help_button = NULL;
    new_w->bulletin_board.default_button = NULL;
    new_w->message_box.internal_pixmap = FALSE ;
    new_w->message_box.baseline = 0;

    if(    !XmRepTypeValidValue( XmRID_DIALOG_TYPE,
                               new_w->message_box.dialog_type, (Widget) new_w)    )
    {   new_w->message_box.dialog_type = XmDIALOG_MESSAGE ;
        }

    if(    !XmRepTypeValidValue( XmRID_ALIGNMENT,
                         new_w->message_box.message_alignment, (Widget) new_w)    )
    {   new_w->message_box.message_alignment = XmALIGNMENT_BEGINNING ;
        }

    CreateWidgets( new_w) ;

    if(    !XmRepTypeValidValue( XmRID_DEFAULT_BUTTON_TYPE,
                              new_w->message_box.default_type, (Widget) new_w)    )
    {   new_w->message_box.default_type = XmDIALOG_OK_BUTTON ;
        }
    switch(    new_w->message_box.default_type    )
    {
        case XmDIALOG_CANCEL_BUTTON:
        {   defaultButton = new_w->bulletin_board.cancel_button ;
            break ;
            }
        case XmDIALOG_HELP_BUTTON:
        {   defaultButton = new_w->message_box.help_button ;
            break ;
            }
        case XmDIALOG_OK_BUTTON:
        {   defaultButton = new_w->message_box.ok_button ;
            break ;
            }
        default:
	{   defaultButton = NULL ;
	    }
        }
      if(    defaultButton    )
      {
          BB_DefaultButton( new_w) = defaultButton ;
          new_w->manager.initial_focus = defaultButton ;
          _XmBulletinBoardSetDynDefaultButton( (Widget) new_w, defaultButton) ;
      }
    return ;
    }
/****************************************************************
 * Destroy the widget specific data structs
 ****************/
static void
Destroy(
        Widget wid )
{
            XmMessageBoxWidget d = (XmMessageBoxWidget) wid ;
/****************/

    if(    (d->message_box.symbol_pixmap != XmUNSPECIFIED_PIXMAP)
        && d->message_box.internal_pixmap    )
    {
        XmDestroyPixmap( d->core.screen, d->message_box.symbol_pixmap) ;
        }
    return ;
    }

/****************************************************************
 * Set the widget id to NULL for the child widget being destroyed.
 ****************/
static void
DeleteChild(
        Widget child )
{
            XmMessageBoxWidget mbox ;
	    XtWidgetProc      delete_child;
/****************/

    if(    XtIsRectObj( child)    )
    {
        mbox = (XmMessageBoxWidget) XtParent( child) ;

        /* Check for which child is getting destroyed and set to NULL.
        */
        if(    child == mbox->message_box.message_wid    )
        {   mbox->message_box.message_wid = NULL ;
            }
        else
        {   if(    child == mbox->message_box.symbol_wid    )
            {   mbox->message_box.symbol_wid = NULL ;
                }
            else
            {   if(    child == mbox->message_box.ok_button    )
                {   mbox->message_box.ok_button = NULL ;
                    }
                else
                {   if(    child == mbox->message_box.help_button    )
                    {   mbox->message_box.help_button = NULL ;
                        }
                    else
                    {   if(    child == mbox->message_box.separator    )
                        {   mbox->message_box.separator = NULL ;
                            }
                        }
                    }
                }
            }
        }
    _XmProcessLock();
    delete_child = ((XmBulletinBoardWidgetClass) xmBulletinBoardWidgetClass)
				  ->composite_class.delete_child;
    _XmProcessUnlock();
    (*delete_child)(child) ;
    return ;
    }

/****************************************************************/
static void
MessageCallback(
        Widget w,
        XtPointer closure,
        XtPointer call_data )
{
            XmMessageBoxWidget tag = (XmMessageBoxWidget) closure ;

    XmAnyCallbackStruct temp;
/****************/

    if (call_data) {
    	temp.reason  = ((XmAnyCallbackStruct *) call_data)->reason;
    	temp.event   = ((XmAnyCallbackStruct *) call_data)->event;
	}
    else {
    	temp.reason  = 0;
    	temp.event   = NULL;
    }

    if (tag->message_box.ok_button == w) {
        temp.reason = XmCR_OK;
        XtCallCallbackList ((Widget) tag, tag->message_box.ok_callback, &temp);
    }
    else if (tag->bulletin_board.cancel_button == w) {
        temp.reason = XmCR_CANCEL;
        XtCallCallbackList ((Widget) tag, tag->message_box.cancel_callback, &temp);
    }
    else if (tag->message_box.help_button == w) {
        temp.reason  = XmCR_HELP;
        XtCallCallbackList ((Widget) tag, tag->manager.help_callback, &temp);
    }
    return ;
}

/****************************************************************
 * Set attributes of a message widget
 ****************/
static Boolean
SetValues(
        Widget cw,
        Widget rw,		/* unused */
        Widget nw,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
            XmMessageBoxWidget current = (XmMessageBoxWidget) cw ;
            XmMessageBoxWidget new_w = (XmMessageBoxWidget) nw ;
            Arg             al[ARG_LIST_CNT] ;  /* arg list       */
            Cardinal        ac = 0;             /* arg list count */
            Boolean         need_layout = FALSE ;
            Boolean         newPixmap = FALSE ;
            Widget          defaultButton ;
            XmRenderTable rt;
            XmString s;
/****************/

    /* "in_set_values" means the GeometryManager won't try to resize
    *    and/or re-layout subwidgets.
    */
    BB_InSetValues( new_w) = True;

    if(    new_w->message_box.message_string    )
    {
        XtSetArg( al[ac], XmNlabelString,
                                      new_w->message_box.message_string) ; ++ac ;
        XtSetArg( al[ac], XmNstringDirection,
                                             BB_StringDirection( new_w)) ; ++ac ;
        new_w->message_box.message_string = NULL ;
        need_layout = TRUE ;
        }
    if(    current->message_box.message_alignment
                                     != new_w->message_box.message_alignment    )
    {   if(    !XmRepTypeValidValue( XmRID_ALIGNMENT,
                         new_w->message_box.message_alignment, (Widget) new_w)    )
        {   new_w->message_box.message_alignment
                                     = current->message_box.message_alignment ;
            }
        else
        {   XtSetArg( al[ac], XmNalignment,
                                   new_w->message_box.message_alignment) ; ++ac ;
            need_layout = TRUE ;
            }
        }
    if( ac )
    {
      /* CR 7596,  create message widget if not there */
      if ( ! new_w->message_box.message_wid ) {
	SetUpMessage(new_w);
	XtManageChild(new_w->message_box.message_wid);
      }

      XtSetValues( new_w->message_box.message_wid, al, ac) ;

      /* Ensure we have the correct baseline */
      XtSetArg(al[0], XmNrenderTable, &rt);
      XtSetArg(al[1], XmNlabelString, &s);
      XtGetValues(new_w->message_box.message_wid, al, 2);
      new_w->message_box.baseline = XmStringBaseline(rt, s);
      XmStringFree(s);
    }

	    /* fix for CR 5895 */
    if(    !new_w->message_box.symbol_pixmap    )
    {   new_w->message_box.symbol_pixmap = XmUNSPECIFIED_PIXMAP ;
        }

    /* Over-ride BBoard to disallow a direct change of button id's
     * unless it's a template dialog.  For those, if you can't set it
     * here then you can never set it.
    */
    if ( ( current->message_box.dialog_type != XmDIALOG_TEMPLATE ) &&
         ( current->bulletin_board.cancel_button !=
                                        new_w->bulletin_board.cancel_button ) )
    {
        new_w->bulletin_board.cancel_button
                                       = current->bulletin_board.cancel_button;
        XmeWarning( (Widget) new_w, WARNING5);
    }

    if(    (current->message_box.dialog_type != new_w->message_box.dialog_type)
        && !XmRepTypeValidValue( XmRID_DIALOG_TYPE,
                               new_w->message_box.dialog_type, (Widget) new_w)    )
    {
        new_w->message_box.dialog_type = current->message_box.dialog_type ;
        }

    if(    new_w->message_box.symbol_pixmap
                                     != current->message_box.symbol_pixmap    )
    {   newPixmap = TRUE ;
        new_w->message_box.internal_pixmap = FALSE ;

        if(    (current->message_box.symbol_pixmap != XmUNSPECIFIED_PIXMAP)
            && current->message_box.internal_pixmap    )
        {
            XmDestroyPixmap( current->core.screen,
                                          current->message_box.symbol_pixmap) ;
            }
        }
    else
    {   /* If symbol pixmap is unchanged and a new dialog type is specified,
        *   then set to default pixmap.
        */
        if(new_w->message_box.dialog_type != current->message_box.dialog_type ||
           new_w->message_box.baseline != current->message_box.baseline)
        {   newPixmap = TRUE ;

            GetMsgBoxPixmap( new_w) ;

            if(    (current->message_box.symbol_pixmap != XmUNSPECIFIED_PIXMAP)
                && current->message_box.internal_pixmap    )
            {
                XmDestroyPixmap( current->core.screen,
                                          current->message_box.symbol_pixmap) ;
                }
            }
        }
    if(    newPixmap    )
    {
      need_layout = TRUE ;

      /* CR 7596,  create child widget if needed */
      if( ! new_w->message_box.symbol_wid ) {
	SetUpSymbol(new_w);
	XtManageChild(new_w->message_box.symbol_wid);
      }

      ac = 0 ;
      XtSetArg( al[ac], XmNlabelPixmap,
	       new_w->message_box.symbol_pixmap) ; ++ac ;
      XtSetValues( new_w->message_box.symbol_wid, al, ac) ;
    }

    /* Check the buttons and labels
    */
    if(    new_w->message_box.ok_label_string    )
    {
        if(    new_w->message_box.ok_button    )
        {   ac = 0 ;
            XtSetArg( al[ac], XmNlabelString,
                                     new_w->message_box.ok_label_string) ; ++ac ;
            XtSetArg( al[ac], XmNstringDirection,
                                             BB_StringDirection( new_w)) ; ++ac ;
            XtSetValues( new_w->message_box.ok_button, al, ac) ;
            }
        new_w->message_box.ok_label_string = NULL ;
        need_layout = TRUE ;
        }
    if(    new_w->message_box.cancel_label_string    )
    {
        if(    new_w->bulletin_board.cancel_button    )
        {   ac = 0 ;
            XtSetArg( al[ac], XmNlabelString,
                                 new_w->message_box.cancel_label_string) ; ++ac ;
            XtSetArg( al[ac], XmNstringDirection,
                                             BB_StringDirection( new_w)) ; ++ac ;
            XtSetValues( new_w->bulletin_board.cancel_button, al, ac) ;
            }
        new_w->message_box.cancel_label_string = NULL ;
        need_layout = TRUE ;
        }
    if(    new_w->message_box.help_label_string    )
    {
        if(    new_w->message_box.help_button    )
        {   ac = 0 ;
            XtSetArg( al[ac], XmNlabelString,
                                   new_w->message_box.help_label_string) ; ++ac ;
            XtSetArg( al[ac], XmNstringDirection,
                                             BB_StringDirection( new_w)) ; ++ac ;
            XtSetValues( new_w->message_box.help_button, al, ac) ;
            }
        new_w->message_box.help_label_string = NULL ;
        need_layout = TRUE ;
        }

    /* If Default Pushbutton changes, reset showAsDefault.
    */
    if(    current->message_box.default_type
                                        != new_w->message_box.default_type    )
    {   if(    !XmRepTypeValidValue( XmRID_DEFAULT_BUTTON_TYPE,
                          new_w->message_box.default_type, (Widget) new_w)    )
        {   new_w->message_box.default_type
	                                  = current->message_box.default_type ;
            }
        else
        {   switch(    new_w->message_box.default_type    )
            {
                case XmDIALOG_CANCEL_BUTTON:
                {   defaultButton = new_w->bulletin_board.cancel_button ;
                    break ;
                    }
                case XmDIALOG_HELP_BUTTON:
                {   defaultButton = new_w->message_box.help_button ;
                    break ;
                    }
                case XmDIALOG_OK_BUTTON:
                {   defaultButton = new_w->message_box.ok_button ;
                    break ;
                    }
                default:
		{   defaultButton = NULL ;
		    }
        }
		BB_DefaultButton( new_w) = defaultButton ;
		_XmBulletinBoardSetDynDefaultButton( (Widget) new_w, defaultButton);

	    if(    (current->manager.initial_focus
	                                     == BB_DefaultButton( current))
	       &&  (current->manager.initial_focus
		                          == new_w->manager.initial_focus)    )
	    {   new_w->manager.initial_focus = defaultButton ;
		_XmSetInitialOfTabGroup( (Widget) new_w, defaultButton) ;
	        }
	    }
        }
    BB_InSetValues( new_w) = False;

    /* Re-layout the sub-widgets if necessary
    */
    if(    need_layout
        && (XtClass( new_w) == xmMessageBoxWidgetClass)    )
    {
        _XmBulletinBoardSizeUpdate( (Widget) new_w) ;
        }
    return( FALSE) ;
    }

/****************************************************************
 * Set up the icon (pixmap label widget) and the label widget itself.
 ****************/
static void
SetUpSymbol(
        XmMessageBoxWidget w )
{
    Arg al[ARG_LIST_CNT];
    int ac;
/****************/

    /* If no pixmap specified, try to get from bitmap file or default
    */
    if(    w->message_box.symbol_pixmap == XmUNSPECIFIED_PIXMAP    )
    {   GetMsgBoxPixmap( w) ;
        }

    if(    !w->message_box.symbol_pixmap    )
    {   w->message_box.symbol_pixmap = XmUNSPECIFIED_PIXMAP ;
        }

    /* Create symbol label even if no pixmap specified; allows SetValues
    *   on dialogType to make pixmap appear in what starts as message box.
    */
    ac = 0;
    XtSetArg (al[ac], XmNwidth, 0);                                    ac++;
    XtSetArg (al[ac], XmNheight, 0);                                   ac++;
    XtSetArg (al[ac], XmNlabelType, XmPIXMAP);                         ac++;
    XtSetArg (al[ac], XmNlabelPixmap, w->message_box.symbol_pixmap);   ac++;
    XtSetArg (al[ac], XmNtraversalOn, False);                          ac++;

    w->message_box.symbol_wid = XmCreateLabelGadget( (Widget) w, "Symbol",
                                                                       al, ac);
    return ;
    }

/****************************************************************
 * Set up the message label (assumes none set yet).
 ****************/
static void
SetUpMessage(
        XmMessageBoxWidget w )
{
    Arg al[ARG_LIST_CNT];
    int ac;
    XmString empty_string = NULL ;
/****************/

    /* set up y value dependent on symbol_wid */

    ac = 0;
    XtSetArg (al[ac], XmNalignment, w->message_box.message_alignment);  ac++;
    XtSetArg (al[ac], XmNborderWidth, 0);                               ac++;
    XtSetArg (al[ac], XmNtraversalOn, False);                           ac++;
    if (w->message_box.message_string) {
       XtSetArg(al[ac], XmNlabelString, w->message_box.message_string); ac++;
       w->message_box.message_string = NULL;
    } else {
	/* we don't want "Message", the name of the label, to become
	   the label string, it would break the AES */
/* BEGIN OSF Fix CR 4847 */
	empty_string = XmStringCreateLocalized(XmS);
/* END OSF Fix CR 4847 */
	XtSetArg(al[ac], XmNlabelString, empty_string); ac++;
    }
    XtSetArg (al[ac], XmNstringDirection, BB_StringDirection( w)) ;
    ac++;

    w->message_box.message_wid = XmCreateLabelGadget( (Widget) w, "Message",
						     al, ac);
/* BEGIN OSF Fix CR 4847 */
    if (empty_string != NULL) XmStringFree(empty_string);
/* END OSF Fix CR 4847 */

    return ;
}

/****************************************************************/
XmGeoMatrix
_XmMessageBoxGeoMatrixCreate(
        Widget wid,
        Widget instigator,
        XtWidgetGeometry *desired )
{
            XmMessageBoxWidget mb = (XmMessageBoxWidget) wid ;
            XmGeoMatrix     geoSpec ;
    register XmGeoRowLayout  layoutPtr ;
    register XmKidGeometry   boxPtr ;
            XmKidGeometry   firstBoxInRow ;
            Widget menubar = NULL;
            Widget workarea = NULL;
            Boolean has_buttons = False;
            Boolean has_message = False;
            Boolean menubar_adjust = False;
            Boolean add_pixmap = False;
            int nrows = 2;
            int nchildren = mb->composite.num_children;
            int i;

    /*
     * Layout MessageBox XmGeoMatrix.
     * Each row is terminated by leaving an empty XmKidGeometry and
     * moving to the next XmGeoRowLayout.
     */

    /* identify menu bar and work area children. */

    for (i=0; i < nchildren; i++)
    {   register Widget w = mb->composite.children[i];

        if( menubar == NULL
            && XmIsRowColumn(w)
            && ((XmRowColumnWidget)w)->row_column.type == XmMENU_BAR)
        {   menubar = w;
            nrows += 1;
            }
        else
        {    if (IsButton(w))
             {   has_buttons = True;
                }
             else
             {   if (workarea == NULL && !IsAutoChild(mb,w))
                {   workarea = w;
                    nrows += 1;
                    }
                }
             }
         }
    if (has_buttons)
	nrows += 1;

    geoSpec = _XmGeoMatrixAlloc( nrows, nchildren, 0) ;
    geoSpec->composite = (Widget) mb ;
    geoSpec->instigator = (Widget) instigator ;
    if (desired)
	geoSpec->instig_request = *desired ;
    geoSpec->margin_w = BB_MarginWidth( mb) + mb->manager.shadow_thickness ;
    geoSpec->margin_h = BB_MarginHeight( mb) + mb->manager.shadow_thickness ;
    geoSpec->no_geo_request = _XmMessageBoxNoGeoRequest ;

    layoutPtr = &(geoSpec->layouts->row) ;
    boxPtr = geoSpec->boxes ;

    /* menu bar */

    if(    menubar && _XmGeoSetupKid( boxPtr, menubar)    )
    {   layoutPtr->fix_up = _XmMenuBarFix ;
        menubar_adjust = True ;
        boxPtr += 2;
        ++layoutPtr;
        }

    /* symbol pixmap and message string */

    firstBoxInRow = boxPtr ;

    if (LayoutIsRtoLM(mb))
    {
       if(    _XmGeoSetupKid( boxPtr, mb->message_box.message_wid)    )
       {   has_message = True ;
	   ++boxPtr ;
           }
       if(    (mb->message_box.symbol_pixmap != XmUNSPECIFIED_PIXMAP)
           && _XmGeoSetupKid( boxPtr, mb->message_box.symbol_wid)    )
       {   ++boxPtr ;
           }
    }
    else
    {
      if(    (mb->message_box.symbol_pixmap != XmUNSPECIFIED_PIXMAP)
	 && _XmGeoSetupKid( boxPtr, mb->message_box.symbol_wid)    )
	{   ++boxPtr ;
	  }
      if(    _XmGeoSetupKid( boxPtr, mb->message_box.message_wid)    )
	{   has_message = True ;
	    ++boxPtr ;
	  }
    }
    if(    boxPtr != firstBoxInRow && (workarea == NULL || has_message)    )
    {   if(    menubar_adjust    )
        {   menubar_adjust = False ;
            }
        else
        {   layoutPtr->space_above = BB_MarginHeight( mb) ;
            }
        layoutPtr->space_between = BB_MarginWidth( mb) ;
        ++boxPtr ;
        ++layoutPtr ;
        firstBoxInRow = boxPtr ;
        }

    /* work area */

    if (LayoutIsRtoLM(mb) && !has_message && boxPtr != firstBoxInRow)
    {   --boxPtr; /* we want the workarea to the left of the pixmap */
	add_pixmap = True;
        }

    if(    workarea && _XmGeoSetupKid( boxPtr, workarea)    )
    {   ++boxPtr ;
        }

    if (add_pixmap)
      if(    (mb->message_box.symbol_pixmap != XmUNSPECIFIED_PIXMAP)
	 && _XmGeoSetupKid( boxPtr, mb->message_box.symbol_wid)    )
	{   ++boxPtr ;
	  }

    if(    boxPtr != firstBoxInRow    )
    {   layoutPtr->fill_mode = XmGEO_EXPAND;
        layoutPtr->fit_mode = XmGEO_PROPORTIONAL;
        if(    menubar_adjust    )
        {   menubar_adjust = False;
            }
        else
        {   layoutPtr->space_above = BB_MarginHeight( mb) ;
            }
        layoutPtr->space_between = BB_MarginWidth(mb);
        layoutPtr->stretch_height = True;
        layoutPtr->even_height = 1;
        boxPtr++;
        ++layoutPtr ;
        }

    /* separator */

    if(    _XmGeoSetupKid( boxPtr, mb->message_box.separator)    )
    {   layoutPtr->fix_up = _XmSeparatorFix ;
        layoutPtr->space_above = BB_MarginHeight( mb) ;
        boxPtr += 2 ;
        ++layoutPtr ;
        }

    /* buttons */

    if (LayoutIsRtoLM(mb))
    {
	if(    _XmGeoSetupKid( boxPtr, mb->message_box.help_button)    )
	{   ++boxPtr ;
	    }

	if(    _XmGeoSetupKid( boxPtr, mb->bulletin_board.cancel_button)    )
	{   ++boxPtr ;
	    }

	for (i = 0; i < nchildren; i++)
	{
	    register Widget w = mb->composite.children[nchildren-i-1];

	    if(   !IsAutoChild(mb,w) && IsButton(w) && _XmGeoSetupKid(boxPtr, w)  )
	    {   ++boxPtr;
		}
	}

	if(    _XmGeoSetupKid( boxPtr, mb->message_box.ok_button)    )
	{   ++boxPtr ;
	    }
    }
    else
    {
      if(    _XmGeoSetupKid( boxPtr, mb->message_box.ok_button)    )
	{   ++boxPtr ;
	  }
      for (i = 0; i < nchildren; i++)
	{
	  register Widget w = mb->composite.children[i];

	  if(   !IsAutoChild(mb,w) && IsButton(w) && _XmGeoSetupKid(boxPtr, w)  )
	    {   ++boxPtr;
	      }
        }
      if(    _XmGeoSetupKid( boxPtr, mb->bulletin_board.cancel_button)    )
	{   ++boxPtr ;
	  }
      if(    _XmGeoSetupKid( boxPtr, mb->message_box.help_button)    )
	{   ++boxPtr ;
	  }
    }

    if(    has_buttons    )
    {   layoutPtr->fill_mode = XmGEO_CENTER ;
        layoutPtr->fit_mode = XmGEO_WRAP ;
        if(    !mb->message_box.minimize_buttons    )
        {   layoutPtr->even_width = 1 ;
            }
        layoutPtr->even_height = 1 ;
        layoutPtr->space_above = BB_MarginHeight( mb) ;
	++layoutPtr ;
        }

    /* end */

    layoutPtr->space_above = BB_MarginHeight( mb) ;
    layoutPtr->end = TRUE ;
    return( geoSpec) ;
    }
/****************************************************************/
Boolean
_XmMessageBoxNoGeoRequest(
        XmGeoMatrix geoSpec )
{
/****************/

    if(    BB_InSetValues( geoSpec->composite)
        && (XtClass( geoSpec->composite) == xmMessageBoxWidgetClass)    )
    {
        return( TRUE) ;
        }
    return( FALSE) ;
    }



/****************************************************************
 * Construct the required captive widgets for the box.  Don't worry about
 *   positioning since a layout will happen later on.
 ****************/
static void
CreateWidgets(
        XmMessageBoxWidget w )
{
    Arg  al[2];

/****************/

    /* create the message label */
    if (!(w->message_box.dialog_type == XmDIALOG_TEMPLATE &&
	w->message_box.message_string == NULL))
	SetUpMessage (w);

    /* create the symbol label */
    if (!(w->message_box.dialog_type == XmDIALOG_TEMPLATE &&
	w->message_box.symbol_pixmap == XmUNSPECIFIED_PIXMAP))
	SetUpSymbol (w);

    /* create the separator */
    XtSetArg (al[0], XmNhighlightThickness, 0);
    w->message_box.separator = XmCreateSeparatorGadget( (Widget) w,
                                                           "Separator", al, 1);

    /* create all pushbuttons, user can unmanage if they don't want them */

    /* "Ok" button... if no label, use default localized "OK" string from
       message catalog, or "OK" if no message catatlog, for label */
    if (!(w->message_box.dialog_type == XmDIALOG_TEMPLATE &&
	w->message_box.ok_label_string == NULL &&
	w->message_box.ok_callback == NULL))
    {
	w->message_box.ok_button = _XmBB_CreateButtonG( (Widget) w,
                                        w->message_box.ok_label_string, "OK",
					XmOkStringLoc) ;
	w->message_box.ok_label_string = NULL;
	XtAddCallback( w->message_box.ok_button, XmNactivateCallback,
                                              MessageCallback, (XtPointer) w) ;
    }
    /* "Cancel" button... if no label, use default localized "Cancel" string from
       message catalog, or "Cancel" if no message catalog, for label */
    if (!(w->message_box.dialog_type == XmDIALOG_TEMPLATE &&
	w->message_box.cancel_label_string == NULL &&
	w->message_box.cancel_callback == NULL))
    {
	w->bulletin_board.cancel_button = _XmBB_CreateButtonG( (Widget) w,
                                w->message_box.cancel_label_string, "Cancel",
				XmCancelStringLoc) ;
	w->message_box.cancel_label_string = NULL;
	XtAddCallback( w->bulletin_board.cancel_button, XmNactivateCallback,
                                              MessageCallback, (XtPointer) w) ;
    }
    /* "Help" button... if no label, use default localized "Help" string from
       message catalog, or "Help" if no message catalog, for label */
    if (!(w->message_box.dialog_type == XmDIALOG_TEMPLATE &&
	w->message_box.help_label_string == NULL &&
	w->manager.help_callback == NULL))
    {
	w->message_box.help_button = _XmBB_CreateButtonG( (Widget) w,
                                    w->message_box.help_label_string, "Help",
				    XmHelpStringLoc) ;
	w->message_box.help_label_string = NULL;

	/* Remove BulletinBoard Unmanage callback from help button.
	*/
	XtRemoveAllCallbacks( w->message_box.help_button, XmNactivateCallback) ;
	XtAddCallback( w->message_box.help_button, XmNactivateCallback,
                                              MessageCallback, (XtPointer) w) ;
    }
    /* Now manage all my children.
    */
    XtManageChildren (w->composite.children, w->composite.num_children);
    return ;
}



/****************************************************************
 * Common create routine for message dialogs...
 *   it will create the shell and widgets, and set the dialog_type to
 *   whatever has been passed in...
 ****************/
static Widget
CreateDialog(
        Widget parent,
        char *name,
        ArgList al,
        int ac,
        unsigned char type )        /* type of dialog being created */
{
    Widget w;
    ArgList  argsNew;
/****************/

     /* add dialogType to arglist and force to type passed in... */

    /*  allocate arglist, copy args, add dialog type arg */
    argsNew = (ArgList) XtMalloc (sizeof(Arg) * (ac + 1));
    memcpy( argsNew, al, sizeof(Arg) * ac);
    XtSetArg (argsNew[ac], XmNdialogType, type);  ac++;

    /*  create MessageBoxDialog free argsNew, return */
    w = XmeCreateClassDialog (xmMessageBoxWidgetClass,
			      parent, name, argsNew, ac) ;
    XtFree((char*)argsNew);
    return w ;
}

/****************************************************************/
Widget
XmCreateMessageBox(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
    return XtCreateWidget (name, xmMessageBoxWidgetClass, parent, al, ac);
}

Widget
XmVaCreateMessageBox(
        Widget parent,
        char *name,
        ...)
{
    register Widget w;
    va_list var;
    int count;

    Va_start(var,name);
    count = XmeCountVaListSimple(var);
    va_end(var);


    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmMessageBoxWidgetClass,
                         parent, False,
                         var, count);
    va_end(var);
    return w;
}

Widget
XmVaCreateManagedMessageBox(
        Widget parent,
        char *name,
        ...)
{
    Widget w = NULL;
    va_list var;
    int count;

    Va_start(var, name);
    count = XmeCountVaListSimple(var);
    va_end(var);

    Va_start(var, name);
    w = XmeVLCreateWidget(name,
                         xmMessageBoxWidgetClass,
                         parent, True,
                         var, count);
    va_end(var);
    return w;
}

/****************************************************************/
Widget
XmCreateMessageDialog(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
    return CreateDialog (parent, name, al, ac, XmDIALOG_MESSAGE);
}
/****************************************************************/
Widget
XmCreateErrorDialog(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
    return CreateDialog (parent, name, al, ac, XmDIALOG_ERROR);
}
/****************************************************************/
Widget
XmCreateInformationDialog(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
    return CreateDialog (parent, name, al, ac, XmDIALOG_INFORMATION);
}
/****************************************************************/
Widget
XmCreateQuestionDialog(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
    return CreateDialog (parent, name, al, ac, XmDIALOG_QUESTION);
}
/****************************************************************/
Widget
XmCreateWarningDialog(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
    return CreateDialog (parent, name, al, ac, XmDIALOG_WARNING);
}
/****************************************************************/
Widget
XmCreateWorkingDialog(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
    return CreateDialog (parent, name, al, ac, XmDIALOG_WORKING);
}

/****************************************************************/
Widget
XmCreateTemplateDialog(
        Widget parent,
        char *name,
        ArgList al,
        Cardinal ac )
{
    return CreateDialog (parent, name, al, ac, XmDIALOG_TEMPLATE);
}

/****************************************************************/
Widget
XmMessageBoxGetChild(
        Widget widget,
        unsigned char child )
{
    XmMessageBoxWidget  w = (XmMessageBoxWidget)widget;
    Widget child_widget = NULL;
    _XmWidgetToAppContext(widget);
/****************/

    _XmAppLock(app);
    switch (child) {
        case XmDIALOG_DEFAULT_BUTTON:
		child_widget =  w->bulletin_board.default_button;
		break;
        case XmDIALOG_SYMBOL_LABEL:
		child_widget = w->message_box.symbol_wid;
		break;
        case XmDIALOG_MESSAGE_LABEL:
		child_widget = w->message_box.message_wid;
		break;
        case XmDIALOG_OK_BUTTON:
		child_widget = w->message_box.ok_button;
		break;
        case XmDIALOG_CANCEL_BUTTON:
		child_widget = w->bulletin_board.cancel_button;
		break;
        case XmDIALOG_HELP_BUTTON:
		child_widget = w->message_box.help_button;
		break;
        case XmDIALOG_SEPARATOR:
		child_widget = w->message_box.separator;
		break;
        default:
		XmeWarning( (Widget) w, WARNING4); break ;
		break;
    }
    _XmAppUnlock(app);
    return child_widget ;
}
/****************************************************************/
static void
GetMessageString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmMessageBoxWidget w = (XmMessageBoxWidget) wid ;
    XmString  data;
    Arg       al[1];
/****************/

    if (w->message_box.message_wid) {
        XtSetArg (al[0], XmNlabelString, &data);
        XtGetValues (w->message_box.message_wid, al, 1);
        *value = (XtArgVal) data;
    }
    else *value = (XtArgVal) NULL;
    return ;
}
/****************************************************************/
static void
GetSymbolPixmap(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmMessageBoxWidget w = (XmMessageBoxWidget) wid ;
    Pixmap  data;
    Arg     al[1];
/****************/

    if (w->message_box.symbol_wid) {
        XtSetArg (al[0], XmNlabelPixmap, &data);
        XtGetValues (w->message_box.symbol_wid, al, 1);
        *value = (XtArgVal) data;
    }
    else *value = (XtArgVal) NULL;
    return ;
}
/****************************************************************/
static void
GetOkLabelString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmMessageBoxWidget w = (XmMessageBoxWidget) wid ;
    XmString  data;
    Arg       al[1];
/****************/

    if (w->message_box.ok_button) {
        XtSetArg (al[0], XmNlabelString, &data);
        XtGetValues (w->message_box.ok_button, al, 1);
        *value = (XtArgVal) data;
    }
    else *value = (XtArgVal) NULL;
    return ;
}
/****************************************************************/
static void
GetCancelLabelString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmMessageBoxWidget w = (XmMessageBoxWidget) wid ;
    XmString  data;
    Arg       al[1];
/****************/

    if (w->bulletin_board.cancel_button) {
        XtSetArg (al[0], XmNlabelString, &data);
        XtGetValues (w->bulletin_board.cancel_button, al, 1);
        *value = (XtArgVal) data;
    }
    else *value = (XtArgVal) NULL;
    return ;
}
/****************************************************************/
static void
GetHelpLabelString(
        Widget wid,
        int resource_offset,	/* unused */
        XtArgVal *value )
{
            XmMessageBoxWidget w = (XmMessageBoxWidget) wid ;
    XmString  data;
    Arg       al[1];
/****************/

    if (w->message_box.help_button) {
        XtSetArg (al[0], XmNlabelString, &data);
        XtGetValues (w->message_box.help_button, al, 1);
        *value = (XtArgVal) data;
    }
    else *value = (XtArgVal) NULL;
    return ;
}
/****************************************************************/
static void
GetMsgBoxPixmap(
        XmMessageBoxWidget mBox )
{
    Arg arg[2];
    Pixmap          tmpPix = XmUNSPECIFIED_PIXMAP ;
    char *          fileName ;
    char *          defaultName ;
    XmAccessColorDataRec acc_color_rec;
    XmRenderTable rt;
    XmString s;

    /* Try to get pixmap from bitmap file or default.
    */
    switch(    mBox->message_box.dialog_type    ) {
        case XmDIALOG_ERROR:
        {   fileName = "xm_error" ;
            defaultName = "default_xm_error" ;
            break ;
            }
        case XmDIALOG_INFORMATION:
        {   fileName = "xm_information" ;
            defaultName = "default_xm_information" ;
            break ;
            }
        case XmDIALOG_QUESTION:
        {   fileName = "xm_question" ;
            defaultName = "default_xm_question" ;
            break ;
            }
        case XmDIALOG_WARNING:
        {   fileName = "xm_warning" ;
            defaultName = "default_xm_warning" ;
            break ;
            }
        case XmDIALOG_WORKING:
        {   fileName = "xm_working" ;
            defaultName = "default_xm_working" ;
            break ;
            }
        default:
        {   fileName = NULL ;
            defaultName = NULL ;
            break ;
            }
        }

    if(    fileName    ){
	int depth ;

	if (_XmGetBitmapConversionModel(XtScreen((Widget)mBox))
	    == XmMATCH_DEPTH)
	    /* we want pixmap out of xbm, use the plain depth*/
	    depth = mBox->core.depth ;
	else
	    /* we want bitmap out of xbm, use the private convention */
	    depth = -mBox->core.depth ;


    /* use full color spec, so that one can take advantage of
	   symbolic shadow in the symbol pixmap */
	acc_color_rec.foreground = mBox->manager.foreground ;
	acc_color_rec.background = mBox->core.background_pixel ;
	acc_color_rec.top_shadow_color = mBox->manager.top_shadow_color ;
	acc_color_rec.bottom_shadow_color = mBox->manager.bottom_shadow_color ;
	acc_color_rec.highlight_color = mBox->manager.highlight_color ;
	acc_color_rec.select_color = XmUNSPECIFIED_PIXEL ;

	/* Get the label baseline, and scale the Pixmap accordingly */
	XtSetArg(arg[0], XmNrenderTable, &rt);
	XtSetArg(arg[1], XmNlabelString, &s);
	XtGetValues(mBox->message_box.message_wid, arg, 2);
	mBox->message_box.baseline = XmStringBaseline(rt, s);
	XmStringFree(s);

	tmpPix = _XmGetScaledPixmap(mBox->core.screen, (Widget)mBox, fileName,
	                            &acc_color_rec, depth, False, 0, 0,
	                            mBox->message_box.baseline * 2.5);
	if (tmpPix == XmUNSPECIFIED_PIXMAP) {
		tmpPix = _XmGetScaledPixmap(mBox->core.screen, (Widget)mBox, defaultName,
		                            &acc_color_rec, depth, False, 0, 0,
		                            mBox->message_box.baseline * 2.5);
	}
    }

    mBox->message_box.symbol_pixmap = tmpPix ;
    mBox->message_box.internal_pixmap = TRUE ;

    return ;
}
/****************************************************************/
