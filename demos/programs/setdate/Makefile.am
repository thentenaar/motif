MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README XmdSetDate.ad

bindir  = ${prefix}/share/Xm/setDate
datadir = ${prefix}/share/Xm/setDate
mandir  = ${prefix}/share/Xm/setDate

data_DATA        = $(setDate_SOURCES) $(EXTRA_DIST)
man_MANS         = setDate.man
bin_PROGRAMS     = setDate
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
setDate_LDADD    = $(top_builddir)/lib/Xm/libXm.la

