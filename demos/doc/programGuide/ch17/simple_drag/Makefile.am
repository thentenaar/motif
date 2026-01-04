MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/ch17/simple_drag
datadir = ${prefix}/share/Xm/ch17/simple_drag
mandir  = ${prefix}/share/Xm/ch17/simple_drag

data_DATA         = $(simple_drag_SOURCES) $(EXTRA_DIST)
man_MANS          = simple_drag.man
bin_PROGRAMS      = simple_drag
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS       = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
simple_drag_LDADD = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                    $(top_builddir)/lib/Xm/libXm.la

