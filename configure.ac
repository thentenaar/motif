dnl Library version
m4_define([XM_MAJOR], [2])
m4_define([XM_MINOR], [4])
m4_define([XM_PATCH], [0])
m4_define([XM_VERSION], [XM_MAJOR.XM_MINOR.XM_PATCH])

dnl Shared library version (libtool)
m4_define([ABI_CURRENT],  [5])
m4_define([ABI_REVISION], [0])
m4_define([ABI_AGE],      [0])
m4_define([ABI_VERSION],  [ABI_CURRENT:ABI_REVISION:ABI_AGE])

AC_PREREQ([2.70])
LT_PREREQ([2.4.2])
AC_INIT([motif], [XM_VERSION], [https://github.com/thentenaar/motif/issues], [motif], [https://github.com/thentenaar/motif])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([lib/Xm/Form.c])
AM_INIT_AUTOMAKE([foreign subdir-objects silent-rules no-installinfo no-define dist-xz])
LT_INIT
AM_SILENT_RULES([yes])
AM_EXTRA_RECURSIVE_TARGETS([demos doc])
AC_LANG([C])

dnl Check for the C compiler / pkg-config
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PATH_PROG([PKG_CONFIG], [pkg-config])
AC_CHECK_PROGS(YACC, byacc, yacc)
AM_PROG_LEX(yywrap)

dnl Flex needs libfl
LEXLIB=
if test "x$ac_cv_prog_LEX" == "xflex"; then
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
PKG_INSTALLDIR
PKG_CHECK_MODULES([X], [x11 xpm xt xmu xext])

dnl Check for particular headers / functions
AC_CHECK_HEADERS([alloca.h iconv.h wctype.h wchar.h fcntl.h sys/poll.h sys/time.h X11/Xos_r.h X11/Xpoll.h])
AC_CHECK_FUNCS([getcwd getwd iconv nanosleep putenv regcomp regex strcasecmp strdup strlcat wcslen])
AC_DEFINE([HAVE_SYS_POLL_H])
AC_DEFINE([HAVE_SYS_TIME_H])

dnl Check for libiconv if needed
AS_IF([test "x$ac_cv_func_iconv" == "xno"],[
	AC_SEARCH_LIBS([libiconv_open], [iconv], [], [
		AC_MSG_ERROR([Unable to find iconv])
	])
])

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

dnl Optimistically look for xrandr
AC_ARG_WITH([xrandr],
	[AS_HELP_STRING([--with-xrandr], [Enable Xrandr support])],
	[with_xrandr=$withval], [with_xrandr=yes])

dnl Xinerama has been deprecated for ages, so the user should manually
dnl indicate that they actually want it unless we lack xrandr.
AC_ARG_WITH([xinerama],
	[AS_HELP_STRING([--with-xinerama], [Enable Xinerama support])],
	[with_xinerama=$withval], [with_xinerama=no])

AS_IF([test "x$with_xrandr" == "xyes"],[
	PKG_CHECK_MODULES([XRANDR], [xrandr], [], [
		with_xrandr=no
		with_xinerama=yes
	])
],[with_xinerama=yes])

AS_IF([test "x$with_xinerama" == "xyes"],[
	PKG_CHECK_MODULES([XINERAMA], [xinerama], [], [with_xinerama=no])
])

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

AC_ARG_ENABLE([message-catalog],
	[AS_HELP_STRING(
		[--enable-message-catalog],
		[enable X/Open message catalog support])
	],
	[enable_message_catalog=$enableval],
	[enable_message_catalog=no]
)

dnl Tests
AC_ARG_ENABLE([tests],
	[AS_HELP_STRING([--enable-tests], [Enable automated tests])],
	[enable_tests=$enableval], [enable_tests=no])
AS_IF([test "x$enable_tests" == "xyes"],[
	AC_PROG_AWK
	AC_REQUIRE_AUX_FILE([tap-driver.sh])
	PKG_CHECK_MODULES([CHECK], [check])
])

dnl Debug builds (assert, etc.)
AS_IF([test "x$enable_debug" == "xyes"],[
	AX_APPEND_COMPILE_FLAGS([-Og -ggdb])
	AX_APPEND_COMPILE_FLAGS([-DDEBUG], [CPPFLAGS])
],[
	AX_APPEND_COMPILE_FLAGS([-O2])
	AX_APPEND_COMPILE_FLAGS([-DNDEBUG], [CPPFLAGS])
])

dnl Compiler flags
AX_CFLAGS

dnl .pc
AC_SUBST([XM_VERSION], [XM_VERSION])

dnl Automake substitutions
AC_SUBST([ABI_VERSION], [ABI_VERSION])
AM_CONDITIONAL([XM_WITH_JPEG], test "x$with_jpeg" == "xyes")
AM_CONDITIONAL([XM_WITH_PNG],  test "x$with_png"  == "xyes")
AM_CONDITIONAL([XM_MSGCAT],    test "x$enable_message_catalog" == "xyes")

dnl Substitutions for lib/Xm/Xm.h
AC_SUBST([XM_MAJOR], [XM_MAJOR])
AC_SUBST([XM_MINOR], [XM_MINOR])
AC_SUBST([XM_PATCH], [XM_PATCH])

AC_DEFINE_UNQUOTED(
	[XM_UTF8],
	$(test "x$enable_utf8" != "xyes"; echo $?),
	[1 if Motif was built with UTF8 Support]
)

AC_DEFINE_UNQUOTED(
	[USE_XFT],
	$(test "x$with_xft" != "xyes" ; echo $?),
	[1 if Motif was built with Xft support]
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
	[XM_WITH_XRANDR],
	$(test "x$with_xrandr"  != "xyes" ; echo $?),
	[1 if Motif was built with Xrandr support]
)

AC_DEFINE_UNQUOTED(
	[XM_WITH_XINERAMA],
	$(test "x$with_xinerama"  != "xyes" ; echo $?),
	[1 if Motif was built with Xinerama support]
)

AC_DEFINE_UNQUOTED(
	[XM_MSGCAT],
	$(test "x$enable_message_catalog" != "xyes" ; echo $?),
	[1 if X/Open message catalog support is enabled]
)

dnl Defines/substitutions for mwm
AC_DEFINE([NO_OL_COMPAT], 1, "No OL Compatability")
AC_SUBST([CDE_INSTALLATION_TOP], "\${prefix}/dt")
AC_SUBST([CDE_CONFIGURATION_TOP], "\${prefix}/etc/dt")
AC_SUBST([MWMRCDIR], "${sysconfdir}/X11")

dnl lib/Xm/VirtKeys.c
AC_SUBST([XMBINDDIR_FALLBACK], "${datadir}/X11/bindings")

dnl lib/Xm/Xmos.c
AC_SUBST([DATADIR], "${datadir}")
AS_IF([test "x$X11BASE" != "x"],[
	AC_SUBST([INCDIR], "${X11BASE}/include/X11")
	AC_SUBST([LIBDIR], "${X11BASE}/lib/X11")
	AS_IF([test "x$RGB_TXT" == "x"],[RGB_TXT="${X11BASE}/share/X11/rgb.txt"])

],[
	AC_SUBST([INCDIR], "${includedir}/X11")
	AC_SUBST([LIBDIR], "${libdir}/X11")
])

dnl lib/Xm/ColorS.c
dnl Various NIXes store this file in different places
AS_IF([test "x$RGB_TXT" == "x"],[RGB_TXT="${datarootdir}/X11/rgb.txt"])
AC_SUBST([RGB_TXT])

AC_CONFIG_HEADERS(include/config.h lib/Xm/Xm.h)
AC_CONFIG_FILES([Makefile \
motif.pc \
bindings/Makefile \
bitmaps/Makefile \
clients/Makefile \
clients/mwm/Makefile \
clients/mwm/WmWsmLib/Makefile \
clients/uil/Makefile \
clients/uil/XmAppl.uil \
clients/uil/Uil.pc \
clients/xmbind/Makefile \
config/Makefile \
config/util/Makefile \
include/Makefile \
include/Dt/Makefile \
icons/Makefile \
lib/Makefile \
lib/Xm/Makefile \
lib/Xm/version.h \
lib/Mrm/Makefile \
lib/Mrm/Mrm.pc \
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
tests/Makefile
])
AC_OUTPUT

