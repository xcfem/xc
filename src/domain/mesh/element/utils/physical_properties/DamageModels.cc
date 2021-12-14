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

#include "DamageModels.h"
#include "material/damage/DamageModel.h"

//! @brief Free memory.
void XC::DamageModels::free(void)
  {
    for(iterator i= begin();i!=end();i++)
      {
	DamageModel *spring= *i;
	if(spring)
	  delete spring;
      }
    clear();
  }

//! @brief Copy materials from the argument.
void XC::DamageModels::alloc(const DamageModels &other)
  {
    free();
    const size_t sz= other.size();
    resize(sz,nullptr);
    for(size_t i= 0;i<sz;i++)
      {
        (*this)[i]= other[i]->getCopy();
        if(!(*this)[i])
          std::cerr << ":DamageModels::" << __FUNCTION__
	            << "; failed allocate damage model pointer\n";
      }
  }

//! @brief Copy materials from the argument.
void XC::DamageModels::alloc(const DamageModel *dmgModel)
  {
    const size_t sz= size();
    free();
    resize(sz,nullptr);
    if(dmgModel)
      {
	for(size_t i= 0;i<sz;i++)
	  {
	    (*this)[i]= dmgModel->getCopy();
	    if(!(*this)[i])
	      std::cerr << "DamageModels::" << __FUNCTION__
			<< "; failed allocate damage model pointer\n";
	  }
      }
  }

//! @brief Constructor.
XC::DamageModels::DamageModels(const size_t &sz, const DamageModel *dmgModel)
  : DamageModelsBase(5,nullptr)
  { alloc(dmgModel); }

//! @brief Copy constructor.
XC::DamageModels::DamageModels(const DamageModels &other)
  : DamageModelsBase(other.size(),nullptr)
  { alloc(other); }

//! @brief Assignment operator.
XC::DamageModels &XC::DamageModels::operator=(const DamageModels &other)
  {
    alloc(other);
    return *this;
  }

//! @brief Destructor.
XC::DamageModels::~DamageModels(void)
  { clear(); }

//! @brief Clears vector.
void XC::DamageModels::clear(void)
  { free(); }

//! @brief Returns true if no damage model has been assigned.
bool XC::DamageModels::empty(void) const
  {
    if(DamageModelsBase::empty())
      return true;
    else
      return ((*this)[0]==nullptr);
  }

//! @brief Set value for all the damage models.
void XC::DamageModels::setDamageModel(const DamageModel *new_dmg)
  { alloc(new_dmg); }

//! @brief Set value for i-th damage model.
void XC::DamageModels::setDamageModel(size_t i,DamageModel *dmg)
  {
    if((*this)[i])
      delete (*this)[i];
    (*this)[i]= dmg;
  }

//! @brief Returns the damage models to its last committed state.
int XC::DamageModels::revertToLastCommit(void)
  {
    int retVal= 0;

    for(iterator i= begin();i!= end();i++)
      {
        DamageModel *dmg= *i;
        if(dmg)
         retVal+= dmg->revertToLastCommit();
        //if(retval!=0) break;
      }
    return retVal;
  }

//! @brief Returns the estado de los dmg_models al inicial.
int XC::DamageModels::revertToStart(void)
  {
    int retVal = 0;

    for(iterator i= begin();i!= end();i++)
      {
        DamageModel *dmg= *i;
        if(dmg)
         retVal+= dmg->revertToStart();
        //if(retval!=0) break;
      }
    return retVal;
  }
