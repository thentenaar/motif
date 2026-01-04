MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README getsubres.help

bindir  = ${prefix}/share/Xm/getsubres
datadir = ${prefix}/share/Xm/getsubres
mandir  = ${prefix}/share/Xm/getsubres

data_DATA        = $(getsubres_SOURCES) $(EXTRA_DIST)
man_MANS         = getsubres.man
bin_PROGRAMS     = getsubres
AM_CPPFLAGS      = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
getsubres_LDADD  = $(top_builddir)/demos/lib/Xmd/libXmd.la \
                   $(top_builddir)/lib/Xm/libXm.la

