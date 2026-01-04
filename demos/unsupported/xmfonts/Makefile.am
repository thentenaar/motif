MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/xmfonts
datadir = ${prefix}/share/Xm/xmfonts

data_DATA        = xmfonts.c
bin_PROGRAMS     = xmfonts
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
xmfonts_LDADD    = $(top_builddir)/lib/Xm/libXm.la

