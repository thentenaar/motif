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

#ifndef _XmDropTransferP_h
#define _XmDropTransferP_h

#include <Xm/DropTrans.h>
#include <Xm/XmP.h>


#ifdef __cplusplus
extern "C" {
#endif

/*  DropTransfer class structure  */

typedef Widget (*XmDropTransferStartTransferProc)(Widget,
	ArgList, Cardinal);
typedef void (*XmDropTransferAddTransferProc)(Widget,
	XmDropTransferEntry, Cardinal);

typedef struct _XmDropTransferClassPart
{
	XmDropTransferStartTransferProc	start_drop_transfer;
	XmDropTransferAddTransferProc	add_drop_transfer;
	XtPointer extension;
} XmDropTransferClassPart;

/*  Full class record declaration */

typedef struct _XmDropTransferClassRec
{
   ObjectClassPart        object_class;
   XmDropTransferClassPart dropTransfer_class;
} XmDropTransferClassRec;

externalref XmDropTransferClassRec xmDropTransferClassRec;


typedef struct _XmDropTransferListRec {
	XmDropTransferEntry	transfer_list;
	Cardinal		num_transfers;
} XmDropTransferListRec, * XmDropTransferList;


/*  The DropTransfer instance record  */

typedef struct _XmDropTransferPart
{
    XmDropTransferEntry		drop_transfers;
    Cardinal			num_drop_transfers;
    Atom			selection;
    Widget			dragContext;
    Time			timestamp;
    Boolean			incremental;
    Window			source_window;
    unsigned int		tag;
    XtSelectionCallbackProc 	transfer_callback;
    unsigned char		transfer_status;

    Atom 			motif_drop_atom;
    
    XmDropTransferList		drop_transfer_lists;
    Cardinal			num_drop_transfer_lists;
    Cardinal			cur_drop_transfer_list;
    Cardinal			cur_xfer;
    Atom *			cur_targets;
    XtPointer *			cur_client_data;
} XmDropTransferPart;

/*  Full instance record declaration  */

typedef struct _XmDropTransferRec
{
	ObjectPart	object;
	XmDropTransferPart dropTransfer;
} XmDropTransferRec;


/********    Private Function Declarations    ********/


/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDropTransferP_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
