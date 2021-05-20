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
//OpComparacion.cc

#include "OpComparacion.h"
#include "Lexico.h"

const Operando &OpMenor::Opera(const Operando *v1,const Operando *v2) const
  {
    if(v1->GetValor()<v2->GetValor())
      return *Lex().NuevoLiteral(1.0);
    else
      return *Lex().NuevoLiteral(0.0);
  }

const Operando &OpLessOrEqual::Opera(const Operando *v1,const Operando *v2) const
  {
    if(v1->GetValor()<=v2->GetValor())
      return *Lex().NuevoLiteral(1.0);
    else
      return *Lex().NuevoLiteral(0.0);
  }

const Operando &OpEqual::Opera(const Operando *v1,const Operando *v2) const
  {
    if(v1->GetValor()==v2->GetValor())
      return *Lex().NuevoLiteral(1.0);
    else
      return *Lex().NuevoLiteral(0.0);
  }

const Operando &OpMayor::Opera(const Operando *v1,const Operando *v2) const
  {
    if(v1->GetValor()>v2->GetValor())
      return *Lex().NuevoLiteral(1.0);
    else
      return *Lex().NuevoLiteral(0.0);
  }

const Operando &OpGreaterOrEqual::Opera(const Operando *v1,const Operando *v2) const
  {
    if(v1->GetValor()>=v2->GetValor())
      return *Lex().NuevoLiteral(1.0);
    else
      return *Lex().NuevoLiteral(0.0);
  }

const Operando &OpDistinto::Opera(const Operando *v1,const Operando *v2) const
  {
    if(v1->GetValor()!=v2->GetValor())
      return *Lex().NuevoLiteral(1.0);
    else
      return *Lex().NuevoLiteral(0.0);
  }
