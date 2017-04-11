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
//TritrizPtrElem.cc

#include "TritrizPtrElem.h"
#include "domain/mesh/element/Element.h"
#include <boost/any.hpp>


#include "xc_utils/src/geom/pos_vec/Pos3d.h"

#include "boost/lexical_cast.hpp"

//! @brief Default constructor.
XC::TritrizPtrElem::TritrizPtrElem(const size_t capas,const MatrizPtrElem &m)
  : TritrizPtrBase<MatrizPtrElem>(capas,m) {}
//! @brief Constructor.
XC::TritrizPtrElem::TritrizPtrElem(const size_t capas,const size_t filas,const size_t cols)
  : TritrizPtrBase<MatrizPtrElem>(capas)
  {
    for(size_t i=0;i<capas;i++)
      (*this)[i]= MatrizPtrElem(filas,cols);
  }

//! @brief Returns (if it exists) a pointer to the element
//! identified by the tag being passed as parameter.
XC::Element *XC::TritrizPtrElem::findElement(const int &tag)
  {
    Element *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrElem &capa= operator()(i);
        retval= capa.findElement(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Returns (if it exists) a pointer to the element
//! identified by the tag being passed as parameter.
const XC::Element *XC::TritrizPtrElem::findElement(const int &tag) const
  {
    const Element *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        const MatrizPtrElem &capa= operator()(i);
        retval= capa.findElement(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Returns the element closest to the point being passed as parameter.
XC::Element *XC::TritrizPtrElem::getNearestElement(const Pos3d &p)
  {
    Element *retval= nullptr, *ptrElem= nullptr;
    const size_t ncapas= GetCapas();
    double d= DBL_MAX;
    double tmp;
    if(ncapas>100)
      std::clog << "La «tritriz» de elementos es tiene "
                << ncapas << " capas "
                << " es mejor buscar por coordenadas en the set asociado."
                << std::endl;
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrElem &capa= operator()(i);
        ptrElem= capa.getNearestElement(p);
        tmp= ptrElem->getDist2(p);
        if(tmp<d)
          {
            d= tmp;
            retval= ptrElem;
          }
      }
    return retval;
  }

//! @brief Returns the element closest to the point being passed as parameter.
const XC::Element *XC::TritrizPtrElem::getNearestElement(const Pos3d &p) const
  {
    TritrizPtrElem *this_no_const= const_cast<TritrizPtrElem *>(this);
    return this_no_const->getNearestElement(p);
  }
