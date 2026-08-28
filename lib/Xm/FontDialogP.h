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
#ifndef XMFONTDIALOGP_H
#define XMFONTDIALOGP_H

#include <X11/IntrinsicP.h>
#include <Xm/BulletinBP.h>

typedef struct _XmFontDialogClassPart {
	XtPointer extension;
} XmFontDialogClassPart;

typedef struct _XmFontDialogClassRec {
	CoreClassPart            core_class;
	CompositeClassPart       composite_class;
	ConstraintClassPart      constraint_class;
	XmManagerClassPart       manager_class;
	XmBulletinBoardClassPart bulletin_board_class;
	XmFontDialogClassPart    fontdlg_class;
} XmFontDialogClassRec;

typedef struct _XmFontDialogPart {
	Widget font;
	Widget font_label;
	Widget style;
	Widget style_label;
	Widget size;
	Widget size_label;
	Widget sample;
	Widget sample_frame;
	Widget sample_label;
	Widget separator;
	Widget ok_button;
	Widget cancel_button;

	/* Callbacks */
	XtCallbackList ok_callback;
	XtCallbackList cancel_callback;

	/* Label strings */
	XmString font_string;
	XmString style_string;
	XmString size_string;
	XmString sample_title;
	XmString sample_text;
	XmString ok_string;
	XmString cancel_string;

	/* Font info */
	XtPointer info;
	XmRenderTable rend; /**< Current sample rendertable */

	/* Other props */
	unsigned char type;
	unsigned char source;
	int visible_item_count;
	int selected_font;
	int selected_style;
	int selected_size;
	Boolean prefer_100dpi;
} XmFontDialogPart;

typedef struct _XmFontDialogRec {
	CorePart core;
	CompositePart composite;
	ConstraintPart constraint;
	XmManagerPart manager;
	XmBulletinBoardPart bulletin_board;
	XmFontDialogPart fontdlg;
} XmFontDialogRec;

extern XmFontDialogClassRec xmFontDialogClassRec;

#endif /* XMFONTDIALOGP_H */

