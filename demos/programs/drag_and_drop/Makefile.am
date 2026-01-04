MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/drag_and_drop
datadir = ${prefix}/share/Xm/drag_and_drop
mandir  = ${prefix}/share/Xm/drag_and_drop

data_DATA        = $(DNDDemo_SOURCES) DNDDemo.h simpledrop.c $(EXTRA_DIST)
man_MANS         = DNDDemo.man simpledrop.man
bin_PROGRAMS     = DNDDemo simpledrop
DNDDemo_SOURCES  = DNDDemo.c DNDDraw.c
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
LDADD            = $(top_builddir)/lib/Xm/libXm.la

