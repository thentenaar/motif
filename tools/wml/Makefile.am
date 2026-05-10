MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = README wmllex.c wmllex.l motif.wml Uil.y Uil.c Uil.h UilLexPars.h UilLexPars.c

AM_YFLAGS = -d

WMLTARGETS = UilConst.h	UilKeyTab.h	 \
	UilSymArTa.h	UilSymArTy.h	UilSymCSet.h	UilSymCtl.h \
	UilSymEnum.h	UilSymGen.h	UilSymNam.h	UilSymRArg.h \
	UilSymReas.h	UilTokName.h	UilUrmClas.h	UilSymChCl.h \
	UilSymChTa.h

CLEANFILES = $(WMLTARGETS) motif.wmd wml.report tokens.dat UilLexPars.[ch] wmllex.c wmlparse.[ch] wmluiltok.c wml-uil.mm

TABLE = motif.wml

wmldir = $(includedir)/uil

wml_HEADERS = UilDBDef.h

noinst_HEADERS = wml.h wmlparse.h wmlnoreturn.h $(WMLTARGETS)

noinst_PROGRAMS = wml wmluiltok wmldbcreate

noinst_DATA = motif.wmd

wmluiltok_SOURCES = wmluiltok.l

wml_SOURCES = wml.c
wml_LDADD = libwml.la
wmldbcreate_LDADD = ../../lib/Xm/libXm.la $(X_LIBS)

AM_CPPFLAGS = -I../../lib -I$(srcdir)/../../lib -I$(srcdir)/../../include $(X_CFLAGS)

noinst_LTLIBRARIES = libwml.la
libwml_la_SOURCES = wmlparse.y wmloutkey.c wmlouth.c wmloutmm.c wmloutp1.c wmlresolve.c wmlsynbld.c wmlutils.c

wmlparse.c: wmllex.c

wmllex.c: wmllex.l

wmlsynbld.c: wmlparse.h

@srcdir@/wmldbcreate.c: $(WMLTARGETS) UilLexPars.h

UilLexPars.h: UilLexPars.c

UilLexPars.c: Uil.c
	$(LN_S) Uil.h UilLexPars.h
	$(LN_S) Uil.c UilLexPars.c

$(WMLTARGETS): wml-uil.mm

wml-uil.mm: wmluiltok wml UilLexPars.c $(TABLE)
	./wmluiltok <$(srcdir)/Uil.y >tokens.dat
	./wml $(srcdir)/$(TABLE)

motif.wmd: wmldbcreate
	./wmldbcreate -o motif.wmd
