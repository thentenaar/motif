MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = aicon.help

bindir  = ${prefix}/share/Xm/aicon
datadir = ${prefix}/share/Xm/aicon

data_DATA        = aicon.c $(EXTRA_DIST)
bin_PROGRAMS     = aicon
AM_CPPFLAGS      = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
aicon_LDADD      = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                   $(top_builddir)/lib/Xm/libXm.la

