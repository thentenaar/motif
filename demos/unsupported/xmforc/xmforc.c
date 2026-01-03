/* $TOG: xmforc.c /main/6 1997/03/31 13:42:03 dbl $ */
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
 */

/*
**  The demo code demonstrates how to use Form attachments and
**  the demo by itself illustrates the RowColumn layout resources.
**
*/

#include <Xm/XmAll.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_STDINT_H
#include <stdint.h>
#elif HAVE_INTTYPES_H
#include <inttypes.h>
#endif

/*-------------------------------------------------------------
**	forwarded functions
*/
void CreateApplication (Widget parent);
void CreateForms (Widget parent);
void LateSelfAttach (Widget form);
void LateCenterAttach (Widget form);
Widget CreateHelp (Widget parent);
Widget CreateRC (Widget parent);
void ResizeHandler (Widget, XtPointer, XEvent *, Boolean *) ;
void SetSensitiveScaleScrollBar (Widget scale, Boolean sensitive);
void	CreateResDialog(void);
void UpdateResDialog(Widget widget);

/*      Xt callbacks
*/
void QuitCB (Widget w, XtPointer client_data, XtPointer call_data);
void HelpCB (Widget w, XtPointer client_data, XtPointer call_data);

void TextFieldCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void PackingCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void EntryAlignmentCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void OrientationCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void NumColCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void WidthCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void ResizeWidthCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void ResizeHeightCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void HeightCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void AdjustLastCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void IsAlignedCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void AdjustMarginCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void NumChildCB (Widget w, XtPointer client_data, XtPointer call_data) ;

void EntryCB (Widget w, XtPointer client_data, XtPointer call_data) ;
void ResOkCB (Widget w, XtPointer client_data, XtPointer call_data) ;

/*-------------------------------------------------------------
**	RowCol structure
*/

typedef struct {
    Widget rc ;
    Cardinal num_children;

    /* some widget ids to be kept around */
    Widget form, form1, scale_width, scale_height, entry_align_option ;

    /* rowcol resources set by the user */
    unsigned char packing ;
    unsigned char orientation ;
    short num_columns ;
    Boolean resize_width, resize_height ;
    Dimension width, height ;
    Boolean adjust_last, adjust_margin, is_aligned ;
    unsigned char entry_alignment ;
    Dimension margin_width, margin_height, spacing, entry_border ;

    Widget cur_button, res_dialog, res_form ;

} RC_Data ;

static RC_Data rc_data ;
static XtAppContext app_context;

/*-------------------------------------------------------------
**	    Main body
*/
int main(int argc, char *argv[])
{
    Widget      toplevel ;

    toplevel = XtAppInitialize(&app_context, "XMdemos", NULL, 0,
			       &argc, argv, NULL, NULL, 0);

    CreateApplication (toplevel);

    XtRealizeWidget(toplevel);

    if (argc != 2) LateSelfAttach(rc_data.form) ;
    LateCenterAttach(rc_data.form1) ;

    /* have to do that because of scrollbar sensitivity always linked
       with scale one */
    SetSensitiveScaleScrollBar (rc_data.scale_width, !rc_data.resize_width);
    XtAppMainLoop(app_context);
    return 0;
}

/*-------------------------------------------------------------
**	Create a MainWindow with a Form in it.
*/
void
CreateApplication (Widget parent)
{
    Widget main_window, menu_bar, menu_pane, cascade, button ;
    Arg args[5];
    int	n ;


    /*	Create app_defined MainWindow.
     */
    n = 0;
    XtSetArg (args[n], XmNscrollingPolicy, XmAPPLICATION_DEFINED);  n++;
    main_window = XmCreateMainWindow (parent, "main_window", args, n);
    XtManageChild (main_window);


    /*	Create MenuBar in MainWindow.
     */
    n = 0;
    menu_bar = XmCreateMenuBar (main_window, "menu_bar", args, n);
    XtManageChild (menu_bar);


    /*	Create "File" PulldownMenu with Quit button
     */
    n = 0;
    menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane", args, n);

    n = 0;
    button = XmCreatePushButton (menu_pane, "Quit", args, n);
    XtManageChild (button);
    XtAddCallback (button, XmNactivateCallback, QuitCB, NULL);

    n = 0;
    XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
    cascade = XmCreateCascadeButton (menu_bar, "File", args, n);
    XtManageChild (cascade);


    /*	Create "Help" PulldownMenu with Help button.
     */
    n = 0;
    menu_pane = XmCreatePulldownMenu (menu_bar, "menu_pane", args, n);

    n = 0;
    button = XmCreatePushButton (menu_pane, "Help", args, n);
    XtManageChild (button);
    XtAddCallback (button, XmNactivateCallback, HelpCB, NULL);

    n = 0;
    XtSetArg (args[n], XmNsubMenuId, menu_pane);  n++;
    cascade = XmCreateCascadeButton (menu_bar, "Help", args, n);
    XtManageChild (cascade);

    n = 0;
    XtSetArg (args[n], XmNmenuHelpWidget, cascade);  n++;
    XtSetValues (menu_bar, args, n);


    /*	Create my complex forms in the MainWindow
     */
    CreateForms(main_window) ;


    /*	Set MainWindow areas
     */
    XmMainWindowSetAreas (main_window, menu_bar, NULL, NULL, NULL,
			  rc_data.form);


}


/*-------------------------------------------------------------
**	CreateForms	- create forms and title in Frame
**
**	This form is built by lines. There are 5 lines, one line
**      for the title, one line for the separator under the title, one
**	for the option menus, one for the scale and one for 2 sub-forms.
**	Within each line, the widgets are vertically attached to the
**	first item in the line, using 2 opposite_widget attachment, which
**	make the line totally driven by the first item.
**	Vertical attachment between lines are thus made only using the
**	first item in each line.
**	Horizontal attachments within a line are made using attach widget
**	(except for the third line where I use positionning). Now the problem
**	with attach_widget is that only the last item in a line will be
**	resized (and you can't use positionning since you don't know the
**	relative sizes of each child in a line). So I'm gonna use late
**      binding and attach_self: all the regular widget attachement that
**      I set here (not the opposite_widget ones) will be changed in
**      attach_self after the first layout has been done.
**
**      After the first layout (read: after XtRealizeWidget(topshell)),
**      I'm also going to do some centering that can't be done statically.
**
*/
void
CreateForms (Widget parent)
{
    Widget title, sep, form2,
           textfield1, textfield2, textfield3, textfield4,
           label1, label2, label3, label4,
           option1, option2, button, pulldown,
           toggle1, toggle2, toggle3, scale1, scale2 ;
    Arg args[20];
    int	n ;

    n = 0;
    rc_data.form = XmCreateForm(parent, "main_form", args, n);
    XtManageChild (rc_data.form);

    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    title = XmCreateLabel(rc_data.form, "rowcol_layout", args, n);
    XtManageChild (title);

    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, title);  n++;
    sep = XmCreateSeparator(rc_data.form, "separator", args, n);
    XtManageChild (sep);

    n = 0;
    pulldown = XmCreatePulldownMenu(rc_data.form, "packing_pulldown", args, n);

    n = 0;
    button = XmCreatePushButton(pulldown, "pack_tight", args, n);
    XtAddCallback (button, XmNactivateCallback, PackingCB,
		   (XtPointer) XmPACK_TIGHT);
    XtManageChild(button);
    rc_data.packing = XmPACK_TIGHT ;

    n = 0;
    button = XmCreatePushButton(pulldown, "pack_column", args, n);
    XtAddCallback (button, XmNactivateCallback, PackingCB,
		   (XtPointer) XmPACK_COLUMN);
    XtManageChild(button);

    n = 0;
    button = XmCreatePushButton(pulldown, "pack_none", args, n);
    XtAddCallback (button, XmNactivateCallback, PackingCB,
		   (XtPointer) XmPACK_NONE);
    XtManageChild(button);

    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNleftPosition, 10);  n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, title);  n++;
    XtSetArg (args[n], XmNsubMenuId, pulldown); n++;
    option1 = XmCreateOptionMenu(rc_data.form, "packing_option", args, n);
    XtManageChild (option1);

    n = 0;
    pulldown = XmCreatePulldownMenu(rc_data.form, "orientation_pulldown",
				    args, n);

    n = 0;
    button = XmCreatePushButton(pulldown, "vertical", args, n);
    XtAddCallback (button, XmNactivateCallback, OrientationCB,
		   (XtPointer) XmVERTICAL);
    XtManageChild(button);
    rc_data.orientation = XmVERTICAL ;

    n = 0;
    button = XmCreatePushButton(pulldown, "horizontal", args, n);
    XtAddCallback (button, XmNactivateCallback, OrientationCB,
		   (XtPointer) XmHORIZONTAL);
    XtManageChild(button);


#define XtSetArg_HorAlignWith(widget)    \
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;\
    XtSetArg (args[n], XmNtopWidget, widget);  n++;\
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;\
    XtSetArg (args[n], XmNbottomWidget, widget);  n++

    n = 0;
    XtSetArg_HorAlignWith(option1) ;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNrightPosition, 90);  n++;
    XtSetArg (args[n], XmNsubMenuId, pulldown); n++;
    option2 = XmCreateOptionMenu(rc_data.form, "orientation_option", args, n);
    XtManageChild (option2);

    rc_data.num_columns = 1 ;
    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, option1);  n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetArg (args[n], XmNshowValue, True);  n++;
    XtSetArg (args[n], XmNminimum, 1);  n++;
    XtSetArg (args[n], XmNvalue, rc_data.num_columns);  n++;
    scale1 = XmCreateScale(rc_data.form, "num_columns", args, n);
    XtAddCallback (scale1, XmNvalueChangedCallback, NumColCB, NULL);
    XtManageChild (scale1);

    n = 0;
    label1 = XmCreateLabel(scale1, "num_column_label", args, n);
    XtManageChild (label1);

    rc_data.num_children = 0 ;
    n = 0;
    XtSetArg_HorAlignWith(scale1) ;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, scale1);  n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetArg (args[n], XmNshowValue, True);  n++;
    XtSetArg (args[n], XmNminimum, 0);  n++;
    XtSetArg (args[n], XmNvalue, rc_data.num_children);  n++;
    scale2 = XmCreateScale(rc_data.form, "num_children", args, n);
    XtAddCallback (scale2, XmNvalueChangedCallback, NumChildCB, NULL);
    XtManageChild (scale2);
    /* num_children is used everywhere for checking existence
       of the rowcolumn */

    n = 0;
    label1 = XmCreateLabel(scale2, "num_children_label", args, n);
    XtManageChild (label1);

    n = 0;
    XtSetArg_HorAlignWith(scale2) ;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, scale2);  n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetArg (args[n], XmNshowValue, True);  n++;
    XtSetArg (args[n], XmNminimum, 1);  n++;
    rc_data.scale_width = XmCreateScale(rc_data.form, "resize_width", args, n);
    XtAddCallback (rc_data.scale_width, XmNvalueChangedCallback,
		   WidthCB, NULL);
    XtManageChild (rc_data.scale_width);
    /*  rc_data.width will be initialized with the first resize */


    rc_data.resize_width = True ;
    n = 0;
    XtSetArg (args[n], XmNset, rc_data.resize_width);  n++;
    toggle1 = XmCreateToggleButton(rc_data.scale_width, "resize_width_toggle",
				   args, n);
    XtAddCallback (toggle1, XmNvalueChangedCallback, ResizeWidthCB, NULL);
    XtManageChild (toggle1);
    SetSensitiveScaleScrollBar (rc_data.scale_width, !rc_data.resize_width);

    n = 0;
    XtSetArg_HorAlignWith(rc_data.scale_width) ;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, rc_data.scale_width);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNorientation, XmHORIZONTAL);  n++;
    XtSetArg (args[n], XmNshowValue, True);  n++;
    XtSetArg (args[n], XmNminimum, 1);  n++;
    rc_data.scale_height = XmCreateScale(rc_data.form, "resize_height",
					 args, n);
    XtAddCallback (rc_data.scale_height, XmNvalueChangedCallback, HeightCB,
		   NULL);
    XtManageChild (rc_data.scale_height);
    /*  rc_data.height will be initialized with the first resize */


    rc_data.resize_height = True ;
    n = 0;
    XtSetArg (args[n], XmNset, rc_data.resize_height);  n++;
    toggle1 = XmCreateToggleButton(rc_data.scale_height,
				   "resize_height_toggle", args, n);
    XtAddCallback (toggle1, XmNvalueChangedCallback, ResizeHeightCB, NULL);
    XtManageChild (toggle1);
    SetSensitiveScaleScrollBar (rc_data.scale_height, !rc_data.resize_height);


    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, scale1);  n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
    rc_data.form1 = XmCreateForm(rc_data.form, "form1", args, n);
    XtManageChild (rc_data.form1);

    rc_data.adjust_last = True ;
    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNset, rc_data.adjust_last);  n++;
    toggle1 = XmCreateToggleButton(rc_data.form1, "adjust_last_toggle",
				   args, n);
    XtAddCallback (toggle1, XmNvalueChangedCallback, AdjustLastCB, NULL);
    XtManageChild (toggle1);

    rc_data.adjust_margin = True ;
    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNtopPosition, 25);  n++;
    XtSetArg (args[n], XmNset, rc_data.adjust_margin);  n++;
    toggle2 = XmCreateToggleButton(rc_data.form1, "adjust_margin_toggle",
				  args, n);
    XtAddCallback (toggle2, XmNvalueChangedCallback, AdjustMarginCB, NULL);
    XtManageChild (toggle2);

    rc_data.is_aligned = True ;
    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNtopPosition, 50);  n++;
    XtSetArg (args[n], XmNset, rc_data.is_aligned);  n++;
    toggle3 = XmCreateToggleButton(rc_data.form1, "is_aligned_toggle",
				   args, n);
    XtAddCallback (toggle3, XmNvalueChangedCallback, IsAlignedCB, NULL);
    XtManageChild (toggle3);


    n = 0;
    pulldown = XmCreatePulldownMenu(rc_data.form1, "entry_align_pulldown",
				    args, n);

    n = 0;
    button = XmCreatePushButton(pulldown, "align_beginning", args, n);
    XtAddCallback (button, XmNactivateCallback, EntryAlignmentCB,
		   (XtPointer) XmALIGNMENT_BEGINNING);
    XtManageChild(button);
    rc_data.entry_alignment = XmALIGNMENT_BEGINNING ;

    n = 0;
    button = XmCreatePushButton(pulldown, "align_center", args, n);
    XtAddCallback (button, XmNactivateCallback, EntryAlignmentCB,
		   (XtPointer) XmALIGNMENT_CENTER);
    XtManageChild(button);

    n = 0;
    button = XmCreatePushButton(pulldown, "align_end", args, n);
    XtAddCallback (button, XmNactivateCallback, EntryAlignmentCB,
		   (XtPointer) XmALIGNMENT_END);
    XtManageChild(button);

    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNtopPosition, 77);  n++;
    XtSetArg (args[n], XmNsubMenuId, pulldown); n++;
    rc_data.entry_align_option = XmCreateOptionMenu(rc_data.form1,
				 "entry_alignment", args, n);
    XtManageChild (rc_data.entry_align_option);

    n = 0;
    XtSetArg_HorAlignWith(rc_data.form1) ;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, rc_data.form1);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    form2 = XmCreateForm(rc_data.form, "form2", args, n);
    XtManageChild (form2);

    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNtopPosition, 0);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_FORM);  n++;
    XtSetArg (args[n], XmNleftOffset, -100);  n++;
    XtSetArg (args[n], XmNcolumns, 4);  n++;
    XtSetArg (args[n], XmNvalue, "3");  n++;
    textfield1 = XmCreateTextField(form2, "margin_width_text", args, n);
    XtAddCallback (textfield1, XmNactivateCallback, TextFieldCB,
		   XmNmarginWidth);
    XtManageChild (textfield1);
    rc_data.margin_width = 3 ;

    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNtopPosition, 25);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, textfield1);  n++;
    XtSetArg (args[n], XmNcolumns, 4);  n++;
    XtSetArg (args[n], XmNvalue, "3");  n++;
    textfield2 = XmCreateTextField(form2, "margin_height_text", args, n);
    XtAddCallback (textfield2, XmNactivateCallback, TextFieldCB,
		   XmNmarginHeight);
    XtManageChild (textfield2);
    rc_data.margin_height = 3 ;

    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNtopPosition, 50);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, textfield1);  n++;
    XtSetArg (args[n], XmNcolumns, 4);  n++;
    XtSetArg (args[n], XmNvalue, "3");  n++;
    textfield3 = XmCreateTextField(form2, "spacing_text", args, n);
    XtAddCallback (textfield3, XmNactivateCallback, TextFieldCB,
		   XmNspacing);
    XtManageChild (textfield3);
    rc_data.spacing = 3 ;

    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNtopPosition, 75);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, textfield1);  n++;
    XtSetArg (args[n], XmNcolumns, 4);  n++;
    XtSetArg (args[n], XmNvalue, "0");  n++;
    textfield4 = XmCreateTextField(form2, "entry_border_text", args, n);
    XtAddCallback (textfield4, XmNactivateCallback, TextFieldCB,
		   XmNentryBorder);
    XtManageChild (textfield4);
    rc_data.entry_border = 0 ;

    n = 0;
    XtSetArg_HorAlignWith(textfield1);
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNrightWidget, textfield1);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    label1 = XmCreateLabel(form2, "margin_width", args, n);
    XtManageChild (label1);

    n = 0;
    XtSetArg_HorAlignWith(textfield2);
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNrightWidget, textfield2);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    label2 = XmCreateLabel(form2, "margin_height", args, n);
    XtManageChild (label2);

    n = 0;
    XtSetArg_HorAlignWith(textfield3);
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNrightWidget, textfield3);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    label3 = XmCreateLabel(form2, "spacing", args, n);
    XtManageChild (label3);

    n = 0;
    XtSetArg_HorAlignWith(textfield4);
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNrightWidget, textfield4);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_FORM);  n++;
    label4 = XmCreateLabel(form2, "entry_border", args, n);
    XtManageChild (label4);
}


/*-------------------------------------------------------------
**	LateSelfAttach
**
**	Run thru the child list of this form and
**	change all the attach_widget target in attach_self, so that
**	prorating happens on resize.
**
*/
void
LateSelfAttach(Widget form)
{
    Arg args[5];
    int	n, i ;
    Widget * children, left_widget, top_widget ;
    Cardinal num_children ;
    unsigned char left_attach, top_attach ;

    n = 0;
    XtSetArg (args[n], XmNnumChildren, &num_children);  n++;
    XtSetArg (args[n], XmNchildren, &children);  n++;
    XtGetValues (form, args, n);

    for (i=0; i < num_children; i++) {
	n = 0;
	XtSetArg (args[n], XmNleftAttachment, &left_attach);  n++;
	XtSetArg (args[n], XmNleftWidget, &left_widget);  n++;
	XtSetArg (args[n], XmNtopAttachment, &top_attach);  n++;
	XtSetArg (args[n], XmNtopWidget, &top_widget);  n++;
	XtGetValues (children[i], args, n);

	if (left_attach == XmATTACH_WIDGET) {
	    n = 0;
	    XtSetArg (args[n], XmNrightAttachment, XmATTACH_SELF);  n++;
	    XtSetArg (args[n], XmNrightOffset, 0);  n++;
	    XtSetValues (left_widget, args, n);
	}

	if (top_attach == XmATTACH_WIDGET) {
	    n = 0;
	    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_SELF);  n++;
	    XtSetArg (args[n], XmNbottomOffset, 0);  n++;
	    XtSetValues (top_widget, args, n);
	}
    }
}

/*-------------------------------------------------------------
**	LateCenterAttach
**
**	Run thru the child list of this form and
**	center the children horizontally, using their current width.
**
*/
void
LateCenterAttach(Widget form)
{
    Arg args[5];
    int	n, i ;
    Widget * children ;
    Cardinal num_children ;
    Dimension width ;

    n = 0;
    XtSetArg (args[n], XmNnumChildren, &num_children);  n++;
    XtSetArg (args[n], XmNchildren, &children);  n++;
    XtGetValues (form, args, n);

    for (i=0; i < num_children; i++) {
	n = 0;
	XtSetArg (args[n], XmNwidth, &width);  n++;
	XtGetValues (children[i], args, n);

	n = 0;
	XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);  n++;
	XtSetArg (args[n], XmNleftPosition, 50);  n++;
	XtSetArg (args[n], XmNleftOffset, -((int)width)/2);  n++;
	XtSetValues (children[i], args, n);
    }
}




/*-------------------------------------------------------------
**	QuitCB			- callback for cancel button
*/
void
QuitCB (Widget		w,		/*  widget id		*/
	XtPointer	client_data,	/*  data from application   */
	XtPointer	call_data)	/*  data from widget class  */
{
	(void)w;
	(void)client_data;
	(void)call_data;
	XtAppSetExitFlag(app_context);
}


/*-------------------------------------------------------------
**	HelpCB			- callback for help button
*/
void
HelpCB (Widget		w,		/*  widget id		*/
	XtPointer	client_data,	/*  data from application   */
	XtPointer	call_data)	/*  data from widget class  */
{
	static Widget message_box = NULL ;

	if (!message_box) message_box = CreateHelp (w);

	XtManageChild (message_box);
}


/*-------------------------------------------------------------
**	CreateHelp		- create help window
*/
Widget
CreateHelp (Widget parent)		/*  parent widget	*/
{
	Widget		message_box;	/*  Message Dialog 	*/
	Arg		args[20];	/*  arg list		*/
	int	n;		/*  arg count		*/

	static char	message[1000];	/*  help text	*/
	XmString	title_string = NULL;
	XmString	message_string = NULL;
	XmString	button_string = NULL;

	/*	Generate message to display.
	*/
	sprintf (message, "\
Use the numChildren resource to control the creation and destruction\n\
of the RowColumn. Then use the other controls to manipulate the RowColumn\n\
layout resources.");
	message_string = XmStringCreateLocalized(message);
	button_string  = XmStringCreateLocalized("Close");
	title_string   = XmStringCreateLocalized("General Help");

	/*	Create MessageBox dialog.
	*/
	n = 0;
	XtSetArg (args[n], XmNdialogTitle, title_string);  n++;
	XtSetArg (args[n], XmNokLabelString, button_string);  n++;
	XtSetArg (args[n], XmNmessageString, message_string);  n++;
	message_box = XmCreateMessageDialog (parent, "helpbox", args, n);
	XtUnmanageChild(XtNameToWidget(message_box, "Cancel"));
	XtUnmanageChild(XtNameToWidget(message_box, "Help"));

	/*	Free strings and return MessageBox.
	*/
	if (title_string) XmStringFree (title_string);
	if (message_string) XmStringFree (message_string);
	if (button_string) XmStringFree (button_string);

	return (message_box);
}


/*-------------------------------------------------------------
**	NumChildCB
*/
void
NumChildCB (Widget	w,		/*  widget id		*/
	    XtPointer	client_data,	/*  data from application   */
	    XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[1] ;
    XmScaleCallbackStruct * scb = (XmScaleCallbackStruct *) call_data ;
    Widget * children, new_child ;
    char name[20] ;
    int i ;
    Cardinal num_children ;

    /* the valueChangeCallback might get called with the same value */
    if (rc_data.num_children == scb->value) return ;

    if (scb->value == 0) {
	/* assert: rc already exists, since we moved from something
	           not null to 0 */

	XtDestroyWidget(XtParent(rc_data.rc));
	rc_data.num_children = 0 ;
	return ;
    }

    /* assert: value != num_children  */

    if (scb->value > rc_data.num_children) {

	/* num_children can be zero */
	if (rc_data.num_children == 0) rc_data.rc = CreateRC(w);

	/* add buttons at the end of the child list */
	for (i = rc_data.num_children ; i < scb->value; i++) {
	    sprintf(name, "push%d", i);
	    new_child = XmCreatePushButton (rc_data.rc, name, NULL, 0);
	    XtManageChild(new_child);
	}
    } else {
	XtSetArg (args[0], XmNchildren, &children);
	XtGetValues (rc_data.rc, args, 1);

	for (i = rc_data.num_children ; i > scb->value; i--) {
	    XtDestroyWidget(children[i-1]);
	}
    }

    rc_data.num_children = scb->value ;

}


/*-------------------------------------------------------------
**	HeightCB
*/
void
HeightCB (Widget	w,		/*  widget id		*/
	  XtPointer	client_data,	/*  data from application   */
	  XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[1] ;
    XmScaleCallbackStruct * scb = (XmScaleCallbackStruct *) call_data ;

    rc_data.height = scb->value ;

    if (rc_data.num_children) {
	XtSetArg(args[0], XmNheight, scb->value);
	XtSetValues(rc_data.rc, args, 1);
    }
}


/*-------------------------------------------------------------
**	AdjustMarginCB
*/
void
AdjustMarginCB (Widget		w,		/*  widget id		*/
		XtPointer	client_data,	/*  data from application   */
		XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[1] ;
    XmToggleButtonCallbackStruct * scb =
	(XmToggleButtonCallbackStruct *) call_data ;

    rc_data.adjust_margin = scb->set ;

    if (rc_data.num_children) {
	XtSetArg(args[0], XmNadjustMargin, scb->set);
	XtSetValues(rc_data.rc, args, 1);
    }
}

/*-------------------------------------------------------------
**	IsAlignedCB
*/
void
IsAlignedCB (Widget		w,		/*  widget id		*/
	     XtPointer		client_data,	/*  data from application   */
	     XtPointer		call_data)	/*  data from widget class  */
{
    Arg args[1] ;
    XmToggleButtonCallbackStruct * scb =
	(XmToggleButtonCallbackStruct *) call_data ;

    rc_data.is_aligned = scb->set ;

    if (rc_data.num_children) {
	XtSetArg(args[0], XmNisAligned, scb->set);
	XtSetValues(rc_data.rc, args, 1);
    }

/*    XtSetSensitive(rc_data.entry_align_option, scb->set);*/

}

/*-------------------------------------------------------------
**	AdjustLastCB
*/
void
AdjustLastCB (Widget		w,		/*  widget id		*/
	      XtPointer		client_data,	/*  data from application   */
	      XtPointer		call_data)	/*  data from widget class  */
{
    Arg args[1] ;
    XmToggleButtonCallbackStruct * scb =
	(XmToggleButtonCallbackStruct *) call_data ;

    rc_data.adjust_last = scb->set ;

    if (rc_data.num_children) {
	XtSetArg(args[0], XmNadjustLast, scb->set);
	XtSetValues(rc_data.rc, args, 1);
    }
}

/*-------------------------------------------------------------
**	ResizeWidthCB
*/
void
ResizeWidthCB (Widget		w,		/*  widget id		*/
	       XtPointer	client_data,	/*  data from application   */
	       XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[4] ;
    Cardinal n ;
    XmToggleButtonCallbackStruct * scb =
	(XmToggleButtonCallbackStruct *) call_data ;

    rc_data.resize_width = scb->set ;

    if (rc_data.num_children) {
	n = 0 ;
	XtSetArg(args[n], XmNresizeWidth, scb->set); n++ ;
	/* Nice hack: when we switch from resizeWidth False to True,
	   width will be ignored, but since we want a relayout
	   to happen, to reflect the new shrink wrap stuff,
	   we need to ask for something different
	   than the current width. So we use (int)scb->set which is 0
	   resizeWidth = False, and adds 1 when it's True */
	XtSetArg(args[n], XmNwidth, rc_data.width + (int)scb->set); n++ ;
	XtSetValues(rc_data.rc, args, n);
    }

    SetSensitiveScaleScrollBar (rc_data.scale_width, !scb->set) ;
}


/*-------------------------------------------------------------
**	ResizeHeightCB
*/
void
ResizeHeightCB (Widget		w,		/*  widget id		*/
		XtPointer	client_data,	/*  data from application   */
		XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[4] ;
    Cardinal n ;
    XmToggleButtonCallbackStruct * scb =
	(XmToggleButtonCallbackStruct *) call_data ;

    rc_data.resize_height = scb->set ;

    if (rc_data.num_children) {
	n = 0 ;
	/* same trick as resize_width, see above*/
	XtSetArg(args[n], XmNresizeHeight, scb->set); n++ ;
	XtSetArg(args[n], XmNheight, rc_data.height + (int)scb->set); n++ ;
	XtSetValues(rc_data.rc, args, n);
    }

    SetSensitiveScaleScrollBar (rc_data.scale_height, !scb->set) ;
}

/*-------------------------------------------------------------*/
void
SetSensitiveScaleScrollBar (Widget scale,
			    Boolean sensitive)
{
    Arg args[4] ;
    Cardinal n ;
    Widget * children ;

    n = 0;
    XtSetArg (args[n], XmNchildren, &children);  n++;
    XtGetValues (scale, args, n);

    /* everybody knows that this is the scrollbar... */
    XtSetSensitive(children[1], sensitive);
}

/*-------------------------------------------------------------
**	WidthCB
*/
void
WidthCB (Widget		w,		/*  widget id		*/
	 XtPointer	client_data,	/*  data from application   */
	 XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[1] ;
    XmScaleCallbackStruct * scb = (XmScaleCallbackStruct *) call_data ;

    rc_data.width = scb->value ;

    if (rc_data.num_children) {
	XtSetArg(args[0], XmNwidth, scb->value);
	XtSetValues(rc_data.rc, args, 1);
    }
}


/*-------------------------------------------------------------
**	NumColCB
*/
void
NumColCB (Widget	w,		/*  widget id		*/
	  XtPointer	client_data,	/*  data from application   */
	  XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[1] ;
    XmScaleCallbackStruct * scb = (XmScaleCallbackStruct *) call_data ;

    if (rc_data.num_columns == scb->value) return ;

    rc_data.num_columns = scb->value ;

    if (rc_data.num_children) {
	XtSetArg(args[0], XmNnumColumns, scb->value);
	XtSetValues(rc_data.rc, args, 1);
    }
}


/*-------------------------------------------------------------
**	OrientationCB
*/
void
OrientationCB (Widget		w,		/*  widget id		*/
	       XtPointer	client_data,	/*  data from application   */
	       XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[1] ;

    rc_data.orientation = (uintptr_t)client_data & 0xff;

    if (rc_data.num_children) {
	XtSetArg(args[0], XmNorientation, client_data);
	XtSetValues(rc_data.rc, args, 1);
    }
}


/*-------------------------------------------------------------
**	EntryAlignmentCB
*/
void
EntryAlignmentCB (Widget	w,		/*  widget id		*/
		  XtPointer	client_data,	/*  data from application   */
		  XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[1] ;

    rc_data.entry_alignment = (uintptr_t)client_data & 0xff;

    if (rc_data.num_children) {
	XtSetArg(args[0], XmNentryAlignment, client_data);
	XtSetValues(rc_data.rc, args, 1);
    }
}


/*-------------------------------------------------------------
**	PackingCB		client_data = resource value
*/
void
PackingCB (Widget	w,		/*  widget id		*/
	   XtPointer	client_data,	/*  data from application   */
	   XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[1] ;

    rc_data.packing = (uintptr_t)client_data & 0xff;

    if (rc_data.num_children) {
	XtSetArg(args[0], XmNpacking, client_data);
	XtSetValues(rc_data.rc, args, 1);
    }
}


/*-------------------------------------------------------------
**	TextFieldCB		client_data = resource name
*/
void
TextFieldCB (Widget	w,		/*  widget id		*/
	     XtPointer	client_data,	/*  data from application   */
	     XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[1] ;
    String string ;
    int val ;

    string = XmTextFieldGetString(w) ;
    if (string) {
	val = atoi(string) ;

	if (strcmp(client_data, XmNmarginWidth))
	    rc_data.margin_width = val ;
	if (strcmp(client_data, XmNmarginHeight))
	    rc_data.margin_height = val ;
	if (strcmp(client_data, XmNspacing))
	    rc_data.spacing = val ;
	if (strcmp(client_data, XmNentryBorder))
	    rc_data.entry_border = val ;

	if (rc_data.num_children) {
	    XtSetArg(args[0], client_data, val);
	    XtSetValues(rc_data.rc, args, 1);
	}
	XtFree(string);
    }
}


/*-------------------------------------------------------------
**	EntryCB		callback for buttons in the RC
*/
void
EntryCB (Widget		w,		/*  widget id		*/
	 XtPointer	client_data,	/*  data from application   */
	 XtPointer	call_data)	/*  data from widget class  */
{
    XmRowColumnCallbackStruct * rcCB =
	    (XmRowColumnCallbackStruct *) call_data ;

    rc_data.cur_button = rcCB->widget;

    if (!rc_data.res_dialog) CreateResDialog();

    UpdateResDialog(rc_data.cur_button);
    XtManageChild (rc_data.res_dialog);

}


/*-------------------------------------------------------------
**	CreateResDialog - create a dialog window for modifying
**                        some current button resources.
*/
void
CreateResDialog (void)
{
    Widget label1, label2, label3, label4, label5,
           textfield1, textfield2, textfield3, textfield4, textfield5 ;
    Arg args[20] ;
    Cardinal n ;

    n = 0 ;
    rc_data.res_dialog = XmCreateMessageDialog(rc_data.rc, "res_dialog",
					       args, n);
    XtAddCallback (rc_data.res_dialog, XmNokCallback, ResOkCB, NULL);

    n = 0 ;
    rc_data.res_form = XmCreateForm(rc_data.res_dialog, "res_form",
				    args, n);
    XtManageChild (rc_data.res_form);

    n = 0;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_POSITION);  n++;
    XtSetArg (args[n], XmNleftPosition, 33);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    textfield1 = XmCreateTextField(rc_data.res_form, "t_x", args, n);
    XtManageChild (textfield1);

    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, textfield1);  n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, textfield1);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    textfield2 = XmCreateTextField(rc_data.res_form, "t_y", args, n);
    XtManageChild (textfield2);

    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, textfield2);  n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, textfield2);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    textfield3 = XmCreateTextField(rc_data.res_form, "t_width", args, n);
    XtManageChild (textfield3);

    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, textfield3);  n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, textfield3);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    textfield4 = XmCreateTextField(rc_data.res_form, "t_height", args, n);
    XtManageChild (textfield4);

    n = 0;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET);  n++;
    XtSetArg (args[n], XmNleftWidget, textfield4);  n++;
    XtSetArg (args[n], XmNtopAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNtopWidget, textfield4);  n++;
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_FORM);  n++;
    XtSetArg (args[n], XmNbottomAttachment, XmATTACH_FORM);  n++;
    textfield5 = XmCreateTextField(rc_data.res_form, "t_recomp_size", args, n);
    XtManageChild (textfield5);


    n = 0;
    XtSetArg_HorAlignWith(textfield1);
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNrightWidget, textfield1);  n++;
    label1 = XmCreateLabel(rc_data.res_form, "l_x", args, n);
    XtManageChild (label1);

    n = 0;
    XtSetArg_HorAlignWith(textfield2);
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNrightWidget, textfield2);  n++;
    label2 = XmCreateLabel(rc_data.res_form, "l_y", args, n);
    XtManageChild (label2);

    n = 0;
    XtSetArg_HorAlignWith(textfield3);
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNrightWidget, textfield3);  n++;
    label3 = XmCreateLabel(rc_data.res_form, "l_width", args, n);
    XtManageChild (label3);

    n = 0;
    XtSetArg_HorAlignWith(textfield4);
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNrightWidget, textfield4);  n++;
    label4 = XmCreateLabel(rc_data.res_form, "l_height", args, n);
    XtManageChild (label4);

    n = 0;
    XtSetArg_HorAlignWith(textfield5);
    XtSetArg (args[n], XmNrightAttachment, XmATTACH_WIDGET);  n++;
    XtSetArg (args[n], XmNrightWidget, textfield5);  n++;
    label5 = XmCreateLabel(rc_data.res_form, "l_recomp_size", args, n);
    XtManageChild (label5);

}


/*-------------------------------------------------------------
**	ResOkCB	     for buttons, called one each text return and OK
*/
void
ResOkCB (Widget		w,		/*  widget id		*/
	 XtPointer	client_data,	/*  data from application   */
	 XtPointer	call_data)	/*  data from widget class  */
{
    Arg args[10] ;
    Cardinal n ;
    char * s_x, * s_y, * s_width, * s_height, *s_recomp_size ;
    Widget * children ;

    /* following code based on the assumption that the children
       in the res_dialog form have a specific order */

    n = 0;
    XtSetArg (args[n], XmNchildren, &children);  n++;
    XtGetValues (rc_data.res_form, args, n);

    s_x = XmTextFieldGetString(children[0]) ;
    s_y = XmTextFieldGetString(children[1]) ;
    s_width = XmTextFieldGetString(children[2]) ;
    s_height = XmTextFieldGetString(children[3]) ;
    s_recomp_size = XmTextFieldGetString(children[4]) ;

    n = 0 ;
    XtSetArg(args[n], XmNx, atoi(s_x)); n++;
    XtSetArg(args[n], XmNy, atoi(s_y)); n++;
    XtSetArg(args[n], XmNwidth, atoi(s_width)); n++;
    XtSetArg(args[n], XmNheight, atoi(s_height)); n++;
    XtSetArg(args[n], XmNrecomputeSize, atoi(s_recomp_size)); n++;
    XtSetValues(rc_data.cur_button, args, n);

    XtFree(s_x);
    XtFree(s_y);
    XtFree(s_width);
    XtFree(s_height);
    XtFree(s_recomp_size);
}


/*-------------------------------------------------------------
**	UpdateResDialog - update the dialog window used to change
**                        this button resources.
*/
void
UpdateResDialog (Widget	widget)		/*  rc child widget	*/
{
    Arg args[20] ;
    Cardinal n ;
    Position b_x, b_y ;
    Dimension b_width, b_height ;
    Boolean b_recomp_size ;
    char s[10] ;
    Widget * children ;
    XmString title ;

    /* first update the name of the dialog */
    title = XmStringCreateLocalized(XtName(widget));
    n = 0;
    XtSetArg (args[n], XmNdialogTitle, title);  n++;
    XtSetValues (rc_data.res_dialog, args, n);
    XmStringFree (title);

    n = 0;
    XtSetArg (args[n], XmNx, &b_x);  n++;
    XtSetArg (args[n], XmNy, &b_y);  n++;
    XtSetArg (args[n], XmNwidth, &b_width);  n++;
    XtSetArg (args[n], XmNheight, &b_height);  n++;
    XtSetArg (args[n], XmNrecomputeSize, &b_recomp_size);  n++;
    XtGetValues (widget, args, n);

    /* following code based on the assumption that the children
       in the res_dialog form have a specific order */

    n = 0;
    XtSetArg (args[n], XmNchildren, &children);  n++;
    XtGetValues (rc_data.res_form, args, n);

    sprintf(s,"%d",b_x);
    n = 0;
    XtSetArg (args[n], XmNvalue, s);  n++;
    XtSetValues (children[0], args, n);

    sprintf(s,"%d",b_y);
    n = 0;
    XtSetArg (args[n], XmNvalue, s);  n++;
    XtSetValues (children[1], args, n);

    sprintf(s,"%d",b_width);
    n = 0;
    XtSetArg (args[n], XmNvalue, s);  n++;
    XtSetValues (children[2], args, n);

    sprintf(s,"%d",b_height);
    n = 0;
    XtSetArg (args[n], XmNvalue, s);  n++;
    XtSetValues (children[3], args, n);

    sprintf(s,"%d",b_recomp_size);
    n = 0;
    XtSetArg (args[n], XmNvalue, s);  n++;
    XtSetValues (children[4], args, n);

}


/*-------------------------------------------------------------
**	CreateRC	- create the rc in a dialog and fill it with
**                        resource coming from rc_data.
*/
Widget
CreateRC (Widget parent)		/*  parent widget	*/
{
    Widget dialog_shell, rc ;
    Arg args[20] ;
    Cardinal n ;

    n = 0 ;
    XtSetArg(args[n], XmNallowShellResize, True); n++ ;
    dialog_shell = XmCreateDialogShell(parent, "RowColumn", args, n);
    XtManageChild(dialog_shell);

    n = 0 ;
    XtSetArg(args[n], XmNpacking, rc_data.packing); n++ ;
    XtSetArg(args[n], XmNorientation, rc_data.orientation); n++ ;
    XtSetArg(args[n], XmNnumColumns, rc_data.num_columns); n++ ;
    XtSetArg(args[n], XmNresizeWidth, rc_data.resize_width); n++ ;
    XtSetArg(args[n], XmNresizeHeight, rc_data.resize_height); n++ ;
    XtSetArg(args[n], XmNwidth, rc_data.width); n++ ;
    XtSetArg(args[n], XmNheight, rc_data.height); n++ ;
    XtSetArg(args[n], XmNadjustLast, rc_data.adjust_last); n++ ;
    XtSetArg(args[n], XmNadjustMargin, rc_data.adjust_margin); n++ ;
    XtSetArg(args[n], XmNisAligned, rc_data.is_aligned); n++ ;
    XtSetArg(args[n], XmNentryAlignment, rc_data.entry_alignment); n++ ;
    XtSetArg(args[n], XmNmarginWidth, rc_data.margin_width); n++ ;
    XtSetArg(args[n], XmNmarginHeight, rc_data.margin_height); n++ ;
    XtSetArg(args[n], XmNspacing, rc_data.spacing); n++ ;
    XtSetArg(args[n], XmNentryBorder, rc_data.entry_border); n++ ;
    rc = XmCreateRowColumn(dialog_shell, "rc", args, n);
    XtManageChild(rc);

    XtAddCallback (rc, XmNentryCallback, EntryCB, NULL);

    XtAddEventHandler(rc, StructureNotifyMask, False, ResizeHandler, NULL);

    return rc ;
}

/*
**        Get here whenever the rowcolumnis move, resize..
**        Have to update the scale value
*/
void
ResizeHandler (Widget widget,
	       XtPointer data,
	       XEvent *event,
	       Boolean *cont)
{
    Arg args[5] ;
    Cardinal n ;

    n = 0 ;
    XtSetArg(args[n], XmNwidth, &(rc_data.width)); n++ ;
    XtSetArg(args[n], XmNheight, &(rc_data.height)); n++ ;
    XtGetValues (widget, args, n);

    /* don't bother checking same value, scale will do it */

    XmScaleSetValue(rc_data.scale_width, rc_data.width);
    XmScaleSetValue(rc_data.scale_height, rc_data.height);

    /* have to do that because of scrollbar sensitivity always linked
       with scale one */
    SetSensitiveScaleScrollBar (rc_data.scale_width, !rc_data.resize_width);
    SetSensitiveScaleScrollBar (rc_data.scale_height, !rc_data.resize_height);


}

