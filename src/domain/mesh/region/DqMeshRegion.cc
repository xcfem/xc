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
//DqMeshRegion.cc

#include "DqMeshRegion.h"
#include "MeshRegion.h" 

//! @brief Release memory for every position.
void XC::DqMeshRegion::free_mem(void)
  {
    const size_t sz= size();
    for(size_t i= 0;i<sz;i++)
      {
        if((*this)[i])
          delete (*this)[i];
       (*this)[i]= nullptr;
      }
  }

//! @brief Constructor.
XC::DqMeshRegion::DqMeshRegion(void)
  : dq_Reg() {}

//! @brief Copy constructor.
XC::DqMeshRegion::DqMeshRegion(const DqMeshRegion  &otro)
  : dq_Reg(otro)
  {
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      {
        const MeshRegion *tmp= otro[i];
        if(tmp)
          (*this)[i]= tmp->getCopy();
      }
  }

//! @brief Assignment operator.
XC::DqMeshRegion &XC::DqMeshRegion::operator=(const DqMeshRegion &otro)
  {
    resize(otro.size());
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      {
        const MeshRegion *tmp= otro[i];
        if(tmp)
          (*this)[i]= tmp->getCopy();
      }
    return *this;
  }

//! @brief Destructor.
XC::DqMeshRegion::~DqMeshRegion(void)
  { free_mem(); }


//! @brief Changes the container size.
void XC::DqMeshRegion::resize(const size_t sz)
  {
    free_mem();
    dq_Reg::resize(sz);
    for(size_t i= 0;i<sz;i++)
      (*this)[i]= nullptr;
  }

void XC::DqMeshRegion::add(const MeshRegion &c)
  { push_back(c.getCopy()); }


XC::MeshRegion *XC::DqMeshRegion::getRegion(int tag)
  {
    MeshRegion *retval= nullptr;
    for(const_iterator i= begin(); i!=end(); i++)
      if((*i)->getTag() == tag)
        {
          retval= *i;
          break;
        }
    return retval;
  }

void XC::DqMeshRegion::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }
