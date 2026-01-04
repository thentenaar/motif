MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = Exm.wml
BUILT_SOURCES        = wml-uil.mm
CLEANFILES           = $(BUILT_SOURCES) Exm.wmd wml.report tokens.dat      \
                       UilLexPars.c UilLexPars.h UilConst.h UilKeyTab.h    \
                       UilSymArTa.h UilSymArTy.h UilSymCSet.h UilSymCtl.h  \
                       UilSymEnum.h UilSymGen.h UilSymNam.h UilSymRArg.h   \
                       UilSymReas.h UilTokName.h UilUrmClas.h UilSymChCl.h \
                       UilSymChTa.h
WMLDIR               = $(top_srcdir)/tools/wml

noinst_PROGRAMS   = wmldbcreate
AM_CPPFLAGS       = -I$(srcdir)/../.. \
                    -I$(top_srcdir)/tools/wml \
                    -I$(top_builddir)/tools/wml \
                    -I$(top_builddir)/clients/uil \
                    $(X_CFLAGS)
DEFAULT_INCLUDES  = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                    -I$(top_builddir)/include -I$(top_builddir)/lib
wmldbcreate_LDADD = $(top_builddir)/lib/Xm/libXm.la

all-local: Exm.wmd

Exm.wmd: wmldbcreate
	./wmldbcreate -o $@

wml-uil.mm: $(WMLDIR)/Uil.y Exm.wml $(WMLDIR)/motif.wml $(top_builddir)/tools/wml/wmluiltok
	$(RM) Exm.h
	$(top_builddir)/tools/wml/wmluiltok < $(WMLDIR)/Uil.y > tokens.dat
	$(LN_S) $(srcdir)/Exm.wml ./Exm.h
	$(CPP) -I$(WMLDIR) Exm.h | $(top_builddir)/tools/wml/wml
	$(RM) Exm.h

