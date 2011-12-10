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

#include "cl_object.h"
#include <stdarg.h>

/** Proposition object type flag. */
#define CL_OBJECT_TYPE_PROPOSITION 0x01

/** Object type representing logical propositions. */
typedef struct cl_proposition_s cl_proposition_t;

/** Function type representing the logic operator */
typedef bool(*cl_proposition_operator_t) (cl_proposition_t * self);

/** Structure holding logic context of the proposition */
typedef struct cl_proposition_context_s cl_proposition_context_t;
struct cl_proposition_context_s {
	cl_proposition_operator_t op;
	void *argv[2];
};

/** Initializes a new proposition. 
 * A proposition can be any atomic proposition, or complex propositional formula.
 * @param op The operator of the proposition.<BR>
 * In a case of an atomic proposition this can be any function of the type @ref cl_proposition_operator.
 * @param ... List of pointers to data on which the operator works on.<BR>
 * In a case of an atomic proposition only one pointer is expected. If the proposition does not need any input you can provide NULL as argument.<BR>
 * In a case of a formula, the number of arguments depends on the operator.
 * @return The new proposition. */
cl_proposition_t *cl_proposition_new(cl_proposition_operator_t op, ...);

/** Returnes a pointer to the proposition's context */
cl_proposition_context_t *cl_proposition_get_context(cl_proposition_t * p);

/** Evaluates the proposition. */
bool cl_proposition_eval(cl_proposition_t * p);

/** The CONSTANT TRUE operator - tautology. */
bool cl_proposition_true_op(cl_proposition_t * self);
/** The CONSTANT FALSE operator - contradiction. */
bool cl_proposition_false_op(cl_proposition_t * self);
/** The NOT operator. */
bool cl_proposition_not_op(cl_proposition_t * self);
/** The AND operator. */
bool cl_proposition_and_op(cl_proposition_t * self);
/** The OR operator. */
bool cl_proposition_or_op(cl_proposition_t * self);
/** The IMPLY operator. */
bool cl_proposition_imply_op(cl_proposition_t * self);
/** The EQUIVALENCY operator. */
bool cl_proposition_equivalent_op(cl_proposition_t * self);
/** The XOR operator. */
bool cl_proposition_xor_op(cl_proposition_t * self);
/** The NAND operator. */
bool cl_proposition_nand_op(cl_proposition_t * self);
/** The NOR operator. */
bool cl_proposition_nor_op(cl_proposition_t * self);
/** The NOT IMPLY operator. */
bool cl_proposition_nimply_op(cl_proposition_t * self);

/** Returns a new, autoreleased, proposition */
#define cl_proposition(...) cl_object_autorelease(cl_proposition_new(__VA_ARGS__))

/** Returns a new, autoreleased proposition - tautology */
#define cl_proposition_true() cl_proposition(&cl_proposition_true_op)
/** Returns a new, autoreleased proposition - contradiction */
#define cl_proposition_false() cl_proposition(&cl_proposition_false_op)
/** Returns a new, autoreleased proposition - negation */
#define cl_proposition_not(p) cl_proposition(&cl_proposition_not_op, p)
/** Returns a new, autoreleased proposition - conjunction */
#define cl_proposition_and(p1, p2) cl_proposition(&cl_proposition_and_op, p1, p2)
/** Returns a new, autoreleased proposition - disjunction */
#define cl_proposition_or(p1, p2) cl_proposition(&cl_proposition_or_op, p1, p2)
/** Returns a new, autoreleased proposition - right implication */
#define cl_proposition_imply(p1, p2) cl_proposition(&cl_proposition_imply_op, p1, p2)
/** Returns a new, autoreleased proposition - left implication */
#define cl_proposition_implied(p1, p2) cl_proposition(&cl_proposition_imply_op, p2, p1)
/** Returns a new, autoreleased proposition - equivalency */
#define cl_proposition_equivalent(p1, p2) cl_proposition(&cl_proposition_equivalent_op, p2, p1)
/** Returns a new, autoreleased proposition - xor */
#define cl_proposition_xor(p1, p2) cl_proposition(&cl_proposition_xor_op, p1, p2)
/** Returns a new, autoreleased proposition - nand */
#define cl_proposition_nand(p1, p2) cl_proposition(&cl_proposition_nand_op, p1, p2)
/** Returns a new, autoreleased proposition - nor */
#define cl_proposition_nor(p1, p2) cl_proposition(&cl_proposition_nor_op, p1, p2)
/** Returns a new, autoreleased proposition - negated right implication */
#define cl_proposition_nimply(p1, p2) cl_proposition(&cl_proposition_nimply_op, p1, p2)
/** Returns a new, autoreleased proposition - negated left implication */
#define cl_proposition_nimplied(p1, p2) cl_proposition(&cl_proposition_nimply_op, p2, p1)

#endif				/* CL_PROPOSITION_H */
