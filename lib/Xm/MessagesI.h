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
#ifndef _XmMessagesI_h
#define _XmMessagesI_h

#include <X11/Intrinsic.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The symbol _XmConst is used for constant data that cannot be
 * declared const in the header file because of usage as arguments to
 * routines which have string arguments that are not declared const.
 *
 * So, _XmConst is always defined to be nothing in header files.
 * In the source file, however, _XmConst is defined to be const,
 * so as to allow shared data in a shared library environment.
 */

#ifndef _XmConst
#define _XmConst
#endif


externalref _XmConst char *_XmMsgBaseClass_0000 ;
externalref _XmConst char *_XmMsgBaseClass_0001 ;
externalref _XmConst char *_XmMsgBaseClass_0002 ;
externalref _XmConst char *_XmMsgBulletinB_0001 ;
externalref _XmConst char *_XmMsgCascadeB_0000 ;
externalref _XmConst char *_XmMsgCascadeB_0001 ;
externalref _XmConst char *_XmMsgCascadeB_0002 ;
externalref _XmConst char *_XmMsgCascadeB_0003 ;
externalref _XmConst char *_XmMsgComboBox_0000 ;
externalref _XmConst char *_XmMsgComboBox_0001 ;
externalref _XmConst char *_XmMsgComboBox_0002 ;
externalref _XmConst char *_XmMsgComboBox_0003 ;
externalref _XmConst char *_XmMsgComboBox_0004 ;
externalref _XmConst char *_XmMsgComboBox_0005 ;
externalref _XmConst char *_XmMsgComboBox_0006 ;
externalref _XmConst char *_XmMsgComboBox_0007 ;
externalref _XmConst char *_XmMsgComboBox_0008 ;
externalref _XmConst char *_XmMsgComboBox_0009 ;
externalref _XmConst char *_XmMsgComboBox_0010 ;
externalref _XmConst char *_XmMsgComboBox_0011 ;
externalref _XmConst char *_XmMsgComboBox_0012 ;
externalref _XmConst char *_XmMsgComboBox_0013 ;
externalref _XmConst char *_XmMsgComboBox_0014 ;
externalref _XmConst char *_XmMsgCommand_0000 ;
externalref _XmConst char *_XmMsgCommand_0001 ;
externalref _XmConst char *_XmMsgCommand_0002 ;
externalref _XmConst char *_XmMsgCommand_0003 ;
externalref _XmConst char *_XmMsgCommand_0004 ;
externalref _XmConst char *_XmMsgCommand_0005 ;
externalref _XmConst char *_XmMsgContainer_0000 ;
externalref _XmConst char *_XmMsgContainer_0001 ;
externalref _XmConst char *_XmMsgCutPaste_0000 ;
externalref _XmConst char *_XmMsgCutPaste_0001 ;
externalref _XmConst char *_XmMsgCutPaste_0002 ;
externalref _XmConst char *_XmMsgCutPaste_0003 ;
externalref _XmConst char *_XmMsgCutPaste_0004 ;
externalref _XmConst char *_XmMsgCutPaste_0005 ;
externalref _XmConst char *_XmMsgCutPaste_0006 ;
externalref _XmConst char *_XmMsgCutPaste_0007 ;
externalref _XmConst char *_XmMsgCutPaste_0008 ;
externalref _XmConst char *_XmMsgCutPaste_0009 ;
externalref _XmConst char *_XmMsgDialogS_0000 ;
externalref _XmConst char *_XmMsgDragBS_0000 ;
externalref _XmConst char *_XmMsgDragBS_0001 ;
externalref _XmConst char *_XmMsgDragBS_0002 ;
externalref _XmConst char *_XmMsgDragBS_0003 ;
externalref _XmConst char *_XmMsgDragBS_0004 ;
externalref _XmConst char *_XmMsgDragBS_0005 ;
externalref _XmConst char *_XmMsgDragBS_0006 ;
externalref _XmConst char *_XmMsgDragICC_0000 ;
externalref _XmConst char *_XmMsgDragICC_0001 ;
externalref _XmConst char *_XmMsgDragIcon_0000 ;
externalref _XmConst char *_XmMsgDragIcon_0001 ;
externalref _XmConst char *_XmMsgDragIcon_0002 ;
externalref _XmConst char *_XmMsgDragOverS_0000 ;
externalref _XmConst char *_XmMsgDragOverS_0001 ;
externalref _XmConst char *_XmMsgDragOverS_0002 ;
externalref _XmConst char *_XmMsgDragOverS_0003 ;
externalref _XmConst char *_XmMsgDragUnder_0000 ;
externalref _XmConst char *_XmMsgDragUnder_0001 ;
externalref _XmConst char *_XmMsgForm_0000 ;
externalref _XmConst char *_XmMsgForm_0002 ;
externalref _XmConst char *_XmMsgForm_0003 ;
externalref _XmConst char *_XmMsgForm_0004 ;
externalref _XmConst char *_XmMsgGadget_0000 ;
externalref _XmConst char *_XmMsgGetSecRes_0000 ;
externalref _XmConst char *_XmMsgGrabS_0000 ;
externalref _XmConst char *_XmMsgLabel_0003 ;
externalref _XmConst char *_XmMsgLabel_0004 ;
externalref _XmConst char *_XmMsgList_0000 ;
externalref _XmConst char *_XmMsgList_0005 ;
externalref _XmConst char *_XmMsgList_0006 ;
externalref _XmConst char *_XmMsgList_0007 ;
externalref _XmConst char *_XmMsgList_0008 ;
externalref _XmConst char *_XmMsgList_0009 ;
externalref _XmConst char *_XmMsgList_0010 ;
externalref _XmConst char *_XmMsgList_0011 ;
externalref _XmConst char *_XmMsgList_0012 ;
externalref _XmConst char *_XmMsgList_0013 ;
externalref _XmConst char *_XmMsgList_0014 ;
externalref _XmConst char *_XmMsgList_0015 ;
externalref _XmConst char *_XmMsgMainW_0000 ;
externalref _XmConst char *_XmMsgMainW_0001 ;
externalref _XmConst char *_XmMsgManager_0000 ;
externalref _XmConst char *_XmMsgManager_0001 ;
externalref _XmConst char *_XmMsgMenuShell_0000 ;
externalref _XmConst char *_XmMsgMenuShell_0001 ;
externalref _XmConst char *_XmMsgMenuShell_0002 ;
externalref _XmConst char *_XmMsgMenuShell_0003 ;
externalref _XmConst char *_XmMsgMenuShell_0004 ;
externalref _XmConst char *_XmMsgMenuShell_0005 ;
externalref _XmConst char *_XmMsgMenuShell_0006 ;
externalref _XmConst char *_XmMsgMenuShell_0007 ;
externalref _XmConst char *_XmMsgMenuShell_0008 ;
externalref _XmConst char *_XmMsgMenuShell_0009 ;
externalref _XmConst char *_XmMsgMessageB_0003 ;
externalref _XmConst char *_XmMsgMessageB_0004 ;
externalref _XmConst char *_XmMsgNavigMap_0000 ;
externalref _XmConst char *_XmMsgNotebook_0000 ;
externalref _XmConst char *_XmMsgPanedW_0000 ;
externalref _XmConst char *_XmMsgPanedW_0001 ;
externalref _XmConst char *_XmMsgPanedW_0002 ;
externalref _XmConst char *_XmMsgPanedW_0003 ;
externalref _XmConst char *_XmMsgPanedW_0004 ;
externalref _XmConst char *_XmMsgPanedW_0005 ;
externalref _XmConst char *_XmMsgPrimitive_0000 ;
externalref _XmConst char *_XmMsgProtocols_0000 ;
externalref _XmConst char *_XmMsgProtocols_0001 ;
externalref _XmConst char *_XmMsgProtocols_0002 ;
externalref _XmConst char *_XmMsgRegion_0000 ;
externalref _XmConst char *_XmMsgResConvert_0000 ;
externalref _XmConst char *_XmMsgResConvert_0001 ;
externalref _XmConst char *_XmMsgResConvert_0002 ;
externalref _XmConst char *_XmMsgResConvert_0003 ;
externalref _XmConst char *_XmMsgResConvert_0004 ;
externalref _XmConst char *_XmMsgResConvert_0005 ;
externalref _XmConst char *_XmMsgResConvert_0006 ;
externalref _XmConst char *_XmMsgResConvert_0007 ;
externalref _XmConst char *_XmMsgResConvert_0008 ;
externalref _XmConst char *_XmMsgResConvert_0009 ;
externalref _XmConst char *_XmMsgResConvert_0010 ;
externalref _XmConst char *_XmMsgResConvert_0011 ;
externalref _XmConst char *_XmMsgResConvert_0012 ;
externalref _XmConst char *_XmMsgResConvert_0013 ;
externalref _XmConst char *_XmMsgResConvert_0014 ;
externalref _XmConst char *_XmMsgResConvert_0015 ;
externalref _XmConst char *_XmMsgResConvert_0016 ;
externalref _XmConst char *_XmMsgRowColumn_0000 ;
externalref _XmConst char *_XmMsgRowColumn_0001 ;
externalref _XmConst char *_XmMsgRowColumn_0002 ;
externalref _XmConst char *_XmMsgRowColumn_0003 ;
externalref _XmConst char *_XmMsgRowColumn_0004 ;
externalref _XmConst char *_XmMsgRowColumn_0005 ;
externalref _XmConst char *_XmMsgRowColumn_0007 ;
externalref _XmConst char *_XmMsgRowColumn_0008 ;
externalref _XmConst char *_XmMsgRowColumn_0015 ;
externalref _XmConst char *_XmMsgRowColumn_0016 ;
externalref _XmConst char *_XmMsgRowColumn_0017 ;
externalref _XmConst char *_XmMsgRowColumn_0018 ;
externalref _XmConst char *_XmMsgRowColumn_0019 ;
externalref _XmConst char *_XmMsgRowColumn_0020 ;
externalref _XmConst char *_XmMsgRowColumn_0022 ;
externalref _XmConst char *_XmMsgRowColumn_0023 ;
externalref _XmConst char *_XmMsgRowColText_0024 ;
externalref _XmConst char *_XmMsgRowColumn_0025 ;
externalref _XmConst char *_XmMsgRowColumn_0026 ;
externalref _XmConst char *_XmMsgRowColumn_0027 ;
externalref _XmConst char *_XmMsgScale_0000 ;
externalref _XmConst char *_XmMsgScale_0001 ;
externalref _XmConst char *_XmMsgScale_0002 ;
externalref _XmConst char *_XmMsgScaleScrBar_0004 ;
externalref _XmConst char *_XmMsgScale_0005 ;
externalref _XmConst char *_XmMsgScale_0006 ;
externalref _XmConst char *_XmMsgScale_0007 ;
externalref _XmConst char *_XmMsgScale_0008 ;
externalref _XmConst char *_XmMsgScale_0009 ;
externalref _XmConst char *_XmMsgScreen_0000 ;
externalref _XmConst char *_XmMsgScreen_0001 ;
externalref _XmConst char *_XmMsgColObj_0001 ;
externalref _XmConst char *_XmMsgColObj_0002 ;
externalref _XmConst char *_XmMsgScrollBar_0000 ;
externalref _XmConst char *_XmMsgScrollBar_0001 ;
externalref _XmConst char *_XmMsgScrollBar_0002 ;
externalref _XmConst char *_XmMsgScrollBar_0003 ;
externalref _XmConst char *_XmMsgScrollBar_0004 ;
externalref _XmConst char *_XmMsgScrollBar_0005 ;
externalref _XmConst char *_XmMsgScrollBar_0006 ;
externalref _XmConst char *_XmMsgScrollBar_0007 ;
externalref _XmConst char *_XmMsgScrollBar_0008 ;
externalref _XmConst char *_XmMsgScrolledW_0004 ;
externalref _XmConst char *_XmMsgScrolledW_0005 ;
externalref _XmConst char *_XmMsgScrolledW_0006 ;
externalref _XmConst char *_XmMsgScrolledW_0007 ;
externalref _XmConst char *_XmMsgScrolledW_0008 ;
externalref _XmConst char *_XmMsgScrolledW_0009 ;
externalref _XmConst char *_XmMsgScrollVis_0000 ;
externalref _XmConst char *_XmMsgSelectioB_0001 ;
externalref _XmConst char *_XmMsgSelectioB_0002 ;
externalref _XmConst char *_XmMsgSpinB_0001 ;
externalref _XmConst char *_XmMsgSpinB_0002 ;
externalref _XmConst char *_XmMsgSpinB_0003 ;
externalref _XmConst char *_XmMsgSpinB_0004 ;
externalref _XmConst char *_XmMsgSpinB_0005 ;
externalref _XmConst char *_XmMsgSpinB_0006 ;
externalref _XmConst char *_XmMsgSpinB_0007 ;
externalref _XmConst char *_XmMsgSpinB_0008 ;
externalref _XmConst char *_XmMsgSpinB_0009 ;
externalref _XmConst char *_XmMsgText_0000 ;
externalref _XmConst char *_XmMsgText_0002 ;
externalref _XmConst char *_XmMsgTextF_0000 ;
externalref _XmConst char *_XmMsgTextF_0001 ;
externalref _XmConst char *_XmMsgTextF_0002 ;
externalref _XmConst char *_XmMsgTextF_0003 ;
externalref _XmConst char *_XmMsgTextF_0004 ;
externalref _XmConst char *_XmMsgTextF_0005 ;
externalref _XmConst char *_XmMsgTextF_0006 ;
externalref _XmConst char *_XmMsgTextFWcs_0000 ;
externalref _XmConst char *_XmMsgTextFWcs_0001 ;
externalref _XmConst char *_XmMsgTextIn_0000 ;
externalref _XmConst char *_XmMsgTextOut_0000 ;
externalref _XmConst char *_XmMsgVendor_0000 ;
externalref _XmConst char *_XmMsgVendor_0001 ;
externalref _XmConst char *_XmMsgVendor_0002 ;
externalref _XmConst char *_XmMsgVendor_0003 ;
externalref _XmConst char *_XmMsgVendorE_0000 ;
externalref _XmConst char *_XmMsgVendorE_0005 ;
externalref _XmConst char *_XmMsgVisual_0000 ;
externalref _XmConst char *_XmMsgVisual_0001 ;
externalref _XmConst char *_XmMsgVisual_0002 ;
externalref _XmConst char *_XmMsgXmIm_0000 ;
externalref _XmConst char *_XmMsgResource_0001 ;
externalref _XmConst char *_XmMsgResource_0002 ;
externalref _XmConst char *_XmMsgResource_0003 ;
externalref _XmConst char *_XmMsgResource_0004 ;
externalref _XmConst char *_XmMsgResource_0005 ;
externalref _XmConst char *_XmMsgResource_0006 ;
externalref _XmConst char *_XmMsgResource_0007 ;
externalref _XmConst char *_XmMsgResource_0008 ;
externalref _XmConst char *_XmMsgResource_0009 ;
externalref _XmConst char *_XmMsgResource_0010 ;
externalref _XmConst char *_XmMsgResource_0011 ;
externalref _XmConst char *_XmMsgResource_0012 ;
externalref _XmConst char *_XmMsgResource_0013 ;
externalref _XmConst char *_XmMsgGeoUtils_0000 ;
externalref _XmConst char *_XmMsgGeoUtils_0001 ;
externalref _XmConst char *_XmMsgGeoUtils_0002 ;
externalref _XmConst char *_XmMsgDropSMgrI_0001 ;
externalref _XmConst char *_XmMsgDropSMgrI_0002 ;
externalref _XmConst char *_XmMsgDropSMgrI_0003 ;
externalref _XmConst char *_XmMsgDragC_0001 ;
externalref _XmConst char *_XmMsgDragC_0002 ;
externalref _XmConst char *_XmMsgDragC_0003 ;
externalref _XmConst char *_XmMsgDragC_0004 ;
externalref _XmConst char *_XmMsgDragC_0005 ;
externalref _XmConst char *_XmMsgDragC_0006 ;
externalref _XmConst char *_XmMsgDropSMgr_0001 ;
externalref _XmConst char *_XmMsgDropSMgr_0002 ;
externalref _XmConst char *_XmMsgDropSMgr_0003 ;
externalref _XmConst char *_XmMsgDropSMgr_0004 ;
externalref _XmConst char *_XmMsgDropSMgr_0005 ;
externalref _XmConst char *_XmMsgDropSMgr_0006 ;
externalref _XmConst char *_XmMsgDropSMgr_0007 ;
externalref _XmConst char *_XmMsgDropSMgr_0008 ;
externalref _XmConst char *_XmMsgDropSMgr_0009 ;
externalref _XmConst char *_XmMsgDropSMgr_0010 ;
externalref _XmConst char *_XmMsgDisplay_0001 ;
externalref _XmConst char *_XmMsgDisplay_0002 ;
externalref _XmConst char *_XmMsgDisplay_0003 ;
externalref _XmConst char *_XmMsgRepType_0000 ;
externalref _XmConst char *_XmMsgRepType_0001 ;
externalref _XmConst char *_XmMsgRepType_0002 ;
externalref _XmConst char *_XmMsgMotif_0000 ;
externalref _XmConst char *_XmMsgMotif_0001 ;
externalref _XmConst char *_XmMsgXmRenderT_0000 ;
externalref _XmConst char *_XmMsgXmRenderT_0001 ;
externalref _XmConst char *_XmMsgXmRenderT_0002 ;
externalref _XmConst char *_XmMsgXmRenderT_0003 ;
externalref _XmConst char *_XmMsgXmRenderT_0004 ;
externalref _XmConst char *_XmMsgXmRenderT_0005 ;
externalref _XmConst char *_XmMsgXmString_0000 ; 
externalref _XmConst char *_XmMsgXmTabList_0000 ; 
externalref _XmConst char *_XmMsgScrollFrameT_0000 ;
externalref _XmConst char *_XmMsgScrollFrameT_0001 ;
externalref _XmConst char *_XmMsgTransfer_0000 ;
externalref _XmConst char *_XmMsgTransfer_0001 ;
externalref _XmConst char *_XmMsgTransfer_0002 ;
externalref _XmConst char *_XmMsgTransfer_0003 ;
externalref _XmConst char *_XmMsgTransfer_0004 ;
externalref _XmConst char *_XmMsgTransfer_0005 ;
externalref _XmConst char *_XmMsgTransfer_0006 ;
externalref _XmConst char *_XmMsgTransfer_0007 ;
externalref _XmConst char *_XmMsgVaSimple_0000 ;
externalref _XmConst char *_XmMsgVaSimple_0001 ;
externalref _XmConst char *_XmMsgVaSimple_0002 ;
externalref _XmConst char *_XmMsgXmSelect_0000 ;
externalref _XmConst char *_XmMsgXmSelect_0001 ;
externalref _XmConst char *_XmMsgXmSelect_0002 ;
externalref _XmConst char *_XmMsgPixConv_0000 ;
externalref _XmConst char *_XmMsgSSpinB_0001 ;
externalref _XmConst char *_XmMsgSSpinB_0002 ;
externalref _XmConst char *_XmMsgSSpinB_0003 ;

externalref _XmConst char *XME_WARNING;
externalref _XmConst char *_XmMsgDataF_0000 ;
externalref _XmConst char *_XmMsgDataF_0001 ;
externalref _XmConst char *_XmMsgDataF_0002 ;
externalref _XmConst char *_XmMsgDataF_0003 ;
externalref _XmConst char *_XmMsgDataF_0004 ;
externalref _XmConst char *_XmMsgDataF_0005 ;
externalref _XmConst char *_XmMsgDataF_0006 ;
externalref _XmConst char *_XmMsgDataFWcs_0000 ;
externalref _XmConst char *_XmMsgDataFWcs_0001 ;

#include "XmMsgI.h"

#ifdef __cplusplus
}  /* Close scope of 'extern "C"' declaration which encloses file. */
#endif

#endif /* _XmMessagesI_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
