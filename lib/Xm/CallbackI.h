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
#ifndef _XmCallbackI_h
#define _XmCallbackI_h

#include "XmI.h"

#ifdef __cplusplus
extern "C" {
#endif


#define _XtCBCalling		1
#define _XtCBFreeAfterCalling	2

/* The internal callback list is a little header followed by an array of
 * XtCallbackRec structs.  Declaring the first element of the array with 
 * the header allows the compiler to take care of any alignment required.
 */
typedef struct internalCallbackRec {
  unsigned short count;
  char           is_padded;	/* contains NULL padding for external form */
  char           call_state;	/* combination of _XtCB{FreeAfter}Calling */
  XtCallbackRec  callbacks;	/* first XtCallback record declared here */
} InternalCallbackRec, *InternalCallbackList;


/********    Private Function Declarations    ********/

extern void _XmAddCallback(InternalCallbackList *callbacks,
			   XtCallbackProc callback,
			   XtPointer closure);
extern void _XmRemoveCallback(InternalCallbackList *callbacks,
			      XtCallbackProc callback,
			      XtPointer closure);
extern void _XmCallCallbackList(Widget widget,
				XtCallbackList callbacks,
				XtPointer call_data);
extern void _XmRemoveAllCallbacks(InternalCallbackList *callbacks);

/********    End Private Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmCallbackI_h */
/* DON'T ADD STUFF AFTER THIS #endif */
