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
//MatrizPtrNod.cc

#include "MatrizPtrNod.h"
#include "domain/mesh/node/Node.h"
#include "domain/domain/Domain.h"
#include "domain/constraints/SFreedom_Constraint.h"


#include "xc_utils/src/geom/pos_vec/Pos3d.h"

#include "boost/lexical_cast.hpp"

//! @brief Returns (if it exists) a pointer to the node
//! cuyo tag se pasa como parámetro.
XC::Node *XC::MatrizPtrNod::buscaNodo(const int &tag)
  {
    Node *retval= nullptr;
    Node *tmp= nullptr;
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
//! cuyo tag se pasa como parámetro.
const XC::Node *XC::MatrizPtrNod::buscaNodo(const int &tag) const
  {
    const Node *retval= nullptr;
    const Node *tmp= nullptr;
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
XC::Node *XC::MatrizPtrNod::getNearestNode(const Pos3d &p)
  {
    Node *retval= nullptr, *ptrNod= nullptr;
    double d= DBL_MAX;
    double tmp;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    if(nfilas*ncols>500)
      std::clog << "La matriz de nodos es tiene "
                << nfilas*ncols << " elementos "
                << " es mejor buscar por coordenadas en el conjunto asociado."
                << std::endl;
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          ptrNod= operator()(j,k);
          tmp= ptrNod->getDist2(p);
          if(tmp<d)
            {
              d= tmp;
              retval= ptrNod;
            }
        }
    return retval;
  }

//! @brief Returns the nodo más próximo al punto being passed as parameter.
const XC::Node *XC::MatrizPtrNod::getNearestNode(const Pos3d &p) const
  {
    MatrizPtrNod *this_no_const= const_cast<MatrizPtrNod *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Impone desplazamiento nulo en los nodos de este conjunto.
void XC::MatrizPtrNod::fix(const SFreedom_Constraint &semilla) const
  {
    if(Null()) return;
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        operator()(j,k)->fix(semilla);
  }

//! @brief Returns a matriz con los identificadores de los nodos.
m_int XC::MatrizPtrNod::getTags(void) const
  {
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    m_int retval(nfilas,ncols,-1);
    for(size_t j= 1;j<=nfilas;j++)
      for(size_t k= 1;k<=ncols;k++)
        {
          const Node *ptr= operator()(j,k);
          if(ptr)
            retval(j,k)= ptr->getTag();
        }
    return retval;
  }
