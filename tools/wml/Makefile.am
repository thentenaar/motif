MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = README wmllex.c wmllex.l motif.wml UilLexPars.y UilLexPars.h UilLexPars.c

AM_YFLAGS = -d

WMLTARGETS = UilConst.h	UilKeyTab.h	 \
	UilSymArTa.h	UilSymArTy.h	UilSymCSet.h	UilSymCtl.h \
	UilSymEnum.h	UilSymGen.h	UilSymNam.h	UilSymRArg.h \
	UilSymReas.h	UilTokName.h	UilUrmClas.h	UilSymChCl.h \
	UilSymChTa.h

TABLE = motif.wml

CLEANFILES = $(WMLTARGETS) motif.wmd wml.report tokens.dat UilLexPars.[ch] wmllex.c wmlparse.[ch] wmluiltok.c wml-uil.mm

wmldir = $(includedir)/uil

wml_HEADERS = UilDBDef.h

noinst_DATA = motif.wmd

noinst_HEADERS = wml.h wmlparse.h wmlnoreturn.h $(WMLTARGETS)

noinst_PROGRAMS = wml wmluiltok wmldbcreate
wmluiltok_SOURCES = wmluiltok.l

wml_SOURCES = wml.c
wml_LDADD = libwml.la
wmldbcreate_LDADD = ../../lib/Xm/libXm.la $(X_LIBS)

AM_CPPFLAGS = -I../../lib -I$(srcdir)/../../lib -I$(srcdir)/../../include $(X_CFLAGS) -I../../clients/uil

noinst_LTLIBRARIES = libwml.la
libwml_la_SOURCES = wmlparse.y wmloutkey.c wmlouth.c wmloutmm.c wmloutp1.c wmlresolve.c wmlsynbld.c wmlutils.c

wmlparse.c: wmllex.c

wmllex.c: wmllex.l

wmlsynbld.c: wmlparse.h

@srcdir@/wmldbcreate.c: $(WMLTARGETS) UilLexPars.c

$(WMLTARGETS): wml-uil.mm

wml-uil.mm: wmluiltok wml $(TABLE)
	./wmluiltok <$(srcdir)/UilLexPars.y >tokens.dat
	./wml $(srcdir)/$(TABLE)

motif.wmd: wmldbcreate
	./wmldbcreate -o motif.wmd
