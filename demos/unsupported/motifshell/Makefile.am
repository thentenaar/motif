MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README help membership motif principles research welcome

bindir  = ${prefix}/share/motif/demos/motifshell
datadir = ${prefix}/share/motif/demos/motifshell

data_DATA        = motifshell.c $(EXTRA_DIST)
bin_PROGRAMS     = motifshell
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
motifshell_LDADD = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

