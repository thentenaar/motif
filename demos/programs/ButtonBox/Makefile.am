MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/motif/demos/ButtonBox
datadir = ${prefix}/share/motif/demos/ButtonBox

data_DATA        = bboxdemo.c
bin_PROGRAMS     = bboxdemo
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
bboxdemo_LDADD   = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

