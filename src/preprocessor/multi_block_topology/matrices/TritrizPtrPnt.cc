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
//TritrizPtrPnt.cc

#include "TritrizPtrPnt.h"
#include "preprocessor/multi_block_topology/entities/Pnt.h"
#include "xc_basic/src/funciones/algebra/integ_num.h"

#include <boost/any.hpp>
#include "domain/domain/Domain.h"
#include "domain/constraints/SFreedom_Constraint.h"


#include "xc_utils/src/geom/pos_vec/Pos3d.h"
#include "xc_utils/src/geom/pos_vec/Vector3d.h"
#include "xc_utils/src/geom/d2/Poligono3d.h"
#include "boost/lexical_cast.hpp"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/Node.h"
#include "preprocessor/multi_block_topology/MultiBlockTopology.h"
#include "xc_utils/src/geom/pos_vec/RangoTritriz.h"
#include "xc_basic/src/matrices/RangoMatriz.h"


//! @brief Default constructor.
XC::TritrizPtrPnt::TritrizPtrPnt(const size_t capas)
  : TritrizPtrBase<MatrizPtrPnt>(capas) {}

//! @brief Constructor.
XC::TritrizPtrPnt::TritrizPtrPnt(const size_t capas,const size_t filas,const size_t cols)
  : TritrizPtrBase<MatrizPtrPnt>(capas,filas,cols) {}

void XC::TritrizPtrPnt::setPnt(const size_t &i,const size_t &j,const size_t &k,const int &id_point)
  {
    if(check_range(i,j,k))
      {
        MultiBlockTopology *c= getMultiBlockTopology();
        Pnt *tmp= TritrizPtrPnt::operator()(i,j,k);
        if(tmp!= nullptr)
          std::clog << "Warning!, position: (" 
                    << i << "," << j << "," << k 
                    << ") is already assigned to point: "
                    << tmp->getName() << std::endl;
        TritrizPtrPnt::operator()(i,j,k)= c->getPoints().busca(id_point);
      }
    else
     std::cerr << "(MatrizPtrPnt::setPnt): '"
               << "'; indices: ("
               << i << ','  << j << ',' << k << ") out of range;"
               << " number of layers: " << GetCapas() << " number of rows: " << getNumFilas() << " number of columns: " << getNumCols()
               << std::endl;
  }

XC::Pnt *XC::TritrizPtrPnt::getPnt(const size_t &i,const size_t &j,const size_t &k)
  { return getAtIJK(i,j,k); }

//! @brief Return the centroide del esquema.
Pos3d XC::TritrizPtrPnt::getCentroide(void) const
  {
    Pos3d retval;
    const size_t ncapas= GetCapas();
    GEOM_FT x= 0.0, y= 0.0, z= 0.0;
    for(size_t i=1;i<=ncapas;i++)
      {
        const MatrizPtrPnt &capa= operator()(i);
        Pos3d p= capa.getCentroide();
        x+= p.x();
        y+= p.y();
        z+= p.z(); 
      }
    x/=ncapas; y/=ncapas; z/=ncapas;
    retval= Pos3d(x,y,z);
    return retval;
  }


//! @brief Returns (if it exists) a pointer to the point
//! identified by the tag being passed as parameter.
XC::Pnt *XC::TritrizPtrPnt::findPoint(const int &tag)
  {
    Pnt *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrPnt &capa= operator()(i);
        retval= capa.findPoint(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Returns a pointer to the MultiBlockTopology object.
const XC::MultiBlockTopology *XC::TritrizPtrPnt::getMultiBlockTopology(void) const
  {
    const MultiBlockTopology *retval= nullptr;
    const EntCmd *ptr= Owner();
    assert(ptr);
    const MapEsquemas3d *e3d= dynamic_cast<const MapEsquemas3d *>(ptr);
    if(e3d)
      retval= e3d->getMultiBlockTopology();
    assert(retval);
    return retval;
  }

//! @brief Returns a pointer to the MultiBlockTopology object.
XC::MultiBlockTopology *XC::TritrizPtrPnt::getMultiBlockTopology(void)
  {
    MultiBlockTopology *retval= nullptr;
    EntCmd *ptr= Owner();
    assert(ptr);
    MapEsquemas3d *e3d= dynamic_cast<MapEsquemas3d *>(ptr);
    if(e3d)
      retval= e3d->getMultiBlockTopology();
    assert(retval);
    return retval;
  }

//! @brief Return the point closest to the one being passed as parameter.
const XC::Pnt *XC::TritrizPtrPnt::getNearestPnt(const Pos3d &p) const
  {
    TritrizPtrPnt *this_no_const= const_cast<TritrizPtrPnt *>(this);
    return this_no_const->getNearestPnt(p);
  }

//! @brief Return the point closest to the one being passed as parameter.
XC::Pnt *XC::TritrizPtrPnt::getNearestPnt(const Pos3d &p)
  {
    Pnt *retval= nullptr, *ptrPnt= nullptr;
    const size_t ncapas= GetCapas();
    double d= DBL_MAX;
    double tmp;
    for(size_t i=1;i<=ncapas;i++)
      {
        MatrizPtrPnt &capa= operator()(i);
        ptrPnt= capa.getNearestPnt(p);
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

//! @brief Returns (if it exists) a pointer to the point
//! identified by the tag being passed as parameter.
const XC::Pnt *XC::TritrizPtrPnt::findPoint(const int &tag) const
  {
    const Pnt *retval= nullptr;
    const size_t ncapas= GetCapas();
    for(size_t i=1;i<=ncapas;i++)
      {
        const MatrizPtrPnt &capa= operator()(i);
        retval= capa.findPoint(tag);
        if(retval) break;
      }
    return retval;
  }

//! @brief Copy the points from the range being passed as parameter, and places
//! the at the positions of the matrix that result form adding to the (i,j,k)
//! indexes of the point the values of the offsetIndices vector; i.e.:
//! (i,j,k)->(i+offsetIndices[0],j+offsetIndices[1],k+offsetIndices[2])
//! and moving the by the vectorOffset vector.
std::deque<size_t> XC::TritrizPtrPnt::copyPoints(const RangoTritriz &rango,const std::vector<size_t> &offsetIndices,const Vector3d &vectorOffset= Vector3d())
  {
    MultiBlockTopology *mbt= getMultiBlockTopology();
    std::deque<size_t> retval;
    const RangoIndice &rcapas= rango.GetRangoCapas();
    const RangoIndice &rfilas= rango.GetRangoFilas();
    const RangoIndice &rcols= rango.GetRangoCols();
    for(size_t i= rcapas.Inf();i<=rcapas.Sup();i++)
      for(size_t j= rfilas.Inf();j<=rfilas.Sup();j++)
        for(size_t k= rcols.Inf();k<=rcols.Sup();k++)
          {
            const Pnt *p= operator()(i,j,k);
            if(p)
              {
                Pnt *newPt= mbt->getPoints().Copia(p,vectorOffset);
                (*this)(i+offsetIndices[0],j+offsetIndices[1],k+offsetIndices[2])= newPt;
                retval.push_back(newPt->GetTag());
              }
          }
    return retval;
  }  

//! @brief Return the points del rango being passed as parameter.
XC::TritrizPtrPnt XC::TritrizPtrPnt::getPointsOnRange(const RangoTritriz &rango)
  {
    TritrizPtrPnt retval(rango.NumCapas(),rango.NumFilas(),rango.NumCols());
    const RangoIndice &rcapas= rango.GetRangoCapas();
    const RangoIndice &rfilas= rango.GetRangoFilas();
    const RangoIndice &rcols= rango.GetRangoCols();
    const size_t rcapas_inf= rcapas.Inf();
    const size_t rfilas_inf= rfilas.Inf();
    const size_t rcols_inf= rcols.Inf();
    for(size_t i= rcapas_inf;i<=rcapas.Sup();i++)
      for(size_t j= rfilas_inf;j<=rfilas.Sup();j++)
        for(size_t k= rcols_inf;k<=rcols.Sup();k++)
          {
            Pnt *p= operator()(i,j,k);
            if(p)
              retval(i-rcapas_inf+1,j-rfilas_inf+1,k-rcols_inf+1)= p;
          }
    return retval;
  }

//! @brief Return the points which indices are being passed as parameter.
XC::Pnt *XC::TritrizPtrPnt::getPoint(const VIndices &iPoint)
  {
    Pnt *retval= nullptr;
    if(iPoint.size()>2)
      {
        if((iPoint[0]>0) && (iPoint[1]>0) && (iPoint[2]>0))
          { retval= (*this)(iPoint[0],iPoint[1],iPoint[2]); }
      }
    else
      std::cerr  << "::" << __FUNCTION__
		<< "; vector of indexes: "
                << iPoint << " is not valid." << std::endl;
    return retval;    
  }

//! @brief Return the points which indices are being passed as parameter.
XC::TritrizPtrPnt XC::TritrizPtrPnt::getPoints(const TritrizIndices &indices)
  {
    const size_t nCapas= indices.GetCapas();
    const size_t nFilas= indices.getNumFilas();
    const size_t nCols= indices.getNumCols();
    TritrizPtrPnt retval(nCapas,nFilas,nCols);
    for(size_t i= 1;i<= nCapas;i++)
      for(size_t j= 1;j<= nFilas;j++)
        for(size_t k= 1;k<= nCapas;k++)
          {
            const VIndices iPoint= indices(i,j,k);
            if(iPoint.size()>2)
              { retval(i,j,k)= getPoint(iPoint); }
            else
	      std::cerr  << "::" << __FUNCTION__
			<< "; vector of indexes: "
                        << iPoint << " is not valid." << std::endl;
          }
    return retval;
  }

//! @brief Return the points which indices are being passed as parameters.
XC::MatrizPtrPnt XC::TritrizPtrPnt::getPoints(const MatrizIndices &indices)
  {
    const size_t nFilas= indices.getNumFilas();
    const size_t nCols= indices.getNumCols();
    MatrizPtrPnt retval(nFilas,nCols);
    for(size_t i= 1;i<= nFilas;i++)
      for(size_t j= 1;j<= nCols;j++)
        {
          const VIndices iPoint= indices(i,j);
          if(iPoint.size()>2)
            { retval(i,j)= getPoint(iPoint); }
          else
            std::cerr  << "::" << __FUNCTION__
		      << "; vector of indexes: "
                      << iPoint << " is not valid." << std::endl;
        }
    return retval;
  }

//! @brief Return the cell builded by the points obtained from the positions
//! of the tritrix that result for adding to the indexex (i,j,k) of each point
//! the values of the vector offsetIndices i. e.:
//! Point (i,j,k): (i+offsetIndices(i,j,k)[0],j+offsetIndices(i,j,k)[1],k+offsetIndices(i,j,k)[2])
XC::TritrizPtrPnt XC::TritrizPtrPnt::getCellPoints(const size_t &i,const size_t &j,const size_t &k,const TritrizIndices &offsetIndices)
  {
    VIndices org(3);
    org[0]= i;org[1]= j;org[2]= k;
    TritrizIndices tmp(offsetIndices);
    tmp.Offset(org);
    return getPoints(tmp);
  }

//! @brief Return the cell builded by the points obtained from the positions
//! of the tritrix that result for adding to the indexes (i,j) of each point
//! the values of the vector offsetIndices i. e.:
//! Point (i,j): (i+offsetIndices(i,j)[0],j+offsetIndices(i,j)[1])
XC::MatrizPtrPnt XC::TritrizPtrPnt::getCellPoints(const size_t &i,const size_t &j,const MatrizIndices &offsetIndices)
  {
    VIndices org(2);
    org[0]= i;org[1]= j;
    MatrizIndices tmp(offsetIndices);
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
// std::deque<size_t> XC::TritrizPtrPnt::CreaQuads(const RangoTritriz &rango,const size_t &nf,const size_t &nc,const m_int &offsetIndices,const double &elemSizeI,const double &elemeSizeJ)
//   {
//     MultiBlockTopology *mbt= getMultiBlockTopology();
//     std::deque<size_t> retval;
//     const RangoIndice &rcapas= rango.GetRangoCapas();
//     const RangoIndice &rfilas= rango.GetRangoFilas();
//     const RangoIndice &rcols= rango.GetRangoCols();
//     for(size_t i= rcapas.Inf();i<=rcapas.Sup();i++)
//       for(size_t j= rfilas.Inf();j<=rfilas.Sup();j++)
//         for(size_t k= rcols.Inf();k<=rcols.Sup();k++)
//           {
//             const Pnt *p= operator()(i,j,k);
//             if(p)
//               {
//                 Pnt *newPt= mbt->getPoints().Copia(p,vectorOffset);
//                 (*this)(i+offsetIndices[0],j+offsetIndices[1],k+offsetIndices[2])= newPt;
//                 retval.push_back(newPt->GetTag());
//               }
//           }
//     return retval;
//   }  

void XC::TritrizPtrPnt::Print(std::ostream &os) const
  {
    const size_t ncapas= GetCapas();
    const size_t nfilas= getNumFilas();
    const size_t ncols= getNumCols();
    for(size_t i=1;i<=ncapas;i++)
      {
        for(size_t j=1;j<=nfilas;j++)
          {
            for(size_t k=1;k<=ncols;k++)
	      os << (*this)(i,j,k)->GetTag() << " ";
	    os << std::endl;
          }
        os << std::endl;
      }
  }

std::ostream &XC::operator<<(std::ostream &os, const TritrizPtrPnt &t)
  {
    t.Print(os);
    return os;
  }

//! @brief Return the indexes of the points (j,k),(j+1,k),(j+1,k+1),(j,k+1). 
std::vector<size_t> XC::getIdPointsQuad(const TritrizPtrPnt::const_ref_capa_i_cte &points,const size_t &j,const size_t &k)
  {
    std::vector<size_t> retval(4,-1);
    const size_t nfilas= points.getNumFilas();
    const size_t ncols= points.getNumCols();
    if(j>=nfilas)
      {
        std::cerr  << __FUNCTION__
	          << "; row index j= " << j << " out of range.\n";
        return retval;
      }
    if(k>=ncols)
      {
        std::cerr  << __FUNCTION__
	          << "; column index k= " << k << " out of range.\n";
        return retval;
      }


    Pos3d p1;
    const Pnt *ptr= points(j,k);
    if(ptr)
      {
        retval[0]= ptr->GetTag();
        if(retval[0]<0)
          std::cerr << __FUNCTION__
		    << "error when obtaining the point identifier ("
		    << j << ',' << k << ").\n";
        p1= ptr->GetPos();
      }

    Pos3d p2;
    ptr= points(j,k+1);
    if(ptr)
      {
        retval[1]= ptr->GetTag();
        if(retval[1]<0)
          std::cerr << __FUNCTION__
		    << "error when obtaining the point identifier ("
		    << j << ',' << k+1 << ").\n";
        p2= ptr->GetPos();
      }

    Pos3d p3;
    ptr= points(j+1,k+1);
    if(ptr)
      {
        retval[2]= ptr->GetTag();
        if(retval[2]<0)
          std::cerr << __FUNCTION__
		    << "error when obtaining the point identifier ("
		    << j+1 << ',' << k+1 << ").\n";
        p3= ptr->GetPos();
      }

    Pos3d p4;
    ptr= points(j+1,k);
    if(ptr)
      {
        retval[3]=ptr->GetTag();
        if(retval[3]<0)
          std::cerr << __FUNCTION__
		    << "error when obtaining the point identifier ("
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

    std::list<Pos3d> posiciones;
    posiciones.push_back(p1);
    posiciones.push_back(p2);
    posiciones.push_back(p3);
    posiciones.push_back(p4);
    Poligono3d tmp(posiciones.begin(),posiciones.end());
    const double area= tmp.Area();
    if(area<1e-3)
      {
        std::cerr << "When computing cell with indexes (" << j << ',' << k
                  << ") a very small area was obtained (" << area << ").\n";
        std::cerr << " position of the point (j,k) " << p1 << std::endl;
	std::cerr << " position of the point (j+1,k) " << p2 << std::endl;
	std::cerr << " position of the point (j+1,k+1) " << p3 << std::endl;
	std::cerr << " position of the point (1,k+1) " << p4 << std::endl;
      }
    return retval;
  }

