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

#ifndef _XmDropTrans_h
#define _XmDropTrans_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XmTRANSFER_FAILURE 0
#define XmTRANSFER_SUCCESS 1

externalref WidgetClass xmDropTransferObjectClass;

typedef struct _XmDropTransferClassRec * XmDropTransferObjectClass;
typedef struct _XmDropTransferRec      * XmDropTransferObject;

#ifndef XmIsDropTransfer
#define XmIsDropTransfer(w) \
	XtIsSubclass((w), xmDropTransferObjectClass)
#endif /* XmIsDropTransfer */

typedef struct _XmDropTransferEntryRec {
	XtPointer	client_data;
	Atom		target;
} XmDropTransferEntryRec, * XmDropTransferEntry;

/********    Public Function Declarations    ********/

extern Widget XmDropTransferStart( 
                        Widget refWidget,
                        ArgList args,
                        Cardinal argCount) ;
extern void XmDropTransferAdd( 
                        Widget widget,
                        XmDropTransferEntry transfers,
                        Cardinal num_transfers) ;

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmDropTrans_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
