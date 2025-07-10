MAINTAINERCLEANFILES = Makefile.in

noinst_LIBRARIES = libWsm.a

libWsm_a_SOURCES = debug.c disp.c free.c pack.c recv.c send.c util.c utm_send.c

noinst_HEADERS = wsm_funcs.h wsm_proto.h utm_send.h

AM_CPPFLAGS = -I$(srcdir)/../../../lib -I$(top_builddir)/lib ${X_CFLAGS}
