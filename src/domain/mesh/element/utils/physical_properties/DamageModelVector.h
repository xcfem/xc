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
//  under the terms of the GNU General Public License published by 
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
//DamageModelVector.h

#ifndef DamageModelVector_h
#define DamageModelVector_h

#include "DamageModels.h"
#include "material/MaterialVector.h"
#include "material/uniaxial/UniaxialMaterial.h"
#include "xc_utils/src/kernel/CommandEntity.h"
#include "utility/actor/actor/MovableID.h"


namespace XC {

//! @ingroup damage
//
//! @brief Vector of pointers to damage models.
//! it's used inJoint2D
class DamageModelVector: public DamageModels, public CommandEntity, public MovableObject
  {
  protected:
    void clearAll(void);

    DbTagData &getDbTagData(void) const;
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public:
    typedef MaterialVector<UniaxialMaterial> material_vector;

    DamageModelVector(const size_t &nDamageModels,const DamageModel *dmgModel= nullptr);
    ~DamageModelVector(void)
      { clearAll(); }

    int commitState(const material_vector &);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };



} // end of XC namespace

#endif
