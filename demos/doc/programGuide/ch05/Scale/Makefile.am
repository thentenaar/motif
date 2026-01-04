MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/ch05/Scale
datadir = ${prefix}/share/Xm/ch05/Scale
mandir  = ${prefix}/share/Xm/ch05/Scale

data_DATA        = $(tic_mark_SOURCES) $(EXTRA_DIST)
man_MANS         = tic_mark.man
bin_PROGRAMS     = tic_mark
tic_mark_SOURCES = tic_mark.c interface.c
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS      = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
tic_mark_LDADD   = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                   $(top_builddir)/lib/Xm/libXm.la

