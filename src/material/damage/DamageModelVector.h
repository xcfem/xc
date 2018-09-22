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

#include "material/MaterialVector.h"
#include "material/uniaxial/UniaxialMaterial.h"
#include "xc_utils/src/kernel/CommandEntity.h"
#include "utility/actor/actor/MovableID.h"
#include "utility/matrix/Vector.h"


namespace XC {

class DamageModel;

//! @ingroup damage
//
//! @brief Vector de pointers to damage models.
//! se emplea en Joint2D
class DamageModelVector: public std::vector<DamageModel *>, public CommandEntity, public MovableObject
  {
  protected:
    void borra_dmg_models(void);
    void clearAll(void);
    void alloc(const std::vector<DamageModel *> &);

    DbTagData &getDbTagData(void) const;
    int sendData(CommParameters &);  
    int recvData(const CommParameters &);
  public:
    typedef MaterialVector<UniaxialMaterial> material_vector;
    typedef typename std::vector<DamageModel *> dmg_model_vector;
    typedef typename dmg_model_vector::iterator iterator;
    typedef typename dmg_model_vector::reference reference;
    typedef typename dmg_model_vector::const_reference const_reference;

    DamageModelVector(const size_t &nDamageModels,const DamageModel *dmgModel= nullptr);
    DamageModelVector(const DamageModelVector &);
    DamageModelVector &operator=(const DamageModelVector &);
    ~DamageModelVector(void)
      { clearAll(); }

    void setDamageModel(const DamageModel *);
    void setDamageModel(size_t i,DamageModel *);
    bool empty(void) const;
    int commitState(const material_vector &);
    int revertToLastCommit(void);
    int revertToStart(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };



} // end of XC namespace

#endif
