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
#include "cl_proposition.h"
#include "cl_proposition_rep.h"

static void destructor(cl_object * obj)
{
	assert(cl_object_type_check(obj, CL_OBJECT_TYPE_PROPOSITION));
	cl_proposition *p = (cl_proposition *) obj;

	/** the context of the atomic propositions is managed outside of the proposition. */
	if (p->_depth) {
		cl_object_release(p->_context.argv[0]);
		cl_object_release(p->_context.argv[1]);
	}
}

cl_proposition *cl_proposition_init(cl_proposition_operator op, ...)
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
	cl_object *obj = cl_object_init(sizeof(cl_proposition), &destructor);
	obj->_obj_info._type |= CL_OBJECT_TYPE_PROPOSITION;

	/* set the operator (NULL will evaluate to FALSE) */
	cl_proposition *res = (cl_proposition *) obj;
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
	size_t depth0 = res->_context.argv[0] ? ((cl_proposition *) res->_context.argv[0])->_depth + 1 : 0;
	size_t depth1 = res->_context.argv[1] ? ((cl_proposition *) res->_context.argv[1])->_depth + 1 : 0;
	res->_depth = depth0 > depth1 ? depth0 : depth1;

	/* finally, if the operator is comutative
	 * set the operand with smaller depth first 
	 * in a hope that it would be sufficient for the final result */
	if (op == cl_proposition_and_op
			|| op == cl_proposition_or_op
			|| op == cl_proposition_nand_op
			|| op == cl_proposition_nor_op) {
		if (depth0 > depth1) {
			void *temp = res->_context.argv[0];
			res->_context.argv[0] = res->_context.argv[1];
			res->_context.argv[1] = temp;
		}
	}

	return res;
}

cl_proposition_context *cl_proposition_get_context(cl_proposition * p)
{
	if (!p) {
		return NULL;
	}

	assert(cl_object_type_check(p, CL_OBJECT_TYPE_PROPOSITION));
	return &(p->_context);
}

bool cl_proposition_eval(cl_proposition * p)
{
	if (!p) {
		return false;
	}

	assert(cl_object_type_check(p, CL_OBJECT_TYPE_PROPOSITION));
	return p->_context.op ? p->_context.op(p) : false;
}

bool cl_proposition_true_op(cl_proposition * self)
{
	return true;
}

bool cl_proposition_false_op(cl_proposition * self)
{
	return false;
}

bool cl_proposition_not_op(cl_proposition * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	return !cl_proposition_eval(self->_context.argv[0]);
}

bool cl_proposition_and_op(cl_proposition * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 && r2;
}

bool cl_proposition_or_op(cl_proposition * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 || r2;
}

bool cl_proposition_imply_op(cl_proposition * self)
{
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return !r1 || r2;
}

bool cl_proposition_equivalent_op(cl_proposition * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 == r2;
}

bool cl_proposition_xor_op(cl_proposition * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 != r2;
}

bool cl_proposition_nand_op(cl_proposition * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return !(r1 && r2);
}

bool cl_proposition_nor_op(cl_proposition * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return !(r1 || r2);
}

bool cl_proposition_nimply_op(cl_proposition * self)
{
	assert(cl_object_type_check(self, CL_OBJECT_TYPE_PROPOSITION));
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 && !r2;
}
