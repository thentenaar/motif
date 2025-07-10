MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/hellomotif
datadir = ${prefix}/share/Xm/hellomotif

bin_PROGRAMS = hellomotif
EXTRA_DIST = hellomotif.uil README
hellomotif_SOURCES = hellomotif.c
data_DATA = $(EXTRA_DIST) $(hellomotif_SOURCES) hellomotif.uid
LDADD = ../../../lib/Xm/libXm.la ../../../lib/Mrm/libMrm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}

all: hellomotif.uid

hellomotif.uid: hellomotif.uil
	../../../clients/uil/uil -o $@ $? -I$(srcdir)/../../../clients/uil -I../../../clients/uil

clean-local:
	$(RM) hellomotif.uid
