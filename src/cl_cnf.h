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

#ifndef CL_CNF_H
#define CL_CNF_H

#include "cl_proposition.h"
#include "cl_collection.h"

/** CNF obejct type flag. */
#define CL_OBJECT_TYPE_CNF 0x04

/** CNF LITERAL obejct type flag. */
#define CL_OBJECT_TYPE_CNF_LITERAL 0x08

/** Object type representing CNF formulas. */
typedef struct cl_cnf_s cl_cnf_t;

/** Object type representing CNF LITERAL. */
typedef struct cl_cnf_literal_s cl_cnf_literal_t;

/** Initializes a new CNF formula. */
cl_cnf_t *cl_cnf_init();

/** Ads the provided clause to the CNF formula.
 * @param self The CNF formula.
 * @param clause The collection representing the clause.
 * NOTE that the @ref CL_COLLECTION_FLAG_UNIQUE flag would be set for the clause before it is added to the CNF.
 * @return true if the clause was successfully added, or false otherwise. */
bool cl_cnf_add(cl_cnf_t * self, cl_collection_t * clause);

/** Initializes a new clause with the literals provided.
 * @param num The number of literals to be expected.
 * @param ... A list of @ref num number of literals.
 * @return The newly constructed clause, or NULL. */
cl_collection_t *cl_cnf_clause_init(size_t num, ...);

/** Constructs a CNF formula out of the provided propositional formula. */
cl_cnf_t *cl_cnf_construct(cl_proposition_t * proposition);

/** Initializes a new literal */
cl_cnf_literal_t *cl_cnf_literal_init();

/** Initializes a new literal, a negation of the provided one.
 * If the original (dual) literal get deallocated, the instance returned
 * by this method becomes a non-negated literal. */
cl_cnf_literal_t *cl_cnf_literal_not(cl_cnf_literal_t * literal);

/** Returnes a set of literals used in the CNF formula.
 * Negations are not included. */
cl_collection_t *cl_cnf_literals(cl_cnf_t * self);

/** Assigns a value to the literal.
 * @param literal The literal.
 * @param value The value to be assigned.
 * @return True if succesfull, or false otherwise.
 * The assignment will fail if the literal is a negation. */
bool cl_cnf_literal_assign(cl_cnf_literal_t * literal, bool value);

/** Evaluates the CNF formula. */
bool cl_cnf_evaluate(cl_cnf_t * self);

/** Initializes and retains a new CNF formula. */
#define cl_cnf_new() cl_object_retain(cl_cnf_init())

/** Initializes and retains a new literal. */
#define cl_cnf_literal_new() cl_object_retain(cl_cnf_literal_init())

/** Initializes and retains a new clause. */
#define cl_cnf_clause_new(num, ...) cl_object_retain(cl_cnf_clause_init(num, __VA_ARGS__))

#endif				/* CL_CNF_H */
