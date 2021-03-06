#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_COPYRIGHT([Copyright 2002, 2003 Edward T. Smith.  All rights reserved.

Copyright 2003, 2004, 2005 The Open Knights Consortium.  All rights reserved.])
AC_INIT([NWN Tools], [2.3.2], [sumpfork@users.sourceforge.net], [nwntools])
AC_SUBST(RELEASE, 1)
AC_CONFIG_SRCDIR(_NwnLib/NwnDefines.h)
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([1.7 gnits dist-bzip2])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_YACC

# Check bison version, substitute name for yychar
bison_version=`bison -V | grep "^bison" | awk '{print $4}'`
AC_MSG_RESULT([checking bison version: $bison_version])

case $bison_version in
    1.*)
        AC_MSG_ERROR([need a bison version >= 2.0])
        ;;
    2.0|2.0.*)
        AC_MSG_RESULT([setting bison lalr1 yychar variable name to yylooka_])
        AC_DEFINE(YYCHAR_NAME,yylooka_,[name of yychar lookahead var in bison lalr1.cc])
        ;;
    2.*)
        AC_MSG_RESULT([setting bison lalr1 yychar variable name to yylooka])
        AC_DEFINE(YYCHAR_NAME,yylooka,[name of yychar lookahead var in bison lalr1.cc])
        ;;
    *)
        AC_MSG_RESULT([setting bison lalr1 to use symbols])
        ;;
esac

# Check for python
AM_PATH_PYTHON(2.2,, :)

AC_ARG_VAR(EBUILD, [ebuild parsing command])
AC_PATH_PROG(EBUILD, [ebuild], [], [/usr/sbin:/usr/bin:/usr/lib/portage/bin])
AM_CONDITIONAL(EBUILD, [test -n "$EBUILD"])

AC_ARG_VAR(RPMBUILD, [RPM building utility])
AC_CHECK_PROG(RPMBUILD, [rpmbuild], [rpmbuild])
AM_CONDITIONAL(RPMBUILD, [test -n "$RPMBUILD"])

#AM_OPTIONS_WXCONFIG
#AM_PATH_WXCONFIG(2.3.4, [wxwin=1])
#if test "$wxwin" != 1 ; then
#	AC_MSG_ERROR([
#		wxWindows must be installed on your system
#		but wx-config script couldn't be found.

#		Please check that wx-config is in path, the directory
#		where wxWindows libraries are installed (returned by
#		'wx-config --libs' command) is in LD_LIBRARY_PATH or
#		equivalent variable and wxWindows version is 2.3.4 or above.
#	])
#fi

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
                 nwnnsscomp/python/Makefile
                 nwnmdlcomp/Makefile])
AC_OUTPUT

