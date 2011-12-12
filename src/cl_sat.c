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

#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "cl_sat.h"
#include "cl_sat_rep.h"

static void destructor(void *self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_SAT));
	cl_sat_t *sat = (cl_sat_t *) self;

	/* TODO: destory the object */
}

cl_sat_t *cl_sat_new()
{
	cl_sat_t *res = cl_object_new(sizeof(cl_sat_t), CL_OBJECT_TYPE_SAT, &destructor);
	return res;
}

static void randomize()
{
	static bool randomized = false;
	if (!randomized) {
		srand(time(0));
		randomized = true;
	}
}

static void init_try(cl_cnf_t * cnf)
{
	cl_collection_t * literals = cl_cnf_literals(cnf);
	for (int i = 0; i < cl_collection_count(literals); i++) {
		cl_cnf_literal_assign(cl_collection_get(literals, i), (rand() % 2));
	}
}

static cl_cnf_literal_t * choose_literal(cl_sat_t *self, cl_cnf_t *cnf)
{
	/* TODO: choose the literal according to some heuristics */
	cl_collection_t * literals = cl_cnf_literals(cnf);
	size_t ind = rand() % cl_collection_count(literals);

	return cl_collection_get(literals, ind);
}

cl_collection_t * cl_sat_solve(cl_sat_t * self, cl_cnf_t *cnf)
{
	randomize();
	init_try(cnf);

	/* if we have a solution - return it imediatly*/
	if (cl_cnf_evaluate(cnf)) {
		return cl_cnf_literals(cnf);
	}

	/* otherwise search for the solution */
	size_t numflips = 0;	
	while (numflips < self->_maxflips) {
		/* flip one literal */
		cl_cnf_literal_t *literal = choose_literal(self, cnf);
		cl_cnf_literal_assign(literal, !cl_cnf_literal_value(literal));
		numflips ++;

		/* return the solution if found */
		if (cl_cnf_evaluate(cnf)) {
			return cl_cnf_literals(cnf);
		}
	}

	return NULL;
}
