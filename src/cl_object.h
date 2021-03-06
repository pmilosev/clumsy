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
#include <stdarg.h>

/** Dummy definiton of the object type flag.
 * Used for convenince with the @ref cl_object_type_check function. */
#define CL_OBJECT_TYPE_OBJECT 0x0

/** Flags data type for object type flags. */
typedef uint8_t cl_object_type_t;

/** Struct containing object's metadata */
typedef struct cl_object_info_s cl_object_info_t;

/** Abstract object type. */
typedef struct cl_object_s cl_object_t;

/** Object's destructor type. */
typedef void (*cl_object_destructor_t) (void *self);

/** Object's printer type. */
typedef char *(*cl_object_printer_t) (void *self);

/** Initializes a new object. 
 * @param size The size of the object to be created.
 * @param type The type flags identifing object's type.
 * @param dest The destructor to be called when the object gets deallocated.
 * @param to_str The object printer. If NULL is provided the default one would be used.
 * @return The new object with retain count 1. */
void *cl_object_new(size_t size, cl_object_type_t type,
		    cl_object_destructor_t dest, cl_object_printer_t to_str);

/** Checks the type flags for the provided object.
 * @param obj The object to be checked.
 * @param typeMask Bit mask, with each bit set to 1 if the object belongs to the type identified by the bit, or 0 otheriwse.
 * @return true If only the bits provided in the mask are set, false otherwise. */
bool cl_object_type_check(void *object, cl_object_type_t typeMask);

/* Returns a string reperesenting the object.
 * The returned string needs to be freed by the caller. */
char *cl_object_to_string(void *self);

/* Default object printer. */
char *cl_object_printer(void *self);

/** Increases the object's referece counter. 
 * @param object The object to be retained.
 * @return The retained object. */
void *cl_object_retain(void *object);

/** Decreses the object's reference counter.
 * Triggers deallocation if the counter has dropped to 0.
 * @param object The object to be released.
 * @return The released object, or NULL if it was deallocated. */
void *cl_object_release(void *object);

/** Schedules the object to be released in the future. */
void *cl_object_autorelease(void *object);

/** Pushes a new autorelease pool on the stack.
 * This method needs to be called at least once before any object is autoreleased,
 * or a call to a method returning an autoreleased object is made. */
void cl_object_pool_push();

/** Pops the top-most autorelease pool from the stack.
 * When this is done all the objects, autoreleased after the pool was pushed on stack,
 * will be released. */
void cl_object_pool_pop();

/** Default object comparator. 
 * Compares the object by memory address. */
int cl_object_comparator(const void *p1, const void *p2);

/** Returns a new, autoreleased, object */
#define cl_object(...) cl_object_autorelease(cl_object_new(__VA_ARGS__))

#endif				/* CL_OBJECT_H */
