//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//FVectorQuad.cc

#include "FVectorQuad.h"
#include <utility/matrix/Vector.h>

//! @brief Default constructor.
XC::FVectorQuad::FVectorQuad(void)
  : FVectorData<8>() { zero(); }

//! @brief Copy constructor.
XC::FVectorQuad::FVectorQuad(const FVectorQuad &other)
  : FVectorData<8>(other)
  {
    p[0]= other.p[0];
    p[1]= other.p[1];
    p[2]= other.p[2];
    p[3]= other.p[3];
    p[4]= other.p[4];
    p[5]= other.p[5];
    p[6] = other.p[6];
    p[7] = other.p[7];
  }

//! @brief Constructor a partir de un vector.
XC::FVectorQuad::FVectorQuad(const XC::Vector &v)
  {
    if(v.Size()>=8)
      {
        p[0] = v[0];
        p[1] = v[1];
        p[2] = v[2];
        p[3] = v[3];
        p[4] = v[4];
        p[5] = v[5];
        p[6] = v[6];
        p[7] = v[7];
      }
    else
      std::cerr << getClassName() << __FUNCTION__
	        << "; a vector of dimension 6 was expected." << std::endl;
  }

//! @brief Assignment operator.
XC::FVectorQuad &XC::FVectorQuad::operator=(const FVectorQuad &other)
  {
    p[0] = other.p[0];
    p[1] = other.p[1];
    p[2] = other.p[2];
    p[3] = other.p[3];
    p[4] = other.p[4];
    p[5] = other.p[5];
    p[6] = other.p[6];
    p[7] = other.p[7];
    return *this;
  }

void XC::FVectorQuad::zero(void)
  {
    p[0]= 0.0;
    p[1]= 0.0;
    p[2]= 0.0;
    p[3]= 0.0;
    p[4]= 0.0;
    p[5]= 0.0;
    p[6]= 0.0;
    p[7]= 0.0;
  }

XC::FVectorQuad &XC::FVectorQuad::operator*=(const double &d)
  {
    p[0]*= d;
    p[1]*= d;
    p[2]*= d;
    p[3]*= d;
    p[4]*= d;
    p[5]*= d;
    p[6]*= d;
    p[7]*= d;
    return *this;    
  }

XC::FVectorQuad &XC::FVectorQuad::operator+=(const FVectorQuad &other)
  {
    p[0]+= other.p[0];
    p[1]+= other.p[1];
    p[2]+= other.p[2];
    p[3]+= other.p[3];
    p[4]+= other.p[4];
    p[5]+= other.p[5];
    p[6]+= other.p[6];
    p[7]+= other.p[7];
    return *this;    
  }

XC::FVectorQuad &XC::FVectorQuad::operator-=(const FVectorQuad &other)
  {
    p[0]-= other.p[0];
    p[1]-= other.p[1];
    p[2]-= other.p[2];
    p[3]-= other.p[3];
    p[4]-= other.p[4];
    p[5]-= other.p[5];
    p[6]-= other.p[6];
    p[7]-= other.p[7];
    return *this;    
  }

//! @brief Adds the force being passed as parameter.
void XC::FVectorQuad::addForce(const size_t &inod,const double &P1,const double &P2)
  {
    if(inod>3)
      std::cerr << "FVectorQuad::addForce; node: "
                << inod << "out of range (0..3)." << std::endl;
    const size_t i= inod*2;
    p[i]-= P1;  //Node 1: i=0. Node 2: i=6. Node 3: i= 12. Node 4: i= 18
    p[i+1]-= P2;
  }

void XC::FVectorQuad::Print(std::ostream &os) const
  {
    os << '[' << p[0] << ',' << p[1] << ',' << p[2] << ',' << p[3] << ',' << p[4] << ','
              << p[5] << ',' << p[6] << ',' << p[7] << ']';
  }

