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

#ifndef CL_COLLECTION_REP_H
#define CL_COLLECTION_REP_H

#include "cl_object_rep.h"

struct cl_collection_s {
	cl_object_info_t _obj_info;
	cl_object_type_t _type;
	cl_collection_flags_t _flags;
	size_t _chunk_size;
	size_t _capacity;
	size_t _count;
	void **_buffer;
};

#endif				/* CL_COLLECTION_REP_H */
