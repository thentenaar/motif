MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/Color
datadir = ${prefix}/share/Xm/Color

bin_PROGRAMS = colordemo
colordemo_SOURCES = util-c.c colordemo.c
EXTRA_DIST =
data_DATA = $(colordemo_SOURCES) $(EXTRA_DIST)
LDADD = $(TOPDIR)/lib/Xm/libXm.la
TOPDIR = ../../..

AM_CPPFLAGS = -I$(TOPDIR)/lib -I$(srcdir)/$(TOPDIR)/lib ${X_CFLAGS}
