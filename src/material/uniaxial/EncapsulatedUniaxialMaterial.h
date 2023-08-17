// -*-c++-*-
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

#ifndef EncapsulatedUniaxialMaterial_h
#define EncapsulatedUniaxialMaterial_h

#include "material/uniaxial/UniaxialMaterial.h"
#include "material/uniaxial/UniaxialMaterialWrapper.h"

namespace XC {
//! @ingroup MatUnx
//
//! @brief Encapsulates a copy to an uniaxial material.
class EncapsulatedUniaxialMaterial: public UniaxialMaterial
  {
  protected:
    UniaxialMaterialWrapper theMaterial;

  public:
    EncapsulatedUniaxialMaterial(int tag, int classTag, const UniaxialMaterial &material); 
    EncapsulatedUniaxialMaterial(int tag= 0,int classTag= 0);

    //! @brief Return a pointer to the encapsulated material.
    inline const UniaxialMaterial *getMaterial(void) const
      { return theMaterial.getMaterial(); }
    //! @brief Return a pointer to the encapsulated material.
    inline UniaxialMaterial *getMaterial(void)
      { return theMaterial.getMaterial(); }
    virtual void setMaterial(const UniaxialMaterial &);
    void setMaterial(const std::string &);
    
    double getStrain(void) const;          
    double getStrainRate(void) const;
    
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  };
} // end of XC namespace


#endif

