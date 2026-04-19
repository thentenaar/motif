MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = small.bm

bindir  = ${prefix}/share/motif/demos/Icon
datadir = ${prefix}/share/motif/demos/Icon

data_DATA              = $(iconbuttondemo_SOURCES) small.bm
bin_PROGRAMS           = iconbuttondemo
iconbuttondemo_SOURCES = misc.c iconbuttondemo.c
AM_CPPFLAGS            = $(X_CFLAGS)
DEFAULT_INCLUDES       = -I. -I$(srcdir)/.. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                         -I$(top_builddir)/include -I$(top_builddir)/lib
iconbuttondemo_LDADD   = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

