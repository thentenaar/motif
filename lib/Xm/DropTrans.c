/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2.1 of the License, or (at your option)
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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: DropTrans.c /main/15 1996/05/02 13:50:19 pascale $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/DropTransP.h>
#include <Xm/DragCP.h>
#include "XmI.h"
#include "DragCI.h"
#include "DragICCI.h"
#include <Xm/AtomMgr.h>
#include <stdio.h>

/* Deactivated the fix, since it causes crash.
   For details see http://bugs.motifzone.net/long_list.cgi?buglist=1361
   
   #define CR1146
*/

#ifdef CR1146
static int isValidStartDropTimerId = 0;
#endif
/********    Static Function Declarations    ********/

static void ClassPartInit( 
                        WidgetClass wc) ;
static void Initialize( 
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static Boolean SetValues( 
                        Widget cw,
                        Widget rw,
                        Widget nw,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy( 
                        Widget w) ;
static void SourceNotifiedCB( 
                        Widget w,
                        XtPointer client_data,
                        Atom *selection,
                        Atom *type,
                        XtPointer value,
                        unsigned long *length,
                        int *format) ;
static void TerminateTransfer( 
                        XmDropTransferObject dt,
                        Atom *selection) ;
static void ProcessTransferEntry( 
                        XmDropTransferObject dt,
                        Cardinal which) ;
static void DropTransferSelectionCB( 
                        Widget w,
                        XtPointer client_data,
                        Atom *selection,
                        Atom *type,
                        XtPointer value,
                        unsigned long *length,
                        int *format) ;
static Widget StartDropTransfer( 
                        Widget refWidget,
                        ArgList args,
                        Cardinal argCount) ;
static void AddDropTransfer( 
                        Widget widget,
                        XmDropTransferEntry transfers,
                        Cardinal num_transfers) ;
static void DragContextDestroyCB(
			Widget widget,
			XtPointer client,
			XtPointer call) ;

/********    End Static Function Declarations    ********/


static XtResource resources[] = {
	{   XmNdropTransfers, XmCDropTransfers,
		XmRDropTransfers, sizeof(XmDropTransferEntry),
		XtOffsetOf( struct _XmDropTransferRec,
			dropTransfer.drop_transfers),
		XmRImmediate, (XtPointer) NULL
	},
	{   XmNnumDropTransfers, XmCNumDropTransfers,
		XmRCardinal, sizeof(Cardinal),
		XtOffsetOf( struct _XmDropTransferRec,
			dropTransfer.num_drop_transfers),
		XmRImmediate, (XtPointer) 0
	},
	{   XmNincremental, XmCIncremental, XmRBoolean, sizeof(Boolean),
		XtOffsetOf( struct _XmDropTransferRec, dropTransfer.incremental),
		XmRImmediate, (XtPointer) FALSE
	},
	{   XmNtransferProc, XmCTransferProc,
		XmRCallbackProc, sizeof(XtSelectionCallbackProc),
		XtOffsetOf( struct _XmDropTransferRec, dropTransfer.transfer_callback),
		XmRImmediate, (XtPointer) 0
	},
	{   XmNtransferStatus, XmCTransferStatus, XmRTransferStatus,
		sizeof(unsigned char),
		XtOffsetOf( struct _XmDropTransferRec, dropTransfer.transfer_status),
		XmRImmediate, (XtPointer) XmTRANSFER_SUCCESS
	},
};


/*  class record definition  */

externaldef(xmgadgetclassrec) XmDropTransferClassRec
	xmDropTransferClassRec = {
   {
      (WidgetClass) &objectClassRec,    /* superclass	         */	
      "XmDropTransfer",                 /* class_name	         */	
      sizeof(XmDropTransferRec),        /* widget_size	         */	
      NULL,                             /* class_initialize      */
      ClassPartInit,                    /* class part initialize */
      False,                  		/* class_inited          */	
      Initialize,                      	/* initialize	         */	
      NULL,                             /* initialize_hook       */
      NULL,	                       	/* obj1  	         */	
      NULL,				/* obj2                  */	
      0,				/* obj3	                 */	
      resources,                        /* resources	         */	
      XtNumber(resources),              /* num_resources         */	
      NULLQUARK,                        /* xrm_class	         */	
      True,                             /* obj4                  */
      XtExposeCompressSeries,           /* obj5                  */	
      True,                             /* obj6                  */
      False,                            /* obj7                  */
      Destroy,                          /* destroy               */	
      NULL,                             /* obj8                  */	
      NULL,				/* obj9                  */	
      SetValues,                        /* set_values	         */	
      NULL,                             /* set_values_hook       */
      NULL,         			/* obj10                 */
      NULL,                             /* get_values_hook       */
      NULL,                             /* obj11    	         */	
      XtVersion,                        /* version               */
      NULL,                             /* callback private      */
      NULL,                             /* obj12                 */
      NULL,                             /* obj13                 */
      NULL,				/* obj14                 */
      NULL,                             /* extension             */
   },
   {
      StartDropTransfer,           	/* start_drop_transfer   */
      AddDropTransfer,             	/* add_drop_transfer     */
      NULL,                        	/* extension             */
   },
};

externaldef(xmdroptransferobjectclass) WidgetClass
	xmDropTransferObjectClass = (WidgetClass)
		&xmDropTransferClassRec;

/*ARGSUSED*/
static void 
ClassPartInit(
        WidgetClass wc )
{
}

/*ARGSUSED*/
static void 
Initialize(
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
	XmDropTransferObject new_w = (XmDropTransferObject) nw;
	XmDropTransferPart *dtp =
		(XmDropTransferPart *) &(new_w->dropTransfer);

	if (dtp->num_drop_transfers != 0)
	{
		dtp->num_drop_transfer_lists = 1;
		dtp->drop_transfer_lists = (XmDropTransferList)
			XtMalloc(sizeof(XmDropTransferListRec) *
				dtp->num_drop_transfer_lists);
		dtp->drop_transfer_lists[0].transfer_list =
		        (XmDropTransferEntry)_XmAllocAndCopy(
			dtp->drop_transfers, sizeof(XmDropTransferEntryRec)
				* dtp->num_drop_transfers);
		dtp->drop_transfer_lists[0].num_transfers =
			dtp->num_drop_transfers;
		
		/* strictly for hygene... */

		dtp->drop_transfers = dtp->drop_transfer_lists[0].transfer_list;
	}
	else
	{
		dtp->drop_transfer_lists = NULL;
		dtp->num_drop_transfer_lists = 0;
	}

	dtp->motif_drop_atom = XInternAtom(
		XtDisplayOfObject(nw), XmS_MOTIF_DROP, False);

	dtp->cur_drop_transfer_list = (Cardinal) -1;
	dtp->cur_xfer = (Cardinal) -1;
	dtp->cur_targets = (Atom *) NULL;
	dtp->cur_client_data = (XtPointer *) NULL;
}

/*ARGSUSED*/
static Boolean 
SetValues(
        Widget cw,
        Widget rw,
        Widget nw,
        ArgList args,
        Cardinal *num_args )
{
	/* Stub */
	/* !!! Should disallow any changes !!! */
	return True;
}

static void 
Destroy(
        Widget w )
{
    XmDropTransferObject new_w = (XmDropTransferObject) w;
    Cardinal 		 i;
    XmDragContext	 dc;

    /*
     * clean up the hanging dragContext 
     */
    dc = (XmDragContext)XmGetDragContext((Widget)new_w, 
					 new_w->dropTransfer.timestamp);
    if (dc && dc->drag.sourceIsExternal)
      XtDestroyWidget((Widget)dc);

    for (i = 0; i < new_w->dropTransfer.num_drop_transfer_lists; i++)
      {
	  XmDropTransferList	currEntry =
	    &(new_w->dropTransfer.drop_transfer_lists[i]);
	  
	  XtFree((char *)currEntry->transfer_list);
      }
    XtFree((char *)new_w->dropTransfer.drop_transfer_lists);
}

/*ARGSUSED*/
static void 
SourceNotifiedCB(
        Widget w,
        XtPointer client_data,
        Atom *selection,
        Atom *type,
        XtPointer value,
        unsigned long *length,
        int *format )
{
    XmDropTransferObject	dt = (XmDropTransferObject)client_data;

    if (value != NULL)
      XtFree((char *) value);
    /* self-immolution aaaaaaiii */
    XtDestroyWidget((Widget)dt);
}

static void 
TerminateTransfer(
        XmDropTransferObject dt,
        Atom *selection )
{
	Atom status;
	XmDropTransferPart *dtp =
		(XmDropTransferPart *) &(dt->dropTransfer);
	XmDragContext	dc = (XmDragContext) dtp->dragContext;

	if (dtp->transfer_status == XmTRANSFER_SUCCESS)
		status = XInternAtom(XtDisplayOfObject((Widget)dt),
			XmSTRANSFER_SUCCESS, False);
	else /* XmTRANSFER_FAILURE */
		status = XInternAtom(XtDisplayOfObject((Widget)dt),
			XmSTRANSFER_FAILURE, False);
	
	/*
	 * we need to pass in the shell since that is the only widget
	 * visible to the initiator.
	 */

	XtGetSelectionValue(dc->drag.currReceiverInfo->shell,
				*selection, status,
				SourceNotifiedCB, (XtPointer)dt,
				dtp->timestamp);
}

static void 
ProcessTransferEntry(
        XmDropTransferObject dt,
        Cardinal which )
{
	XmDropTransferPart *dtp =
		(XmDropTransferPart *) &(dt->dropTransfer);
	XmDropTransferList	tl = &(dtp->drop_transfer_lists[which]);
	XmDragContext dc = (XmDragContext)dtp->dragContext;
	Cardinal i;
	Arg args[1];
	Atom real_selection_atom;

	dtp->cur_drop_transfer_list = which;
	dtp->cur_targets = (Atom *)
		XtMalloc((tl->num_transfers * sizeof(Atom)));
	dtp->cur_client_data = (XtPointer *)
		XtMalloc((tl->num_transfers * sizeof(XtPointer)));

	i = 0;
	XtSetArg(args[i], XmNiccHandle, &real_selection_atom); i++;
	XtGetValues(dtp->dragContext, args, i);

	for (i=0; i < tl->num_transfers; i++)
	{
		dtp->cur_targets[i] = tl->transfer_list[i].target;
		/* 
		 * all of the client data have to point to us so that we can
		 * bootstrap 
		 */
		dtp->cur_client_data[i] = (XtPointer)dt;
	}

	dtp->cur_xfer = 0;

	/*
	 * we need to pass in the destShell since that is the only widget
	 * visible to the initiator.
	 */

	/*
	 * As both an optimization and workaround for an Xt bug,
	 * if the number of transfers is just one then call the
	 * singular version of XtGetSelectionValue{,Incremental}.
	 * If there is only one item there is no need to call
	 * the plural version; XtGetSelectionValues
	 * Also there is a bug in some Xt's which fail to handle
	 * both MULTIPLE and INCR together correctly and can hang
	 * the drop transfer forever.  By adding this special case
	 * we allow knowledgeable clients to request any target that
	 * might be large enough to trigger an INCR as a single item
	 * that would avoid the MULTIPLE case.
	 */

	if (dtp->incremental)
	{
           if (tl->num_transfers == 1) {
               XtGetSelectionValueIncremental(
                       dc->drag.currReceiverInfo->shell,
                       real_selection_atom, dtp->cur_targets[0],
                       DropTransferSelectionCB,
                       (XtPointer)dtp->cur_client_data[0],
                       dtp->timestamp);
           } else {

		XtGetSelectionValuesIncremental(
			dc->drag.currReceiverInfo->shell,
			real_selection_atom, dtp->cur_targets,
			tl->num_transfers, DropTransferSelectionCB,
			(XtPointer *)dtp->cur_client_data, 
			dtp->timestamp);
            }
	}
	else
	{
           if (tl->num_transfers == 1) {
               XtGetSelectionValue(dc->drag.currReceiverInfo->shell,
                       real_selection_atom, dtp->cur_targets[0],
                       DropTransferSelectionCB,
                       (XtPointer)dtp->cur_client_data[0],
                       dtp->timestamp);
           } else {
		XtGetSelectionValues(dc->drag.currReceiverInfo->shell,
			real_selection_atom, dtp->cur_targets,
			tl->num_transfers, DropTransferSelectionCB,
			(XtPointer *)dtp->cur_client_data, 
			dtp->timestamp);
           }
	}
}


/* 
 * This routine is called with the shell as the widget and us as the
 * client data. We can't pass ourselves since we're not windowed
 */
/*ARGSUSED*/
static void 
DropTransferSelectionCB(
        Widget w,
        XtPointer client_data,
        Atom *selection,
        Atom *type,
        XtPointer value,
        unsigned long *length,
        int *format )
{
	XmDropTransferObject dt = (XmDropTransferObject) client_data;
	XmDropTransferPart *dtp =
		(XmDropTransferPart *) &(dt->dropTransfer);
	XmDropTransferList	tl =
		&(dtp->drop_transfer_lists[dtp->cur_drop_transfer_list]);


	(*(dtp->transfer_callback)) 
		((Widget)dt, tl->transfer_list[dtp->cur_xfer].client_data,
			selection, type, value, length, format);

       /* The transfer list needs to be reassigned at this point in case
	* an XmDropTransferAdd() was called in the callback.
        */
	tl = &(dtp->drop_transfer_lists[dtp->cur_drop_transfer_list]);

	if ( !(dtp->incremental)
		||
		((dtp->incremental) && (value != NULL) && (*length == 0)))
	{
		dtp->cur_xfer++;

		if (dtp->cur_xfer == tl->num_transfers)
		{
			XtFree((char *)dtp->cur_targets);
			XtFree((char *)dtp->cur_client_data);

			if (++(dtp->cur_drop_transfer_list) <
				dtp->num_drop_transfer_lists)
			{
			/* Get the next transfer entry in the list and process it */
				ProcessTransferEntry(dt, dtp->cur_drop_transfer_list);
			}
			else /* notify the source that we're done */
				TerminateTransfer(dt, selection);
		}
	}
}


/*ARGSUSED*/
static void 
StartDropTimer(
        XtPointer clientData,
        XtIntervalId *id )
{
	XmDropTransferObject dt = (XmDropTransferObject)clientData;
	XmDropTransferPart *dtp;
	Arg my_args[1];
	int i;
	Atom selection;

#ifdef CR1146
    isValidStartDropTimerId = 0;
#endif

	
	dtp = (XmDropTransferPart *) &(dt->dropTransfer);

	if (dtp->num_drop_transfer_lists)
	{
		/* Get the first transfer entry in the list and process it */
		ProcessTransferEntry(dt, 0);
	}
	else /* notify the source that we've changed our mind */
	{
		i = 0;
		XtSetArg(my_args[i], XmNiccHandle, &selection); i++;
		XtGetValues(dtp->dragContext, my_args, i);

		TerminateTransfer(dt, &selection);
	}
}

static void
DragContextDestroyCB(
        Widget widget,
        XtPointer client,
        XtPointer call)
{
        XtIntervalId timer = (XtIntervalId) client;
#ifdef CR1146
        if (!isValidStartDropTimerId) return;
#endif
        XtRemoveTimeOut(timer);
}

static Widget 
StartDropTransfer(
        Widget refWidget,
        ArgList args,
        Cardinal argCount )
{
	static int which = 0;
	XmDropTransferObject dt;
	char buf[30];
	XtIntervalId timer;

	_XmProcessLock();
	sprintf(buf, "Transfer%d", which++);
	_XmProcessUnlock();
	dt = (XmDropTransferObject) XtCreateWidget(buf,
		xmDropTransferObjectClass,
		(Widget) XmGetXmDisplay(XtDisplayOfObject(refWidget)),
		args, argCount);

	dt->dropTransfer.dragContext = refWidget;
	dt->dropTransfer.timestamp =
	  ((XmDragContext)refWidget)->drag.dragFinishTime;

	/*
	 * The processing of the dropTransfer should happen after the
	 * dropStart message is echoed to the initiator. Since we're
	 * being called out of the dropProc of the receiver we can't
	 * just proceed or else the entire transfer could happen
	 * inline. We therefore add ourselves as a zero length timer
	 * which will allow us to get called after the dropProc has
	 * returned. 
 	 * To prevent against this code's being called twice by disparate
 	 * parts of the drag/drop code (especially on failure) self-guard by
 	 * watching for destruction of the widget and avoid the rest of the
 	 * operations.
 	 */

	timer = XtAppAddTimeOut(XtWidgetToApplicationContext( (Widget)dt),
				0, StartDropTimer, (XtPointer)dt);
#ifdef CR1146
    if (isValidStartDropTimerId) {
        printf("Motif Error in file %s at line %d: A DropTimer is already active,\n", __FILE__, __LINE__);
    }
    isValidStartDropTimerId = 1;
#endif
	XtAddCallback(refWidget, XmNdestroyCallback, DragContextDestroyCB, 
		      (XtPointer)timer);

	return((Widget) dt);
}



Widget 
XmDropTransferStart(
        Widget refWidget,
        ArgList args,
        Cardinal argCount )
{
	Widget ddo = (Widget) XmGetXmDisplay(XtDisplayOfObject(refWidget));
	XmDropTransferObjectClass wc;
	Arg lclArgs[1];
	int n;
	Widget ret_val;
	_XmWidgetToAppContext(refWidget);

	_XmAppLock(app);
	n = 0;
	XtSetArg(lclArgs[n], XmNdropTransferClass, &wc); n++;
	XtGetValues(ddo, lclArgs, n);

	ret_val = (*(wc->dropTransfer_class.start_drop_transfer))
		(refWidget, args, argCount);
	_XmAppUnlock(app);
	return ret_val;
}



static void 
AddDropTransfer(
        Widget widget,
        XmDropTransferEntry transfers,
        Cardinal num_transfers )
{
        XmDropTransferObject dto = (XmDropTransferObject) widget;
	XmDropTransferPart *dtp =
		(XmDropTransferPart *) &(dto->dropTransfer);
	Cardinal index = dtp->num_drop_transfer_lists++;

	dtp->drop_transfer_lists = (XmDropTransferList)
		XtRealloc((char *)dtp->drop_transfer_lists,
			sizeof(XmDropTransferListRec) *
			dtp->num_drop_transfer_lists);
	dtp->drop_transfer_lists[index].transfer_list =
	        (XmDropTransferEntry)_XmAllocAndCopy(
		transfers, sizeof(XmDropTransferEntryRec) * num_transfers);
	dtp->drop_transfer_lists[index].num_transfers = num_transfers;
}


void 
XmDropTransferAdd(
        Widget widget,
        XmDropTransferEntry transfers,
        Cardinal num_transfers )
{
	XmDropTransferObject dt = (XmDropTransferObject) widget;
	XmDropTransferObjectClass wc;
	_XmWidgetToAppContext(widget);

	_XmAppLock(app);
	wc = (XmDropTransferObjectClass) XtClass(dt);
	((*(wc->dropTransfer_class.add_drop_transfer))
		(widget, transfers, num_transfers));
	_XmAppUnlock(app);
}


