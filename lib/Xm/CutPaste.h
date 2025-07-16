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
#ifndef _XmCutPaste_h
#define _XmCutPaste_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

/* XmClipboard return status definitions */

typedef enum {
  XmClipboardFail = 0,
  XmClipboardSuccess = 1,
  XmClipboardTruncate = 2,
  XmClipboardLocked = 4,
  XmClipboardBadFormat = 5,
  XmClipboardNoData = 6
} XmClipboardStatus;

/* XmClipboard pre-1.2 definitions */

#define ClipboardFail     	0
#define ClipboardSuccess  	1
#define ClipboardTruncate 	2
#define ClipboardLocked   	4
#define ClipboardBadFormat   	5
#define ClipboardNoData   	6

typedef struct {
    long DataId;
    long PrivateId;
} XmClipboardPendingRec, *XmClipboardPendingList;

typedef void (*XmCutPasteProc)( Widget w, long * data_id, long * private_id,
							        int * reason) ;
typedef void (*VoidProc)( Widget w, int * data_id, int * private_id,
							        int * reason) ;


/********    Public Function Declarations    ********/

extern int XmClipboardBeginCopy(
                        Display *display,
                        Window window,
                        XmString label,
                        Widget widget,
                        VoidProc callback,
                        long *itemid) ;
extern int XmClipboardStartCopy(
                        Display *display,
                        Window window,
                        XmString label,
                        Time timestamp,
                        Widget widget,
                        XmCutPasteProc callback,
                        long *itemid) ;
extern int XmClipboardCopy(
                        Display *display,
                        Window window,
                        long itemid,
                        char *format,
                        XtPointer buffer,
                        unsigned long length,
                        long private_id,
                        long *dataid) ;
extern int XmClipboardEndCopy(
                        Display *display,
                        Window window,
                        long itemid) ;
extern int XmClipboardCancelCopy(
                        Display *display,
                        Window window,
                        long itemid) ;
extern int XmClipboardWithdrawFormat(
                        Display *display,
                        Window window,
                        long data) ;
extern int XmClipboardCopyByName(
                        Display *display,
                        Window window,
                        long data,
                        XtPointer buffer,
                        unsigned long length,
                        long private_id) ;
extern int XmClipboardUndoCopy(
                        Display *display,
                        Window window) ;
extern int XmClipboardLock(
                        Display *display,
                        Window window) ;
extern int XmClipboardUnlock(
                        Display *display,
                        Window window,
                        Boolean all_levels) ;
extern int XmClipboardStartRetrieve(
                        Display *display,
                        Window window,
                        Time timestamp) ;
extern int XmClipboardEndRetrieve(
                        Display *display,
                        Window window) ;
extern int XmClipboardRetrieve(
                        Display *display,
                        Window window,
                        char *format,
                        XtPointer buffer,
                        unsigned long length,
                        unsigned long *outlength,
                        long *private_id) ;
extern int XmClipboardInquireCount(
                        Display *display,
                        Window window,
                        int *count,
                        unsigned long *maxlength) ;
extern int XmClipboardInquireFormat(
                        Display *display,
                        Window window,
                        int n,
                        XtPointer buffer,
                        unsigned long bufferlength,
                        unsigned long *outlength) ;
extern int XmClipboardInquireLength(
                        Display *display,
                        Window window,
                        char *format,
                        unsigned long *length) ;
extern int XmClipboardInquirePendingItems(
                        Display *display,
                        Window window,
                        char *format,
                        XmClipboardPendingList *list,
                        unsigned long *count) ;
extern int XmClipboardRegisterFormat(
                        Display *display,
                        char *format_name,
                        int format_length) ;

/********    End Public Function Declarations    ********/


#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif
#endif /* _XmCutPaste_h */
