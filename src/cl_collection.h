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

#ifndef CL_COLLECTION_H
#define CL_COLLECTION_H

#include "cl_object.h"
#include <inttypes.h>

/** Collection object type. Sets the third bit of the type flag. */
#define CL_OBJECT_TYPE_COLLECTION 0x04

/** Collection flags type. */
typedef uint8_t cl_collection_flags;

/** SORTED collection flag. 
 * If this flag is set the collection would always be sorted. 
 * This might have impact on the performance when adding new elements. */
#define CL_COLLECTION_FLAG_SORTED 0x01

/** UNIQUE collection flag.
 * If this flag is set the collection will permit adding the same object more than once.
 * I.e. It will behave like a set, 
 * oposed to when the flag is not set and the collection represents an array. */
#define CL_COLLECTION_FLAG_UNIQUE 0x02

/** UNLIMITED collection flag.
 * If this flag is set, the collection can be resized 
 * if there is not enough room for the new object */
#define CL_COLLECTION_FLAG_UNLIMITED 0x04

/** AUTORESIZE collection flag.
 * Same as the @ref UNLIMITED flag, but the collection would also 
 * be automatically shrinked if enough elements were removed. */
#define CL_COLLECTION_FLAG_AUTORESIZE 0x0C

/** The default chunk size for the collection when changing size. */
#define CL_COLLECTION_DEFAULT_CHUNK 512

/** Object type representing the collection. */
typedef struct cl_collection_s cl_collection;

/** Initializes a new collection.
 * @param nmemb The size of the collection in number of elements.
 * If the UNLIMITED or AUTORESIZE flag is set, this would be the size of the chunk at each change.
 * If 0 is provided, the @ref CL_COLLECTION_DEFAULT_CHUNK will be used.
 * @param type The type of the objects the collection should be expecting (sanity check).
 * @param flags On or more of the flags defined above. */
cl_collection *cl_collection_init(size_t nmemb, cl_object_type type,
				  cl_collection_flags flags);

/** Initializes and retains a new collection. */
#define cl_collection_new(nmemb, size, flags) cl_object_retain(cl_collection_init(nmemb, size, flags))

#endif				/* CL_COLLECTION_H */
