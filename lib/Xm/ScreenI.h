/**
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
 *
 */

/* $XConsortium: ScreenI.h /main/5 1995/07/13 17:53:44 drk $ */
#ifndef _XmScreenI_h
#define _XmScreenI_h

#include <Xm/ScreenP.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref XrmQuark _XmInvalidCursorIconQuark;
externalref XrmQuark _XmValidCursorIconQuark;
externalref XrmQuark _XmNoneCursorIconQuark;
externalref XrmQuark _XmDefaultDragIconQuark;
externalref XrmQuark _XmMoveCursorIconQuark;
externalref XrmQuark _XmCopyCursorIconQuark;
externalref XrmQuark _XmLinkCursorIconQuark;

/********    Private Function Declarations    ********/
extern XmDragIconObject _XmScreenGetOperationIcon(Widget w, unsigned char operation);
extern XmDragIconObject _XmScreenGetStateIcon(Widget w, unsigned char state);
extern XmDragIconObject _XmScreenGetSourceIcon(Widget w);
extern Pixmap _XmAllocScratchPixmap(XmScreen scr, Cardinal depth,
                                    Dimension width, Dimension height);
extern void _XmFreeScratchPixmap(XmScreen scr, Pixmap pixmap);
extern XmDragCursorCache * _XmGetDragCursorCachePtr(XmScreen scr);
extern Cursor _XmGetMenuCursorByScreen(Screen *screen);
extern Boolean _XmGetMoveOpaqueByScreen(Screen *screen);
extern unsigned char _XmGetUnpostBehavior(Widget w);
extern int _XmGetFontUnit(Screen *screen, int dimension);
extern void _XmScreenRemoveFromCursorCache(XmDragIconObject icon);
extern XmScreenColorProc _XmGetColorCalculationProc(Screen *screen);
extern XmAllocColorProc _XmGetColorAllocationProc(Screen *screen);
extern Pixmap _XmGetInsensitiveStippleBitmap(Widget w);
extern XtEnum _XmGetBitmapConversionModel(Screen *screen);

/********    End Private Function Declarations    ********/

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif
#endif /* _XmScreenI_h */
