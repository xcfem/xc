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
//Segnal.h

#ifndef SEGNAL_H
#define SEGNAL_H

#include <string>

//! @brief Clase de una señal
//!
//! La clase de una expresión puede ser:
//! -fin_exp: Señala el final de la expresión.
//! -parent_izdo: Paréntesis izquierdo.
//! -parent_dcho: Paréntesis derecho.
//! -operando: Objeto sobre el que actúa un operador.
//! -op_unitario: Operador unitario, que opera sobre un sólo operando (factorial, trigonometric functions, negación,...).
//! -op_binario: Operador binario, que opera sobre dos operandos (suma, resta, multiplicación,...).
//!
enum Clase { fin_exp,
             parent_izdo, parent_dcho,
             operando,
             op_unitario, op_binario};

class Variable;
class Lexico;


//! @brief Componente elemental de una expresión.
//!
//! Componente elemental de una expresión.
//! La prioridad de un operador es:
//!
//! 6: Todos los operadores aritméticos unitarios, elevado a (^).
//! 5: Producto, división, mod, div.
//! 4: Suma, resta,
//! 3: Operadores de comparación (equality, inequality, menor que, mayor que,...).
//! 3: No (booleano).
//! 2: y,o (booleanos).
//! 1: asignación.
//! -1: Paréntesis.
class Segnal
  {
  public:
    inline Segnal(void) {}
    inline virtual ~Segnal(void) {}
    virtual const std::string &getName(void) const= 0;
    virtual Clase GetClase(void) const= 0;
    virtual const double &GetValor(const double &v1= 0.0, const double &v2= 0.0) const= 0;
    inline virtual int GetPrioridad(void) const
      { return -1; }
    const std::string &StrClase(void) const;
    virtual std::ostream &Imprime(std::ostream &os) const;
    inline virtual bool EqualTo(const double &) const
      { return false; }
    friend int operator<(const Segnal &s1,const Segnal &s2)
      { return (s1.getName() < s2.getName()); }
    friend int operator==(const Segnal &s,const double &d)
      { return s.EqualTo(d); }
    friend int operator==(const double &d,const Segnal &s)
      { return (s == d); }
  };

std::ostream &operator<<(std::ostream &, const Segnal &);


#endif


