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
#include "cl_object.h"
#include "cl_object_rep.h"

static const size_t MAGIC = 0x0b7ecdL;

void *cl_object_init(size_t size, cl_object_type_t type,
		     cl_object_destructor_t dest)
{
	/* the size provided shuold at least fit the abstract object */
	assert(size >= sizeof(cl_object_t));

	cl_object_t *res = malloc(size);
	assert(res);

	res->_obj_info._MAGIC = MAGIC;
	res->_obj_info._type = type;
	res->_obj_info._ref = 0;
	res->_obj_info._dest = dest;

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

void *cl_object_release(void *object)
{
	if (!object) {
		return NULL;
	}

	assert(cl_object_type_check(object, CL_OBJECT_TYPE_OBJECT));
	cl_object_t *obj = (cl_object_t *) object;

	/* if the reference is already 0 don't decrease it */
	obj->_obj_info._ref -= obj->_obj_info._ref ? 1 : 0;

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

int cl_object_comparator(const void * p1, const void *p2)
{
	void * o1 = *((void **)p1);
	void * o2 = *((void **)p2);

	assert(cl_object_type_check(o1, CL_OBJECT_TYPE_OBJECT));
	assert(cl_object_type_check(o2, CL_OBJECT_TYPE_OBJECT));

	return o1 == o2 ? 0
		: o1 < o2 ? -1 : 1;
}
