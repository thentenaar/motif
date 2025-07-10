MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = README simple_drag.man

noinst_PROGRAMS = simple_drag

simple_drag_SOURCES = simple_drag.c

AM_CPPFLAGS = -I$(top_builddir)/lib -I$(top_srcdir)/lib -I$(top_srcdir)/demos/lib $(X_CFLAGS)

LDADD = $(top_builddir)/demos/lib/Xmd/libXmd.la $(top_builddir)/lib/Xm/libXm.la
