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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: MapEvents.c /main/12 1995/09/19 23:05:22 cde-sun $"
#endif
#endif

#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <Xm/XmP.h>
#include "XmI.h"
#include "MapEventsI.h"

typedef String (*XmEventParseProc)(String str,
				   unsigned int closure,
				   unsigned long *detail,
				   Boolean *status);

typedef struct {
   XmConst char    *event;
   XrmQuark         signature;
   int              eventType;
   XmEventParseProc parseProc;
   unsigned int     closure;
} EventKey;

/********    Static Function Declarations    ********/

static int StrToHex(
                        String str) ;
static int StrToOct(
                        String str) ;
static int StrToNum(
                        String str) ;
static void FillInQuarks(
                        EventKey *table) ;
static Boolean LookupModifier(
                        String name,
                        Modifiers *valueP) ;
static String ScanAlphanumeric(
                        String str) ;
static String ScanWhitespace(
                        String str) ;
static String ParseImmed(
                        String str,
                        unsigned int closure,
                        unsigned long *detail,
			Boolean *status) ;
static String ParseKeySym(
                        String str,
                        unsigned int closure,
                        unsigned long *detail,
			Boolean *status) ;
static String ParseModifiers(
                        String str,
                        Modifiers *modifiers,
                        Boolean *status) ;
static String ParseEventType(
                        String str,
                        EventKey *table,
                        int *eventType,
                        Cardinal *_index,
                        Boolean *status) ;
static String _MapEvent(
                        String str,
                        EventKey *table,
                        int *eventType,
                        unsigned long *detail,
                        Modifiers *modifiers,
			Boolean *status) ;

/********    End Static Function Declarations    ********/


static EventKey modifierStrings[] = {

/* Modifier,	Quark,		Mask */

{"None",	NULLQUARK,	0,		NULL,		None},
{"Shift",	NULLQUARK,	0,		NULL,		ShiftMask},
{"Lock",	NULLQUARK,	0,		NULL,		LockMask},
{"Ctrl",	NULLQUARK,	0,		NULL,		ControlMask},
{"Meta",	NULLQUARK,	0,		NULL,		Mod1Mask},
{"Alt",		NULLQUARK,	0,		NULL,		Mod1Mask},
{"Mod1",	NULLQUARK,	0,		NULL,		Mod1Mask},
{"Mod2",	NULLQUARK,	0,		NULL,		Mod2Mask},
{"Mod3",	NULLQUARK,	0,		NULL,		Mod3Mask},
{"Mod4",	NULLQUARK,	0,		NULL,		Mod4Mask},
{"Mod5",	NULLQUARK,	0,		NULL,		Mod5Mask},
{NULL,		NULLQUARK,	0,		NULL,		0}};

static EventKey buttonEvents[] = {

/* Event Name,	Quark,		Event Type,	DetailProc,	Closure */

{"Btn1Down",	NULLQUARK,	ButtonPress,	ParseImmed,	Button1},
{"Button1",	NULLQUARK,	ButtonPress,	ParseImmed,	Button1},
{"Btn1",	NULLQUARK,	ButtonPress,	ParseImmed,	Button1},
{"Btn2Down",	NULLQUARK,	ButtonPress,	ParseImmed,	Button2},
{"Button2",	NULLQUARK,	ButtonPress,	ParseImmed,	Button2},
{"Btn2",	NULLQUARK,	ButtonPress,	ParseImmed,	Button2},
{"Btn3Down",	NULLQUARK,	ButtonPress,	ParseImmed,	Button3},
{"Button3",	NULLQUARK,	ButtonPress,	ParseImmed,	Button3},
{"Btn3",	NULLQUARK,	ButtonPress,	ParseImmed,	Button3},
{"Btn4Down",	NULLQUARK,	ButtonPress,	ParseImmed,	Button4},
{"Button4",	NULLQUARK,	ButtonPress,	ParseImmed,	Button4},
{"Btn4",	NULLQUARK,	ButtonPress,	ParseImmed,	Button4},
{"Btn5Down",	NULLQUARK,	ButtonPress,	ParseImmed,	Button5},
{"Button5",	NULLQUARK,	ButtonPress,	ParseImmed,	Button5},
{"Btn5",	NULLQUARK,	ButtonPress,	ParseImmed,	Button5},
{NULL,		NULLQUARK,	0,		NULL,		0}};


static EventKey keyEvents[] = {

/* Event Name,	Quark,		Event Type,	DetailProc	Closure */

{"KeyPress",	NULLQUARK,	KeyPress,	ParseKeySym,	0},
{"Key",		NULLQUARK,	KeyPress,	ParseKeySym,	0},
{"KeyDown",	NULLQUARK,	KeyPress,	ParseKeySym,	0},
{"KeyUp",	NULLQUARK,	KeyRelease,	ParseKeySym,	0},
{"KeyRelease",	NULLQUARK,	KeyRelease,	ParseKeySym,	0},
{NULL,		NULLQUARK,	0,		NULL,		0}};

static XmConst Modifiers buttonModifierMasks[] = {
    0, Button1Mask, Button2Mask, Button3Mask, Button4Mask, Button5Mask
};

static Boolean initialized = FALSE;



/*************************************<->*************************************
 *
 *  Numeric convertion routines
 *
 *   Description:
 *   -----------
 *     StrToHex = Parse an ASCII string as a hexadecimal integer.
 *     StrToOct = Parse an ASCII string as an octal integer.
 *     StrToNum = Parse an ASCII string as a hex, octal or decimal integer
 *         based on leading "0", "0x", or "0X" characters.
 *
 *
 *   Inputs:
 *   ------
 *     str = A null-terminated span of digits.
 *         StrToNum handles C prefix conventions (0d = octal, 0xd = hex),
 *	   but no other non-digits are allowed.
 *
 *   Outputs:
 *   -------
 *     Returns -1 if conversion fails, otherwise the numeric value.
 *         Empty strings convert to 0.
 *
 *   Procedures Called
 *   -----------------
 *     None.
 *
 *************************************<->***********************************/
static int
StrToHex(
        String str )
{
    char   c;
    int    val = 0;

    while ((c = *str) != '\0') {
	if ('0' <= c && c <= '9') val = val*16+c-'0';
	else if ('a' <= c && c <= 'f') val = val*16+c-'a'+10;
	else if ('A' <= c && c <= 'F') val = val*16+c-'A'+10;
	else return -1;
	str++;
    }

    return val;
}

static int
StrToOct(
        String str )
{
    char c;
    int  val = 0;

    while ((c = *str) != '\0') {
        if ('0' <= c && c <= '7') val = val*8+c-'0'; else return -1;
	str++;
    }

    return val;
}

static int
StrToNum(
        String str )
{
    char c;
    int val = 0;

    if (*str == '0') {
	str++;
	if (*str == 'x' || *str == 'X') return StrToHex(++str);
	else return StrToOct(str);
    }

    while ((c = *str) != '\0') {
	if ('0' <= c && c <= '9') val = val*10+c-'0';
	else return -1;
	str++;
    }

    return val;
}


/*************************************<->*************************************
 *
 *  FillInQuarks (EventKey *table)
 *
 *   Description:
 *   -----------
 *     Converts each string entry in the modifier/event tables to a
 *     quark, thus facilitating faster comparisons.
 *
 *
 *   Inputs:
 *   ------
 *     table[*].event = strings to be converted (NULL at end of table).
 *
 *   Outputs:
 *   -------
 *     table[*].signature = quarks for each string.
 *
 *   Procedures Called
 *   -----------------
 *     XrmStringToQuark.
 *
 *************************************<->***********************************/
static void
FillInQuarks(
        EventKey *table )
{
    int i;

    for (i=0; table[i].event; i++)
        table[i].signature = XrmPermStringToQuark(table[i].event);
}


/*************************************<->*************************************
 *
 *  LookupModifier (name, *valueP)
 *
 *   Description:
 *   -----------
 *     Compare the passed in string to the list of valid modifiers.
 *
 *
 *   Inputs:
 *   ------
 *     name = string to be located in the global modiferStrings table.
 *     (modiferStrings = global table of valid modifiers).
 *
 *   Outputs:
 *   -------
 *     *valueP = closure of modifierString with a matching signature.
 *     Return value indicates whether a match was found.
 *
 *   Procedures Called
 *   -----------------
 *     XrmStringToQuark.
 *
 *************************************<->***********************************/
static Boolean
LookupModifier(
        String name,
        Modifiers *valueP )
{
    int i;
    XrmQuark signature = XrmStringToQuark(name);

    for (i=0; modifierStrings[i].event != NULL; i++)
	if (modifierStrings[i].signature == signature) {
	    *valueP = modifierStrings[i].closure;
	    return TRUE;
	}

    return FALSE;
}


/*************************************<->*************************************
 *
 *  ScanAlphanumeric (String str)
 *
 *   Description:
 *   -----------
 *     Scan string until a non-alphanumeric character is encountered.
 *
 *
 *   Inputs:
 *   ------
 *     str = string to be scanned.
 *
 *   Outputs:
 *   -------
 *     Return value points to the first non-alphanumeric character in str.
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/
static String
ScanAlphanumeric(
        String str )
{
    while (
        ('A' <= *str && *str <= 'Z') || ('a' <= *str && *str <= 'z')
	|| ('0' <= *str && *str <= '9')) str++;
    return str;
}


/*************************************<->*************************************
 *
 *  ScanWhitespace (String str)
 *
 *   Description:
 *   -----------
 *     Scan the string, skipping over all white space characters.
 *     Whitespace is defined as tab or space.
 *
 *
 *   Inputs:
 *   ------
 *     str = the string to be scanned.
 *
 *   Outputs:
 *   -------
 *     Return value points to the first non-whitespace character in str.
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/
static String
ScanWhitespace(
        String str )
{
    while (*str == ' ' || *str == '\t') str++;
    return str;
}


/*************************************<->*************************************
 *
 *  ParseImmed
 *
 *   Description:
 *   -----------
 *     An XmEventParseProc.  Copy closure into detail.
 *
 *
 *   Inputs:
 *   ------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Outputs:
 *   -------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/
static String
ParseImmed(
        String str,
        unsigned int closure,
        unsigned long *detail,
	Boolean *status)
{
   *detail = closure;
   *status = TRUE;
   return str;
}


/*************************************<->*************************************
 *
 *  ParseKeySym (parameters)
 *
 *   Description:
 *   -----------
 *     An XmeventParseProc.
 *
 *
 *   Inputs:
 *   ------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Outputs:
 *   -------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/
static String
ParseKeySym(
        String str,
        unsigned int closure,
        unsigned long *detail,
	Boolean *status)
{
  char keySymName[100];
  char *start = str;

  /* Initialize the return values. */
  *detail = NoSymbol;
  *status = FALSE;

  str = ScanWhitespace(str);

  if (*str == '\\') {
    /* "\x"; interpret "x" as a Keysym. */
    str++;
    keySymName[0] = *str++;
    keySymName[1] = '\0';
    *detail = XStringToKeysym(keySymName);
  } else if (*str == ',' || *str == ':') {
    /* No detail; return a failure */
    return str;
  } else {
    while (*str != ',' &&
	   *str != ':' &&
	   *str != ' ' &&
	   *str != '\t' &&
	   *str != '\n' &&
	   *str != '\0') str++;
    (void) strncpy(keySymName, start, str-start);
    keySymName[str-start] = '\0';
    *detail = XStringToKeysym(keySymName);
  }

  if (*detail == NoSymbol)
    {
      if (( '0' <= keySymName[0]) && (keySymName[0] <= '9'))
	{
	  int retval = StrToNum(keySymName);
	  if (-1 == retval)
	    {
              *detail = 0;
              return str;
	    }
	  else
	    {
	      *detail = retval;
	      *status = TRUE;
	      return str;
	    }
	}
      return str;
    }
  else
    {
      *status = TRUE;
      return str;
    }
}

/*************************************<->*************************************
 *
 *  ParseModifiers (parameters)
 *
 *   Description:
 *   -----------
 *     Parse the string, extracting all modifier specifications.
 *
 *
 *   Inputs:
 *   ------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Outputs:
 *   -------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/
static String
ParseModifiers(
        String str,
        Modifiers *modifiers,
        Boolean *status )
{
    String start;
    char modStr[100];
    Boolean notFlag;
    Modifiers maskBit;

    /* Initially assume all is going to go well */
    *status = TRUE;
    *modifiers = 0;

    /* Attempt to parse the first button modifier */
    str = ScanWhitespace(str);
    start = str;
    str = ScanAlphanumeric(str);
    if (start != str) {
         (void) strncpy(modStr, start, str-start);
          modStr[str-start] = '\0';
          if (LookupModifier(modStr, &maskBit))
          {
	    if (maskBit== None) {
		*modifiers = 0;
                str = ScanWhitespace(str);
	        return str;
            }
         }
         str = start;
    }


    /* Keep parsing modifiers, until the event specifier is encountered */
    while ((*str != '<') && (*str != '\0')) {
        if (*str == '~') {
             notFlag = TRUE;
             str++;
          } else
              notFlag = FALSE;

	start = str;
        str = ScanAlphanumeric(str);
        if (start == str) {
           /* ERROR: Modifier or '<' missing */
           *status = FALSE;
           return str;
        }
        (void) strncpy(modStr, start, str-start);
        modStr[str-start] = '\0';

        if (!LookupModifier(modStr, &maskBit))
        {
           /* Unknown modifier name */
           *status = FALSE;
           return str;
        }

	if (notFlag)
           *modifiers &= ~maskBit;
	else
           *modifiers |= maskBit;
        str = ScanWhitespace(str);
    }

    return str;
}


/*************************************<->*************************************
 *
 *  ParseEventType (parameters)
 *
 *   Description:
 *   -----------
 *     xxxxxxxxxxxxxxxxxxxxxxx
 *
 *
 *   Inputs:
 *   ------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Outputs:
 *   -------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/
static String
ParseEventType(
        String str,
        EventKey *table,
        int *eventType,
        Cardinal *_index,
        Boolean *status )
{
    String start = str;
    char eventTypeStr[100];
    Cardinal   i;
    XrmQuark	signature;

    /* Parse out the event string */
    str = ScanAlphanumeric(str);
    (void) strncpy(eventTypeStr, start, str-start);
    eventTypeStr[str-start] = '\0';

    /* Attempt to match the parsed event against our supported event set */
    signature = XrmStringToQuark(eventTypeStr);
    for (i = 0; table[i].signature != NULLQUARK; i++)
        if (table[i].signature == signature)
        {
           *_index = i;
           *eventType = table[*_index].eventType;

           *status = TRUE;
           return str;
        }

    /* Unknown event specified */
    *status = FALSE;
    return (str);
}


/*************************************<->*************************************
 *
 *  _MapEvent (parameters)
 *
 *   Description:
 *   -----------
 *     xxxxxxxxxxxxxxxxxxxxxxx
 *
 *
 *   Inputs:
 *   ------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Outputs:
 *   -------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/
static String
_MapEvent(
        String str,
        EventKey *table,
        int *eventType,
        unsigned long *detail,
        Modifiers *modifiers,
	Boolean *status)
{
  Cardinal index=0;

  /* Initialize, if first time called */
  _XmProcessLock();
  if (!initialized)
    {
      initialized = TRUE;
      FillInQuarks (buttonEvents);
      FillInQuarks (modifierStrings);
      FillInQuarks (keyEvents);
    }
  _XmProcessUnlock();

  /* Parse the modifiers and the '<' */
  str = ParseModifiers(str, modifiers, status);
  if (*str != '<')
    *status = FALSE;
  if (*status == FALSE)
    return str;
  str++;

  /* Parse the event type and detail and the '>' */
  str = ParseEventType(str, table, eventType, &index, status);
  if (*str != '>')
    *status = FALSE;
  if (*status == FALSE)
    return str;
  str++;

  /* Save the detail */
  return ((*(table[index].parseProc))(str, table[index].closure,
				       detail, status));
}

/*************************************<->*************************************
 *
 *  _MapBtnEvent (parameters)
 *
 *   Description:
 *   -----------
 *     xxxxxxxxxxxxxxxxxxxxxxx
 *
 *
 *   Inputs:
 *   ------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Outputs:
 *   -------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/
Boolean
_XmMapBtnEvent(
        String str,
        int *eventType,
        unsigned int *button,
        Modifiers *modifiers )
{
  Boolean status;
  unsigned long detail;
  _MapEvent (str, buttonEvents, eventType, &detail, modifiers, &status);
  *button = detail;
  if (status == FALSE)
    return (FALSE);

  /*
   * The following is a fix for an X11 deficiency in regards to
   * modifiers in grabs.
   */
  if (*eventType == ButtonRelease)
    {
      /* the button that is going up will always be in the modifiers... */
      *modifiers |= buttonModifierMasks[*button];
    }

  return (TRUE);
}

/*************************************<->*************************************
 *
 *  _XmMapKeyEvents (parameters)
 *
 *   Description:
 *   -----------
 *     Parse a comma-separated list of key events.
 *
 *
 *   Inputs:
 *   ------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Outputs:
 *   -------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/

int
_XmMapKeyEvents(String      str,
		int       **eventTypes,
		KeySym    **keysyms,
		Modifiers **modifiers)
{
  Boolean status = TRUE;
  int count = 0;
  char *ptr = str;

  *eventTypes = NULL;
  *keysyms = NULL;
  *modifiers = NULL;
  while (status)
    {
      int       	tmp_type;
      unsigned long	tmp_sym;
      Modifiers 	tmp_mods;

      /* Parse a single event. */
      ptr = _MapEvent(ptr, keyEvents, &tmp_type, &tmp_sym, &tmp_mods, &status);
      if (!status)
	break;

      /* Save this event. */
      *eventTypes = (int *)
	XtRealloc((char*) *eventTypes, (count + 1) * sizeof(int));
      (*eventTypes)[count] = tmp_type;
      *keysyms = (KeySym *)
	XtRealloc((char*) *keysyms, (count + 1) * sizeof(KeySym));
      (*keysyms)[count] = (KeySym)tmp_sym;
      *modifiers = (Modifiers *)
	XtRealloc((char*) *modifiers, (count + 1) * sizeof(Modifiers));
      (*modifiers)[count] = tmp_mods;
      count++;

      /* Skip the separator. */
      ptr = ScanWhitespace(ptr);
      if (*ptr == '\0')
	break;
      else if (*ptr == ',')
	ptr++;
      else
	status = FALSE;
    }

  /* Discard partial results if something fails. */
  if (!status)
    {
      count = 0;
      XtFree((char*) *eventTypes);
      *eventTypes = NULL;
      XtFree((char*) *keysyms);
      *keysyms = NULL;
      XtFree((char*) *modifiers);
      *modifiers = NULL;
    }

  return count;
}

/*************************************<->*************************************
 *
 *  _XmMatchBtnEvent (parameters)
 *
 *   Description:
 *   -----------
 *     Compare the passed in event to the event described by the parameter
 *     list.
 *
 *
 *   Inputs:
 *   ------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Outputs:
 *   -------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/
Boolean
_XmMatchBtnEvent(
        XEvent *event,
        int eventType,
        unsigned int button,
        Modifiers modifiers )
{
   Modifiers state =
     event->xbutton.state & (ShiftMask | LockMask | ControlMask | Mod1Mask |
			     Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask);
   if (((eventType == XmIGNORE_EVENTTYPE)||(event->type == eventType)) &&
       (event->xbutton.button == button) &&
       ((modifiers == AnyModifier)||(state == modifiers)) )
      return (TRUE);
   else
      return (FALSE);
}

/*************************************<->*************************************
 *
 *  _XmMatchKeyEvent (parameters)
 *
 *   Description:
 *   -----------
 *     Compare the passed in event to the event described by the parameter
 *     list.
 *
 *
 *   Inputs:
 *   ------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Outputs:
 *   -------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/
Boolean
_XmMatchKeyEvent(
        XEvent *event,
        int eventType,
        unsigned int key,
        Modifiers modifiers )
{
   Modifiers state, mods;

   _XmCheckInitModifiers();

   state = event->xkey.state & ~(LockMask|ScrollLockMask|NumLockMask);
   mods  = modifiers & ~(LockMask|ScrollLockMask|NumLockMask);

   if ((event->type == eventType) &&
       (event->xkey.keycode == key) &&
       (state == mods))
      return (TRUE);
   else
      return (FALSE);
}

