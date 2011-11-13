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
#include "../cl_collection_rep.h"

START_TEST(test_array)
{
	cl_object_t * obj[3] = {
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL),
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL),
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL)
	};

	/* test initialization */
	cl_collection_t *arr = cl_collection_new(3, CL_OBJECT_TYPE_OBJECT, 0);
	fail_if(arr == NULL);
	fail_unless(cl_object_type_check(arr, CL_OBJECT_TYPE_OBJECT));
	fail_unless(arr->_capacity == 3);
	fail_unless(arr->_chunk_size == 3);
	fail_unless(arr->_count == 0);
	fail_unless(arr->_flags == 0);
	fail_if(arr->_buffer == NULL);
		
	/* successfull addition */
	for (int i=0; i < 3; i++) {
		fail_unless(cl_collection_add(arr, obj[i]) == i);
		fail_unless(obj[i]->_obj_info._ref == 2);
	}

	/* there should be no space left */
	fail_unless(cl_collection_add(arr, obj[0]) == SIZE_MAX);

	/* test autoresize (expanding) */
	cl_collection_flag_set(arr, CL_COLLECTION_FLAG_AUTORESIZE);
	fail_unless(cl_collection_flag_check(arr, CL_COLLECTION_FLAG_UNLIMITED));
	fail_unless(cl_collection_add(arr, obj[0]) == 3);
	fail_unless(arr->_capacity == 6);
	fail_unless(arr->_chunk_size == 3);
	fail_unless(arr->_count == 4);

	/* test auto object retrieval */
	fail_unless(obj[0]->_obj_info._ref == 3);
	fail_unless(cl_collection_check(arr) == obj[0]);
	fail_unless(cl_collection_pick(arr) == obj[0]);

	fail_unless(obj[2]->_obj_info._ref == 2);
	fail_unless(cl_collection_pick(arr) == obj[2]);
	fail_unless(arr->_deleted == obj[2]);
	fail_unless(obj[0]->_obj_info._ref == 2);
	fail_unless(obj[2]->_obj_info._ref == 2);

	/* test autoresize (shrinking) */
	fail_unless(arr->_capacity == 3);
	fail_unless(arr->_chunk_size == 3);
	fail_unless(arr->_count == 2);

	/* test search */
	fail_unless(cl_collection_find(arr, obj[0]) == 0);
	fail_unless(cl_collection_find(arr, obj[1]) == 1);
	fail_unless(cl_collection_find(arr, obj[2]) == SIZE_MAX);

	/* test insert */
	fail_unless(cl_collection_insert(arr, 3, obj[2]) == SIZE_MAX);
	fail_unless(cl_collection_insert(arr, 0, obj[2]) == 0);

	/* test indexing */
	fail_unless(cl_collection_get(arr, 0) == obj[2]);
	fail_unless(cl_collection_get(arr, 1) == obj[0]);
	fail_unless(cl_collection_get(arr, 2) == obj[1]);
	fail_unless(cl_collection_get(arr, 3) == NULL);

	/* test removal */
	fail_unless(cl_collection_remove(arr, obj[0]) == obj[0]);
	fail_unless(cl_collection_remove(arr, obj[0]) == NULL);
	fail_unless(cl_collection_remove(arr, obj[2]) == obj[2]);
	fail_unless(cl_collection_delete(arr, 0) == obj[1]);
	fail_unless(cl_collection_delete(arr, 0) == NULL);
	fail_unless(arr->_capacity == 3);
	fail_unless(arr->_chunk_size == 3);
	fail_unless(arr->_count == 0);

	/* test destructor */
	cl_object_release(arr);
	for (int i=0; i < 3; i++) {
		fail_unless(obj[i]->_obj_info._ref == 1);
		cl_object_release(obj[i]);
	}
}

END_TEST

START_TEST(test_set)
{
}

END_TEST

START_TEST(test_queue)
{
}

END_TEST

START_TEST(test_stack)
{
}

END_TEST

Suite *test_suite(void)
{
	Suite *s = suite_create("TEST OBJECT");

	TCase *tc_core = tcase_create("TEST_COLLECTION");
	tcase_add_test(tc_core, test_array);
	tcase_add_test(tc_core, test_set);
	tcase_add_test(tc_core, test_queue);
	tcase_add_test(tc_core, test_stack);
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
