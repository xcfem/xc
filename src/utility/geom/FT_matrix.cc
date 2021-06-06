//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//FT_matrix.cc

#include "utility/utils/misc_utils/matem.h"
#include "FT_matrix.h"
#include "utility/matrices/m_double.h"
#include "cgal_types.h"



// GEOM_FT neutro_suma(const GEOM_FT &)
//   { return double_to_FT(0.0); }
// GEOM_FT neutro_producto(const GEOM_FT &)
//   { return double_to_FT(1.0); }
GEOM_FT Abs(const FT_matrix &m)
  { return m.Abs(); }

//! @brief Constructor
FT_matrix::FT_matrix(void)
  : ZMatrix<GEOM_FT>(1,1) {}

//! @brief Constructor
FT_matrix::FT_matrix(size_type rows,size_type cols)
  : ZMatrix<GEOM_FT>(rows,cols) {}

//! @brief Constructor
FT_matrix::FT_matrix(size_type rows,size_type cols,GEOM_FT val)
  : ZMatrix<GEOM_FT>(rows,cols,val) {}

//! @brief Constructor
FT_matrix::FT_matrix(const FT_matrix &orig,size_t f1, size_t c1, size_t f2, size_t c2)
  : ZMatrix<GEOM_FT>(f2-f1+1,c2-c1+1)
  {
    orig.check_get_box(f1,c1,f2,c2);
    for(size_t i=1;i<=n_rows;i++)
      for(size_t j=1;j<=n_columns;j++)
        (*this)(i,j)= orig(i+f1-1,j+c1-1);
  }

FT_matrix FT_matrix::getBox(size_t f1, size_t c1, size_t f2, size_t c2) const
  { return FT_matrix(*this,f1,c1,f2,c2); }
FT_matrix FT_matrix::getRow(size_t iRow) const
  { return getBox(iRow,1,iRow,n_columns); }
FT_matrix FT_matrix::getColumn(size_t col) const
  { return getBox(1,col,n_rows,col); }

FT_matrix FT_matrix::GetTrn(void) const
  { return traspuesta(*this); }

//! @brief Return the matrix recíproca de la being passed as parameter.
FT_matrix operator-(const FT_matrix &m)
  {
    FT_matrix neg(m);
    neg.Neg();
    return neg;
  }

FT_matrix operator+(const FT_matrix &a,const FT_matrix &b)
  {
    FT_matrix retval(a);
    retval+=b;
    return retval;
  }

FT_matrix operator-(const FT_matrix &a,const FT_matrix &b)
  {
    FT_matrix retval(a);
    retval-=b;
    return retval;
  }

//!  @brief Producto de matrices.
FT_matrix operator*(const FT_matrix &a,const FT_matrix &b)
  {
    FT_matrix retval(a);
    retval*=b;
    return retval;
  }

//!  @brief Producto por un escalar.
FT_matrix operator*(const GEOM_FT &d,const FT_matrix &a)
  {
    FT_matrix retval(a);
    retval*=d;
    return retval;
  }

//!  @brief Producto por un escalar.
FT_matrix operator*(const FT_matrix &a,const GEOM_FT &d)
  { return d*a; }

//!  @brief Producto vectorial. ¡Ojo! está escrito para vectores de dimensión 3 xxx.
FT_matrix operator^(const FT_matrix &v1,const FT_matrix &v2)
  { return prod_vectorial(v1,v2); }


//! @brief Return an identity matrix with f rows and f columns.
FT_matrix identity(const size_t &f)
  {
    FT_matrix retval(f,f);
    retval.Idn();
    return retval;
  }

FT_matrix identity(const FT_matrix &m)
  {
    FT_matrix retval(m);
    retval.Idn();
    return retval;
  }

//! @brief Return the transposed of the argument.
FT_matrix traspuesta(const FT_matrix &m)
  { 
    FT_matrix retval(m);
    retval.Trn();
    return retval;
  }

//! @brief Producto tensorial de dos tensores de primer orden.
//! -u: row vector.
//! -v: column vector.
FT_matrix prod_tensor(const FT_matrix &u,const FT_matrix &v)
  {
    const size_t sz_u= u.size();
    const size_t sz_v= v.size();
    FT_matrix retval(sz_u,sz_v);
    for(size_t i=1;i<=sz_u;i++)
      for(size_t j=1;j<=sz_v;j++)
        retval(i,j)= u[i-1]*v[j-1];
    return retval;
  }

//! @brief Divide the components of the matrix by its modulus.
void normalize(FT_matrix &m)
  {
    const GEOM_FT vabs= ::Abs(m);
    if(vabs>0) m*= 1/vabs;
  }

//! @brief Divides the components of each row of the matrix by the norm
//! of such row.
void NormalizeRows(FT_matrix &m)
  {
    const size_t n_rows= m.getNumberOfRows(),n_columns= m.getNumberOfColumns();
    for(size_t i=1;i<=n_rows;i++)
      {
        const GEOM_FT row_norm= ::Abs(m.getRow(i));
        if(row_norm>0)
          for(size_t j=1;j<=n_columns;j++)
	    m(i,j)/= row_norm;
        else
	  std::cerr << "NormalizeRows; the row: " << i
		    << " has zero norm." << std::endl;
      }
  }

//! @brief Return the normalized matrix.
FT_matrix normalize(const FT_matrix &m)
  {
    FT_matrix retval(m);
    normalize(retval);
    return retval;
  }

//! @brief Return the row-normalized matrix de la being passed as parameter.
FT_matrix NormalizeRows(const FT_matrix &m)
  {
    FT_matrix retval(m);
    NormalizeRows(retval);
    return retval;
  }

ZMatrix<double> to_double(const FT_matrix &m)
  {
    const size_t n_rows= m.getNumberOfRows(),n_columns= m.getNumberOfColumns();
    m_double retval(n_rows,n_columns);
    for(size_t i= 1;i<=n_rows;i++)
      for(size_t j= 1;j<=n_columns;j++)
	retval(i,j)= m(i,j);
    return retval;
  }

FT_matrix from_double(const ZMatrix<double> &m)
  {
    const size_t n_rows= m.getNumberOfRows(),n_columns= m.getNumberOfColumns();
    FT_matrix retval(n_rows,n_columns);
    for(size_t i= 1;i<=n_rows;i++)
      for(size_t j= 1;j<=n_columns;j++)
	retval(i,j)= double_to_FT(m(i,j));
    return retval;
  }


