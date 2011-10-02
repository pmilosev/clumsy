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
#include "cl_object.h"
#include "cl_proposition.h"
#include "cl_proposition_rep.h"

static void destructor(cl_object * obj)
{
	cl_proposition *p = (cl_proposition *) obj;

	/** the context of the atomic propositions is managed outside of the proposition. */
	if (p->_depth) {
		cl_object_release((cl_object *) p->_context.argv[0]);
		cl_object_release((cl_object *) p->_context.argv[1]);
	}
}

cl_proposition *cl_proposition_init(cl_proposition_operator op, ...)
{
	va_list ap;
	size_t argc = 1;

	/* initialize the object */
	cl_proposition *res =
	    (cl_proposition *) cl_object_init(sizeof(cl_proposition),
					      &destructor);

	/* set the operator (NULL will evaluate to FALSE) */
	res->_context.op = op;

	/* expecting 2 arguments for OR and AND, 1 argument for everything else */
	if ((op == &cl_proposition_and_op)
	    || (op == &cl_proposition_or_op)) {
		argc = 2;
	}

	/* load the arguments */
	va_start(ap, op);
	res->_context.argv[0] = argc > 0 ? va_arg(ap, void *) : NULL;
	res->_context.argv[1] = argc > 1 ? va_arg(ap, void *) : NULL;
	va_end(ap);

	/* if this is a brand new proposition set the depth to 0 */
	res->_depth = 0;

	/* in a case of binary operator */
	if ((op == &cl_proposition_and_op)
	    || (op == &cl_proposition_or_op)) {

		/* retain the arguments */
		cl_object_retain((cl_object *) res->_context.argv[0]);
		cl_object_retain((cl_object *) res->_context.argv[1]);

		res->_depth =
		    ((cl_proposition *) res->_context.argv[0])->_depth + 1;
		size_t depth2 =
		    ((cl_proposition *) res->_context.argv[1])->_depth + 1;

		/* swap the arguments if the second one is with smaller depth 
		 * this might increase evaluation performance in cases where the
		 * result from the first argument defines the final result 
		 * (eg. TRUE || arg2) */
		if (res->_depth > depth2) {
			void *temp = res->_context.argv[0];
			res->_context.argv[0] = res->_context.argv[1];
			res->_context.argv[1] = temp;
		} else {
			/* depth always represents the worst-case scenario */
			res->_depth = depth2;
		}
	} else if (op == &cl_proposition_not_op) {
		cl_object_retain((cl_object *) res->_context.argv[0]);
		res->_depth =
		    ((cl_proposition *) res->_context.argv[0])->_depth + 1;
	}

	return res;
}

cl_proposition_context *cl_proposition_get_context(cl_proposition * p)
{
	if (!p) {
		return NULL;
	}

	return &(p->_context);
}

bool cl_proposition_eval(cl_proposition * p)
{
	if (!p) {
		return false;
	}

	return p->_context.op ? p->_context.op(p) : false;
}

bool cl_proposition_not_op(cl_proposition * self)
{
	return !cl_proposition_eval(self->_context.argv[0]);
}

bool cl_proposition_and_op(cl_proposition * self)
{
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 && r2;
}

bool cl_proposition_or_op(cl_proposition * self)
{
	bool r1 = cl_proposition_eval(self->_context.argv[0]);
	bool r2 = cl_proposition_eval(self->_context.argv[1]);

	return r1 || r2;
}
