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
//Giros.h
//Functions that create rotation matrices.

#ifndef GIROS_H
#define GIROS_H

#include "ZMatrix.h"

//En dos dimensiones.
ZMatrix<double> Giro2D(double ang);

//En tres dimensiones
ZMatrix<double> GiroX(double ang);
ZMatrix<double> GiroY(double ang);
ZMatrix<double> GiroZ(double ang);
ZMatrix<double> GiroXYZ(double rx,double ry,double rz);


#endif
