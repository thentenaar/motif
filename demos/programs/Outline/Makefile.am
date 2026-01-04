MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/Outline
datadir = ${prefix}/share/Xm/Outline

data_DATA        = $(outline_SOURCES)
bin_PROGRAMS     = outline
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
outline_LDADD    = $(top_builddir)/lib/Xm/libXm.la

