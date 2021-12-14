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
//EsfBeamColumn3d.cc

#include "EsfBeamColumn3d.h"
#include "utility/actor/actor/MovableVector.h"

//! @brief Default constructor.
XC::EsfBeamColumn3d::EsfBeamColumn3d(void)
  : Vector(6) {}

//! @brief Constructor.
XC::EsfBeamColumn3d::EsfBeamColumn3d(const Vector &v)
  : Vector(6)
  {
    const size_t sz= v.Size();
    if(sz>=6)
      for(int i=0;i<6;i++)
        (*this)[i]= v[i];
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << " vector of dimension 6 expected "
                << v << " was obtained." << std::endl;
  }

//! @brief Copy constructor.
XC::EsfBeamColumn3d::EsfBeamColumn3d(const XC::EsfBeamColumn3d &other)
  : Vector(other) {}

//! @brief Assignment operator.
XC::EsfBeamColumn3d &XC::EsfBeamColumn3d::operator=(const EsfBeamColumn3d &other)
  {
    Vector::operator=((const Vector &)(other));
    return *this;
  }

int XC::sendEsfBeamColumn3d(const EsfBeamColumn3d &esf,int posDbTag,DbTagData &dt,Communicator &comm)
  { return comm.sendVector(esf,dt,CommMetaData(posDbTag)); }

int XC::receiveEsfBeamColumn3d(EsfBeamColumn3d &esf,int posDbTag,DbTagData &dt,const Communicator &comm)
  {
    Vector tmp= esf;
    int res= comm.receiveVector(tmp,dt,CommMetaData(posDbTag));
    esf= EsfBeamColumn3d(tmp);
    return res;
  }
