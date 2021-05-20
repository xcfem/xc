// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.  
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//mchne_eps.h
//Epsilon de la máquina

#ifndef MCHNE_EPS_H
#define MCHNE_EPS_H

#include "matem.h"
#include <limits>

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

static const double VARIABLE_IS_NOT_USED mchne_eps_flt= std::numeric_limits<float>::epsilon(); //machine_epsilon((const float &) 0.0);
static const double VARIABLE_IS_NOT_USED mchne_eps_dbl= std::numeric_limits<double>::epsilon(); //machine_epsilon((const double &) 0.0);
static const double VARIABLE_IS_NOT_USED mchne_eps_ldbl= std::numeric_limits<long double>::epsilon(); //machine_epsilon((const long double &) 0.0);

static const double VARIABLE_IS_NOT_USED sqrt_mchne_eps_flt= sqrt(mchne_eps_flt);
static const double VARIABLE_IS_NOT_USED sqrt_mchne_eps_dbl= sqrt(mchne_eps_dbl);
static const double VARIABLE_IS_NOT_USED sqrt_mchne_eps_ldbl= sqrt(mchne_eps_ldbl);

#endif
