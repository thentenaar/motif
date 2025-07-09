dnl Process this file with autoconf to produce a configure script.
AC_INIT
AC_CONFIG_SRCDIR([lib/Xm/Form.c])
AC_PREREQ(2.52)
AC_CONFIG_AUX_DIR(.)
AC_CHECK_FILE(/usr/X/include/X11/X.h,
  AC_PREFIX_DEFAULT(/usr/X),
  AC_PREFIX_DEFAULT(/usr))
AC_CHECK_FILE(/usr/X11R6/include/X11/X.h,
  AC_PREFIX_DEFAULT(/usr/X11R6),
  AC_PREFIX_DEFAULT(/usr))

dnl AM_MAINTAINER_MODE
AC_CANONICAL_TARGET

dnl The version of the Motif libraries
CURRENT=4
REVISION=4
AGE=0
AC_SUBST(CURRENT)
AC_SUBST(REVISION)
AC_SUBST(AGE)

dnl This is really dumb but it seems to be bug
AM_INIT_AUTOMAKE(motif,2.3.8,no-define)

LIBTOOL_VERSION=$CURRENT:$REVISION:$AGE

dnl
dnl Override a default in libtool.
dnl
dnl AC_DISABLE_STATIC

AC_CONFIG_HEADERS(include/config.h lib/Xm/Xm.h)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CPP
AM_PROG_LIBTOOL

AC_EXEEXT

dnl AC_PROG_YACC
dnl Do this the old fashioned way. 'bison -y' doesn't cut it
AC_CHECK_PROGS(YACC, byacc, yacc)

dnl AC_PROG_SPLINT
AC_CHECK_PROGS(SPLINT, splint)

AM_PROG_LEX
if test "x$LEX" == xflex; then
  AC_SUBST([LEXLIB], ['-lfl'])
fi

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET


dnl Checks for libraries.

AC_PATH_X
AC_PATH_XTRA
LT_LIB_XTHREADS

dnl Checks for Xos_r.h

AC_CHECK_HEADERS([X11/Xos_r.h])
AC_CHECK_HEADERS([X11/Xpoll.h])

dnl Compile in EditRes support if we can find Xmu

save_LIBS="$LIBS"
save_CFLAGS="$CFLAGS"
LIBS="$X_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
AC_CHECK_HEADERS(X11/Xmu/Editres.h,
AC_CHECK_LIB(Xmu, _XEditResCheckMessages,
X_XMU=-lXmu
AC_DEFINE(HAVE_LIBXMU, 1, This System has libXmu)
, X_XMU="", -lXt -lX11)
AC_CHECK_LIB(Xmu,XmuNCopyISOLatin1Lowered ,
AC_DEFINE(HAVE_XMU_N_COPY_ISO,1,Have XmuNCopyISOLatin1Lowered)
, , -lXt -lX11)
,,[[#include <X11/Intrinsic.h>]]
)


AC_SUBST(X_XMU)
LIBS="$save_LIBS"
CFLAGS="$save_CFLAGS"
AM_CONDITIONAL(OM_XMU, test x$X_XMU != x)

dnl Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS(fcntl.h limits.h malloc.h sys/malloc.h strings.h sys/file.h sys/time.h unistd.h wchar.h)
AC_CHECK_HEADERS(wctype.h, AC_DEFINE(HAS_WIDECHAR_FUNCTIONS,1,System supports wchar))
AC_CHECK_HEADER(langinfo.h,,AC_DEFINE(CSRG_BASED,1,System Has langinfo.h))
AC_CHECK_HEADER(X11/Xos_r.h,,AC_DEFINE(NEED_XOS_R_H,1,System Missing Xos_r.h))
AC_CHECK_HEADER(X11/Xpoll.h,,AC_DEFINE(NEED_XPOLL_H,1,system Missing X11/Xpoll.h))
dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_INTPTR_T
AC_TYPE_UINTPTR_T
AC_HEADER_TIME
AC_STRUCT_TM
AC_TYPE_UID_T
WEAK_ALIAS_SUPPORT

dnl Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_MEMCMP
AC_FUNC_SETPGRP
AC_TYPE_SIGNAL
AC_FUNC_STRCOLL
AC_FUNC_STRFTIME
AC_FUNC_FORK([])
AC_FUNC_VPRINTF
AM_FUNC_VOID_SPRINTF
AC_CHECK_FUNCS(wcslen wcscpy wcsncpy wcschr wcscat wcsncat getcwd gettimeofday mkdir re_comp regcmp select strcspn strerror strstr strtod strtol uname)
AC_CHECK_FUNCS(strdup,,AC_DEFINE(NEED_STRDUP, 1, no strdup))
AC_CHECK_FUNCS(strcasecmp,,AC_DEFINE(NEED_STRCASECMP, 1, no strcasecmp))
AC_CHECK_FUNCS(putenv,,AC_DEFINE(NO_PUTENV, 1, No PUTENV))
AC_CHECK_FUNCS(regcomp,,AC_DEFINE(NO_REGCOMP, 1, no regcmp))
AC_CHECK_FUNCS(memmove,,AC_DEFINE(NO_MEMMOVE, 1, no memmove))
AC_CHECK_TYPE(XICProc,,AC_DEFINE(NO_XICPROC, 1, XICProc isn't defined), [#include <X11/Xlib.h>])

AC_ARG_ENABLE(message-catalog, [  --enable-message-catalog
                          Enable building of the message catalog (default=no)])
if test "$enable_message_catalog" != "yes"
then
    AC_DEFINE(NO_MESSAGE_CATALOG, 1, No Defined Message Catalog)
fi
AM_CONDITIONAL(MessageCatalog, test x$enable_message_catalog = xyes)

AC_ARG_ENABLE(themes, [  --enable-themes         Enable themes (default=no)])
if test "$enable_themes" = "yes"
then
    if test -f $srcdir/lib/Xm/Theme.h
    then
	AC_DEFINE(USE_XMTHEMES, 1, "Use XmThemes")
	XMTHEME_DIST = "Theme.h ThemeP.h DynLibP.h Theme.c DynLib.c"
    else
	AC_MSG_WARN([--enable-themes specified, but I don't have the code. Disabled])
	enable_themes="no"
    fi
fi
AC_SUBST(XMTHEME_DIST)
AM_CONDITIONAL(OM_XMTHEMES, test "$enable_themes" = "yes")

AC_ARG_ENABLE(debug-themes, [  --enable-debug-themes   Enable themes debugging (default=no)])
if test "$enable_themes" = "yes"
then
    if test "$enable_debug_themes" = "yes"
    then
	AC_DEFINE(DEBUGTHEMES, 1, "Debug Themes")
    fi
fi

if test x$GCC = xyes
then
    CFLAGS="$CFLAGS -Wall -fno-strict-aliasing -Wno-unused -Wno-comment"
	CFLAGS="$CFLAGS -Wno-unused-result -Wno-misleading-indentation"
	CFLAGS="$CFLAGS -Wno-deprecated-declarations -Wno-implicit-fallthrough"
	CFLAGS="$CFLAGS -Wno-int-in-bool-context"
fi
AC_DEFINE(NO_OL_COMPAT, 1, "No OL Compatability")

CDE_INSTALLATION_TOP="\${prefix}/dt"
AC_SUBST(CDE_INSTALLATION_TOP)

CDE_CONFIGURATION_TOP="\${prefix}/etc/dt"
AC_SUBST(CDE_CONFIGURATION_TOP)

LIBDIR="${libdir}/X11"
AC_SUBST(LIBDIR)

MWMRCDIR="${sysconfdir}/X11"
AC_SUBST(MWMRCDIR)

INCDIR="${includedir}/X11"
AC_SUBST(INCDIR)

XMBINDDIR_FALLBACK="${datadir}/X11/bindings"
AC_SUBST(XMBINDDIR_FALLBACK)

RM="rm -f"
AC_SUBST(RM)

AC_ARG_ENABLE(motif22-compatibility, [  --disable-motif22-compatibility
                       Disable binary compatibility with OpenMotif 2.2])
AC_ARG_ENABLE(utf8, [  --disable-utf8
                       Disable UTF-8 support])

case "$host_os" in
    freebsd*) CFLAGS="$CFLAGS -DCSRG_BASED -DXNO_MTSAFE_API -DXNO_MTSAFE_PWDAPI" ;;
    cygwin*) CFLAGS="$CFLAGS -DXNO_MTSAFE_DIRENTDAPI";;
esac

if test x$enable_motif22_compatibility = x
then
    enable_motif22_compatibility="yes"
fi

if test "$enable_motif22_compatibility" = "yes"
then
  AC_DEFINE([OM22_COMPATIBILITY], [1], [Compatibility with OpenMotif 2.2])
  OM22_COMPATIBILITY_FALSE=!
  OM22_COMPATIBILITY_TRUE=
else
  OM22_COMPATIBILITY_FALSE=
  OM22_COMPATIBILITY_TRUE=!
fi
AC_SUBST(OM22_COMPATIBILITY_FALSE)
AC_SUBST(OM22_COMPATIBILITY_TRUE)

if test x$enable_utf8 = x
then
    enable_utf8="yes"
fi

if test "$enable_utf8" = "yes"
then
  AC_DEFINE([UTF8_SUPPORTED], [1], [UTF8 is supported])
  AC_SEARCH_LIBS([iconv_open], [iconv], ,
    AC_SEARCH_LIBS([libiconv_open], [iconv]))
fi

AC_ARG_ENABLE(printing, [  --disable-printing
                       Disable printing support])

if test x$enable_printing = x
then
    enable_printing="yes"
fi

if test "$enable_printing" = "yes"
then
  AC_MSG_CHECKING([for libXp])
  AC_CHECK_HEADERS(X11/extensions/Print.h,
    AC_CHECK_LIB(Xp, XpCreateContext, ,enable_printing="no"),
  enable_printing="no")
fi

if test "$enable_printing" = "yes"
then
  LIB_XP=-lXp
  AC_DEFINE([PRINTING_SUPPORTED], [1], [Printing is supported])
  PRINTING_SUPPORTED_FALSE=!
  PRINTING_SUPPORTED_TRUE=
else
  LIB_XP=
  PRINTING_SUPPORTED_FALSE=
  PRINTING_SUPPORTED_TRUE=!
fi
AC_SUBST(PRINTING_SUPPORTED_FALSE)
AC_SUBST(PRINTING_SUPPORTED_TRUE)

AM_CONDITIONAL(PRINTING, test "$enable_printing" = "yes")
AC_SUBST(LIB_XP)

AC_FIND_XFT
AC_IMAGE_SUPPORT



# AM_CONDITIONAL(Motif22Compatibility, test x$enable_motif22_compatibility = xyes)

AC_CONFIG_FILES([Makefile \
bindings/Makefile \
bitmaps/Makefile \
clients/Makefile \
clients/mwm/Makefile \
clients/mwm/WmWsmLib/Makefile \
clients/uil/Makefile \
clients/xmbind/Makefile \
config/Makefile \
config/util/Makefile \
include/Makefile \
include/Dt/Makefile \
lib/Makefile \
lib/Xm/Makefile \
lib/Mrm/Makefile \
localized/Makefile \
localized/util/Makefile \
doc/Makefile \
doc/man/Makefile \
doc/man/man1/Makefile \
doc/man/man3/Makefile \
doc/man/man4/Makefile \
doc/man/man5/Makefile \
tools/Makefile \
tools/wml/Makefile \
demos/Makefile \
demos/lib/Makefile \
demos/lib/Xmd/Makefile \
demos/lib/Wsm/Makefile \
demos/lib/Exm/Makefile \
demos/lib/Exm/wml/Makefile \
demos/programs/Makefile \
demos/programs/Exm/Makefile \
demos/programs/Exm/app_in_c/Makefile \
demos/programs/Exm/app_in_uil/Makefile \
demos/programs/Exm/simple_app/Makefile \
demos/programs/airport/Makefile \
demos/programs/animate/Makefile \
demos/programs/drag_and_drop/Makefile \
demos/programs/draw/Makefile \
demos/programs/earth/Makefile \
demos/programs/filemanager/Makefile \
demos/programs/fileview/Makefile \
demos/programs/getsubres/Makefile \
demos/programs/hellomotif/Makefile \
demos/programs/hellomotifi18n/Makefile \
demos/programs/hellomotifi18n/C/Makefile \
demos/programs/hellomotifi18n/C/uid/Makefile \
demos/programs/hellomotifi18n/english/Makefile \
demos/programs/hellomotifi18n/english/uid/Makefile \
demos/programs/hellomotifi18n/french/Makefile \
demos/programs/hellomotifi18n/french/uid/Makefile \
demos/programs/hellomotifi18n/hebrew/Makefile \
demos/programs/hellomotifi18n/hebrew/uid/Makefile \
demos/programs/hellomotifi18n/japan/Makefile \
demos/programs/hellomotifi18n/japan/uid/Makefile \
demos/programs/hellomotifi18n/japanese/Makefile \
demos/programs/hellomotifi18n/japanese/uid/Makefile \
demos/programs/hellomotifi18n/swedish/Makefile \
demos/programs/hellomotifi18n/swedish/uid/Makefile \
demos/programs/i18ninput/Makefile \
demos/programs/panner/Makefile \
demos/programs/periodic/Makefile \
demos/programs/piano/Makefile \
demos/programs/popups/Makefile \
demos/programs/sampler2_0/Makefile \
demos/programs/setdate/Makefile \
demos/programs/todo/Makefile \
demos/programs/workspace/Makefile \
demos/programs/tooltips/Makefile \
demos/programs/FontSel/Makefile \
demos/programs/ButtonBox/Makefile \
demos/programs/ColorSel/Makefile \
demos/programs/Column/Makefile \
demos/programs/Combo2/Makefile \
demos/programs/Ext18List/Makefile \
demos/programs/Ext18List/pixmaps/Makefile \
demos/programs/IconB/Makefile \
demos/programs/Outline/Makefile \
demos/programs/Paned/Makefile \
demos/programs/TabStack/Makefile \
demos/programs/Tree/Makefile \
demos/programs/pixmaps/Makefile \
demos/unsupported/Makefile \
demos/unsupported/Exm/Makefile \
demos/unsupported/aicon/Makefile \
demos/unsupported/dainput/Makefile \
demos/unsupported/dogs/Makefile \
demos/unsupported/hellomotif/Makefile \
demos/unsupported/motifshell/Makefile \
demos/unsupported/uilsymdump/Makefile \
demos/unsupported/xmapdef/Makefile \
demos/unsupported/xmfonts/Makefile \
demos/unsupported/xmforc/Makefile \
demos/unsupported/xmform/Makefile \
demos/doc/Makefile \
demos/doc/programGuide/Makefile \
demos/doc/programGuide/ch05/Makefile \
demos/doc/programGuide/ch05/Scale/Makefile \
demos/doc/programGuide/ch06/Makefile \
demos/doc/programGuide/ch06/spin_box/Makefile \
demos/doc/programGuide/ch06/combo_box/Makefile \
demos/doc/programGuide/ch08/Makefile \
demos/doc/programGuide/ch08/Notebook/Makefile \
demos/doc/programGuide/ch08/Container/Makefile \
demos/doc/programGuide/ch16/Makefile \
demos/doc/programGuide/ch17/Makefile \
demos/doc/programGuide/ch17/simple_drop/Makefile \
demos/doc/programGuide/ch17/simple_drag/Makefile \
lib/Xm/xmstring.list \
])
AC_OUTPUT
