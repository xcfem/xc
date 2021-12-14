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
//LexAlgebra.h

#ifndef LEXALGEBRA_H
#define LEXALGEBRA_H

#include "Lexico.h"
#include "TablaExpresiones.h"

class VarExpr;


//! @brief Léxico para expresiones algebraicas.
class LexAlgebra : public Lexico
  {
  private:
    friend class ExprAlgebra;
    TablaExpresiones expresiones;
    inline ExprAlgebra GetExpresion(const std::string &nmb)
      { return CalcDirExpr(nmb)->GetExpr(); }
  public:
    LexAlgebra(void);
    inline bool EsExpresion(const std::string &nmb)
      { return (expresiones.find(nmb)!=expresiones.end()); }
    virtual VarExpr *CalcDirExpr(const std::string &nmb);
    inline virtual Variable *CalcDirVar(const std::string &nmb)
      {
        Variable *retval= CalcDirExpr(nmb);
        if(!retval)
         retval= vars.CalcDir(nmb);
        return retval;
      }
    NamedOperand *NuevaVar(const Variable &v);
    inline NamedOperand *NuevaVar(const std::string &palabra)
      { return NuevaVar(Variable(palabra)); }
    NamedOperand *NuevaExpr(const VarExpr &v);
    inline NamedOperand *NuevaExpr(const std::string &palabra)
      { return NuevaExpr(VarExpr(palabra)); }
    void AsignaExpr(const std::string &palabra,const ExprAlgebra &e);
 };
#endif
