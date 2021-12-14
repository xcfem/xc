// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//Carsten_Steger.h

#ifndef CARSTEN_STEGER_H
#define CARSTEN_STEGER_H

#include "utility/utils/misc_utils/matem.h"
#include "../../cgal_types.h"

//Aux functions for computing moments of arbitrary order  
//(0,1,2,3,...) of a polygon according to the algorithm
//según el algoritmo described by Carsten Steger
//in the document

// On the calculation of arbitrary moments of polygons
// Carsten Steger
// Technical Report FGBV-96-05
// October 1996

class PolygonalSurface2d;

GEOM_FT p_q_moment(const PolygonalSurface2d &sp,const int &p, const int &q);


#endif







