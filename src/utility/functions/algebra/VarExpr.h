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
//VarExpr.h

#ifndef VAREXPR_H
#define VAREXPR_H

#include "ExprAlgebra.h"

class VarExpr : public Variable
  {
    ExprAlgebra expr;
  public:
    VarExpr(void) : Variable() {}
    VarExpr(const std::string &nmb): Variable(nmb) {}
    VarExpr(const VarExpr &other): Variable(other) {}
    VarExpr &operator=(const VarExpr &other)
      {
        Variable::operator =(other);
        expr= other.expr;
        return *this;
      }
    inline ExprAlgebra GetExpr(void) const
      { return expr; }
    inline virtual void Asigna(const double &d)
      {
        Variable::Asigna(d);
        expr= ExprAlgebra(d);
      }
    inline virtual void AsignaExpr(const ExprAlgebra &ex)
      {
        Variable::Asigna(0.0);
        expr= ex;
      }
    virtual bool Evaluable(void) const
      { return ((Variable::Evaluable()) && (expr.Evaluable())); }
    inline virtual const Operando &Opera(const Operando *, const Operando *) const
      {
        if(Evaluable())
          return *Lex().NuevoLiteral(expr.ToNum());
        else
          return *this;
      }
    inline virtual const double &GetValor(const double &v1=0.0, const double &v2=0.0) const
      { return expr.ToNum(); }
  };

#endif
