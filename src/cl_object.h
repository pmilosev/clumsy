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

#include <stdbool.h>

/** Object type flag for the top most object. */
#define CL_OBJECT_TYPE_OBJECT 0x01L

/** Struct containing object's metadata */
typedef struct cl_object_info_s cl_object_info;

/** Abstract object type. */
typedef struct cl_object_s cl_object;

/** Object's destructor type. */
typedef void (*cl_object_destructor) (void * self);

/** Initializes a new object. 
 * @param size The size of the object to be created.
 * @param dest The destructor to be called when the object gets deallocated.
 * @return The new object with retain count 0. */
cl_object *cl_object_init(size_t size, cl_object_destructor dest);

/** Checks the type flags for the provided object.
 * @param obj The object to be checked.
 * @param typeMask Bit mask, with each bit set to 1 if the object belongs to the type identified by the bit, or 0 otheriwse.
 * @return true If only the bits provided in the mask are set, false otherwise. */
bool cl_object_type_check(void * object, size_t typeMask);

/** Increases the object's referece counter. 
 * @param object The object to be retained.
 * @return The retained object. */
void *cl_object_retain(void * object);

/** Decreses the object's reference counter.
 * Triggers deallocation if the counter has dropped to 0.
 * @param object The object to be released.
 * @return The released object, or NULL if it was deallocated. */
void *cl_object_release(void * object);

/** Initializes and retains a new object */
#define cl_object_new(size, dest) cl_object_retain(cl_object_init(size, dest))

#endif				/* CL_OBJECT_H */
