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

#ifndef CL_OBJECT_H
#define CL_OBJECT_H

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

/** Flags data type for object type flags. */
typedef uint8_t cl_object_type_t;

/** Struct containing object's metadata */
typedef struct cl_object_info_s cl_object_info_t;

/** Abstract object type. */
typedef struct cl_object_s cl_object_t;

/** Object's destructor type. */
typedef void (*cl_object_destructor_t) (void *self);

/** Initializes a new object. 
 * @param size The size of the object to be created.
 * @param type The type flags identifing object's type.
 * @param dest The destructor to be called when the object gets deallocated.
 * @return The new object with retain count 0. */
void *cl_object_init(size_t size, cl_object_type_t type,
		     cl_object_destructor_t dest);

/** Checks the type flags for the provided object.
 * @param obj The object to be checked.
 * @param typeMask Bit mask, with each bit set to 1 if the object belongs to the type identified by the bit, or 0 otheriwse.
 * @return true If only the bits provided in the mask are set, false otherwise. */
bool cl_object_type_check(void *object, cl_object_type_t typeMask);

/** Increases the object's referece counter. 
 * @param object The object to be retained.
 * @return The retained object. */
void *cl_object_retain(void *object);

/** Decreses the object's reference counter.
 * Triggers deallocation if the counter has dropped to 0.
 * @param object The object to be released.
 * @return The released object, or NULL if it was deallocated. */
void *cl_object_release(void *object);

/** Initializes and retains a new object */
#define cl_object_new(size, dest) cl_object_retain(cl_object_init(size, dest))

#endif				/* CL_OBJECT_H */
