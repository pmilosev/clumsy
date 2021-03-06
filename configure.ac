#
#   Copyright (C) 2011  Pece Milosev
#
#   This file is part of 'clumsy'.
#   'clumsy' is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   'clumsy' is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

# COMMON autoconf AND automake CONFIG
AC_PREREQ([2.67])
AC_INIT([clumsy],[0.0.0],[https://github.com/pmilosev/clumsy/issues],[clumsy],[https://github.com/pmilosev/clumsy])
AC_CONFIG_SRCDIR([src/clumsy.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([subdir-objects])

LT_INIT
AC_PROG_CC
AM_PROG_CC_C_O

# SKIP UNIT TESTS IF CHECK IS NOT PRESENT
skip_check=true
PKG_CHECK_MODULES([CHECK], [check >= 0.9.8], 
                  [skip_check=false], 
                  [AC_MSG_WARN([Unit tests disabled.])])

AM_CONDITIONAL(SKIP_CHECK, $skip_check)

# ENABLE SPLINT SUPPORT
AC_CHECK_PROG(HAVE_SPLINT, splint, true, false)
AM_CONDITIONAL(USE_SPLINT, $HAVE_SPLINT)

# ENABLE INDENT SUPPORT
AC_CHECK_PROG(HAVE_INDENT, indent, true, false)
AM_CONDITIONAL(USE_INDENT, $HAVE_INDENT)

# ENABLE VALGRIND SUPPORT
use_valgrind=false
AC_ARG_ENABLE(valgrind,
              AS_HELP_STRING([--enable-valgrind], [Use valgrind when running unit tests.]),
              [use_valgrind=true])

if [[ "$use_valgrind" = "true" ]]; then
    AC_CHECK_PROG(HAVE_VALGRIND, valgrind, yes, no)

    if [[ "$HAVE_VALGRIND" = "no" ]]; then
        AC_MSG_ERROR([Valgrind not found in PATH.])
    fi
fi

AM_CONDITIONAL(USE_VALGRIND, $use_valgrind)

# ENABLE GCOV COVERAGE ANALYSIS SUPPORT
coverage=false
AC_ARG_ENABLE(coverage,
              AS_HELP_STRING([--enable-coverage], [Enable tests coverage statistics.]),
              [coverage=true])

if [[ "$coverage" = "true" ]]; then
    if [[ "$GCC" = "yes" ]]; then
        CFLAGS="-fprofile-arcs -ftest-coverage $CFLAGS"
    else
        AC_MSG_ERROR([Can only enable coverage when using gcc.])
    fi
fi

# SPECIFY THE MAKEFILES
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
