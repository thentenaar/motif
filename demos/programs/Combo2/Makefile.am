MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = Combo.ad

bindir  = ${prefix}/share/motif/demos/combo2
datadir = ${prefix}/share/motif/demos/combo2

data_DATA        = $(EXTRA_DIST) combo.c
bin_PROGRAMS     = combo
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
combo_LDADD      = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

