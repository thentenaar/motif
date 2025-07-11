/* $XConsortium: import.h /main/4 1995/07/15 21:01:29 drk $ */
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
/*
 * HISTORY
 */


/*
 * Set up the macros that control compiling just the parts of the source
 * wanted for the host and target libraries.
 *
 * Any given snatch of the source may be wanted in one or more of the
 * following places:
 *	+ the unshared library
 *	+ the target shared library
 *	+ the host shared library
 */

/* All code in affected modules should be bracketed by one or another of these*/

#ifdef MAKE_DATA_SHLIB
#  define MAKE_DATA_SHLIB 1
#else
#  undef  MAKE_DATA_SHLIB
#endif

#ifdef MAKE_TEXT_SHLIB
#  define MAKE_TEXT_SHLIB 1
#else
#  undef  MAKE_TEXT_SHLIB
#endif

#if MAKE_DATA_SHLIB
#  undef  sgi_TEXT_ONLY
#  define sgi_DATA_ONLY 1
#  undef  sgi_UNSHARED_ONLY
#  undef  sgi_TEXT_OR_UNSHARED
#  define sgi_DATA_OR_UNSHARED 1
#  define sgi_DATA_OR_TEXT 1
#  define sgi_SHARED_INITIALIZER(type, val) (type)__sgi_xmUninitialized
extern void __sgi_xmUninitialized();
#  define sgi_SHARED_INIT(adr, val) THIS SHOULDNT OCCUR
#  define SHARED_STATIC extern
/* Enable the commented-out bad systax once we get text/data separation */
#  define CALL_STAT /* STAT SHOULDN'T BE GETTING CALLED FROM THE DATA OBJECT */

#else /*MAKE_TEXT_SHLIB*/
#  if MAKE_TEXT_SHLIB
#    define sgi_TEXT_ONLY 1
#    undef  sgi_DATA_ONLY
#    undef  sgi_UNSHARED_ONLY
#    define sgi_TEXT_OR_UNSHARED 1
#    undef  sgi_DATA_OR_UNSHARED
#    define sgi_DATA_OR_TEXT 1
#    define sgi_SHARED_INITIALIZER(type, val) (type)__sgi_xmUninitialized
extern void __sgi_xmUninitialized();
#    define sgi_SHARED_INIT(adr, val) \
{if (__sgi_xmUninitialized!=(XtProc)(adr)) __sgi_xmUninitialized(); (adr)=(val);}
#    define SHARED_STATIC extern
#    define CALL_STAT (*__shlibXm_stat)
extern int      (*__shlibXm_stat)(const char *, struct stat *);
extern int __shlibXmVersion;

#  else
#    undef  sgi_TEXT_ONLY
#    undef  sgi_DATA_ONLY
#    define sgi_UNSHARED_ONLY 1
#    define sgi_TEXT_OR_UNSHARED 1
#    define sgi_DATA_OR_UNSHARED 1
#    undef  sgi_DATA_OR_TEXT
#    define sgi_SHARED_INITIALIZER(type, val) val
#    define sgi_SHARED_INIT(adr, val) THIS WHOLE ROUTINE SHOULD BE ifdefd OUT
#    define SHARED_STATIC static
#    define CALL_STAT stat
#  endif
#endif

#define sgi_SUPERCLASS(val) sgi_SHARED_INITIALIZER(WidgetClass,(WidgetClass)val)
#define sgi_CHILDCLASS(val) sgi_SHARED_INITIALIZER(WidgetClass,(WidgetClass)val)
#define sgi_SECONDARYCLASS(val) sgi_SHARED_INITIALIZER(WidgetClass,(WidgetClass)val)
#define sgi_SUPERCLASS_INIT(struct,val) sgi_SHARED_INIT(struct.core_class.superclass,(WidgetClass)val)
#define sgi_SUPERCLASS_GADGET_INIT(struct,val) sgi_SHARED_INIT(struct.rect_class.superclass,(WidgetClass)val)
#define sgi_SUPERCLASS_OBJECT_INIT(struct,val) sgi_SHARED_INIT(struct.object_class.superclass,(WidgetClass)val)
#define sgi_CHILDCLASS_INIT(struct,val) sgi_SHARED_INIT(struct.desktop_class.child_class,(WidgetClass)val)
#define sgi_SECONDARYCLASS_INIT(struct,val) sgi_SHARED_INIT(struct.secondaryObjectClass,(WidgetClass)val)

/* DEBUG -- delete this VVVVV */
/* "MAKE_DATA_SHLIB" MAKE_DATA_SHLIB
/* "MAKE_TEXT_SHLIB" MAKE_TEXT_SHLIB
/* "sgi_TEXT_ONLY" sgi_TEXT_ONLY
/* "sgi_DATA_ONLY" sgi_DATA_ONLY
/* "sgi_UNSHARED_ONLY" sgi_UNSHARED_ONLY
/* "sgi_TEXT_OR_UNSHARED" sgi_TEXT_OR_UNSHARED
/* "sgi_DATA_OR_UNSHARED" sgi_DATA_OR_UNSHARED
/* "sgi_DATA_OR_TEXT" sgi_DATA_OR_TEXT
/* DEBUG -- delete this ^^^^^ */

#if defined ATTSHAREDLIB && MAKE_TEXT_SHLIB

/* Set up the definitions that will reference the jump table */
/* We include them from a separately generated file to shorten the
 * make process.  All .c files include import.h, so any change to this
 * file will require recompiling all .c's into .o's (which are
 * unshared).  By putting in the include of import.def.h under the
 * above ifdef, make depend will not recognize that the .o's depend on
 * import.def.h.  Since we want to force recompilation of the .to's,
 * the Makefile should explicitly make the .to's depend on import.def.h.
 */
#include "import.def.h"

/* import.def.h #define'd stat.  We need to undef it as this is both a
 * library entry point and a structure declared in stat.h.  to call stat,
 * use the call stat macro above.
 */
#undef stat

/* /* Hand additions */

/*
 * UGLY -- Xt non-global routine from mit/lib/Xt/Converters.c
 * Not even other things in Xt use it.
 *
 * ResConvert.c and VendorE.c need it to build the shared library.
 */
 	extern void LowerCase(char *source, char *dest);

/*
 * regex is not in an include file so define it here
 */

	extern char *regex(char *re, char *subject);

 

#endif /*ATTSHAREDLIB*/
