MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/motif/demos/Color
datadir = ${prefix}/share/motif/demos/Color

data_DATA         = $(colordemo_SOURCES)
bin_PROGRAMS      = colordemo
colordemo_SOURCES = colordemo.c util-c.c
AM_CPPFLAGS       = $(X_CFLAGS)
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
colordemo_LDADD   = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

