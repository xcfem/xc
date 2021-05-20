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
//Operando.h

#ifndef OPERANDO_H
#define OPERANDO_H

#include "Segnal.h"
#include "ProtoExpr.h"
#include "string"

//! @brief Posición de un operador
//!
//! La posición de un operador puede ser:
//! - prefija: Cuando se escribe antes que los operandos: sin(a), -1.0,not(b),...
//! - infija: Cuando se escribe entre los operandos: +,-,*,/...
//! - sufija: Cuando se escribe detrás de los operandos: factorial.
enum Posicion{prefija,infija,sufija};

class Rama;
class ConjVariables;

//! @brief Operando.
class Operando: public Segnal, public ProtoExpresion
  {
  protected:
    double valor;
  public:
    inline explicit Operando(const double &d= 0.0)
      : valor(d) {}
    void Put(const double &v)
      { valor= v; }
    virtual const std::string &getName(void) const;
    virtual const std::string &GetFullString(void) const;
    inline virtual Clase GetClase(void) const
      { return operando; }
    inline virtual Posicion GetPosicion(void) const
      { return infija; }
    virtual bool Evaluable(void) const
      { return false; }
    inline virtual const Operando &Opera(const Operando *, const Operando *) const
      { return *this; }
    virtual Rama Diferencia(const Variable &,const Rama &) const;
    virtual Rama *Simplifica(Rama *) const;
    virtual Rama *Distrib(Rama *) const;
    inline virtual void GetVariables(ConjVariables &) const {}
    inline virtual const double &GetValor(const double &v1=0.0, const double &v2=0.0) const
      { return valor; }
  };

#endif
