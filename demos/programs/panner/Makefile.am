MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/motif/demos/panner
datadir = ${prefix}/share/motif/demos/panner
mandir  = ${prefix}/share/motif/demos/panner

data_DATA        = $(panner_SOURCES) $(EXTRA_DIST)
man_MANS         = panner.man
bin_PROGRAMS     = panner
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
panner_LDADD     = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

