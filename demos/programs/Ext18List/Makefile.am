MAINTAINERCLEANFILES = Makefile.in

bindir  = ${prefix}/share/Xm/Ext18list
datadir = ${prefix}/share/Xm/Ext18list
SUBDIRS = pixmaps

data_DATA         = $(ext18list_SOURCES) extlist.h
noinst_HEADERS    = extlist.h
bin_PROGRAMS      = ext18list
ext18list_SOURCES = util-c.c callbacks-c.c creation.c ext18list.c
AM_CPPFLAGS       = -I$(srcdir) $(X_CFLAGS)
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
ext18list_LDADD   = $(top_builddir)/lib/Xm/libXm.la

