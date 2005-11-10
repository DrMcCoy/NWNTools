
AM_CXXFLAGS = -fno-default-inline

AM_YFLAGS = --defines=NscParser.h --skeleton=lalr1.cc

noinst_LIBRARIES = libnsc.a

libnsc_a_SOURCES = \
	NscCodeGenerator.cpp \
	NscCompiler.cpp \
	NscContext.cpp \
	NscDecompiler.cpp \
	NscParserRoutines.cpp \
	NscPStackEntry.cpp \
	Nsc.h \
	NscCodeGenerator.h \
	NscContext.h \
	NscPStackEntry.h \
	NscSymbolTable.h \
	NscParser.ypp

BUILT_SOURCES = \
	NscParser.cpp \
	NscParser.h

libnsc_a_LIBADD = @LIBOBJS@

EXTRA_DIST = \
	NscParser.h \
	_NscLib.vcproj

MAINTAINERCLEANFILES = \
	Makefile.in

