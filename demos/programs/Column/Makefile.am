MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/Column
datadir = ${prefix}/share/Xm/Column

data_DATA        = column.c
bin_PROGRAMS     = column
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
column_LDADD     = $(top_builddir)/lib/Xm/libXm.la

