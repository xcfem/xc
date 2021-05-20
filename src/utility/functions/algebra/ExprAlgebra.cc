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
//ExprAlgeblra.cc
#include "ExprAlgebra.h"
#include "TablaVariables.h"
#include "LexAlgebra.h"
#include "ConjVariables.h"
#include "num_integration.h"
#include <cmath>
#include "MapValores.h"

//! @brief Constructor.
ExprAlgebra::ExprAlgebra(void)
  : ArbolExpr("") {}
//! @brief Constructor.
ExprAlgebra::ExprAlgebra(const std::string &str)
  : ArbolExpr(str) {}
//! @brief Constructor.
ExprAlgebra::ExprAlgebra(const ExprAlgebra &other)
  : ArbolExpr(other) {}
//! @brief Constructor.
ExprAlgebra::ExprAlgebra(const double &d)
  : ArbolExpr(d) {}

void ExprAlgebra::expand(void)
  {
    ConjVariables cv= Variables();
    for(ConjVariables::const_iterator i=cv.begin();i!=cv.end();i++)
      {
        const std::string &nmb_var= (*i)->getName();
        if(LexA().EsExpresion(nmb_var))
          {
            ExprAlgebra e= LexA().GetExpresion(nmb_var);
            eval(nmb_var.c_str(),e);
          }
      }
    Distrib();
  }

//! @brief Evalúa la expresión a partir de los valores que se pasan como parámetro.
void ExprAlgebra::eval(const MapValores &mv)
  {
    for(MapValores::const_iterator i= mv.begin();i!=mv.end();i++)
      Asigna((*i).first,(*i).second);
    Opera();
  }

ExprAlgebra ExprAlgebra::Eval(const std::string &valores) const
  {
    ExprAlgebra retval(*this);
    retval.eval(MapValores(valores));
    return retval;
  }

//! @brief Devuelve la raiz de la expresion aproximandola por Newton.
double ExprAlgebra::RaizNewton(const double &tol,size_t max_iter,const std::string &var,const double &d,double &err)
  {
    if(this->NumVariables() > 1)
      {
        std::cerr << "Functions: " << *this << " has more than one variable." << std::endl;
        return NAN;
      }
    const ExprAlgebra dif= (*this)/(this->Diferencia(var)).Colct();
    double D,x= d;
    size_t iter= 0;
    do
      {
        D= dif(var,x).ToNum();
        x-=D;
        iter++;
      }
    while((iter<max_iter) && (fabs(D)>tol));
    err= fabs(D);
    return x;
  }

//! @brief Devuelve el valor absoluto del argumento.
ExprAlgebra abs(const ExprAlgebra &other)
  {
    ExprAlgebra retval(other);
    retval.Abs();
    return retval;
  }

//! @brief Devuelve la raía cuadrada del argumento.
ExprAlgebra sqrt(const ExprAlgebra &other)
  {
    ExprAlgebra retval(other);
    retval.Sqrt();
    return retval;
  }

//! @brief Devuelve a^b.
ExprAlgebra pow(const ExprAlgebra &a,const ExprAlgebra &b)
  {
    ExprAlgebra retval(a);
    retval.pow(b);
    return retval;
  }
//! @brief Devuelve a^b.
ExprAlgebra pow(const ExprAlgebra &a,const double &b)
  {
    ExprAlgebra retval(a);
    retval.pow(ExprAlgebra(b));
    return retval;
  }
//! @brief Devuelve a^b.
ExprAlgebra pow(const double &a,const ExprAlgebra &b)
  {
    ExprAlgebra retval(a);
    retval.pow(b);
    return retval;
  }

ExprAlgebra expand(const ExprAlgebra &a)
  {
    ExprAlgebra retval(a);
    retval.expand();
    return retval;
  }

ExprAlgebra colct(const ExprAlgebra &a)
  {
    ExprAlgebra retval(a);
    retval.Colct();
    return retval;
  }

//! @brief Integrates the expresion using the trapezoidal rule.
//! 
//! @param i: Integration interval.
ExprAlgebra ExprAlgebra::IntegTrapecio(const Intervalo1D &i) const
  { return integ_trapecio(*this,i); }

//! @brief Integrates the expresion using the Simpson rule.
//! 
//! @param i: Integration interval.
ExprAlgebra ExprAlgebra::IntegSimpson(const Intervalo1D &i) const
  { return integ_simpson(*this,i); }


