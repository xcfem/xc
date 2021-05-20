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
//OpBooleano.h

#ifndef OPBOOLEANO_H
#define OPBOOLEANO_H

#include "OpBinario.h"
#include "OpUnario.h"

//! @brief Clase base para operadores de comparación.
class OpBooleano: public OpBinario
  {
  public:
    OpBooleano(void) : OpBinario() {}
    inline virtual int GetPrioridad(void) const
      { return 3; }
  };

//! @brief Operador AND.
//! Devuelve 1 si son verdaderas ambas condiciones y 0 en caso contrario.
class OpAnd : public OpBooleano
  {
    
  public:
    OpAnd(void) : OpBooleano() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpAnd; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

//! @brief Operador OR.
//! Devuelve 1 si alguna de las condiciones es verdadera y 0 en caso contrario.
class OpOr : public OpBooleano
  {
    
  public:
    OpOr(void) : OpBooleano() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpOr; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

class OpNot : public OpUnario
  {
    
  public:
    OpNot(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpNot; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
  };

#endif
