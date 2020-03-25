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
//SectionFDPhysicalProperties.h

#ifndef Joint2DPhysicalProperties_h
#define Joint2DPhysicalProperties_h

#include "UniaxialMatPhysicalProperties.h"
#include "DamageModelVector.h"
#include "SpringModels.h"

namespace XC {

//! @ingroup PhysicalProperties
//
//! @brief Physical properties for shells.
class Joint2DPhysicalProperties: public UniaxialMatPhysicalProperties
  {
  protected:
    DamageModelVector theDamages;
    ID fixedEnd;

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
    void set_springs(const SpringModels &);
    void set_damage_models(const DamageModelVector &);
  public:
    Joint2DPhysicalProperties(const size_t &nMat= 0,const UniaxialMaterial *ptr_mat= nullptr); 
    Joint2DPhysicalProperties(const SpringModels &);
    Joint2DPhysicalProperties(const SpringModels &, const DamageModelVector &);

    inline const DamageModelVector &getDamageModelVector(void) const
      { return theDamages; }
    inline const ID &getFixedEndInfo(void) const
      { return fixedEnd; }

    // public methods to set the state of the properties
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  }; 

} // end of XC namespace
#endif
