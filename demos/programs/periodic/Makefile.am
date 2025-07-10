MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/periodic
datadir = ${prefix}/share/Xm/periodic

bin_PROGRAMS = periodic
EXTRA_DIST = periodic.uil periodic_l.uil XmdPeriodic.ad README
periodic_SOURCES = periodic.c
data_DATA = $(EXTRA_DIST) $(periodic_SOURCES) periodic.uid
LDADD = ../../../lib/Xm/libXm.la ../../../lib/Mrm/libMrm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}

all: periodic.uid

periodic.uid: periodic.uil
	../../../clients/uil/uil -o $@ $? -I$(srcdir)/../../../clients/uil -I../../../clients/uil

clean-local:
	$(RM) periodic.uid


