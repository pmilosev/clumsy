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

#ifndef CL_PROPOSITION_H
#define CL_PROPOSITION_H

#include <stdbool.h>
#include <stdarg.h>

/** Object type representing logical propositions. */
typedef struct cl_proposition_s cl_proposition;

/** Function type representing the logic operator */
typedef bool(*cl_proposition_operator) (cl_proposition * self);

/** Structure holding logic context of the proposition */
typedef struct cl_proposition_context_s cl_proposition_context;
struct cl_proposition_context_s {
	cl_proposition_operator op;
	void *argv[2];
};

/** Initializes a new proposition. 
 * A proposition can be any atomic proposition, or complex propositional formula.
 * @param op The operator of the proposition.<BR>
 * In a case of an atomic formula this can be any function of the type @ref cl_proposition_operator.
 * @param ... One or more pointers, pointing to the context of the proposition.<BR>
 * In a case of an atomic proposition only one pointer representing the complete context is expected.<BR>
 * In a case of a formula, the number of arguments depends on the operator.
 * @return The new proposition. */
cl_proposition *cl_proposition_init(cl_proposition_operator op, ...);

/** Returnes a pointer to the proposition's context */
cl_proposition_context *cl_proposition_get_context(cl_proposition * p);

/** Evaluates the proposition. */
bool cl_proposition_eval(cl_proposition * p);

/** The NOT operator. */
bool cl_proposition_not_op(cl_proposition * self);
/** The AND operator. */
bool cl_proposition_and_op(cl_proposition * self);
/** The OR operator. */
bool cl_proposition_or_op(cl_proposition * self);

/** Initializes a new proposition, negation of the provided one. */
#define cl_proposition_not(p) cl_proposition_init(&cl_proposition_not, p)

/** Initializes a new proposition, conjunction of the provided propositions. */
#define cl_proposition_and(p1, p2) cl_proposition_init(&cl_proposition_and, p1, p2)

/** Initializes a new proposition, disjunction of the provided propositions. */
#define cl_proposition_or(p1, p2) cl_proposition_init(&cl_proposition_or, p1, p2)

/** Initializes a new proposition, implicaton from p1 to p2. */
#define cl_proposition_imply(p1, p2) cl_proposition_or(cl_proposition_not(p1), p2)

/** Initializes and retains a new proposition */
#define cl_proposition_new(op, ...) ((cl_proposition *) cl_object_retain((cl_object *) cl_proposition_init(op, __VA_ARGS__)))

#endif				/* CL_PROPOSITION_H */
