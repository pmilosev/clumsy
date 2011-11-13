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
#include <signal.h>
#include "../clumsy.h"
#include "../cl_object_rep.h"

static bool destructor_called = false;
static cl_object_t *tested_object = NULL;
static void destructor(void * obj)
{
	fail_unless(obj == tested_object);
	destructor_called = true;
}

START_TEST(test_object_assert_size)
{
	cl_object_t *obj = cl_object_init(0, CL_OBJECT_TYPE_OBJECT, NULL);
}

END_TEST

START_TEST(test_object_comparator)
{
	cl_object_t *obj1 = cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL);
	cl_object_t *obj2 = cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL);
	if (obj1 > obj2) {
		void *temp = obj1;
		obj1 = obj2;
		obj2 = temp;
	}

	fail_unless(cl_object_comparator(&obj1, &obj1) == 0);
	fail_unless(cl_object_comparator(&obj2, &obj1) > 0);
	fail_unless(cl_object_comparator(&obj1, &obj2) < 0);

	cl_object_release(obj1);
	cl_object_release(obj2);
}

END_TEST

START_TEST(test_object_management)
{
	/* test object initialization */
	cl_object_t *obj =
	    cl_object_init(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL);
	fail_unless(obj->_obj_info._ref == 0);
	fail_unless(obj->_obj_info._dest == NULL);

	/* test object type check */
	fail_if(cl_object_type_check(NULL, CL_OBJECT_TYPE_OBJECT));
	obj->_obj_info._MAGIC ^= 0x01L;
	fail_if(cl_object_type_check(obj, CL_OBJECT_TYPE_OBJECT));
	obj->_obj_info._MAGIC ^= 0x01L;
	fail_unless(cl_object_type_check(obj, CL_OBJECT_TYPE_OBJECT));

	/* test object retain */
	fail_unless(cl_object_retain(NULL) == NULL);
	fail_unless(cl_object_retain(obj) == obj);
	cl_object_retain(obj);
	fail_unless(obj->_obj_info._ref == 2);

	/* test regular object release */
	fail_unless(cl_object_release(obj) == obj);
	fail_unless(obj->_obj_info._ref == 1);
	fail_unless(cl_object_release(obj) == NULL);

	/* test non-retained object release */
	obj = cl_object_init(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL);
	fail_unless(cl_object_release(obj) == NULL);

	/* test object destruction */
	obj =
	    cl_object_init(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT,
			   &destructor);
	tested_object = obj;
	destructor_called = false;
	fail_unless(cl_object_release(obj) == NULL);
	fail_unless(destructor_called);
}

END_TEST

Suite *test_suite(void)
{
	Suite *s = suite_create("TEST OBJECT");

	TCase *tc_core = tcase_create("TEST_OBECT");
	tcase_add_test_raise_signal(tc_core, test_object_assert_size, SIGABRT);
	tcase_add_test(tc_core, test_object_management);
	tcase_add_test(tc_core, test_object_comparator);
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
