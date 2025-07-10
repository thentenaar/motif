MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/Tabstack
datadir = ${prefix}/share/Xm/Tabstack

bin_PROGRAMS = tabstack
tabstack_SOURCES = bxutils.c callbacks.c creation.c main.c creation.h
EXTRA_DIST =
data_DATA = $(tabstack_SOURCES) $(EXTRA_DIST)
TOPDIR = ../../..
LDADD = $(TOPDIR)/lib/Xm/libXm.la

AM_CPPFLAGS = -I$(TOPDIR)/lib -I$(srcdir)/$(TOPDIR)/lib ${X_CFLAGS}
