//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
//
//  Copyright (C)  Luis C. Pérez Tato
//
//  XC utils is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//CooSys.cc

#include "CooSys.h"
#include "../pos_vec/Pos3d.h"

#include "utility/utils/text/text_string.h"
#include "utility/functions/algebra/ExprAlgebra.h"


//! @brief Constructor.
CooSys::CooSys(const size_t &ne,const size_t &dim)
  : ProtoGeom(), rot(ne,dim) 
  { identity(); }

//! @brief Row-normalize matrix.
void CooSys::normalize(void)
  {
    if(!rot.Nulo())
      NormalizeRows(rot);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << rot << " es nula." << std::endl;
  }
//! @brief Make the base orthogonal.
void CooSys::ortogonaliza(void)
  {
    const size_t ne= numberOfAxis();
    if(ne==1)
      return;
    else //2 o 3 dimensions.
      {
        const FT_matrix i= rot.getRow(1);
        const FT_matrix j= rot.getRow(2);
        const GEOM_FT imod2= i.Abs2();
        if(imod2<geom_epsilon2)
          {
	    std::cerr << getClassName() << "::" << __FUNCTION__
	              << "; orthogonalize: unit vector i= " << i
                      << " is too small (" << imod2 << ")." << std::endl;
            return;
          }
        else
          {
            const GEOM_FT alpha= -(dot(j,i))/imod2;
            const FT_matrix nj= j+alpha*i;
            putRow(2,nj);
	  }
        if(ne>2) //3 dimensiones.
          {
            const FT_matrix nk= i ^ getRow(2);
            putRow(3,nk);
          }
      }
  }
//! @brief Makes the base orthonormal.
void CooSys::orthonormalize(void)
  {
    ortogonaliza();
    normalize();
  }
//! @brief Aligns the coordinate system with the global one.
//! i. e:
//! - If it's a one-dimensional coordinate system it is placed on the
//! first axis of the global coordinate system.
//! - If it's a two-dimensional coordinate system it is placed on the
//! first axis and second axis of the global coordinate system.
//! - If it's a three-dimensional coordinate system we make it match the
//! global one.
void CooSys::identity(void)
  {
    rot.Con(0);
    for(size_t i=1;i<=rot.getNumberOfRows();i++)
      rot(i,i)= 1;
  }
//! @brief Assigns the (i,j) component of the matrix.
void CooSys::put(const size_t &i,const size_t &j,const GEOM_FT &rot_ij)
  { rot(i,j)= rot_ij; }

//! @brief Return the matrix i row.
FT_matrix CooSys::getRow(const size_t &i) const
  {
    assert(i<=numberOfAxis());
    return rot.getRow(i);
  }
//! @brief Return true if the vectors are normalized.
bool CooSys::EsNormal(void) const
  {
    const size_t ne= numberOfAxis();
    for(size_t i=1;i<=ne;i++)
      {
        const GEOM_FT imod= rot.getRow(i).Abs2();
        if((imod-1)> geom_epsilon)
          return false;
      }
    return true;
  }

//! @brief Return true if it's a dextrorotatory coordinate system.
bool CooSys::EsDextrogiro(void) const
  {
    if(numberOfAxis()<3)
      return true;
    else
      {
        if(rot.getNumberOfColumns()<3)
          return true;
        else
          {
            const FT_matrix i= getRow(1).GetTrn();
            const FT_matrix j= getRow(2).GetTrn();
            const FT_matrix ivectj= i^j;
            return (dot(getRow(3),ivectj)>0);
          }
      }
  }

//! @brief Return verdadero si los vectores son ortogonales.
bool CooSys::EsOrtogonal(void) const
  {
    const size_t ne= numberOfAxis();
    if(ne<2)
      return true;
    else
      if(ne<3)
        return (::Abs(dot(rot.getRow(1),rot.getRow(2).GetTrn()))<geom_epsilon);
      else
        {
          if(::Abs(dot(rot.getRow(1),rot.getRow(2).GetTrn()))>geom_epsilon)
            return false;
          if(::Abs(dot(rot.getRow(1),rot.getRow(3).GetTrn()))>geom_epsilon)
            return false;
          if(::Abs(dot(rot.getRow(2),rot.getRow(3).GetTrn()))>geom_epsilon)
            return false;
        }
    return true;
  }

//! @brief Return verdadero si los vectores son unitarios y ortogonales.
bool CooSys::EsOrtonormal(void) const
  { return (EsOrtogonal() && EsNormal()); }

//! @brief Set the i row of the matrix.
void CooSys::putRow(const size_t &axis,const FT_matrix &v)
  { rot.putRow(axis,v); }
//! @brief Return the matrix que transforma un vector expresado
//! en locales al mismo vector expresado en globales.
FT_matrix CooSys::TransAGlobal(void) const
  { return rot.GetTrn(); }
//! @brief Return the matrix que transforma un vector expresado
//! en globales al mismo vector expresado en locales.
FT_matrix CooSys::TransDeGlobal(void) const
  { return rot; }
//! @brief Return the transformation matrix from this system to
//! the argument one.
FT_matrix CooSys::GetTransA(const CooSys &dest) const
  { return (dest.rot*TransAGlobal()); }

//! @brief Return the global coordinates of the vector.
//!
//! @param v: vector in local coordinates.
FT_matrix CooSys::getGlobalCoordinates(const FT_matrix &v) const
  { return TransAGlobal()*v; }

//! @brief Return the local coordinates of the vector.
//!
//! @param v: vector in global coordinates.
FT_matrix CooSys::getLocalCoordinates(const FT_matrix &v) const
  { return TransDeGlobal()*v; }

//! @brief Prints the matrix.
void CooSys::Print(std::ostream &os) const
  { os << rot; }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict CooSys::getPyDict(void) const
  {
    boost::python::dict retval= ProtoGeom::getPyDict();
    retval["nr"]= rot.getNumberOfRows();
    retval["nc"]= rot.getNumberOfColumns();
    boost::python::list values;
    for(FT_matrix::const_iterator i= rot.begin();i!=rot.end(); i++)
      values.append(*i);
    retval["values"]= values;
    return retval;
  }

//! @brief Set the values of the object members from a Python dictionary.
void CooSys::setPyDict(const boost::python::dict &d)
  {
    ProtoGeom::setPyDict(d);
    const size_t nr= boost::python::extract<size_t>(d["nr"]);
    const size_t nc= boost::python::extract<size_t>(d["nc"]);
    const boost::python::list values= boost::python::extract<boost::python::list>(d["values"]);
    const size_t sz= nr*nc;
    std::vector<FT_matrix::value_type> v(sz);
    for(size_t i= 0;i<sz;i++)
      v[i]= boost::python::extract<FT_matrix::value_type>(values[i]);
    rot= FT_matrix(nr, nc, v.begin(), v.end());
  }

std::ostream &operator<<(std::ostream &os,const CooSys &sc)
  {
    sc.Print(os);
    return os;
  }
