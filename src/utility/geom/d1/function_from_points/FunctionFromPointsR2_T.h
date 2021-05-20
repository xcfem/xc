// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
//FunctionFromPointsR2_T.h

#ifndef FUNCTIONFROMPOINTSR2_T_H
#define FUNCTIONFROMPOINTSR2_T_H

#include <map>
#include <iostream>
#include <utility/geom/pos_vec/Pos2dArray.h>
#include <utility/geom/pos_vec/Vector2d.h>

//! @brief f:R2->T defined on a set of points.
//!
//! Function that for each point P of the plane returns a
//! T class value. The correspondence is established by points,
//! that is, by pairs (x1,T1), (x2,T2), ...
template <class T>
class FunctionFromPointsR2_T
  {
  public:
    typedef TMatrix<T,std::vector<T> > m_values;
  private:
    Pos2dArray dominio; //< @brief Points where the function is defined.
    m_values valores; //< @brief Function values on each point.
  public:
    //! @brief Constructor.
    FunctionFromPointsR2_T(const size_t &n_rows=1,const size_t &n_columns=1)
      : dominio(n_rows,n_columns), valores(n_rows,n_columns) {}
    //! @brief Constructor.
    FunctionFromPointsR2_T(const Pos2dArray &dom,const T &vdef)
      : dominio(dom), valores(m_values(dom.getNumberOfRows(),dom.getNumberOfColumns(),vdef)) {}
    //! @brief Constructor.
    FunctionFromPointsR2_T(const Pos2dArray &dom,const m_values &v);
    std::string getClassName(void) const;
    //! @brief Return el number of rows del dominio.
    size_t getNumberOfRows(void) const
      { return dominio.getNumberOfRows(); }
    //! @brief Return el number of columns del dominio.
    size_t getNumberOfColumns(void) const
      { return dominio.getNumberOfColumns(); }
    //! @brief Return true if there are no points.
    size_t empty(void) const
      { return dominio.empty(); }
    const Pos2d &Posicion(const size_t &i,const size_t &j) const;
    Pos2d &Posicion(const size_t &i,const size_t &j);
    const T &Valor(const size_t &i,const size_t &j) const;
    T &Valor(const size_t &i,const size_t &j);
  };

//! Constructor.
template <class T>
FunctionFromPointsR2_T<T>::FunctionFromPointsR2_T(const Pos2dArray &dom,const m_values &v)
  : dominio(dom), valores(m_values(dom.getNumberOfRows(),dom.getNumberOfColumns(),T()))
  {
    if((v.getNumberOfRows()==dom.getNumberOfRows()) && (v.getNumberOfColumns()==dom.getNumberOfColumns()) )
      valores= v;
    else
      {
	std::cerr << this->getClassName() << "::" << __FUNCTION__
		  << ": the value matrix has dimensions different"
	          << " than those of the point matrix."
		  << std::endl;
      }
  }

//! @brief Returns demangled class name.
template <class T>
std::string FunctionFromPointsR2_T<T>::getClassName(void) const
  {
    std::string tmp= typeid(*this).name();
    std::bad_exception  e;
    int status;
    char *realname= abi::__cxa_demangle(tmp.c_str(), 0, 0, &status);
    if(realname)
      tmp= std::string(realname);
    free(realname);
    return tmp;
  }


//! @brief Return the point de índices (i,j).
template <class T>
const Pos2d &FunctionFromPointsR2_T<T>::Posicion(const size_t &i,const size_t &j) const
  { return dominio(i,j); }
//! @brief Return a reference to the point at position (i,j).
template <class T>
Pos2d &FunctionFromPointsR2_T<T>::Posicion(const size_t &i,const size_t &j)
  { return dominio(i,j); }
//! @brief Return the value that corresponds to the point a position (i,j).
template <class T>
const T &FunctionFromPointsR2_T<T>::Valor(const size_t &i,const size_t &j) const
  { return valores(i,j); }
//! @brief Return a reference to the the value that corresponds to the point
//! a position (i,j).
template <class T>
T &FunctionFromPointsR2_T<T>::Valor(const size_t &i,const size_t &j)
  { return valores(i,j); }
template <class T>
std::ostream &operator<<(std::ostream &os, const FunctionFromPointsR2_T<T> &m)
  {
    const size_t n_rows= m.getNumberOfRows();
    const size_t n_columns= m.getNumberOfColumns();
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        os << "position: " << m.Posicion(i,j) << " valor: " << m.Valor(i,j) << std::endl;
    return os;
  }


#endif
