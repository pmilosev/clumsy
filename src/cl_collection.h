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
#define CL_OBJECT_TYPE_COLLECTION 0x02

/** Collection flags type.
 * The flags can be combined by using the bitwise OR operator.
 * Since some flags don't make sense when combined together,
 * the smaller value flags are given the priority.
 * E.G. CL_COLLECTION_FLAG_SORTED would have priority over CL_COLLECTION_FLAG_QUEUE.
 * That is the collection would always be sorted, regardless of the order in which the objects are inserted. */
typedef uint8_t cl_collection_flags_t;

/** SORTED collection flag. 
 * If this flag is set the collection would always be sorted. 
 * This would reduce the perofrmance of the functions for adding new objects to the collection,
 * but would increase the performance of the search functions. */
#define CL_COLLECTION_FLAG_SORTED 0x01

/** UNIQUE collection flag.
 * If this flag is set the collection will permit adding the same object more than once.
 * I.e. It will behave like a set, 
 * oposed to when the flag is not set and the collection represents an array.
 * Current implementation implies the @ref CL_COLLECTION_FLAG_SORTED flag, but this might change.*/
#define CL_COLLECTION_FLAG_UNIQUE 0x02

/** QUEUE collection flag.
 * If this flag is set the collection would behave as a FIFO queue.
 * Otherwise, if not sorted it will behave as a stack. */
#define CL_COLLECTION_FLAG_QUEUE 0x04

/** UNLIMITED collection flag.
 * If this flag is set, the collection can be resized 
 * if there is not enough room for the new object */
#define CL_COLLECTION_FLAG_UNLIMITED 0x08

/** AUTORESIZE collection flag.
 * Same as the @ref UNLIMITED flag, but the collection would also 
 * be automatically shrinked if enough objects were removed. */
#define CL_COLLECTION_FLAG_AUTORESIZE 0x10

/** The default chunk size for the collection when changing size. */
#define CL_COLLECTION_DEFAULT_CHUNK 512

/** Object type representing the collection. */
typedef struct cl_collection_s cl_collection_t;

/** Initializes a new collection.
 * @param nmemb The size of the collection in number of objects.
 * If the UNLIMITED or AUTORESIZE flag is set, this would be the size of the chunk at each change.
 * If 0 is provided, the @ref CL_COLLECTION_DEFAULT_CHUNK will be used.
 * @param type The type of the objects the collection should be expecting (sanity check).
 * @param flags On or more of the flags defined above. */
cl_collection_t *cl_collection_init(size_t nmemb, cl_object_type_t type,
				    cl_collection_flags_t flags);

/** Returns the currently allocated length of the collection. */
size_t cl_collection_capacity(cl_collection_t * self);

/** Returns the number of objects in the collection. */
size_t cl_collection_count(cl_collection_t * self);

/** Adds the object to the collection.
 * By default the collection is behaving as a non-sorted array, adding the objects at the end of the array.
 * This behaviour can be changed by setting some of the flags defined.
 * @param self The collection to which to add the object.
 * @param object The object to be added. The object is retained by the collection.
 * @return The index at which the object was added, or SIZE_MAX if there was some problem. */
size_t cl_collection_add(cl_collection_t * self, void *object);

/** Inserts the object at the provided index.
 * The insert of the object will fail if it breaks the rules set by some of the flags.
 * E.G. The @ref CL_COLLECTION_FLAG_SORTED flag is set, but the insertation would lead to an unsorted array.
 * @param self The collection to which to add the object.
 * @param index The index at witch the object should be added.
 * @param object The object to be added. The object is retained by the collection.
 * @return The index at which the object was added, or SIZE_MAX if there was some problem. */
size_t cl_collection_insert(cl_collection_t * self, size_t index, void *object);

/** Returns the index of the provided object, or SIZE_MAX if the object can not be found. */
size_t cl_collection_find(cl_collection_t * self, void *object);

/** Returns the object at the provided index, or NULL if the index is out of bounds. */
void *cl_collection_get(cl_collection_t * self, size_t index);

/** Returns one object from the collection.
 * Depending of the flags set, this can be the first, last or any random object from the collection. */
void *cl_collection_check(cl_collection_t * self);

/** Removes one object from the collection.
 * Same as @ref cl_collection_check, but additionally removes the returned object. */
void *cl_collection_pick(cl_collection_t * self);

/** Removes the provided object from the collection. 
 * @param self The collection from which to remove the object.
 * @param object The object to be removed. The object is released by the collection.
 * @return The object if removed, or NULL otherwise. */
void *cl_collection_remove(cl_collection_t * self, void *object);

/** Removes the object at the provided index.
 * @param self The collection from which to remove the object.
 * @param index The index of the object to be removed. 
 * @return The object that was removed, or NULL if index is out of bounds.
 * The object is released by the collection. */
void *cl_collection_delete(cl_collection_t * self, size_t index);

/** Sets the provided flags for the collection. */
void cl_collection_flag_set(cl_collection_t * self, cl_collection_flags_t flags);

/** Unsets the provided flags for the collection. */
void cl_collection_flag_unset(cl_collection_t * self,
			      cl_collection_flags_t flags);

/** Returns wether any of the flags specified by the mask are set for the collection. */
bool cl_collection_flag_check(cl_collection_t * self,
			      cl_collection_flags_t mask);

/** Initializes and retains a new collection. */
#define cl_collection_new(nmemb, type, flags) cl_object_retain(cl_collection_init(nmemb, type, flags))

#endif				/* CL_COLLECTION_H */
