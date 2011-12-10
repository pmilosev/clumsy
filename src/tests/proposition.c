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

static bool is_grater_than(cl_proposition_t * proposition)
{
	fail_if(proposition == NULL);
	cl_proposition_context_t *ctx = cl_proposition_get_context(proposition);

	int *data = (int *)(ctx->argv[0]);
	return data[0] > data[1];
}

void setup()
{
	cl_object_pool_push();
}

void teardown()
{
	cl_object_pool_pop();
}

START_TEST(test_atomic_proposition)
{
	/* try evaluating NULL proposition */
	fail_if(cl_proposition_eval(NULL));

	/* constant TRUE propositions */
	cl_proposition_t *p = cl_object_retain(cl_proposition_true());
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* constant FALSE propositions */
	p = cl_object_retain(cl_proposition_false());
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* custom constant FALSE proposition */
	p = cl_proposition_new(NULL, NULL);
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* Create a proposition */
	int data[2] = { 1, 2 };
	p = cl_proposition_new(&is_grater_than, &data);
	fail_if(p == NULL);

	/* Check the context */
	cl_proposition_context_t *ctx = cl_proposition_get_context(NULL);
	fail_unless(ctx == NULL);
	ctx = cl_proposition_get_context(p);
	fail_if(ctx == NULL);
	fail_unless(ctx->op == &is_grater_than);
	fail_unless(ctx->argv[0] == &data);
	fail_unless(ctx->argv[1] == NULL);

	/* FALSE for current state */
	fail_if(cl_proposition_eval(p));

	/* TRUE in the next state */
	data[0] = 3;
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);
}

END_TEST START_TEST(test_complex_proposition)
{
	cl_proposition_t *p = NULL;

	/* false */
	cl_proposition_t *pfalse = cl_proposition_new(NULL, NULL);
	fail_if(pfalse == NULL);
	fail_if(cl_proposition_eval(pfalse));

	/* not false */
	p = cl_object_retain(cl_proposition_not(pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* not true */
	p = cl_object_retain(cl_proposition_not(cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true and true */
	p = cl_object_retain(cl_proposition_and
			     (cl_proposition_not(pfalse),
			      cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true and false */
	p = cl_object_retain(cl_proposition_and
			     (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false and true */
	p = cl_object_retain(cl_proposition_and
			     (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false and false */
	p = cl_object_retain(cl_proposition_and(pfalse, pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true or true */
	p = cl_object_retain(cl_proposition_or
			     (cl_proposition_not(pfalse),
			      cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true or false */
	p = cl_object_retain(cl_proposition_or
			     (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false or true */
	p = cl_object_retain(cl_proposition_or
			     (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false or false */
	p = cl_object_retain(cl_proposition_or(pfalse, pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true imply true */
	p = cl_object_retain(cl_proposition_imply
			     (cl_proposition_not(pfalse),
			      cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true imply false */
	p = cl_object_retain(cl_proposition_imply
			     (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false imply true */
	p = cl_object_retain(cl_proposition_imply
			     (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false imply false */
	p = cl_object_retain(cl_proposition_imply(pfalse, pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true impled by true */
	p = cl_object_retain(cl_proposition_implied
			     (cl_proposition_not(pfalse),
			      cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true impled by false */
	p = cl_object_retain(cl_proposition_implied
			     (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false impled by true */
	p = cl_object_retain(cl_proposition_implied
			     (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false impled by false */
	p = cl_object_retain(cl_proposition_implied(pfalse, pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true equivalent to true */
	p = cl_object_retain(cl_proposition_equivalent
			     (cl_proposition_not(pfalse),
			      cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true equivalent to false */
	p = cl_object_retain(cl_proposition_equivalent
			     (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false equivalent to true */
	p = cl_object_retain(cl_proposition_equivalent
			     (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false equivalent to false */
	p = cl_object_retain(cl_proposition_equivalent(pfalse, pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true xor true */
	p = cl_object_retain(cl_proposition_xor
			     (cl_proposition_not(pfalse),
			      cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true xor flase */
	p = cl_object_retain(cl_proposition_xor
			     (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false xor true */
	p = cl_object_retain(cl_proposition_xor
			     (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false xor flase */
	p = cl_object_retain(cl_proposition_xor(pfalse, pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true nand true */
	p = cl_object_retain(cl_proposition_nand
			     (cl_proposition_not(pfalse),
			      cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true nand false */
	p = cl_object_retain(cl_proposition_nand
			     (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false nand true */
	p = cl_object_retain(cl_proposition_nand
			     (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false nand false */
	p = cl_object_retain(cl_proposition_nand(pfalse, pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true nor true */
	p = cl_object_retain(cl_proposition_nor
			     (cl_proposition_not(pfalse),
			      cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true nor false */
	p = cl_object_retain(cl_proposition_nor
			     (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false nor true */
	p = cl_object_retain(cl_proposition_nor
			     (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false nor false */
	p = cl_object_retain(cl_proposition_nor(pfalse, pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true not imply true */
	p = cl_object_retain(cl_proposition_nimply
			     (cl_proposition_not(pfalse),
			      cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true not imply false */
	p = cl_object_retain(cl_proposition_nimply
			     (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false not imply true */
	p = cl_object_retain(cl_proposition_nimply
			     (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false not imply false */
	p = cl_object_retain(cl_proposition_nimply(pfalse, pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true not impled by true */
	p = cl_object_retain(cl_proposition_nimplied
			     (cl_proposition_not(pfalse),
			      cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* true not impled by false */
	p = cl_object_retain(cl_proposition_nimplied
			     (cl_proposition_not(pfalse), pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false not impled by true */
	p = cl_object_retain(cl_proposition_nimplied
			     (pfalse, cl_proposition_not(pfalse)));
	fail_if(p == NULL);
	fail_unless(cl_proposition_eval(p));
	p = cl_object_release(p);

	/* false not impled by false */
	p = cl_object_retain(cl_proposition_nimplied(pfalse, pfalse));
	fail_if(p == NULL);
	fail_if(cl_proposition_eval(p));
	p = cl_object_release(p);

	pfalse = cl_object_release(pfalse);
}

END_TEST Suite *test_suite(void)
{
	Suite *s = suite_create("TEST PROPOSITIONS");

	TCase *tc_core = tcase_create("TEST_PROPOSITION");
	tcase_add_checked_fixture(tc_core, setup, teardown);
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
