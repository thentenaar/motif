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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: RepType.c /main/17 1997/09/15 10:10:39 cshi $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "RepTypeI.h"
#include "MessagesI.h"
#include <Xm/XmosP.h>		/* for (indirectly) atoi() */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "XmI.h"

#define MESSAGE0 _XmMMsgRepType_0000
#define MESSAGE1 _XmMMsgRepType_0001
#define MESSAGE2 _XmMMsgRepType_0002

/* INSTRUCTIONS to add a statically-stored representation type:
 *    (For dynamically allocated/created representation types, see the
 *     man page for XmRepTypeRegister).
 *
 *  1) Determine whether or not the numerical values of the representation
 *     type can be enumerated with consecutive numerical values beginning
 *     with value zero.  If this is the case, continue with step 2).
 *
 *     If this is not the case, the representation type needs an extra
 *     array in the data structure to map the numerical resource value to
 *     the array position of the value name in the representation type data
 *     structures.  If the representation type must be mapped in this way,
 *     go to step 2M).
 *
 *  2) Define a static array of the names of the values for the
 *     representation type in RepType.c.  Use the representation type name,
 *     plus the suffix "Names" for the name of the array (see existing name
 *     arrays for an example).  The ordering of the value names in this
 *     array determines the numerical value of each name, beginning with
 *     zero and incrementing consecutively.
 *
 *  3) Add an enumeration symbol for the ID number of the representation
 *     type in the enum statement in the RepTypeI.h module.
 *     Keep the enum alphabetically sorted.
 *
 *     Note: if the numerical value is int sized or will be put into an
 *     int sized field, then the ID number of representation type
 *     needs to also be added to the "special case for int
 *     sized fields" clause in the ConvertRepType function below.
 *
 *  4) Add an element to the static array of representation type data
 *     structures named "_XmStandardRepTypes". Use the same format as the
 *     other elements in the array; the fields which are initialized with
 *     FALSE and NULL should be the same for all elements of the array.
 *     Always add the new entry sorted in the array.
 *
 *  5) You're done.  A generic "string to representation type" converter
 *     for the representation type that you just added will be automatically
 *     registered when all other Xm converters are registered.
 *
 ******** For "mapped" representation types: ********
 *
 *  2M) Define a static array of the numerical values for the
 *     representation type in RepType.c.  Use the enumerated symbols
 *     (generally defined in Xm.h) to initialize the array of numerical
 *     values.  Use the representation type name, plus the suffix "Map"
 *     for the name of the array (see existing map arrays for an example).
 *
 *  3M) Go to 2).
 *
 *************************************************************************
 *
 *  NOTE: We have decided to use the reptype mechanism for action parameter
 *  symbolic names. In order to capture the name of the widget class, the
 *  name of the action the parameters apply to and the fact that this reptype
 *  is for parameters to an action, the names tend to be rather long. This
 *  is a feature not a bug. Furthermore, we decided not to create new
 *  XmR types for each reptype which serves that purpose. To that end, I
 *  have hardcoded strings in the reptype table where the XmR value would
 *  normally go.
 */

/********    Static Function Declarations    ********/

static String * CopyStringArray(
                        String *StrArray,
                        unsigned char NumEntries,
                        Boolean UppercaseFormat) ;
static void CopyRecord(
                       XmRepTypeEntry OutputEntry,
		       String rep_type_name,
		       String *value_names,
		       unsigned char *values,
		       unsigned char num_values,
		       Boolean reverse_installed,
		       XmRepTypeId rep_type_id,
		       Boolean copy_in) ;
static Boolean ValuesConsecutiveStartingAtZero(
                        unsigned char *values,
                        unsigned char num_values) ;
static XmRepTypeEntry GetRepTypeRecord(
                        XmRepTypeId rep_type_id) ;
static Boolean ConvertRepType(
                        Display *disp,
                        XrmValue *args,
                        Cardinal *n_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;
static Boolean ReverseConvertRepType(
                        Display *disp,
                        XrmValue *args,
                        Cardinal *n_args,
                        XrmValue *from,
                        XrmValue *to,
                        XtPointer *converter_data) ;

/********    End Static Function Declarations    ********/



/*** For consistency with the enum and array declaration, keep these
  declarations sorted. Put the map array right after the string
  array if any. ***/

static XmConst char *XmConst AlignmentNames[] =
{   "alignment_beginning", "alignment_center", "alignment_end"
    } ;
static XmConst char *XmConst AnimationStyleNames[] =
{   "drag_under_none", "drag_under_pixmap", "drag_under_shadow_in",
    "drag_under_shadow_out", "drag_under_highlight"
    };
static XmConst char *XmConst ArrowDirectionNames[] =
{   "arrow_up", "arrow_down", "arrow_left", "arrow_right"
    } ;
static XmConst char *XmConst ArrowLayoutNames[] =
  {
  "arrows_end",
  "arrows_beginning",
  "arrows_split",
  "arrows_flat_end",
  "arrows_flat_beginning"
  };
static XmConst char *XmConst ArrowOrientationNames[] =
  {
  "arrows_vertical",
  "arrows_horizontal"
  };
static XmConst char *XmConst ArrowSensitivityNames[] =
  {
  "arrows_insensitive",		/* 0b000 */
  "arrows_increment_sensitive",	/* 0b001 */
  "arrows_decrement_sensitive", /* 0b010 */
  "arrows_sensitive",		/* 0b011 */
  "arrows_default_sensitivity"  /* 0b100 Inherit */
  };
static XmConst char *XmConst AttachmentNames[] =
{   "attach_none", "attach_form", "attach_opposite_form", "attach_widget",
    "attach_opposite_widget", "attach_position", "attach_self"
    } ;
static XmConst char *XmConst AudibleWarningNames[] =
{   "none", "bell"
    } ;
static XmConst char *XmConst AutoDragModelNames[] =
{   "auto_drag_enabled",	"auto_drag_disabled",
    } ;
static XmConst char *XmConst AutomaticSelectionNames[] =
{   "no_auto_select", "auto_select",
    XtEoff, XtEfalse, XtEno, "0",
    XtEon, XtEtrue, XtEyes, "1"
    } ;
static XmConst unsigned char AutomaticSelectionMap[] =
{   XmNO_AUTO_SELECT, XmAUTO_SELECT,
    XmNO_AUTO_SELECT, XmNO_AUTO_SELECT, XmNO_AUTO_SELECT, XmNO_AUTO_SELECT,
    XmAUTO_SELECT, XmAUTO_SELECT, XmAUTO_SELECT, XmAUTO_SELECT
    } ;
static XmConst char *XmConst BindingTypeNames[] =
{   "none", "pixmap", "solid", "spiral", "pixmap_overlap_only"
    } ;
static XmConst char *XmConst BitmapConversionModelNames[] =
{   "match_depth", "dynamic_depth"
    } ;
static XmConst char *XmConst BlendModelNames[] =
{   "blend_all", "blend_state_source", "blend_just_source", "blend_none"
    } ;
#define ChildHorizontalAlignmentNames   AlignmentNames

static XmConst char *XmConst ChildPlacementNames[] =
{   "place_top", "place_above_selection", "place_below_selection"
    } ;
static XmConst char *XmConst ChildTypeNames[] =
{   "frame_generic_child", "frame_workarea_child", "frame_title_child"
    } ;
static XmConst char *XmConst ChildVerticalAlignmentNames[] =
{   "alignment_baseline_top", "alignment_center",
    "alignment_baseline_bottom",
    "alignment_widget_top", "alignment_widget_bottom",
    "alignment_child_top", "alignment_child_bottom"
    } ;

static XmConst unsigned char ChildVerticalAlignmentMap[] =
{   XmALIGNMENT_BASELINE_TOP, XmALIGNMENT_CENTER,
    XmALIGNMENT_BASELINE_BOTTOM,
    XmALIGNMENT_WIDGET_TOP, XmALIGNMENT_WIDGET_BOTTOM,
    XmALIGNMENT_CHILD_TOP, XmALIGNMENT_CHILD_BOTTOM
    } ;
static XmConst char *XmConst ComboBoxListActionActionParamNames[] =
{
    "up", "down", "listprevpage", "listnextpage", "listbegindata",
    "listenddata"
    } ;
static XmConst char *XmConst ComboBoxTypeNames[] =
{   "combo_box", "drop_down_combo_box", "drop_down_list"
    } ;
static XmConst char *XmConst CommandSelectionBoxUpOrDownActionParamNames[] =
{   "previous", "next", "first", "last"
    } ;
static XmConst char *XmConst CommandWindowLocationNames[] =
{   "command_above_workspace", "command_below_workspace"
    } ;
static XmConst char *XmConst ContainerCursorActionParamNames[] =
{
    "left", "right", "up", "down", "first", "last"
  } ;
static XmConst char *XmConst ContainerExpandCollapseActionParamNames[] =
{
    "left", "right", "collapse", "expand"
  } ;
static XmConst char *XmConst ContainerStartTransferActionParamNames[] =
{
    "link", "move", "copy"
  } ;

static XmConst char *XmConst DefaultButtonEmphasisNames[] =
{
    "external_highlight", "internal_highlight"
} ;

static XmConst char *XmConst DefaultButtonTypeNames[] =
{   "dialog_none", "dialog_cancel_button", "dialog_ok_button",
    "dialog_help_button"
    } ;
static XmConst unsigned char DefaultButtonTypeMap[] =
{   XmDIALOG_NONE, XmDIALOG_CANCEL_BUTTON, XmDIALOG_OK_BUTTON,
    XmDIALOG_HELP_BUTTON
    } ;
static XmConst char *XmConst DeleteResponseNames[] =
{   "destroy", "unmap", "do_nothing"
    } ;

static XmConst char *XmConst DialogStyleNames[] =
{   "dialog_modeless", "dialog_work_area", "dialog_primary_application_modal",
    "dialog_application_modal", "dialog_full_application_modal",
    "dialog_system_modal"
    } ;
static XmConst unsigned char DialogStyleMap[] =
{   XmDIALOG_MODELESS, XmDIALOG_WORK_AREA, XmDIALOG_PRIMARY_APPLICATION_MODAL,
    XmDIALOG_APPLICATION_MODAL, XmDIALOG_FULL_APPLICATION_MODAL,
    XmDIALOG_SYSTEM_MODAL
    } ;

static XmConst char *XmConst DialogTypeNames[] =
{   "dialog_template", "dialog_error", "dialog_information", "dialog_message",
    "dialog_question", "dialog_warning", "dialog_working"
    } ;
static XmConst char *XmConst DirectionNames[] =
{   "left_to_right", "right_to_left",
    "left_to_right_top_to_bottom", "right_to_left_top_to_bottom",
    "left_to_right_bottom_to_top", "right_to_left_bottom_to_top",
    "top_to_bottom_left_to_right", "top_to_bottom_right_to_left",
    "bottom_to_top_left_to_right", "bottom_to_top_right_to_left"
   } ;
static XmConst unsigned char DirectionMap[] =
{   XmLEFT_TO_RIGHT, XmRIGHT_TO_LEFT,
    XmLEFT_TO_RIGHT_TOP_TO_BOTTOM, XmRIGHT_TO_LEFT_TOP_TO_BOTTOM,
    XmLEFT_TO_RIGHT_BOTTOM_TO_TOP, XmRIGHT_TO_LEFT_BOTTOM_TO_TOP,
    XmTOP_TO_BOTTOM_LEFT_TO_RIGHT, XmTOP_TO_BOTTOM_RIGHT_TO_LEFT,
    XmBOTTOM_TO_TOP_LEFT_TO_RIGHT, XmBOTTOM_TO_TOP_RIGHT_TO_LEFT
   };
static XmConst char *XmConst DragInitiatorProtocolStyleNames[] =
{   "drag_none", "drag_drop_only", "drag_prefer_preregister",
    "drag_preregister", "drag_prefer_dynamic", "drag_dynamic",
    "drag_prefer_receiver"
	};
static XmConst char *XmConst DragReceiverProtocolStyleNames[] =
{   "drag_none", "drag_drop_only", "drag_prefer_preregister",
    "drag_preregister", "drag_prefer_dynamic", "drag_dynamic"
	};
static XmConst char *XmConst DropSiteActivityNames[] =
{   "drop_site_active", "drop_site_inactive", "drop_site_ignore"
	};
static XmConst char *XmConst DropSiteTypeNames[] =
{   "drop_site_simple",   "drop_site_composite"
	};
static XmConst char *XmConst EditModeNames[] =
{   "multi_line_edit", "single_line_edit"
    } ;

static XmConst char *XmConst EnableBtn1Names[] =
{   XtEfalse, XtEtrue, XtEoff, "button2_adjust", "button2_transfer" };

static XmConst unsigned char EnableBtn1Map[] =
{   XmOFF, XmBUTTON2_ADJUST, XmOFF, XmBUTTON2_ADJUST, XmBUTTON2_TRANSFER };

static XmConst char *XmConst EnableWarpNames[] =
{   "enable_warp_on",	"enable_warp_off",
    } ;

static XmConst char *XmConst EntryViewTypeNames[] =
{   "large_icon", "small_icon", "any_icon"
    } ;
static XmConst char *XmConst FileFilterStyleNames[] =
{   "filter_none", "filter_hidden_files"
    } ;
static XmConst char *XmConst FileSelectionBoxUpOrDownActionParamNames[] =
{   "previous", "next", "first", "last"
    } ;
static XmConst char *XmConst FileTypeMaskNames[] =
{   "file_directory", "file_regular", "file_any_type"
    } ;
static XmConst unsigned char FileTypeMaskMap[] =
{   XmFILE_DIRECTORY, XmFILE_REGULAR, XmFILE_ANY_TYPE
    } ;
static XmConst char *XmConst FontTypeNames[] =
{   "font_is_font", "font_is_fontset", "as_is", "font_is_xft"
   } ;
static XmConst unsigned char FontTypeMap[] =
{   XmFONT_IS_FONT, XmFONT_IS_FONTSET, XmAS_IS, XmFONT_IS_XFT
   } ;
static XmConst char *XmConst IconAttachmentNames[] =
{   "attach_north_west", "attach_north", "attach_north_east", "attach_east",
    "attach_south_east", "attach_south", "attach_south_west", "attach_west",
    "attach_center", "attach_hot"
    } ;
static XmConst char *XmConst IndicatorOnNames[] =
    {
    "indicator_none", "indicator_fill", "indicator_box", "indicator_check",
    "indicator_check_box", "indicator_cross", "indicator_cross_box",
    XtEoff, XtEfalse, XtEno,
    XtEon, XtEtrue, XtEyes
    };
static XmConst unsigned  char IndicatorOnMap[] =
    {
     XmINDICATOR_NONE, XmINDICATOR_FILL, XmINDICATOR_BOX, XmINDICATOR_CHECK,
     XmINDICATOR_CHECK_BOX, XmINDICATOR_CROSS, XmINDICATOR_CROSS_BOX,
     XmINDICATOR_NONE, XmINDICATOR_NONE, XmINDICATOR_NONE,
     XmINDICATOR_FILL, XmINDICATOR_FILL, XmINDICATOR_FILL
    };
static XmConst char *XmConst IndicatorTypeNames[] =
{   "n_of_many", "one_of_many", "one_of_many_round", "one_of_many_diamond"
    } ;
static XmConst unsigned char IndicatorTypeMap[] =
{   XmN_OF_MANY, XmONE_OF_MANY, XmONE_OF_MANY_ROUND, XmONE_OF_MANY_DIAMOND
    } ;
static XmConst char *XmConst InputPolicyNames[] =
{   "per_shell", "per_widget"
    } ;
static XmConst char *XmConst KeyboardFocusPolicyNames[] =
{   "explicit", "pointer"
    } ;

static XmConst char *XmConst LabelTypeNames[] =
{   "pixmap", "string", "pixmap_and_string"
    } ;
static XmConst unsigned char LabelTypeMap[] =
{   XmPIXMAP, XmSTRING, XmPIXMAP_AND_STRING
    } ;
static XmConst char *XmConst LayoutTypeNames[] =
{   "outline", "spatial", "detail"
   } ;
static XmConst char *XmConst LineStyleNames[] =
{   "no_line", "single"
    } ;
static XmConst char *XmConst LineTypeNames[] =
{   "no_line", "single_line", "double_line",
    "single_dashed_line", "double_dashed_line", "as_is"
    } ;
static XmConst unsigned char LineTypeMap[] =
{   XmNO_LINE, XmSINGLE_LINE, XmDOUBLE_LINE,
    XmSINGLE_DASHED_LINE, XmDOUBLE_DASHED_LINE, XmAS_IS
    } ;
static XmConst char *XmConst ListSizePolicyNames[] =
{   "variable", "constant", "resize_if_possible"
    } ;
static XmConst char *XmConst LoadModelNames[] =
{   "load_deferred", "load_immediate", "as_is"
    } ;
static XmConst unsigned char LoadModelMap[] =
{   XmLOAD_DEFERRED, XmLOAD_IMMEDIATE, XmAS_IS
    } ;
static XmConst char *XmConst MatchBehaviorNames[] =
{   "none", "quick_navigate"
    } ;
static XmConst char *XmConst MultiClickNames[] =
{   "multiclick_discard", "multiclick_keep"
    } ;
static XmConst char *XmConst NavigationTypeNames[] =
{   "none", "tab_group", "sticky_tab_group", "exclusive_tab_group"
    } ;
static XmConst char *XmConst NotebookChildTypeNames[] =
{   "none", "page", "major_tab", "minor_tab", "status_area", "page_scroller"
    } ;
static XmConst char *XmConst NotebookTraverseTabActionParamNames[] =
{   "home", "end", "next", "previous"
    } ;
static XmConst char *XmConst OrientationNames[] =
{   XtEvertical, XtEhorizontal
    } ;
static XmConst unsigned char OrientationMap[] =
{   XmVERTICAL, XmHORIZONTAL
    } ;
static XmConst char *XmConst OutlineButtonPolicyNames[] =
{   "outline_button_present", "outline_button_absent"
    } ;
static XmConst char *XmConst OutlineStateNames[] =
{   "collapsed", "expanded"
    } ;
static XmConst char *XmConst PackingNames[] =
{   "pack_tight", "pack_column", "pack_none"
    } ;
static XmConst unsigned char PackingMap[] =
{   XmPACK_TIGHT, XmPACK_COLUMN, XmPACK_NONE
    } ;
static XmConst char *XmConst PanedWindowSashActionParamNames[] =
{
    "start", "move", "commit", "key"
    };
static XmConst char *XmConst PanedWindowSashDirectionActionParamNames[] =
{
    "up", "down", "right", "left", "first", "last"
    };
static XmConst char *XmConst PanedWindowSashIncrementActionParamNames[] =
{
    "defaultincr", "largeincr"
    };
static XmConst char *XmConst PathModeNames[] =
{   "path_mode_full", "path_mode_relative"
    } ;
static XmConst char *XmConst PositionModeNames[] =
{
  "zero_based", "one_based"
};
static XmConst char *XmConst PositionTypeNames[] =
  {
  "position_index",
  "position_value"
  };
static XmConst char *XmConst PrimaryOwnershipNames[] =
{   "own_never", "own_always", "own_multiple", "own_possible_multiple"
    } ;
static XmConst char *XmConst ProcessingDirectionNames[] =
{   "max_on_top", "max_on_bottom", "max_on_left", "max_on_right"
    } ;
static XmConst char *XmConst ResizePolicyNames[] =
{   "resize_none", "resize_grow", "resize_any"
    } ;
static XmConst char *XmConst RowColumnTypeNames[] =
{   "work_area", "menu_bar", "menu_pulldown", "menu_popup", "menu_option"
    } ;
static XmConst char *XmConst ScrollBarDisplayPolicyNames[] =
{   "static", "as_needed"
    } ;
static XmConst char *XmConst ScrollBarIncrementDownOrRightActionParamNames[] =
{   "down", "right"
    } ;
static XmConst char *XmConst ScrollBarIncrementUpOrLeftActionParamNames[] =
{   "up", "left"
    } ;
static XmConst char *XmConst ScrollBarPageDownOrRightActionParamNames[] =
{   "down", "right"
    } ;
static XmConst char *XmConst ScrollBarPageUpOrLeftActionParamNames[] =
{   "up", "left"
    } ;
static XmConst char *XmConst ScrollBarPlacementNames[] =
{   "bottom_right", "top_right", "bottom_left", "top_left"
    } ;
/* NOTE: work_area, menu_bar and separator have to match the existing ones */
static XmConst char *XmConst ScrolledWindowChildTypeNames[] =
{   "work_area", "menu_bar",
    "hor_scrollbar", "vert_scrollbar",
    "command_window",
    "separator", "message_window",
    "scroll_hor", "scroll_vert", "no_scroll",
    "clip_window", "generic_child"
    } ;
static XmConst char *XmConst ScrollingPolicyNames[] =
{   "automatic", "application_defined"
    } ;
static XmConst char *XmConst SelectionBoxUpOrDownActionParamNames[] =
{   "previous", "next", "first", "last"
    } ;
static XmConst char *XmConst SelectionModeNames[] =
{   "normal_mode", "add_mode"
    } ;
static XmConst char *XmConst SelectionPolicyNames[] =
{   "single_select", "multiple_select", "extended_select", "browse_select"
    } ;
static XmConst char *XmConst SelectionTechniqueNames[] =
{   "marquee", "marquee_extend_start", "marquee_extend_both",
    "touch_only", "touch_over"
    } ;
static XmConst char *XmConst SelectionTypeNames[] =
{   "dialog_work_area", "dialog_prompt", "dialog_selection", "dialog_command",
    "dialog_file_selection"
    } ;
static XmConst char *XmConst SeparatorTypeNames[] =
{   "no_line", "single_line", "double_line", "single_dashed_line",
    "double_dashed_line", "shadow_etched_in", "shadow_etched_out",
    "shadow_etched_in_dash", "shadow_etched_out_dash"
    } ;
static XmConst char *XmConst SetNames[] =
{   "unset", "set", "indeterminate",
    XtEoff, XtEfalse, XtEno, "0",
    XtEon, XtEtrue, XtEyes, "1"
    } ;
static XmConst unsigned char SetMap[] =
{   XmUNSET, XmSET, XmINDETERMINATE,
    XmUNSET, XmUNSET, XmUNSET, XmUNSET,
    XmSET, XmSET, XmSET, XmSET
    } ;
static XmConst char *XmConst ShadowTypeNames[] =
{   "shadow_etched_in", "shadow_etched_out", "shadow_in", "shadow_out"
    } ;
static XmConst unsigned char ShadowTypeMap[] =
{   XmSHADOW_ETCHED_IN, XmSHADOW_ETCHED_OUT, XmSHADOW_IN, XmSHADOW_OUT
    } ;
static XmConst char *XmConst ShowArrowsNames[] =
{   "each_side", XtEtrue, XtEyes, XtEon, "1",
    "max_side", "min_side",
    "none", XtEfalse, XtEno, XtEoff, "0"
    } ;
static XmConst unsigned char ShowArrowsMap[] =
{   1,1,1,1,1,
    XmMAX_SIDE, XmMIN_SIDE,
    0,0,0,0,0
    } ;
static XmConst char *XmConst ShowValueNames[] =
{   "near_slider", XtEtrue, XtEyes, XtEon, "1",
    "near_border",
    "none", XtEfalse, XtEno, XtEoff, "0"
    } ;
static XmConst unsigned char ShowValueMap[] =
{   1,1,1,1,1,
    XmNEAR_BORDER,
    0,0,0,0,0
    } ;

static XmConst char *XmConst SliderMarkNames[] =
{   "none", "etched_line", "thumb_mark", "round_mark"
    } ;
static XmConst char *XmConst SliderVisualNames[] =
{   "background", "foreground", "trough_color", "shadowed_background"
    } ;
static XmConst char *XmConst SlidingModeNames[] =
{   "slider", "thermometer",
    } ;
static XmConst char *XmConst SpatialIncludeModelNames[] =
{   "append", "closest", "first_fit"
   } ;
static XmConst char *XmConst SpatialResizeModelNames[] =
{   "grow_minor", "grow_major", "grow_balanced"
    } ;
static XmConst char *XmConst SpatialSnapModelNames[] =
{   "none", "snap_to_grid", "center"
    } ;
static XmConst char *XmConst SpatialStyleNames[] =
{   "none", "grid", "cells"
    } ;
static XmConst char *XmConst SpinBoxChildTypeNames[] =
  {
  "string",
  "numeric",
  };

static XmConst unsigned char SpinBoxChildTypeMap[] =
  {
  (unsigned char) XmSTRING,
  (unsigned char) XmNUMERIC,
  };
static XmConst char *XmConst StringDirectionNames[] =
{   "string_direction_l_to_r", "string_direction_r_to_l"
    } ;
static XmConst char *XmConst TearOffModelNames[] =
{   "tear_off_enabled", "tear_off_disabled"
    } ;
static XmConst char *XmConst TextExtendMovementActionParamNames[] =
{   "extend"
    } ;
static XmConst char *XmConst TextFieldExtendMovementActionParamNames[] =
{   "extend"
    } ;
static XmConst char *XmConst TextFieldDirectionActionParamNames[] =
{   "right", "left"
    } ;
static XmConst char *XmConst TextHorizontalDirectionActionParamNames[] =
{   "right", "left"
    } ;
static XmConst char *XmConst TextVerticalDirectionActionParamNames[] =
{   "up", "down"
    } ;
static XmConst char *XmConst ToggleModeNames[] =
{   "toggle_boolean", "toggle_indeterminate"
    } ;
static XmConst char *XmConst UnitTypeNames[] =
{   "pixels", "100th_millimeters", "1000th_inches", "100th_points",
    "100th_font_units", "inches", "centimeters", "millimeters",
    "points", "font_units"
    } ;
static XmConst char *XmConst UnpostBehaviorNames[] =
{   "unpost", "unpost_and_replay"
    } ;
static XmConst char *XmConst VerticalAlignmentNames[] =
{   "alignment_baseline_top", "alignment_center", "alignment_baseline_bottom",
    "alignment_contents_top", "alignment_contents_bottom"
    } ;
static XmConst char *XmConst ViewTypeNames[] =
{   "large_icon", "small_icon"
    } ;
static XmConst char *XmConst VisualEmphasisNames[] =
{   "selected", "not_selected"
    };
static XmConst char *XmConst VisualPolicyNames[] =
{   "variable", "constant"
    } ;
static XmConst char *XmConst WhichButtonNames[] =
{   "button1", "1", "button2", "2", "button3", "3", "button4", "4",
    "button5", "5"
    } ;
static XmConst unsigned char WhichButtonMap[] =
{   Button1, Button1, Button2, Button2, Button3, Button3, Button4, Button4,
    Button5, Button5
    } ;
static XmConst char *XmConst PixmapPlacementNames[] =
{   "top", "bottom", "left", "right"
    } ;
static XmConst unsigned char PixmapPlacementMap[] =
{   XmPIXMAP_TOP, XmPIXMAP_BOTTOM, XmPIXMAP_LEFT, XmPIXMAP_RIGHT
    } ;



/* Note that this array does not initialize rep_type_id fields,
 * for this field is useless.  It always matches the index of the
 * entry in the array.  We have to keep the field since the structure
 * is public.  For the API, rep_type_id is set on query.
 */


static XmRepTypeEntryRec StandardRepTypes[] = {
  {
    XmRAlignment, (String*)AlignmentNames, NULL,
    XtNumber(AlignmentNames), FALSE,
  },
  {
    XmRAnimationStyle, (String*)AnimationStyleNames, NULL,
    XtNumber(AnimationStyleNames), FALSE,
  },
  {
    XmRArrowDirection, (String*)ArrowDirectionNames, NULL,
    XtNumber(ArrowDirectionNames), FALSE,
  },
  {
    XmRArrowLayout, (String*)ArrowLayoutNames, NULL,
    XtNumber(ArrowLayoutNames),  FALSE,
  },
  {
    XmRArrowOrientation, (String*)ArrowOrientationNames, NULL,
    XtNumber(ArrowOrientationNames),  FALSE,
  },
  {
    XmRArrowSensitivity, (String*)ArrowSensitivityNames, NULL,
    XtNumber(ArrowSensitivityNames),  FALSE,
  },
  {
    XmRAttachment, (String*)AttachmentNames, NULL,
    XtNumber(AttachmentNames), FALSE,
  },
  {
    XmRAudibleWarning, (String*)AudibleWarningNames, NULL,
    XtNumber(AudibleWarningNames), FALSE,
  },
  {
    XmRAutoDragModel, (String*)AutoDragModelNames, NULL,
    XtNumber(AutoDragModelNames), FALSE,
  },
  {
    XmRAutomaticSelection, (String*)AutomaticSelectionNames,
    (unsigned char *)AutomaticSelectionMap,
    XtNumber(AutomaticSelectionNames), FALSE,
  },
  {
    XmRBindingType, (String*)BindingTypeNames, NULL,
    XtNumber(BindingTypeNames), FALSE,
  },
  {
    XmRBitmapConversionModel, (String*)BitmapConversionModelNames, NULL,
    XtNumber(BitmapConversionModelNames), FALSE,
  },
  {
    XmRBlendModel, (String*)BlendModelNames, NULL,
    XtNumber(BlendModelNames), FALSE,
  },
  {
    XmRChildHorizontalAlignment, (String*)ChildHorizontalAlignmentNames, NULL,
    XtNumber(ChildHorizontalAlignmentNames), FALSE,
  },
  {
    XmRChildPlacement, (String*)ChildPlacementNames, NULL,
    XtNumber(ChildPlacementNames), FALSE,
  },
  {
    XmRChildType, (String*)ChildTypeNames, NULL,
    XtNumber(ChildTypeNames), FALSE,
  },
  {
    XmRChildVerticalAlignment, (String*)ChildVerticalAlignmentNames,
    (unsigned char *)ChildVerticalAlignmentMap,
    XtNumber(ChildVerticalAlignmentNames), FALSE,
  },
  {
    "ComboBoxListActionActionParam", /* See instructions above. */
    (String*)ComboBoxListActionActionParamNames, NULL,
    XtNumber(ComboBoxListActionActionParamNames), TRUE,
  },
  {
    XmRComboBoxType, (String*)ComboBoxTypeNames, NULL,
    XtNumber(ComboBoxTypeNames), FALSE,
  },
  {
    "CommandSelectionBoxUpOrDownActionParam",
    (String*)CommandSelectionBoxUpOrDownActionParamNames, NULL,
    XtNumber(CommandSelectionBoxUpOrDownActionParamNames), TRUE,
  },
  {
    XmRCommandWindowLocation, (String*)CommandWindowLocationNames, NULL,
    XtNumber(CommandWindowLocationNames), FALSE,
  },
  {
    "ContainerCursorActionParam", /* See instructions above. */
    (String*)ContainerCursorActionParamNames,
    NULL, XtNumber(ContainerCursorActionParamNames), TRUE,
  },
  {
    "ContainerExpandCollapseActionParamName", /* See instructions above. */
    (String*)ContainerExpandCollapseActionParamNames,
    NULL, XtNumber(ContainerExpandCollapseActionParamNames), TRUE,
  },
  {
    "ContainerStartTransferActionParam", /* See instructions above. */
    (String*)ContainerStartTransferActionParamNames,
    NULL, XtNumber(ContainerStartTransferActionParamNames), TRUE,
  },
  {
    XmRDefaultButtonEmphasis, (String*)DefaultButtonEmphasisNames, NULL,
    XtNumber(DefaultButtonEmphasisNames), FALSE,
  },
  {
    XmRDefaultButtonType, (String*)DefaultButtonTypeNames,
    (unsigned char *)DefaultButtonTypeMap,
    XtNumber(DefaultButtonTypeNames), FALSE,
  },
  {
    XmRDeleteResponse, (String*)DeleteResponseNames, NULL,
    XtNumber(DeleteResponseNames),  FALSE,
  },
  {
    XmRDialogStyle, (String*)DialogStyleNames, (unsigned char *)DialogStyleMap,
    XtNumber(DialogStyleNames), FALSE,
  },
  {
    XmRDialogType, (String*)DialogTypeNames, NULL,
    XtNumber(DialogTypeNames), FALSE,
  },
  {
    XmRDirection, (String*)DirectionNames, (unsigned char *)DirectionMap,
    XtNumber(DirectionNames), FALSE,
  },
  {
    XmRDragInitiatorProtocolStyle, (String*)DragInitiatorProtocolStyleNames, NULL,
    XtNumber(DragInitiatorProtocolStyleNames), FALSE,
  },
  {
    XmRDragReceiverProtocolStyle, (String*)DragReceiverProtocolStyleNames, NULL,
    XtNumber(DragReceiverProtocolStyleNames), FALSE,
  },
  {
    XmRDropSiteActivity, (String*)DropSiteActivityNames, NULL,
    XtNumber(DropSiteActivityNames), FALSE,
  },
  {
    XmRDropSiteType, (String*)DropSiteTypeNames, NULL,
    XtNumber(DropSiteTypeNames), FALSE,
  },
  {
    XmREditMode, (String*)EditModeNames, NULL,
    XtNumber(EditModeNames), FALSE,
  },
  {
    XmREnableBtn1Transfer, (String*)EnableBtn1Names,
    (unsigned char *) EnableBtn1Map,
    XtNumber(EnableBtn1Names), FALSE
  },
  {
    XmREnableWarp, (String*)EnableWarpNames, NULL,
    XtNumber(EnableWarpNames), FALSE,
  },
  {
    XmREntryViewType, (String*)EntryViewTypeNames, NULL,
    XtNumber(EntryViewTypeNames), FALSE,
  },
  {
    XmRFileFilterStyle, (String*)FileFilterStyleNames, NULL,
    XtNumber(FileFilterStyleNames), FALSE,
  },
  {
    "FileSelectionBoxUpOrDownActionParam", /* See instructions above. */
    (String*)FileSelectionBoxUpOrDownActionParamNames,
    NULL, XtNumber(FileSelectionBoxUpOrDownActionParamNames), TRUE,
  },
  {
    XmRFileTypeMask, (String*)FileTypeMaskNames,
    (unsigned char *)FileTypeMaskMap,
    XtNumber(FileTypeMaskNames), FALSE,
  },
  {
    XmRFontType, (String*)FontTypeNames, (unsigned char *)FontTypeMap,
    XtNumber(FontTypeNames), FALSE,
  },
  {
    XmRIconAttachment, (String*)IconAttachmentNames, NULL,
    XtNumber(IconAttachmentNames), FALSE,
  },
  {
    XmRIndicatorOn, (String*)IndicatorOnNames, (unsigned char *)IndicatorOnMap,
    XtNumber(IndicatorOnNames), FALSE,
  },
  {
    XmRIndicatorType, (String*)IndicatorTypeNames,
    (unsigned char *)IndicatorTypeMap,
    XtNumber(IndicatorTypeNames), FALSE,
  },
  {
    XmRInputPolicy, (String*)InputPolicyNames, NULL,
    XtNumber(InputPolicyNames), FALSE,
  },
  {
    XmRKeyboardFocusPolicy, (String*)KeyboardFocusPolicyNames, NULL,
    XtNumber(KeyboardFocusPolicyNames),  FALSE,
  },
  {
    XmRLabelType, (String*)LabelTypeNames, (unsigned char *)LabelTypeMap,
    XtNumber(LabelTypeNames), FALSE,
  },
  {
    XmRLayoutType, (String*)LayoutTypeNames, NULL,
    XtNumber(LayoutTypeNames), FALSE,
  },
  {
    XmRLineStyle, (String*)LineStyleNames, NULL,
    XtNumber(LineStyleNames), FALSE,
  },
  {
    XmRLineType, (String*)LineTypeNames, (unsigned char *)LineTypeMap,
    XtNumber(LineTypeNames), FALSE,
  },
  {
    XmRListSizePolicy, (String*)ListSizePolicyNames, NULL,
    XtNumber(ListSizePolicyNames), FALSE,
  },
  {
    XmRLoadModel, (String*)LoadModelNames, (unsigned char *)LoadModelMap,
    XtNumber(LoadModelNames), FALSE,
  },
  {
    XmRMatchBehavior, (String*)MatchBehaviorNames, NULL,
    XtNumber(MatchBehaviorNames), FALSE,
  },
  {
    XmRMultiClick, (String*)MultiClickNames, NULL,
    XtNumber(MultiClickNames), FALSE,
  },
  {
    XmRNavigationType, (String*)NavigationTypeNames, NULL,
    XtNumber(NavigationTypeNames), FALSE,
  },
  {
    XmRNotebookChildType, (String*)NotebookChildTypeNames, NULL,
    XtNumber(NotebookChildTypeNames), FALSE,
  },
  {
    "NotebookTraverseTabActionParam", /* See instructions above. */
    (String*)NotebookTraverseTabActionParamNames, NULL,
    XtNumber(NotebookTraverseTabActionParamNames ), TRUE,
  },
  {
    XmROrientation, (String*)OrientationNames, (unsigned char *)OrientationMap,
    XtNumber(OrientationNames), FALSE,
  },
  {
    XmROutlineButtonPolicy, (String*)OutlineButtonPolicyNames, NULL,
    XtNumber(OutlineButtonPolicyNames), FALSE,
  },
  {
    XmROutlineState, (String*)OutlineStateNames, NULL,
    XtNumber(OutlineStateNames), FALSE,
  },
  {
    XmRPacking, (String*)PackingNames, (unsigned char *)PackingMap,
    XtNumber(PackingNames), FALSE,
  },
  {
    "PanedWindowSashActionParam", /* See instructions above. */
    (String*)PanedWindowSashActionParamNames, NULL,
    XtNumber(PanedWindowSashActionParamNames ), TRUE,
  },
  {
    "PanedWindowSashDirectionActionParam", /* See instructions above. */
    (String*)PanedWindowSashDirectionActionParamNames, NULL,
    XtNumber(PanedWindowSashDirectionActionParamNames ), TRUE,
  },
  {
    "PanedWindowSashIncrementActionParam", /* See instructions above. */
    (String*)PanedWindowSashIncrementActionParamNames, NULL,
    XtNumber(PanedWindowSashIncrementActionParamNames ), TRUE,
  },
  {
    XmRPathMode, (String*)PathModeNames, NULL,
    XtNumber(PathModeNames), FALSE,
  },
  {
    XmRPositionMode, (String*)PositionModeNames, NULL,
    XtNumber(PositionModeNames), FALSE,
  },
  {
    XmRPositionType, (String*)PositionTypeNames, NULL,
    XtNumber(PositionTypeNames), FALSE,
  },
  {
    XmRPrimaryOwnership, (String*)PrimaryOwnershipNames, NULL,
    XtNumber(PrimaryOwnershipNames), FALSE,
  },
  {
    XmRProcessingDirection, (String*)ProcessingDirectionNames, NULL,
    XtNumber(ProcessingDirectionNames), FALSE,
  },
  {
    XmRResizePolicy, (String*)ResizePolicyNames, NULL,
    XtNumber(ResizePolicyNames), FALSE,
  },
  {
    XmRRowColumnType, (String*)RowColumnTypeNames, NULL,
    XtNumber(RowColumnTypeNames), FALSE,
  },
  {
    XmRScrollBarDisplayPolicy, (String*)ScrollBarDisplayPolicyNames, NULL,
    XtNumber(ScrollBarDisplayPolicyNames), FALSE,
  },
  {
    "ScrollBarIncrementDownOrRightActionParam", /* See instructions above. */
    (String*)ScrollBarIncrementDownOrRightActionParamNames,
    NULL, XtNumber(ScrollBarIncrementDownOrRightActionParamNames), TRUE,
  },
  {
    "ScrollBarIncrementUpOrLeftActionParam", /* See instructions above. */
    (String*)ScrollBarIncrementUpOrLeftActionParamNames,
    NULL, XtNumber(ScrollBarIncrementUpOrLeftActionParamNames), TRUE,
  },
  {
    "ScrollBarPageDownOrRightActionParam", /* See instructions above. */
    (String*)ScrollBarPageDownOrRightActionParamNames,
    NULL, XtNumber(ScrollBarPageDownOrRightActionParamNames), TRUE,
  },
  {
    "ScrollBarPageUpOrLeftActionParam", /* See instructions above. */
    (String*)ScrollBarPageUpOrLeftActionParamNames,
    NULL, XtNumber(ScrollBarPageUpOrLeftActionParamNames), TRUE,
  },
  {
    XmRScrollBarPlacement, (String*)ScrollBarPlacementNames, NULL,
    XtNumber(ScrollBarPlacementNames), FALSE,
  },
  {
    XmRScrolledWindowChildType, (String*)ScrolledWindowChildTypeNames, NULL,
    XtNumber(ScrolledWindowChildTypeNames), FALSE,
  },
  {
    XmRScrollingPolicy, (String*)ScrollingPolicyNames, NULL,
    XtNumber(ScrollingPolicyNames), FALSE,
  },
  {
    "SelectionBoxUpOrDownActionParam", /* See instructions above. */
    (String*)SelectionBoxUpOrDownActionParamNames,
    NULL, XtNumber(SelectionBoxUpOrDownActionParamNames), TRUE,
  },
  {
    XmRSelectionMode, (String*)SelectionModeNames, NULL,
    XtNumber(SelectionModeNames), FALSE,
  },
  {
    XmRSelectionPolicy, (String*)SelectionPolicyNames, NULL,
    XtNumber(SelectionPolicyNames), FALSE,
  },
  {
    XmRSelectionTechnique, (String*)SelectionTechniqueNames, NULL,
    XtNumber(SelectionTechniqueNames), FALSE,
  },
  {
    XmRSelectionType, (String*)SelectionTypeNames, NULL,
    XtNumber(SelectionTypeNames), FALSE,
  },
  {
    XmRSeparatorType, (String*)SeparatorTypeNames, NULL,
    XtNumber(SeparatorTypeNames), FALSE,
  },
  {
    XmRSet, (String*)SetNames, (unsigned char *)SetMap,
    XtNumber(SetNames), FALSE,
  },
  {
    XmRShadowType, (String*)ShadowTypeNames, (unsigned char *)ShadowTypeMap,
    XtNumber(ShadowTypeNames), FALSE,
  },
  {
    XmRShowArrows, (String*)ShowArrowsNames, (unsigned char *)ShowArrowsMap,
    XtNumber(ShowArrowsNames), FALSE,
  },
  {
    XmRShowValue, (String*)ShowValueNames, (unsigned char *)ShowValueMap,
    XtNumber(ShowValueNames), FALSE,
  },
  {
    XmRSliderMark, (String*)SliderMarkNames, NULL,
    XtNumber(SliderMarkNames), FALSE,
  },
  {
    XmRSliderVisual, (String*)SliderVisualNames, NULL,
    XtNumber(SliderVisualNames), FALSE,
  },
  {
    XmRSlidingMode, (String*)SlidingModeNames, NULL,
    XtNumber(SlidingModeNames), FALSE,
  },
  {
    XmRSpatialIncludeModel, (String*)SpatialIncludeModelNames, NULL,
    XtNumber(SpatialIncludeModelNames), FALSE,
  },
  {
    XmRSpatialResizeModel, (String*)SpatialResizeModelNames, NULL,
    XtNumber(SpatialResizeModelNames), FALSE,
  },
  {
    XmRSpatialSnapModel, (String*)SpatialSnapModelNames, NULL,
    XtNumber(SpatialSnapModelNames), FALSE,
  },
  {
    XmRSpatialStyle, (String*)SpatialStyleNames, NULL,
    XtNumber(SpatialStyleNames), FALSE,
  },
  {
    XmRSpinBoxChildType, (String*)SpinBoxChildTypeNames,
    (unsigned char *)SpinBoxChildTypeMap,
    XtNumber(SpinBoxChildTypeNames),  FALSE,
  },
  {
    XmRStringDirection, (String*)StringDirectionNames, NULL,
    XtNumber(StringDirectionNames), FALSE,
  },
  {
    XmRTearOffModel, (String*)TearOffModelNames, NULL,
    XtNumber(TearOffModelNames), FALSE,
  },
  {
    "TextExtendMovementActionParam", /* See instructions above. */
    (String*)TextExtendMovementActionParamNames, NULL,
    XtNumber(TextExtendMovementActionParamNames ), TRUE,
  },
  {
    "TextFieldDirectionActionParam", /* See instructions above. */
    (String*)TextFieldDirectionActionParamNames, NULL,
    XtNumber(TextFieldDirectionActionParamNames ), TRUE,
  },
  {
    "TextFieldExtendMovementActionParam", /* See instructions above. */
    (String*)TextFieldExtendMovementActionParamNames, NULL,
    XtNumber(TextFieldExtendMovementActionParamNames ), FALSE,
  },
  {
    "TextHorizontalDirectionActionParam", /* See instructions above. */
    (String*)TextHorizontalDirectionActionParamNames, NULL,
    XtNumber(TextHorizontalDirectionActionParamNames ), TRUE,
  },
  {
    "TextVerticalDirectionActionParam", /* See instructions above. */
    (String*)TextVerticalDirectionActionParamNames, NULL,
    XtNumber(TextVerticalDirectionActionParamNames ), TRUE,
  },
  {
    XmRToggleMode, (String*)ToggleModeNames, NULL,
    XtNumber(ToggleModeNames), FALSE,
  },
  {
    XmRUnitType, (String*)UnitTypeNames, NULL,
    XtNumber(UnitTypeNames), FALSE,
  },
  {
    XmRUnpostBehavior, (String*)UnpostBehaviorNames, NULL,
    XtNumber(UnpostBehaviorNames), FALSE,
  },
  {
    XmRVerticalAlignment, (String*)VerticalAlignmentNames, NULL,
    XtNumber(VerticalAlignmentNames), FALSE,
  },
  {
    XmRViewType, (String*)ViewTypeNames, NULL,
    XtNumber(ViewTypeNames), FALSE,
  },
  {
    XmRVisualEmphasis, (String*)VisualEmphasisNames, NULL,
    XtNumber(VisualEmphasisNames), FALSE,
  },
  {
    XmRVisualPolicy, (String*)VisualPolicyNames, NULL,
    XtNumber(VisualPolicyNames), FALSE,
  },
  {
    XmRWhichButton, (String*)WhichButtonNames, (unsigned char *)WhichButtonMap,
    XtNumber(WhichButtonNames),  FALSE,
  },
  {
    XmRPixmapPlacement, (String*)PixmapPlacementNames, (unsigned char *)PixmapPlacementMap,
    XtNumber(PixmapPlacementNames), FALSE,
  }
} ;

static XmConst Cardinal StandardNumRecs = XtNumber( StandardRepTypes );
static XmRepTypeEntryRec *DynamicRepTypes = NULL;
static Cardinal DynamicRepTypeNumRecords = 0;




static String *
CopyStringArray(
		String *StrArray,
                unsigned char NumEntries,
                Boolean UppercaseFormat)
{
    unsigned int Index ;
    String * TmpStr ;
    int PrefixSize = 0 ;

    TmpStr = (String *) XtMalloc((NumEntries + 1) * sizeof(String));
    TmpStr[NumEntries] = NULL ;

    if (UppercaseFormat) PrefixSize = 2 ;

    Index = 0 ;
    while(Index < NumEntries)
      {
	 TmpStr[Index] = XtMalloc(PrefixSize + strlen(StrArray[Index]) + 1);
	 strcpy(TmpStr[Index] + PrefixSize, StrArray[Index]);
	 Index ++ ;
      }

    if (UppercaseFormat) {
	Index = 0 ;
	while( Index < NumEntries)
	    {
		Cardinal i ;

		TmpStr[Index][0] = 'X' ;
		TmpStr[Index][1] = 'm' ;
		i = 2 ;
		while (TmpStr[Index][i]) {
		    if (islower(TmpStr[Index][i]))
			TmpStr[Index][i] = toupper(TmpStr[Index][i]);
		    i++;
		}
		++Index ;
            }
    }

    return( TmpStr) ;
}


static void
CopyRecord(
	   XmRepTypeEntry OutputEntry,
	   String rep_type_name,
	   String *value_names,
	   unsigned char *values,
	   unsigned char num_values,
	   Boolean reverse_installed,
	   XmRepTypeId rep_type_id,
	   Boolean copy_in)
{
    OutputEntry->rep_type_name = XtNewString(rep_type_name) ;

    OutputEntry->value_names = CopyStringArray(value_names, num_values,
					       False);

    /* only when the record is copied out to the app we want to
       create a array of consecutive values */
    if (values || !copy_in)
	OutputEntry->values = (unsigned char *)
	    XtMalloc(sizeof(unsigned char)*num_values);
    else
	OutputEntry->values = NULL;
    if (values) {
	memcpy(OutputEntry->values, values, (size_t)num_values);
    } else if (!copy_in) {
	Cardinal i ;
	for (i=0; i<num_values;i++) OutputEntry->values[i] = i ;
    }

    OutputEntry->num_values = num_values ;
    OutputEntry->reverse_installed = reverse_installed ;
    OutputEntry->rep_type_id = rep_type_id ;

}



static Boolean
ValuesConsecutiveStartingAtZero(
        unsigned char *values,
        unsigned char num_values)
{
    if(    values    )
      {   while(    num_values--    )
	    {   if(    num_values != values[num_values]    )
		  {   return( FALSE) ;
		  }
	      }
        }
    return( TRUE) ;
}


static XmRepTypeEntry
GetRepTypeRecord(
     XmRepTypeId rep_type_id)
{
    if (rep_type_id < StandardNumRecs) {
	return (XmRepTypeEntry) &StandardRepTypes[rep_type_id];
    }
    if (rep_type_id < DynamicRepTypeNumRecords + StandardNumRecs) {
	return &DynamicRepTypes[rep_type_id - StandardNumRecs];
    }

    return (XmRepTypeEntry)NULL ;
}


XmRepTypeId
XmRepTypeRegister(
        String rep_type_name,
        String *value_names,
        unsigned char *values,
        unsigned char num_values)
{
    XmRepTypeEntry NewRecord ;
    XtConvertArgRec convertArg;
    XmRepTypeId reptype_id;

    if (!num_values || !rep_type_name || !value_names)
	return( XmREP_TYPE_INVALID) ;

    _XmProcessLock();
    /** expand the dynamic table */
    DynamicRepTypes = (XmRepTypeList)
	XtRealloc( (char *) DynamicRepTypes, (sizeof(XmRepTypeEntryRec)
				* (DynamicRepTypeNumRecords + 1))) ;

   /** fill in the new record */
    NewRecord = &DynamicRepTypes[DynamicRepTypeNumRecords] ;

    /* the new reptype ID values are located after the standard ones */

    CopyRecord(NewRecord,
	       rep_type_name, value_names,
	       (ValuesConsecutiveStartingAtZero( values, num_values)) ?
	       NULL:values,
	       num_values, False,
	       DynamicRepTypeNumRecords + StandardNumRecs,
	       True);


    /** register the converter to Xt */
    convertArg.address_mode = XtImmediate;
    convertArg.address_id   = (XPointer)(long)NewRecord->rep_type_id;
    convertArg.size         = sizeof(convertArg.address_id);

    XtSetTypeConverter( XmRString, NewRecord->rep_type_name, ConvertRepType,
		       &convertArg, 1, XtCacheNone, NULL) ;


    DynamicRepTypeNumRecords++ ;
    reptype_id  = NewRecord->rep_type_id;
    _XmProcessUnlock();
    /*XmRepTypeAddReverse(reptype_id);*/
    return reptype_id;
}

void
XmRepTypeAddReverse(
     XmRepTypeId rep_type_id)
{

    XtConvertArgRec convertArg;
    XmRepTypeEntry Record;

    _XmProcessLock();
    Record = GetRepTypeRecord( rep_type_id);

    if(    Record  &&  !Record->reverse_installed    )
      {
	  convertArg.address_mode = XtImmediate;
	  convertArg.address_id   = (XPointer)(long)rep_type_id;
	  convertArg.size         = sizeof(convertArg.address_id);
	  XtSetTypeConverter( Record->rep_type_name, XmRString,
			     ReverseConvertRepType, &convertArg,
			     1, XtCacheNone, NULL) ;
	  Record->reverse_installed = TRUE ;
      }
    _XmProcessUnlock();
    return ;
}

Boolean
XmRepTypeValidValue(
     XmRepTypeId rep_type_id,
     unsigned char test_value,
     Widget enable_default_warning)
{
    XmRepTypeEntry Record;

    _XmProcessLock();
    Record = GetRepTypeRecord( rep_type_id);
    if (!Record) {
	_XmProcessUnlock();
	if (enable_default_warning) {
	    XmeWarning(enable_default_warning, MESSAGE1);
	}
	return FALSE;
    } else {
	if (Record->values) {
	    unsigned int Index;
	    for (Index=0; Index < Record->num_values; Index++ ) {
		if (Record->values[Index] == test_value) {
		    _XmProcessUnlock();
		    return(TRUE) ;
		}
	    }
	} else if (test_value < Record->num_values) {
	    _XmProcessUnlock();
	    return (TRUE) ;
	}
	if (enable_default_warning) {
	    char *params[2];
	    params[0] = (char *)(long)test_value;
	    params[1] = Record->rep_type_name;
	    _XmProcessUnlock();
	    _XmWarningMsg(enable_default_warning, "illegalRepTypeValue",
			  MESSAGE2, params, 2) ;
	    return FALSE;
	}
    }

    _XmProcessUnlock();
    return FALSE ;
}



XmRepTypeList
XmRepTypeGetRegistered( void )
{
    unsigned int TotalEntries ;
    XmRepTypeList OutputList ;
    unsigned int Index ;

    /* Total up the data sizes of the static and run-time lists. */

    _XmProcessLock();
    TotalEntries = StandardNumRecs + DynamicRepTypeNumRecords ;

    OutputList = (XmRepTypeList)
	XtMalloc((TotalEntries + 1)  * sizeof(XmRepTypeEntryRec)) ;


    for ( Index = 0; Index < StandardNumRecs; Index++ )
      {
	  XmRepTypeEntry Record = (XmRepTypeEntry) &(StandardRepTypes[Index]);

	  CopyRecord(&(OutputList[Index]),
		     Record->rep_type_name, Record->value_names,
		     Record->values, Record->num_values,
		     Record->reverse_installed, Index,
		     False) ;
      }

    for ( Index = 0; Index < DynamicRepTypeNumRecords; Index++ )
      {
	  XmRepTypeEntry Record = &(DynamicRepTypes[Index]) ;

	  CopyRecord(&(OutputList[StandardNumRecs + Index]),
		     Record->rep_type_name, Record->value_names,
		     Record->values, Record->num_values,
		     Record->reverse_installed, Index + StandardNumRecs,
		     False) ;
      }

    OutputList[TotalEntries].rep_type_name = NULL ;

    _XmProcessUnlock();
    return(OutputList) ;
}

XmRepTypeEntry
XmRepTypeGetRecord(
        XmRepTypeId rep_type_id)
{
    XmRepTypeEntry Record;
    XmRepTypeEntry OutputRecord ;

    _XmProcessLock();
    Record = GetRepTypeRecord( rep_type_id);
    if(    Record    )
      {
	  OutputRecord = (XmRepTypeEntry)
	      XtMalloc(sizeof( XmRepTypeEntryRec)) ;

	  CopyRecord(OutputRecord,
		     Record->rep_type_name, Record->value_names,
		     Record->values, Record->num_values,
		     Record->reverse_installed, rep_type_id,
		     False) ;

	  _XmProcessUnlock();
	  return( OutputRecord) ;
      }
    _XmProcessUnlock();
    return( NULL) ;
}



XmRepTypeId
XmRepTypeGetId(
        String rep_type_name)
{
    Cardinal Index ;


    _XmProcessLock();
    /* First look in the statically defined lists */

    /* Just an ordered search, could do better, but this routine
       is probably not be worth it */

    for ( Index = 0; Index < StandardNumRecs ; Index++ ) {
	int compare_name = strcmp(rep_type_name,
				  StandardRepTypes[Index].rep_type_name ) ;
	if(compare_name == 0) {
		_XmProcessUnlock();
		return  Index;
	}
	else if (compare_name < 0) break ;

     }


    /* Not in the static list; look in the run-time list. */
    /* This one is not ordered: have to go thru */
    for ( Index = 0; Index < DynamicRepTypeNumRecords; Index++ ) {

	  if( !strcmp( rep_type_name,
		      DynamicRepTypes[Index].rep_type_name )) {
	    _XmProcessUnlock();
	    return Index + StandardNumRecs ;
	  }
      }

    _XmProcessUnlock();
    return( XmREP_TYPE_INVALID) ;
}

String *
XmRepTypeGetNameList(
        XmRepTypeId rep_type_id,
        Boolean use_uppercase_format)
{
    XmRepTypeEntry Record;
    String *name_list = NULL;

    _XmProcessLock();
    Record = GetRepTypeRecord( rep_type_id);
    if(Record) {
	name_list = CopyStringArray(Record->value_names, Record->num_values,
				use_uppercase_format);
    }
    _XmProcessUnlock();
    return name_list;
}



static Boolean
ConvertRepType(
        Display *disp,
        XrmValue *args,
        Cardinal *n_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    char *in_str = (char *) (from->addr) ;
    XtPointer argvalue = *((XtPointer*)args[0].addr);
    XmRepTypeId RepTypeID = (XmRepTypeId)(long)argvalue;
    XmRepTypeEntry Record;
    Cardinal Index = 0 ;

    _XmProcessLock();
    Record = GetRepTypeRecord( RepTypeID);
    while(Index < Record->num_values) {

	if(XmeNamesAreEqual( in_str, Record->value_names[Index])) {

	    if ((RepTypeID == XmRID_EDIT_MODE) ||
		(RepTypeID == XmRID_WHICH_BUTTON) ||
		(RepTypeID == XmRID_FONT_TYPE))
	      {
		/* special case for int sized fields */

		int conversion_buffer ;
		conversion_buffer = (int)
		    ((Record->values) ? Record->values[Index] : Index) ;

		_XmProcessUnlock();
		_XM_CONVERTER_DONE (to, int, conversion_buffer, ;)

	     } else {

		unsigned char conversion_buffer  ;
		conversion_buffer = (unsigned char)
		    ((Record->values) ? Record->values[Index] : Index) ;

		_XmProcessUnlock();
		_XM_CONVERTER_DONE (to, unsigned char, conversion_buffer, ;)
	     }
	}
	++Index ;
    }

    _XmProcessUnlock();
     XtDisplayStringConversionWarning( disp, in_str, Record->rep_type_name);

    return( FALSE) ;
}

static Boolean
ReverseConvertRepType(
        Display *disp,
        XrmValue *args,
        Cardinal *n_args,	/* unused */
        XrmValue *from,
        XrmValue *to,
        XtPointer *converter_data) /* unused */
{
    XtPointer argvalue = *(XtPointer *)args[0].addr;
    XmRepTypeId RepTypeID = (XmRepTypeId)(long)argvalue;
    XmRepTypeEntry Record;
    unsigned char in_value = *(unsigned char *) from->addr ;
    unsigned short NumValues;
    char **OutValue = NULL ;
    String in_str ;
    String reverse_message = MESSAGE0 ;

    _XmProcessLock();
    Record = GetRepTypeRecord( RepTypeID);
    NumValues = Record->num_values;

    if(Record->values)  {   /* mapped type */
	  unsigned short Index = 0 ;

	  while(Index < NumValues    )  {
		if(in_value == Record->values[Index]    )
		  {
		      OutValue = (char **) &Record->value_names[Index] ;
		      break ;
		  }
		++Index ;
            }
      } else {
	  if(    in_value < NumValues    )
	    {
		OutValue = (char **) &Record->value_names[in_value] ;
            }
      }


    _XmProcessUnlock();
#if 0
    if (OutValue)  _XM_CONVERTER_DONE (to, String, *OutValue, ;)
#else
    if (OutValue)
    {
	if (to->addr)
	{
	    if (to->size < sizeof(String))
	    {
		to->size = sizeof(String);
	    	return(False);
	    }
	    else
	    {
		to->addr = (XtPointer)*OutValue;
	    }
	}
	else
	{
	    to->addr = (XtPointer)OutValue;
	}
	to->size = sizeof(String);
	return(True);
    }
#endif


	/** generate a message and display it */
    in_str = (char *) XtMalloc (strlen(reverse_message) + 10) ;
    sprintf(in_str, "%s %d", reverse_message, in_value);

    XtDisplayStringConversionWarning( disp, in_str, Record->rep_type_name) ;

    return( FALSE) ;
}

void
_XmRepTypeInstallConverters( void )
{
    unsigned short Index = XmRID_UNIT_TYPE ;
    XtConvertArgRec convertArg;

    /* in order to be able to implement the XmCvtStringToUnitType
       converter as using the native code in RepType, we must
       have a different unit type converter name to refer to from
       ResConvert.c when calling XtConvertAndStore */
    convertArg.address_mode = XtImmediate;
    convertArg.address_id   = (XPointer)(long)Index;
    convertArg.size         = sizeof(convertArg.address_id);
    XtSetTypeConverter(XmRString,
		       REAL_UNIT_TYPE_NAME,
		       ConvertRepType, &convertArg, 1,
		       XtCacheNone, NULL) ;

    /* Install the static consecutive-valued converters. */
    for ( Index = 0; Index < StandardNumRecs; Index ++ ) {

      /* Special case the record used for the action param, where
	 we don't need to install the converter. For these, we
	 have used the reverse_installed field set to True to
	 notify this routine not to install them */
      if (StandardRepTypes[Index].reverse_installed) continue ;

      /* only update the index data, the other field are already good */
      convertArg.address_id = (XPointer)(long)Index;

      XtSetTypeConverter(XmRString,
			 StandardRepTypes[Index].rep_type_name,
			 ConvertRepType, &convertArg, 1,
			 XtCacheNone, NULL) ;
     XmRepTypeAddReverse(Index);
    }
}

void
XmRepTypeInstallTearOffModelConverter( void )
{
  /* Obsolete,  we now do this by default */
}

/*
 * Given a rep_type_id and a string parameter, this function tries to
 * find the reptype value for that string. The resulting reptype value is
 * returned in result.  For backwards compatibility, the string parameter
 * for some actions could be a numeric rather than an alphabetic string
 * (i.e. "0"). If an action parameter reptype is able to accept a
 * numeric, then pass True for the can_be_numeric parameter. This will
 * cause the function to first parse the string as a numeric. If it
 * succeeds in doing so, it checks to verify that the value is a valid
 * value for the given reptype. If it can't parse the string as a
 * numeric, it tries to do the rep type lookup. If both fail, it returns
 * False and does not set result. If either succeeds, it returns True and
 * sets the result parameter to be the resulting numeric value. The
 * widget parameter is used by various warning message functions so any
 * widget may be passed.
 */

Boolean
_XmConvertActionParamToRepTypeId(Widget widget, XmRepTypeId rep_type_id,
				 char *parameter, Boolean can_be_numeric,
				 int *result)
{
    int value, i;
    XtPointer aligned_value;
    XrmValue args, from, to;

    /* If the parameter can be numeric (for backward compatibility) then
       try to convert the parameter to a number */
    if (can_be_numeric)
    {
	value = i = 0;
	while (isspace(parameter[i])) ++i; /* skip leading white space */
	if (isdigit(parameter[i]))
	{
	    value = atoi(parameter + i);
	    /* If the number was converted, verify that it is a valid value
	       for the reptype. If so return it in the result argument and
	       return True. Otherwise, don't set the result argument and
	       return False. */
	    if (XmRepTypeValidValue(rep_type_id, value, widget))
	    {
		*result = value;
		return(True);
	    }
	    return(False);
	}
    }

    /* If we made it this far, then the parameter can't be numeric or can
       be numeric but we were unable to parse it as such. Try to convert
       the parameter via the rep type converters. */
    args.size = sizeof(rep_type_id);
    aligned_value = (XtPointer)(long)(rep_type_id);
    args.addr = (char*)(&aligned_value);
    from.size = sizeof(char *);
    from.addr = parameter;
    to.size = sizeof(unsigned char);
    to.addr = (XPointer) &value;

    if (ConvertRepType(XtDisplay(widget), &args, NULL, &from, &to, NULL))
    {
	/* We converted okay. Set up result and return True. */
	*result = *((unsigned char *)(to.addr));
	return(True);
    }

    /* All conversions failed. Just return False. */
    return(False);
}


#ifdef DEBUG
void _XmCheckStandardNumRecs() {
    Cardinal Index ;
    XmRepTypeEntry Record, PrevRecord = NULL ;

    for ( Index = 0; Index < StandardNumRecs; Index++ )
      {
	  Record = &(StandardRepTypes[Index]) ;

	  printf("Record[%d]: %s", Index, Record->rep_type_name) ;
	  if (PrevRecord)
	      if (strcmp(Record->rep_type_name,
			 PrevRecord->rep_type_name) <= 0)
		  printf(" ** UNSORTED ENTRY **");
	  printf("\n");

	  PrevRecord = Record ;

      }
}

#endif
