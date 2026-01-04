MAINTAINERCLEANFILES = Makefile.in
EXTRA_DIST           = README

bindir  = ${prefix}/share/Xm/wsm
datadir = ${prefix}/share/Xm/wsm
mandir  = ${prefix}/share/Xm/wsm

data_DATA        = command_ui.h wsm_cb.h wsm_create.h wsmData.h wsmDebug.h \
                   wsm.h wsmSend.h wsmStruct.h wsm_ui.h xrmLib.h           \
                   $(wsm_SOURCES) $(EXTRA_DIST)
man_MANS         = wsm.man
bin_PROGRAMS     = wsm
wsm_SOURCES      = client_win.c command_ui.c wsm.c wsmData.c wsmDebug.c \
                   wsmSend.c wsmStruct.c wsm_cb.c wsm_create.c xrmLib.c
AM_CPPFLAGS      = -I$(top_srcdir)/demos/lib $(X_CFLAGS)
DEFAULT_INCLUDES = -I. -I$(top_srcdir)/include -I$(top_srcdir)/lib \
                   -I$(top_builddir)/include -I$(top_builddir)/lib
wsm_LDADD        = $(top_builddir)/demos/lib/Wsm/libWsm.a \
                   $(top_builddir)/lib/Xm/libXm.la

