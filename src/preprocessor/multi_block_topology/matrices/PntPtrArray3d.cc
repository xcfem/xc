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
//PntPtrArray3d.cc

#include "PntPtrArray3d.h"
#include "preprocessor/multi_block_topology/entities/0d/Pnt.h"
#include "xc_utils/src/functions/algebra/num_integration.h"

#include "domain/domain/Domain.h"
#include "domain/constraints/SFreedom_Constraint.h"


#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Polygon3d.h"
#include "boost/lexical_cast.hpp"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/multi_block_topology/MultiBlockTopology.h"
#include "xc_utils/src/matrices/3d_arrays/Array3dRange.h"
#include "xc_utils/src/matrices/MatrixRange.h"


//! @brief Default constructor.
XC::PntPtrArray3d::PntPtrArray3d(const size_t n_layers)
  : PtrArray3dBase<PntPtrArray>(n_layers) {}

//! @brief Constructor.
XC::PntPtrArray3d::PntPtrArray3d(const size_t n_layers,const size_t n_rows,const size_t cols)
  : PtrArray3dBase<PntPtrArray>(n_layers,n_rows,cols) {}

void XC::PntPtrArray3d::setPnt(const size_t &i,const size_t &j,const size_t &k,const int &id_point)
  {
    if(check_range(i,j,k))
      {
        MultiBlockTopology *c= getMultiBlockTopology();
        Pnt *tmp= PntPtrArray3d::operator()(i,j,k);
        if(tmp!= nullptr)
          std::clog << "PntPtrArray3d::" << __FUNCTION__
	            << "; warning!, position: (" 
                    << i << "," << j << "," << k 
                    << ") is already assigned to point: "
                    << tmp->getName() << std::endl;
        PntPtrArray3d::operator()(i,j,k)= c->getPoints().busca(id_point);
      }
    else
     std::cerr << "PntPtrArray3d::" << __FUNCTION__
	       << "; indices: ("
               << i << ','  << j << ',' << k << ") out of range;"
               << " number of layers: " << getNumberOfLayers()
	       << " number of rows: " << getNumberOfRows()
	       << " number of columns: " << getNumberOfColumns()
               << std::endl;
  }

XC::Pnt *XC::PntPtrArray3d::getPnt(const size_t &i,const size_t &j,const size_t &k)
  { return getAtIJK(i,j,k); }

//! @brief Return the framework centroid.
Pos3d XC::PntPtrArray3d::getCentroide(void) const
  {
    Pos3d retval;
    const size_t numberOfLayers= getNumberOfLayers();
    GEOM_FT x= 0.0, y= 0.0, z= 0.0;
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        const PntPtrArray &layer= operator()(i);
        Pos3d p= layer.getCentroide();
        x+= p.x();
        y+= p.y();
        z+= p.z(); 
      }
    x/=numberOfLayers; y/=numberOfLayers; z/=numberOfLayers;
    retval= Pos3d(x,y,z);
    return retval;
  }


//! @brief Returns (if it exists) a pointer to the point
//! identified by the tag being passed as parameter.
XC::Pnt *XC::PntPtrArray3d::findPoint(const int &tag)
  {
    Pnt *retval= nullptr;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        PntPtrArray &layer= operator()(i);
        retval= layer.findPoint(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Returns a pointer to the MultiBlockTopology object.
const XC::MultiBlockTopology *XC::PntPtrArray3d::getMultiBlockTopology(void) const
  {
    const MultiBlockTopology *retval= nullptr;
    const CommandEntity *ptr= Owner();
    assert(ptr);
    const Framework3d *e3d= dynamic_cast<const Framework3d *>(ptr);
    if(e3d)
      retval= e3d->getMultiBlockTopology();
    assert(retval);
    return retval;
  }

//! @brief Returns a pointer to the MultiBlockTopology object.
XC::MultiBlockTopology *XC::PntPtrArray3d::getMultiBlockTopology(void)
  {
    MultiBlockTopology *retval= nullptr;
    CommandEntity *ptr= Owner();
    assert(ptr);
    Framework3d *e3d= dynamic_cast<Framework3d *>(ptr);
    if(e3d)
      retval= e3d->getMultiBlockTopology();
    assert(retval);
    return retval;
  }

//! @brief Return the point closest to the one being passed as parameter.
const XC::Pnt *XC::PntPtrArray3d::getNearestPnt(const Pos3d &p) const
  {
    PntPtrArray3d *this_no_const= const_cast<PntPtrArray3d *>(this);
    return this_no_const->getNearestPnt(p);
  }

//! @brief Return the point closest to the one being passed as parameter.
XC::Pnt *XC::PntPtrArray3d::getNearestPnt(const Pos3d &p)
  {
    Pnt *retval= nullptr, *ptrPnt= nullptr;
    const size_t numberOfLayers= getNumberOfLayers();
    double d= DBL_MAX;
    double tmp;
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        PntPtrArray &layer= operator()(i);
        ptrPnt= layer.getNearestPnt(p);
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

//! @brief Returns (if it exists) a pointer to the point
//! identified by the tag being passed as parameter.
const XC::Pnt *XC::PntPtrArray3d::findPoint(const int &tag) const
  {
    const Pnt *retval= nullptr;
    const size_t numberOfLayers= getNumberOfLayers();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        const PntPtrArray &layer= operator()(i);
        retval= layer.findPoint(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Copy the points from the range being passed as parameter, and places
//! the at the positions of the matrix that result form adding to the (i,j,k)
//! indexes of the point the values of the offsetIndices vector; i.e.:
//! (i,j,k)->(i+offsetIndices[0],j+offsetIndices[1],k+offsetIndices[2])
//! and moving the by the vectorOffset vector.
std::deque<size_t> XC::PntPtrArray3d::copyPoints(const Array3dRange &rango,const std::vector<size_t> &offsetIndices,const Vector3d &vectorOffset= Vector3d())
  {
    MultiBlockTopology *mbt= getMultiBlockTopology();
    std::deque<size_t> retval;
    const RangoIndice &layer_range= rango.getLayerRange();
    const RangoIndice &row_range= rango.getRowRange();
    const RangoIndice &rcols= rango.getColumnRange();
    for(size_t i= layer_range.Inf();i<=layer_range.Sup();i++)
      for(size_t j= row_range.Inf();j<=row_range.Sup();j++)
        for(size_t k= rcols.Inf();k<=rcols.Sup();k++)
          {
            const Pnt *p= operator()(i,j,k);
            if(p)
              {
                Pnt *newPt= mbt->getPoints().Copy(p,vectorOffset);
                (*this)(i+offsetIndices[0],j+offsetIndices[1],k+offsetIndices[2])= newPt;
                retval.push_back(newPt->getTag());
              }
          }
    return retval;
  }  

//! @brief Return the points del rango being passed as parameter.
XC::PntPtrArray3d XC::PntPtrArray3d::getPointsOnRange(const Array3dRange &rango)
  {
    PntPtrArray3d retval(rango.getNumberOfLayers(),rango.getNumberOfRows(),rango.getNumberOfColumns());
    const RangoIndice &layer_range= rango.getLayerRange();
    const RangoIndice &row_range= rango.getRowRange();
    const RangoIndice &rcols= rango.getColumnRange();
    const size_t layer_range_inf= layer_range.Inf();
    const size_t row_range_inf= row_range.Inf();
    const size_t rcols_inf= rcols.Inf();
    for(size_t i= layer_range_inf;i<=layer_range.Sup();i++)
      for(size_t j= row_range_inf;j<=row_range.Sup();j++)
        for(size_t k= rcols_inf;k<=rcols.Sup();k++)
          {
            Pnt *p= operator()(i,j,k);
            if(p)
              retval(i-layer_range_inf+1,j-row_range_inf+1,k-rcols_inf+1)= p;
          }
    return retval;
  }

//! @brief Return the points which indices are being passed as parameter.
XC::Pnt *XC::PntPtrArray3d::getPoint(const VIndices &iPoint)
  {
    Pnt *retval= nullptr;
    if(iPoint.size()>2)
      {
        if((iPoint[0]>0) && (iPoint[1]>0) && (iPoint[2]>0))
          { retval= (*this)(iPoint[0],iPoint[1],iPoint[2]); }
      }
    else
      std::cerr  << "PntPtrArray3d::" << __FUNCTION__
		 << "; vector of indexes: "
                 << iPoint << " is not valid." << std::endl;
    return retval;    
  }

//! @brief Return the points which indices are being passed as parameter.
XC::PntPtrArray3d XC::PntPtrArray3d::getPoints(const Indices3dArray &indices)
  {
    const size_t numberOfLayers= indices.getNumberOfLayers();
    const size_t numberOfRows= indices.getNumberOfRows();
    const size_t numberOfColumns= indices.getNumberOfColumns();
    PntPtrArray3d retval(numberOfLayers,numberOfRows,numberOfColumns);
    for(size_t i= 1;i<= numberOfLayers;i++)
      for(size_t j= 1;j<= numberOfRows;j++)
        for(size_t k= 1;k<= numberOfLayers;k++)
          {
            const VIndices iPoint= indices(i,j,k);
            if(iPoint.size()>2)
              { retval(i,j,k)= getPoint(iPoint); }
            else
	      std::cerr << "PntPtrArray3d::" << __FUNCTION__
			<< "; vector of indexes: "
                        << iPoint << " is not valid." << std::endl;
          }
    return retval;
  }

//! @brief Return the points which indices are being passed as parameters.
XC::PntPtrArray XC::PntPtrArray3d::getPoints(const IndicesMatrix &indices)
  {
    const size_t numberOfRows= indices.getNumberOfRows();
    const size_t numberOfColumns= indices.getNumberOfColumns();
    PntPtrArray retval(numberOfRows,numberOfColumns);
    for(size_t i= 1;i<= numberOfRows;i++)
      for(size_t j= 1;j<= numberOfColumns;j++)
        {
          const VIndices iPoint= indices(i,j);
          if(iPoint.size()>2)
            { retval(i,j)= getPoint(iPoint); }
          else
            std::cerr << "PntPtrArray3d::" << __FUNCTION__
		      << "; vector of indexes: "
                      << iPoint << " is not valid." << std::endl;
        }
    return retval;
  }

//! @brief Return the cell built by the points obtained from the positions
//! of the tritrix that result for adding to the indexex (i,j,k) of each point
//! the values of the vector offsetIndices i. e.:
//! Point (i,j,k): (i+offsetIndices(i,j,k)[0],j+offsetIndices(i,j,k)[1],k+offsetIndices(i,j,k)[2])
XC::PntPtrArray3d XC::PntPtrArray3d::getCellPoints(const size_t &i,const size_t &j,const size_t &k,const Indices3dArray &offsetIndices)
  {
    VIndices org(3);
    org[0]= i;org[1]= j;org[2]= k;
    Indices3dArray tmp(offsetIndices);
    tmp.Offset(org);
    return getPoints(tmp);
  }

//! @brief Return the cell built by the points obtained from the positions
//! of the tritrix that result for adding to the indexes (i,j) of each point
//! the values of the vector offsetIndices i. e.:
//! Point (i,j): (i+offsetIndices(i,j)[0],j+offsetIndices(i,j)[1])
XC::PntPtrArray XC::PntPtrArray3d::getCellPoints(const size_t &i,const size_t &j,const IndicesMatrix &offsetIndices)
  {
    VIndices org(2);
    org[0]= i;org[1]= j;
    IndicesMatrix tmp(offsetIndices);
    tmp.Offset(org);
    return getPoints(tmp);
  }

// //! @brief Creates quad surfaces between the point range passed as paramete, it places them
// //! between the positions of the tritrix that result from adding to the
// //! indexes (i,j) of the point the values of offsetIndices so:
// //! Point 1: (i+offsetIndices[0,0],j+offsetIndices[0,1])
// //! Point 2: (i+offsetIndices[1,0],j+offsetIndices[1,1])
// //! Point 3: (i+offsetIndices[2,0],j+offsetIndices[2,1])
// //! ...
// //! @param nf: Number of rows of the matrix that holds the pointers to point.
// //! @param nc: Number of columns of the matrix that holds the pointers to point.
// std::deque<size_t> XC::PntPtrArray3d::CreaQuads(const Array3dRange &rango,const size_t &nf,const size_t &nc,const m_int &offsetIndices,const double &elemSizeI,const double &elemeSizeJ)
//   {
//     MultiBlockTopology *mbt= getMultiBlockTopology();
//     std::deque<size_t> retval;
//     const RangoIndice &layer_range= rango.getLayerRange();
//     const RangoIndice &row_range= rango.getRowRange();
//     const RangoIndice &rcols= rango.getColumnRange();
//     for(size_t i= layer_range.Inf();i<=layer_range.Sup();i++)
//       for(size_t j= row_range.Inf();j<=row_range.Sup();j++)
//         for(size_t k= rcols.Inf();k<=rcols.Sup();k++)
//           {
//             const Pnt *p= operator()(i,j,k);
//             if(p)
//               {
//                 Pnt *newPt= mbt->getPoints().Copy(p,vectorOffset);
//                 (*this)(i+offsetIndices[0],j+offsetIndices[1],k+offsetIndices[2])= newPt;
//                 retval.push_back(newPt->getTag());
//               }
//           }
//     return retval;
//   }  

void XC::PntPtrArray3d::Print(std::ostream &os) const
  {
    const size_t numberOfLayers= getNumberOfLayers();
    const size_t numberOfRows= getNumberOfRows();
    const size_t numberOfColumns= getNumberOfColumns();
    for(size_t i=1;i<=numberOfLayers;i++)
      {
        for(size_t j=1;j<=numberOfRows;j++)
          {
            for(size_t k=1;k<=numberOfColumns;k++)
	      os << (*this)(i,j,k)->getTag() << " ";
	    os << std::endl;
          }
        os << std::endl;
      }
  }

std::ostream &XC::operator<<(std::ostream &os, const PntPtrArray3d &t)
  {
    t.Print(os);
    return os;
  }

//! @brief Return the indexes of the points (j,k),(j+1,k),(j+1,k+1),(j,k+1). 
std::vector<size_t> XC::getIdPointsQuad(const PntPtrArray3d::constant_i_layer_const_ref &points,const size_t &j,const size_t &k)
  {
    std::vector<size_t> retval(4,-1);
    const size_t numberOfRows= points.getNumberOfRows();
    const size_t numberOfColumns= points.getNumberOfColumns();
    if(j>=numberOfRows)
      {
        std::cerr << "PntPtrArray3d::" << __FUNCTION__
	          << "; row index j= " << j << " out of range.\n";
        return retval;
      }
    if(k>=numberOfColumns)
      {
        std::cerr << "PntPtrArray3d::" << __FUNCTION__
	          << "; column index k= " << k << " out of range.\n";
        return retval;
      }


    Pos3d p1;
    const Pnt *ptr= points(j,k);
    if(ptr)
      {
        retval[0]= ptr->getTag();
        if(retval[0]<0)
          std::cerr << "PntPtrArray3d::" << __FUNCTION__
		    << "; error when obtaining the point identifier ("
		    << j << ',' << k << ").\n";
        p1= ptr->GetPos();
      }

    Pos3d p2;
    ptr= points(j,k+1);
    if(ptr)
      {
        retval[1]= ptr->getTag();
        if(retval[1]<0)
          std::cerr << "PntPtrArray3d::" << __FUNCTION__
		    << "; error when obtaining the point identifier ("
		    << j << ',' << k+1 << ").\n";
        p2= ptr->GetPos();
      }

    Pos3d p3;
    ptr= points(j+1,k+1);
    if(ptr)
      {
        retval[2]= ptr->getTag();
        if(retval[2]<0)
          std::cerr << "PntPtrArray3d::" << __FUNCTION__
		    << "; error when obtaining the point identifier ("
		    << j+1 << ',' << k+1 << ").\n";
        p3= ptr->GetPos();
      }

    Pos3d p4;
    ptr= points(j+1,k);
    if(ptr)
      {
        retval[3]=ptr->getTag();
        if(retval[3]<0)
          std::cerr << "PntPtrArray3d::" << __FUNCTION__
		    << "; error when obtaining the point identifier ("
		    << j+1 << ',' << k << ").\n";
        p4= ptr->GetPos();
      }

//     const Vector3d v2= p2-p1;
//     const Vector3d v3= p3-p2;
//     const Vector3d v4= p4-p3;
//     const Vector3d v1= p1-p4;
//     const double d1= dot(v1,v3);
//     const double d2= dot(v2,v4);
//     if((d1>0))
//       {
//         std::swap(p3,p2);
//         std::swap(retval[2],retval[1]);
//       }
//     if((d2>0))
//       {
// 	std::swap(p3,p4);
//         std::swap(retval[2],retval[3]);
//       }

    std::list<Pos3d> positions;
    positions.push_back(p1);
    positions.push_back(p2);
    positions.push_back(p3);
    positions.push_back(p4);
    Polygon3d tmp(positions.begin(),positions.end());
    const double area= tmp.getArea();
    if(area<1e-3)
      {
        std::cerr << "PntPtrArray3d::" << __FUNCTION__
		  << "; When computing cell with indexes (" << j << ',' << k
                  << ") a very small area was obtained (" << area << ").\n"
		  << " position of the point (j,k) " << p1 << std::endl
		  << " position of the point (j+1,k) " << p2 << std::endl
		  << " position of the point (j+1,k+1) " << p3 << std::endl
		  << " position of the point (1,k+1) " << p4 << std::endl;
      }
    return retval;
  }

