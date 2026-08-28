/**
 * Motif
 *
 * Copyright (c) 2026 Tim Hentenaar
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
#ifndef XMFONTDIALOG_H
#define XMFONTDIALOG_H

#include <stdarg.h>
#include <X11/Intrinsic.h>

typedef enum {
	XmFONT_SOURCE_X,  /**< Core X fonts     */
	XmFONT_SOURCE_XFT /**< Xft / Fontconfig */
} XmFontDialogSource;

typedef struct _XmFontDialogClassRec *XmFontDialogWidgetClass;
typedef struct _XmFontDialogRec      *XmFontDialogWidget;
extern WidgetClass xmFontDialogWidgetClass;

Widget XmCreateFontDialog(Widget parent, char *name, ArgList args, Cardinal cnt);
Widget XmVaCreateFontDialog(Widget parent, char *name, ...);
Widget XmVaCreateManagedFontDialog(Widget parent, char *name, ...);

#endif /* XMFONTDIALOG_H */

