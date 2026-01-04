MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/ch08/notebook
datadir = ${prefix}/share/Xm/ch08/notebook
mandir  = ${prefix}/share/Xm/ch08/notebook

data_DATA         = $(notebook_SOURCES) $(EXTRA_DIST)
man_MANS          = notebook.man
bin_PROGRAMS      = notebook
notebook_SOURCES  = interface.c notebook.c
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
AM_CPPFLAGS       = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
notebook_LDADD    = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                    $(top_builddir)/lib/Xm/libXm.la

