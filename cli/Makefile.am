## Makefile.am -- Process this file with automake to produce Makefile.in

INCLUDES        = -I$(top_builddir)/lib -I$(top_builddir)/common

bin_PROGRAMS        = juken exporter importer
juken_SOURCES   = cli.cpp juken.cpp consolelistener.cpp
juken_LDADD     = $(top_builddir)/lib/libjuken.a $(top_builddir)/common/libcommon.a

exporter_SOURCES   = export.cpp exporter.cpp xmlparser.cpp changerdata.cpp logginglistener.cpp
exporter_LDADD     = -lexpat $(top_builddir)/lib/libjuken.a $(top_builddir)/common/libcommon.a

importer_SOURCES   = import.cpp importer.cpp xmlparser.cpp changerdata.cpp logginglistener.cpp
importer_LDADD     = -lexpat $(top_builddir)/lib/libjuken.a $(top_builddir)/common/libcommon.a
