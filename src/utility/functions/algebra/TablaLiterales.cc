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
//TablaLiterales.cc

#include "TablaLiterales.h"
#include "Rama.h"
#include <iostream>
#include <cassert>


//! @brief Constructor.
TablaLiterales::Literal::Literal(const double &d)
  : Operando(d) {}

// TablaLiterales::Literal &TablaLiterales::Literal::operator=(const Literal &otro)
//   {
//     std::cerr << "Literal; no se debe llamar al operador asignación." << std::endl;
//     Operando::operator=(otro);
//     return *this;
//   }

Rama TablaLiterales::Literal::Diferencia(const Variable &,const Rama &r) const
  { return Rama(0.0); }

std::ostream &TablaLiterales::Literal::Imprime(std::ostream &os) const
  {
    os << valor;
    return os;
  }

//! @brief Constructor.
TablaLiterales::TablaLiterales(void)
  {}

//! @brief Nueva referencia a un literal.
void TablaLiterales::NuevaRef(const Literal &l)
  {
    iterator i= literales.find(l);
    if(i!=literales.end()) //Ya existe.
      (*i).second++;
    else
      std::cerr << "TablaLiterales::NuevaRef; no se encontró el literal: " << l.GetValor() << std::endl;
  }

//! @brief Nueva referencia a un literal.
void TablaLiterales::NuevaRef(const double &d)
  {
    const Literal l(d);
    NuevaRef(l);
  }

//! @brief Quita el literal de la tabla si efectivamente el puntero corresponde a un literal.
void TablaLiterales::NuevaRef(const Segnal *s)
  {
    const Literal *ptr= dynamic_cast<const Literal *>(s);
    if(ptr)
      NuevaRef(*ptr);
  }

//! @brief Añade el literal a la tabla.
const TablaLiterales::Literal *TablaLiterales::Nuevo(const Literal &l)
  { 
    const TablaLiterales::Literal *retval= NULL;

    //Nos aseguramos de que no ha ocurrido
    //el «static initialization order fiasco»
    //cuando declaramos una ExprAlgebra como estática.
    assert(ProtoExpresion::TieneLexico());
  
    iterator i= literales.find(l);
    if(i==literales.end()) //Nuevo.
      {
        value_type tmp(l,1);
        i= (literales.insert(tmp)).first;
      }
    else //Ya existe.
      { (*i).second++; }
    retval= &((*i).first);
    return retval;
  }

//! @brief Quita el literal de la tabla.
void TablaLiterales::Borra(const Literal &l)
  {
    iterator i= literales.find(l);
    if(i!=literales.end()) //existe.
      {
        (*i).second--;
        if((*i).second == 0)
          literales.erase(i);
      }
    else
      std::cerr << "TablaLiterales::Borra; no se encontró el literal: " << l.GetValor() << std::endl;
  }

//! @brief Quita el literal de la tabla.
void TablaLiterales::Borra(const double &d)
  {
    const Literal l(d);
    Borra(l);
  }

//! @brief Quita el literal de la tabla si efectivamente el puntero corresponde a un literal.
void TablaLiterales::BorraSiLiteral(const Segnal *s)
  {
    const Literal *ptr= dynamic_cast<const Literal *>(s);
    if(ptr)
      Borra(*ptr);
  }
