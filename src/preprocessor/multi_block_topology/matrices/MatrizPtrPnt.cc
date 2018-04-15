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
//MatrizPtrPnt.cc

#include "MatrizPtrPnt.h"
#include "TritrizPtrPnt.h"
#include "preprocessor/multi_block_topology/entities/Pnt.h"
#include "domain/domain/Domain.h"
#include "domain/constraints/SFreedom_Constraint.h"


#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"

#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/multi_block_topology/MultiBlockTopology.h"
#include "xc_basic/src/matrices/RangoMatriz.h"



void XC::MatrizPtrPnt::setPnt(const size_t &j,const size_t &k,const int &id_point)
  {
    if(check_range(j,k))
      {
        MultiBlockTopology *c= getMultiBlockTopology();
        Pnt *tmp= at(j,k);
        if(tmp!= nullptr)
          std::clog << "Warning!, position: (" 
                    << j << "," << k 
                    << ") is already assigned to point: "
                    << tmp->getName() << std::endl;
        at(j,k)= c->getPoints().busca(id_point);
     }
   else
     std::cerr << "(MatrizPtrPnt::setPnt): '"
               << "'; indices: ("
               << j << ',' << k << ") out of range;"
               << " number of rows: " << n_rows << " number of columns: " << n_columns
               << std::endl;
  }

XC::Pnt *XC::MatrizPtrPnt::getPnt(const size_t &j,const size_t &k)
  {
    Pnt *retval= nullptr;
    if(check_range(j,k))
      retval= at(j,k);
    return retval;
  }

//! @brief Returns (if it exists) a pointer to point
//! identified by the tag is being passed as parameter.
XC::Pnt *XC::MatrizPtrPnt::findPoint(const size_t &tag)
  {
    Pnt *retval= nullptr;
    Pnt *tmp= nullptr;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->GetTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Returns a pointer to the MultiBlockTopology object.
const XC::MultiBlockTopology *XC::MatrizPtrPnt::getMultiBlockTopology(void) const
  {
    const MultiBlockTopology *retval= nullptr;
    const EntCmd *ptr= Owner();
    assert(ptr);
    const MapEsquemas2d *e2d= dynamic_cast<const MapEsquemas2d *>(ptr);
    if(e2d)
      retval= e2d->getMultiBlockTopology();
    else //Tritriz.
      {
        const TritrizPtrPnt *t= dynamic_cast<const TritrizPtrPnt *>(ptr);
        retval= t->getMultiBlockTopology();
      }
    assert(retval);
    return retval;
  }

//! @brief Returns a pointer to the MultiBlockTopology object.
XC::MultiBlockTopology *XC::MatrizPtrPnt::getMultiBlockTopology(void)
  {
    MultiBlockTopology *retval= nullptr;
    EntCmd *ptr= Owner();
    assert(ptr);
    MapEsquemas2d *e2d= dynamic_cast<MapEsquemas2d *>(ptr);
    if(e2d)
      retval= e2d->getMultiBlockTopology();
    else //Tritriz.
      {
        TritrizPtrPnt *t= dynamic_cast<TritrizPtrPnt *>(ptr);
        retval= t->getMultiBlockTopology();
      }
    assert(retval);
    return retval;
  }

//! @brief Returns (if it exists) a pointer to the point
//! identified by the tag passed as parameter.
const XC::Pnt *XC::MatrizPtrPnt::findPoint(const size_t &tag) const
  {
    const Pnt *retval= nullptr;
    const Pnt *tmp= nullptr;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          tmp= operator()(j,k);
          if(tmp)
            {
              if(tag == tmp->GetTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Return the point closest to the point being passed as parameter.
XC::Pnt *XC::MatrizPtrPnt::getNearestPnt(const Pos3d &p)
  {
    Pnt *retval= nullptr, *ptrPnt= nullptr;
    double d= DBL_MAX;
    double tmp;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          ptrPnt= operator()(j,k);
          if(ptrPnt)
            {
              tmp= ptrPnt->DistanciaA2(p);
              if(tmp<d)
                {
                  d= tmp;
                  retval= ptrPnt;
                }
            }
        }
    return retval;
  }

//! @brief Return the ppoint closest to the point being passed as parameter.
const XC::Pnt *XC::MatrizPtrPnt::getNearestPnt(const Pos3d &p) const
  {
    MatrizPtrPnt *this_no_const= const_cast<MatrizPtrPnt *>(this);
    return this_no_const->getNearestPnt(p);
  }

//! @brief Returns a matriz with the point identifiers.
m_int XC::MatrizPtrPnt::getTags(void) const
  {
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    m_int retval(numberOfRows,numberOfColumns,-1);
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          const Pnt *ptr= operator()(j,k);
          if(ptr)
            retval(j,k)= ptr->GetTag();
        }
    return retval;
  }

//! @brief Return the centroide del esquema.
Pos3d XC::MatrizPtrPnt::getCentroide(void) const
  {
    Pos3d retval;
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    const double sz= numberOfRows*numberOfColumns;
    GEOM_FT x= 0.0, y= 0.0, z= 0.0;
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          const Pnt *ptr= operator()(j,k);
          if(ptr)
            {
              const Pos3d &p= ptr->GetPos();
              x+= p.x();
              y+= p.y();
              z+= p.z();
            }
        }
    x/=sz; y/=sz; z/=sz;
    retval= Pos3d(x,y,z);
    return retval;
  }

//! @brief Copy the points from the range being passed as parameter, and places
//! the at the positions of the matrix that result form adding to the (i,j)
//! indexes of the point the values of the offsetIndices vector; i.e.:
//! (i,j)->(i+offsetIndices[0],j+offsetIndices[1])
//! and moving the by the vectorOffset vector.
std::deque<size_t> XC::MatrizPtrPnt::copyPoints(const RangoMatriz &rango,const std::vector<size_t> &offsetIndices,const Vector3d &vectorOffset= Vector3d())
  {
    MultiBlockTopology *mbt= getMultiBlockTopology();
    std::deque<size_t> retval;
    const RangoIndice &row_range= rango.getRowRange();
    const RangoIndice &rcols= rango.getColumnRange();
    for(size_t i= row_range.Inf();i<=row_range.Sup();i++)
      for(size_t j= rcols.Inf();j<=rcols.Sup();j++)
        {
          const Pnt *p= operator()(i,j);
          if(p)
            {
              Pnt *newPt= mbt->getPoints().Copia(p,vectorOffset);
              (*this)(i+offsetIndices[0],j+offsetIndices[1])= newPt;
              retval.push_back(newPt->GetTag());
            }
        }
    return retval;
  }  
