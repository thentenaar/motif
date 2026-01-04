MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/ch16/UTMXmText
datadir = ${prefix}/share/Xm/ch16/UTMXmText
mandir  = ${prefix}/share/Xm/ch16/UTMXmText

data_DATA         = $(UTMXmText_SOURCES) $(EXTRA_DIST)
man_MANS          = UTMXmText.man
bin_PROGRAMS      = UTMXmText
UTMXmText_SOURCES = UTMXmText.c
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS       = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
UTMXmText_LDADD   = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                    $(top_builddir)/lib/Xm/libXm.la

