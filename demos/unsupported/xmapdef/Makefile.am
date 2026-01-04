MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/xmapdef
datadir = ${prefix}/share/Xm/xmapdef

data_DATA        = xmapdef.c
bin_PROGRAMS     = xmapdef
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
xmapdef_LDADD    = $(top_builddir)/lib/Xm/libXm.la

