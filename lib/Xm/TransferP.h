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
/*
 * HISTORY
 */
/* $TOG: TransferP.h /main/6 1999/09/01 17:14:43 mgreess $ */
#ifndef _TransferP_H
#define _TransferP_H

#include <Xm/Transfer.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void XmeConvertMerge(XtPointer, Atom, int, unsigned long,
		     XmConvertCallbackStruct*);
extern Boolean XmePrimarySource(Widget, Time);
extern Boolean XmeNamedSource(Widget, Atom, Time);
extern Boolean XmeSecondarySource(Widget, Time);
extern void XmeSecondaryTransfer(Widget, Atom, XtEnum, Time);
extern Boolean XmeClipboardSource(Widget, XtEnum, Time);
extern Widget XmeDragSource(Widget, XtPointer, XEvent*, ArgList, Cardinal);

extern Boolean XmePrimarySink(Widget, XtEnum, XtPointer, Time);
extern Boolean XmeNamedSink(Widget, Atom, XtEnum, XtPointer, Time);
extern Boolean XmeSecondarySink(Widget, Time);
extern Boolean XmeClipboardSink(Widget, XtEnum, XtPointer);
extern void XmeDropSink(Widget, ArgList, Cardinal);

extern Atom *XmeStandardTargets(Widget, int, int*);
extern void XmeStandardConvert(Widget, XtPointer, XmConvertCallbackStruct*);
extern Atom XmeGetEncodingAtom(Widget);
extern void XmeTransferAddDoneProc(XtPointer,
				   XmSelectionFinishedProc);

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _TransferP_H */
