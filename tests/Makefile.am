
AM_TESTS_ENVIRONMENT = env AM_TAP_AWK='$(AWK)'
LOG_DRIVER           = $(SHELL) $(top_srcdir)/tap-driver.sh
TESTS                = tests
AM_CPPFLAGS          = -I$(top_builddir)/include -I$(top_builddir)/lib $(X_CFLAGS) $(XFT_CFLAGS) $(CHECK_CFLAGS)
check_PROGRAMS       = tests
noinst_HEADERS       = suites.h
tests_SOURCES        = runner.c XmFontListEntry.c XmFontList.c Png.c Jpeg.c Svg.c
tests_LDADD          = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS) $(XFT_LIBS) $(CHECK_LIBS)
MAINTAINERCLEANFILES = Makefile.in

