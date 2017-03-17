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
//KDTreeElements.cc

#include "KDTreeElements.h"
#include "domain/mesh/element/Element.h"
#include "xc_utils/src/geom/pos_vec/Pos3d.h"

//! @brief Constructor.
XC::ElemPos::ElemPos(const Element &e)
  :  KDTreePos(e.getPosCdg()), elemPtr(&e){}

//! @brief Constructor.
XC::ElemPos::ElemPos(const Pos3d &p)
  : KDTreePos(p), elemPtr(nullptr) {}
  
//! @brief Constructor.
XC::KDTreeElements::KDTreeElements(void)
  : tree_type(std::ptr_fun(ElemPos::tac)), pend_optimizar(0) {}


void XC::KDTreeElements::insert(const Element &n)
  {
    tree_type::insert(n);
  }

void XC::KDTreeElements::erase(const Element &n)
  {
    tree_type::erase(n);
    pend_optimizar++;
    if(pend_optimizar>=10)
      {
	tree_type::optimise();
        pend_optimizar= 0;
      }
  }

void XC::KDTreeElements::clear(void)
  { tree_type::clear(); }

//! @brief Returns the node closest to the position being passed as parameter.
const XC::Element *XC::KDTreeElements::getNearestElement(const Pos3d &pos) const
  {
    const Element *retval= nullptr;
    ElemPos target(pos);
    std::pair<tree_type::const_iterator,double> found = find_nearest(target);
    if(found.first != end())
      retval= found.first->getElementPtr();
    return retval;
  }

//! @brief Returns the element closest to the position
//! being passed as parameter.
const XC::Element *XC::KDTreeElements::getNearestElement(const Pos3d &pos, const double &r) const
  {
    const Element *retval= nullptr;
    ElemPos target(pos);
    std::pair<tree_type::const_iterator,double> found = find_nearest(target,r);
    if(found.first != end())
      retval= found.first->getElementPtr();
    return retval;
  }
