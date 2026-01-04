MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/autopopups
datadir = ${prefix}/share/Xm/autopopups
mandir  = ${prefix}/share/Xm/autopopups

data_DATA        = $(autopopups_SOURCES) $(EXTRA_DIST)
man_MANS         = autopopups.man
bin_PROGRAMS     = autopopups
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
autopopups_LDADD = $(top_builddir)/lib/Xm/libXm.la

