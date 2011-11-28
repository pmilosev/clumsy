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

#ifndef CL_CNF_REP_H
#define CL_CNF_REP_H

#include "cl_object_rep.h"

struct cl_cnf_s {
	cl_object_info_t _obj_info;
	cl_collection_t *_set;
};

struct cl_cnf_literal_s {
	cl_object_info_t _obj_info;
	cl_proposition_t *_proposition;
	cl_cnf_literal_t *_dual;
	bool _negation;
	bool _value;
};

#endif				/* CL_CNF_REP_H */
