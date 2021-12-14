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
//LexAlgebra.cc


#include "LexAlgebra.h"
#include "VarExpr.h"

LexAlgebra::LexAlgebra(void)
  : Lexico(), expresiones() {}

VarExpr *LexAlgebra::CalcDirExpr(const std::string &nmb)
  { return expresiones.CalcDir(nmb); }

NamedOperand *LexAlgebra::NuevaVar(const Variable &v)
  {
    //Si existía como expresión advertimos del problema:
    TablaExpresiones::const_iterator i= expresiones.find(v.getName());
    if(i!=expresiones.end())
      std::cerr << "¡Ojo! la variable: '" << v.getName() 
           << "' ya existe como expresión." << std::endl;
    // Append it to the expression table.
    return vars.Nueva(v);
  }

NamedOperand *LexAlgebra::NuevaExpr(const VarExpr &v)
  {
    //Si existía como variable advertimos del problema:
    TablaVariables::const_iterator i= vars.find(v.getName());
    if(i!=vars.end())
      std::cerr << "¡Ojo! la expresión: '" << v.getName() 
           << "' ya existe como variable." << std::endl;
    // If truly new, append it to the expression table.
    TablaExpresiones::iterator j= expresiones.find(v.getName());
    NamedOperand *retval= NULL;
    if(j==expresiones.end())
      retval= expresiones.Nueva(v);
    else
      retval= &((*j).second);
    return retval;
  }

void LexAlgebra::AsignaExpr(const std::string &palabra,const ExprAlgebra &e)
  { expresiones[palabra].AsignaExpr(e); }


