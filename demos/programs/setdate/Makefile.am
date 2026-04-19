MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README XmdSetDate.ad

bindir  = ${prefix}/share/motif/demos/setDate
datadir = ${prefix}/share/motif/demos/setDate
mandir  = ${prefix}/share/motif/demos/setDate

data_DATA        = $(setDate_SOURCES) $(EXTRA_DIST)
man_MANS         = setDate.man
bin_PROGRAMS     = setDate
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
setDate_LDADD    = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

