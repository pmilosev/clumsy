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

static int inverse_comparator(const void * p1, const void *p2)
{
	void * o1 = *((void **)p1);
	void * o2 = *((void **)p2);

	return o1 == o2 ? 0
		: o1 < o2 ? 1 : -1;
}

START_TEST(test_array)
{
	cl_object_t *obj[3] = {
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL),
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL),
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL)
	};

	/* test initialization */
	cl_collection_t *arr =
	    cl_collection_new(3, CL_OBJECT_TYPE_COLLECTION, 0);

	fail_if(arr == NULL);
	fail_unless(cl_object_type_check(arr, CL_OBJECT_TYPE_COLLECTION));
	fail_unless(cl_collection_capacity(arr) == 3);
	fail_unless(cl_collection_count(arr) == 0);
	fail_unless(arr->_chunk_size == 3);
	fail_unless(arr->_flags == 0);
	fail_if(arr->_buffer == NULL);

	/* search in an empty collection */
	fail_unless(cl_collection_find(arr, 0, obj[1]) == SIZE_MAX);
	fail_unless(cl_collection_pick(arr) == NULL);

	/* successfull addition */
	for (int i = 0; i < 3; i++) {
		fail_unless(cl_collection_add(arr, obj[i]) == i);
		fail_unless(obj[i]->_obj_info._ref == 2);
	}

	/* there should be no space left */
	fail_unless(cl_collection_add(arr, obj[0]) == SIZE_MAX);

	/* test autoresize (expanding) */
	cl_collection_flag_set(arr, CL_COLLECTION_FLAG_AUTORESIZE);
	fail_unless(cl_collection_flag_check
		    (arr, CL_COLLECTION_FLAG_UNLIMITED));
	fail_unless(cl_collection_add(arr, obj[0]) == 3);
	fail_unless(arr->_chunk_size == 3);
	fail_unless(cl_collection_capacity(arr) == 6);
	fail_unless(cl_collection_count(arr) == 4);

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
	fail_unless(arr->_chunk_size == 3);
	fail_unless(cl_collection_capacity(arr) == 3);
	fail_unless(cl_collection_count(arr) == 2);

	/* test insert */
	fail_unless(cl_collection_insert(arr, 3, obj[2]) == SIZE_MAX);
	fail_unless(cl_collection_insert(arr, 0, obj[2]) == 0);

	/* test indexing */
	fail_unless(cl_collection_get(arr, 0) == obj[2]);
	fail_unless(cl_collection_get(arr, 1) == obj[0]);
	fail_unless(cl_collection_get(arr, 2) == obj[1]);
	fail_unless(cl_collection_get(arr, 3) == NULL);

	/* test removal */
	cl_collection_add(arr, obj[0]);
	fail_unless(cl_collection_remove(arr, obj[0]) == 2);
	fail_unless(cl_collection_remove(arr, obj[0]) == 0);
	fail_unless(cl_collection_remove(arr, obj[2]) == 1);
	fail_unless(cl_collection_delete(arr, 0) == obj[1]);
	fail_unless(cl_collection_delete(arr, 0) == NULL);
	fail_unless(arr->_chunk_size == 3);
	fail_unless(cl_collection_capacity(arr) == 3);
	fail_unless(cl_collection_count(arr) == 0);

	/* test destructor */
	cl_object_release(arr);
	for (int i = 0; i < 3; i++) {
		fail_unless(obj[i]->_obj_info._ref == 1);
		cl_object_release(obj[i]);
	}
}

END_TEST START_TEST(test_set)
{
	cl_object_t *obj[3] = {
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL),
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL),
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL)
	};

	cl_collection_t *arr = cl_collection_new(0, CL_OBJECT_TYPE_COLLECTION, CL_COLLECTION_FLAG_UNIQUE);

	/* SORTED flag should be implied */
	fail_unless(cl_collection_flag_check(arr, CL_COLLECTION_FLAG_SORTED));

	/* successfull addition */
	for (int i = 0; i < 3; i++) {
		fail_if(cl_collection_add(arr, obj[i]) == SIZE_MAX);
		fail_unless(obj[i]->_obj_info._ref == 2);
	}

	/* add some random elements */
	for (int i = 4; i < CL_COLLECTION_DEFAULT_CHUNK - 3; i++) {
		cl_collection_add(arr, cl_object_init(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL));
	}

	/* try to add duplicates */
	for (int i = 0; i < 3; i++) {
		fail_unless(cl_collection_add(arr, obj[i]) == SIZE_MAX);
	}

	/* cleanup */
	for (int i = 0; i < 3; i++) {
		cl_object_release(obj[i]);
	}

	cl_object_release(arr);
}

END_TEST START_TEST(test_flags_comparator)
{
	cl_object_t *obj[3] = {
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL),
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL),
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL)
	};

	cl_collection_t *arr = cl_collection_new(0, CL_OBJECT_TYPE_COLLECTION, CL_COLLECTION_FLAG_UNIQUE);

	/* add all but sorted in oposite direction */
	qsort(obj, 3, sizeof(void *), &inverse_comparator);
	for (int i = 0; i < 3; i++) {
		fail_unless(cl_collection_add(arr, obj[i]) == 0);
		fail_unless(obj[i]->_obj_info._ref == 2);
	}

	/* clear the SORTED (implies UNIQUE) flag */
	cl_collection_flag_unset(arr, CL_COLLECTION_FLAG_SORTED);
	fail_if(cl_collection_flag_check(arr, CL_COLLECTION_FLAG_UNIQUE));

	/* add non-sorted duplicate data */
	void *temp = obj[0];
	obj[0] = obj[1];
	obj[1] = obj[2];
	obj[2] = temp;
	for (int i = 3; i < 30; i++) {
		fail_unless(cl_collection_add(arr, obj[i % 3]) == i);
	}

	/* add the SORTED flag and test ordering */
	cl_collection_flag_set(arr, CL_COLLECTION_FLAG_SORTED);
	for (int i = 1; i < 30; i++) {
		fail_unless(arr->_buffer[i - 1] <= arr->_buffer[i]);
	}

	/* test search */
	void *o = arr->_buffer[0];
	fail_unless(cl_collection_find(arr, 0, o) == 0);
	fail_unless(cl_collection_find(arr, 3, o) == 3);
	fail_unless(cl_collection_find(arr, 10, o) == SIZE_MAX);

	o = arr->_buffer[10];
	fail_unless(cl_collection_find(arr, 0, o) == 10);
	fail_unless(cl_collection_find(arr, 13, o) == 13);
	fail_unless(cl_collection_find(arr, 20, o) == SIZE_MAX);
	
	o = arr->_buffer[20];
	fail_unless(cl_collection_find(arr, 0, o) == 20);
	fail_unless(cl_collection_find(arr, 23, o) == 23);
	fail_unless(cl_collection_find(arr, 30, o) == SIZE_MAX);
	fail_unless(cl_collection_find(arr, 31, o) == SIZE_MAX);

	/* test changing of the comparator */
	cl_collection_comparator_set(arr, &inverse_comparator);
	for (int i = 1; i < 30; i++) {
		fail_unless(arr->_buffer[i - 1] >= arr->_buffer[i]);
	}

	/* test filtering */
	cl_collection_flag_set(arr, CL_COLLECTION_FLAG_UNIQUE);
	fail_unless(cl_collection_count(arr) == 3);

	/* cleanup */
	for (int i = 0; i < 3; i++) {
		cl_object_release(obj[i]);
	}

	cl_object_release(arr);
}

END_TEST START_TEST(test_queue)
{
	cl_object_t *obj[3] = {
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL),
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL),
		cl_object_new(sizeof(cl_object_t), CL_OBJECT_TYPE_OBJECT, NULL)
	};

	cl_collection_t *arr = cl_collection_new(0, CL_OBJECT_TYPE_COLLECTION, CL_COLLECTION_FLAG_QUEUE);

	/* successfull addition */
	for (int i = 0; i < 3; i++) {
		fail_if(cl_collection_add(arr, obj[i]) == SIZE_MAX);
		fail_unless(obj[i]->_obj_info._ref == 2);
	}

	/* test FIFO retrieval */
	for (int i = 0; i < 3; i++) {
		fail_unless(cl_collection_pick(arr) == obj[i]);
	}

	/* cleanup */
	for (int i = 0; i < 3; i++) {
		cl_object_release(obj[i]);
	}

	cl_object_release(arr);
}

END_TEST Suite *test_suite(void)
{
	Suite *s = suite_create("TEST COLLECTION");

	TCase *tc_core = tcase_create("TEST_COLLECTION");
	tcase_add_test(tc_core, test_array);
	tcase_add_test(tc_core, test_set);
	tcase_add_test(tc_core, test_flags_comparator);
	tcase_add_test(tc_core, test_queue);
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
