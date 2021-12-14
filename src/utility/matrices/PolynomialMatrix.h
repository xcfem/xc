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
//PolynomialMatrix.h
//Matrix of polynomials

#ifndef POLYNOMIALMATRIX_H
#define POLYNOMIALMATRIX_H

#include "ZMatrix.h"
#include "utility/functions/polynomials/Polynomial.h"
#include "m_double.h"

typedef ZMatrix<Polynomial> polynomial_matrix;

class PolynomialMatrix: public polynomial_matrix
  {
    PolynomialMatrix(const m_double &m);

    void eval(short unsigned int j,const double &val);
    void eval(short unsigned int j,const Polynomial &val);
    void eval(const vZ_double &v);
    void eval(const mZ_double &v);
  public:
    PolynomialMatrix(size_type n_rows=1,size_type cols=1,const Polynomial &val=Polynomial())
      : polynomial_matrix(n_rows,cols,val) {}
    PolynomialMatrix(const PolynomialMatrix &other) : polynomial_matrix(other) {}
    PolynomialMatrix &operator=(const PolynomialMatrix &m)
      {
	polynomial_matrix::operator=(m);
        return *this;
      }
    PolynomialMatrix &Trn(void);
    PolynomialMatrix GetTrn(void) const;

    PolynomialMatrix NeutroSuma(void) const
      { return PolynomialMatrix(getNumberOfRows(),getNumberOfColumns(),Polynomial::neutro_suma()); }
    PolynomialMatrix NeutroProducto(void) const
      { return PolynomialMatrix(getNumberOfRows(),getNumberOfColumns(),Polynomial::neutro_producto()); }

    PolynomialMatrix Eval(short unsigned int j,const double &val) const;
    PolynomialMatrix Eval(short unsigned int j,const Polynomial &val) const;
    PolynomialMatrix Eval(const vZ_double &v) const;
    PolynomialMatrix Eval(const mZ_double &v) const;

    PolynomialMatrix &operator*=(const double &d);

    PolynomialMatrix &operator+=(const PolynomialMatrix &mp)
      {
	polynomial_matrix::operator+=(mp);
        return *this;
      }
    PolynomialMatrix &operator-=(const PolynomialMatrix &mp)
      {
	polynomial_matrix::operator-=(mp);
        return *this;
      }
    PolynomialMatrix &operator+=(const m_double &m);
    PolynomialMatrix &operator-=(const m_double &m);

    PolynomialMatrix &Primitiva(short unsigned int k);
    PolynomialMatrix &Diferencial(short unsigned int k);

    friend PolynomialMatrix operator*(const PolynomialMatrix &m1,const PolynomialMatrix &m2);
    friend PolynomialMatrix operator*(const m_double &m,const PolynomialMatrix &mE)
      {
        PolynomialMatrix retval= mE*PolynomialMatrix(m);
        return retval;
      }
    friend PolynomialMatrix operator*(const PolynomialMatrix &mE, const m_double &m)
      { return m*mE; }
    friend PolynomialMatrix operator-(const m_double &m1,const PolynomialMatrix &m2);
  };

inline PolynomialMatrix neutro_suma(const PolynomialMatrix &mp)
  { return mp.NeutroSuma(); }
inline PolynomialMatrix neutro_producto(const PolynomialMatrix &mp)
  { return mp.NeutroProducto(); }


//! @brief Evaluate the polynomials of the matrix.
inline PolynomialMatrix Eval(const PolynomialMatrix &m,short unsigned int k,const double &d)
  { return m.Eval(k,d); }

//! @brief Evaluate the polynomials of the matrix with a polynomial argument.
inline PolynomialMatrix Eval(const PolynomialMatrix &m,short unsigned int k,const Polynomial &p)
  { return m.Eval(k,p); }

Polynomial Eval(const Polynomial &p,const PolynomialMatrix &vp);
m_double Eval(const PolynomialMatrix &p,const m_double &x);
PolynomialMatrix Eval(const PolynomialMatrix &m1,const PolynomialMatrix &m2);

inline void CambioVar(PolynomialMatrix &m,short unsigned int j,const Polynomial &cambio)
  { m= Eval(m,j,cambio); }
inline void CambioVar(PolynomialMatrix &m,const PolynomialMatrix &cambio)
  { m= Eval(m,cambio); }

PolynomialMatrix operator+(const PolynomialMatrix &m1,const PolynomialMatrix &m2);
PolynomialMatrix operator-(const PolynomialMatrix &m1,const PolynomialMatrix &m2);
PolynomialMatrix operator+(const PolynomialMatrix &m1,const m_double &m2);
inline PolynomialMatrix operator +(const m_double &m1,const PolynomialMatrix &m2)
  { return m2+m1; }
PolynomialMatrix operator-(const PolynomialMatrix &m1,const m_double &m2);

Polynomial dot(const PolynomialMatrix &v1,const m_double &v2);
Polynomial dot(const m_double &v1,const PolynomialMatrix &v2);

PolynomialMatrix operator *(const Polynomial &p,const m_double &m);
inline PolynomialMatrix operator*(const m_double &m,const Polynomial &p)
  { return p*m; }
PolynomialMatrix operator*(const PolynomialMatrix &m,const double &d);
inline PolynomialMatrix operator *(const double &d,const PolynomialMatrix &m)
  { return m*d; }
inline PolynomialMatrix operator /(const PolynomialMatrix &m,const double &d)
  { return m*(1/d); }

PolynomialMatrix Primitiva(const PolynomialMatrix &m,short unsigned int k);
PolynomialMatrix Diferencial(const PolynomialMatrix &m, short unsigned int k);

PolynomialMatrix Integral(const PolynomialMatrix &m,short unsigned int j,const double &x0,const double &x1);
PolynomialMatrix Integral(const PolynomialMatrix &m,short unsigned int j,const Polynomial &x0,const Polynomial &x1);
PolynomialMatrix Integral(const PolynomialMatrix &m,short unsigned int j,const Polynomial &x0,const double &x1);
PolynomialMatrix Integral(const PolynomialMatrix &m,short unsigned int j,const double &x0,const Polynomial &x1);
PolynomialMatrix Derivada(const PolynomialMatrix &m,short unsigned int j,const double &x);
PolynomialMatrix Derivada(const PolynomialMatrix &m,short unsigned int j,const Polynomial &x);
 
#endif
