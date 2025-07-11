/* $TOG: UTMtransfer.h /main/6 1999/11/17 12:49:34 jff $ */
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

#ifndef _Transfer_h
#define _Transfer_h

#include <Xm/TransferP.h>

typedef struct _transferdatarec {
  Atom          target;
  Atom          type;
  unsigned long length;
  int           format;
  XtPointer     value;
} TransferDataRec, *TransferData;

typedef enum { Mprimary, Msecondary, Mclipboard } TransferMode;

extern TransferMode Tmode;

extern Widget MainW, list, target, labelV, labelN, typeOM;
extern Widget OwnPrimLG, OwnSecLG;


extern void ListSelectItemCB(Widget w, XtPointer client_data, 
			     XtPointer call_data);
extern void SetTypeCB(Widget w, XtPointer client_data, XtPointer call_data);
extern void TargetDestinationCB(Widget w, XtPointer ignore, 
				XtPointer call_data);
extern void TargetConvertCB(Widget w, XtPointer ignore, 
				XtPointer call_data);
char *GetSafeAtom(Display *display, Atom a);
char *GetStringFrom(XtEnum operation);


#endif /* _Transfer_h */

