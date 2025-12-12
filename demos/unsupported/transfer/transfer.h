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

#ifdef REV_INFO
#ifndef lint
static char *rcsidtransferH = "$XConsortium: transfer.h /main/6 1995/07/14 09:47:34 drk $";
#endif
#endif

#ifndef _Transfer_h
#define _Transfer_h

typedef enum { Mprimary, Msecondary, Mclipboard, Mnamed } TransferMode;

#define _NAMED "NAMED_TRANSFER_ATOM"

extern TransferMode Tmode;

extern Widget editBox;
extern Widget toplevel, list, target, text1, text2, typeOM;
extern Widget op, omd, onamed, defer, text_display;
extern Widget format_om, format_s, format_l, format_t,
  format_cs, format_id, format_ih;

extern void targetDestinationCallback(Widget w, XtPointer ignore,
				      XmDestinationCallbackStruct *cs);
extern void targetConvertCallback(Widget w, XtPointer ignore,
				  XmConvertCallbackStruct *cs);
extern void inputCallback(Widget w, XtPointer ignore,
			  XmDrawingAreaCallbackStruct *cbstruct);
extern void ListSelectItem(Widget w, XtPointer ignore, 
			   XmListCallbackStruct *listcb);
extern void SetType(Widget w, char *in, XtPointer ignore);
extern void SetFormat(Widget, char, XtPointer);
extern void SetValue(Widget widget, char *ignore, 
		     XmAnyCallbackStruct *callback_data);
extern void RemoveValue(Widget widget, char *ignore, 
		     XmAnyCallbackStruct *callback_data);
extern void quitCB(Widget widget, char *ignore, 
		     XmAnyCallbackStruct *callback_data);
extern void setMode(Widget widget, int param, XmAnyCallbackStruct *);
extern void cut(Widget widget, char *ignore, XmAnyCallbackStruct *);
extern void copy(Widget widget, char *ignore, XmAnyCallbackStruct *);
extern void paste(Widget widget, char *ignore, XmAnyCallbackStruct *);
extern void pasteMove(Widget widget, char *ignore, XmAnyCallbackStruct *);
extern void deferCB(Widget widget, char *ignore,
		    XmToggleButtonCallbackStruct *callback_data);
extern char* GetSafeAtom(Display *display, Atom a);
extern char* ConvertXmStringToString(XmString);
extern void EditItem(Widget w, int param, XtPointer ignore);
extern void unmanageCB(Widget, Widget, XtPointer);
extern void manageCB(Widget, Widget, XtPointer);

typedef struct _transferdatarec {
  Atom		target;
  Atom		type;
  unsigned long	length;
  int		format;
  XtPointer	value;
  Boolean	deferred;
} TransferDataRec, *TransferData;



extern Widget create_mainWindow_widget(char * name,
				       Widget parent);
extern Widget create_editbox_widget(char * name,
				    Widget parent);

#endif /* _Transfer_h */
