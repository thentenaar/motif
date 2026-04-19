MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README terre.xbm

bindir  = ${prefix}/share/motif/demos/earth
datadir = ${prefix}/share/motif/demos/earth
mandir  = ${prefix}/share/motif/demos/earth

data_DATA        = $(earth_SOURCES) $(EXTRA_DIST)
man_MANS         = earth.man
bin_PROGRAMS     = earth
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
earth_LDADD      = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

