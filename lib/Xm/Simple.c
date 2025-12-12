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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: Simple.c /main/11 1995/09/19 23:08:54 cde-sun $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <stdio.h>
#include <X11/StringDefs.h>
#include <Xm/RowColumnP.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/CascadeBG.h>
#include <Xm/LabelG.h>
#include <Xm/SeparatoG.h>
#include "XmI.h"

/********    Static Function Declarations    ********/

static void EvaluateConvenienceStructure( 
                        Widget wid,
                        XmSimpleMenu sm) ;

/********    End Static Function Declarations    ********/

static XtResource SimpleMenuResources[] =
{
	{ XmNbuttonCount, XmCButtonCount, XmRInt, sizeof(int),
	  XtOffsetOf( struct _XmSimpleMenuRec, count),
	  XmRImmediate, (XtPointer) 0
	},
	{ XmNpostFromButton, XmCPostFromButton, XmRInt, sizeof(int),
	  XtOffsetOf( struct _XmSimpleMenuRec, post_from_button),
	  XmRImmediate, (XtPointer) -1
	},
	{ XmNsimpleCallback, XmCCallback, XmRCallbackProc, 
	  sizeof(XtCallbackProc), XtOffsetOf( struct _XmSimpleMenuRec, callback),
	  XmRImmediate, (XtPointer) NULL
	},
	{ XmNbuttons, XmCButtons, XmRXmStringTable,
	  sizeof(XmStringTable), XtOffsetOf( struct _XmSimpleMenuRec, label_string),
	  XmRImmediate, (XtPointer) NULL
	},
	{ XmNbuttonAccelerators, XmCButtonAccelerators, XmRStringTable, 
	  sizeof(String *), XtOffsetOf( struct _XmSimpleMenuRec,  accelerator),
	  XmRImmediate, (XtPointer) NULL
	},
	{ XmNbuttonAcceleratorText, XmCButtonAcceleratorText, 
	  XmRXmStringTable, sizeof(XmStringTable),
	  XtOffsetOf( struct _XmSimpleMenuRec, accelerator_text),
	  XmRImmediate, (XtPointer) NULL
	},
	{ XmNbuttonMnemonics, XmCButtonMnemonics, XmRKeySymTable,
	  sizeof(XmKeySymTable), XtOffsetOf( struct _XmSimpleMenuRec, mnemonic),
	  XmRImmediate, (XtPointer) NULL
	},
	{ XmNbuttonMnemonicCharSets, XmCButtonMnemonicCharSets, 
	  XmRCharSetTable, sizeof(XmStringCharSetTable),
	  XtOffsetOf( struct _XmSimpleMenuRec, mnemonic_charset),
	  XmRImmediate, (XtPointer) NULL
	},
	{ XmNbuttonType, XmCButtonType, XmRButtonType,
	  sizeof(XmButtonTypeTable), XtOffsetOf( struct _XmSimpleMenuRec, button_type),
	  XmRImmediate, (XtPointer) NULL
	},
	{ XmNbuttonSet, XmCButtonSet, XmRInt,
	  sizeof(int), XtOffsetOf( struct _XmSimpleMenuRec, button_set),
	  XmRImmediate, (XtPointer) -1
	},
	{ XmNoptionLabel, XmCOptionLabel, XmRXmString,
	  sizeof(XmString), XtOffsetOf( struct _XmSimpleMenuRec, option_label),
	  XmRImmediate, (XtPointer) NULL
	},
	{ XmNoptionMnemonic, XmCOptionMnemonic, XmRKeySym,
	  sizeof (KeySym), XtOffsetOf( struct _XmSimpleMenuRec, option_mnemonic),
          XmRImmediate, (XtPointer) NULL
	},
};

static void 
EvaluateConvenienceStructure(
        Widget wid,
        XmSimpleMenu sm )
{
        XmRowColumnWidget rc = (XmRowColumnWidget) wid ;
	int i, n;
	char name_buf[25];
	int button_count = 0;
	int separator_count = 0;
	int label_count = 0;
	Arg args[6];
	Widget child;
	XmButtonType btype;

	for (i = 0; i < sm->count; i++)
	{
		n = 0;
		if (sm->label_string && sm->label_string[i])
		{
			XtSetArg(args[n], XmNlabelString, sm->label_string[i]);
			n++;
		}
		if (sm->accelerator && sm->accelerator[i])
		{
			XtSetArg(args[n], XmNaccelerator, sm->accelerator[i]);
			n++;
		}
		if (sm->accelerator_text && sm->accelerator_text[i])
		{
			XtSetArg(args[n], XmNacceleratorText, 
				sm->accelerator_text[i]); 
			n++;
		}
		if (sm->mnemonic && sm->mnemonic[i])
		{
			XtSetArg(args[n], XmNmnemonic, sm->mnemonic[i]);
			n++;
		}
		if (sm->mnemonic_charset && sm->mnemonic_charset[i])
		{
			XtSetArg(args[n], XmNmnemonicCharSet, 
				sm->mnemonic_charset[i]); 
			n++;
		}
		
		/* Dynamic Defaulting of button type */

		if (sm->button_type && sm->button_type[i])
			btype = sm->button_type[i];
		else
			btype = XmNONE;

		if (btype == XmNONE)
		{
			if (rc->row_column.type == XmMENU_BAR)
				btype = XmCASCADEBUTTON;
			else
				btype = XmPUSHBUTTON;
		}
		
		switch (btype)
		{
			case XmTITLE:
				sprintf(name_buf,"label_%d", label_count++);
				child = XtCreateManagedWidget( name_buf,
                                     xmLabelGadgetClass, (Widget) rc, args, n);
			break;
			case XmDOUBLE_SEPARATOR:
				XtSetArg(args[n], XmNseparatorType, XmDOUBLE_LINE); n++;
			case XmSEPARATOR:
				sprintf(name_buf,"separator_%d", separator_count++);
				child = XtCreateManagedWidget(name_buf, 
                                 xmSeparatorGadgetClass, (Widget) rc, args, n);
			break;
			case XmPUSHBUTTON:
				sprintf(name_buf,"button_%d", button_count++);
				child = XtCreateManagedWidget(name_buf, 
                                                       xmPushButtonGadgetClass,
                                                         (Widget) rc, args, n);
				if (sm->callback)
					XtAddCallback(child,
                                             XmNactivateCallback, sm->callback,
                                               (XtPointer)(unsigned long)(button_count - 1));
			break;
			case XmRADIOBUTTON:
				XtSetArg(args[n], XmNindicatorType, XmONE_OF_MANY); n++;
			case XmCHECKBUTTON:
				sprintf(name_buf,"button_%d", button_count++);
				XtSetArg(args[n], XmNindicatorOn, TRUE); n++;
				child = XtCreateManagedWidget(name_buf,
                                                     xmToggleButtonGadgetClass,
                                                         (Widget) rc, args, n);
				if (sm->callback)
					XtAddCallback(child,
                                         XmNvalueChangedCallback, sm->callback,
                                               (XtPointer)(unsigned long)(button_count - 1));
			break;
			case XmCASCADEBUTTON:
				sprintf(name_buf,"button_%d", button_count++);
				child = XtCreateManagedWidget(name_buf,
                                                    xmCascadeButtonGadgetClass,
                                                         (Widget) rc, args, n);
				if (sm->callback)
					XtAddCallback(child,
                                             XmNactivateCallback, sm->callback,
                                               (XtPointer)(unsigned long)(button_count - 1));
			break;
			default:
				/* this is an error condition */
				;
			break;
		}
	}
}

Widget 
XmCreateSimpleMenuBar(
        Widget parent,
        String name,
        ArgList args,
        Cardinal arg_count )
{
	Widget rc;
	XmSimpleMenuRec mr;
	_XmWidgetToAppContext(parent);

	_XmAppLock(app);

	XtGetSubresources(parent, &mr, name, XmCSimpleMenuBar,
		SimpleMenuResources, XtNumber(SimpleMenuResources), 
		args, arg_count);

	rc = XmCreateMenuBar(parent, name, args, arg_count);

	EvaluateConvenienceStructure( rc, &mr);

	_XmAppUnlock(app);
	return(rc);
}

Widget 
XmCreateSimplePopupMenu(
        Widget parent,
        String name,
        ArgList args,
        Cardinal arg_count )
{
	Widget rc;
	XmSimpleMenuRec mr;
	_XmWidgetToAppContext(parent);

	_XmAppLock(app);

	XtGetSubresources(parent, &mr, name, XmCSimplePopupMenu,
		SimpleMenuResources, XtNumber(SimpleMenuResources), 
		args, arg_count);

	rc = XmCreatePopupMenu(parent, name, args, arg_count);

	EvaluateConvenienceStructure( rc, &mr);

	_XmAppUnlock(app);
	return(rc);
}

Widget 
XmCreateSimplePulldownMenu(
        Widget parent,
        String name,
        ArgList args,
        Cardinal arg_count )
{
	Widget rc;
	XmSimpleMenuRec mr;
	int n, i;
	Arg local_args[3];
	WidgetList buttons;
	Cardinal num_buttons;

	_XmWidgetToAppContext(parent);
	_XmAppLock(app);

	XtGetSubresources(parent, &mr, name, XmCSimplePulldownMenu,
		SimpleMenuResources, XtNumber(SimpleMenuResources), 
		args, arg_count);
	
	rc = XmCreatePulldownMenu(parent, name, args, arg_count);

	EvaluateConvenienceStructure(rc, &mr);

	if (mr.post_from_button >= 0)
	{
		n = 0;
		XtSetArg(local_args[n], XtNchildren, &buttons); n++;
		XtSetArg(local_args[n], XtNnumChildren, &num_buttons); n++;
		XtGetValues(parent, local_args, n);

		if (!num_buttons)
		{
			/* error condition */
			_XmAppUnlock(app);
			return(rc);
		}
		else
		{
			for (i = 0; i < num_buttons; i++)
			{
				if (((XmIsCascadeButtonGadget(buttons[i])) ||
					(XmIsCascadeButton(buttons[i])))
					&&
					(i == mr.post_from_button))
					break;
			}

			if ( i < num_buttons)
			{
				n = 0;
				XtSetArg(local_args[n], XmNsubMenuId, rc); n++;
				XtSetValues(buttons[i], local_args, n);
			}
		}
	}
	_XmAppUnlock(app);
	return(rc);
}

Widget 
XmCreateSimpleOptionMenu(
        Widget parent,
        String name,
        ArgList args,
        Cardinal arg_count )
{
	Widget rc, sub_rc;
	XmSimpleMenuRec mr;
	int n, i, button_count;
	Arg local_args[5];
	WidgetList buttons;
	Cardinal num_buttons;
	_XmWidgetToAppContext(parent);

	_XmAppLock(app);

	XtGetSubresources(parent, &mr, name, XmCSimpleOptionMenu,
		SimpleMenuResources, XtNumber(SimpleMenuResources), 
		args, arg_count);
	
	rc = XmCreateOptionMenu(parent, name, args, arg_count);

	sub_rc = XmCreatePulldownMenu(parent, name, args, arg_count);

	EvaluateConvenienceStructure(sub_rc, &mr);

	n = 0;
	if (mr.option_label)
	{
		XtSetArg(local_args[n], XmNlabelString, mr.option_label); n++;
	}
	if (mr.option_mnemonic)
	{
		XtSetArg(local_args[n], XmNmnemonic, mr.option_mnemonic); n++;
	}
	
	XtSetArg(local_args[n], XmNsubMenuId, sub_rc); n++;
	XtSetValues(rc, local_args, n);

	if (mr.button_set >= 0)
	{
		n = 0;
		XtSetArg(local_args[n], XtNchildren, &buttons); n++;
		XtSetArg(local_args[n], XtNnumChildren, &num_buttons); n++;
		XtGetValues(sub_rc, local_args, n);

		if (!num_buttons)
		{
			/* error condition */
			_XmAppUnlock(app);
			return(rc);
		}
		else
		{
			button_count = 0;
			for (i = 0; i < num_buttons; i++)
			{				/* count only PushB */
				if ((XmIsPushButtonGadget(buttons[i])) ||
					(XmIsPushButton(buttons[i])))
				{
					if (button_count == mr.button_set)
						break;
					button_count++;
				}
			}

			if ( i < num_buttons)
			{
				n = 0;
				XtSetArg(local_args[n], XmNmenuHistory, buttons[i]); n++;
				XtSetValues(rc, local_args, n);
			}
		}
	}

	_XmAppUnlock(app);
	return(rc);
}

Widget 
XmCreateSimpleRadioBox(
        Widget parent,
        String name,
        ArgList args,
        Cardinal arg_count )
{
	Arg local_args[5];
	Widget rc, child;
	int i, n;
	XmSimpleMenuRec mr;
	char name_buf[20];

	rc = XmCreateRadioBox(parent, name, args, arg_count);

	XtGetSubresources(parent, &mr, name, XmCSimpleRadioBox,
		SimpleMenuResources, XtNumber(SimpleMenuResources), 
		args, arg_count);

	for(i=0; i < mr.count; i++)
	{
		sprintf(name_buf,"button_%d", i);

		n = 0;
		if (mr.label_string && mr.label_string[i])
		{
			XtSetArg(local_args[n], 
				XmNlabelString, mr.label_string[i]); n++;
		}
		if (mr.button_set == i)
		{
			XtSetArg(local_args[n], XmNset, TRUE); n++;
		}
		child = XtCreateManagedWidget(name_buf, 
			xmToggleButtonGadgetClass, (Widget) rc, local_args, n);
		if (mr.callback)
			XtAddCallback(child, XmNvalueChangedCallback,
				mr.callback, (XtPointer)(unsigned long)i);
	}
	
	return(rc);
}

Widget 
XmCreateSimpleCheckBox(
        Widget parent,
        String name,
        ArgList args,
        Cardinal arg_count )
{
	Arg local_args[5];
	Widget rc, child;
	int i, n;
	XmSimpleMenuRec mr;
	char name_buf[20];


	rc = XmCreateRadioBox(parent, name, args, arg_count);

	n = 0;
        XtSetArg(local_args[n], XmNradioBehavior, FALSE); n++;

	XtSetValues(rc, local_args, n);
	

	XtGetSubresources(parent, &mr, name, XmCSimpleCheckBox,
		SimpleMenuResources, XtNumber(SimpleMenuResources), 
		args, arg_count);

	for(i=0; i < mr.count; i++)
	{
		sprintf(name_buf,"button_%d", i);

		n = 0;
		if (mr.label_string && mr.label_string[i])
		{
			XtSetArg(local_args[n], 
				XmNlabelString, mr.label_string[i]); n++;
		}
		child = XtCreateManagedWidget(name_buf,
			xmToggleButtonGadgetClass, (Widget) rc, local_args, n);
		if (mr.callback)
			XtAddCallback(child, XmNvalueChangedCallback,
				mr.callback, (XtPointer)(unsigned long)i);
	}

	return(rc);
}
