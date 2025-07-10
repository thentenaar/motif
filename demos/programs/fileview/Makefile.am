MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/fileview
datadir = ${prefix}/share/Xm/fileview

bin_PROGRAMS = fileview
fileview_SOURCES = main.c text.c file.c file.h fileE.h fileview.h main.h \
                   mainE.h text.h textE.h
EXTRA_DIST = $(UILS) English.df French.df German.df README Fileview.ad
data_DATA = French.uid German.uid English.uid $(fileview_SOURCES) $(EXTRA_DIST)

UILS= French.uil German.uil English.uil

LDADD = ../../../lib/Xm/libXm.la ../../../lib/Mrm/libMrm.la

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}

all: French.uid German.uid English.uid

French.uid: French.uil
	../../../clients/uil/uil -o $@ $? -I$(srcdir)/../../../clients/uil -I../../../clients/uil

German.uid: German.uil
	../../../clients/uil/uil -o $@ $? -I$(srcdir)/../../../clients/uil -I../../../clients/uil

English.uid: English.uil
	../../../clients/uil/uil -o $@ $? -I$(srcdir)/../../../clients/uil -I../../../clients/uil

clean-local:
	$(RM) French.uid German.uid English.uid
