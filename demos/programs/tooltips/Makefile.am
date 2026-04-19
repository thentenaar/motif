MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/motif/demos/tooltips
datadir = ${prefix}/share/motif/demos/tooltips

data_DATA        = tooltips.c
bin_PROGRAMS     = tooltips
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
tooltips_LDADD   = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

