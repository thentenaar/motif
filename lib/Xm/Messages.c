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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: Messages.c /main/23 1997/03/19 11:49:58 dbl $"
#endif
#endif

#include <Xm/XmP.h>
#include "MessagesI.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcomment"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcomment"
#endif

#if 0				/* Comments below contain nested comments. */
/*===========================================================================*/
/* IMPORTANT NOTICE!! Please READ before editing this file!		     */
/*									     */
/* This file is also used to generate the XPG4 type message catalog.	     */
/* NO MESSAGES CAN BE REMOVED! If a message is no longer used, it can	     */
/* be removed from the object module by adding an #if 0 or /* above the      */
/* line(s) containing the message, and the corresponding #endif or 	     */
/* end-of-comment after the line(s).					     */
/*									     */
/* When ADDING messages to existing "sets" - a set is found inside /**** *****/
/* always add the new "externaldef(messages)" definition after everything    */
/* else in the set. Example:                                                 */
/*									     */
/* /**************** First.c ****************/
/*									     */
/* externaldef(messages) const char *_XmMsgFirst_0000 =		     */
/*    "Memory error";							     */
/*									     */
/* /* Needed for message catalog BC. Do not remove */
/* /***+MSG_First_1000 "Fake message."*/
/* /***+$ MSG_First_1000 message is obsolete - DO NOT localize this message.*/
/*									     */
/*									     */
/* /**************** Second.c ****************/
/*									     */
/* Messages added to the set "First" should be added after the line          */
/* /****+MSG_First_1000 "Fake message."*/
/* not after "Memory error".                                                 */
/*									     */
/* If you need to add another message "set", do so right before the          */
/* XME_WARNING definition.                                                   */
/*									     */
/*===========================================================================*/
#endif


/*
 * Text for message catalog header
 */
/***+$ This message catalog contains messages issued by Motif toolkit library.*/
/***+$ Consult X-Window systems, OSF/MOTIF Programmer's Guide and OSF/MOTIF*/
/***+$ Programmer's Reference Manual for technical terms if you have any*/
/***+$ doubts about their meanings.*/
/***+$ Do not translate variables such as %s, %d %s ... etc.*/
/***+$ Do not translate Motif resource name, such as XmNlabelType.*/
/***+$ Special terms with capital letter(s) should not be translated.*/
/***+$ "False" and "True" are keywords. Do not translate.*/
/***+ */
/***+ */

/**************** ArrowButton.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_ArrowButton_1000 The arrow direction is not correct.*/
/***+$ MSG_ArrowButton_1000 message is obsolete - DO NOT localize this message.*/

/**************** BulletinB.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_BulletinB_1000 Incorrect resize policy.*/
/***+$ MSG_BulletinB_1000 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgBulletinB_0001 =
   "Dialog style must be XmDIALOG_MODELESS.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_BulletinB_1001 Incorrect shadow type.*/
/***+$ MSG_BulletinB_1001 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_BulletinB_1002 Null font list (no VendorShell default)*/
/***+$ MSG_BulletinB_1002 message is obsolete - DO NOT localize this message.*/

/**************** CascadeB.c / CascadeBG.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_CascadeB_1000 XmCascadeButton must have correct type of \n\*/
/***+XmRowColumnWidgetClass parent.*/
/***+$ MSG_CascadeB_1000 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgCascadeB_0001 =
   "Only XmRowColumn widgets of type XmMENU_PULLDOWN can be submenus.";
/***+$ please do not translate submenus. */

externaldef(messages) const char *_XmMsgCascadeB_0002 =
   "XmNmappingDelay must be greater than or equal to 0.";

externaldef(messages) const char *_XmMsgCascadeB_0000 =
   "XmCascadeButton[Gadget] must have XmRowColumn parent with \n\
XmNrowColumnType XmMENU_PULLDOWN, XmMENU_POPUP, XmMENU_BAR or XmMENU_OPTION.";

externaldef(messages) const char *_XmMsgCascadeB_0003 =
   "XtGrabPointer failed.";

externaldef(messages) const char *_XmMsgRowColText_0024 =
   "XtGrabKeyboard failed.";

/**************** Command.c ****************/

externaldef(messages) const char *_XmMsgCommand_0000 =
   "The dialog type must be XmDIALOG_COMMAND.";

externaldef(messages) const char *_XmMsgCommand_0001 =
   "Invalid child type; Command widget does not have this child.";
/***+$ please do not translate Command.*/

externaldef(messages) const char *_XmMsgCommand_0002 =
   "NULL or empty XmString.";

externaldef(messages) const char *_XmMsgCommand_0003 =
   "NULL or empty XmString passed to XmCommandAppendValue.";

externaldef(messages) const char *_XmMsgCommand_0004 =
   "XmNmustMatch is always False for a Command widget.";
/***+$ please do not translate False or Command. */

externaldef(messages) const char *_XmMsgCommand_0005 =
   "XmNhistoryMaxItems must be a positive integer greater than zero.";

/**************** CutPaste.c ****************/

externaldef(messages) const char *_XmMsgCutPaste_0000 =
   "Must call XmClipboardStartCopy() before XmClipboardCopy()";

externaldef(messages) const char *_XmMsgCutPaste_0001 =
   "Must call XmClipboardStartCopy() before XmClipboardEndCopy()";

externaldef(messages) const char *_XmMsgCutPaste_0002 =
   "Too many formats in XmClipboardCopy()";

externaldef(messages) const char *_XmMsgCutPaste_0003 =
   "ClipboardBadDataType";
/***+$ please do not translate ClipboardBadDataType.*/

externaldef(messages) const char *_XmMsgCutPaste_0004 =
   "incorrect data type";

externaldef(messages) const char *_XmMsgCutPaste_0005 =
   "ClipboardCorrupt";
/***+$ please do not translate ClipboardCorrupt.*/

externaldef(messages) const char *_XmMsgCutPaste_0006 =
   "internal error - corrupt data structure";

externaldef(messages) const char *_XmMsgCutPaste_0007 =
   "ClipboardBadFormat";
/***+$ please do not translate ClipboardBadFormat.*/

externaldef(messages) const char *_XmMsgCutPaste_0008 =
   "Registered format length must be 8, 16, or 32";

externaldef(messages) const char *_XmMsgCutPaste_0009 =
   "Registered format name must not be null";


/**************** DialogS.c ****************/

externaldef(messages) const char *_XmMsgDialogS_0000 =
   "DialogShell widget supports only one RectObj child";
/***+$ please do not translate DialogShell and RectObj.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_DialogS_1000 gadgets aren't allowed in Shell*/
/***+$ MSG_DialogS_1000 message is obsolete - DO NOT localize this message.*/

/**************** DrawingA.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_DrawingA_1001 Margin width or height cannot be negative.*/
/***+$ MSG_DrawingA_1001 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_DrawingA_1002 Incorrect resize policy.*/
/***+$ MSG_DrawingA_1002 message is obsolete - DO NOT localize this message.*/

/**************** Form.c ****************/

externaldef(messages) const char *_XmMsgForm_0000 =
   "Fraction base cannot be zero.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_Form_1000 Incorrect Form attachment type.*/
/***+$ MSG_Form_1000 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Form_1001 Cannot set constraints for non-resizable widget.*/
/***+$ MSG_Form_1001 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Form_1002 Attachment widget must not be null.*/
/***+$ MSG_Form_1002 message is obsolete - DO NOT localize this message.*/
/***+$ please do not translate Attachment.*/

externaldef(messages) const char *_XmMsgForm_0002 =
   "Circular dependency in the children of the Form widget.\n\
Check for circular attachments between the children.";
/***+$ please do not translate Form.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Form_1006 Edge attached to a widget but no widget specified.*/
/***+$ MSG_Form_1006 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgForm_0003 =
   "Abandoned edge synchronization after 10000 iterations.\n\
Check for contradictory constraints on the children of this Form widget.";
/***+$ please do not translate Form.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Form_0004 Attachment widget must have same parent as widget.*/
/***+$ MSG_Form_0004 message is obsolete - DO NOT localize this message.*/
/***+$ please do not translate Attachment.*/

/**************** Frame.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Frame_1000 Only one child should be inserted in a Frame.*/
/***+$ MSG_Frame_1000 message is obsolete - DO NOT localize this message.*/
/***+$ please do not translate Frame.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Frame_1001 Invalid margin width.*/
/***+$ MSG_Frame_1001 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Frame_1002 Invalid margin height.*/
/***+$ MSG_Frame_1002 message is obsolete - DO NOT localize this message.*/

/**************** Gadget.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Gadget_1000 Invalid highlight thickness.*/
/***+$ MSG_Gadget_1000 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_Gadget_1001 The unit type is incorrect.*/
/***+$ MSG_Gadget_1001 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_Gadget_1002 Invalid shadow thickness.*/
/***+$ MSG_Gadget_1002 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_Gadget_1003 Cannot set pixmap resource to unspecified.*/
/***+$ MSG_Gadget_1003 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgGadget_0000 =
   "Cannot change XmNlayoutDirection after initialization.";


/**************** Label.c / LabelG.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Label_1001 Invalid XmNlabelType*/
/***+$ MSG_Label_1001 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_Label_1002 Invalid value in XmNalignment*/
/***+$ MSG_Label_1002 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_Label_1003 Invalid value in XmNstringDirection*/
/***+$ MSG_Label_1003 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgLabel_0003 =
   "XmNlabelString must be a Compound String.";
/***+$ please do not translate Compound String.*/

externaldef(messages) const char *_XmMsgLabel_0004 =
   "XmNacceleratorText must be a Compound String.";
/***+$ please do not translate Compound String.*/


/**************** List.c ****************/

externaldef(messages) const char *_XmMsgList_0000 =
   "When changed, XmNvisibleItemCount must be at least 1.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_List_1001 Invalid Selection Policy.*/
/***+$ MSG_List_1001 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_List_1002 Invalid Size Policy.*/
/***+$ MSG_List_1002 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_List_1003 Invalid ScrollBar Display Policy.*/
/***+$ MSG_List_1003 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_List_1004 Invalid String Direction.*/
/***+$ MSG_List_1004 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgList_0005 =
   "Cannot change XmNlistSizePolicy after initialization.";

externaldef(messages) const char *_XmMsgList_0006 =
   "When changed, XmNitemCount must be non-negative.";
/***+$ non-negative means zero or positive.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_List_1007 NULL font in SetValues ignored.*/
/***+$ MSG_List_1007 message is obsolete - DO NOT localize this message.*/
/***+$ please do not translate SetValues*/

externaldef(messages) const char *_XmMsgList_0007 =
   "Item(s) to be deleted are not present in the list.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_List_1009 No Horizontal Scrollbar to set.*/
/***+$ MSG_List_1009 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_List_1010 Invalid Margin setting.*/
/***+$ MSG_List_1010 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgList_0008 =
   "XmNlistSpacing must be non-negative.";

externaldef(messages) const char *_XmMsgList_0009 =
   "Cannot set XmNitems to NULL when XmNitemCount is positive.";

externaldef(messages) const char *_XmMsgList_0010 =
   "XmNselectedItemCount must not be negative.";

externaldef(messages) const char *_XmMsgList_0011 =
   "Cannot set XmNselectedItems to NULL when XmNselectedItemCount \
is positive.";

externaldef(messages) const char *_XmMsgList_0012 =
   "XmNtopItemPosition must be non-negative.";

externaldef(messages) const char *_XmMsgList_0013 =
   "XmNitems and XmNitemCount mismatch!";

/* Needed for message catalog BC. Do not remove */
/***+MSG_List_1017 Cannot leave add mode in multiple selection.*/
/***+$ MSG_List_1017 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgList_0014 =
   "XmNselectedPositionCount must not be negative.";

externaldef(messages) const char *_XmMsgList_0015 =
   "Cannot set XmNselectedPositions to NULL when XmNselectedPositionCount \
is positive.";

/**************** MainW.c ****************/

externaldef(messages) const char *_XmMsgMainW_0000 =
   "The MenuBar cannot be changed to NULL.";
/***+$ please do not translate MenuBar.*/

externaldef(messages) const char *_XmMsgMainW_0001 =
   "The CommandWindow cannot be changed to NULL.";
/***+$ please do not translate CommandWindow.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_MainW_1003 Negative margin value ignored.*/
/***+$ MSG_MainW_1003 message is obsolete - DO NOT localize this message.*/

/**************** MenuShell.c ****************/

externaldef(messages) const char *_XmMsgMenuShell_0000 =
   "MenuShell widgets accept only XmRowColumn children.";
/***+$ please do not translate MenuShell.*/

externaldef(messages) const char *_XmMsgMenuShell_0001 =
   "Attempting to manage a pulldown menu that is not attached \n\
to a Cascade button.";

externaldef(messages) const char *_XmMsgMenuShell_0002 =
   "XmPopup requires a subclass of shellWidgetClass.";
/***+$ please do not translate shellWidgetClass.*/

externaldef(messages) const char *_XmMsgMenuShell_0003 =
   "XmPopdown requires a subclass of shellWidgetClass.";
/***+$ please do not translate shellWidgetClass.*/

externaldef(messages) const char *_XmMsgMenuShell_0004 =
   "XtMenuPopup requires exactly one argument.";

externaldef(messages) const char *_XmMsgMenuShell_0005 =
   "XtMenuPopup only supports ButtonPress, KeyPress or EnterNotify events.";
/***+$ please do not translate ButtonPress, KeyPress or EnterNotify.*/

externaldef(messages) const char *_XmMsgMenuShell_0006 =
   "Cannot find popup widget \"%s\" in XtMenuPopup.";

externaldef(messages) const char *_XmMsgMenuShell_0007 =
   "Cannot find popup widget \"%s\" in XtMenuPopdown.";

externaldef(messages) const char *_XmMsgMenuShell_0008 =
   "XtMenuPopdown called with more than one argument.";

externaldef(messages) const char *_XmMsgMenuShell_0009 =
   "Cannot change XmNlayoutDirection after initialization.";


/**************** MessageB.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_MessageB_1001 Invalid Dialog Type.*/
/***+$ MSG_MessageB_1001 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_MessageB_1002 Invalid Default Button Type.*/
/***+$ MSG_MessageB_1002 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_MessageB_1003 Invalid Alignment Type.*/
/***+$ MSG_MessageB_1003 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgMessageB_0003 =
   "Invalid child type; widget does not have this child";

externaldef(messages) const char *_XmMsgMessageB_0004 =
   "Cancel button cannot be changed.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_MessageB_1006 Use XmNdefaultButtonType to set MessageBox default button.*/
/***+$ MSG_MessageB_1006 message is obsolete - DO NOT localize this message.*/


/**************** PanedW.c ****************/

externaldef(messages) const char *_XmMsgPanedW_0000 =
   "Minimum value must be greater than 0.";

externaldef(messages) const char *_XmMsgPanedW_0001 =
   "Maximum value must be greater than 0.";

externaldef(messages) const char *_XmMsgPanedW_0002 =
   "Minimum value must be less than maximum value.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_PanedW_0003 Constraints do not allow appropriate sizing.*/
/***+$ MSG_PanedW_0003 message is obsolete - DO NOT localize this message.*/


externaldef(messages) const char *_XmMsgPanedW_0004 =
   "Too few parameters in sash callback.";

externaldef(messages) const char *_XmMsgPanedW_0005 =
   "Invalid first parameter in sash callback.";

/**************** PWidget.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_PWI_1001 fontList is not defined*/
/***+$ MSG_PWI_1001 message is obsolete - DO NOT localize this message.*/
/***+$ please do not translate fontList.*/

/**************** Protocols.c ****************/

externaldef(messages) const char *_XmMsgProtocols_0000 =
   "Widget must be a VendorShell.";
/***+$ please do not translate VendorShell.*/

externaldef(messages) const char *_XmMsgProtocols_0001 =
   "Protocol manager already exists.";

externaldef(messages) const char *_XmMsgProtocols_0002 =
   "There are more protocols than widget can handle; 32 is the limit.";

/**************** PushB.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_PushB_1001 Not enough memory*/
/***+$ MSG_PushB_1001 message is obsolete - DO NOT localize this message.*/

/**************** RowColumn.c ****************/
/***+$ In this set, please do not translate the word that starts with "XmN" or "Xm". */

/* Needed for message catalog BC. Do not remove */
/***+MSG_RowColumn_1001 Attempt to set width to zero.\n\*/
/***+Set to default value 16.*/
/***+$ MSG_RowColumn_1001 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgRowColumn_0000 =
   "Attempt to set width to zero ignored";

/* Needed for message catalog BC. Do not remove */
/***+MSG_RowColumn_1003 Attempt to set height to zero.\n\*/
/***+Set to default value 16.*/
/***+$ MSG_RowColumn_1003 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgRowColumn_0001 =
   "Attempt to set height to zero ignored";

externaldef(messages) const char *_XmMsgRowColumn_0002 =
   "XmNhelpWidget forced to NULL since it is not used by popup menus.";

externaldef(messages) const char *_XmMsgRowColumn_0003 =
   "XmNhelpWidget forced to NULL since it is not used by pulldown menus.";

externaldef(messages) const char *_XmMsgRowColumn_0004 =
   "XmNhelpWidget forced to NULL since it is not used by option menus.";

externaldef(messages) const char *_XmMsgRowColumn_0005 =
   "XmNhelpWidget forced to NULL since it is not used by Work Areas.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_RowColumn_1009 Unknown value of XmNrowColumnType.\n\*/
/***+It is set to WorkArea.*/
/***+$ MSG_RowColumn_1009 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgRowColumn_0007 =
   "Widget hierarchy not appropriate for this XmNrowColumnType:\n\
defaulting to XmWORK_AREA.";

externaldef(messages) const char *_XmMsgRowColumn_0008 =
   "Attempt to change XmNrowColumnType after initialization ignored";

/* Needed for message catalog BC. Do not remove */
/***+MSG_RowColumn_1012 Unknown value of XmNorientation.\n\*/
/***+The default value is used.*/
/***+$ MSG_RowColumn_1012 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_RowColumn_1013 Attempt to set XmNorientation to unknown value.\n\*/
/***+The value is ignored.*/
/***+$ MSG_RowColumn_1013 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_RowColumn_1014 Unknown value of XmNpacking.\n\*/
/***+The default value is used.*/
/***+$ MSG_RowColumn_1014 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_RowColumn_1015 Attempt to set XmNpacking to unknown value.\n\*/
/***+The value is ignored.*/
/***+$ MSG_RowColumn_1015 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_RowColumn_1016 Unknown value of XmNentryAlignment.\n\*/
/***+The default value is used.*/
/***+$ MSG_RowColumn_1016 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_RowColumn_1017 Attempt to set XmNentryAlignment to unknown value.\n\*/
/***+The value is ignored.*/
/***+$ MSG_RowColumn_1017 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgRowColumn_0015 =
   "Attempt to set XmNisHomogenous to FALSE for a RowColumn widget of type \
XmMENU_BAR ignored.";
/***+$ please do not translate FALSE and RowColumn.*/

externaldef(messages) const char *_XmMsgRowColumn_0016 =
   "Attempt to change XmNentryClass for a RowColumn widget of type \
XmMENU_BAR ignored.";
/***+$ please do not translate RowColumn.*/

externaldef(messages) const char *_XmMsgRowColumn_0017 =
   "Attempt to change XmNwhichButton via XtSetValues for a RowColumn widget \
of type XmMENU_PULLDOWN ignored.";
/***+$ please do not translate RowColumn.*/

externaldef(messages) const char *_XmMsgRowColumn_0018 =
   "Attempt to change XmNmenuPost via XtSetValues for a RowColumn widget \
of type XmMENU_PULLDOWN ignored.";
/***+$ please do not translate RowColumn.*/

externaldef(messages) const char *_XmMsgRowColumn_0019 =
   "Attempt to set XmNmenuPost to an illegal value ignored.";

externaldef(messages) const char *_XmMsgRowColumn_0020 =
   "Attempt to change XmNshadowThickness for a RowColumn widget not of type \
XmMENU_PULLDOWN or XmMENU_POPUP ignored.";
/***+$ please do not translate RowColumn.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_RowColumn_1024 Attempt to change XmNorientation for a RowColumn \n\*/
/***+widget of type XmMENU_OPTION.\n\*/
/***+The value is ignored.*/
/***+$ MSG_RowColumn_1024 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgRowColumn_0022 =
   "Attempt to add wrong child type to a menu (that is, XmRowColumn) widget";
/***+$ please do not translate RowColumn.*/

externaldef(messages) const char *_XmMsgRowColumn_0023 =
   "Attempt to add wrong child type to a homogeneous RowColumn widget";
/***+$ please do not translate RowColumn.*/

externaldef(messages) const char *_XmMsgRowColumn_0025 =
   "Attempt to change XmNisHomogeneous for a RowColumn widget of type \
XmMENU_OPTION ignored";
/***+$ please do not translate RowColumn.*/

externaldef(messages) const char *_XmMsgRowColumn_0026 =
   "Enabling tear off on a shared menupane is not recommended";

externaldef(messages) const char *_XmMsgRowColumn_0027 =
   "Illegal mnemonic character;  Could not convert X KEYSYM to a keycode";
/***+$ please do not translate X KEYSYM.*/

/**************** Scale.c ****************/

externaldef(messages) const char *_XmMsgScale_0000 =
   "The minumum scale value is greater than or equal to the maximum scale \
value.";

externaldef(messages) const char *_XmMsgScale_0001 =
   "The specified scale value is less than the minimum scale value.";

externaldef(messages) const char *_XmMsgScale_0002 =
   "The specified scale value is greater than the maximum scale value.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_Scale_1004 Incorrect orientation.*/
/***+$ MSG_Scale_1004 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgScaleScrBar_0004 =
   "Incorrect processing direction.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_Scale_0005 = Invalid highlight thickness.*/
/***+$ MSG_Scale_0005 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgScale_0006 =
   "Invalid XmNscaleMultiple; greater than (max - min)";

externaldef(messages) const char *_XmMsgScale_0007 =
   "Invalid XmNscaleMultiple; less than zero";

externaldef(messages) const char *_XmMsgScale_0008 =
   "(Maximum - minimum) cannot be greater than INT_MAX / 2;\n\
minimum has been set to zero, maximum may have been set to (INT_MAX/2).";

externaldef(messages) const char *_XmMsgScale_0009 =
   "XmNshowValue has an incorrect value";

/**************** ScrollBar.c ****************/

externaldef(messages) const char *_XmMsgScrollBar_0000 =
   "The minimum scrollbar value is greater than or equal to\n\
the maximum scrollbar value.";

externaldef(messages) const char *_XmMsgScrollBar_0001 =
   "The specified slider size is less than 1.";

externaldef(messages) const char *_XmMsgScrollBar_0002 =
   "The specified scrollbar value is less than the minimum\n\
scrollbar value.";

externaldef(messages) const char *_XmMsgScrollBar_0003 =
   "The specified scrollbar value is greater than the maximum\n\
scrollbar value minus the scrollbar slider size.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_ScrollBar_1005 Incorrect orientation.*/
/***+$ MSG_ScrollBar_1005 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_ScrollBar_1006 Incorrect processing direction.*/
/***+$ MSG_ScrollBar_1006 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgScrollBar_0004 =
   "The scrollbar increment is less than 1.";

externaldef(messages) const char *_XmMsgScrollBar_0005 =
   "The scrollbar page increment is less than 1.";

externaldef(messages) const char *_XmMsgScrollBar_0006 =
   "The scrollbar initial delay is less than 1.";

externaldef(messages) const char *_XmMsgScrollBar_0007 =
   "The scrollbar repeat delay is less than 1.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_ScrollBar_1011 Error in context manager; scrollbar backgrounds\n\*/
/***+cannot be set correctly*/
/***+$ MSG_ScrollBar_1011 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_ScrollBar_1012 Error in context manager; scrollbar foregrounds\n\*/
/***+cannot be set correctly*/
/***+$ MSG_ScrollBar_1012 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgScrollBar_0008 =
   "Specified slider size is greater than the maximum scrollbar\n\
value minus the minimum scrollbar value.";


/**************** ScrolledW.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_ScrolledW_1001 Invalid ScrollBar Display policy.*/
/***+$ MSG_ScrolledW_1001 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_ScrolledW_1002 Invalid Scrolling Policy.*/
/***+$ MSG_ScrolledW_1002 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_ScrolledW_1003 Invalid Visual Policy.*/
/***+$ MSG_ScrolledW_1003 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_ScrolledW_1004 Invalid placement policy.*/
/***+$ MSG_ScrolledW_1004 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgScrolledW_0004 =
   "Cannot change scrolling policy after initialization.";

externaldef(messages) const char *_XmMsgScrolledW_0005 =
   "Cannot change visual policy after initialization.";

externaldef(messages) const char *_XmMsgScrolledW_0006 =
   "Cannot set AS_NEEDED scrollbar policy with a\n\
visual policy of VARIABLE.";
/***+$ please do not translate AS_NEEDED or VARIABLE.*/

externaldef(messages) const char *_XmMsgScrolledW_0007 =
   "Cannot change scrollbar widget in AUTOMATIC mode.";
/***+$ please do not translate AUTOMATIC.*/

externaldef(messages) const char *_XmMsgScrolledW_0008 =
   "Cannot change clip window";

externaldef(messages) const char *_XmMsgScrolledW_0009 =
   "Cannot set visual policy of CONSTANT in APPLICATION_DEFINED mode.";
/***+$ please do not translate CONSTANT or APPLICATION_DEFINED.*/

externaldef(messages) const char *_XmMsgScrollVis_0000 =
   "Wrong parameters passed to the XmScrollVisible function.";

/**************** SelectioB.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_SelectioB_1001 Incorrect dialog type.*/
/***+$ MSG_SelectioB_1001 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgSelectioB_0001 =
   "Dialog type cannot be modified.";
/***+$ please do not translate Dialog.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_SelectioB_1003 Only one work area child allowed.*/
/***+$ MSG_SelectioB_1003 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgSelectioB_0002 =
   "Widget does not support this child type.";

/**************** Separator.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Separator_1001 Invalid separator type.*/
/***+$ MSG_Separator_1001 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_Separator_1002 Invalid orientation.*/
/***+$ MSG_Separator_1002 message is obsolete - DO NOT localize this message.*/

/**************** Text.c ****************/

externaldef(messages) const char *_XmMsgText_0000 =
   "Incorrect source text is ignored.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_Text_1003 Invalid edit mode.*/
/***+$ MSG_Text_1003 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Text_0002 Text widget is editable; XmNtraversalOn must be True.*/
/***+$ MSG_Text_0002 message is obsolete - DO NOT localize this message.*/
/***+$ please do not translate XmNtraversalOn and True.*/

externaldef(messages) const char *_XmMsgTextIn_0000 =
   "Cannot find position while attempting to move to previous line.";

externaldef(messages) const char *_XmMsgTextOut_0000 =
   "Number of rows must be greater than 0.";

externaldef(messages) const char *_XmMsgTextF_0002 =
   "XmFontListInitFontContext failed.";
/***+$ please do not translate XmFontListInitFontContext.*/

externaldef(messages) const char *_XmMsgTextF_0003 =
   "XmFontListGetNextFont failed.";
/***+$ please do not translate XmFontListGetNextFont.*/

externaldef(messages) const char *_XmMsgTextF_0004 =
   "Character '%s' not supported in font.  Discarded.";

externaldef(messages) const char *_XmMsgTextFWcs_0000 =
   "Character '%s' not supported in font.  Discarded.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_TextFWcs_0001 Cannot use multibyte locale without a fontset.  Value discarded.*/
/***+$ MSG_TextFWcs_0001 message is obsolete - DO NOT localize this message.*/


/**************** TextF.c ****************/

externaldef(messages) const char *_XmMsgTextF_0000 =
   "Cursor position must be greater than or equal to 0.";

externaldef(messages) const char *_XmMsgTextF_0001 =
   "Number of columns must be greater than 0.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_TextF_0005 XmNtraversalOn must always be True.*/
/***+$ please do not translate XmNtraversalOn and True.*/
/***+$ MSG_TextF_0005 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgTextF_0006 =
   "Number of columns must be greater than or equal to 0.";

/**************** ToggleB.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_ToggleB_1001 Indicator type should be either XmONE_OF_MANY or XmN_OF_MANY*/
/***+$ MSG_ToggleB_1001 message is obsolete - DO NOT localize this message.*/

/**************** Traversal.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Traversal_1001 Invalid value for navigation_type*/
/***+$ MSG_Traversal_1001 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_Traversal_1002 Wrong value in old for navigation_type!!*/
/***+$ MSG_Traversal_1002 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_Traversal_1003 Traversal bootstrap situation with bad parameters*/
/***+$ MSG_Traversal_1003 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_Traversal_1004 Attempt to traverse to new tab using bad parameters*/
/***+$ MSG_Traversal_1004 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_Traversal_1005 startWidget is not in child list*/
/***+$ MSG_Traversal_1005 message is obsolete - DO NOT localize this message.*/
/***+$ please do not translate startWidget*/
/***+MSG_Traversal_1006 Bad parameters to TraverseToChild*/
/***+$ MSG_Traversal_1006 message is obsolete - DO NOT localize this message.*/
/***+$ please do not translate TraverseToChild.*/


/**************** Vendor.c ****************/

externaldef(messages) const char *_XmMsgVendor_0000 =
   "Invalid value for XmNdeleteResponse";

/* Needed for message catalog BC. Do not remove */
/***+MSG_Vendor_1002 Invalid XmNpreeditType, default to OverTheSpot*/
/***+$ MSG_Vendor_1002 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgVendor_0001 =
   "Invalid value for XmNinputPolicy";

externaldef(messages) const char *_XmMsgVendor_0002 =
   "XmNlayoutDirection cannot be changed.";

externaldef(messages) const char *_XmMsgVendor_0003 =
   "Fatal Error: \n\
_XmGetDefaultDisplay cannot be used prior to VendorS.Initialize, returns NULL";
/***+$ please do not translate VendorS.Initialize.*/

/**************** VendorE.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_VendorE_1001 FetchUnitType: bad widget class*/
/***+$ MSG_VendorE_1001 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_VendorE_0000 String to no-op conversion needs no extra arguments.*/
/***+$ MSG_VendorE_0000 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_VendorE_0005 FetchUnitType called without a widget to reference.*/
/***+$ please do not translate FetchUnitType.*/
/***+$ MSG_VendorE_0005 message is obsolete - DO NOT localize this message.*/


/**************** VirtKeys.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_VirtKey_1001 Virtual bindings Initialize hasn't been called*/
/***+$ MSG_VirtKey_1001 message is obsolete - DO NOT localize this message.*/

/**************** Visual.c ****************/

externaldef(messages) const char *_XmMsgVisual_0000 =
   "Invalid color requested from _XmAccessColorData.";

externaldef(messages) const char *_XmMsgVisual_0001 =
   "Cannot allocate colormap entry for default background.";

externaldef(messages) const char *_XmMsgVisual_0002 =
   "Cannot parse default background color specification.";


/**************** XmIm.c *******************/

externaldef(messages) const char *_XmMsgXmIm_0000 =
 "Cannot open input method - using XLookupString.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_XmIm_1002 Cannot create the Input Method Object*/
/***+$ MSG_XmIm_1002 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_XmIm_1003 XmIMFocus invoked with NULL widget.*/
/***+$ MSG_XmIm_1003 message is obsolete - DO NOT localize this message.*/
/* Needed for message catalog BC. Do not remove */
/***+MSG_XmIm_1004 XmIMMove invoked without the Input Method focus.*/
/***+$ MSG_XmIm_1004 message is obsolete - DO NOT localize this message.*/

/**************** Resources ************************/

externaldef(messages) const char *_XmMsgResource_0001 =
 "OK";
/***+$ "OK" as in "Go ahead"/"Confirmed to proceed as instructed"/"Fine"*/

externaldef(messages) const char *_XmMsgResource_0002 =
 "Cancel";
/***+$ ... as in "Never mind"/"abort this operation"/"Stop current request"*/

externaldef(messages) const char *_XmMsgResource_0003 =
 "Selection";
/***+$ ... as in "choosing from a list of items"*/

externaldef(messages) const char *_XmMsgResource_0004 =
 "Apply";
/***+$ ... as in "To do something with the things selected"*/

externaldef(messages) const char *_XmMsgResource_0005 =
 "Help";
/***+$ ... as in "Need more guidance on how to deal with this dialog box"*/

externaldef(messages) const char *_XmMsgResource_0006 =
 "Filter";
/***+$ ... the noun "Filter", as in "a sifter/a filter/a qualifier to screen out"*/
/***+$ files with*/

externaldef(messages) const char *_XmMsgResource_0007 =
 "Files";
/***+$ ... as in "a list of the files"*/

externaldef(messages) const char *_XmMsgResource_0008 =
 "Directories";
/***+$ ... as in "a list of the directories" */

externaldef(messages) const char *_XmMsgResource_0009 =
 "Items";
/***+$ ... as in "a list of things to choose from"*/

externaldef(messages) const char *_XmMsgResource_0010 =
 "Filter";
/***+$ ... the verb "Filter", as in "To screen out some files"*/
/***+$ Note this has a slightly different semantics from MSG_Res_1006.*/
/***+$ In English, it is the same word, but MSG_Res_1006 means "a sifter", and*/
/***+$ MSG_Res_1010 means "Push this button to filter out some files"*/
/***+$ MSG_Res_1006 is a label indicating "entry below is qualifier to filter out" */
/***+$ files with*/

externaldef(messages) const char *_XmMsgResource_0011 =
 "Directory";

externaldef(messages) const char *_XmMsgResource_0012 =
 ">";
/***+$ command-line prompt in the Command widget. */

externaldef(messages) const char *_XmMsgResource_0013 =
 "Find";

/*===========================================================================
 * New for 1.2
 *===========================================================================*/

/**************** BaseClass.c ****************/

externaldef(messages) const char *_XmMsgBaseClass_0000 =
   "No context found for extension";

externaldef(messages) const char *_XmMsgBaseClass_0001 =
   "_XmPopWidgetExtData; no extension found with XFindContext";

/* Needed for message catalog BC. Do not remove */
/***+MSG_BaseClass_0002 XmFreeWidgetExtData is an unsupported routine*/
/***+$ MSG_BaseClass_0002 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_GetSecRes_0000 getLabelSecResData: Not enough memory*/
/***+$ please do not translate getLabelSecResData.*/
/***+$ MSG_GetSecRes_0000 message is obsolete - DO NOT localize this message.*/

/*************** Display.c ***************/

externaldef(messages) const char *_XmMsgDisplay_0001 =
 "Creating multiple XmDisplays for the same X display.  Only the\n\
first XmDisplay created for a particular X display can be referenced\n\
by calls to XmGetXmDisplay";
/***+$ please do not translate XmDisplay or XmGetXmDisplay*/

externaldef(messages) const char *_XmMsgDisplay_0002 =
 "Received TOP_LEVEL_LEAVE with no active DragContext";
/***+$ please do not translate TOP_LEVEL_LEAVE or DragContext*/

externaldef(messages) const char *_XmMsgDisplay_0003 =
 "Cannot set XmDisplay class to a non-subclass of XmDisplay";
/***+$ please do not translate XmDisplay*/

/**************** DragBS.c ****************/

externaldef(messages) const char *_XmMsgDragBS_0000 =
   "_MOTIF_DRAG_WINDOW property has been destroyed";
/***+$ please do not translate _MOTIF_DRAG_WINDOW*/

externaldef(messages) const char *_XmMsgDragBS_0001 =
   "The protocol version levels do not match.";

externaldef(messages) const char *_XmMsgDragBS_0002 =
   "Unable to open display.";

externaldef(messages) const char *_XmMsgDragBS_0003 =
   "The atom table is empty.";

externaldef(messages) const char *_XmMsgDragBS_0004 =
   "The target table is empty.";

externaldef(messages) const char *_XmMsgDragBS_0005 =
   "The target table has an inconsistent property.";

externaldef(messages) const char *_XmMsgDragBS_0006 =
   "Invalid target table index.";


/*************** DragC.c *******************/

externaldef(messages) const char *_XmMsgDragC_0001 =
   "GenerateCallback does not expect XmCR_DROP_SITE_ENTER as a reason.";
/***+$ please do not translate GenerateCallback or XmCR_DROP_SITE_ENTER*/

externaldef(messages) const char *_XmMsgDragC_0002 =
   "Invalid selection in DropConvertCallback";
/***+$ please do not translate DropConvertCallback*/

externaldef(messages) const char *_XmMsgDragC_0003 =
   "The drop selection was lost.";

externaldef(messages) const char *_XmMsgDragC_0004 =
   "XGrabPointer failed.";

externaldef(messages) const char *_XmMsgDragC_0005 =
   "ExternalNotifyHandler: the callback reason is not acceptable.";
/***+$ please do not translate ExternalNotifyHandler*/

externaldef(messages) const char *_XmMsgDragC_0006 =
   "XmDragStart must be called as a result of a button press or motion event.";
/***+$ please do not translate XmDragStart*/

/**************** DragICC.c ****************/

externaldef(messages) const char *_XmMsgDragICC_0000 =
   "Unknown drag and drop message type.";

externaldef(messages) const char *_XmMsgDragICC_0001 =
   "The protocol version levels do not match.";


/**************** DragIcon.c ****************/

externaldef(messages) const char *_XmMsgDragIcon_0000 =
   "No geometry specified for dragIcon pixmap";
/***+$ please do not translate dragIcon*/

externaldef(messages) const char *_XmMsgDragIcon_0001 =
   "A dragIcon created with no pixmap";
/***+$ please do not translate dragIcon*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_DragIcon_0002 String to Bitmap converter needs Screen argument.*/
/***+$ MSG_DragIcon_0002 message is obsolete - DO NOT localize this message.*/


/**************** DragOverS.c ****************/

externaldef(messages) const char *_XmMsgDragOverS_0000 =
   "Depth mismatch.";

externaldef(messages) const char *_XmMsgDragOverS_0001 =
   "Unknown icon attachment.";

externaldef(messages) const char *_XmMsgDragOverS_0002 =
   "Unknown drag state.";

externaldef(messages) const char *_XmMsgDragOverS_0003 =
   "Unknown XmNblendModel.";
/***+$ please do not translate XmNblendModel */

/**************** DragUnder.c ****************/

externaldef(messages) const char *_XmMsgDragUnder_0000 =
   "Unable to get drop site window geometry.";

externaldef(messages) const char *_XmMsgDragUnder_0001 =
   "Invalid animationPixmapDepth.";
/***+$ please do not translate animationPixmapDepth*/


/*************** DropSMgr.c *******************/

externaldef(messages) const char *_XmMsgDropSMgr_0001 =
    "Cannot create drop sites that are children of a simple drop site.";

externaldef(messages) const char *_XmMsgDropSMgr_0002 =
    "Receiving motion events without an active drag context";

externaldef(messages) const char *_XmMsgDropSMgr_0003 =
    "Receiving operation changed without an active drag context.";

externaldef(messages) const char *_XmMsgDropSMgr_0004 =
    "Creating an active drop site with no drop procedure.";

externaldef(messages) const char *_XmMsgDropSMgr_0005 =
    "Cannot set rectangles or number of rectangles of composite drop sites.";

externaldef(messages) const char *_XmMsgDropSMgr_0006 =
    "Registering a widget as a drop site out of sequence.\n\
Ancestors must be registered before any of their \n\
descendants are registered.";

externaldef(messages) const char *_XmMsgDropSMgr_0007 =
    "Cannot register widget as a drop site more than once.";

externaldef(messages) const char *_XmMsgDropSMgr_0008 =
    "Drop site type may only be set at creation time.";
/***+$ please do not translate DropSite*/

externaldef(messages) const char *_XmMsgDropSMgr_0009 =
    "Cannot change rectangles of non-simple drop site.";

externaldef(messages) const char *_XmMsgDropSMgr_0010 =
    "Cannot register a Shell as a drop site.";

externaldef(messages) const char *_XmMsgDropSMgrI_0001 =
    "Cannot register a drop site that is a descendent of a simple drop site";

externaldef(messages) const char *_XmMsgDropSMgrI_0002 =
    "Cannot create a discontiguous child list for a composite drop site.";

externaldef(messages) const char *_XmMsgDropSMgrI_0003 =
    "%s is not a drop site child of %s";


/*************** GeoUtils.c *******************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_GeoUtils_0000 Failure of geometry request to \"almost\" reply".*/
/***+$ please do not translate "almost"*/
/***+$ MSG_GeoUtils_0000 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_GeoUtils_0001 Invalid layout of children found.*/
/***+$ MSG_GeoUtils_0001 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_GeoUtils_0002 Invalid order found in XmSelectionBox.*/
/***+$ MSG_GeoUtils_0002 message is obsolete - DO NOT localize this message.*/

/**************** Region.c ****************/

externaldef(messages) const char *_XmMsgRegion_0000 =
   "Memory error";


/*************** RepType.c ***************/

externaldef(messages) const char *_XmMsgRepType_0001 =
    "Illegal representation type id";

externaldef(messages) const char *_XmMsgRepType_0002 =
    "Illegal value (%d) for rep type XmR%s";
/***+$ please do not translate XmR%s*/

externaldef(messages) const char *_XmMsgRepType_0000 =
    "Reverse Conversion of ";

/**************** ResConvert.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_ResConvert_0000 FetchUnitType: bad widget class.*/
/***+$ please do not translate FetchUnitType.*/
/***+$ MSG_ResConvert_0000 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgResConvert_0001 =
   "Improperly defined default list! Exiting...";

externaldef(messages) const char *_XmMsgResConvert_0002 =
   "Missing colon in font string \"%s\"; any remaining fonts in list unparsed";

externaldef(messages) const char *_XmMsgResConvert_0003 =
   "Invalid delimiter in tag \"%s\"; any remaining fonts in list unparsed";

/* Needed for message catalog BC. Do not remove */
/***+MSG_ResConvert_0004 Unmatched quotation marks in string \"%s\"; any remaining fonts in list unparsed"*/
/***+$ MSG_ResConvert_0004 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgResConvert_0005 =
   "Unmatched quotation marks in tag \"%s\"; any remaining fonts in list unparsed";

externaldef(messages) const char *_XmMsgResConvert_0006 =
   "Null tag found when converting to type %s; any remaining fonts in list unparsed";

externaldef(messages) const char *_XmMsgResConvert_0007 =
   "Cannot convert XmString to Compound Text";

externaldef(messages) const char *_XmMsgResConvert_0008 =
   "Insufficient memory for XmbTextListToTextProperty";

externaldef(messages) const char *_XmMsgResConvert_0009 =
   "Locale not supported for XmbTextListToTextProperty";

externaldef(messages) const char *_XmMsgResConvert_0010 =
   "XmbTextListToTextProperty failed";

externaldef(messages) const char *_XmMsgResConvert_0011 =
   "Cannot convert widget name to Widget.";
/***+$ please do not translate Widget.*/

externaldef(messages) const char *_XmMsgResConvert_0012 =
   "Cannot convert compound text to XmString";

externaldef(messages) const char *_XmMsgResConvert_0013 =
   "Cannot convert XmString to compound text";

/* Needed for message catalog BC. Do not remove */
/***+MSG_ResConvert_0014 FetchUnitType called without a widget to reference.*/
/***+$ please do not translate FetchUnitType.*/
/***+$ MSG_ResConvert_0014 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_ResConvert_0015 FetchDisplayArg called without a widget to reference.*/
/***+$ please do not translate FetchDisplayArg.*/
/***+$ MSG_ResConvert_0015 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_ResConvert_0016 FetchWidgetArg called without a widget to reference.*/
/***+$ please do not translate FetchWidgetArg.*/
/***+$ MSG_ResConvert_0016 message is obsolete - DO NOT localize this message.*/



/**************** Screen.c ****************/

externaldef(messages) const char *_XmMsgScreen_0000 =
   "Icon screen mismatch";

externaldef(messages) const char *_XmMsgScreen_0001 =
  "Cannot get XmScreen because XmDisplay was not found.";


/*===========================================================================
 * New for CDE
 *===========================================================================*/

/**************** ColObj.c ****************/

/* This is really for ColorObj.c, but CDE 1.0 established the message */
/* set name.  We use their set name for backward compatibility. */

externaldef(messages) const char *_XmMsgColObj_0001 =
  "Could not allocate memory for color object data.";
externaldef(messages) const char *_XmMsgColObj_0002 =
  "Bad screen number from color server selection.";


/***+ */
/***+$set MS_IG*/
/***+ */
/***+MSG_IG_1 Incorrect alignment.*/
/***+$ MSG_IG_1 message is obsolete - DO NOT localize this message.*/
/***+MSG_IG_2 Incorrect behavior.*/
/***+$ MSG_IG_2 message is obsolete - DO NOT localize this message.*/
/***+MSG_IG_3 Incorrect fill mode.*/
/***+$ MSG_IG_3 message is obsolete - DO NOT localize this message.*/
/***+MSG_IG_4 Incorrect string or pixmap position.*/
/***+$ MSG_IG_4 message is obsolete - DO NOT localize this message.*/
/***+MSG_IG_5 Incorrect margin width or height.*/
/***+$ MSG_IG_5 message is obsolete - DO NOT localize this message.*/
/***+MSG_IG_6 Incorrect shadow type.*/
/***+$ MSG_IG_6 message is obsolete - DO NOT localize this message.*/

/*===========================================================================
 * New for 2.0
 *===========================================================================*/

/**************** ComboBox.c ****************/

externaldef(messages) const char *_XmMsgComboBox_0000 =
	"Applications cannot add children to XmComboBox widgets.";

externaldef(messages) const char *_XmMsgComboBox_0001 =
	"XmNcomboBoxType resource cannot be changed by XtSetValues.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_ComboBox_0002 XmFontListGetNextFont failed.*/
/***+$ MSG_ComboBox_0002 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_ComboBox_0003 XmFontListInitFontContext failed.*/
/***+$ MSG_ComboBox_0003 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgComboBox_0004 =
	"Internal widget has been destroyed.  Behavior is undefined.";

externaldef(messages) const char *_XmMsgComboBox_0005 =
	"Internal widget has been unmanaged.  Behavior is undefined.";

externaldef(messages) const char *_XmMsgComboBox_0006 =
	"XmQUICK_NAVIGATE is only valid for ComboBoxes of XmNcomboBoxType XmDROP_DOWN_LIST";

externaldef(messages) const char *_XmMsgComboBox_0007 =
	"Action invoked with the wrong number of parameters.";

externaldef(messages) const char *_XmMsgComboBox_0008 =
	"Action routine called from a widget that is not a descendant of ComboBox";

externaldef(messages) const char *_XmMsgComboBox_0009 =
	"XmComboBoxSelectItem called with an item not present in the ComboBox.";

externaldef(messages) const char *_XmMsgComboBox_0010 =
	"XmComboBoxSetItem called with an item not present in the ComboBox.";

externaldef(messages) const char *_XmMsgComboBox_0011 =
	"XmComboBoxDeletePos called with an invalid position.";

externaldef(messages) const char *_XmMsgComboBox_0012 =
	"XmComboBox utility routine called with an invalid widget.";

externaldef(messages) const char *_XmMsgComboBox_0013 =
	"Applications may not set the automatic XmComboBox widget children.";

externaldef(messages) const char *_XmMsgComboBox_0014 =
	"XmComboBox positionMode cannot be changed after creation.";
/***+$ please do not translate positionMode. */

/**************** Container.c ****************/

externaldef(messages) const char *_XmMsgContainer_0000 =
	"Action invoked with the wrong number of parameters.";

externaldef(messages) const char *_XmMsgContainer_0001 =
	"XmNdetailColumnHeading and XmNdetailColumnHeadingCount do not match!";

/**************** CSText.c ***************/

/***+MSG_CSText_0000 Invalid margin height; must be greater than or equal to 0.*/
/***+$ MSG_CSText_0000 message is obsolete - DO NOT localize this message.*/
/***+MSG_CSText_0001 Invalid margin width; must be greater than or equal to 0.*/
/***+$ MSG_CSText_0001 message is obsolete - DO NOT localize this message.*/
/***+MSG_CSText_0002 Invalid edit mode.*/
/***+$ MSG_CSText_0002 message is obsolete - DO NOT localize this message.*/
/***+MSG_CSText_0003 XmNtraversalOn must always be true.*/
/***+$ MSG_CSText_0003 message is obsolete - DO NOT localize this message.*/
/***+MSG_CSText_0004 Cannot change XmNscrollHorizontal after initialization.*/
/***+$ MSG_CSText_0004 message is obsolete - DO NOT localize this message.*/
/***+MSG_CSText_0005 Cannot change XmNscrollVertical after initialization.*/
/***+$ MSG_CSText_0005 message is obsolete - DO NOT localize this message.*/
/***+MSG_CSText_0006 Cannot change XmNscrollTopSide after initialization.*/
/***+$ MSG_CSText_0006 message is obsolete - DO NOT localize this message.*/
/***+MSG_CSText_0007 Cannot change XmNscrollLeftSide after initialization.*/
/***+$ MSG_CSText_0007 message is obsolete - DO NOT localize this message.*/

/**************** GrabShell.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_GrabS_0000 XmPopup requires a subclass of shellWidgetClass.*/
/***+$ please do not translate shellWidgetClass.*/
/***+$ MSG_GrabS_0000 message is obsolete - DO NOT localize this message.*/

/**************** Manager.c ****************/

externaldef(messages) const char *_XmMsgManager_0000 =
   "Widget class %s has invalid CompositeClassExtension record.";
/***+$ please do not translate CompositeClassExtension.*/

externaldef(messages) const char *_XmMsgManager_0001 =
   "Cannot change XmNlayoutDirection or XmNstringDirection after initialization.";


/**************** NavigMap.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_NavigMap_0000 _XmNavigate called with invalid direction.*/
/***+$ MSG_NavigMap_0000 message is obsolete - DO NOT localize this message.*/

/**************** Notebook.c ****************/

externaldef(messages) const char *_XmMsgNotebook_0000 =
   "XmNnotebookChildType resource cannot be set by XtSetValues.";

/**************** PixConv.c ****************/

externaldef(messages) const char *_XmMsgPixConv_0000 =
  "Wrong number of parameters for CvtStringToPixmap";
/***+$ please do not translate CvtStringToPixmap.*/

/**************** Primitive.c ****************/

externaldef(messages) const char *_XmMsgPrimitive_0000 =
   "Cannot change XmNlayoutDirection after initialization.";


/**************** ScrollFrameT.c ****************/

externaldef(messages) const char *_XmMsgScrollFrameT_0000 =
  "AssocNavigator requires a navigator trait";
/***+$ please do not translate AssocNavigator.*/

externaldef(messages) const char *_XmMsgScrollFrameT_0001 =
  "DeAssocNavigator requires a navigator trait";
/***+$ please do not translate DeAssocNavigator.*/

/**************** SpinB.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_SpinB_0001 Invalid value for XmNarrowLayout.*/
/***+$ MSG_SpinB_0001 message is obsolete - DO NOT localize this message.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_SpinB_0002 XmNminimumValue equals XmNmaximumValue.*/
/***+$ MSG_SpinB_0002 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgSpinB_0003 =
   "No items supplied for XmSTRING child.";

externaldef(messages) const char *_XmMsgSpinB_0004 =
   "XmNincrementValue cannot be 0. A value of 1 will be used.";

externaldef(messages) const char *_XmMsgSpinB_0005 =
   "Spin direction specified by XmNincrementValue\n\
has been reversed to match the specified\n\
XmNminimumValue and XmNmaximumValue.";

externaldef(messages) const char *_XmMsgSpinB_0006 =
    "XmNposition out of range; minimum XmNposition used.";

externaldef(messages) const char *_XmMsgSpinB_0007 =
    "XmNposition out of range; maximum XmNposition used.";

externaldef(messages) const char *_XmMsgSpinB_0008 =
   "Invalid value for XmNpositionType.  Using default value.";

/* Needed for message catalog BC. Do not remove */
/***+MSG_SpinB_0009 XmNpositionType resource can only be set at creation time.*/
/***+$ MSG_SpinB_0009 message is obsolete - DO NOT localize this message.*/

/**************** Transfer.c ****************/

externaldef(messages) const char *_XmMsgTransfer_0000 =
  "Calling SelectionCallbackWrapper when transfers should be finished";
/***+$ please do not translate SelectionCallbackWrapper.*/

/* Needed for message catalog BC. Do not remove */
/***+MSG_Transfer_0001 Cannot lock the clipboard; aborting transfer.*/
/***+$ MSG_Transfer_0001 message is obsolete - DO NOT localize this message.*/

externaldef(messages) const char *_XmMsgTransfer_0002 =
  "The format and type of the callback supplied data does not match the data being merged.";

externaldef(messages) const char *_XmMsgTransfer_0003 =
  "The status in the XmConvertCallbackStruct is not XmCONVERT_MERGE.";

externaldef(messages) const char *_XmMsgTransfer_0004 =
  "CONVERT_MORE is not yet supported.";
/***+$ please do not translate CONVERT_MORE.*/

externaldef(messages) const char *_XmMsgTransfer_0005 =
  "Bad atom value found.";

externaldef(messages) const char *_XmMsgTransfer_0006 =
  "Warning: Attempt to start a MULTIPLE transfer when one is in progress.";
/***+$ please do not translate MULTIPLE.*/

externaldef(messages) const char *_XmMsgTransfer_0007 =
  "Warning: Attempt to send a MULTIPLE transfer when one is not in progress.";
/***+$ please do not translate MULTIPLE.*/


/**************** VaSimple.c ****************/

externaldef(messages) const char *_XmMsgVaSimple_0000 =
  "XtVaTypedArg conversion needs non-null widget handle.";

externaldef(messages) const char *_XmMsgVaSimple_0001 =
  "Unable to find type of resource for conversion.";

externaldef(messages) const char *_XmMsgVaSimple_0002 =
  "Type conversion failed.";

/*************** Xm.c *******************/

externaldef(messages) const char *_XmMsgMotif_0000 =
    "\nName: %s\nClass: %s\n";

externaldef(messages) const char *_XmMsgMotif_0001 =
    "Action invoked with the wrong number of parameters.";


/*************** XmRenderT.c ************/

externaldef(messages) const char *_XmMsgXmRenderT_0000 =
  "XmNtag cannot be NULL.  Setting to empty string.";

externaldef(messages) const char *_XmMsgXmRenderT_0001 =
  "Display is NULL.  Cannot load font.";

externaldef(messages) const char *_XmMsgXmRenderT_0002 =
  "XmNfontType invalid.  Cannot load font.";

externaldef(messages) const char *_XmMsgXmRenderT_0003 =
  "Conversion failed.  Cannot load font.";

externaldef(messages) const char *_XmMsgXmRenderT_0004 =
  "XmNfontType set to XmAS_IS.  Cannot load font.";

externaldef(messages) const char *_XmMsgXmRenderT_0005 =
  "XmNloadModel is XmLOAD_IMMEDIATE but XmNfont and XmNfontName not specified.\n\
Cannot load font.";

/**************** XmSelect.c ****************/

/* Needed for message catalog BC. Do not remove */
/***+MSG_XmSelect_0000 Internal error: no selection property context for display.*/
/***+$ MSG_XmSelect_0000 message is obsolete - DO NOT localize this message.*/

/***+MSG_XmSelect_0001 Selection owner returned type INCR property with format != 32.*/
/***+$ please do not translate INCR.*/
/***+$ MSG_XmSelect_0001 message is obsolete - DO NOT localize this message.*/

/***+MSG_XmSelect_0002 XtGetSelectionRequest called for widget \"%s\" outside of ConvertSelection proc.*/
/***+$ please do not translate ConvertSelection.*/
/***+$ MSG_XmSelect_0002 message is obsolete - DO NOT localize this message.*/


/*************** XmString.c ************/

externaldef(messages) const char *_XmMsgXmString_0000 =
  "No font found.";

/*************** XmTabList.c ************/

externaldef(messages) const char *_XmMsgXmTabList_0000 =
  "Tab value cannot be negative.";

/**************** SSpinB.c ****************/

externaldef(messages) const char *_XmMsgSSpinB_0001 =
   "XmNtextField resource cannot be set.";

externaldef(messages) const char *_XmMsgSSpinB_0002 =
   "XmNpositionType resource can only be set at creation time.";

externaldef(messages) const char *_XmMsgSSpinB_0003 =
   "Item does not exist.  XmNposition is unchanged.";

/**************** DataF.c ****************/

externaldef(messages) const char *_XmMsgDataF_0000 =
   "Invalid cursor position, must be >= 0.";

externaldef(messages) const char *_XmMsgDataF_0001 =
   "Invalid columns, must be > 0.";

externaldef(messages) const char *_XmMsgDataF_0002 =
   "XmFontListInitFontContext Failed.";

externaldef(messages) const char *_XmMsgDataF_0003 =
   "XmFontListGetNextFont Failed.";

externaldef(messages) const char *_XmMsgDataF_0004 =
   "Character '%c', not supported in font.  Discarded.";

externaldef(messages) const char *_XmMsgDataF_0005 =
   "Traversal_on must always be true.";

externaldef(messages) const char *_XmMsgDataF_0006 =
   "Invalid columns, must be >= 0.";

externaldef(messages) const char *_XmMsgDataFWcs_0000 =
   "Character '%s', not supported in font.  Discarded.";

externaldef(messages) const char *_XmMsgDataFWcs_0001 =
   "Cannot use multibyte locale without a fontset.  Value discarded.";

/*
 * This one is not part of the message catalog
 */
externaldef(messages) const char *XME_WARNING="XmeWarning";

#if XM_MSGCAT
externaldef(messages) nl_catd Xm_catd = NULL;
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

