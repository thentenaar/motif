
MAINTAINERCLEANFILES = Makefile.in
DEFAULT_INCLUDES     = -I$(top_srcdir)/include -I$(top_srcdir)/lib      \
                       -I$(top_srcdir)/lib/Xm -I$(top_builddir)/include \
                       -I$(top_builddir)/lib
AM_TESTS_ENVIRONMENT = env AM_TAP_AWK='$(AWK)'
LOG_DRIVER           = $(SHELL) $(top_srcdir)/tap-driver.sh
TESTS                = tests
AM_CPPFLAGS          = -I. -DRESDIR="$(abs_top_srcdir)/tests/" \
                       $(X_CFLAGS) $(XFT_CFLAGS) $(CHECK_CFLAGS)
check_PROGRAMS       = tests
noinst_HEADERS       = suites.h
tests_LDADD          = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS) $(XFT_LIBS) $(CHECK_LIBS)
tests_SOURCES        = runner.c XmDesktopObject.c XmFontListEntry.c XmFontList.c \
                       XmScreen.c XmString.c Png.c Jpeg.c Svg.c Cursor.c

