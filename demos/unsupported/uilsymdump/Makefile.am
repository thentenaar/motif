MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/motif/demos/uilsymdump
datadir = ${prefix}/share/motif/demos/uilsymdump

data_DATA        = uilsymdump.c $(EXTRA_DIST)
bin_PROGRAMS     = uilsymdump
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib \
                   -I$(top_srcdir)/clients -I$(top_builddir)/clients
uilsymdump_LDADD = $(top_builddir)/clients/uil/libUil.la $(X_LIBS)

