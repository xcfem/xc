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
//MatrizPtrElem.cc

#include "MatrizPtrElem.h"
#include "domain/mesh/element/Element.h"


#include "xc_utils/src/geom/pos_vec/Pos3d.h"

#include "boost/lexical_cast.hpp"

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag is being passed as parameter.
XC::Element *XC::MatrizPtrElem::buscaElemento(const int &tag)
  {
    Element *retval= nullptr;
    Element *tmp= nullptr;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->getTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag is being passed as parameter.
const XC::Element *XC::MatrizPtrElem::buscaElemento(const int &tag) const
  {
    const Element *retval= nullptr;
    const Element *tmp= nullptr;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->getTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Returns the nodo más próximo al punto being passed as parameter.
XC::Element *XC::MatrizPtrElem::getNearestElement(const Pos3d &p)
  {
    Element *retval= nullptr, *ptrElem= nullptr;
    double d= DBL_MAX;
    double tmp;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    if(nfilas*ncols>500)
      std::clog << "La matriz de elementos es tiene "
                << nfilas*ncols << " componentes "
                << " es mejor buscar por coordenadas en the set asociado."
                << std::endl;
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          ptrElem= operator()(j,k);
          tmp= ptrElem->getDist2(p);
          if(tmp<d)
            {
              d= tmp;
              retval= ptrElem;
            }
        }
    return retval;
  }

//! @brief Returns the nodo más próximo al punto being passed as parameter.
const XC::Element *XC::MatrizPtrElem::getNearestElement(const Pos3d &p) const
  {
    MatrizPtrElem *this_no_const= const_cast<MatrizPtrElem *>(this);
    return this_no_const->getNearestElement(p);
  }
