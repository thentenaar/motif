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

#ifndef _XmCombinationBox2P_h_
#define _XmCombinationBox2P_h_

#include <Xm/DropDownP.h>
#include <Xm/XmP.h>

#ifdef __cplusplus
extern "C" {
#endif

#define XmCombinationBox2_UP 		XmDropDown_UP
#define XmCombinationBox2_UNPOSTED	XmDropDown_UNPOSTED
#define XmCombinationBox2_DOWN        	XmDropDown_DOWN
#define XmCombinationBox2_POSTED	XmDropDown_POSTED
#define XmCombinationBox2_IN_PROGRESS 	XmDropDown_IN_PROGRESS
#define XmCombinationBox2_BEGIN_POPUP_FROM_TEXT \
  XmDropDown_BEGIN_POPUP_FROM_TEXT

#define XmComboBox2_h_space(w) (((XmCombinationBox2Widget)(w))->combo.h_space)
#define XmComboBox2_v_space(w) (((XmCombinationBox2Widget)(w))->combo.v_space)
#define XmComboBox2_popup_offset(w) (((XmCombinationBox2Widget)(w))->combo.popup_offset)
#define XmComboBox2_verify(w) (((XmCombinationBox2Widget)(w))->combo.verify)
#define XmComboBox2_editable(w) (((XmCombinationBox2Widget)(w))->combo.editable)
#define XmComboBox2_show_label(w) (((XmCombinationBox2Widget)(w))->combo.show_label)
#define XmComboBox2_customized_combo_box(w) (((XmCombinationBox2Widget)(w))->combo.customized_combo_box)
#define XmComboBox2_use_text_field(w) (((XmCombinationBox2Widget)(w))->combo.use_text_field)
#define XmComboBox2_popup_shell(w) (((XmCombinationBox2Widget)(w))->combo.popup_shell)
#define XmComboBox2_popup_cursor(w) (((XmCombinationBox2Widget)(w))->combo.popup_cursor)
#define XmComboBox2_translations(w) (((XmCombinationBox2Widget)(w))->combo.translations)
#define XmComboBox2_verify_text_callback(w) (((XmCombinationBox2Widget)(w))->combo.verify_text_callback)
#define XmComboBox2_verify_text_failed_callback(w) (((XmCombinationBox2Widget)(w))->combo.verify_text_failed_callback)
#define XmComboBox2_update_text_callback(w) (((XmCombinationBox2Widget)(w))->combo.update_text_callback)
#define XmComboBox2_update_shell_callback(w) (((XmCombinationBox2Widget)(w))->combo.update_shell_callback)
#define XmComboBox2_visible_items(w) (((XmCombinationBox2Widget)(w))->combo.visible_items)
#define XmComboBox2_new_visual_style(w) (((XmCombinationBox2Widget)(w))->combo.new_visual_style)

#define XmComboBox2_old_text(w) (((XmCombinationBox2Widget)(w))->combo.old_text)
#define XmComboBox2_focus_owner(w) (((XmCombinationBox2Widget)(w))->combo.focus_owner)
#define XmComboBox2_focus_state(w) (((XmCombinationBox2Widget)(w))->combo.focus_state)
#define XmComboBox2_list_state(w) (((XmCombinationBox2Widget)(w))->combo.list_state)
#define XmComboBox2_text_x(w) (((XmCombinationBox2Widget)(w))->combo.text_x)
#define XmComboBox2_list(w) (((XmCombinationBox2Widget)(w))->combo.list)
#define XmComboBox2_label(w) (((XmCombinationBox2Widget)(w))->combo.label)
#define XmComboBox2_text(w) (((XmCombinationBox2Widget)(w))->combo.text)
#define XmComboBox2_arrow(w) (((XmCombinationBox2Widget)(w))->combo.arrow)

#define XmComboBox2_autoTraversal(w) (((XmCombinationBox2Widget)(w))->combo.autoTraversal)
#define XmComboBox2_activateOnFill(w) (((XmCombinationBox2Widget)(w))->combo.activateOnFill)
#define XmComboBox2_doActivate(w) (((XmCombinationBox2Widget)(w))->combo.doActivate)
#define XmComboBox2_inValueChanged(w) (((XmCombinationBox2Widget)(w))->combo.inValueChanged)

/* Should return True to ignore invalid entry warning. Combination Box
 *  does not currently use this. Presumes do it in subclasses
 */
typedef Boolean (*XmCombinationBox2TextProc)(
    Widget,
    char *text
);

typedef Boolean (*XmCombinationBox2TextListMapProc)(
    Widget,		/* combo box */
    Widget,		/* text */
    Widget		/* list */
);

/* Version number for the first Revision  */
#define XmCombinationBox2ExtensionVersion 2

typedef XmDropDownClassPart XmCombinationBox2ClassPartExtension;
typedef XmDropDownClassPart XmCombinationBox2ClassPart;
typedef XmDropDownClassRec  XmCombinationBox2ClassRec;
typedef XmDropDownPart      XmCombinationBox2Part;
typedef XmDropDownRec       XmCombinationBox2Rec;
extern XmCombinationBox2ClassRec xmCombinationBox2ClassRec XM_DEPRECATED;

#ifdef __cplusplus
}	/* Closes scope of 'extern "C"' declaration */
#endif

#endif /* _XmCombinationBox2P_h_ */
