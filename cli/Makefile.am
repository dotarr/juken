## Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES        = -I$(top_builddir)/lib -I$(top_builddir)/common

bin_PROGRAMS        = juken-cli
juken_cli_SOURCES   = main.cpp juken.cpp exportlistener.cpp

juken_cli_LDADD     = $(top_builddir)/lib/libjuken.a $(top_builddir)/common/libcommon.a
