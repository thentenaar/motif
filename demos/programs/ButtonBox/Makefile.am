MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/ButtonBox
datadir = ${prefix}/share/Xm/ButtonBox

bin_PROGRAMS = bboxdemo
EXTRA_DIST =
bboxdemo_SOURCES = bboxdemo.c
data_DATA = $(EXTRA_DIST) $(bboxdemo_SOURCES)
TOPDIR = ../../..
LDADD = $(TOPDIR)/lib/Xm/libXm.la

AM_CPPFLAGS = -I$(TOPDIR)/lib -I$(srcdir)/$(TOPDIR)/lib ${X_CFLAGS}
