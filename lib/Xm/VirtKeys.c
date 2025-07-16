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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: VirtKeys.c /main/22 1999/06/02 14:45:52 samborn $"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <X11/keysym.h>
#include <Xm/DisplayP.h>
#include <Xm/TransltnsP.h>
#include <Xm/AtomMgr.h>
#include <Xm/XmosP.h>
#include "MapEventsI.h"
#include "VirtKeysI.h"
#include "XmosI.h"
#include "XmI.h"

#define defaultFallbackBindings _XmVirtKeys_fallbackBindingString

#define BUFFERSIZE	2048
#define MAXLINE		256

/********    Static Function Declarations    ********/

static Boolean CvtStringToVirtualBinding(Display *dpy,
					 XrmValuePtr args,
					 Cardinal *num_args,
					 XrmValuePtr fromVal,
					 XrmValuePtr toVal,
					 XtPointer *closure_ret);
static void FillBindingsFromDB(Display *dpy,
			       XrmDatabase rdb,
			       XmVKeyBinding *keys,
			       Cardinal *num_keys);
static Boolean GetBindingsProperty(Display *display,
				   String property,
				   String *binding);
static void FindVirtKey(Display *dpy,
                        KeyCode keycode,
                        Modifiers modifiers,
                        Modifiers *modifiers_return,
                        KeySym *keysym_return);
static Modifiers EffectiveStdModMask(Display *dpy,
				     KeySym *kc_map,
				     int ks_per_kc);
static void LoadVendorBindings(Display *display,
			       char *path,
			       FILE *fp,
			       String *binding);

/********    End Static Function Declarations    ********/

static XmConst XmVirtualKeysymRec virtualKeysyms[] =
{
  { XmVosfActivate,		osfXK_Activate	      },
  { XmVosfAddMode,		osfXK_AddMode	      },
  { XmVosfBackSpace,		osfXK_BackSpace	      },
  { XmVosfBackTab,		osfXK_BackTab	      }, /* Defunct */
  { XmVosfBeginData,		osfXK_BeginData	      }, /* Defunct */
  { XmVosfBeginLine,		osfXK_BeginLine	      },
  { XmVosfCancel,		osfXK_Cancel	      },
  { XmVosfClear,		osfXK_Clear	      },
  { XmVosfCopy,			osfXK_Copy	      },
  { XmVosfCut,			osfXK_Cut	      },
  { XmVosfDelete,		osfXK_Delete	      },
  { XmVosfDeselectAll,		osfXK_DeselectAll     },
  { XmVosfDown,			osfXK_Down	      },
  { XmVosfEndData,		osfXK_EndData	      }, /* Defunct */
  { XmVosfEndLine,		osfXK_EndLine	      },
  { XmVosfEscape,		osfXK_Escape	      }, /* Defunct */
  { XmVosfExtend,		osfXK_Extend	      }, /* Defunct */
  { XmVosfHelp,			osfXK_Help	      },
  { XmVosfInsert,		osfXK_Insert	      },
  { XmVosfLeft,			osfXK_Left	      },
  { XmVosfLeftLine,		osfXK_LeftLine	      }, /* X11R6 */
  { XmVosfMenu,			osfXK_Menu	      },
  { XmVosfMenuBar,		osfXK_MenuBar	      },
  { XmVosfNext,			osfXK_Next	      }, /* X11R6 */
  { XmVosfNextField,		osfXK_NextField	      }, /* Defunct */
  { XmVosfNextMenu,		osfXK_NextMenu	      }, /* Defunct */
  { XmVosfNextMinor,		osfXK_NextMinor	      }, /* X11R6 */
  { XmVosfPageDown,		osfXK_PageDown	      },
  { XmVosfPageLeft,		osfXK_PageLeft	      },
  { XmVosfPageRight,		osfXK_PageRight	      },
  { XmVosfPageUp,		osfXK_PageUp	      },
  { XmVosfPaste,		osfXK_Paste	      },
  { XmVosfPrevField,		osfXK_PrevField	      }, /* Defunct */
  { XmVosfPrevMenu,		osfXK_PrevMenu	      }, /* Defunct */
  { XmVosfPrimaryPaste,		osfXK_PrimaryPaste    },
  { XmVosfPrior,		osfXK_Prior	      }, /* X11R6 */
  { XmVosfPriorMinor,		osfXK_PriorMinor      }, /* X11R6 */
  { XmVosfQuickPaste,		osfXK_QuickPaste      },
  { XmVosfReselect,		osfXK_Reselect	      },
  { XmVosfRestore,		osfXK_Restore	      },
  { XmVosfRight,		osfXK_Right	      },
  { XmVosfRightLine,		osfXK_RightLine	      }, /* X11R6 */
  { XmVosfSelect,		osfXK_Select	      },
  { XmVosfSelectAll,		osfXK_SelectAll	      },
  { XmVosfSwitchDirection,	osfXK_SwitchDirection }, /* X11R6 */
  { XmVosfUndo,			osfXK_Undo	      },
  { XmVosfUp,			osfXK_Up	      }
};

static XmConst XmDefaultBindingStringRec fallbackBindingStrings[] =
{
  { "Acorn Computers Ltd",
      _XmVirtKeys_acornFallbackBindingString },
  { "Apollo Computer Inc.",
      _XmVirtKeys_apolloFallbackBindingString },
  { "DECWINDOWS DigitalEquipmentCorp.",
      _XmVirtKeys_decFallbackBindingString },
  { "Data General Corporation  Rev 04",
      _XmVirtKeys_dgFallbackBindingString },
  { "Double Click Imaging, Inc. KeyX",
      _XmVirtKeys_dblclkFallbackBindingString },
  { "Hewlett-Packard Company",
      _XmVirtKeys_hpFallbackBindingString },
  { "International Business Machines",
      _XmVirtKeys_ibmFallbackBindingString },
  { "Intergraph Corporation",
      _XmVirtKeys_ingrFallbackBindingString },
  { "Megatek Corporation",
      _XmVirtKeys_megatekFallbackBindingString },
  { "Motorola Inc. (Microcomputer Division) ",
      _XmVirtKeys_motorolaFallbackBindingString },
  { "Silicon Graphics Inc.",
      _XmVirtKeys_sgiFallbackBindingString },
  { "Silicon Graphics",
      _XmVirtKeys_sgiFallbackBindingString },
  { "Siemens Munich by SP-4's Hacker Crew",
      _XmVirtKeys_siemensWx200FallbackBindingString },
  { "Siemens Munich (SP-4's hacker-clan)",
      _XmVirtKeys_siemens9733FallbackBindingString },
  { "Sun Microsystems, Inc.",
      _XmVirtKeys_sunFallbackBindingString },
  { "X11/NeWS - Sun Microsystems Inc.",
      _XmVirtKeys_sunFallbackBindingString },
  { "Tektronix, Inc.",
      _XmVirtKeys_tekFallbackBindingString }
};

/*ARGSUSED*/
static Boolean
CvtStringToVirtualBinding(Display    *dpy,
			  XrmValuePtr args,
			  Cardinal   *num_args,
			  XrmValuePtr fromVal,
			  XrmValuePtr toVal,
			  XtPointer  *closure_ret )
{
    char 	 *str = (char *)fromVal->addr;
    XKeyEvent	  event;
    int		  count, tmp;
    int		 *eventTypes;
    KeySym       *keysyms;
    unsigned int *modifiers;
    int		  j;
    int		  codes_per_sym;
    KeyCode	  minK;
    Modifiers	  used_mods;

    /* Lookup codes_per_sym, and let Xt cache the result instead of */
    /* always downloading a new copy with XGetKeyboardMapping(). */
    /* This also initializes Xt's per-display data structures, so */
    /* we can use XtTranslateKey() instead of XLookupString(). */
    (void) XtGetKeysymTable(dpy, &minK, &codes_per_sym);

    count = _XmMapKeyEvents(str, &eventTypes, &keysyms, &modifiers);
    if (count > 0)
      {
	Boolean fini;

	for (tmp = 0; tmp < count; tmp++)
	  {
	    fini = False;

	    /*
	     * Here's a nasty bit of code. If some vendor defines one of
	     * the standard modifiers to be the mode switch mod, the
	     * keysym returned by XtTranslateKey is the mode-shifted
	     * one. This may or may not be bogus, but we have to live
	     * with it :-(. Soo, we need to translate the keysym to a
	     * keycode, then ask someone to translate the combo for us.
	     */
	    event.display = dpy;
	    event.keycode = XKeysymToKeycode(dpy, keysyms[tmp]);

	    /*
	     * In case the guy specifies a symbol that is modified (like
	     * HP's Del which is <shift><escape>), we'll find the implied
	     * modifers and 'OR' it together with the explicitly stated
	     * modifers.
	     */
	    event.state = 0;
	    int keysyms_per_keycode=0;
	    int min_codes_per_sym = 0;
	    if (event.keycode > 0)
		{   KeySym *keysymTab = XGetKeyboardMapping(dpy, event.keycode, 1, &keysyms_per_keycode);
		    if ((keysymTab != NULL) && (keysyms_per_keycode > 0))
		      { if (keysyms_per_keycode > codes_per_sym)
			  min_codes_per_sym = codes_per_sym;
			else
			  min_codes_per_sym = keysyms_per_keycode;

			if (keysymTab[0] != keysyms[tmp])
			  for (j = 1; j < min_codes_per_sym; j++)
			    if (keysymTab[j] == keysyms[tmp])
				{
                  /*
		   * Gross Hack for Hobo keyboard ..
		   * Assumptions:
		   * 	1. Hobo keyboard has XK_Return  as the first entry
		   *		and XK_KP_Enter as the 4th entry in its
		   *            keycode to keysym key map.
		   *	2. This fix is only designed to work for the precise
		   *            combination of the Sun server with vendor
		   *            string "Sun Microsystems, Inc." and the Hobo
		   *            keyboard, as the fix assumes knowledge of the
		   *            server keycode to keysym key map.
		   */

				  if ((keysyms[tmp] == XK_KP_Enter) &&
				      (j == 4) &&
				      (keysymTab[0] == XK_Return) &&
				      (strcmp("Sun Microsystems, Inc.", ServerVendor(dpy)) == 0))
					{
					  fini = True;
					}
				  else
				    event.state = 1 << (j-1);

				  break;
				}
			XFree(keysymTab);
		      }
	      }

	    if (!fini) {
		event.state |= modifiers[tmp];
		XtTranslateKey(dpy, event.keycode, event.state, &used_mods,
			       keysyms + tmp);
	    }
	  }

	/* Fail if insufficient storage was provided. */
	if ((toVal->addr != NULL) &&
	    (toVal->size < sizeof(XmKeyBindingRec) * count))
	  {
	    toVal->size = sizeof(XmKeyBindingRec) * count;

	    XtFree((char*) eventTypes);
	    XtFree((char*) keysyms);
	    XtFree((char*) modifiers);
	    return False;
	  }

	/* Allocate storage if none was provided. */
	toVal->size = sizeof(XmKeyBindingRec) * count;
	if (toVal->addr == NULL)
	  toVal->addr = XtMalloc(toVal->size);

	/* Copy the data. */
	for (tmp = 0; tmp < count; tmp++)
	  {
	    ((XmKeyBinding) toVal->addr)[tmp].keysym = keysyms[tmp];
	    ((XmKeyBinding) toVal->addr)[tmp].modifiers = modifiers[tmp];
	  }

	XtFree((char*) eventTypes);
	XtFree((char*) keysyms);
	XtFree((char*) modifiers);
	return True;
      }

    /* The value supplied could not be converted. */
    XtDisplayStringConversionWarning(dpy, str, XmRVirtualBinding);
    return False;
}

static void
FillBindingsFromDB(Display       *dpy,
		   XrmDatabase    rdb,
		   XmVKeyBinding *keys,
		   Cardinal      *num_keys)
{
  XrmName 	    xrm_name[2];
  XrmClass 	    xrm_class[2];
  XrmRepresentation rep_type;
  XrmValue 	    value;
  Cardinal	    vk_num;
  XrmQuark	    XmQVirtualBinding = XrmPermStringToQuark(XmRVirtualBinding);
  XrmQuark	    XmQString         = XrmPermStringToQuark(XmRString);

  xrm_class[0] = XmQVirtualBinding;
  xrm_class[1] = 0;

  /* Previously entries in display.bindings corresponded exactly to */
  /* entries in virtualKeysyms.  Since multiple keysyms can be bound */
  /* to a single virtual key, display.num_bindings and the virtkey */
  /* field in XmVKeyBindingRec were added.  This also allows us to */
  /* remove null bindings from the table, improving search times. */

  /* Allocate the initial arrays. */
  *num_keys = 0;
  *keys = NULL;

  /* Process each known virtual key. */
  for (vk_num = 0; vk_num < XtNumber(virtualKeysyms); vk_num++)
    {
      Boolean	   free_keys = False;
      XmKeyBinding new_keys = NULL;
      Cardinal	   new_num = 0;

      xrm_name[0] = XrmPermStringToQuark(virtualKeysyms[vk_num].name);
      xrm_name[1] = 0;
      if (XrmQGetResource(rdb, xrm_name, xrm_class, &rep_type, &value))
	{
	  if (rep_type == XmQVirtualBinding)
	    {
	      /* Resource already exists in the desired format? */
	      new_keys = (XmKeyBinding) value.addr;
	      new_num = value.size / sizeof(XmKeyBindingRec);
	    }

	  else if (rep_type == XmQString)
	    {
	      /* Convert String resource to key bindings. */
	      XrmValue toVal;
	      toVal.addr = NULL;
	      toVal.size = 0;
	      if (XtCallConverter(dpy, CvtStringToVirtualBinding,
				  NULL, 0, &value, &toVal, NULL))
		{
		  new_keys = (XmKeyBinding) toVal.addr;
		  new_num = toVal.size / sizeof(XmKeyBindingRec);
		  free_keys = True;
		}
	    }

	  /* Append the new bindings to the end of the table. */
	  if (new_num > 0)
	    {
	      int tmp;

	      *keys = (XmVKeyBinding)
		XtRealloc((char*) *keys,
			  (*num_keys + new_num) * sizeof(XmVKeyBindingRec));

	      for (tmp = 0; tmp < new_num; tmp++)
		{
		  (*keys)[*num_keys + tmp].keysym = new_keys[tmp].keysym;
		  (*keys)[*num_keys + tmp].modifiers = new_keys[tmp].modifiers;
		  (*keys)[*num_keys + tmp].virtkey =
		    virtualKeysyms[vk_num].keysym;
		}

	      *num_keys += new_num;
	    }

	  if (free_keys)
	    XtFree((char *)new_keys);
	}
    }
}

static Boolean
GetBindingsProperty(Display *display,
		    String   property,
		    String  *binding)
{
  char		*prop = NULL;
  Atom		actual_type;
  int		actual_format;
  unsigned long	num_items;
  unsigned long	bytes_after;


  if ( binding == NULL )
    return False;

  XGetWindowProperty (display,
		      RootWindow(display, 0),
		      XInternAtom(display, property, FALSE),
		      0, (long)1000000,
		      FALSE, XA_STRING,
		      &actual_type, &actual_format,
		      &num_items, &bytes_after,
		      (unsigned char **) &prop);

  if ((actual_type != XA_STRING) ||
      (actual_format != 8) ||
      (num_items == 0))
    {
      if (prop != NULL)
	XFree(prop);
      return False;
    }
  else
    {
      *binding = prop;
      return True;
    }
}

/*
 * This routine is called by the XmDisplay Initialize method to set
 * up the virtual bindings table, XtKeyProc, and event handler.
 */
void
_XmVirtKeysInitialize(Widget widget)
{
  XmDisplay xmDisplay = (XmDisplay) widget;
  Display *dpy = XtDisplay(xmDisplay);
  XrmDatabase keyDB;
  String bindingsString;
  String fallbackString = NULL;
  Boolean needXFree = False;

  if (!XmIsDisplay(widget))
    return;

  bindingsString = xmDisplay->display.bindingsString;
  xmDisplay->display.lastKeyEvent = NULL;

  if (bindingsString == NULL)
    {
      /* XmNdefaultVirtualBindings not set, try _MOTIF_BINDINGS */
      if (GetBindingsProperty(XtDisplay(xmDisplay),
			      XmS_MOTIF_BINDINGS,
			      &bindingsString) == True)
	{
	  needXFree = True;
	}
      else if (GetBindingsProperty(XtDisplay(xmDisplay),
				   XmS_MOTIF_DEFAULT_BINDINGS,
				   &bindingsString) == True)
	{
	  needXFree = True;
	}
      else
	{
	  /* property not set, find a useful fallback */
	  _XmVirtKeysLoadFallbackBindings(XtDisplay(xmDisplay),
					  &fallbackString);
	  bindingsString = fallbackString;
	}
    }

  XtSetTypeConverter(XmRString, XmRVirtualBinding, CvtStringToVirtualBinding,
		     NULL, 0, XtCacheNone, (XtDestructor)NULL);
  keyDB = XrmGetStringDatabase( bindingsString );
  FillBindingsFromDB (XtDisplay(xmDisplay), keyDB,
		      &xmDisplay->display.bindings,
		      &xmDisplay->display.num_bindings);

  XrmDestroyDatabase(keyDB);
  if (needXFree)
    XFree (bindingsString);
  if (fallbackString)
    XtFree (fallbackString);

  XtSetKeyTranslator(dpy, (XtKeyProc)XmTranslateKey);
}

/*
 * This routine is called by the XmDisplay Destroy method to free
 * up the virtual bindings table.
 */
void
_XmVirtKeysDestroy(Widget widget)
{
  XmDisplay xmDisplay = (XmDisplay) widget;

  XtFree((char*)xmDisplay->display.lastKeyEvent);
  XtFree((char*)xmDisplay->display.bindings);
}

static void
FindVirtKey(Display *dpy,
	    KeyCode keycode,
	    Modifiers modifiers,
	    Modifiers *modifiers_return,
	    KeySym *keysym_return )
{
  Cardinal      i;
  XmDisplay     xmDisplay = (XmDisplay) XmGetXmDisplay( dpy);
  XmVKeyBinding keyBindings = xmDisplay->display.bindings;
  KeyCode       min_kcode;
  int           ks_per_kc;
  KeySym       *ks_table = XtGetKeysymTable( dpy, &min_kcode, &ks_per_kc);
  KeySym       *kc_map = &ks_table[(keycode - min_kcode) * ks_per_kc];
  Modifiers     EffectiveSMMask = EffectiveStdModMask( dpy, kc_map, ks_per_kc);

  /* Get the modifiers from the actual event */
  Modifiers VirtualStdMods = 0;
  Modifiers StdModMask;

  for (i = 0; i < xmDisplay->display.num_bindings; i++)
    {
      unsigned j = ks_per_kc;
      KeySym vks = keyBindings[i].keysym;

      if (vks)
	{
	  while (j--)
	    {
	      /* Want to walk through keymap (containing all possible
	       * keysyms generated by this keycode) to compare against
	       * virtual key keysyms.  Any keycode that can possibly
	       * generate a virtual keysym must be sure to return all
	       * modifiers that are in the virtual key binding, since
	       * this means that those modifiers are now part of the
	       * "standard modifiers" for this keycode.  (A "standard
	       * modifier" is a modifier that can affect which keysym
	       * is generated from a particular keycode.)
	       */
	      if ((j == 1)  &&  (kc_map[j] == NoSymbol))
		{
		  KeySym uc, lc;

		  XtConvertCase( dpy, kc_map[0], &lc, &uc);
		  if ((vks == lc)  ||  (vks == uc))
		    VirtualStdMods |= keyBindings[i].modifiers;
		  break;
		}
	      else if (vks == kc_map[j])
		{
		  /* The keysym generated by this keycode can possibly
		   * be influenced by the virtual key modifier(s), so must
		   * add the modifier(s) associated with this virtual
		   * key to the returned list of "standard modifiers".
		   * The Intrinsics requires that the set of modifiers
		   * returned by the keyproc is constant for a given
		   * keycode.
		   */
		  VirtualStdMods |= keyBindings[i].modifiers;
		  break;
		}
	    }
	}
    }

  /* Don't want to return standard modifiers that do not
   * impact the keysym selected for a particular keycode,
   * since this blocks matching of translation productions
   * which use ":" style translations with the returned
   * standard modifier in the production.  The ":" style
   * of production is essential for proper matching of
   * Motif translations (PC numeric pad, for instance).
   *
   * Recent fixes to the Intrinsics should have included this
   * change to the set of standard modifiers returned from
   * the default key translation routine, but could not be
   * done for reasons of backwards compatibility (which is
   * not an issue for Xm, since we do not export this facility).
   * So, we do the extra masking here after the return from
   * the call to XtTranslateKey.
   */
  *modifiers_return &= EffectiveSMMask;

  /* Modifiers present in the virtual binding table for the
   * keysyms associated with this keycode, which are or might
   * have been used to change the keysym generated by this
   * keycode (to a virtual keysym), must be included in the
   * returned set of standard modifiers.  Remember that "standard
   * modifiers" for a keycode are those modifiers that can affect
   * which keysym is generated by that keycode.
   */
  *modifiers_return |= VirtualStdMods;

  /* Effective standard modifiers that are set in the event
   * will be 0 in the following bit mask, which will be used
   * to collapse conflicting modifiers in the virtual
   * key binding table, as described below.
   */
  StdModMask = ~(modifiers & EffectiveSMMask);

  for (i = 0; i < xmDisplay->display.num_bindings; i++)
    {
      XmVKeyBinding currBinding = &keyBindings[i];
      KeySym vks = currBinding->keysym;

      /* The null binding should not be interpreted as a match
       * keysym is zero (e.g. pre-edit terminator)
       */
      /* Those modifiers that are effective standard modifiers and
       * that are set in the event will be ignored in the following
       * conditional (i.e., the state designated in the virtual key
       * binding will not be considered), since these modifiers have
       * already had their affect in the determination of the value
       * of *keysym_return.  This allows matching that is consistent
       * with industry-standard interpretation for keys such as
       * those of the PC-style numeric pad.  This apparent loss of
       * binding semantics is an unavoidable consequence of specifying
       * a modifier in the virtual binding table that is already being
       * used to select one of several keysyms associated with a
       * particular keycode (usually as printed on the keycap).
       * The disambiguation of the collapsing of key bindings
       * is based on "first match" in the virtual key binding table.
       */
      if (vks && (vks == *keysym_return) &&
	  ((currBinding->modifiers & StdModMask) ==
	   (modifiers & VirtualStdMods & StdModMask)))
	{
	  *keysym_return = currBinding->virtkey;
	  break;
	}
    }
}

static Modifiers
EffectiveStdModMask(Display *dpy,
		    KeySym *kc_map,
		    int ks_per_kc)
{
  /* This routine determines which set of modifiers can possibly
   * impact the keysym that is generated by the keycode associated
   * with the keymap passed in.  The basis of the algorithm used
   * here is described in section 12.7 "Keyboard Encoding" of the
   * R5 "Xlib - C Language X Interface" specification.
   */
  KeySym uc;
  KeySym lc;

  /* Since the group modifier can be any of Mod1-Mod5 or Control, we will
   * return all of these bits if the group modifier is found to be effective.
   * Lock will always be returned (for backwards compatibility with
   * productions assuming that Lock is always a "don't care" modifier
   * for non-alphabetic keys).  Shift will be returned unless it has
   * no effect on the selection of keysyms within either group.
   */
  Modifiers esm_mask = (Mod5Mask | Mod4Mask | Mod3Mask | Mod2Mask | Mod1Mask |
			ControlMask | LockMask | ShiftMask);
  switch (ks_per_kc)
    {
    default:	/* CR 8799: Ignore non-standard modifier groups. */
    case 4:
      if (kc_map[3] != NoSymbol)
	{
	  /* The keysym in position 4 is selected when the group
	   * modifier is set and the Shift (or Lock) modifier is
	   * set, so both Shift/Lock and the group modifiers are
	   * all "effective" standard modifiers.
	   */
	  break;
	}
    case 3:
      if (kc_map[2] == NoSymbol)
	{
	  /* Both Group 2 keysyms are NoSymbol, so the group
	   * modifier has no effect; only Shift and Lock remain
	   * as possible effective modifiers.
	   */
	  esm_mask = ShiftMask | LockMask;
	}
      else
	{
	  XtConvertCase( dpy, kc_map[2], &lc, &uc);
	  if (lc != uc)
	    {
	      /* The Group 2 keysym is case-sensitive, so group
	       * modifiers and Shift/Lock modifiers are effective.
	       */
	      break;
	    }
	}
      /* At this fall-through, the group modifier bits have been
       * decided, while the case is still out on Shift/Lock.
       */
    case 2:
      if (kc_map[1] != NoSymbol)
	{
	  /* Shift/Lock modifier selects keysym from Group 1,
	   * so leave those bits in the mask.  The group modifier
	   * was determined above, so leave those bits in the mask.
	   */
	  break;
	}
    case 1:
      if (kc_map[0] != NoSymbol)
	{
	  XtConvertCase( dpy, kc_map[0], &lc, &uc);
	  if (lc != uc)
	    {
	      /* The Group 1 keysym is case-sensitive, so Shift/Lock
	       * modifiers are effective.
	       */
	      break;
	    }
	}
      /* If we did not break out of the switch before this, then
       * the Shift modifier is not effective; mask it out.
       */
      esm_mask &= ~ShiftMask;
    case 0:
      break;
    }

  return esm_mask;
}

void
XmTranslateKey(Display     *dpy,
	       KeyCode      keycode,
	       Modifiers    modifiers,
	       Modifiers   *modifiers_return,
	       KeySym      *keysym_return )
{
  _XmDisplayToAppContext(dpy);
  _XmAppLock(app);
  XtTranslateKey(dpy, keycode, modifiers, modifiers_return, keysym_return);

  FindVirtKey(dpy, keycode, modifiers, modifiers_return, keysym_return);
  _XmAppUnlock(app);
}

int
XmeVirtualToActualKeysyms(Display      *dpy,
			  KeySym        virtKeysym,
			  XmKeyBinding *actualKeyData)
{
  int           matches;
  Cardinal      index;
  XmDisplay     xmDisplay = (XmDisplay)XmGetXmDisplay (dpy);
  XmVKeyBinding keyBindings = xmDisplay->display.bindings;
  _XmDisplayToAppContext(dpy);

  _XmAppLock(app);
  /* Initialize the return parameters. */
  *actualKeyData = NULL;

  /* Count the number of matches. */
  matches = 0;
  for (index = 0; index < xmDisplay->display.num_bindings; index++)
    if (keyBindings[index].virtkey == virtKeysym)
      matches++;

  /* Allocate the return array. */
  if (matches > 0)
    {
      *actualKeyData = (XmKeyBinding)
	XtMalloc(matches * sizeof(XmKeyBindingRec));

      matches = 0;
      for (index = 0; index < xmDisplay->display.num_bindings; index++)
	if (keyBindings[index].virtkey == virtKeysym)
	  {
	    (*actualKeyData)[matches].keysym = keyBindings[index].keysym;
	    (*actualKeyData)[matches].modifiers = keyBindings[index].modifiers;
	    matches++;
	  }
    }

  _XmAppUnlock(app);
  return matches;
}

Boolean
_XmVirtKeysLoadFileBindings(char   *fileName,
			    String *binding )
{
  FILE *fileP;
  int buffersize;
  int count;
  int firsttime;
  char line[256];
  Boolean skip;

  if ((fileP = fopen (fileName, "r")) != NULL)
    {
      skip = False;
      count = 0;
      buffersize = 1;
      firsttime = 1;

      while (fgets(line, sizeof(line), fileP) != NULL) {

	/* handle '!' comments; they can extend across mutliple reads */
	if (skip) {
	  if (line[strlen(line) - 1] == '\n') skip = False;
	  continue;
	}
	if (line[0] == '!') {
	  if (line[strlen(line) - 1] == '\n') continue;
	  else {
	    skip = True;
	    continue;
	  }
	}

	/* must be >=, because buffersize is always 1 bigger for '\0' */
	if (count + strlen(line) >= buffersize) {
	  buffersize += BUFFERSIZE;
	  *binding = XtRealloc(*binding, buffersize);

	  /* always make sure that the end of *binding is null terminated */
	  if (firsttime) {
	    *binding[0] = '\0';
	    firsttime = 0;
	  }
	}

	count += strlen(line);

	strcat(*binding, line);
      }

      /* trim unused buffer space */
      *binding = XtRealloc (*binding, count + 1);

      fclose (fileP);
      return True;
    }

  return False;
}

static void
LoadVendorBindings(Display *display,
		   char    *path,
		   FILE    *fp,
		   String  *binding )
{
  char buffer[MAXLINE];
  char *bindFile;
  char *vendor;
  char *vendorV;
  char *ptr;
  char *start;

  vendor = ServerVendor(display);
  vendorV = XtMalloc (strlen(vendor) + 20); /* assume rel.# is < 19 digits */
  sprintf (vendorV, "%s %d", vendor, VendorRelease(display));

  while (fgets (buffer, MAXLINE, fp) != NULL)
    {
      ptr = buffer;
      while (*ptr != '"' && *ptr != '!' && *ptr != '\0')
	ptr++;
      if (*ptr != '"')
	continue;

      start = ++ptr;
      while (*ptr != '"' && *ptr != '\0')
	ptr++;
      if (*ptr != '"')
	continue;

      *ptr = '\0';
      if ((strcmp (start, vendor) == 0) || (strcmp (start, vendorV) == 0))
	{
	  ptr++;
	  while (isspace((unsigned char)*ptr) && *ptr)
	    ptr++;
	  if (*ptr == '\0')
	    continue;

	  start = ptr;
	  while (!isspace((unsigned char)*ptr) && *ptr != '\n' && *ptr)
	    ptr++;
	  *ptr = '\0';

	  bindFile = _XmOSBuildFileName (path, start);
	  if (_XmVirtKeysLoadFileBindings (bindFile, binding))
	    {
	      XtFree (bindFile);
	      break;
	    }
	  XtFree (bindFile);
	}
    }

  XtFree (vendorV);
}

int
_XmVirtKeysLoadFallbackBindings(Display	*display,
				String	*binding )
{
  enum { XmA_MOTIF_BINDINGS, XmA_MOTIF_DEFAULT_BINDINGS, NUM_ATOMS };
  static char *atom_names[] = {XmS_MOTIF_BINDINGS, XmS_MOTIF_DEFAULT_BINDINGS};

  XmConst XmDefaultBindingStringRec *currDefault;
  int i;
  FILE *fp;
  char *homeDir;
  char *fileName;
  char *bindDir;
  static XmConst char xmbinddir_fallback[] = XMBINDDIR_FALLBACK;
  Atom atoms[XtNumber(atom_names)];

  *binding = NULL;

  assert(XtNumber(atom_names) == NUM_ATOMS);
  XInternAtoms(display, atom_names, XtNumber(atom_names), False, atoms);

  /* Load .motifbind - necessary, if mwm and xmbind are not used */
  homeDir = XmeGetHomeDirName();
  fileName = _XmOSBuildFileName(homeDir, MOTIFBIND);
  _XmVirtKeysLoadFileBindings(fileName, binding);
  XtFree(fileName);

  /* Look for a match in the user's xmbind.alias */
  if (*binding == NULL)
    {
      fileName = _XmOSBuildFileName (homeDir, XMBINDFILE);
      if ((fp = fopen (fileName, "r")) != NULL)
	{
	  LoadVendorBindings (display, homeDir, fp, binding);
	  fclose (fp);
	}
      XtFree (fileName);
    }

  if (*binding != NULL)
    {
      /* Set the user property for future Xm applications. */
      XChangeProperty (display, RootWindow(display, 0),
		       atoms[XmA_MOTIF_BINDINGS],
		       XA_STRING, 8, PropModeReplace,
		       (unsigned char *)*binding, strlen(*binding));
      return 0;
    }

  /* Look for a match in the system xmbind.alias */
  if (*binding == NULL)
    {
      if ((bindDir = getenv(XMBINDDIR)) == NULL)
	bindDir = (char*) xmbinddir_fallback;
      fileName = _XmOSBuildFileName (bindDir, XMBINDFILE);
      if ((fp = fopen (fileName, "r")) != NULL)
	{
	  LoadVendorBindings (display, bindDir, fp, binding);
	  fclose (fp);
	}
      XtFree (fileName);
    }

  /* Check hardcoded fallbacks (for 1.1 bc) */
  if (*binding == NULL)
    {
      for (i = 0, currDefault = fallbackBindingStrings;
	   i < XtNumber(fallbackBindingStrings);
	   i++, currDefault++)
	{
	  if (strcmp(currDefault->vendorName, ServerVendor(display)) == 0)
	    {
	      *binding = XtMalloc (strlen (currDefault->defaults) + 1);
	      strcpy (*binding, currDefault->defaults);
	      break;
	    }
	}
    }

  /* Use generic fallback bindings */
  if (*binding == NULL)
    {
      *binding = XtMalloc (strlen (defaultFallbackBindings) + 1);
      strcpy (*binding, defaultFallbackBindings);
    }

  /* Set the fallback property for future Xm applications */
  XChangeProperty (display, RootWindow(display, 0),
		   atoms[XmA_MOTIF_DEFAULT_BINDINGS],
		   XA_STRING, 8, PropModeReplace,
		   (unsigned char *)*binding, strlen(*binding));

  return 0;
}
