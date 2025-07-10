MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/piano
datadir = ${prefix}/share/Xm/piano

bin_PROGRAMS = piano
piano_SOURCES = piano.c
EXTRA_DIST = piano.images README Inv8_part1.piano Inv8_part2.piano \
             piano.man
data_DATA  = $(EXTRA_DIST) $(piano_SOURCES)
LDADD = ../../../lib/Xm/libXm.la -lm

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
