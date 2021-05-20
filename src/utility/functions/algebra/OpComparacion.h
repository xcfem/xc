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
//OpComparacion.h

#ifndef OPCOMPARACION_H
#define OPCOMPARACION_H

#include "OpBinario.h"
#include "operator_names.h"

//! @brief Clase base para operadores de comparación.
class OpComparacion: public OpBinario
  {
  public:
    OpComparacion(void) : OpBinario() {}
    inline virtual int GetPrioridad(void) const
      { return 3; }
  };

//! @brief Operador menor que.
//! Devuelve 1 si el valor izquierdo es menor que el derecho
//! y 0 en caso contrario.
class OpMenor : public OpComparacion
  {
    
  public:
    OpMenor(void) : OpComparacion() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpMenor; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

//! @brief Operador less or equal than.
//! Return 1 if the left value is smaller than the right one,
//!  0 otherwise.
class OpLessOrEqual : public OpComparacion
  {
    
  public:
    OpLessOrEqual(void) : OpComparacion() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpLessOrEqual; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

//! @brief Equal operator
//! Return 1 if the left value is equal to the right one,
//!  0 otherwise.
class OpEqual : public OpComparacion
  {
    
  public:
    OpEqual(void) : OpComparacion() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpEqual; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

//! @brief Operador mayor que.
//! Devuelve 1 si el valor izquierdo es mayor que el derecho
//! y 0 en caso contrario.
class OpMayor : public OpComparacion
  {
    
  public:
    OpMayor(void) : OpComparacion() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpMayor; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

//! @brief Operador greater thar or equal to.
//! Return 1 if the left value is greater than or equal to the right one,
//!  0 otherwise.
class OpGreaterOrEqual : public OpComparacion
  {
    
  public:
    OpGreaterOrEqual(void) : OpComparacion() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpGreaterOrEqual; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

//! @brief Operador distinto.
//! Devuelve 1 si el valor izquierdo es distinto del derecho
//! y 0 en caso contrario.
class OpDistinto : public OpComparacion
  {
    
  public:
    OpDistinto(void) : OpComparacion() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpDistinto; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

#endif
