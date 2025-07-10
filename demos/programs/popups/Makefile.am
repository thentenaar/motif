MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/autopopups
datadir = ${prefix}/share/Xm/autopopups

bin_PROGRAMS = autopopups
autopopups_SOURCES = autopopups.c
EXTRA_DIST = README autopopups.man
data_DATA = $(EXTRA_DIST) $(autopopups_SOURCES)
LDADD = ../../../lib/Xm/libXm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
