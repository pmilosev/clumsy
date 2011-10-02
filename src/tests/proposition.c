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
#include "../cl_proposition_rep.h"

static bool is_grater_than(cl_proposition * proposition)
{
	fail_if(proposition == NULL);
	cl_proposition_context *ctx = cl_proposition_get_context(proposition);

	int *data = (int *)(ctx->argv[0]);
	return data[0] > data[1];
}

START_TEST(test_atomic_proposition)
{
	/* constant FALSE proposition */
	cl_proposition *p = cl_proposition_new(NULL, NULL);
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	cl_object_release((cl_object *) p);

	/* FALSE proposition */
	int data[2] = { 1, 2 };
	p = cl_proposition_new(&is_grater_than, &data);
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));

	/* TRUE for the current state / interpretation */
	data[0] = 3;
	fail_unless(cl_proposition_eval(p));
	cl_object_release((cl_object *) p);
}

END_TEST;

Suite *test_suite(void)
{
	Suite *s = suite_create("TEST PROPOSITIONS");

	TCase *tc_core = tcase_create("TEST_PROPOSITION");
	tcase_add_test(tc_core, test_atomic_proposition);
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
