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
//OpBooleano.cc
#include "OpBooleano.h"
#include "Lexico.h"

const Operando &OpAnd::Opera(const Operando *v1,const Operando *v2) const
  {
    if((v1->GetValor()==1.0) && (v2->GetValor()==1.0))
      return *Lex().NuevoLiteral(1.0);
    else
      return *Lex().NuevoLiteral(0.0);
  }

const Operando &OpOr::Opera(const Operando *v1,const Operando *v2) const
  {
    if((v1->GetValor()==1.0) || (v2->GetValor()==1.0))
      return *Lex().NuevoLiteral(1.0);
    else
      return *Lex().NuevoLiteral(0.0);
  }

const Operando &OpNot::Opera(const Operando *,const Operando *v2) const
  {
    if(v2->GetValor()==1.0)
      return *Lex().NuevoLiteral(0.0);
    else
      return *Lex().NuevoLiteral(1.0);
  }
