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
#include "Variable.h"
#include "Literal.h"
#include "Lexico.h"
#include "Rama.h"
#include "ConjVariables.h"

Rama Variable::Diferencia(const Variable &v,const Rama &r) const
  {
    std::cerr << "Variable::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     if(getName() == v.getName())
//       r->Put(Lex().NuevoLiteral(1.0),NULL,NULL);
//     else
//       r->Put(Lex().NuevoLiteral(0.0),NULL,NULL);
//    return r;
  }
void Variable::GetVariables(ConjVariables &cv) const
  { cv.insert(this); }

const Operando &Variable::Opera(const Operando *, const Operando *) const
  {
    if(Evaluable())
      return *Lex().NuevoLiteral(GetValor());
    else
      return *this;
  }
