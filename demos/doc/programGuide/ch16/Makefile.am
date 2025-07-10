MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = README UTMXmText.man

noinst_PROGRAMS = UTMXmText

UTMXmText_SOURCES = UTMXmText.c

AM_CPPFLAGS = -I$(top_builddir)/lib -I$(top_srcdir)/lib -I$(top_srcdir)/demos/lib $(X_CFLAGS)

LDADD = $(top_builddir)/demos/lib/Xmd/libXmd.la $(top_builddir)/lib/Xm/libXm.la
