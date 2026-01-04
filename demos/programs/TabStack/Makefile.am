MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/Tabstack
datadir = ${prefix}/share/Xm/Tabstack

data_DATA        = $(tabstack_SOURCES)
bin_PROGRAMS     = tabstack
noinst_HEADERS   = creation.h
tabstack_SOURCES = bxutils.c callbacks.c creation.c main.c
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
tabstack_LDADD   = $(top_builddir)/lib/Xm/libXm.la

