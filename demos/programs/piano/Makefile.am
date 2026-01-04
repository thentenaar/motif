MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README piano.images Inv8_part1.piano Inv8_part2.piano

bindir  = ${prefix}/share/Xm/piano
datadir = ${prefix}/share/Xm/piano
mandir  = ${prefix}/share/Xm/piano

data_DATA        = $(piano_SOURCES) $(EXTRA_DIST)
man_MANS         = piano.man
bin_PROGRAMS     = piano
AM_CPPFLAGS      = $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
piano_LDADD      = $(top_builddir)/lib/Xm/libXm.la

