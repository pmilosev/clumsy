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

typedef struct cl_proposition_s cl_proposition;
typedef bool(*cl_proposition_func) (cl_proposition *);

struct cl_proposition_s {
	cl_proposition_func func;
	void *data;
};

cl_proposition *cl_proposition_new(cl_proposition_func, void *);

#define cl_proposition_eval(p) (p->func ? p->func(p) : false)
#define cl_proposition_destroy(p) free(p)

#endif /* CL_PROPOSITION_H */
