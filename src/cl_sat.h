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

#ifndef CL_SAT_H
#define CL_SAT_H

#include "cl_object.h"
#include "cl_cnf.h"
#include "cl_collection.h"

/** SAT solver object type flag. */
#define CL_OBJECT_TYPE_SAT 0x10

/** SAT flags type.
 * The flags can be combined by using the bitwise OR operator. */
typedef uint8_t cl_sat_flags_t;

/** Object type representing the SAT solver. */
typedef struct cl_sat_s cl_sat_t;

/** Initializes a new SAT solver. */
cl_sat_t *cl_sat_new();

/** Runs the solver for the provided CNF formula.
 * @return A set of all the literlas in the formula,
 * as returned by the @ref cl_cnf_literals function, 
 * with their proper assignment which satifies the formula.
 * Or NULL if such assignment was not fund. */
cl_collection_t * cl_sat_solve(cl_sat_t * self, cl_cnf_t *cnf);

/** Returns a new, autoreleased, SAT solver */
#define cl_sat(...) cl_object_autorelease(cl_sat_new(__VA_ARGS__))

#endif				/* CL_SAT_H */
