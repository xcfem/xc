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
//inertia.cc

#include "inertia.h"
#include <iostream>

//! @brief Return the angle that defines a principal axes of inertia.
double theta_inertia(const double &Ix,const double &Iy,const double &Pxy)
  {
    const double tol= std::numeric_limits<double>::epsilon();
    const double eps= abs(Ix-Iy)/std::max(Ix,Iy);
    if(eps<tol)
      if(abs(Pxy)<tol)
	return 0.0;
      else
        if(Pxy<0)
	  return M_PI/4.0;
        else
	  return -M_PI/4.0;
    else
      return (atan(-2*Pxy/(Ix-Iy)))/2.0;
  }
