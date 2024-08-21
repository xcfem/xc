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
//FVectorBrick.cc

#include "FVectorBrick.h"
#include <utility/matrix/Vector.h>

//! @brief Default constructor.
XC::FVectorBrick::FVectorBrick(void)
  : FVectorData<24>() { zero(); }

//! @brief Copy constructor.
XC::FVectorBrick::FVectorBrick(const FVectorBrick &other)
  : FVectorData<24>(other)
  {
    for(int i= 0; i<24; i++)
      p[i]= other.p[i];
  }

//! @brief Constructor a partir de un vector.
XC::FVectorBrick::FVectorBrick(const XC::Vector &v)
  {
    if(v.Size()>=24)
      {
	for(int i= 0; i<24; i++)
	  p[i] = v[i];
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; a vector of dimension 6 was expected."
		<< std::endl;
  }

//! @brief Assignment operator.
XC::FVectorBrick &XC::FVectorBrick::operator=(const FVectorBrick &other)
  {
    for(int i= 0; i<24; i++)
      p[i] = other.p[i];
    return *this;
  }

void XC::FVectorBrick::zero(void)
  {
    for(int i= 0; i<24; i++)
      p[i]= 0.0;
  }

XC::FVectorBrick &XC::FVectorBrick::operator*=(const double &d)
  {
    for(int i= 0; i<24; i++)
      p[i]*= d;
    return *this;    
  }

XC::FVectorBrick &XC::FVectorBrick::operator+=(const FVectorBrick &other)
  {
    for(int i= 0; i<24; i++)
      p[i]+= other.p[i];
    return *this;    
  }

XC::FVectorBrick &XC::FVectorBrick::operator-=(const FVectorBrick &other)
  {
    for(int i= 0; i<24; i++)
      p[i]-= other.p[i];
    return *this;    
  }

//! @brief Adds the force being passed as parameter.
void XC::FVectorBrick::addForce(const size_t &inod,const double &P1,const double &P2, const double &P3)
  {
    if(inod>7)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; node: " << inod
		<< "out of range (0..7)." << std::endl;
    const size_t i= inod*3;
    p[i]-= P1;  //Node 1: i=0. Node 2: i=3. Node 3: i= 6. Node 4: i= 9 ...
    p[i+1]-= P2;
    p[i+2]-= P3;
  }

void XC::FVectorBrick::Print(std::ostream &os) const
  {
    os << '[' << p[0] << ',' << p[1] << ',' << p[2]
       << ',' << p[3] << ',' << p[4] << ',' << p[5]
       << ',' << p[6] << ',' << p[7] << ']';
  }

