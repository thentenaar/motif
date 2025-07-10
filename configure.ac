dnl Library version
m4_define([XM_MAJOR], [2])
m4_define([XM_MINOR], [3])
m4_define([XM_PATCH], [9])
m4_define([XM_VERSION], [XM_MAJOR.XM_MINOR.XM_PATCH])

dnl Shared library version (libtool)
m4_define([ABI_CURRENT],  [4])
m4_define([ABI_REVISION], [5])
m4_define([ABI_AGE],      [0])
m4_define([ABI_VERSION],  [ABI_CURRENT:ABI_REVISION:ABI_AGE])

AC_PREREQ([2.70])
LT_PREREQ([2.4.6])
AC_INIT([motif], [XM_VERSION], [https://github.com/thentenaar/motif])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([lib/Xm/Form.c])
AM_INIT_AUTOMAKE([foreign subdir-objects silent-rules no-installinfo no-define])
LT_INIT
AM_SILENT_RULES([yes])
AC_LANG([C])

dnl Check for the C compiler / pkg-config
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PATH_PROG([PKG_CONFIG], [pkg-config])
AC_CHECK_PROGS(YACC, byacc, yacc)
AM_PROG_LEX(noyywrap)

dnl Flex needs libfl
LEXLIB=
if test "x$LEX" == "xflex"; then
	LEXLIB=-lfl
fi
AC_SUBST([LEXLIB])

dnl Check the host system type and compiler characteristics
AC_CANONICAL_HOST
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_INTPTR_T
AC_TYPE_UINTPTR_T
AC_CHECK_TYPE([XICProc],,,[#include <X11/Xlib.h>])

dnl Check for X
PKG_CHECK_MODULES([X], [x11 xpm xt xmu xext])

dnl Check for particular headers / functions
AC_CHECK_HEADERS([alloca.h iconv.h wctype.h sys/poll.h sys/time.h X11/Xos_r.h X11/Xpoll.h])
AC_CHECK_FUNCS([getcwd getwd iconv nanosleep putenv regcomp regex strcasecmp strdup strlcat])
AC_DEFINE([HAVE_SYS_POLL_H])
AC_DEFINE([HAVE_SYS_TIME_H])
AC_DEFINE_UNQUOTED([HAS_WIDECHAR_FUNCTIONS], $(test "x$ac_cv_header_wctype_h" == "x" ; echo $?),
                   [True if we have wctype.h])

dnl Image formats
AC_ARG_WITH([jpeg],
	[AS_HELP_STRING([--with-jpeg], [Enable jpeg support])],
	[with_jpeg=$withval], [with_jpeg=yes])

AC_ARG_WITH([png],
	[AS_HELP_STRING([--with-png], [Enable png support])],
	[with_png=$withval], [with_png=yes])

AC_ARG_WITH([xft],
	[AS_HELP_STRING([--with-xft], [Enable xft support])],
	[with_xft=$withval], [with_xft=yes])

AS_IF([test "x$with_jpeg" == "xyes"],[PKG_CHECK_MODULES([JPEG], [libjpeg])])
AS_IF([test "x$with_png"  == "xyes"],[PKG_CHECK_MODULES([PNG],  [libpng])])
AS_IF([test "x$with_xft"  == "xyes"],[PKG_CHECK_MODULES([XFT],  [xft >= 2])])

AC_ARG_ENABLE([debug],
	[AS_HELP_STRING(
		[--enable-debug],
		[enable debugging support])
	], [enable_debug=$enableval], [enable_debug=no]
)

AC_ARG_ENABLE([utf8],
	[AS_HELP_STRING(
		[--enable-utf8],
		[enable UTF-8 support])
	], [enable_utf8=$enableval], [enable_utf8=yes]
)

AC_ARG_ENABLE([printing],
	[AS_HELP_STRING(
		[--enable-printing],
		[enable printing support])
	], [enable_printing=$enableval], [enable_printing=no]
)

dnl Printing support relies on the X print protocol
AS_IF([test "x$enable_printing" == "xyes"],[
	PKG_CHECK_MODULES([XP], [xp], [], [enable_printing=no])
	AS_IF([test "x$enable_printing" == "xno"], [AC_MSG_FAILURE([libXp not found])])
])

AC_ARG_ENABLE([motif22-compatibility],
	[AS_HELP_STRING(
		[--enable-motif22-compatibility],
		[enable backwards compatibility for Motif 2.2])
	],
	[enable_motif22_compatibility=$enableval],
	[enable_motif22_compatibility=no]
)

dnl Signal backward-compatibility via libtool's age
AS_IF([test "x$enable_motif22_compatibility" == "xyes"],[
	ABI_AGE=1
	dnl For lib/Xm/xmstring.list
	XM_22_COMPAT_TRUE=
	XM_22_COMPAT_FALSE=!
].[
	XM_22_COMPAT_TRUE=!
	XM_22_COMPAT_FALSE=
])

AC_ARG_ENABLE([message-catalog],
	[AS_HELP_STRING(
		[--enable-message-catalog],
		[enable X/Open message catalog support])
	],
	[enable_message_catalog=$enableval],
	[enable_message_catalog=no]
)

dnl Debug builds (assert, etc.)
AS_IF([test "x$enable_debug" == "xyes"],[
	AX_APPEND_COMPILE_FLAGS([-Og -ggdb])
],[
	AX_APPEND_COMPILE_FLAGS([-O2])
	AX_APPEND_COMPILE_FLAGS([-DNDEBUG], [CPPFLAGS])
])

dnl Compiler flags
AX_CFLAGS

dnl Automake substitutions
AC_SUBST([ABI_VERSION], [ABI_VERSION])
AM_CONDITIONAL([XM_PRINTING],  test "x$enable_printing" == "xyes")
AM_CONDITIONAL([XM_WITH_JPEG], test "x$with_jpeg" == "xyes")
AM_CONDITIONAL([XM_WITH_PNG],  test "x$with_png"  == "xyes")
AM_CONDITIONAL([XM_MSGCAT],    test "x$enable_message_catalog" == "xyes")

dnl Substitutions for lib/Xm/Xm.h
AC_SUBST([XM_MAJOR], [XM_MAJOR])
AC_SUBST([XM_MINOR], [XM_MINOR])
AC_SUBST([XM_PATCH], [XM_PATCH])
AC_DEFINE_UNQUOTED(
	[XM_22_COMPAT],
	$(test "x$enable_motif22_compatibility" != "xyes" ; echo $?),
	[1 for OpenMotif 2.2 Compatibility]
)

AC_DEFINE_UNQUOTED(
	[XM_UTF8],
	$(test "x$enable_utf8" != "xyes"; echo $?),
	[1 if Motif was built with UTF8 Support]
)

AC_DEFINE_UNQUOTED(
	[XM_PRINTING],
	$(test "x$enable_printing" != "xyes"; echo $?),
	[1 if Motif was built with printing support]
)

AC_DEFINE_UNQUOTED(
	[USE_XFT],
	$(test "x$pkg_cv_XFT_LIBS"  == "x" ; echo $?),
	[1 if Motif ws built with Xft support]
)

AC_DEFINE_UNQUOTED(
	[XM_WITH_JPEG],
	$(test "x$with_jpeg" != "xyes" ; echo $?),
	[1 if Motif was built with JPEG support]
)

AC_DEFINE_UNQUOTED(
	[XM_WITH_PNG],
	$(test "x$with_png"  != "xyes" ; echo $?),
	[1 if Motif was built with PNG support]
)

AC_DEFINE_UNQUOTED(
	[XM_MSGCAT],
	$(test "x$enable_message_catalog" != "xyes" ; echo $?),
	[1 if X/Open message catalog support is enabled]
)

dnl Substitutions for lib/Xm/xmstring.list
AC_SUBST(XM_22_COMPAT_TRUE)
AC_SUBST(XM_22_COMPAT_FALSE)

dnl Defines/substitutions for mwm
AC_DEFINE([NO_OL_COMPAT], 1, "No OL Compatability")
AC_SUBST([CDE_INSTALLATION_TOP], "\${prefix}/dt")
AC_SUBST([CDE_CONFIGURATION_TOP], "\${prefix}/etc/dt")
AC_SUBST([MWMRCDIR], "${sysconfdir}/X11")

dnl lib/Xm/VirtKeys.c
AC_SUBST([XMBINDDIR_FALLBACK], "${datadir}/X11/bindings")

dnl lib/Xm/Xmos.c
AC_SUBST([INCDIR], "${includedir}/X11")
AC_SUBST([LIBDIR], "${libdir}/X11")

AC_CONFIG_HEADERS(include/config.h lib/Xm/Xm.h)
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
lib/Xm/version.h \
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

