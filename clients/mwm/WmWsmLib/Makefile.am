MAINTAINERCLEANFILES = Makefile.in

SRCS = debug.c disp.c free.c pack.c recv.c send.c util.c utm_send.c

noinst_HEADERS = utm_send.h	wsm_funcs.h	wsm_proto.h

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib $(X_CFLAGS)

libWsm_a_SOURCES = $(SRCS)

noinst_LIBRARIES = libWsm.a
