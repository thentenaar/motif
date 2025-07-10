MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = README tic_mark.man

noinst_PROGRAMS = tic_mark

tic_mark_SOURCES = tic_mark.c interface.c

AM_CPPFLAGS = -I$(top_builddir)/lib -I$(top_srcdir)/lib -I$(top_srcdir)/demos/lib $(X_CFLAGS)

LDADD = $(top_builddir)/demos/lib/Xmd/libXmd.la $(top_builddir)/lib/Xm/libXm.la
