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
//BoxConstRef.h 

#ifndef BOXCONSTREF_H
#define BOXCONSTREF_H

#include "ProtoMatrix.h"
#include "RangoIndice.h"

//! @brief Base class for the references to a matrix box.
class BoxBaseRef: public ProtoMatrix
  {
  protected:
    size_t offset_f; //!< row offset.
    size_t offset_c; //!< column offset.
  public:
    BoxBaseRef(const ProtoMatrix &m,const size_t &f1= 1,const size_t &c1= 1);
    BoxBaseRef(const ProtoMatrix &m,const size_t &,const size_t &,const size_t &,const size_t &);
    BoxBaseRef(const ProtoMatrix &mat,const RangoIndice &row_range,const RangoIndice &column_range);
    BoxBaseRef(const ProtoMatrix &mat,const RangoIndice &,const size_t &);
    BoxBaseRef(const ProtoMatrix &mat,const size_t &,const RangoIndice &);
    RangoIndice getRowRange(void) const;
    RangoIndice getColumnRange(void) const;
  };

//! @brief Reference to a matrix box.
template <class MAT>
class BoxConstRef: public BoxBaseRef
  {
    const MAT &m;
  public:
    typedef typename MAT::const_reference const_reference;

    BoxConstRef(const MAT &m,const size_t &f1= 1,const size_t &c1= 1);
    BoxConstRef(const MAT &m,const size_t &,const size_t &,const size_t &,const size_t &);
    BoxConstRef(const MAT &mat,const RangoIndice &row_range,const RangoIndice &column_range);
    BoxConstRef(const MAT &mat,const RangoIndice &,const size_t &);
    BoxConstRef(const MAT &mat,const size_t &,const RangoIndice &);
    virtual const_reference operator()(size_t iRow=1,size_t col=1) const
      { return m(iRow+offset_f,col+offset_c); }
    void Print(std::ostream &) const;
  };

//! @brief Constructor por defecto.
template<class MAT>
BoxConstRef<MAT>::BoxConstRef(const MAT &mat,const size_t &f1,const size_t &c1)
  : BoxBaseRef(mat,f1,c1), m(mat) {}

//! @brief Constructor.
template<class MAT>
BoxConstRef<MAT>::BoxConstRef(const MAT &mat,const size_t &f1,const size_t &c1,const size_t &f2,const size_t &c2)
  : BoxBaseRef(mat,f1,c1,f2,c2), m(mat)
  {}

//! @brief Constructor.
template<class MAT>
BoxConstRef<MAT>::BoxConstRef(const MAT &mat,const RangoIndice &row_range,const RangoIndice &column_range)
  : BoxBaseRef(mat,row_range,column_range), m(mat) {}

//! @brief Column alone constructor.
template<class MAT>
BoxConstRef<MAT>::BoxConstRef(const MAT &mat,const RangoIndice &row_range,const size_t &col)
  : BoxBaseRef(mat,row_range,col), m(mat) {}

//! @brief Row alone constructor.
template<class MAT>
BoxConstRef<MAT>::BoxConstRef(const MAT &mat,const size_t &iRow,const RangoIndice &column_range)
  : BoxBaseRef(mat,iRow,column_range), m(mat) {}

template<class MAT>
void BoxConstRef<MAT>::Print(std::ostream &os) const
  {
    os << '[';
    size_t n_rows= this->getNumberOfRows(),n_columns= this->getNumColumns();
    for(size_t i= 1;i<=n_rows;i++)
      {
        os << '[';
        if(n_columns > 0) os << (*this)(i,1);
	for(size_t j= 2;j<=n_columns;j++)
	  os << ',' << (*this)(i,j);
        os << ']';
      }
    os << ']';
  }

template <class MAT>
inline std::ostream &operator<<(std::ostream &os,const BoxConstRef<MAT> &c)
  {
    c.Print(os);
    return os;
  }

#endif
