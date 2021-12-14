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
//inertia.h

#ifndef INERTIA_H
#define INERTIA_H

#include <cmath>
#include "matem.h"

double theta_inertia(const double &Ix,const double &Iy,const double &Pxy);

//! @brief Auxiliary function to compute principal moments of inertia.
inline double R_inertia(const double &Ix,const double &Iy,const double Pxy)
  { return sqrt(sqr((Ix-Iy)/2)+sqr(Pxy)); }

//! @brief Return the principal major moment of inertia.
inline double I1_inertia(const double &iy,const double &iz,const double &pyz)
  { return (iy+iz)/2+R_inertia(iy,iz,pyz); }

//! @brief Return the principal minor moment of inertia.
inline double I2_inertia(const double &iy,const double &iz,const double &pyz)
  { return (iy+iz)/2-R_inertia(iy,iz,pyz); }


#endif
