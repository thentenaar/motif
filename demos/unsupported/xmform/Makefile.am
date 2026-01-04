MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/xmform
datadir = ${prefix}/share/Xm/xmform

data_DATA        = xmform.c
bin_PROGRAMS     = xmform
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
xmform_LDADD     = $(top_builddir)/lib/Xm/libXm.la

