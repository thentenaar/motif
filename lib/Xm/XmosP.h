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
#ifndef _XmosP_h
#define _XmosP_h

/* Some SVR4 systems don't have bzero. */
#include <X11/Xfuncs.h>

/*
 * Fix for 8975 - using LOGNAME instead of USER on SYSV and SVR4
*/

#ifndef USER_VAR
#if defined(SYSV) || defined(SVR4)
#define USER_VAR "LOGNAME"
#else
#define USER_VAR "USER"
#endif
#endif

#include <stdlib.h> /* Needed for MB_CUR_MAX, mbtowc, mbstowcs and mblen */

/* On Sun systems, mblen is broken. It doesn't return 0 when the
   string is empty. Here's a patch. NOTE: On Sun systems, mblen
   is a macro wrapper around mbtowc. Hence the implementation below. */
#if defined(sun)
#undef  mblen
#define mblen(ptr, size) \
  ((ptr && *(ptr) == '\0') ? 0 : mbtowc((wchar_t *)0, (ptr), (size)))
#endif

#include <limits.h>		/* for MB_LEN_MAX et al */

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif
#ifndef LONG_MAX
#define LONG_MAX 2147483647
#endif
#ifdef BOGUS_MB_MAX  /* some systems don't properly set MB_[CUR|LEN]_MAX */
#undef MB_LEN_MAX
#define MB_LEN_MAX 1 /* temp fix for ultrix */
#undef MB_CUR_MAX
#define MB_CUR_MAX 1 /* temp fix for ultrix */
#endif /* BOGUS_MB_MAX */

/**********************************************************************/
/* here we duplicate Xtos.h, since we can't include this private file */

#if defined(INCLUDE_ALLOCA_H) || defined(HAVE_ALLOCA_H)
#include <alloca.h>
#endif

#ifdef CRAY
#define WORD64
#endif

/* Don't Use Alloca On Solaris */
#if defined(sun)
#define NO_ALLOCA
#endif

/* stolen from server/include/os.h */
#ifndef NO_ALLOCA
/*
 * os-dependent definition of local allocation and deallocation
 * If you want something other than XtMalloc/XtFree for ALLOCATE/DEALLOCATE
 * LOCAL then you add that in here.
 */
#if defined(__HIGHC__)

#if HCVERSION < 21003
#define ALLOCATE_LOCAL(size)	alloca(size)
pragma on(alloca);
#else /* HCVERSION >= 21003 */
#define	ALLOCATE_LOCAL(size)	_Alloca(size)
#endif /* HCVERSION < 21003 */

#define DEALLOCATE_LOCAL(ptr)  /* as nothing */

#endif /* defined(__HIGHC__) */


#ifdef __GNUC__

#ifndef alloca /* gnu itself might have done that already */
#define alloca __builtin_alloca
#endif

#define ALLOCATE_LOCAL(size) alloca(size)
#define DEALLOCATE_LOCAL(ptr)  /* as nothing */
#else /* ! __GNUC__ */
/*
 * warning: mips alloca is unsuitable, do not use.
 */
#if defined(vax) || defined(sun) || defined(apollo) || defined(stellar)
/*
 * Some System V boxes extract alloca.o from libPW.a; if you
 * decide that you don't want to use alloca, you might want to fix it here.
 */
char *alloca();
#define ALLOCATE_LOCAL(size) alloca(size)
#define DEALLOCATE_LOCAL(ptr)  /* as nothing */
#endif /* who does alloca */
#endif /* __GNUC__ */

#endif /* NO_ALLOCA */

#ifndef ALLOCATE_LOCAL
#define ALLOCATE_LOCAL(size)	XtMalloc(size)
#define DEALLOCATE_LOCAL(ptr)	XtFree(ptr)
#endif /* ALLOCATE_LOCAL */

/* End of Xtos.h */
/*****************/

#include <Xm/XmP.h>

/* For padding structures in Mrm we need to know how big pointers are. */
#if !defined(CRAY) && !defined(__alpha)
#define MrmShortPtr
#endif

#ifdef __cplusplus
extern "C" {
#endif


#define MATCH_CHAR 'P'  /* referenced in InitPath strings and in the files
			 that uses it (ImageCache.c and Mrmhier.c) */

/* OS-dependent file info for VirtKeys */

#define XMBINDDIR "XMBINDDIR"
#ifndef XMBINDDIR_FALLBACK
#define XMBINDDIR_FALLBACK "/usr/lib/Xm/bindings"
#endif
#define XMBINDFILE "xmbind.alias"
#define MOTIFBIND ".motifbind"

#ifdef __cplusplus
}
#endif
#endif /* _XmosP_h */

