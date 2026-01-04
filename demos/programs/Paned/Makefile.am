MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/Paned2
datadir = ${prefix}/share/Xm/Paned2

data_DATA        = $(paned_SOURCES)
bin_PROGRAMS     = paned
paned_SOURCES    = creation-c.c paned.c
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
paned_LDADD      = $(top_builddir)/lib/Xm/libXm.la

