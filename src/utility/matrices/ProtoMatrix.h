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
//ProtoMatrix.h 
#ifndef PROTOMATRIX_H
#define PROTOMATRIX_H

#include <string>
#include <iostream>

//! @defgroup Matrices
//! @ingroup Matrices
//! 
//! @brief Base class for matrices.
class ProtoMatrix
  {
  protected:
    size_t n_rows; //number of rows.
    size_t n_columns; //number of columns.
    inline void PutDim(size_t nRows,size_t nCols)
      {
        n_rows= nRows;
        n_columns= nCols;
      }
    inline void inic(size_t n_rows,size_t n_columns)
      { PutDim(n_rows,n_columns); }
    inline virtual bool check_range(const size_t &iRow,const size_t &col) const
      { return ((iRow<=n_rows) && (col<=n_columns)); }
    inline void check_put_box(size_t f,size_t c,const ProtoMatrix &box) const
      {
        if(((f+box.getNumberOfRows())>(n_rows+1)) || ((c+box.getNumberOfColumns())>(n_columns+1)))
          std::cerr << __FUNCTION__
	            << "; indices out of range." << std::endl;
      }
    inline void check_get_box(size_t f1, size_t c1, size_t f2, size_t c2) const
      {
        if ( (f2 < f1) || (c2 < c1) )
          std::cerr << "Erroneous indexes in "
	            << __FUNCTION__ << " function." << std::endl;
      }
    inline void check_sto_sum(const ProtoMatrix &m) const
      {
        if (!CompDim(*this,m))
          std::cerr << "Matrices de dimensiones distintas en operador += " << std::endl
                    << "  this: " << " (" << n_rows << 'x' << n_columns << ')' << std::endl
                    << "  m= " << " (" << m.n_rows << 'x' << m.n_columns << ')' << std::endl;
      }
    inline void check_sto_dif(const ProtoMatrix &m) const
      {
        if (!CompDim(*this,m))
          std::cerr << "Matrices de dimensiones distintas en operador -="  << std::endl
                    << "  this: " << " (" << n_rows << 'x' << n_columns << ')' << std::endl
                    << "  m= " << " (" << m.n_rows << 'x' << m.n_columns << ')' << std::endl;
      }
    inline void check_traza(void) const
      {
        if(!Cuadrada())
          std::cerr << "Not a square matrix." << std::endl;
      }
    inline friend int check_dot(const ProtoMatrix &v1,const ProtoMatrix &v2)
      {
        if (!v1.isRow())
          std::cerr << "First matrix of scalar product is not a row matrix."
		    << std::endl;
        if (!v2.isColumn())
          std::cerr << "Second matrix of scalar product is not a column matrix."
		    << std::endl;
        if (v1.n_columns != v2.n_rows)
          std::cerr << "Matrices de dimensiones incompatibles en producto escalar."
                    << "  m1= " << v1 << " (" << v1.n_rows << 'x' << v1.n_columns << ')' << std::endl
                    << "  m2= " << v2 << " (" << v2.n_rows << 'x' << v2.n_columns << ')'
                    << std::endl;
        return 1;
      }
    inline friend int check_sum(const ProtoMatrix &m1,const ProtoMatrix &m2)
      {
        if (!CompDim(m1,m2))
          {
            std::cerr << "Matrices de dimensiones distintas en operador +" 
                 /* << m1.GetStrDim() << ' ' << m2.GetStrDim() */ << std::endl;
            return 0;
          }
        return 1;
      }
    inline friend int check_dif(const ProtoMatrix &m1,const ProtoMatrix &m2)
      {
        if (!CompDim(m1,m2))
          {
            std::cerr << "Matrices de dimensiones distintas en operador -"  
                 /* << m1.GetStrDim() << ' ' << m2.GetStrDim() */ << std::endl;
            return 0;
          }
        return 1;
      }
    inline friend int check_prod(const ProtoMatrix &m1,const ProtoMatrix &m2)
      {
        if (m1.n_columns != m2.n_rows)
          {
            std::cerr << "Matrices de dimensiones incompatibles en producto." << std::endl;
            std::cerr << "  m1= " << m1 << std::endl;
            std::cerr << "  m2= " << m2 << std::endl;
            return 0;
          }
        return 1;
      }
  public:
    ProtoMatrix(size_t n_rows= 1,size_t n_columns= 1)
      { inic(n_rows,n_columns); }
    ProtoMatrix(const ProtoMatrix &other)
      { inic(other.n_rows,other.n_columns); }
    ProtoMatrix& operator =(const ProtoMatrix &m)
      {
        inic(m.n_rows,m.n_columns);
        return *this;
      }
    virtual bool operator==(const ProtoMatrix &other) const
      {
	bool retval= false;
	if(this==&other)
	  retval= true;
	else
	  {
	    retval= (this->n_rows==other.n_rows) && (this->n_columns==other.n_columns);
	  }
	return retval;
      }
    virtual ~ProtoMatrix(void) {}
    inline virtual void resize(size_t n_rows,size_t n_columns)
      { inic(n_rows,n_columns); }
    inline virtual size_t Tam(void)
      { return (n_rows*n_columns); }
    inline size_t getNumberOfRows(void) const
      { return n_rows; }                    
    inline size_t getNumberOfColumns(void) const
      { return n_columns; }
    inline bool CheckIndices(const size_t &f,const size_t &c) const
      { return check_range(f,c);; }
    //! @brief Return true if the indices correspond to a component
    //" of the matrix "interior".
    inline bool interior(const size_t &i,const size_t &j) const
      { return ( (i>1) && (j>1) && (i<n_rows) && (j<n_columns) ); }
    inline int Cuadrada(void) const
      { return (n_rows == n_columns); }
    inline bool isRow(void) const
      { return (n_rows == 1); }
    inline bool isColumn(void) const
      { return (n_columns == 1); }
    virtual void Print(std::ostream &os) const=0;
    friend inline bool compareRowNumber(const ProtoMatrix &m1,const ProtoMatrix &m2)
      { return (m1.n_rows == m2.n_rows); }
    friend inline bool compareColumnNumber(const ProtoMatrix &m1,const ProtoMatrix &m2)
      { return (m1.n_columns == m2.n_columns); }
    friend inline bool CompDim(const ProtoMatrix &m1,const ProtoMatrix &m2)
      { return (compareRowNumber(m1,m2) && compareColumnNumber(m1,m2)); }
    friend inline std::ostream &operator<<(std::ostream &os,const ProtoMatrix &m)
      {
        m.Print(os);
        return os;
      }
  };

#endif
