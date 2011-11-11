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
#include "assert.h"

static void destructor(void *obj)
{
	assert(cl_object_type_check(obj, CL_OBJECT_TYPE_COLLECTION));
	cl_collection_t *c = (cl_collection_t *) obj;

	/* TODO: release all objects and free the memory */
}

cl_collection_t *cl_collection_init(size_t nmemb, cl_object_type_t type,
				  cl_collection_flags_t flags)
{
	/* try if enough memory can be allocated */
	nmemb = nmemb ? nmemb : CL_COLLECTION_DEFAULT_CHUNK;
	void **buff = malloc(nmemb * sizeof(void *));
	assert(buff);

	/* init the object */
	cl_collection_t *res =
	    cl_object_init(sizeof(cl_collection_t), CL_OBJECT_TYPE_COLLECTION,
			   &destructor);

	/* set the collection attributes */
	res->_flags = flags;
	res->_type = type;
	res->_chunk_size = nmemb;
	res->_capacity = nmemb;
	res->_count = 0;
	res->_buffer = buff;

	return res;
}
