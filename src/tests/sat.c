/*
 *   Copyright (C) 2011  Pece Milosev
 *
 *   This file is part of 'clumsy'.
 *   'clumsy' is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   'clumsy' is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <check.h>
#include <stdlib.h>
#include "../clumsy.h"
#include "../cl_sat_rep.h"

void setup()
{
	cl_object_pool_push();
}

void teardown()
{
	cl_object_pool_pop();
}

START_TEST(test_sat)
{
	/* CNF: P ^ Q*/
	cl_cnf_t *cnf = cl_cnf();
	cl_cnf_add(cnf, cl_cnf_clause(1, cl_cnf_literal()));
	cl_cnf_add(cnf, cl_cnf_clause(1, cl_cnf_literal()));

	/* New SAT solver */
	cl_sat_t *sat = cl_sat();
	sat->_maxflips = 100;

	/* Run the solver */
	cl_collection_t *sollution = cl_sat_solve(sat, cnf);
	fail_unless(cl_cnf_evaluate(cnf) == ((bool) sollution));
}

END_TEST Suite *test_suite(void)
{
	Suite *s = suite_create("TEST SAT");

	TCase *tc_core = tcase_create("TEST_SAT");
	tcase_add_checked_fixture(tc_core, setup, teardown);
	tcase_add_test(tc_core, test_sat);
	suite_add_tcase(s, tc_core);

	return s;
}

int main(void)
{
	int number_failed;
	Suite *s = test_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
