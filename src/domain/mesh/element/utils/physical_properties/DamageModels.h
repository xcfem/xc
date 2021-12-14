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

#ifndef DamageModels_h
#define DamageModels_h


#include <vector>

namespace XC {
class DamageModel;

//! @ingroup ElemJoint
//!
//! @brief Damage models for Joint elements.
class DamageModels: protected std::vector<DamageModel *>
  {
  private:
    void alloc(const DamageModels &);
    void alloc(const DamageModel *);
    void free(void);
  protected:
    inline reference operator[](size_t i)
      { return DamageModelsBase::operator[](i); }    
  public:
    typedef std::vector<DamageModel *> DamageModelsBase;
    DamageModels(const size_t &sz= 5, const DamageModel *ptr= nullptr);
    DamageModels(const DamageModels &);
    ~DamageModels(void);
    DamageModels &operator=(const DamageModels &);
    bool empty(void) const;
    void clear(void);
    void setDamageModel(const DamageModel *);
    void setDamageModel(size_t i,DamageModel *);

    inline const_reference operator[](size_t i) const
      { return DamageModelsBase::operator[](i); }
    
    int revertToLastCommit(void);
    int revertToStart(void);
  };
  
} // end of XC namespace

#endif
