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
//VectorCells.cc

#include "VectorCells.h"
#include <material/section/repres/cell/Cell.h> 

//! @brief Frees memory for all the cells.
void XC::VectorCells::free_mem(void)
  {
    const size_t sz= size();
    for(size_t i= 0;i<sz;i++)
      free_mem(i);
  }

//! @brief Frees the memory reserved for the cell at i-th position.
void XC::VectorCells::free_mem(const size_t i)
  {
    if((*this)[i])
      delete (*this)[i];
    (*this)[i]= nullptr;
  }

//! @brief Stores a copy of the cell at the i-th position.
void XC::VectorCells::alloc(const size_t i,const Cell &c)
  {
    free_mem(i);
    (*this)[i]= c.getCopy();
  }

//! @brief Constructor.
XC::VectorCells::VectorCells(const size_t &sz)
  : v_cell(sz,nullptr) {}

//! @brief Copy constructor.
XC::VectorCells::VectorCells(const VectorCells  &other)
  : v_cell(other.size(),nullptr)
  {
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      {
        const Cell *tmp= other[i];
        if(tmp)
          (*this)[i]= tmp->getCopy();
      }
  }

//! @brief Assignment operator.
XC::VectorCells &XC::VectorCells::operator=(const VectorCells &other)
  {
    resize(other.size());
    const size_t sz= size();
    for(size_t i=0;i<sz;i++)
      {
        const Cell *tmp= other[i];
        if(tmp)
          (*this)[i]= tmp->getCopy();
      }
    return *this;
  }

//! @brief Destructor.
XC::VectorCells::~VectorCells(void)
  { free_mem(); }


//! @brief Destructor.
void XC::VectorCells::resize(const size_t sz)
  {
    free_mem();
    v_cell::resize(sz);
    for(size_t i= 0;i<sz;i++)
      (*this)[i]= nullptr;
  }

//! @brief Put the cell in the i-th position.
//! @param i: cell index.
//! @param c: cell value.
void XC::VectorCells::put(const size_t i,const Cell &c)
  { alloc(i,c); }

//! @brief Print stuff.
void XC::VectorCells::Print(std::ostream &os) const
  {
    for(const_iterator i= begin();i!=end();i++)
      (*i)->Print(os);
  }
