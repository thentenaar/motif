MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = Exm.wml
CLEANFILES = Exm.wmd $(REPORT) $(WMLTARGETS) tokens.dat wml-uil.mm

AM_YFLAGS = -d

noinst_PROGRAMS = wmldbcreate

WMLDIR = $(top_srcdir)/tools/wml

AM_CPPFLAGS = -I$(top_srcdir)/lib \
        -I$(top_builddir)/lib \
	-I$(top_srcdir)/include \
	-I$(srcdir)/../.. \
	-I$(top_srcdir)/tools/wml \
	-I$(top_builddir)/tools/wml \
	-I$(top_builddir)/clients/uil \
	${X_CFLAGS}

LDADD = $(top_builddir)/lib/Xm/libXm.la

DISTCLEANFILES = motif.wmd

all: Exm.wmd

WMDTABLE = Exm.wmd

$(WMDTABLE): wmldbcreate $(TARGETS)
	./wmldbcreate -o $@


# INCLUDES = -I. -I$(INCLUDESRC) -I$(MINCLUDESRC) -I$(MLIBSRC) -I../../. -I$(MTOP)/include
# DEPLIBS = $(DEPXMLIB)  $(DEPXTOOLLIB) $(DEPXLIB)
# LOCAL_LIBRARIES = XmClientLibs
#
# WMLDIR = $(MTOP)/tools/wml
TABLE = Exm.wml
# WMDTABLE = Exm.wmd
# WMLTOOLS = $(WMLDIR)/wml $(WMLDIR)/wmluiltok
REPORT = wml.report
UILY = $(WMLDIR)/Uil.y
YACCTARGETS = UilLexPars.c	UilLexPars.h

WMLTARGETS = UilConst.h	UilKeyTab.h	 \
	UilSymArTa.h	UilSymArTy.h	UilSymCSet.h	UilSymCtl.h \
	UilSymEnum.h	UilSymGen.h	UilSymNam.h	UilSymRArg.h \
	UilSymReas.h	UilTokName.h	UilUrmClas.h	UilSymChCl.h \
	UilSymChTa.h

TARGETS = $(YACCTARGETS) $(WMLTARGETS)
#
# SRCS = wmldbcreate.c
#
# NormalLibraryObjectRule()
#
# all::   includes $(WMDTABLE)
#
# install:: all
#
UilLexPars.c UilLexPars.h: $(UILY)

$(TARGETS):	$(TABLE) $(WMLDIR)/motif.wml
	$(RM) $(TARGETS) $(REPORT) wml-uil.mm
# 	$(YACC) -d $(YACCFLAGS) $(UILY)
# 	$(MV) y.tab.c UilLexPars.c
# 	$(MV) y.tab.h UilLexPars.h
	$(top_builddir)/tools/wml/wmluiltok < $(UILY) > tokens.dat
# 	$(RM) wml-uil.mm
	$(RM) Exm.h
	$(LN_S) $(srcdir)/$(TABLE) ./Exm.h
	$(CPP) -I$(WMLDIR) Exm.h | $(top_builddir)/tools/wml/wml
	$(RM) Exm.h
# 	$(RM) tokens.dat
#
# $(WMDTABLE):	wmldbcreate
# 	$(CLIENTENVSETUP) ./wmldbcreate -o $@
#
# NormalProgramTarget(wmldbcreate,wmldbcreate.o,$(DEPLIBS),$(LOCAL_LIBRARIES),$(SYSLIBS))
#
# UilDBDef.h:    $(WMLDIR)/UilDBDef.h
# 	rm -f UilDBDef.h
# 	ln -s $(WMLDIR)/UilDBDef.h UilDBDef.h
#
# clean::
# 	$(RM) $(TARGETS) $(WMDTABLE) $(REPORT) \
# 	*.mm *.sdml *.txt tokens.dat *.h wmldbcreate UilLexPars.c *~
#
# DependTarget()
#
# includes:: $(TARGETS)  UilDBDef.h

.NOTPARALLEL:

