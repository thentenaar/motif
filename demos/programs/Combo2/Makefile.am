MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/combo2
datadir = ${prefix}/share/Xm/combo2

bin_PROGRAMS = combo
EXTRA_DIST = Combo.ad
combo_SOURCES = combo.c
data_DATA = $(EXTRA_DIST) $(combo_SOURCES)
TOPDIR = ../../..
LDADD = $(TOPDIR)/lib/Xm/libXm.la

AM_CPPFLAGS = -I$(TOPDIR)/lib -I$(srcdir)/$(TOPDIR)/lib ${X_CFLAGS}
