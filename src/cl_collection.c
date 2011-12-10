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

#include "cl_collection.h"
#include "cl_collection_rep.h"
#include <assert.h>
#include <string.h>

static void destructor(void *self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));
	cl_collection_t *c = (cl_collection_t *) self;

	if (c->_buffer) {
		for (int i = 0; i < c->_count; i++) {
			cl_object_release(c->_buffer[i]);
		}

		free(c->_buffer);
	}
}

cl_collection_t *cl_collection_new(size_t nmemb, cl_object_type_t type,
				   cl_collection_flags_t flags)
{
	/* try if enough memory can be allocated */
	nmemb = nmemb ? nmemb : CL_COLLECTION_DEFAULT_CHUNK;
	void **buff = malloc(nmemb * sizeof(void *));
	assert(buff);

	/* init the object */
	cl_collection_t *res =
	    cl_object_new(sizeof(cl_collection_t), CL_OBJECT_TYPE_COLLECTION,
			  &destructor);

	/* set the collection attributes */
	res->_comparator = &cl_object_comparator;
	res->_type = type;
	res->_chunk_size = nmemb;
	res->_capacity = nmemb;
	res->_count = 0;
	res->_buffer = buff;
	res->_flags = 0;
	cl_collection_flag_set(res, flags);

	return res;
}

void cl_collection_comparator_set(cl_collection_t * self, __compar_fn_t compar)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));

	compar = compar ? compar : &cl_object_comparator;
	if (compar == self->_comparator) {
		return;
	}

	self->_comparator = compar;
	if (cl_collection_flag_check(self, CL_COLLECTION_FLAG_SORTED)) {
		qsort(self->_buffer, self->_count, sizeof(void *), compar);
	}
}

size_t cl_collection_capacity(cl_collection_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));
	return self->_capacity;
}

size_t cl_collection_count(cl_collection_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));
	return self->_count;
}

static size_t index(cl_collection_t * self, size_t start, void *object)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));
	if (start > self->_count) {
		return SIZE_MAX;
	}

	void **array = self->_buffer;
	size_t count = self->_count;
	size_t ind = start;

	/* if not sorted, search sequentially */
	if (!cl_collection_flag_check(self, CL_COLLECTION_FLAG_SORTED)) {
		for (ind = start; ind < count; ind++) {
			if (self->_comparator(&array[ind], &object) == 0) {
				break;
			}
		}

		return ind;
	}

	/* otherwise use bianry search */
	int min = start;
	int max = self->_count - 1;
	while ((min <= max)
	       && (self->_comparator(&object, &array[ind]) != 0)) {
		ind = (min + max) / 2;
		if (self->_comparator(&object, &array[ind]) > 0) {
			min = ++ind;
		} else {
			max = ind - 1;
		}
	}

	/* since bsearch does not guarantee the lowest index
	 * if there are duplicates, do the correction sequentially */
	while (ind > start && self->_comparator(&object, &array[ind - 1]) == 0) {
		ind--;
	}

	return ind;
}

size_t cl_collection_add(cl_collection_t * self, void *object)
{
	size_t ind = cl_collection_flag_check(self, CL_COLLECTION_FLAG_SORTED)
	    ? index(self, 0, object)
	    : self->_count;
	cl_collection_insert(self, ind, object);
}

size_t cl_collection_insert(cl_collection_t * self, size_t index, void *object)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));

	/* index out of bounds */
	if (index > self->_count) {
		return SIZE_MAX;
	}

	/* check if the object should be unique - implies sorted */
	if (cl_collection_flag_check(self, CL_COLLECTION_FLAG_UNIQUE)) {
		bool ok = true;
		ok &= index < self->_count
		    ? self->_comparator(&object, &(self->_buffer[index])) < 0
		    : true;
		ok &= index > 0
		    ? self->_comparator(&object,
					&(self->_buffer[index - 1])) > 0 : true;

		if (!ok) {
			return SIZE_MAX;
		}
	}

	/* maximum capacity was reached */
	if (self->_count == self->_capacity) {
		if (!cl_collection_flag_check
		    (self, CL_COLLECTION_FLAG_UNLIMITED)) {
			return SIZE_MAX;
		}

		void *temp = realloc(self->_buffer,
				     (self->_capacity +
				      self->_chunk_size) * sizeof(void *));
		if (!temp) {
			return SIZE_MAX;
		}

		self->_buffer = temp;
		self->_capacity += self->_chunk_size;
	}

	/* shift the objects to the right if needed */
	if (index < self->_count) {
		memmove(self->_buffer + index + 1, self->_buffer + index,
			(self->_count - index) * sizeof(void *));
	}

	assert(cl_object_retain(object));
	self->_buffer[index] = object;
	self->_count++;

	return index;
}

size_t cl_collection_find(cl_collection_t * self, size_t start, void *object)
{
	size_t ind = index(self, start, object);
	if (ind >= self->_count
	    || self->_comparator(&object, &(self->_buffer[ind])) != 0) {
		return SIZE_MAX;
	}

	return ind;
}

void *cl_collection_get(cl_collection_t * self, size_t index)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));
	return index < self->_count ? self->_buffer[index] : NULL;
}

static size_t check(cl_collection_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));
	if (self->_count == 0) {
		return SIZE_MAX;
	}

	if (cl_collection_flag_check(self, CL_COLLECTION_FLAG_QUEUE)) {
		return 0;
	}

	return self->_count - 1;
}

void *cl_collection_check(cl_collection_t * self)
{
	return cl_collection_get(self, check(self));
}

void *cl_collection_pick(cl_collection_t * self)
{
	size_t index = check(self);
	cl_object_t *obj = cl_collection_get(self, index);
	cl_object_retain(obj);
	cl_collection_delete(self, index);

	return cl_object_autorelease(obj);
}

size_t cl_collection_remove(cl_collection_t * self, void *object)
{
	size_t count = 0;
	size_t i = index(self, 0, object);
	void *obj = cl_collection_get(self, i);

	while (obj && (self->_comparator(&obj, &object) == 0)) {
		cl_collection_delete(self, i);
		count++;

		i = index(self, 0, object);
		obj = cl_collection_get(self, i);
	}

	return count;
}

void cl_collection_delete(cl_collection_t * self, size_t index)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));

	cl_object_t *obj = cl_collection_get(self, index);
	if (!obj) {
		return;
	}

	/* shift the objects and reduce the buffer if needed */
	self->_count--;
	memmove(self->_buffer + index, self->_buffer + index + 1,
		(self->_count - index) * sizeof(void *));

	// TODO: measure the performance if we don't shift the memory when elements are removed from the front
	// but keep a min index instead and only do the moving when adjasting the size of the buffer

	if (cl_collection_flag_check(self, CL_COLLECTION_FLAG_AUTORESIZE)
	    && (self->_capacity - self->_count > self->_chunk_size)) {

		void *temp = realloc(self->_buffer,
				     (self->_capacity -
				      self->_chunk_size) * sizeof(void *));

		if (temp) {
			self->_buffer = temp;
			self->_capacity -= self->_chunk_size;
		}
	}

	cl_object_release(obj);
}

void cl_collection_flag_set(cl_collection_t * self, cl_collection_flags_t flags)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));

	/* check current flags */
	bool sorted = cl_collection_flag_check(self, CL_COLLECTION_FLAG_SORTED);
	bool unique = cl_collection_flag_check(self, CL_COLLECTION_FLAG_UNIQUE);

	/* set flags */
	self->_flags |= flags;

	/* set implied flags */
	self->_flags |=
	    cl_collection_flag_check(self, CL_COLLECTION_FLAG_UNIQUE)
	    ? CL_COLLECTION_FLAG_SORTED : 0;
	self->_flags |=
	    cl_collection_flag_check(self, CL_COLLECTION_FLAG_AUTORESIZE)
	    ? CL_COLLECTION_FLAG_UNLIMITED : 0;

	/* sort if required */
	if (!sorted
	    && cl_collection_flag_check(self, CL_COLLECTION_FLAG_SORTED)) {
		qsort(self->_buffer, self->_count, sizeof(void *),
		      self->_comparator);
	}

	/* filter if required */
	if (!unique
	    && cl_collection_flag_check(self, CL_COLLECTION_FLAG_UNIQUE)) {
		for (int i = 1; i < self->_count; i++) {
			if (self->_comparator(&(self->_buffer[i - 1]),
					      &(self->_buffer[i])) == 0) {
				cl_collection_delete(self, i);
				i--;
			}
		}
	}
}

void cl_collection_flag_unset(cl_collection_t * self,
			      cl_collection_flags_t flags)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));
	self->_flags &= ~flags;

	/* unset implied flags */
	self->_flags &=
	    ~(cl_collection_flag_check(self, CL_COLLECTION_FLAG_SORTED)
	      ? 0 : CL_COLLECTION_FLAG_UNIQUE);
	self->_flags &=
	    ~(cl_collection_flag_check(self, CL_COLLECTION_FLAG_UNLIMITED)
	      ? 0 : CL_COLLECTION_FLAG_AUTORESIZE);
}

bool cl_collection_flag_check(cl_collection_t * self,
			      cl_collection_flags_t mask)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_COLLECTION));
	return self->_flags & mask;
}
