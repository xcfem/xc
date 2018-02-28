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
//VectorCells.h

#ifndef VectorCells_h 
#define VectorCells_h 

#include <vector>
#include <iostream>

namespace XC {

  class Cell;

//! @ingroup MATSCCCell
//
//! @brief Cells vector
class VectorCells: protected std::vector<Cell *>
  {
  public:
    typedef std::vector<Cell *> v_cell;
    typedef v_cell::reference reference;
    typedef v_cell::const_reference const_reference;
    typedef v_cell::iterator iterator;
    typedef v_cell::const_iterator const_iterator;
  private:
    void free_mem(void);
    void free_mem(const size_t i);
    void alloc(const size_t i,const Cell &c);
    inline reference operator[](const size_t i)
      { return v_cell::operator[](i); }
  public:

    VectorCells(const size_t &sz= 0);
    VectorCells(const VectorCells  &otro);
    VectorCells &operator=(const VectorCells  &otro);
    ~VectorCells(void);

    inline size_t size(void) const
      { return v_cell::size(); }

    void resize(const size_t sz);

    void put(const size_t i,const Cell &c);
    inline const_reference operator[](const size_t i) const
      { return v_cell::operator[](i); }

    void Print(std::ostream &s) const;
  };

} // end of XC namespace


#endif
