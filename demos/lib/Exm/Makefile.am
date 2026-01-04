MAINTAINERCLEANFILES = Makefile.in
SUBDIRS              = wml

noinst_HEADERS	     = CommandB.h CommandBP.h ExmMrm.h Grid.h GridP.h   \
                       MenuB.h MenuBP.h Panner.h PannerP.h Simple.h     \
					   SimpleP.h ExmString.h ExmStringP.h StringTrans.h \
					   StringTransP.h TabB.h TabBP.h
noinst_LIBRARIES     = libExm.a libExmMrm.a
libExm_a_SOURCES     = CommandB.c Grid.c Panner.c MenuB.c Simple.c \
                       ExmString.c StringTrans.c TabB.c
libExmMrm_a_SOURCES  = ExmMrm.c
AM_CPPFLAGS          = -I$(srcdir)/.. $(X_CFLAGS)
DEFAULT_INCLUDES     = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                       -I$(top_builddir)/include -I$(top_builddir)/lib

