## Process this file with automake to produce Makefile.in

SUBDIRS = _MathLib _NwnLib _NscLib _NmcLib nwnnsscomp nwnmdlcomp

AM_CXXFLAGS = -fno-default-inline

CLEANFILES = \
	config.h.in~

EXTRA_DIST = \
	README-1.2.html \
	win32_config.h \
	NWNTools.sln \
	NWNTools.suo \
	Save \
	_ExpatLib \
	nwntreasure \
	nwnexplorer

dist-hook:
	rm -rf `find $(distdir) -type d -name CVS`

MAINTAINERCLEANFILES = \
	INSTALL \
	Makefile.in \
	aclocal.m4 \
	config.h.in \
	configure \
	depcomp \
	install-sh \
	missing \
	mkinstalldirs

