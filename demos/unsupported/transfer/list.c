/* $XConsortium: list.c /main/5 1995/07/15 20:46:14 drk $ */
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
/*
 * HISTORY
 */

#include <Xm/XmP.h>
#include <Xm/CSText.h>
#include <Xm/TransferP.h>
#include <errno.h>
#include "transfer.h"

#define MAXT 100
#define DEFSTR "<Deferred>"

extern TransferDataRec datums[];
extern unsigned int num_datums;

static XmString GetItemValue(Widget);
static char GetDisplayFormatFromType(Widget, Atom);
static void setFormatAndMenu(char);
static void SetDisplayTypeFromType(Widget, Atom);

static Boolean global_set_defer = False;
static Atom global_set_type_spec = None;
static char global_read_format_spec = 'S';
static XtPointer global_value = NULL;
static Atom global_type = None;
static unsigned long global_length = 0;
static int global_format = 8;
static Boolean global_deferred = False;
static int current_selected_item = -1;

static Atom type = None;

void 
SetType(Widget w, char *in, XtPointer ignore)
{
  if (strlen(in) == 0)
    global_set_type_spec = XmeGetEncodingAtom(w);
  else if (strcmp(in, "NONE") == 0)
    global_set_type_spec = None;
  else
    global_set_type_spec = XInternAtom(XtDisplay(w), in, False);
}

/*******************************************************************
 * FindRightButton
 * 
 * picks the correct button for the type option menu to display
 * note that the names are ATOMNAME_button,  so it prepends a 
 * '*' in front and '_button' after the name before looking up
 * the button.
 ******************************************************************/

static Widget 
FindRightButton(char* atom_string)
{
  char buffer[80];

  strcpy(buffer, "*b_");
  strcat(buffer, atom_string);

  return(XtNameToWidget(toplevel, buffer));
}

/*********************************************************************
 * ListSelectItem
 * 
 * this is called when the user selects an item in the list.  It takes
 * the type of the item and decides how to display it in the CSText
 * widget
 *********************************************************************/

void 
ListSelectItem(Widget w, XtPointer ignore, XmListCallbackStruct *listcb)
{
  int index = listcb -> item_position;
  Atom XA_COMPOUND_TEXT = XInternAtom(XtDisplay(w), XmSCOMPOUND_TEXT, False),
       XA_PIXEL = XInternAtom(XtDisplay(w), "PIXEL", False),
       XA_LOCALE_TEXT = XmeGetEncodingAtom(w),
       XA_MOTIF_CS = XInternAtom(XtDisplay(w), 
				 XmS_MOTIF_COMPOUND_STRING, False),
       XA_MOTIF_RT = XInternAtom(XtDisplay(w), 
				 XmS_MOTIF_RENDER_TABLE, False);
  Widget mem = (Widget) NULL;
  char *targetname, *typename;
  XmString output, tmp;

  if (index < 1 || index > num_datums) return;

  index --;

  current_selected_item = index;
  global_type = type = datums[index].type;
  global_value = datums[index].value;
  global_length = datums[index].length;
  global_format = datums[index].format;
  global_deferred = datums[index].deferred;

  /* We create a big string and then place the string into
     the display field */

  /* Target */
  /* output = XmStringGenerate("Target: \t", NULL, NULL, NULL); */
  output = XmStringCreateLocalized("Target: ");
  targetname = GetSafeAtom(XtDisplay(w), datums[index].target);
  if (targetname == NULL) targetname = XtNewString("Unknown");
  tmp = XmStringCreateLocalized(targetname);
  XtFree(targetname);
  output = XmStringConcatAndFree(output, tmp);
  output = XmStringConcatAndFree(output, XmStringSeparatorCreate());

  /* Type */
  /* tmp = XmStringGenerate("Type: \t", NULL, NULL, NULL); */
  tmp = XmStringCreateLocalized("Type: ");
  output = XmStringConcatAndFree(output, tmp);
  typename = GetSafeAtom(XtDisplay(w), datums[index].type);
  if (typename == NULL) typename = XtNewString("Unknown");
  tmp = XmStringCreateLocalized(typename);
  XtFree(typename);
  output = XmStringConcatAndFree(output, tmp);
  output = XmStringConcatAndFree(output, XmStringSeparatorCreate());

  /* Value display */
  /* tmp = XmStringGenerate("Value: \t", NULL, NULL, NULL); */
  tmp = XmStringCreateLocalized("Value: ");
  output = XmStringConcatAndFree(output, tmp);
  tmp = GetItemValue(text_display);
  output = XmStringConcatAndFree(output, tmp);

  /* Put value into display */
  XmCSTextSetString(text_display, output);
  XmStringFree(output);
}

/*********************************************************************
 * EditItem
 * 
 * This is called from the Properties or New menu item.  If param
 * is 0 then this is a new item.
 *********************************************************************/

void 
EditItem(Widget w, int param, XtPointer ignore)
{
  int index = current_selected_item;
  Atom XA_COMPOUND_TEXT = XInternAtom(XtDisplay(w), XmSCOMPOUND_TEXT, False),
       XA_PIXEL = XInternAtom(XtDisplay(w), "PIXEL", False),
       XA_LOCALE_TEXT = XmeGetEncodingAtom(w),
       XA_MOTIF_CS = XInternAtom(XtDisplay(w), 
				 XmS_MOTIF_COMPOUND_STRING, False),
       XA_MOTIF_RT = XInternAtom(XtDisplay(w), 
				 XmS_MOTIF_RENDER_TABLE, False);
  Widget mem = (Widget) NULL;
  char *targetname;
  XmString tmp;

  if (index >= num_datums) return;

  global_type = type = datums[index].type;
  global_value = datums[index].value;
  global_length = datums[index].length;
  global_format = datums[index].format;
  global_deferred = datums[index].deferred;
  global_set_defer = global_deferred;
  global_set_type_spec = global_type;
  global_read_format_spec = 'S';

  if (param != 0) {
    /* Put name of target into display field */
    targetname = GetSafeAtom(XtDisplay(w), datums[index].target);
    
    if (targetname == NULL) targetname = XtNewString("Unknown");

    tmp = XmStringCreateLocalized(targetname);
    XmCSTextSetString(text2, tmp);
    XmStringFree(tmp);
    tmp = GetItemValue(text1);
    XmCSTextSetString(text1, tmp);
    XmStringFree(tmp);
    XtFree(targetname);
    SetDisplayTypeFromType(text1, global_type);
    global_read_format_spec = GetDisplayFormatFromType(text1, global_type);
    setFormatAndMenu(global_read_format_spec);
    XmToggleButtonGadgetSetState(defer, global_deferred, False);
  } else {
    tmp = XmStringCreateLocalized("");
    XmCSTextSetString(text1, tmp);
    XmCSTextSetString(text2, tmp);
    SetDisplayTypeFromType(text1, XA_STRING);
    setFormatAndMenu('S');
    XmToggleButtonGadgetSetState(defer, False, False);
  }

}

static char 
GetDisplayFormatFromType(Widget w, Atom type)
{
  Atom XA_COMPOUND_TEXT = XInternAtom(XtDisplay(w), XmSCOMPOUND_TEXT, False),
       XA_PIXEL = XInternAtom(XtDisplay(w), "PIXEL", False),
       XA_LOCALE_TEXT = XmeGetEncodingAtom(w),
       XA_MOTIF_CS = XInternAtom(XtDisplay(w), 
				 XmS_MOTIF_COMPOUND_STRING, False),
       XA_MOTIF_RT = XInternAtom(XtDisplay(w), 
				 XmS_MOTIF_RENDER_TABLE, False);

  if (type == XA_ATOM ||
      type == XInternAtom(XtDisplay(w), "ATOM_PAIR", False) ||
      type == XA_INTEGER || type == XA_PIXEL ||
      type == XInternAtom(XtDisplay(w), "SPAN", False)) {
    return('D');
  } else if (type == XA_STRING) {
    return('S');
  } else if (type == XA_COMPOUND_TEXT) {
    return('T');
  } else if (type == XA_LOCALE_TEXT) {
    return('L');
  } else if (type == XA_DRAWABLE || type == XA_WINDOW ||
	     type == XA_COLORMAP) {
    return('H');
  } else if (type == XA_MOTIF_CS) {
    return('C');
  } else if (type == XmeGetEncodingAtom(w)) {
    return('L');
  } else {
    return('H');
  }
}

static void
SetDisplayTypeFromType(Widget w, Atom type)
{
  Atom XA_COMPOUND_TEXT = XInternAtom(XtDisplay(w), XmSCOMPOUND_TEXT, False),
       XA_PIXEL = XInternAtom(XtDisplay(w), "PIXEL", False),
       XA_LOCALE_TEXT = XmeGetEncodingAtom(w),
       XA_MOTIF_CS = XInternAtom(XtDisplay(w), 
				 XmS_MOTIF_COMPOUND_STRING, False),
       XA_MOTIF_RT = XInternAtom(XtDisplay(w), 
				 XmS_MOTIF_RENDER_TABLE, False);
  Widget mem;

  if (type == XA_ATOM ||
      type == XInternAtom(XtDisplay(w), "ATOM_PAIR", False)) {
    char str[80];
    mem = FindRightButton("ATOM");
  } else if (type == XA_INTEGER || type == XA_PIXEL) {
    if (type == XA_INTEGER)
      mem = FindRightButton("INTEGER");
    else
      mem = FindRightButton("PIXEL");
  } else if (type == XInternAtom(XtDisplay(w), "SPAN", False)) {
    mem = FindRightButton("SPAN");
  } else if (type == XA_STRING) {
    mem = FindRightButton("STRING");
  } else if (type == XA_COMPOUND_TEXT) {
    char *str;
    mem = FindRightButton("COMPOUND_TEXT");
  } else if (type == XA_LOCALE_TEXT) {
    mem = FindRightButton("LOCALE_TEXT");
  } else if (type == XA_DRAWABLE || type == XA_WINDOW ||
	     type == XA_COLORMAP) {
    if (type == XA_DRAWABLE)
      mem = FindRightButton("DRAWABLE");
    else if (type == XA_COLORMAP)
      mem = FindRightButton("COLORMAP");
    else
      mem = FindRightButton("WINDOW");
  } else if (type == XA_MOTIF_CS) {
    mem = FindRightButton("_MOTIF_COMPOUND_STRING");
  } else if (type == XmeGetEncodingAtom(w)) {
    mem = FindRightButton("LOCALE");
  } else {
    char *typestring;
    char buffer[80];

    mem = FindRightButton("unknown");

    typestring = GetSafeAtom(XtDisplay(w), type);
    sprintf(buffer, "Unknown type %s (%d) found", typestring, type);
    XFree(typestring);
  }

  if (mem != (Widget) NULL)
    XtVaSetValues(typeOM, XmNmenuHistory, mem, NULL);
}

static void
setFormatAndMenu(char in)
{
  Widget mem = (Widget) NULL;

  if (in == 'S') {
    /* C String display */
    mem = format_s;
  } else if (in == 'L') {
    /* Locale Text display */
    mem = format_l;
  } else if (in == 'T') {
    /* Compound Text display */
    mem = format_t;
  } else if (in == 'C') {
    /* Compound String display */
    mem = format_cs;
  } else if (in == 'D') {
    /* Decimal display */
    mem = format_id;
  } else {
    /* Hex display */
    mem = format_ih;
  }

  SetFormat(NULL, in, NULL);

  if (mem != (Widget) NULL)
    XtVaSetValues(format_om, XmNmenuHistory, mem, NULL);
}

char *output_buffer = NULL;
int output_buffer_max = 0;
int output_buffer_current = 0;

void ClearOB() 
{
  output_buffer_current = 0;
  if (output_buffer_max > 0)
    output_buffer[0] = 0;
}

void 
AppendOB(char *str)
{
  int len;
  int i;

  len = strlen(str);
  while(len + output_buffer_current > output_buffer_max) {
    if (output_buffer_max == 0) {
      output_buffer_max = 256;
      output_buffer = XtMalloc(output_buffer_max);
    } else {
      output_buffer_max *= 2;
      output_buffer = XtRealloc(output_buffer, output_buffer_max);
    }
  }

  /* Copy data */
  for(i = 0; i <= len; i++)
    output_buffer[output_buffer_current + i] = str[i];

  /* Update end pointer */
  output_buffer_current += len;
}

void 
SetFormat(Widget w, char in, XtPointer ignore)
{
  int byte_length;
  XmString strtmp;

  global_read_format_spec = in;
}

static 
XmString GetItemValue(Widget w)
{
  int byte_length;
  XmString strtmp;
  char in;

  byte_length = global_length * global_format / 8;

  in = GetDisplayFormatFromType(w, global_type);

  if (in == 'S') {
    /* C String display */
    strtmp = XmStringCreateLocalized(global_value);
  } else if (in == 'L') {
    /* Locale Text display */
    strtmp = XmStringCreateLocalized(global_value);
  } else if (in == 'T') {
    /* Compound Text display */
    strtmp = XmCvtCTToXmString(global_value);
  } else if (in == 'C') {
    /* Compound String display */
    strtmp = XmCvtByteStreamToXmString(global_value);
  } else if (in == 'D') {
    /* Decimal display */
    int count, i;

    count = byte_length / 4; 
    ClearOB();
    for(i = 0; i < count; i++) {
      char buf[32];

      sprintf(buf, "%d ", ((int *) global_value)[i]);
      AppendOB(buf);
    }
    strtmp = XmStringCreateLocalized(output_buffer);
  } else {
    /* Decimal display */
    int count, i;

    count = byte_length / 4; 
    ClearOB();
    for(i = 0; i < count; i++) {
      char buf[32];

      sprintf(buf, "0x%x ", ((int *) global_value)[i]);
      AppendOB(buf);
    }
    strtmp = XmStringCreateLocalized(output_buffer);
  }

  return(strtmp);
}

/*******************************************************************
 * SetValue
 *
 * No,  it's not a widget method.  This SetValue implements an action
 * which takes the current value in the interface and stores it away
 * for that particular target.  This allows arbitrary data to be stored
 * into the data array for later transfer.
 *******************************************************************/
void
SetValue(Widget widget, char *ignore, XmAnyCallbackStruct *callback_data)
{
  XmString value;
  XmString target_name;
  char *type_widget_name;
  char *display_format_widget_name;
  char *target_string;
  Widget mem_history;
  Arg args[1];
  int index;
  int base;
  Atom target;
  
  target_name = XmCSTextGetString(text2);
  value = XmCSTextGetString(text1);
  
  if (global_set_type_spec == None) return;

  /* Get the target string from the XmString */
  target_string = ConvertXmStringToString(target_name);
  target = XInternAtom(XtDisplay(widget), target_string, False);
  XtFree(target_string);
  
  /* Find the index of this target,  or add it to the end */
  for(index = 0; index < num_datums; index++)
    if (datums[index].target == target) break;

  /* Add to the end,  if this is a new target */
  if (index == num_datums) {
    XmListAddItemUnselected(list, target_name, 0);
    datums[index].target = target;
    num_datums++;
  }

  /* Handle deferred */
  if (datums[index].deferred != global_set_defer) {
    XmString new_item[1];
    datums[index].deferred = global_set_defer;

    if (datums[index].deferred) {
      char *buffer;

      buffer = XtMalloc(strlen(target_string) + 2 + strlen(DEFSTR));
      strcpy(buffer, target_string);
      strcat(buffer, "\t");
      strcat(buffer, DEFSTR);
      new_item[0] = XmStringGenerate(buffer, NULL, XmCHARSET_TEXT, NULL);
      XtFree(buffer);
    } else
      new_item[0] = XmStringCopy(target_name);

    XmListReplaceItemsPos(list, new_item, 1, index + 1);
    XmListSelectPos(list, index + 1, False);
    XmStringFree(new_item[0]);
  }

  datums[index].type = global_set_type_spec;
  
  /* Convert the value */
  if (global_read_format_spec == 'S') {
    datums[index].value = ConvertXmStringToString(value);
    if (datums[index].value != NULL) {
      datums[index].length = strlen(datums[index].value);
    } else {
      datums[index].length = 0;
    }
    datums[index].format = 8;
  } else if (global_read_format_spec == 'L') {
    char *ctext;
    char *ltext = NULL;
    Atom COMPOUND_TEXT = XInternAtom(XtDisplay(widget), 
				     XmSCOMPOUND_TEXT, False);
    XTextProperty text_prop;
    int status;
    char ** values;
    int num_values = 0;
    int malloc_size = 0;
    int i;

    ctext = XmCvtXmStringToCT(value);

    text_prop.value = (unsigned char *) ctext;
    text_prop.encoding = COMPOUND_TEXT;
    text_prop.format = 8;
    text_prop.nitems = strlen(ctext);

    status = XmbTextPropertyToTextList(XtDisplay(widget), &text_prop, &values,
				       &num_values);

    XtFree(ctext);

    if (num_values) { 
      for (i = 0; i < num_values ; i++)
	malloc_size += strlen(values[i]);

      ltext = XtMalloc ((unsigned) malloc_size + 1);
      ltext[0] = '\0';
      for (i = 0; i < num_values ; i++)
	strcat(ltext, values[i]);
      XFreeStringList(values);
    } else {
      ltext = XtNewString("");
    }

    datums[index].value = ltext;
    datums[index].length = malloc_size;
    datums[index].format = 8;
  } else if (global_read_format_spec == 'T') {
    datums[index].value = XmCvtXmStringToCT(value);
    if (datums[index].value != NULL) {
      datums[index].length = strlen(datums[index].value);
    } else {
      datums[index].length = 0;
    }
    datums[index].format = 8;
  } else if (global_read_format_spec == 'C') {
    datums[index].length = 
      XmCvtXmStringToByteStream(value, 
				(unsigned char **) &datums[index].value);
    datums[index].format = 8;
  } else if (global_read_format_spec == 'D' ||
	     global_read_format_spec == 'H') {
    /* Doesn't matter,  the numbers contain the base information.
       We assume a string with whitespace separating the values. */
    int max_values = 5;
    long *array = (long *) XtMalloc(sizeof(long) * max_values);
    int count = 0;
    char *convert;
    char *current;
    char *end;
    Boolean done = False;
    
    convert = ConvertXmStringToString(value);
    current = convert;

    if (global_read_format_spec == 'H')
      base = 16;
    else
      base = 0;

    while(! done) {
      /* Realloc array if getting too big */
      if (count >= max_values) {
	max_values += 5;
	array = (long *) XtRealloc((char*) array, sizeof(long) * max_values);
      }
      if (current == 0) break;
      /* Now grab the next number */
      array[count++] = strtol(current, &end, base);
      if (current == end || errno == ERANGE) {
	done = True;
	count--;
      } else 
	current = end;
    }

    datums[index].value = (XtPointer) array;
    datums[index].length = count;
    datums[index].format = 32;
  }

  XmStringFree(target_name);
  XmStringFree(value);
  XmListSelectPos(list, index + 1, True);
}

/*******************************************************************
 * RemoveValue
 *
 * This removes an element from the data array.
 *******************************************************************/
void 
RemoveValue(Widget widget, char *ignore, XmAnyCallbackStruct *callback_data)
{
  if (current_selected_item < 0 ||
      current_selected_item >= num_datums) {
    XBell(XtDisplay(widget), 100);
  } else {
    int i;

    /* Free current value pointed at */
    XtFree(datums[current_selected_item].value);

    num_datums--;
    /* Remove item from internal list */
    for(i = current_selected_item; i < num_datums; i++)
      datums[i] = datums[i + 1];

    /* Remove from screen display */
    XmListDeletePos(list, current_selected_item + 1);
    /* Now update the display with the new value */
    if (current_selected_item < num_datums)
      XmListSelectPos(list, current_selected_item + 1, True);
    else if (num_datums > 0)
      XmListSelectPos(list, current_selected_item, True);
  }
}

void 
deferCB(Widget widget, char *ignore,
	XmToggleButtonCallbackStruct *callback_data)
{
  char *targetname;
  XmString new_item[1];

  global_set_defer = callback_data -> set;
}

XmParseMapping table[2];
int table_size = -1;

char*
ConvertXmStringToString(XmString xmstr)
{
  char *rvalue;

  if (table_size < 0) {
    XmString tmp;
    int nargs;
    Arg args[10];

    table_size = 0;
    tmp = XmStringComponentCreate(XmSTRING_COMPONENT_TAB, 0, NULL);
    nargs = 0;
    XtSetArg(args[nargs], XmNincludeStatus, XmINSERT), nargs++;
    XtSetArg(args[nargs], XmNsubstitute, tmp), 	 nargs++;
    XtSetArg(args[nargs], XmNpattern, "\t"), 		 nargs++;
    table[table_size++] = XmParseMappingCreate(args, nargs);
    XmStringFree(tmp);

    tmp = XmStringSeparatorCreate();
    nargs = 0;
    XtSetArg(args[nargs], XmNincludeStatus, XmINSERT), nargs++;
    XtSetArg(args[nargs], XmNsubstitute, tmp),	 nargs++;
    XtSetArg(args[nargs], XmNpattern, "\n"),		 nargs++;
    table[table_size++] = XmParseMappingCreate(args, nargs);

    XmStringFree(tmp);
  }

  rvalue = (char *) XmStringUnparse(xmstr, NULL, 
				    XmMULTIBYTE_TEXT, XmMULTIBYTE_TEXT,
				    table, table_size, XmOUTPUT_ALL);

  return(rvalue);
}
