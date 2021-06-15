// -*-c++-*-
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
//EPPBaseMaterial.h

#ifndef EPPBaseMaterial_h
#define EPPBaseMaterial_h

#include <material/uniaxial/ElasticBaseMaterial.h>

namespace XC {
//! @ingroup MatUnx
//
//! @brief Base class for elastic perfectly plastic materials.
class EPPBaseMaterial: public ElasticBaseMaterial
  {
  protected:
    double trialStress; //!< current trial stress
    double trialTangent; //!< current trial tangent
    int sendData(Communicator &);
    int recvData(const Communicator &);

  public:
    EPPBaseMaterial(int tag, int classtag, double E= 0.0, double e0= 0.0);


    inline double getStress(void) const
      { return trialStress; }
    inline double getTangent(void) const
      { return trialTangent; }


  };
} // end of XC namespace


#endif
