MAINTAINERCLEANFILES = Makefile.in

bindir = ${prefix}/share/Xm/wsm
datadir = ${prefix}/share/Xm/wsm

EXTRA_DIST = README wsm.man
bin_PROGRAMS = wsm
wsm_SOURCES = client_win.c command_ui.c wsm.c wsmData.c wsmDebug.c \
              wsmSend.c wsmStruct.c wsm_cb.c wsm_create.c xrmLib.c \
              command_ui.h wsm_cb.h wsm_create.h wsmData.h wsmDebug.h \
              wsm.h wsmSend.h wsmStruct.h wsm_ui.h xrmLib.h
data_DATA = $(wsm_SOURCES) $(EXTRA_DIST)
AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(srcdir)/../../lib -I$(top_builddir)/lib ${X_CFLAGS}
LDADD = ../../../lib/Xm/libXm.la -L../../lib/Wsm -lWsm
