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
//Macaulay.h
#ifndef MACAULAY_H
#define MACAULAY_H

#include "../polynomials/function.h"

typedef vectorZ<double> vZ_double;

class Macaulay : public Function
  {
  protected:
    int n;
    double a;
    double coef;
  public:
    Macaulay(void): Function()
      { n= 0; a= 0.0; coef= 1.0;}
    Macaulay(const NmbVars &vars, const int n= 0, const double a= 0.0): Function(vars)
      { this->n= n; this->a= a; coef= 1; }
    Macaulay(const char *vars, const int n= 0, const double a= 0.0): Function(vars)
      { this->n= n; this->a= a; coef= 1; }
    Macaulay(const Macaulay &otro): Function(otro)
      { n= otro.n; a= otro.a; coef= otro.coef; }
    Macaulay &operator=(const Macaulay &m)
      {
        Function::operator=(m);
        n= m.n; a= m.a; coef= m.coef;
        return *this;
      }
    int Grado(void) const
      { return n; }
    double GetA(void) const
      { return a; }
    double Eval(const double &x) const;
    double operator()(const double &x) const
    //Evalua el Macaulay en el punto x.
      { return Eval(x); }
    Macaulay &operator*=(const Macaulay &m);
    friend Macaulay operator *(const Macaulay &m1,const Macaulay &m2);
    friend bool operator ==(const Macaulay &p1,const Macaulay &p2);
    friend std::ostream &operator <<(std::ostream &stream,const Macaulay &p);
    Macaulay Primitiva(void) const;
    Macaulay Diferencial(void) const;
    double Integral(const double &a, const double &b) const;
    double Derivada(const double &x) const;
    friend double dot(const Macaulay &m1,const Macaulay &m2, const double &a, const double &b);
  };

#endif








