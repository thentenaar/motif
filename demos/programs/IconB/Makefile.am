MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/Icon
datadir = ${prefix}/share/Xm/Icon

EXTRA_DIST = small.bm
bin_PROGRAMS = iconbuttondemo
iconbuttondemo_SOURCES = misc.c  iconbuttondemo.c
data_DATA = $(iconbuttondemo_SOURCES) $(EXTRA_DIST)
TOPDIR = ../../..
LDADD = $(TOPDIR)/lib/Xm/libXm.la

AM_CPPFLAGS = -I$(TOPDIR)/lib -I$(srcdir)/.. \
           -I$(srcdir)/$(TOPDIR)/lib \
           ${X_CFLAGS}
