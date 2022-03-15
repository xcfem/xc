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
//ElemPtrArray3d.h
//Pointers to element in an array of matrices

#ifndef ELEMPTRARRAY3d_H
#define ELEMPTRARRAY3d_H

#include "utility/kernel/CommandEntity.h"
#include "ElemPtrArray.h"
#include "PtrArray3dBase.h"


namespace XC{

class Vector;
  
//! @ingroup MultiBlockTopologyMR
//! 
//! @brief Three-dimensional array of pointers to elements.
class ElemPtrArray3d: public PtrArray3dBase<ElemPtrArray>
  {
  public:
    ElemPtrArray3d(const size_t n_layers= 0,const ElemPtrArray &m= ElemPtrArray());
    ElemPtrArray3d(const size_t ,const size_t ,const size_t );

    Element *findElement(const int &);
    const Element *findElement(const int &) const;
    Element *getNearestElement(const Pos3d &p);
    const Element *getNearestElement(const Pos3d &p) const;
    boost::python::list getPyElementList(void) const;
    
    // Loads on elements.
    void vector2dPointLoadGlobal(const Vector &,const Vector &);
    void vector2dPointLoadLocal(const Vector &,const Vector &);
    void vector3dPointLoadGlobal(const Vector &,const Vector &);
    void vector3dPointLoadLocal(const Vector &,const Vector &);
    
    std::set<const Element *> getConnectedElements(const Node *) const;
    std::set<Element *> getConnectedElements(const Node *);
    boost::python::list getConnectedElementsPy(const Node *);    
  };

} //end of XC namespace.

#endif
