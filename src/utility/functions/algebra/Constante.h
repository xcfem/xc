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
//Constante.h

#ifndef CONSTANTE_H
#define CONSTANTE_H

#include "Literal.h"
#include "NamedOperand.h"

class Constante : public virtual NamedOperand, public virtual Literal
  {
  public:
    Constante(void) : NamedOperand() {}
    Constante(const std::string &nmb,const double &d): NamedOperand(nmb,d) {}
    Constante(const Constante &otro): NamedOperand(otro) {}
    Constante &operator=(const Constante &otro)
      {
        NamedOperand::operator =(otro);
        return *this;
      }
    virtual bool Evaluable(void) const
      { return Literal::Evaluable(); }
    friend int operator <(const Constante &c1,const Constante &c2)
      { return ((const Literal &)c1 < (const Literal &)c2); }
    virtual Rama Diferencia(const Variable &v,const Rama &) const;
  };

#endif
