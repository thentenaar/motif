MAINTAINERCLEANFILES = Makefile.in Uil.cat UilMsgCatI.h

EXTRA_DIST = Uil.msg
#
# I don't seem to have all the pieces to make this thing
#
bin_PROGRAMS = uil

libUil_la_LDFLAGS = -version-info @ABI_VERSION@

UilParser.lo: UilParser.c
	$(LTCOMPILE) -DCALLABLE -c UilParser.c

libUil_la_LIBADD = UilParser.lo ../../lib/Mrm/libMrm.la ../../lib/Xm/libXm.la

uil_LDADD = ../../lib/Mrm/libMrm.la ../../lib/Xm/libXm.la

AM_CPPFLAGS = -DINCDIR=\"@INCDIR@\" \
              -DLIBDIR=\"@LIBDIR@\" \
              -I$(srcdir)/../../include \
              -I$(srcdir)/../../lib \
              -I$(srcdir)/../../tools/wml \
              -I../../include \
              -I../../lib \
              -I../../tools/wml ${X_CFLAGS}

UilParser.c:
	test -f UilParser.c || $(LN_S) $(srcdir)/UilMain.c UilParser.c

UilLexPars.c: $(top_builddir)/tools/wml/Uil.c
	test -f UilLexPars.c || $(LN_S) $(top_builddir)/tools/wml/Uil.c UilLexPars.c

BUILT_SOURCES = UilParser.c UilLexPars.c UilDBDef.h

CLEANFILES = UilParser.c UilLexPars.c UilDBDef.h

COMMON_SRC =	UilCmd.c	UilDiags.c	UilKeyTab.c	UilLexAna.c  \
		UilLexPars.c	UilLstLst.c	UilP2Out.c	UilP2Reslv.c \
		UilSarComp.c	UilSarExp.c	UilSarInc.c	UilSarMod.c \
		UilSarObj.c	UilSarProc.c	UilSarVal.c	UilSrcSrc.c  \
		UilSymNam.c	UilSymStor.c	UilData.c	\
		UilLstMac.c	UilSemVal.c	UilSemCSet.c	UilDB.c

SRCS =		$(COMMON_SRC)
SRCS2 =		$(COMMON_SRC)	UilMain.c

HEADERS_1 = 	Uil.h		UilSymGl.h	UilSymDef.h \
		UilDef.h	XmAppl.uil

uildir = $(includedir)/uil

uil_HEADERS = $(HEADERS_1)

noinst_HEADERS = UilCmdDef.h \
UilCompGl.h \
UilDefI.h \
UilDiagDef.h \
UilIODef.h \
UilLexDef.h \
UilMess.h \
UilMessTab.h \
UilSarDef.h \
UilSrcDef.h

lib_LTLIBRARIES = libUil.la

uil_SOURCES = $(SRCS2)
uil_CFLAGS = $(AM_CFLAGS)

libUil_la_SOURCES = $(SRCS)
libUil_la_DEPENDENCIES = UilDBDef.h UilParser.lo

UilDBDef.h:
	$(LN_S) $(srcdir)/../../tools/wml/UilDBDef.h UilDBDef.h

if XM_MSGCAT
$(COMMON_SRC): $(CAT_FILES)

CAT_FILES = Uil.cat UilMsgCatI.h

$(CAT_FILES): Uil.msg
	$(mkinstalldirs) $(top_builddir)/localized/C/msg
	$(top_builddir)/localized/util/mkcatdefs UilMsgCatI.h $(srcdir)/Uil.msg >$(top_builddir)/localized/C/msg/Uil.msg
	gencat Uil.cat $(top_builddir)/localized/C/msg/Uil.msg
endif

