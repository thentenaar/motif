MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/xmforc
datadir = ${prefix}/share/Xm/xmforc

data_DATA        = xmforc.c
bin_PROGRAMS     = xmforc
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
xmforc_LDADD     = $(top_builddir)/lib/Xm/libXm.la

