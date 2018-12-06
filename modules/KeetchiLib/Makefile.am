SUBDIRS = lib src/testapp

# additional files for package
EXTRA_DIST = bootstratp.sh
EXTRA_DIST += Doxyfile.in

ACLOCAL_AMFLAGS=-I m4

# Add make targets for doxygen
@DX_RULES@
MOSTLYCLEANFILES = $(DX_CLEANFILES)
MOSTLYCLEANFILES+= $(DOT_CLEANFILES)
