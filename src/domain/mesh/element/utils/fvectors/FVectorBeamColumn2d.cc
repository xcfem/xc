//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//FVectorBeamColumn2d.cc

#include "FVectorBeamColumn2d.h"
#include <utility/matrix/Vector.h>

//! @brief Default constructor.
XC::FVectorBeamColumn2d::FVectorBeamColumn2d(void)
  { zero(); }

//! @brief Copy constructor.
XC::FVectorBeamColumn2d::FVectorBeamColumn2d(const XC::FVectorBeamColumn2d &otro)
  {
    p[0] = otro.p[0];
    p[1] = otro.p[1];
    p[2] = otro.p[2];
  }

//! @brief Constructor a partir de un vector.
XC::FVectorBeamColumn2d::FVectorBeamColumn2d(const XC::Vector &v)
  {
    if(v.Size()>=3)
      {
        p[0] = v[0];
        p[1] = v[1];
        p[2] = v[2];
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; vector of dimension 3 was expected." << std::endl;
  }

//! @brief Assignment operator.
XC::FVectorBeamColumn2d &XC::FVectorBeamColumn2d::operator=(const XC::FVectorBeamColumn2d &otro)
  {
    p[0] = otro.p[0];
    p[1] = otro.p[1];
    p[2] = otro.p[2];
    return *this;
  }

void XC::FVectorBeamColumn2d::zero(void)
  {
    p[0]= 0.0;
    p[1]= 0.0;
    p[2]= 0.0;
  }

XC::FVectorBeamColumn2d &XC::FVectorBeamColumn2d::operator*=(const double &d)
  {
    p[0]*= d;
    p[1]*= d;
    p[2]*= d;
    return *this;    
  }

XC::FVectorBeamColumn2d &XC::FVectorBeamColumn2d::operator+=(const FVectorBeamColumn2d &otro)
  {
    p[0]+= otro.p[0];
    p[1]+= otro.p[1];
    p[2]+= otro.p[2];
    return *this;    
  }

XC::FVectorBeamColumn2d &XC::FVectorBeamColumn2d::operator-=(const FVectorBeamColumn2d &otro)
  {
    p[0]-= otro.p[0];
    p[1]-= otro.p[1];
    p[2]-= otro.p[2];
    return *this;    
  }

void XC::FVectorBeamColumn2d::Print(std::ostream &os) const
  { os << '[' << p[0] << ',' << p[1] << ',' << p[2] << ']'; }
