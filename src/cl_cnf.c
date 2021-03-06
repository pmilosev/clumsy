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

#include "cl_cnf.h"
#include "cl_cnf_rep.h"
#include <assert.h>
#include <string.h>

static void cnf_destructor(void *self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_CNF));

	cl_cnf_t *cnf = (cl_cnf_t *) self;
	cl_object_release(cnf->_set);
}

static void literal_destructor(void *self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_CNF_LITERAL));

	cl_cnf_literal_t *lit = (cl_cnf_literal_t *) self;
	cl_object_release(lit->_proposition);
	if (lit->_dual) {
		lit->_dual->_dual = NULL;
		lit->_dual->_negation = false;
	}
}

static char *cnf_printer(void *self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_CNF));
	cl_cnf_t *cnf = (cl_cnf_t *) self;

	return cl_object_to_string(cnf->_set);
}

static char *literal_printer(void *self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_CNF_LITERAL));
	cl_cnf_literal_t *lit = (cl_cnf_literal_t *) self;

	char *buffer = NULL;

	if (lit->_negation) {
		assert(lit->_dual);
		char *temp = cl_object_to_string(lit->_dual);
		size_t len = strlen(temp) + 2;
		buffer = malloc(sizeof(char) * len);
		assert(buffer);

		strcpy(buffer, "~");
		strcpy(buffer + 1, temp);

		free(temp);
		return buffer;
	}

	char *temp = cl_object_printer(lit);
	size_t len1 = strlen(temp);
	size_t len = len1;
	len += lit->_value ? 10 : 11;
	buffer = malloc(sizeof(char) * len);
	assert(buffer);

	strcpy(buffer, "[");
	strcpy(buffer + 1, temp);
	if (lit->_value) {
		strcpy(buffer + len1 + 1, " : TRUE]");
	} else {
		strcpy(buffer + len1 + 1, " : FALSE]");
	}

	free(temp);
	return buffer;
}

cl_cnf_t *cl_cnf_new()
{
	cl_cnf_t *self = cl_object_new(sizeof(cl_cnf_t),
				       CL_OBJECT_TYPE_CNF, &cnf_destructor,
				       &cnf_printer);

	self->_set = cl_collection_new(0, CL_OBJECT_TYPE_COLLECTION,
				       CL_COLLECTION_FLAG_UNIQUE |
				       CL_COLLECTION_FLAG_AUTORESIZE);

	return self;
}

cl_collection_t *cl_cnf_clause_new(size_t num, ...)
{
	va_list ap;

	cl_collection_t *clause =
	    cl_collection_new(num, CL_OBJECT_TYPE_CNF_LITERAL,
			      CL_COLLECTION_FLAG_UNIQUE |
			      CL_COLLECTION_FLAG_AUTORESIZE);

	va_start(ap, num);
	for (int i = 0; i < num; i++) {
		cl_cnf_literal_t *lit = va_arg(ap, cl_cnf_literal_t *);
		cl_collection_add(clause, lit);
	}
	va_end(ap);

	return clause;
}

cl_cnf_literal_t *cl_cnf_literal_new()
{
	cl_cnf_literal_t *self = cl_object_new(sizeof(cl_cnf_literal_t),
					       CL_OBJECT_TYPE_CNF_LITERAL,
					       &literal_destructor,
					       &literal_printer);

	self->_proposition = NULL;
	self->_dual = NULL;
	self->_negation = false;
	self->_value = false;

	return self;
}

cl_cnf_literal_t *cl_cnf_literal_not(cl_cnf_literal_t * literal)
{
	assert(cl_object_type_check(literal, CL_OBJECT_TYPE_CNF_LITERAL));

	/* if the dual literal already exists, return it */
	if (literal->_dual) {
		return literal->_dual;
	}

	/* construct the dual / nagated literal */
	cl_cnf_literal_t *self = cl_cnf_literal();
	self->_dual = literal;
	literal->_dual = self;

	self->_value = !literal->_value;
	self->_negation = true;

	return self;
}

bool cl_cnf_add(cl_cnf_t * self, cl_collection_t * clause)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_CNF));
	assert(cl_object_type_check(clause, CL_OBJECT_TYPE_COLLECTION));

	cl_collection_flag_set(clause, CL_COLLECTION_FLAG_UNIQUE);
	return cl_collection_add(self->_set, clause) != SIZE_MAX;
}

/* TODO: */
cl_cnf_t *cl_cnf_construct(cl_proposition_t * proposition)
{
	return NULL;
}

cl_collection_t *cl_cnf_literals(cl_cnf_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_CNF));

	cl_collection_t *res = cl_collection(0, CL_OBJECT_TYPE_CNF_LITERAL,
					     CL_COLLECTION_FLAG_UNIQUE |
					     CL_COLLECTION_FLAG_AUTORESIZE);

	cl_collection_t *set = self->_set;
	for (int i = 0; i < cl_collection_count(set); i++) {
		cl_collection_t *clause = cl_collection_get(set, i);
		for (int j = 0; j < cl_collection_count(clause); j++) {
			cl_cnf_literal_t *lit = cl_collection_get(clause, j);

			if (!lit->_negation) {
				cl_collection_add(res, lit);
			}
		}
	}

	return res;
}

bool cl_cnf_literal_assign(cl_cnf_literal_t * literal, bool value)
{
	assert(cl_object_type_check(literal, CL_OBJECT_TYPE_CNF_LITERAL));

	if (!literal->_negation) {
		literal->_value = value;
		if (literal->_dual) {
			literal->_dual->_value = !value;
		}

		return true;
	}

	return false;
}

bool cl_cnf_literal_value(cl_cnf_literal_t * literal)
{
	assert(cl_object_type_check(literal, CL_OBJECT_TYPE_CNF_LITERAL));
	return literal->_value;
}

bool cl_cnf_evaluate(cl_cnf_t * self)
{
	if (!self) {
		return true;
	}

	assert(cl_object_type_check(self, CL_OBJECT_TYPE_CNF));

	bool res = true;
	cl_collection_t *set = self->_set;

	for (int i = 0; i < cl_collection_count(set); i++) {
		bool subres = false;
		cl_collection_t *clause = cl_collection_get(set, i);

		for (int j = 0; j < cl_collection_count(clause); j++) {
			cl_cnf_literal_t *lit = cl_collection_get(clause, j);
			subres |= lit->_value;

			if (subres) {
				break;
			}
		}

		res &= subres;
		if (!res) {
			break;
		}
	}

	return res;
}
