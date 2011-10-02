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
	cl_proposition_release(p);

	/* FALSE proposition */
	int data[2] = { 1, 2 };
	p = cl_proposition_new(&is_grater_than, &data);
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));

	/* TRUE for the current state / interpretation */
	data[0] = 3;
	fail_unless(cl_proposition_eval(p));
	cl_proposition_release(p);
}

END_TEST;

START_TEST(test_complex_proposition)
{
	cl_proposition *p = NULL;

	/* false */
	cl_proposition *pfalse = cl_proposition_new(NULL, NULL);
	fail_if(pfalse == NULL);
	fail_if(cl_proposition_eval(pfalse));

	/* not false */
	p = cl_proposition_retain(cl_proposition_not(pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* not true */
	p = cl_proposition_retain(cl_proposition_not
				  (cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* true and true */
	p = cl_proposition_retain(cl_proposition_and
				  (cl_proposition_not(pfalse),
				   cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* true and false */
	p = cl_proposition_retain(cl_proposition_and
				  (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* false and true */
	p = cl_proposition_retain(cl_proposition_and
				  (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* false and false */
	p = cl_proposition_retain(cl_proposition_and(pfalse, pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* true or true */
	p = cl_proposition_retain(cl_proposition_or
				  (cl_proposition_not(pfalse),
				   cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* true or false */
	p = cl_proposition_retain(cl_proposition_or
				  (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* false or true */
	p = cl_proposition_retain(cl_proposition_or
				  (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* false or false */
	p = cl_proposition_retain(cl_proposition_or(pfalse, pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* true imply true */
	p = cl_proposition_retain(cl_proposition_imply
				  (cl_proposition_not(pfalse),
				   cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* true imply false */
	p = cl_proposition_retain(cl_proposition_imply
				  (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* false imply true */
	p = cl_proposition_retain(cl_proposition_imply
				  (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	/* false imply false */
	p = cl_proposition_retain(cl_proposition_imply(pfalse, pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));

	p = cl_proposition_release(p);
	fail_unless(p == NULL);

	pfalse = cl_proposition_release(pfalse);
	fail_unless(pfalse == NULL);
}

END_TEST;

Suite *test_suite(void)
{
	Suite *s = suite_create("TEST PROPOSITIONS");

	TCase *tc_core = tcase_create("TEST_PROPOSITION");
	tcase_add_test(tc_core, test_atomic_proposition);
	tcase_add_test(tc_core, test_complex_proposition);
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
