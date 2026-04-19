MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/motif/demos/Column
datadir = ${prefix}/share/motif/demos/Column

data_DATA        = column.c
bin_PROGRAMS     = column
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
column_LDADD     = $(top_builddir)/lib/Xm/libXm.la $(X_LIBS)

