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
//! which tag is being passed as parameter.
XC::Node *XC::MatrizPtrNod::findNode(const int &tag)
  {
    Node *retval= nullptr;
    Node *tmp= nullptr;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
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
//! which tag is being passed as parameter.
const XC::Node *XC::MatrizPtrNod::findNode(const int &tag) const
  {
    const Node *retval= nullptr;
    const Node *tmp= nullptr;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
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

//! @brief Returns the node closest to the point being passed as parameter.
XC::Node *XC::MatrizPtrNod::getNearestNode(const Pos3d &p)
  {
    Node *retval= nullptr, *ptrNod= nullptr;
    double d= DBL_MAX;
    double tmp;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    if(numberOfRows*numberOfColumns>500)
      std::clog << "Node matrix has "
                << numberOfRows*numberOfColumns << " elements "
                << " better to look by coordinates in the associated set."
                << std::endl;
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
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

//! @brief Returns the node closest to the point being passed as parameter.
const XC::Node *XC::MatrizPtrNod::getNearestNode(const Pos3d &p) const
  {
    MatrizPtrNod *this_no_const= const_cast<MatrizPtrNod *>(this);
    return this_no_const->getNearestNode(p);
  }

//! @brief Impone desplazamiento nulo en the nodes de this set.
void XC::MatrizPtrNod::fix(const SFreedom_Constraint &semilla) const
  {
    if(Null()) return;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        operator()(j,k)->fix(semilla);
  }

//! @brief Returns a matriz con los identifiers of the nodes.
m_int XC::MatrizPtrNod::getTags(void) const
  {
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    m_int retval(numberOfRows,numberOfColumns,-1);
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          const Node *ptr= operator()(j,k);
          if(ptr)
            retval(j,k)= ptr->getTag();
        }
    return retval;
  }
