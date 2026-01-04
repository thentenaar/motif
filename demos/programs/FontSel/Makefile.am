MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/fontsel
datadir = ${prefix}/share/Xm/fontsel

data_DATA        = $(ext18list_SOURCES) fontsel.h
noinst_HEADERS   = fontsel.h
bin_PROGRAMS     = fontsel
fontsel_SOURCES  = callbacks-c.c creation-c.c fontsel.c
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
fontsel_LDADD    = $(top_builddir)/lib/Xm/libXm.la

