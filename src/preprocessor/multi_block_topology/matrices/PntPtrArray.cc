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
//PntPtrArray.cc

#include "PntPtrArray.h"
#include "PntPtrArray3d.h"
#include "preprocessor/multi_block_topology/entities/Pnt.h"
#include "domain/domain/Domain.h"
#include "domain/constraints/SFreedom_Constraint.h"


#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"

#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/multi_block_topology/MultiBlockTopology.h"
#include "xc_utils/src/matrices/MatrixRange.h"



void XC::PntPtrArray::setPnt(const size_t &j,const size_t &k,const int &id_point)
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
     std::cerr << "(PntPtrArray::setPnt): '"
               << "'; indices: ("
               << j << ',' << k << ") out of range;"
               << " number of rows: " << n_rows << " number of columns: " << n_columns
               << std::endl;
  }

XC::Pnt *XC::PntPtrArray::getPnt(const size_t &j,const size_t &k)
  {
    Pnt *retval= nullptr;
    if(check_range(j,k))
      retval= at(j,k);
    return retval;
  }

//! @brief Returns (if it exists) a pointer to point
//! identified by the tag is being passed as parameter.
XC::Pnt *XC::PntPtrArray::findPoint(const size_t &tag)
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
              if(tag == tmp->getTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Returns a pointer to the MultiBlockTopology object.
const XC::MultiBlockTopology *XC::PntPtrArray::getMultiBlockTopology(void) const
  {
    const MultiBlockTopology *retval= nullptr;
    const CommandEntity *ptr= Owner();
    assert(ptr);
    const Framework2d *e2d= dynamic_cast<const Framework2d *>(ptr);
    if(e2d)
      retval= e2d->getMultiBlockTopology();
    else //Array 3d.
      {
        const PntPtrArray3d *t= dynamic_cast<const PntPtrArray3d *>(ptr);
        retval= t->getMultiBlockTopology();
      }
    assert(retval);
    return retval;
  }

//! @brief Returns a pointer to the MultiBlockTopology object.
XC::MultiBlockTopology *XC::PntPtrArray::getMultiBlockTopology(void)
  {
    MultiBlockTopology *retval= nullptr;
    CommandEntity *ptr= Owner();
    assert(ptr);
    Framework2d *e2d= dynamic_cast<Framework2d *>(ptr);
    if(e2d)
      retval= e2d->getMultiBlockTopology();
    else //Array 3d.
      {
        PntPtrArray3d *t= dynamic_cast<PntPtrArray3d *>(ptr);
        retval= t->getMultiBlockTopology();
      }
    assert(retval);
    return retval;
  }

//! @brief Returns (if it exists) a pointer to the point
//! identified by the tag passed as parameter.
const XC::Pnt *XC::PntPtrArray::findPoint(const size_t &tag) const
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
              if(tag == tmp->getTag())
                {
                  retval= tmp;
                  break;
                }
            }
        }
    return retval;
  }

//! @brief Return the point closest to the point being passed as parameter.
XC::Pnt *XC::PntPtrArray::getNearestPnt(const Pos3d &p)
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
              tmp= ptrPnt->getSquaredDistanceTo(p);
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
const XC::Pnt *XC::PntPtrArray::getNearestPnt(const Pos3d &p) const
  {
    PntPtrArray *this_no_const= const_cast<PntPtrArray *>(this);
    return this_no_const->getNearestPnt(p);
  }

//! @brief Returns an array with the point identifiers.
m_int XC::PntPtrArray::getTags(void) const
  {
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    m_int retval(numberOfRows,numberOfColumns,-1);
    for(size_t j= 1;j<=numberOfRows;j++)
      for(size_t k= 1;k<=numberOfColumns;k++)
        {
          const Pnt *ptr= operator()(j,k);
          if(ptr)
            retval(j,k)= ptr->getTag();
        }
    return retval;
  }

//! @brief Return the framework centroid.
Pos3d XC::PntPtrArray::getCentroide(void) const
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
std::deque<size_t> XC::PntPtrArray::copyPoints(const MatrixRange &rango,const std::vector<size_t> &offsetIndices,const Vector3d &vectorOffset= Vector3d())
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
              Pnt *newPt= mbt->getPoints().Copy(p,vectorOffset);
              (*this)(i+offsetIndices[0],j+offsetIndices[1])= newPt;
              retval.push_back(newPt->getTag());
            }
        }
    return retval;
  }  
