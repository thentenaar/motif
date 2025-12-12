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
static char rcsid[] = "$XConsortium: VaSimple.c /main/11 1995/10/25 20:26:43 cde-sun $"
#endif
#endif

#include <Xm/VaSimpleP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "MessagesI.h"
#include "VaSimpleI.h"
#include "XmI.h"

#ifndef va_dcl
#define va_dcl int va_alist;
#endif


#define XMBUTTONS_ARGS_PER_LIST		5
#define XMCASCADE_ARGS_PER_LIST		3
#define XMTITLE_ARGS_PER_LIST		1
#define XMSEPARATOR_ARGS_PER_LIST	0

#define _XmINVALID_BUTTON_TYPE		0xff

#define MESSAGE1 _XmMMsgVaSimple_0000
#define MESSAGE2 _XmMMsgVaSimple_0001
#define MESSAGE3 _XmMMsgVaSimple_0002


/********  Static Function Declarations  ********/
static XmButtonType _XmVaBType_to_XmBType( 
                        String symbol) ;
static void _XmCountNestedList( 
                        XtTypedArgList avlist,
                        int *total_count,
                        int *typed_count) ;
static int _XmTypedArgToArg( 
                        Widget widget,
                        XtTypedArgList typed_arg,
                        ArgList arg_return,
                        XtResourceList resources,
                        Cardinal num_resources) ;
static int _XmNestedArgtoArg( 
                        Widget widget,
                        XtTypedArgList avlist,
                        ArgList args,
                        XtResourceList resources,
                        Cardinal num_resources) ;
static int _XmNestedArgtoTypedArg( 
                        XtTypedArgList args,
                        XtTypedArgList avlist) ;
static void _XmVaProcessEverything( 
                        Widget widget,
                        va_list var,
                        XmButtonTypeTable *buttonTypes,
                        XmStringTable *buttonStrings,
                        XmKeySymTable *buttonMnemonics,
                        String **buttonAccelerators,
                        XmStringTable *buttonAcceleratorText,
                        int button_count,
                        ArgList *args,
                        int num_args) ;
/********  End Static Function Declarations  ********/


static XmButtonType 
_XmVaBType_to_XmBType(
        String symbol )
{
    if (strcmp(symbol, XmVaPUSHBUTTON) == 0)
      	return (XmPUSHBUTTON);
    else if (strcmp(symbol, XmVaTOGGLEBUTTON) == 0)
      	return (XmTOGGLEBUTTON);
    else if (strcmp(symbol, XmVaCHECKBUTTON) == 0)
      	return (XmCHECKBUTTON);
    else if (strcmp(symbol, XmVaRADIOBUTTON) == 0)
      	return (XmRADIOBUTTON);
    else if (strcmp(symbol, XmVaCASCADEBUTTON) == 0)
      	return (XmCASCADEBUTTON);
    else if (strcmp(symbol, XmVaSEPARATOR) == 0)
      	return (XmSEPARATOR);
    else if (strcmp(symbol, XmVaDOUBLE_SEPARATOR) == 0)
      	return (XmDOUBLE_SEPARATOR);
    else if (strcmp(symbol, XmVaTITLE) == 0)
      	return (XmTITLE);
    else
      	return (_XmINVALID_BUTTON_TYPE);
}

/*
 *    Given a nested list, _XmCountNestedList() returns counts of the
 *    total number of attribute-value pairs and the count of those
 *    attributes that are typed. The list is counted recursively.
 */
static void 
_XmCountNestedList(
        XtTypedArgList avlist,
        int *total_count,
        int *typed_count )
{
    for (; avlist->name != NULL; avlist++) {
        if (strcmp(avlist->name, XtVaNestedList) == 0) {
            _XmCountNestedList((XtTypedArgList)avlist->value, total_count,
                	       typed_count);
        } else {
            if (avlist->type != NULL) {
                ++(*typed_count);
            }
            ++(*total_count);
        }
    }    
}

/*
 * Function: XmeCountVaList
 *
 * Description: Takes a variable list and returns the number of total
 *              items in it. This is a much simplified version of 
 *              _XmCountVaList. It is intended to be used in convience 
 *              creation routines for widgets both inside and outside the 
 *              toolkit. 
 *
 * Input: al - a variable lenth list, at the beginning of the list.
 *
 * Output:  the number of items in it before the last NULL
 *
 */
int
XmeCountVaListSimple(va_list al)
{
   int d1, d2, d3, count;
   
   _XmCountVaList(al, &d1, &d2, &d3, &count);
  
   return count;
}

/*
 *    Given a variable length attribute-value list, _XmCountVaList()
 *    returns counts of the total number of attribute-value pairs,
 *    and the count of the number of those attributes that are typed.
 *    The list is counted recursively.
 */
void 
_XmCountVaList(
        va_list var,
        int *button_count,
        int *args_count,
        int *typed_count,
        int *total_count )
{
    String          attr;
    int		    i;

    *button_count = 0;
    *args_count = 0;
    *typed_count = 0;
    *total_count = 0;
 
    for(attr = va_arg(var, String) ; attr != NULL;
                        attr = va_arg(var, String)) {
/* Count typed Args */
        if (strcmp(attr, XtVaTypedArg) == 0) {
            (void)va_arg(var, String);
            (void)va_arg(var, String);
            (void)va_arg(var, XtArgVal);
            (void)va_arg(var, int);
            ++(*total_count);
            ++(*typed_count);
        } else if (strcmp(attr, XtVaNestedList) == 0) {
            _XmCountNestedList(va_arg(var, XtTypedArgList),
			       total_count, typed_count);
        } else {
/* Count valid VaBUTTONS (done here because of the variable arg length) */
	    if (strcmp(attr, XmVaCASCADEBUTTON) == 0) {
		for (i = 1; i < XMCASCADE_ARGS_PER_LIST; i++)
		  (void)va_arg(var, XtArgVal);		
		++(*total_count);
		++(*button_count);
	    } else if ((strcmp(attr, XmVaSEPARATOR) == 0) ||
		(strcmp(attr, XmVaDOUBLE_SEPARATOR) == 0)) {
		++(*total_count);
		++(*button_count);
	    } else if (strcmp(attr, XmVaTITLE) == 0){
		  (void)va_arg(var, XtArgVal);
		++(*total_count);
		++(*button_count);
/* Check matches other known VaBUTTONS */
	    } else if (_XmVaBType_to_XmBType(attr) != _XmINVALID_BUTTON_TYPE) {
		for (i = 1; i < XMBUTTONS_ARGS_PER_LIST; i++)
		  (void)va_arg(var, XtArgVal);
		++(*total_count);
		++(*button_count);
/* Else it's a simple Arg */
	    } else {
		(void)va_arg(var, XtArgVal);
		++(*args_count);
		++(*total_count);
	    }
	}
    }
}

/*
 *    _XmTypedArgToArg() invokes a resource converter to convert the
 *    passed typed arg into a name/value pair and stores the name/value
 *    pair in the passed Arg structure. It returns 1 if the conversion
 *    succeeded and 0 if the conversion failed.
 */
static int 
_XmTypedArgToArg(
        Widget widget,
        XtTypedArgList typed_arg,
        ArgList arg_return,
        XtResourceList resources,
        Cardinal num_resources )
{     
    String              to_type = NULL;
    XrmValue            from_val, to_val;
    register int        i;
      

    if (widget == NULL) {
        XtAppWarningMsg(XtWidgetToApplicationContext(widget),
            "nullWidget", "xtConvertVarTArgList", "XtToolkitError",
            MESSAGE1, (String *)NULL, (Cardinal *)NULL);
        return(0);
    }
       
    /* again we assume that the XtResourceList is un-compiled */

    for (i = 0; i < num_resources; i++) {
        if (StringToName(typed_arg->name) ==
            StringToName(resources[i].resource_name)) {
            to_type = resources[i].resource_type;
            break;
        }
    }

    if (to_type == NULL) {
        XtAppWarningMsg(XtWidgetToApplicationContext(widget),
            "unknownType", "xtConvertVarTArgList", "XtToolkitError",
            MESSAGE2, (String *)NULL, (Cardinal *)NULL);
        return(0);
    }
       
    to_val.addr = NULL;
    from_val.size = typed_arg->size;
    if ((strcmp(typed_arg->type, XtRString) == 0) ||
            (typed_arg->size > sizeof(XtArgVal))) {
        from_val.addr = (XPointer)typed_arg->value;
    } else {
            from_val.addr = (XPointer)&typed_arg->value;
    }
       
    _XmProcessLock();
    XtConvert(widget, typed_arg->type, &from_val, to_type, &to_val);
 
    if (to_val.addr == NULL) {
	_XmProcessUnlock();
        XtAppWarningMsg(XtWidgetToApplicationContext(widget),
            "conversionFailed", "xtConvertVarToArgList", "XtToolkitError",
            MESSAGE3, (String *)NULL, (Cardinal *)NULL);
        return(0);
    }

    arg_return->name = typed_arg->name;

    if (strcmp(to_type, XtRString) == 0) {
	arg_return->value = (XtArgVal) to_val.addr;
    }
    else {
	if (to_val.size == sizeof(long))
	    arg_return->value = (XtArgVal) *(long *)to_val.addr;
	else if (to_val.size == sizeof(int))
	    arg_return->value = (XtArgVal) *(int *)to_val.addr;
	else if (to_val.size == sizeof(short))
	    arg_return->value = (XtArgVal) *(short *)to_val.addr;
	else if (to_val.size == sizeof(char))
	    arg_return->value = (XtArgVal) *(char *)to_val.addr;
	else
	    arg_return->value = *(XtArgVal *)to_val.addr;
    }
       
    _XmProcessUnlock();
    return(1);
}

/*
 *    _XmNestedArgtoArg() converts the passed nested list into
 *    an ArgList/count.
 */
static int 
_XmNestedArgtoArg(
        Widget widget,
        XtTypedArgList avlist,
        ArgList args,
        XtResourceList resources,
        Cardinal num_resources )
{
    int         count = 0;
 
    for (; avlist->name != NULL; avlist++) {
        if (avlist->type != NULL) {
            /* If widget is NULL, the typed arg is ignored */
            if (widget != NULL) {
                /* this is a typed arg */
                count += _XmTypedArgToArg(widget, avlist, (args+count),
                             resources, num_resources);
            }
        } else if (strcmp(avlist->name, XtVaNestedList) == 0) {
            count += _XmNestedArgtoArg(widget, (XtTypedArgList)avlist->value,
                        (args+count), resources, num_resources);
        } else {
            (args+count)->name = avlist->name;
            (args+count)->value = avlist->value;
            ++count;
        }
    }

    return(count);
}

static int 
_XmNestedArgtoTypedArg(
        XtTypedArgList args,
        XtTypedArgList avlist )
{    
    int         count = 0;
     
    for (; avlist->name != NULL; avlist++) { 
        if (avlist->type != NULL) { 
            (args+count)->name = avlist->name; 
            (args+count)->type = avlist->type; 
            (args+count)->size = avlist->size;
            (args+count)->value = avlist->value;
            ++count; 
        } else if(strcmp(avlist->name, XtVaNestedList) == 0) {             
            count += _XmNestedArgtoTypedArg((args+count),  
                            (XtTypedArgList)avlist->value); 
        } else {                             
            (args+count)->name = avlist->name; 
	    (args+count)->type = NULL;
            (args+count)->value = avlist->value; 
            ++count;
        }                                     
    }         
    return(count);
}


/*
 *    Given a variable argument list, _XmVaToTypedArgList() returns 
 *    the equivalent TypedArgList. _XmVaToTypedArgList() handles nested
 *    lists.
 *    Note: _XmVaToTypedArgList() does not do type conversions.
 */
void 
_XmVaToTypedArgList(
        va_list var,
        int max_count,
        XtTypedArgList *args_return,
        Cardinal *num_args_return )
{
    XtTypedArgList	args = NULL;
    String              attr;
    int			count;

    args = (XtTypedArgList)
	XtMalloc((unsigned)(max_count * sizeof(XtTypedArg))); 

    for(attr = va_arg(var, String), count = 0 ; attr != NULL;
		    attr = va_arg(var, String)) {
        if (strcmp(attr, XtVaTypedArg) == 0) {
	    args[count].name = va_arg(var, String);
	    args[count].type = va_arg(var, String);
	    args[count].value = va_arg(var, XtArgVal);
	    args[count].size = va_arg(var, int);
	    ++count;
	} else if (strcmp(attr, XtVaNestedList) == 0) {
   	    count += _XmNestedArgtoTypedArg(&args[count], 
			va_arg(var, XtTypedArgList));
	} else {
	    args[count].name = attr;
	    args[count].type = NULL;
	    args[count].value = va_arg(var, XtArgVal);
	    ++count;
	}
    }

    *args_return = args;
    *num_args_return = count;
}

static void 
_XmVaProcessEverything(
        Widget widget,
        va_list var,
        XmButtonTypeTable *buttonTypes,
        XmStringTable *buttonStrings,
        XmKeySymTable *buttonMnemonics,
        String **buttonAccelerators,
        XmStringTable *buttonAcceleratorText,
        int button_count,
        ArgList *args,
        int num_args )
{

    XtTypedArg		typed_args;
    String              attr;
    int			count, bcount;



    *args = (ArgList) XtMalloc(num_args * sizeof(Arg));

/* Process the routine specific args */
    *buttonTypes = (XmButtonTypeTable)
	XtMalloc((unsigned)(button_count * sizeof(XmButtonType *)));

    *buttonStrings = (XmStringTable)
	XtMalloc((unsigned)(button_count * sizeof(XmString *)));

    *buttonMnemonics = (XmKeySymTable)
	XtMalloc((unsigned)(button_count * sizeof(KeySym *)));

    *buttonAccelerators = (String *)
	XtMalloc((unsigned)(button_count * sizeof(String *)));

    *buttonAcceleratorText = (XmStringTable)
	XtMalloc((unsigned)(button_count * sizeof(XmString *)));

    bcount = 0;
    for(attr = va_arg(var, String), count = 0 ; attr != NULL;
		    attr = va_arg(var, String)) {
        if (strcmp(attr, XtVaTypedArg) == 0) {
	    typed_args.name = va_arg(var, String);
	    typed_args.type = va_arg(var, String);
	    typed_args.value = va_arg(var, XtArgVal);
	    typed_args.size = va_arg(var, int);
	    count += _XmTypedArgToArg(widget, &typed_args, &(*args)[count],
                                                                      NULL, 0);
	} else if (strcmp(attr, XtVaNestedList) == 0) {
	    count += _XmNestedArgtoArg(widget, va_arg(var, XtTypedArgList),
			&(*args)[count], NULL, 0);
	} else if ((strcmp(attr, XmVaSEPARATOR) == 0) ||
	    (strcmp(attr, XmVaDOUBLE_SEPARATOR) == 0)) {
		(*buttonTypes)[bcount] = _XmVaBType_to_XmBType(attr);
		(*buttonStrings)[bcount] = (XmString) NULL;
		(*buttonMnemonics)[bcount] = (KeySym) NULL;
		(*buttonAccelerators)[bcount] = (String) NULL;
		(*buttonAcceleratorText)[bcount] = (XmString) NULL;
	    ++bcount;
	} else if (strcmp(attr, XmVaTITLE) == 0){
		(*buttonTypes)[bcount] = _XmVaBType_to_XmBType(attr);
		(*buttonStrings)[bcount] = va_arg(var, XmString);
		(*buttonMnemonics)[bcount] = (KeySym) NULL;
		(*buttonAccelerators)[bcount] = (String) NULL;
		(*buttonAcceleratorText)[bcount] = (XmString) NULL;
	    ++bcount;
	} else if (strcmp(attr, XmVaCASCADEBUTTON) == 0) {
		(*buttonTypes)[bcount] = _XmVaBType_to_XmBType(attr);
		(*buttonStrings)[bcount] = va_arg(var, XmString);
		(*buttonMnemonics)[bcount] = va_arg(var, KeySym);
		(*buttonAccelerators)[bcount] = (String) NULL;
		(*buttonAcceleratorText)[bcount] = (XmString) NULL;
	    ++bcount;
	} else if (_XmVaBType_to_XmBType(attr) != _XmINVALID_BUTTON_TYPE) {
		(*buttonTypes)[bcount] = _XmVaBType_to_XmBType(attr);
		(*buttonStrings)[bcount] = va_arg(var, XmString);
		(*buttonMnemonics)[bcount] = va_arg(var, KeySym);
		(*buttonAccelerators)[bcount] = va_arg(var, String);
		(*buttonAcceleratorText)[bcount] = va_arg(var, XmString);
	    ++bcount;
	} else {
	    (*args)[count].name = attr;
	    (*args)[count].value = va_arg(var, XtArgVal);
	    ++count;
	}

    }
}


Widget
XmVaCreateSimpleMenuBar(Widget parent, String name, ...)
{
#define MB_EXTRA_ARGS 1

    va_list		var;
    register Widget	widget;
    ArgList		args;
    int			button_count, args_count, typed_count, total_count;
    int			n, num_args;
    XmButtonTypeTable	menuBarButtonTypes;
    XmStringTable	menuBarStrings;
    XmKeySymTable	menuBarMnemonics;
    String *		menuBarAccelerators;
    XmStringTable	menuBarAcceleratorText;

    _XmWidgetToAppContext(parent);
    _XmAppLock(app);

/* Count number of arg (arg in this case means number of arg groups) */

    Va_start(var,name);
    _XmCountVaList(var, &button_count, &args_count, &typed_count, &total_count);
    va_end(var);

/* Convert into a normal ArgList */

    Va_start(var,name);

    num_args = args_count + (XMCASCADE_ARGS_PER_LIST + MB_EXTRA_ARGS);

    _XmVaProcessEverything(parent, var, &menuBarButtonTypes, &menuBarStrings,
		&menuBarMnemonics, &menuBarAccelerators, &menuBarAcceleratorText,
		button_count, &args, num_args);

    n = args_count;
    XtSetArg (args[n], XmNbuttonCount, button_count); n++;
    XtSetArg (args[n], XmNbuttonType, menuBarButtonTypes); n++;
    XtSetArg (args[n], XmNbuttons, menuBarStrings); n++;
    XtSetArg (args[n], XmNbuttonMnemonics, menuBarMnemonics); n++;

    widget = XmCreateSimpleMenuBar(parent, name, args, n);

    if (args != NULL) {
	XtFree((char *)args);
    }
    if (menuBarButtonTypes != NULL) {
	XtFree((char *)menuBarButtonTypes);
    }
    if (menuBarStrings != NULL) {
	XtFree((char *)menuBarStrings);
    }
    if (menuBarMnemonics != NULL) {
	XtFree((char *)menuBarMnemonics);
    }
    if (menuBarAccelerators != NULL) {
	XtFree((char *)menuBarAccelerators);
    }
    if (menuBarAcceleratorText != NULL) {
	XtFree((char *)menuBarAcceleratorText);
    }

    va_end(var);

    _XmAppUnlock(app);
    return (widget);
}


Widget
XmVaCreateSimplePulldownMenu(Widget parent, String name, int post_from_button, XtCallbackProc callback, ...)
{
#define PD_EXTRA_ARGS 3

    va_list		var;
    register Widget	widget;
    Arg			*args;
    int			button_count, args_count, typed_count, total_count;
    int			n, num_args;
    XmButtonTypeTable	pulldownMenuButtonTypes;
    XmStringTable	pulldownMenuStrings;
    XmKeySymTable	pulldownMenuMnemonics;
    String *		pulldownMenuAccelerators;
    XmStringTable	pulldownMenuAcceleratorText;

    _XmWidgetToAppContext(parent);
    _XmAppLock(app);

/* Count number of arg (arg in this case means number of arg groups) */
    Va_start(var,callback);
    _XmCountVaList(var, &button_count, &args_count, &typed_count, &total_count);
    va_end(var);

/* Convert into a normal ArgList */
    Va_start(var,callback);

    num_args = args_count + (XMBUTTONS_ARGS_PER_LIST + PD_EXTRA_ARGS);

    _XmVaProcessEverything(parent, var, &pulldownMenuButtonTypes,
		&pulldownMenuStrings, &pulldownMenuMnemonics, 
		&pulldownMenuAccelerators, &pulldownMenuAcceleratorText,
		button_count, &args, num_args);

    n = args_count;
    XtSetArg (args[n], XmNsimpleCallback, callback); n++;
    XtSetArg (args[n], XmNpostFromButton, post_from_button); n++;
    XtSetArg (args[n], XmNbuttonCount, button_count); n++;
    XtSetArg (args[n], XmNbuttonType, pulldownMenuButtonTypes); n++;
    XtSetArg (args[n], XmNbuttons, pulldownMenuStrings); n++;
    XtSetArg (args[n], XmNbuttonMnemonics, pulldownMenuMnemonics); n++;
    XtSetArg (args[n], XmNbuttonAccelerators,
	      pulldownMenuAccelerators); n++;
    XtSetArg (args[n], XmNbuttonAcceleratorText,
	      pulldownMenuAcceleratorText); n++;

    widget = XmCreateSimplePulldownMenu(parent, name, args, n);

    if (args != NULL) {
	XtFree((char *)args);
    }
    if (pulldownMenuButtonTypes != NULL) {
	XtFree((char *)pulldownMenuButtonTypes);
    }
    if (pulldownMenuStrings != NULL) {
	XtFree((char *)pulldownMenuStrings);
    }
    if (pulldownMenuMnemonics != NULL) {
	XtFree((char *)pulldownMenuMnemonics);
    }
    if (pulldownMenuAccelerators != NULL) {
	XtFree((char *)pulldownMenuAccelerators);
    }
    if (pulldownMenuAcceleratorText != NULL) {
	XtFree((char *)pulldownMenuAcceleratorText);
    }

    va_end(var);

    _XmAppUnlock(app);
    return (widget);
}


Widget
XmVaCreateSimplePopupMenu(Widget parent, String name, XtCallbackProc callback, ...)
{
#define PU_EXTRA_ARGS 2


    va_list		var;
    register Widget	widget;
    Arg			*args;
    int			button_count, args_count, typed_count, total_count;
    int			n, num_args;
    XmButtonTypeTable	popupMenuButtonTypes;
    XmStringTable	popupMenuStrings;
    XmKeySymTable	popupMenuMnemonics;
    String *		popupMenuAccelerators;
    XmStringTable	popupMenuAcceleratorText;

    _XmWidgetToAppContext(parent);
    _XmAppLock(app);

/* Count number of arg (arg in this case means number of arg groups) */
    Va_start(var,callback);
    _XmCountVaList(var, &button_count, &args_count, &typed_count, &total_count);
    va_end(var);

/* Convert into a normal ArgList */
    Va_start(var,callback);

    num_args = args_count + (XMBUTTONS_ARGS_PER_LIST + PU_EXTRA_ARGS);

    _XmVaProcessEverything(parent, var, &popupMenuButtonTypes,
		&popupMenuStrings, &popupMenuMnemonics, 
		&popupMenuAccelerators, &popupMenuAcceleratorText,
		button_count, &args, num_args);

    n = args_count;
    XtSetArg (args[n], XmNsimpleCallback, callback); n++;
    XtSetArg (args[n], XmNbuttonCount, button_count); n++;
    XtSetArg (args[n], XmNbuttonType, popupMenuButtonTypes); n++;
    XtSetArg (args[n], XmNbuttons, popupMenuStrings); n++;
    XtSetArg (args[n], XmNbuttonMnemonics, popupMenuMnemonics); n++;
    XtSetArg (args[n], XmNbuttonAccelerators,
	      popupMenuAccelerators); n++;
    XtSetArg (args[n], XmNbuttonAcceleratorText,
	      popupMenuAcceleratorText); n++;

    widget = XmCreateSimplePopupMenu(parent, name, args, n);

    if (args != NULL) {
	XtFree((char *)args);
    }
    if (popupMenuButtonTypes != NULL) {
	XtFree((char *)popupMenuButtonTypes);
    }
    if (popupMenuStrings != NULL) {
	XtFree((char *)popupMenuStrings);
    }
    if (popupMenuMnemonics != NULL) {
	XtFree((char *)popupMenuMnemonics);
    }
    if (popupMenuAccelerators != NULL) {
	XtFree((char *)popupMenuAccelerators);
    }
    if (popupMenuAcceleratorText != NULL) {
	XtFree((char *)popupMenuAcceleratorText);
    }

    va_end(var);

    _XmAppUnlock(app);
    return (widget);
}


Widget
XmVaCreateSimpleOptionMenu(Widget parent, String name, XmString option_label, KeySym option_mnemonic, int button_set, XtCallbackProc callback, ...)
{
#define OM_EXTRA_ARGS 5


    va_list		var;
    register Widget	widget;
    Arg			*args;
    int			button_count, args_count, typed_count, total_count;
    int			n, num_args;
    XmButtonTypeTable	optionMenuButtonTypes;
    XmStringTable	optionMenuStrings;
    XmKeySymTable	optionMenuMnemonics;
    String *		optionMenuAccelerators;
    XmStringTable	optionMenuAcceleratorText;

    _XmWidgetToAppContext(parent);
    _XmAppLock(app);

/* Count number of arg (arg in this case means number of arg groups) */
    Va_start(var,callback);
    _XmCountVaList(var, &button_count, &args_count, &typed_count, &total_count);
    va_end(var);

/* Convert into a normal ArgList */
    Va_start(var,callback);

    num_args = args_count + (XMBUTTONS_ARGS_PER_LIST + OM_EXTRA_ARGS);

    _XmVaProcessEverything(parent, var, &optionMenuButtonTypes,
		&optionMenuStrings, &optionMenuMnemonics, 
		&optionMenuAccelerators, &optionMenuAcceleratorText,
		button_count, &args, num_args);

    n = args_count;
    XtSetArg (args[n], XmNsimpleCallback, callback); n++;
    XtSetArg (args[n], XmNoptionLabel, option_label); n++;
    XtSetArg (args[n], XmNoptionMnemonic, option_mnemonic); n++;
    XtSetArg (args[n], XmNbuttonSet, button_set); n++;
    XtSetArg (args[n], XmNbuttonCount, button_count); n++;
    XtSetArg (args[n], XmNbuttonType, optionMenuButtonTypes); n++;
    XtSetArg (args[n], XmNbuttons, optionMenuStrings); n++;
    XtSetArg (args[n], XmNbuttonMnemonics, optionMenuMnemonics); n++;
    XtSetArg (args[n], XmNbuttonAccelerators,
	      optionMenuAccelerators); n++;
    XtSetArg (args[n], XmNbuttonAcceleratorText,
	      optionMenuAcceleratorText); n++;

    widget = XmCreateSimpleOptionMenu(parent, name, args, n);

    if (args != NULL) {
	XtFree((char *)args);
    }
    if (optionMenuButtonTypes != NULL) {
	XtFree((char *)optionMenuButtonTypes);
    }
    if (optionMenuStrings != NULL) {
	XtFree((char *)optionMenuStrings);
    }
    if (optionMenuMnemonics != NULL) {
	XtFree((char *)optionMenuMnemonics);
    }
    if (optionMenuAccelerators != NULL) {
	XtFree((char *)optionMenuAccelerators);
    }
    if (optionMenuAcceleratorText != NULL) {
	XtFree((char *)optionMenuAcceleratorText);
    }

    va_end(var);

    _XmAppUnlock(app);
    return (widget);
}


Widget
XmVaCreateSimpleRadioBox(Widget parent, String name, int button_set, XtCallbackProc callback, ...)
{
#define RB_EXTRA_ARGS 3

    va_list		var;
    register Widget	widget;
    Arg			*args;
    int			button_count, args_count, typed_count, total_count;
    int			n, num_args;
    XmButtonTypeTable	radioBoxButtonTypes;
    XmStringTable	radioBoxStrings;
    XmKeySymTable	radioBoxMnemonics;
    String *		radioBoxAccelerators;
    XmStringTable	radioBoxAcceleratorText;

    _XmWidgetToAppContext(parent);
    _XmAppLock(app);

/* Count number of arg (arg in this case means number of arg groups) */
    Va_start(var,callback);
    _XmCountVaList(var, &button_count, &args_count, &typed_count, &total_count);
    va_end(var);

/* Convert into a normal ArgList */
    Va_start(var,callback);

    num_args = args_count + (XMBUTTONS_ARGS_PER_LIST + RB_EXTRA_ARGS);

    _XmVaProcessEverything(parent, var, &radioBoxButtonTypes,
		&radioBoxStrings, &radioBoxMnemonics, 
		&radioBoxAccelerators, &radioBoxAcceleratorText,
		button_count, &args, num_args);

    n = args_count;
    XtSetArg (args[n], XmNsimpleCallback, callback); n++;
    XtSetArg (args[n], XmNbuttonSet, button_set); n++;
    XtSetArg (args[n], XmNbuttonCount, button_count); n++;
    XtSetArg (args[n], XmNbuttonType, radioBoxButtonTypes); n++;
    XtSetArg (args[n], XmNbuttons, radioBoxStrings); n++;
    XtSetArg (args[n], XmNbuttonMnemonics, radioBoxMnemonics); n++;
    XtSetArg (args[n], XmNbuttonAccelerators,
	      radioBoxAccelerators); n++;
    XtSetArg (args[n], XmNbuttonAcceleratorText,
	      radioBoxAcceleratorText); n++;

    widget = XmCreateSimpleRadioBox(parent, name, args, n);

    if (args != NULL) {
	XtFree((char *)args);
    }
    if (radioBoxButtonTypes != NULL) {
	XtFree((char *)radioBoxButtonTypes);
    }
    if (radioBoxStrings != NULL) {
	XtFree((char *)radioBoxStrings);
    }
    if (radioBoxMnemonics != NULL) {
	XtFree((char *)radioBoxMnemonics);
    }
    if (radioBoxAccelerators != NULL) {
	XtFree((char *)radioBoxAccelerators);
    }
    if (radioBoxAcceleratorText != NULL) {
	XtFree((char *)radioBoxAcceleratorText);
    }

    va_end(var);

    _XmAppUnlock(app);
    return (widget);
}


Widget
XmVaCreateSimpleCheckBox(Widget parent, String name, XtCallbackProc callback, ...)
{
#define CB_EXTRA_ARGS 2

    va_list		var;
    register Widget	widget;
    Arg			*args;
    int			button_count, args_count, typed_count, total_count;
    int			n, num_args;
    XmButtonTypeTable	checkBoxButtonTypes;
    XmStringTable	checkBoxStrings;
    XmKeySymTable	checkBoxMnemonics;
    String *		checkBoxAccelerators;
    XmStringTable	checkBoxAcceleratorText;

    _XmWidgetToAppContext(parent);
    _XmAppLock(app);

/* Count number of arg (arg in this case means number of arg groups) */
    Va_start(var,callback);
    _XmCountVaList(var, &button_count, &args_count, &typed_count, &total_count);
    va_end(var);

/* Convert into a normal ArgList */
    Va_start(var,callback);

    num_args = args_count + (XMBUTTONS_ARGS_PER_LIST + CB_EXTRA_ARGS);

    _XmVaProcessEverything(parent, var, &checkBoxButtonTypes,
		&checkBoxStrings, &checkBoxMnemonics, 
		&checkBoxAccelerators, &checkBoxAcceleratorText,
		button_count, &args, num_args);

    n = args_count;
    XtSetArg (args[n], XmNsimpleCallback, callback); n++;
    XtSetArg (args[n], XmNbuttonCount, button_count); n++;
    XtSetArg (args[n], XmNbuttonType, checkBoxButtonTypes); n++;
    XtSetArg (args[n], XmNbuttons, checkBoxStrings); n++;
    XtSetArg (args[n], XmNbuttonMnemonics, checkBoxMnemonics); n++;
    XtSetArg (args[n], XmNbuttonAccelerators,
	      checkBoxAccelerators); n++;
    XtSetArg (args[n], XmNbuttonAcceleratorText,
	      checkBoxAcceleratorText); n++;

    widget = XmCreateSimpleCheckBox(parent, name, args, n);

    if (args != NULL) {
	XtFree((char *)args);
    }
    if (checkBoxButtonTypes != NULL) {
	XtFree((char *)checkBoxButtonTypes);
    }
    if (checkBoxStrings != NULL) {
	XtFree((char *)checkBoxStrings);
    }
    if (checkBoxMnemonics != NULL) {
	XtFree((char *)checkBoxMnemonics);
    }
    if (checkBoxAccelerators != NULL) {
	XtFree((char *)checkBoxAccelerators);
    }
    if (checkBoxAcceleratorText != NULL) {
	XtFree((char *)checkBoxAcceleratorText);
    }

    va_end(var);

    _XmAppUnlock(app);
    return (widget);
}

/*
 * Function: XmeVLCreateWidget
 *     
 * Description:  Creates a widget of WidgetClass wc using a va_list of 
 *               arguments. Intended for generic use in Widget convience 
 *               functions. Intended for use by future widget writers 
 *               subclassing widgets. 
 *     
 * Input: name - The widget name 
 *        WidgetClass - The class of the widget
 *        parent  - a widget that is a parent
 *        managed - if True the widget will be managed, if false it will not
 *        al - a variable argument list of type va_list from stdarg.h
 *        count - an integer value indicating the number of things in al
 * Output:
 *       a returned widget if one can be created, NULL on error. 
 */
Widget
XmeVLCreateWidget(
        char *name, 
        WidgetClass wc, 
        Widget parent, 
        Boolean managed,
        va_list al, 
        int count)
{
    Widget w;
    ArgList args;
    int n;
    String attr;

    _XmWidgetToAppContext(parent);
    _XmAppLock(app);
    
    /* The size as specified from count */
    args = (ArgList)XtMalloc(count * sizeof(Arg));
    
    /*
     * go through each element and copy the name and the value
     * (remember the value might be a pointer) to the value field
     */
    for (attr = va_arg(al, String), n = 0; 
         attr != NULL; 
         attr = va_arg(al, String), n++)
    {
    	args[n].name = attr;
    	args[n].value = va_arg(al, XtArgVal);
    }
    
    va_end(al);
    
    /* Create the widget managed or not */
    if (managed)w = XtCreateManagedWidget(name, wc, parent, args, n); 
    else w = XtCreateWidget(name, wc, parent, args, n);

    /* 
     * Just free the arg list, not the values in it, that is taken care 
     * of elseware
     */
    XtFree((char *)args);
    
    _XmAppUnlock(app);
    
    return(w);

}
