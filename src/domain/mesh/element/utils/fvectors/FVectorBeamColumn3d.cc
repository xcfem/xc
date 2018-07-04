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
//FVectorBeamColumn3d.cc

#include "FVectorBeamColumn3d.h"
#include <utility/matrix/Vector.h>

//! @brief Default constructor.
XC::FVectorBeamColumn3d::FVectorBeamColumn3d(void)
  { zero(); }

//! @brief Copy constructor.
XC::FVectorBeamColumn3d::FVectorBeamColumn3d(const XC::FVectorBeamColumn3d &other)
  {
    p[0] = other.p[0];
    p[1] = other.p[1];
    p[2] = other.p[2];
    p[3] = other.p[3];
    p[4] = other.p[4];
  }

//! @brief Constructor a partir de un vector.
XC::FVectorBeamColumn3d::FVectorBeamColumn3d(const XC::Vector &v)
  {
    if(v.Size()>=5)
      {
        p[0] = v[0];
        p[1] = v[1];
        p[2] = v[2];
        p[3] = v[3];
        p[4] = v[4];
      }
    else
      std::cerr << getClassName() << __FUNCTION__
	        << "; a vector of dimension 5 was expected." << std::endl;
  }

//! @brief Assignment operator.
XC::FVectorBeamColumn3d &XC::FVectorBeamColumn3d::operator=(const XC::FVectorBeamColumn3d &other)
  {
    p[0] = other.p[0];
    p[1] = other.p[1];
    p[2] = other.p[2];
    p[3] = other.p[3];
    p[4] = other.p[4];
    return *this;
  }

void XC::FVectorBeamColumn3d::zero(void)
  {
    p[0]= 0.0;
    p[1]= 0.0;
    p[2]= 0.0;
    p[3]= 0.0;
    p[4]= 0.0;
  }

XC::FVectorBeamColumn3d &XC::FVectorBeamColumn3d::operator*=(const double &d)
  {
    p[0]*= d;
    p[1]*= d;
    p[2]*= d;
    p[3]*= d;
    p[4]*= d;
    return *this;    
  }

XC::FVectorBeamColumn3d &XC::FVectorBeamColumn3d::operator+=(const FVectorBeamColumn3d &other)
  {
    p[0]+= other.p[0];
    p[1]+= other.p[1];
    p[2]+= other.p[2];
    p[3]+= other.p[3];
    p[4]+= other.p[4];
    return *this;    
  }

XC::FVectorBeamColumn3d &XC::FVectorBeamColumn3d::operator-=(const FVectorBeamColumn3d &other)
  {
    p[0]-= other.p[0];
    p[1]-= other.p[1];
    p[2]-= other.p[2];
    p[3]-= other.p[3];
    p[4]-= other.p[4];
    return *this;    
  }

void XC::FVectorBeamColumn3d::Print(std::ostream &os) const
  {
    os << '[' << p[0] << ',' << p[1] << ',' << p[2] << ',' << p[3] << ',' << p[4] << ']';
  }

