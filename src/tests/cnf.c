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
#include <stdio.h>
#include "../clumsy.h"
#include "../cl_cnf_rep.h"

static bool is_grater_than(cl_proposition_t * proposition)
{
	fail_if(proposition == NULL);
	cl_proposition_context_t *ctx = cl_proposition_get_context(proposition);

	int *data = (int *)(ctx->argv[0]);
	return data[0] > data[1];
}

static void check_cnf(cl_cnf_t * cnf)
{
	/* get a set of all the literals: {P, ~Q, ~R, M} */
	cl_collection_t *literals = cl_object_retain(cl_cnf_literals(cnf));
	fail_if(literals == NULL);
	fail_unless(cl_collection_count(literals) == 4);

	/* The formula should be true only for assignments
	 * for which P, ~Q and M are all true */
	cl_cnf_literal_t *l0 = cl_collection_get(literals, 0);
	cl_cnf_literal_t *l1 = cl_collection_get(literals, 1);
	cl_cnf_literal_t *l2 = cl_collection_get(literals, 2);
	cl_cnf_literal_t *l3 = cl_collection_get(literals, 3);
	int count = 0;
	for (int i = 0; i < 16; i++) {
		cl_cnf_literal_assign(l0, i & 0x01);
		cl_cnf_literal_assign(l1, i & 0x02);
		cl_cnf_literal_assign(l2, i & 0x04);
		cl_cnf_literal_assign(l3, i & 0x08);

		count += cl_cnf_evaluate(cnf) ? 1 : 0;
	}

	fail_unless(count == 2);
	cl_object_release(literals);
}

void setup()
{
	cl_object_pool_push();
}

void teardown()
{
	cl_object_pool_pop();
}

START_TEST(test_cnf)
{
	/* explicitly push a new autorelease pool 
	 * as some literlas are made as negations of an
	 * autoreleased ones.
	 * They will only be converted into non-negations
	 * after the autoreleased literals get deallocated */
	cl_object_pool_push();

	/* new CNF */
	cl_cnf_t *cnf = cl_cnf_new();

	/* test: P ^ ~Q ^ (~R v ~~R) ^ (~P v M) */
	cl_cnf_literal_t *p = cl_cnf_literal_new();
	cl_cnf_literal_t *m = cl_cnf_literal_new();

	/* add P */
	fail_unless(cl_cnf_add(cnf, cl_cnf_clause(1, p)));

	/* add ~Q */
	fail_unless(cl_cnf_add
		    (cnf,
		     cl_cnf_clause(1, cl_cnf_literal_not(cl_cnf_literal()))));

	/* add ~R v ~~R */
	cl_collection_t *clause =
	    cl_collection_new(2, CL_OBJECT_TYPE_CNF_LITERAL, 0);
	cl_collection_add(clause, cl_cnf_literal_not(cl_cnf_literal()));
	cl_collection_add(clause,
			  cl_cnf_literal_not(cl_collection_check(clause)));
	cl_collection_flag_set(clause, CL_COLLECTION_FLAG_UNIQUE);
	fail_unless(cl_collection_count(clause) == 2);
	fail_unless(cl_cnf_add(cnf, clause));
	cl_object_release(clause);

	/* add ~P v M */
	fail_unless(cl_cnf_add
		    (cnf, cl_cnf_clause(2, cl_cnf_literal_not(p), m)));

	/* test automatic value assign */
	cl_cnf_literal_t *notp = cl_cnf_literal_not(p);
	cl_cnf_literal_assign(p, true);
	fail_unless(cl_cnf_literal_value(p) == true);
	fail_unless(cl_cnf_literal_value(notp) == false);
	cl_cnf_literal_assign(p, false);
	fail_unless(cl_cnf_literal_value(p) == false);
	fail_unless(cl_cnf_literal_value(notp) == true);

	cl_object_release(p);
	cl_object_release(m);

	/* here we flush the autorelease pool to convert all negations,
	 * for which the original literal was autoreleased,
	 * into proper non-negation literals. */
	cl_object_pool_pop();

	check_cnf(cnf);
	cl_object_release(cnf);
}

END_TEST START_TEST(test_cnf_proposition)
{
	/* test: P ^ ~Q ^ (~R v ~~R) ^ (~P v M) ^ P 
	 * P ^ (~(Q v R) v (~Q ^ R)) ^ (~(P v ~P) v (M ^ P))
	 * P ^ ((Q v R) => (~Q ^ R)) ^ ((P <= P) => ~(M => P)) */

	/* propositions with same function and data poiters are counted as one */
	int data_p[2] = { 2, 3 };
	int data_q[2] = { 2, 3 };
	int data_r[2] = { 2, 3 };
	int data_m[2] = { 2, 3 };
	cl_proposition_t *p = cl_proposition_new(&is_grater_than, &data_p);
	cl_proposition_t *pdup = cl_proposition_new(&is_grater_than, &data_p);
	cl_proposition_t *q = cl_proposition_new(&is_grater_than, &data_q);
	cl_proposition_t *r = cl_proposition_new(&is_grater_than, &data_r);
	cl_proposition_t *m = cl_proposition_new(&is_grater_than, &data_m);

	/* *INDENT-OFF* */
	cl_cnf_t * cnf = cl_object_retain(cl_cnf_construct(
				cl_proposition_and(
					p,
					cl_proposition_and(
						cl_proposition_imply(
							cl_proposition_or(
								q,
								r),
							cl_proposition_and(
								cl_proposition_not(q),
								r)),
						cl_proposition_imply(
							cl_proposition_implied(
								p,
								pdup),
							cl_proposition_not(
								cl_proposition_imply(
									m,
									pdup)))))));
	/* *INDENT-ON* */

	check_cnf(cnf);
	cl_object_release(cnf);
}

END_TEST START_TEST(test_printer)
{
	cl_cnf_t *cnf = cl_cnf();
	cl_cnf_literal_t *lit = cl_cnf_literal();
	cl_cnf_add(cnf, cl_cnf_clause(1, lit));
	cl_cnf_literal_assign(lit, true);

	char *str = cl_object_to_string(cnf);
	char *expected = malloc(sizeof(char) * 64);
	fail_if(expected == NULL);
	sprintf(expected, "{{[%p : TRUE]}}", lit);

	fail_unless(strcmp(expected, str) == 0);
	free(str);
	free(expected);
}

END_TEST Suite *test_suite(void)
{
	Suite *s = suite_create("TEST CNF");

	TCase *tc_core = tcase_create("TEST_CNF");
	tcase_add_checked_fixture(tc_core, setup, teardown);
	tcase_add_test(tc_core, test_cnf);
	tcase_add_test(tc_core, test_printer);
	//tcase_add_test(tc_core, test_cnf_proposition);
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
