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
//ElemPtrArray.h
//Point matrix

#ifndef MATRIZPTRELEM_H
#define MATRIZPTRELEM_H

#include "PtrArrayBase.h"
#include <vector>

class Pos3d;

namespace XC{

  class Element;

//! @ingroup MultiBlockTopology
//! 
//! \defgroup MultiBlockTopologyMR References to nodes and elements.
//
//! @ingroup MultiBlockTopologyMR
//
//! @brief Matrix of pointers to elements.
class ElemPtrArray: public PtrArrayBase<Element>
  {
  protected:

  public:
    //! @brief Constructor.
    ElemPtrArray(const size_t &f=0,const size_t &c=0)
      : PtrArrayBase<Element>(f,c) {}

    Element *findElement(const int &);
    const Element *findElement(const int &) const;
    Element *getNearestElement(const Pos3d &p);
    const Element *getNearestElement(const Pos3d &p) const;


  };

} //end of XC namespace

#endif
