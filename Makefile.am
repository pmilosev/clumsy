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

CFLAGS = @CFLAGS@ --pedantic --std=c99
# DEFINE THE MAIN GOAL
lib_LTLIBRARIES = libclumsy.la
libclumsy_la_SOURCES = src/clumsy.h\
					   src/cl_proposition.h src/cl_proposition.c
libclumsy_la_HEADERS = src/clumsy.h
libclumsy_ladir = .

# SETUP THE UNIT TESTS
check_PROGRAMS = src/tests/proposition.test

src_tests_proposition_test_SOURCES = src/clumsy.h\
									 src/tests/proposition.c
src_tests_proposition_test_CFLAGS = @CHECK_CFLAGS@
src_tests_proposition_test_LDADD = libclumsy.la @CHECK_LIBS@

# skip unit tests if check is not installed
if SKIP_CHECK
TESTS=
else
TESTS=$(check_PROGRAMS)
endif

# ENABLE VALGRIND SUPPORT
if USE_VALGRIND
TESTS_ENVIRONMENT=src/tests/valgrind-wrapper.sh
endif
EXTRA_DIST=src/tests/valgrind-wrapper.sh

# ENABLE STATIC ANALYZER SUPPORT
# analyze all .[ch] files under the ./src and its subdirs, except the tests
if USE_SPLINT
analyze:
	@splint `find ./src -type f -name '*.[ch]' | grep -v './src/tests'`
endif

# CLEAN ALL FILES THAT CAN BE GENERATED WITH AUTORECONF
autoclean: distclean
	rm -rf src/Makefile.in
	rm -rf tests/Makefile.in
	rm -rf ./Makefile.in
	rm -rf ./aclocal.m4
	rm -rf ./autom4te.cache
	rm -rf ./compile
	rm -rf ./config.h
	rm -rf ./config.h.in
	rm -rf ./config.log
	rm -rf ./config.status
	rm -rf ./configure
	rm -rf ./depcomp
	rm -rf ./install-sh
	rm -rf ./missing
	rm -rf ./.deps
	rm -rf ./config.guess
	rm -rf ./config.h.in~
	rm -rf ./config.sub
	rm -rf ./ltmain.sh

