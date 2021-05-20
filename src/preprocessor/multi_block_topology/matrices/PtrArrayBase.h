// -*-c++-*-
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
//PtrArrayBase.h
//Pointers in a matrix structure.

#ifndef PTRARRAYBASE_H
#define PTRARRAYBASE_H

#include "utility/kernel/CommandEntity.h"
#include "utility/matrices/TMatrix.h"
#include <vector>
#include "boost/any.hpp"
#include "boost/lexical_cast.hpp"


#include "utility/matrices/m_int.h"

namespace XC{


//! @ingroup MultiBlockTopologyMR
//
//! @brief Base class for matrices of pointers to nodes, elements and points.
template <class T>
class PtrArrayBase: public TMatrix<T *,std::vector<T *> >, public CommandEntity
  {
  public:
    typedef TMatrix<T *,std::vector<T *> > m_ptr;
    typedef typename m_ptr::iterator iterator;
    typedef typename m_ptr::const_iterator const_iterator;
    typedef typename m_ptr::value_type value_type;
    typedef typename m_ptr::reference reference;
    typedef typename m_ptr::const_reference const_reference;
  protected:
    //! @brief Constructor.
    PtrArrayBase(const size_t &f=0,const size_t &c=0, const value_type &def_value= nullptr)
      : m_ptr(f,c,def_value), CommandEntity() {}
  public:
    virtual bool operator==(const PtrArrayBase<T> &) const;
    bool Null(void) const;
    bool HasNull(void) const;

    void dim(const size_t &,const size_t &);

    std::vector<int> getRowObjectsTags(const size_t &);
    std::vector<int> getColumnObjectsTags(const size_t &);
    std::vector<int> getRowInteriorObjectsTags(const size_t &);
    std::vector<int> getColumnInteriorObjectsTags(const size_t &);
    std::vector<int> getTagsInteriorObjs(void);
    std::vector<int> getTagsObjs(void);
  };

//! @brief Matrix dimensions.
template <class T>
void XC::PtrArrayBase<T>::dim(const size_t &nRows,const size_t &numberOfColumns)
  { this->resize(nRows,numberOfColumns,nullptr); }

//! @brief Comparison operator.
template <class T>
bool XC::PtrArrayBase<T>::operator==(const PtrArrayBase<T> &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= m_ptr::operator==(other);
	retval= retval && CommandEntity::operator==(other);
      }
    return retval;
  }

 
//! @brief Returns true if it's empty or the pointers are NULL.
template <class T>
bool PtrArrayBase<T>::Null(void) const
  {
    if(this->empty())
      return true;
    else
      return (this->operator()(1,1)==nullptr);
  }

//! @brief Returns true if it's empty or any of the pointers are NULL.
template <class T>
bool PtrArrayBase<T>::HasNull(void) const
  {
    bool retval= false;
    if(Null())
      retval= true;
    else
      {
        const size_t numberOfRows= this->getNumberOfRows();
        const size_t numberOfColumns= this->getNumberOfColumns();
        for(size_t j= 1;j<=numberOfRows;j++)
          for(size_t k= 1;k<=numberOfColumns;k++)
            if(this->operator()(j,k)== nullptr)
              {
                retval= true;
                break;
              }
      }
    return retval;
  }

//! @brief Asks each of the objects in the row to execute
//! the code being passed as parameter.
template <class T>
std::vector<int> XC::PtrArrayBase<T>::getRowObjectsTags(const size_t &f)
  {
    const std::string nmbBlq= getClassName()+":row_objects:"+boost::lexical_cast<std::string>(f);
    const size_t numCols= this->getNumberOfColumns();
    std::vector<int> retval(numCols);
    for(size_t i= 1;i<=numCols;i++)
      retval[i-1]= (*this)(f,i)->getTag();
    return retval;
  }

//! @brief Asks each of the objects in the column to execute
//! the code being passed as parameter.
template <class T>
std::vector<int>  XC::PtrArrayBase<T>::getColumnObjectsTags(const size_t &c)
  {
    const std::string nmbBlq= getClassName()+":column_objects:"+boost::lexical_cast<std::string>(c);
    const size_t n_rows= this->getNumberOfRows();
    std::vector<int> retval(n_rows);
    for(size_t i= 1;i<=n_rows;i++)
      retval[i-1]= (*this)(i,c)->getTag();
    return retval;
  }

//! @brief Asks each of the objects at the interior of the row to execute
//! the code being passed as parameter.
template <class T>
std::vector<int> XC::PtrArrayBase<T>::getRowInteriorObjectsTags(const size_t &f)
  {
    const std::string nmbBlq= getClassName()+":row_interior_objects:"+boost::lexical_cast<std::string>(f);
    const size_t numCols= this->getNumberOfColumns();
    std::vector<int> retval(numCols-2);
    for(size_t i= 2;i<numCols;i++)
      retval[i-2]= (*this)(f,i)->getTag();
    return retval;
  }

//! @brief Asks each of the objects at the interior of the column to execute
//! the code being passed as parameter.
template <class T>
std::vector<int> XC::PtrArrayBase<T>::getColumnInteriorObjectsTags(const size_t &c)
  {
    const std::string nmbBlq= getClassName()+":column_interior_objects:"+boost::lexical_cast<std::string>(c);
    const size_t n_rows= this->getNumberOfRows();
    std::vector<int> retval(n_rows-2);
    for(size_t i= 2;i<n_rows;i++)
      retval[i-2]= (*this)(i,c)->getTag();
    return retval;
  }

//! @brief Asks each of the objects at the interior to execute
//! the code being passed as parameter.
template <class T>
std::vector<int> XC::PtrArrayBase<T>::getTagsInteriorObjs(void)
  {
    const size_t n_rows= this->getNumberOfRows();
    const size_t numCols= this->getNumberOfColumns();
    std::vector<int> retval((n_rows-1)*(numCols-1));
    if(n_rows==1)
      retval= this->getRowInteriorObjectsTags(1);
    if(numCols==1)
      retval= this->getColumnInteriorObjectsTags(1);
    else
      {
        m_int tmp(n_rows-2,numCols-2);
        for(size_t i= 2;i<n_rows;i++)
          for(size_t j= 2;j<numCols;j++)
    	    tmp(i-1,j-1)= (*this)(i,j)->getTag();
        retval= tmp.getVector();
      }
    return retval;
  }

template <class T>
std::vector<int> XC::PtrArrayBase<T>::getTagsObjs(void)
  {
    const size_t n_rows= this->getNumberOfRows();
    const size_t numCols= this->getNumberOfColumns();
    std::vector<int> retval(n_rows*numCols);
    m_int tmp(n_rows,numCols);
    for(size_t i= 1;i<=n_rows;i++)
      for(size_t j= 1;j<=numCols;j++)
        tmp(i,j)= (*this)(i,j)->getTag();
    retval= tmp.getVector();
    return retval;
  }

} //end of XC namespace

#endif
