## Makefile.am -- Process this file with automake to produce Makefile.in

auxdir          = @ac_aux_dir@
AUX_DIST        = $(auxdir)/install-sh $(auxdir)/missing $(auxdir)/mkinstalldirs
AUX_DIST_EXTRA  = $(auxdir)/ac_lib_readline.m4 $(auxdir)/adl_func_getopt_long.m4

EXTRA_DIST      = bootstrap

SUBDIRS         = common lib cli

MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure $(AUX_DIST)

ACLOCAL         = aclocal -I $(auxdir)

dist-hook:
	(cd $(disdir) && mkdir $(auxdir))
	for file in $(AUX_DIST) $(AUX_DIST_EXTRA); do \
	    cp $$file $(distdir)/$$file; \
	done
