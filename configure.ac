#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_COPYRIGHT([Copyright 2002, 2003 Edward T. Smith.  All rights reserved.

Copyright 2003 The Open Knights Consortium.  All rights reserved.])
AC_INIT([NWN Tools], [2.0.0], [eyrdan@users.sourceforge.net], [nwntools])
AC_SUBST(RELEASE, 1)
AC_CONFIG_SRCDIR(_NwnLib/NwnDefines.h)
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([1.7 gnits dist-bzip2])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB

AC_ARG_VAR(MSTA, [COCOM syntax description translator])
AC_CHECK_PROG(MSTA, [msta], [msta])

AC_ARG_VAR(EBUILD, [ebuild parsing command])
AC_PATH_PROG(EBUILD, [ebuild], [], [/usr/sbin:/usr/bin:/usr/lib/portage/bin])
AM_CONDITIONAL(EBUILD, [test -n "$EBUILD"])

AC_ARG_VAR(RPMBUILD, [RPM building utility])
AC_CHECK_PROG(RPMBUILD, [rpmbuild], [rpmbuild])
AM_CONDITIONAL(RPMBUILD, [test -n "$RPMBUILD"])

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC # memory.h, stdlib.h, string.h
AC_CHECK_HEADERS([malloc.h stddef.h])
AC_CHECK_HEADERS([libiberty.h libgen.h ctype.h assert.h stdarg.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_C_BIGENDIAN
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_CHECK_FUNCS([stricmp strcasecmp])
AC_CHECK_FUNCS([strnicmp strncasecmp])
AC_CHECK_FUNCS([_snprintf snprintf])
AC_CHECK_FUNCS([strlwr])
AC_CHECK_FUNCS([alloca malloc memcmp realloc free stdtod vprintf])
AC_CHECK_FUNCS([memmove memset pow sqrt strchr strrchr strtol basename])

AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([_MathLib/Makefile
                 _NscLib/Makefile
                 _NwnLib/Makefile
                 _NmcLib/Makefile
                 Makefile
		 nwntools.spec
                 nwnnsscomp/Makefile
                 nwnmdlcomp/Makefile])
                 
AC_OUTPUT

