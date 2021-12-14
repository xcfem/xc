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
//ExprAlgebra.h

#ifndef EXPRALGEBRA_H
#define EXPRALGEBRA_H


#include "ArbolExpr.h"

class Intervalo1D;

//! @brief Expresión algebraica.
class ExprAlgebra : public ArbolExpr
  {
    friend class ExpressionMatrix;
    inline void eval(void)
      {
        expand();
        Opera();
      }
    inline void eval(const std::string &palabra,const ExprAlgebra &a)
      {
        Asigna(palabra,a);
        Opera();
      }
    void eval(const MapValores &);
    inline void eval(const std::string &palabra,const double &d)
      {
        Asigna(palabra,d);
        Opera();
      }
    void expand(void);

  public:
    ExprAlgebra(void);
    explicit ExprAlgebra(const std::string &);
    ExprAlgebra(const ExprAlgebra &);
    explicit ExprAlgebra(const double &);
    ExprAlgebra &operator=(const ExprAlgebra &other)
      {
        ArbolExpr::operator=(other);
        return *this;
      }
    ExprAlgebra &operator+=(const ExprAlgebra &a2)
      {
        ArbolExpr::operator+=(a2);
        return *this;
      }
    ExprAlgebra &operator-=(const ExprAlgebra &a2)
      {
        ArbolExpr::operator-=(a2);
        return *this;
      }
    ExprAlgebra &operator*=(const ExprAlgebra &a2)
      {
        ArbolExpr::operator*=(a2);
        return *this;
      }
    ExprAlgebra &operator/=(const ExprAlgebra &a2)
      {
        ArbolExpr::operator/=(a2);
        return *this;
      }
    inline ExprAlgebra operator-(void)
      {
        ExprAlgebra retval(*this);
        retval.Neg();
        return retval;
      }
    inline ExprAlgebra Eval(void) const
      {
        ExprAlgebra retval(*this);
        retval.eval();
        return retval;
      }
    inline ExprAlgebra Eval(const std::string &palabra,const ExprAlgebra &a) const
      {
        ExprAlgebra retval(*this);
        retval.eval(palabra,a);
        return retval;
      }
    inline ExprAlgebra Eval(const std::string &palabra,const double &d) const
      {
        ExprAlgebra retval(*this);
        retval.eval(palabra,d);
        return retval;
      }
    ExprAlgebra Eval(const std::string &valores) const;
    inline ExprAlgebra Diferencia(const std::string &var) const
      {
        ExprAlgebra retval(*this);
        retval.Dif(var);
        return retval;
      }
    inline ExprAlgebra Colct(void) const
      {
        ExprAlgebra retval(*this);
        retval.Simplifica();
        return retval;
      }
    double RaizNewton(const double &tol,size_t max_iter,const std::string &var,const double &d,double &err);
    inline ExprAlgebra operator()(const std::string &p,const ExprAlgebra &a) const
      { return Eval(p,a); }
    inline ExprAlgebra operator()(const std::string &p,const double &d) const
      { return Eval(p,d); }
    ExprAlgebra IntegTrapecio(const Intervalo1D &i) const;
    ExprAlgebra IntegSimpson(const Intervalo1D &i) const;
    friend ExprAlgebra operator+(const ExprAlgebra &a1,const ExprAlgebra &a2)
      {
        ExprAlgebra retval(a1);
        return retval+=a2;
      }
    friend ExprAlgebra operator+(const ExprAlgebra &a1,const double &a2)
      {
        ExprAlgebra retval(a1);
        return retval+= ExprAlgebra(a2);
      }
    friend ExprAlgebra operator+(const double &a1,const ExprAlgebra &a2)
      {
        ExprAlgebra retval(a1);
        return retval+=a2;
      }
    friend ExprAlgebra operator-(const ExprAlgebra &a1,const ExprAlgebra &a2)
      {
        ExprAlgebra retval(a1);
        return retval-=a2;
      }
    friend ExprAlgebra operator-(const ExprAlgebra &a1,const double &a2)
      {
        ExprAlgebra retval(a1);
        return retval-= ExprAlgebra(a2);
      }
    friend ExprAlgebra operator-(const double &a1,const ExprAlgebra &a2)
      {
        ExprAlgebra retval(a1);
        return retval-=a2;
      }
    friend ExprAlgebra operator*(const ExprAlgebra &a1,const ExprAlgebra &a2)
      {
        ExprAlgebra retval(a1);
        return retval*=a2;
      }
    friend ExprAlgebra operator*(const ExprAlgebra &a1,const double &a2)
      {
        ExprAlgebra retval(a1);
        return retval*= ExprAlgebra(a2);
      }
    friend ExprAlgebra operator*(const double &a1,const ExprAlgebra &a2)
      {
        ExprAlgebra retval(a1);
        return retval*=a2;
      }
    friend ExprAlgebra operator/(const ExprAlgebra &a1,const ExprAlgebra &a2)
      {
        ExprAlgebra retval(a1);
        return retval/=a2;
      }
    friend ExprAlgebra operator/(const ExprAlgebra &a1,const double &a2)
      {
        ExprAlgebra retval(a1);
        return retval/= ExprAlgebra(a2);
      }
    friend ExprAlgebra operator/(const double &a1,const ExprAlgebra &a2)
      {
        ExprAlgebra retval(a1);
        return retval/=a2;
      }
    friend ExprAlgebra expand(const ExprAlgebra &);
    friend ExprAlgebra abs(const ExprAlgebra &);
    friend ExprAlgebra sqrt(const ExprAlgebra &);
    friend ExprAlgebra pow(const ExprAlgebra &a,const ExprAlgebra &b);
    friend ExprAlgebra pow(const ExprAlgebra &a,const double &b);
    friend ExprAlgebra pow(const double &a,const ExprAlgebra &b);
  };

ExprAlgebra expand(const ExprAlgebra &);
ExprAlgebra colct(const ExprAlgebra &);

#endif






