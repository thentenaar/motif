MAINTAINERCLEANFILES = Makefile.in

EXTRA_DIST = exm_in_uil.uil

CLEANFILES = exm_in_uil.uid

#noinst_PROGRAMS = exm_in_uil
#exm_in_uil_DEPENDENCIES = exm_in_uil.uid

LDADD = -L../../../lib/Exm -lExmMrm -lExm  $(top_builddir)/lib/Mrm/libMrm.la

AM_CPPFLAGS = -I$(top_srcdir)/lib -I$(top_srcdir)/lib -I../../../lib -I$(top_builddir)/lib ${X_CFLAGS}

SUFFIXES = .uil .uid

.uil.uid:
	$(top_builddir)/clients/uil/uil -wmd ../../../lib/Exm/wml/Exm.wmd -I$(srcdir) -o $@ $<

#  EXMSRC        = ../../../lib/Exm
#  EXMINCLUDESRC	= $(MINCLUDESRC)
#  EXMLIB	= $(EXMSRC)/libExm.a
#  EXMMRMLIB	= $(EXMSRC)/libExmMrm.a
#  INCLUDES	= -I$(EXMINCLUDESRC)
#  DEPLIBS	= $(EXMMRMLIB) $(EXMLIB) MrmClientDepLibs
#LOCAL_LIBRARIES = $(EXMMRMLIB) $(EXMLIB) MrmClientLibs
#LOCAL_UILFLAGS  = -wmd $(EXMSRC)/wml/Exm.wmd
#
#  SRCS		= exm_in_uil.c
#  OBJS		= exm_in_uil.o
#
#MSimpleProgramTarget(exm_in_uil)
#MSimpleUidTarget(exm_in_uil)
#
#InstallManPage(exm_in_uil,$(MANDIR))
#InstallProgram(exm_in_uil,$(BINDIR))
#
#DependTarget()
