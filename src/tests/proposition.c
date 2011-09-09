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

bool 
is_grater_than (cl_proposition * proposition)
{
    fail_unless(proposition);
    fail_unless(proposition->func == &is_grater_than);
    fail_unless(proposition->data);
    
    int * data = (int *) proposition->data;
    return data[0] > data[1];
}

START_TEST (test_simple_proposition)
{
    /* constant FALSE proposition */
    cl_proposition * p1 = cl_proposition_new(NULL, NULL);
    fail_unless(p1);
    fail_if(cl_proposition_eval(p1));
    cl_proposition_destroy(p1);

    /* FALSE proposition */
    int data[2] = {1, 2};
    p1 = cl_proposition_new(&is_grater_than, data);
    fail_unless(p1);
    fail_if(cl_proposition_eval(p1));

    /* TRUE for the current state / interpretation */
    data[0] = 3;
    fail_unless(cl_proposition_eval(p1));
    cl_proposition_destroy(p1);
}
END_TEST

Suite *
test_suite (void)
{
    Suite * s = suite_create("TEST PROPOSITIONS");

    TCase * tc_core = tcase_create("TEST_SIMPLE_PROPOSITION");
    tcase_add_test(tc_core, test_simple_proposition);
    suite_add_tcase(s, tc_core);

    return s;
}

int 
main (void)
{
    int number_failed;
    Suite * s = test_suite();
    SRunner * sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
