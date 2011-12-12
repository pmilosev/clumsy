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
#include <string.h>
#include <assert.h>
#include "cl_proposition.h"
#include "cl_proposition_rep.h"

static void destructor(void *self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	cl_proposition_t *p = (cl_proposition_t *) self;

	/** the context of the atomic propositions is managed outside of the proposition. */
	if (p->_depth) {
		cl_object_release(p->_context.argv[0]);
		cl_object_release(p->_context.argv[1]);
	}
}

static char *print_complex_proposition(cl_proposition_t * p, char *op)
{
	char *prop1 = cl_object_to_string(p->_context.argv[0]);
	char *prop2 = cl_object_to_string(p->_context.argv[1]);
	size_t len1 = strlen(prop1);
	size_t len2 = strlen(prop2);
	size_t lenop = strlen(op);
	size_t len = len1 + len2 + lenop + 5;
	char *buffer = malloc(sizeof(char) * len);
	assert(buffer);

	strcpy(buffer, "(");
	strcpy(buffer + 1, prop1);
	strcpy(buffer + len1 + 1, " ");
	strcpy(buffer + len1 + 2, op);
	strcpy(buffer + len1 + lenop + 2, " ");
	strcpy(buffer + len1 + lenop + 3, prop2);
	strcpy(buffer + len1 + lenop + len2 + 3, ")");

	free(prop1);
	free(prop2);

	return buffer;
}

static char *print_negation(char *prop)
{
	size_t len = strlen(prop) + 2;
	char *buffer = malloc(sizeof(char) * len);
	assert(buffer);

	strcpy(buffer, "~");
	strcpy(buffer + 1, prop);

	return buffer;
}

static char *proposition_printer(void *self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	cl_proposition_t *p = (cl_proposition_t *) self;

	char *buffer = NULL;
	if (p->_context.op == cl_proposition_true_op) {
		buffer = malloc(sizeof(char) * 5);
		assert(buffer);
		strcpy(buffer, "TRUE");
	} else if (p->_context.op == cl_proposition_false_op) {
		buffer = malloc(sizeof(char) * 6);
		assert(buffer);
		strcpy(buffer, "FALSE");
	} else if (p->_context.op == cl_proposition_not_op) {
		char *temp = cl_object_to_string(p->_context.argv[0]);
		buffer = print_negation(temp);
		free(temp);
	} else if (p->_context.op == cl_proposition_and_op) {
		buffer = print_complex_proposition(p, "^");
	} else if (p->_context.op == cl_proposition_or_op) {
		buffer = print_complex_proposition(p, "v");
	} else if (p->_context.op == cl_proposition_imply_op) {
		buffer = print_complex_proposition(p, "=>");
	} else if (p->_context.op == cl_proposition_equivalent_op) {
		buffer = print_complex_proposition(p, "<=>");
	} else if (p->_context.op == cl_proposition_xor_op) {
		buffer = print_complex_proposition(p, "+");
	} else if (p->_context.op == cl_proposition_nand_op) {
		char *temp = print_complex_proposition(p, "^");
		buffer = print_negation(temp);
		free(temp);
	} else if (p->_context.op == cl_proposition_nor_op) {
		char *temp = print_complex_proposition(p, "v");
		buffer = print_negation(temp);
		free(temp);
	} else if (p->_context.op == cl_proposition_nimply_op) {
		char *temp = print_complex_proposition(p, "=>");
		buffer = print_negation(temp);
		free(temp);
	} else {
		buffer = cl_object_printer(self);
	}

	return buffer;
}

cl_proposition_t *cl_proposition_new(cl_proposition_operator_t op, ...)
{
	va_list ap;
	size_t argc = 1;
	bool formula = true;

	/* find the right number of arguments we are expecting */
	if (op == cl_proposition_true_op) {
		argc = 0;
	} else if (op == cl_proposition_false_op) {
		argc = 0;
	} else if (op == cl_proposition_not_op) {
		argc = 1;
	} else if (op == cl_proposition_and_op) {
		argc = 2;
	} else if (op == cl_proposition_or_op) {
		argc = 2;
	} else if (op == cl_proposition_imply_op) {
		argc = 2;
	} else if (op == cl_proposition_equivalent_op) {
		argc = 2;
	} else if (op == cl_proposition_xor_op) {
		argc = 2;
	} else if (op == cl_proposition_nand_op) {
		argc = 2;
	} else if (op == cl_proposition_nor_op) {
		argc = 2;
	} else if (op == cl_proposition_nimply_op) {
		argc = 2;
	} else {
		formula = false;
	}

	/* initialize the object */
	cl_proposition_t *res =
	    cl_object_new(sizeof(cl_proposition_t), CL_OBJECT_TYPE_PROPOSITION,
			  &destructor, &proposition_printer);

	/* set the operator (NULL will evaluate to FALSE) */
	res->_context.op = op;

	/* load the arguments */
	va_start(ap, op);
	res->_context.argv[0] = argc > 0 ? va_arg(ap, void *) : NULL;
	res->_context.argv[1] = argc > 1 ? va_arg(ap, void *) : NULL;
	va_end(ap);

	/* if atomic proposition, set the depth  to 0 and return  */
	if (!formula) {
		res->_depth = 0;
		return res;
	}

	/* retain the arguments */
	cl_object_retain(res->_context.argv[0]);
	cl_object_retain(res->_context.argv[1]);

	/* in a case of a forumla the depth should be set pesimistic */
	size_t depth0 =
	    res->_context.argv[0] ? ((cl_proposition_t *) res->_context.
				     argv[0])->_depth + 1 : 0;
	size_t depth1 =
	    res->_context.argv[1] ? ((cl_proposition_t *) res->_context.
				     argv[1])->_depth + 1 : 0;
	res->_depth = depth0 > depth1 ? depth0 : depth1;

	/* finally, if the operator is comutative
	 * set the operand with smaller depth first 
	 * in a hope that it would be sufficient for the final result */
	if (op == cl_proposition_and_op
	    || op == cl_proposition_or_op
	    || op == cl_proposition_nand_op || op == cl_proposition_nor_op) {
		if (depth0 > depth1) {
			void *temp = res->_context.argv[0];
			res->_context.argv[0] = res->_context.argv[1];
			res->_context.argv[1] = temp;
		}
	}

	return res;
}

cl_proposition_context_t *cl_proposition_get_context(cl_proposition_t * p)
{
	if (!p) {
		return NULL;
	}

	assert(cl_object_type_check(p, CL_OBJECT_TYPE_PROPOSITION));
	return &(p->_context);
}

bool cl_proposition_eval(cl_proposition_t * p)
{
	if (!p) {
		return false;
	}

	assert(cl_object_type_check(p, CL_OBJECT_TYPE_PROPOSITION));
	return p->_context.op ? p->_context.op(p) : false;
}

bool cl_proposition_true_op(cl_proposition_t * self)
{
	return true;
}

bool cl_proposition_false_op(cl_proposition_t * self)
{
	return false;
}

bool cl_proposition_not_op(cl_proposition_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	return !cl_proposition_eval(self->_context.argv[0]);
}

bool cl_proposition_and_op(cl_proposition_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 && r2;
}

bool cl_proposition_or_op(cl_proposition_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 || r2;
}

bool cl_proposition_imply_op(cl_proposition_t * self)
{
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return !r1 || r2;
}

bool cl_proposition_equivalent_op(cl_proposition_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 == r2;
}

bool cl_proposition_xor_op(cl_proposition_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 != r2;
}

bool cl_proposition_nand_op(cl_proposition_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return !(r1 && r2);
}

bool cl_proposition_nor_op(cl_proposition_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return !(r1 || r2);
}

bool cl_proposition_nimply_op(cl_proposition_t * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 && !r2;
}
