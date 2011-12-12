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

#include <assert.h>
#include <stdio.h>
#include "cl_object.h"
#include "cl_object_rep.h"
#include "cl_collection.h"

static const size_t MAGIC = 0x0b7ecdL;
static cl_collection_t *pool_stack = NULL;

char *cl_object_printer(void *self)
{
	/* '0x' = 2 chars
	 * 64-bits = 8 bytes = 16 chars
	 * '\0' = 1 char
	 * ----------------
	 *  MAX 19 chars would be needed */
	char *buffer = malloc(sizeof(char) * 19);
	assert(buffer);

	sprintf(buffer, "%p", self);
	return buffer;
}

void *cl_object_new(size_t size, cl_object_type_t type,
		    cl_object_destructor_t dest, cl_object_printer_t to_str)
{
	/* the size provided shuold at least fit the abstract object */
	assert(size >= sizeof(cl_object_t));

	cl_object_t *res = malloc(size);
	assert(res);

	res->_obj_info._MAGIC = MAGIC;
	res->_obj_info._type = type;
	res->_obj_info._ref = 1;
	res->_obj_info._dest = dest;
	res->_obj_info._to_str = to_str ? to_str : &cl_object_printer;

	return res;
}

bool cl_object_type_check(void *object, cl_object_type_t typeMask)
{
	if (!object) {
		return false;
	}

	cl_object_t *obj = (cl_object_t *) object;
	return (obj->_obj_info._MAGIC == MAGIC)
	    && (typeMask ? obj->_obj_info._type & typeMask : true);
}

void *cl_object_retain(void *object)
{
	if (!object) {
		return NULL;
	}

	assert(cl_object_type_check(object, CL_OBJECT_TYPE_OBJECT));
	((cl_object_t *) object)->_obj_info._ref += 1;
	return object;
}

char *cl_object_to_string(void *self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_OBJECT));
	return ((cl_object_t *) self)->_obj_info._to_str(self);
}

void *cl_object_release(void *object)
{
	if (!object) {
		return NULL;
	}

	assert(cl_object_type_check(object, CL_OBJECT_TYPE_OBJECT));
	cl_object_t *obj = (cl_object_t *) object;

	/* decrease the reference count */
	obj->_obj_info._ref--;

	/* if the reference count is above 0 do nothing */
	if (obj->_obj_info._ref) {
		return object;
	}

	/* otherwise deallocate the object */
	if (obj->_obj_info._dest) {
		obj->_obj_info._dest(object);
	}

	free(obj);
	return NULL;
}

void *cl_object_autorelease(void *object)
{
	if (!object) {
		return NULL;
	}

	assert(cl_object_type_check(object, CL_OBJECT_TYPE_OBJECT));
	assert(cl_collection_count(pool_stack));

	cl_object_t *obj = (cl_object_t *) object;
	cl_collection_t *pool = cl_collection_check(pool_stack);

	cl_collection_add(pool, obj);
	return cl_object_release(obj);
}

void cl_object_pool_push()
{
	if (!pool_stack) {
		pool_stack = cl_collection_new(0, CL_OBJECT_TYPE_OBJECT,
					       CL_COLLECTION_FLAG_AUTORESIZE);
	}

	cl_collection_t *pool = cl_collection_new(0, CL_OBJECT_TYPE_OBJECT,
						  CL_COLLECTION_FLAG_AUTORESIZE);

	cl_collection_add(pool_stack, pool);
	cl_object_release(pool);
}

void cl_object_pool_pop()
{
	if (!pool_stack) {
		return;
	}

	/* remove the top most pool, 
	 * which will automatically release all the 
	 * autoreleased objects it is currently holding */
	size_t pindex = cl_collection_count(pool_stack) - 1;
	cl_collection_delete(pool_stack, pindex);

	/* release the stack if it is empty */
	if (cl_collection_count(pool_stack) == 0) {
		cl_object_release(pool_stack);
		pool_stack = NULL;
	}
}

int cl_object_comparator(const void *p1, const void *p2)
{
	void *o1 = *((void **)p1);
	void *o2 = *((void **)p2);

	assert(cl_object_type_check(o1, CL_OBJECT_TYPE_OBJECT));
	assert(cl_object_type_check(o2, CL_OBJECT_TYPE_OBJECT));

	return o1 == o2 ? 0 : o1 < o2 ? -1 : 1;
}
