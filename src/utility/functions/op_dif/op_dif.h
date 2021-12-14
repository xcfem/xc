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
//op_dif.h
//Operadores diferenciales.
#ifndef OP_DIF_H
#define OP_DIF_H

#include "utility/matrices/m_double.h"
#include "utility/utils/misc_utils/matem.h"
#include "utility/utils/misc_utils/mchne_eps.h"

inline double calcula_incremento(const double &t)
  {
    double retval(sqrt_mchne_eps_dbl); // sqrt_mchne_eps_dbl: Valor de la raiz cuadrada de
                                       //la precisión de la máquina.
    if(t != 0.0) retval*=fabs(t);
    return retval;
  }

//! @brief Return the partial derivative of the function argument.
//!
//! Return the partial derivative of the function argument
//! evaluated at the point x with respecto to the i component
//! using the "forward difference" method.
//!
//! @param x: point to evaluate the function.
//! @param f: function to differentiate.
//! @param i: index of the component to differentiate with respect to.
template<class F>
inline double parcial_fd(const m_double &x,const F &f,const size_t &i)
  {
    m_double x2(x);
    const double temp= x2(i);
    const double h= calcula_incremento(temp);
    x2(i)+=h;
    return (f(x2)-f(x))/h;
  }

//! @brief Return the partial derivative of the function argument.
//!
//! Return the partial derivative of the function argument
//! evaluated at the point x with respecto to the i component
//! using the "extended central difference" method.
//!
//! @param x: point to evaluate the function.
//! @param f: function to differentiate.
//! @param i: index of the component to differentiate with respect to.
template<class F>
inline double parcial_cdf(const m_double &x,const F &f,const size_t &i)
  {
    m_double x_2(x); //Punto a -2*h de x
    m_double x_1(x); //Punto a -h de x
    m_double x1(x);  //Punto a +x de x
    m_double x2(x);  //Punto a +2h de x
    const double temp= x(i);
    const double h= calcula_incremento(temp);
    const double h2= 2.0*h;
    x_2(i)-=h2;
    x_1(i)-=h;
    x1(i)+=h;
    x2(i)+=h2;
    return (f(x_2)-8.0*f(x_1)+8.0*f(x1)-f(x2))/(12.0*h);
  }

//! @brief Return the gradient vector of the function argument
//! using the "extended central difference" method.
//!
//! @param x: point to evaluate the function.
//! @param f: function to compute the gradient.
template<class F>
inline m_double gradiente_cdf(const m_double &x,const F &f)
  {
    m_double g(x);
    size_t n_rows= x.getNumberOfRows();
    for(size_t i= 1;i<=n_rows;i++)
      g(i)= parcial_cdf(x,f,i);
    return g;
  }

//! @brief Return the gradient vector of the function argument
//! as row vector using the "extended central difference" method.
//!
//! @param x: point to evaluate the function.
//! @param f: function to compute the gradient.
template<class F>
inline m_double extended_central_differece_row_gradient(const m_double &x,const F &f)
  {
    m_double g(1,x.getNumberOfRows());
    size_t n_rows= x.getNumberOfRows();
    for(size_t i= 1;i<=n_rows;i++)
      g(1,i)= parcial_cdf(x,f,i);
    return g;
  }

//! @brief Return the jacobian vector de f at the point x
//! using the "extended central difference" method
template<class MF>
inline m_double jacobiano_cdf(const m_double &x,const MF &mf)
  {
    size_t n_rows= mf.getNumberOfRows();
    size_t n_columns= x.getNumberOfRows();
    m_double J(n_rows,n_columns);
    for(size_t i= 1;i<=n_rows;i++)
      {
        m_double g= extended_central_differece_row_gradient(x,mf(i));
        J.putRow(i,g);
      }
    return J;
  }

#endif
