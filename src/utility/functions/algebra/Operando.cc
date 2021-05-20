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
//Operando.cc

#include "Operando.h"
#include "Rama.h"
#include "Lexico.h"
#include "boost/lexical_cast.hpp"
#include <sstream>

Rama Operando::Diferencia(const Variable &,const Rama &r) const
  {
    std::cerr << "Operando::" << __FUNCTION__
              << " not implemented." << std::endl;
    return Rama();
//     r->Put(Lex().NuevoLiteral(0.0),r->getLeft(),r->getRight());
//     return r;
  }

//! @brief Simplifica la expresión.
Rama *Operando::Simplifica(Rama *r) const
  { return r; }

//! @brief En su caso, aplica la propiedad distributiva.
Rama *Operando::Distrib(Rama *r) const
  { return r; }

//! @brief Return the operand name.
const std::string &Operando::getName(void) const
  { return GetFullString(); }

//! @brief Devuelve la cadena de caracteres correspondiente al valor
//! con toda la precisión posible.
const std::string &Operando::GetFullString(void) const
  {
    static std::string strName= "";
    strName= boost::lexical_cast<std::string>(valor);
    return strName;
  }

