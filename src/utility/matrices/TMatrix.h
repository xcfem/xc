// -*-c++-*-
//----------------------------------------------------------------------------
//  xc utils library; general purpose classes and functions.
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
//TMatrix.h 
#ifndef TMATRIX_H
#define TMATRIX_H

#include <iostream>
#include "ProtoMatrix.h"
#include "BoxConstRef.h"
#include "RowConstRef.h"
#include "ColumnConstRef.h"
#include <list>
#include <algorithm>
#include <sstream>

//T Elementos a almacenar.
//STO Objeto para almacenamiento.
template <class T, class STO>
class TMatrix: public ProtoMatrix, public STO
  {
  protected:
    inline virtual size_t Indice(const size_t &iRow,const size_t &iCol) const
      { return iRow*n_columns-n_columns+iCol-1; }
    bool equal_to(const TMatrix<T,STO> &m2) const;
    TMatrix(const TMatrix<T,STO> &orig,size_t f1, size_t c1, size_t f2, size_t c2);
  public:
    typedef std::list<T> lst_T;
    typedef typename STO::iterator iterator;
    typedef typename STO::const_iterator const_iterator;
    typedef typename STO::value_type value_type;
    typedef typename STO::reference reference;
    typedef typename STO::const_reference const_reference;
    typedef BoxConstRef<TMatrix<T,STO> > box_const_ref;
    typedef RowConstRef<TMatrix<T,STO> > row_const_ref;
    typedef ColumnConstRef<TMatrix<T,STO> > const_ref_col;

    TMatrix(size_t rows= 1,size_t n_columns= 1);
    TMatrix(size_t n_rows,size_t n_columns,T val);
    template <class InputIterator>
    TMatrix(const size_t &n_rows,const size_t &n_columns,InputIterator b,InputIterator e);
    TMatrix(const TMatrix<T,STO> &);
    TMatrix<T,STO>& operator=(const TMatrix<T,STO> &m);
    TMatrix<T,STO>& operator=(const T &n)
      { return Con(n); }
    virtual bool operator==(const TMatrix<T,STO> &) const;
    void resize(size_t n_rows,size_t n_columns,T val);
    inline size_t size(void) const
      { return STO::size(); }
    inline const STO &getVector(void) const
      { return static_cast<const STO &>(*this); }

    inline const_reference front() const
      { return STO::front(); }
    inline reference front()
      { return STO::front(); }
    inline const_reference back() const
      { return STO::back(); }
    inline reference back()
      { return STO::back(); }
    inline const_iterator begin() const
      { return STO::begin(); }
    inline iterator begin()
      { return STO::begin(); }
    inline const_iterator end() const
      { return STO::end(); }
    inline iterator end()
      { return STO::end(); }
    TMatrix<T,STO>& Con(const T &t);
    virtual reference operator()(size_t iRow=1,size_t col=1)
      { return STO::operator[](Indice(iRow,col)); }
    virtual const_reference operator()(size_t iRow=1,size_t col=1) const
      { return STO::operator[](Indice(iRow,col)); }
    virtual reference at(size_t iRow=1,size_t col=1)
      { return STO::at(Indice(iRow,col)); }
    virtual const_reference at(size_t iRow=1,size_t col=1) const
      { return STO::at(Indice(iRow,col)); }
    //! @brief Put the box int the position (f,c) of this matrix.
    void assign(size_t row,size_t col,const T &value)
      { STO::assign(Indice(row,col),value); }
    void swap(size_t f1,size_t c1,size_t f2, size_t c2)
      { std::swap((*this)(f1,c1),(*this)(f2,c2)); }    
    TMatrix<T,STO> &Trn(void)
      {
        TMatrix<T,STO> temp= GetTrn();
        (*this)= temp;
        return *this;
      }
    TMatrix<T,STO> GetTrn(void) const;
    TMatrix<T,STO> getBox(size_t f1, size_t c1, size_t f2, size_t c2) const
      { return TMatrix<T,STO>(*this,f1,c1,f2,c2); }
    box_const_ref GetBoxConstRef(size_t f1, size_t c1, size_t f2, size_t c2) const
      { return box_const_ref(*this,f1,c1,f2,c2); }
    box_const_ref GetBoxConstRef(const RangoIndice &row_range,const RangoIndice &column_range) const
      { return box_const_ref(*this,row_range,column_range); }
    box_const_ref GetBoxConstRef(size_t f=1, size_t c=1) const
      { return box_const_ref(*this,f,c); }
    TMatrix<T,STO> getRow(size_t iRow) const
      { return getBox(iRow,1,iRow,n_columns); }
    row_const_ref getRowConstRef(size_t f, size_t c1, size_t c2) const
      { return row_const_ref(*this,f,c1,c2); }
    row_const_ref getRowConstRef(size_t f, const RangoIndice &column_range) const
      { return row_const_ref(*this,f,column_range); }
    row_const_ref getRowConstRef(size_t f=1, size_t c=1) const
      { return row_const_ref(*this,f,c); }
    const_ref_col getColumnConstRef(size_t c, size_t f1, size_t f2) const
      { return const_ref_col(*this,c,f1,f2); }
    const_ref_col getColumnConstRef(const RangoIndice &row_range,size_t c) const
      { return const_ref_col(*this,row_range,c); }
    const_ref_col getColumnConstRef(size_t c=1, size_t f=1) const
      { return const_ref_col(*this,c,f); }
    TMatrix<T,STO> getColumn(size_t col) const
      { return getBox(1,col,n_rows,col); }
    TMatrix<T,STO> GetMenor(size_t f,size_t c) const;
    void putBox(size_t f,size_t c,const TMatrix<T,STO> &);      
    void putRow(size_t iRow,const TMatrix<T,STO> &f)
      {
        if (!compareColumnNumber(*this,f)) return;
        putBox(iRow,1,f);
      }
    void PutCol(size_t col,const TMatrix<T,STO> &c)
      {
        if (!compareRowNumber(*this,c)) return;
        putBox(1,col,c);
      }
    void OrlaCol(const TMatrix<T,STO> &c)
      {
        TMatrix<T,STO> m(n_rows,n_columns+1);
        m.putBox(1,1,*this);
        m.PutCol(n_columns+1,c);
        *this= m;
      }
    void decorateRow(const TMatrix<T,STO> &f)
      {
        TMatrix<T,STO> m(n_rows+1,n_columns);
        m.putBox(1,1,*this);
        m.putRow(n_rows+1,f);
        *this= m;
      }
    void swapRows(size_t f1,size_t f2);
    void swapColumns(size_t c1,size_t c2);
    virtual void Print(std::ostream &) const;
    virtual void Input(std::istream &);
    virtual void Input(const std::string &);
    virtual ~TMatrix(void) {}
  };

//! @brief Constructor por defecto.
template <class T,class STO>
TMatrix<T,STO>::TMatrix(size_t n_rows,size_t n_columns)
  : ProtoMatrix(n_rows,n_columns), STO(Tam()) {}
//! @brief Constructor.
template <class T,class STO>
TMatrix<T,STO>::TMatrix(size_t n_rows,size_t n_columns,T val)
  : ProtoMatrix(n_rows,n_columns), STO(Tam(),val) {}
//! @brief Constructor de copia.
template <class T,class STO>
TMatrix<T,STO>::TMatrix(const TMatrix<T,STO> &other) 
  : ProtoMatrix(other),STO(other) {}

//! @brief Constructor con lista inicialización.
template <class T,class STO> template<class InputIterator>
TMatrix<T,STO>::TMatrix(const size_t &n_rows,const size_t &n_columns,InputIterator b,InputIterator e)
  : ProtoMatrix(n_rows,n_columns), STO(b,e)
  {
    if(STO::size()!=(n_rows*n_columns))
      std::cerr << "TMatrix; the number of elements of the list"
	        << " is not equal to the number of elements of the"
	        << " matrix." << std::endl;
  }


//! @brief Assignment operator.
template <class T,class STO>
TMatrix<T,STO>& TMatrix<T,STO>::operator=(const TMatrix<T,STO> &m)
  {
    ProtoMatrix::operator=(m);
    STO::operator=(m);
    return *this;
  }

//! @brief Assignment operator.
template <class T,class STO>
void TMatrix<T,STO>::resize(size_t n_rows,size_t n_columns,T val)
  {
    ProtoMatrix::resize(n_rows,n_columns);
    STO::resize(Tam());
  }

//! @brief Comparison operator.
template <class T,class STO>
bool TMatrix<T,STO>::operator==(const TMatrix<T,STO> &other) const
  {
    bool retval= false;
    if(this==&other)
      retval= true;
    else
      {
        retval= ProtoMatrix::operator==(other);
        if(retval)
          retval= this->equal_to(other);
      }
    return retval;
  }
 
template <class MATR>
MATR GetMenor(const MATR &matrix,const size_t &f,const size_t &c)
  {
    const size_t fl= matrix.getNumberOfRows();
    const size_t cl= matrix.getNumberOfColumns();
    MATR menor(fl-1,cl-1);
    size_t m,m1,p,p1;
    for(m= m1= 1; m<=fl;m++)
      {
        if (m == f) continue;
        for(p= p1= 1;p<=cl;p++)
          {
            if (p == c) continue;
            menor(m1,p1)= matrix(m,p);
            p1++;
          }
        m1++;
      }
    return menor;
  }

//! @brief Return the minor of the matrix that corresponds to the
//! row and the column arguments.
template <class T,class STO>
TMatrix<T,STO> TMatrix<T,STO>::GetMenor(size_t f,size_t c) const
  { return ::GetMenor(*this,f,c); }

//! @brief Put the box int the position (f,c) of this matrix.
template <class T,class STO>
void TMatrix<T,STO>::putBox(size_t f,size_t c,const TMatrix<T,STO> &box)
  {
    check_put_box(f,c,box);
    for(size_t i=1;i<=box.n_rows;i++)
      for(size_t j=1;j<=box.n_columns;j++)
        TMatrix<T,STO>::operator()(i+f-1,j+c-1)= box(i,j);
  }

//! @brief Operador salida.
template <class T,class STO>
std::ostream &operator<<(std::ostream &os,const TMatrix<T,STO> &m)
  {
    m.Print(os);
    return os;
  }

//! @brief Lectura desde istream.
template <class T,class STO>
void TMatrix<T,STO>::Input(std::istream &is)
  {
    std::cerr << "TMatrix<T,STO>::" << __FUNCTION__
              << " not implemented." << std::endl;
    return;
  }

//! @brief Lectura desde string.
template <class T,class STO>
void TMatrix<T,STO>::Input(const std::string &str)
  {
    std::istringstream iss(str);
    Input(iss);
  }

template <class T,class STO>
std::istream &operator >> (std::istream &is,TMatrix<T,STO> &m)
  {
    m.Input(is);
    return is;
  }

template <class M>
inline M traspuesta(const M &m)
  { return m.GetTrn(); }

template<class T,class STO>
TMatrix<T,STO>::TMatrix(const TMatrix<T,STO> &orig,size_t f1, size_t c1, size_t f2, size_t c2): ProtoMatrix(f2-f1+1,c2-c1+1), STO(Tam()) 
  {
    orig.check_get_box(f1,c1,f2,c2);
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        (*this)(i,j)= orig(i+f1-1,j+c1-1);
  }

template<class T,class STO>
TMatrix<T,STO>& TMatrix<T,STO>::Con(const T &t)
  {
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        (*this)(i,j)= t;
    return *this;
  }

template<class T,class STO>
TMatrix<T,STO> TMatrix<T,STO>::GetTrn(void) const
  {
    TMatrix<T,STO> retval(n_columns,n_rows);
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        retval(j,i)= (*this)(i,j); 
    return retval;
  }
template<class T,class STO>
void TMatrix<T,STO>::swapRows(size_t f1,size_t f2)
  { for(size_t c=1;c<=n_columns;c++) swap(f1,c,f2,c); }
template<class T,class STO>
void TMatrix<T,STO>::swapColumns(size_t c1,size_t c2)
  { for(size_t f=1;f<=n_rows;f++) swap(f,c1,f,c2); }
template<class T,class STO>
void TMatrix<T,STO>::Print(std::ostream &os) const
  {
    os << '[';
    const size_t n_rows= getNumberOfRows(),n_columns= getNumberOfColumns();
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
template<class T,class STO>
bool TMatrix<T,STO>::equal_to(const TMatrix<T,STO> &m2) const
  {
    if (!CompDim(*this,m2)) return false;
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        if ((*this)(i,j) != m2(i,j)) return false;
    return true;
  }

//! @brief Return a column matrix which components are those 
//! identified by the row indexes of the integer vector argument 
//! being passed as parameter.
//! @param matrix: Matrix to extract the values of.
//! @param row_indexes: row indexes of the components to be extracted.
//! @param icol: Index of the column of the elements to extract.
template <class MATRV,class MATRI>
MATRV ExtraeValores(const MATRV &matrix,const MATRI &row_indexes,const size_t &icol= 1)
  {
    MATRV retval(row_indexes.getNumberOfRows(),1);
    for(size_t iRow=1;iRow<=retval.getNumberOfRows();iRow++)
      retval(iRow,1)= matrix(row_indexes(iRow),icol);
    return retval;
  }

#endif

