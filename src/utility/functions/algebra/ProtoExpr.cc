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
//ProtoExpresion.cc
#include "LexAlgebra.h"
#include "ProtoExpr.h"
#include <cassert>

LexAlgebra *ProtoExpresion::ptr_lex= NULL;

//!@brief Constructor.
ProtoExpresion::ProtoExpresion(void)
  {}

//! @brief Devuelve una referencia al léxico.
Lexico &ProtoExpresion::Lex(void)
  { return *ptr_lex; }

//! @brief Devuelve una referencia al léxico.
LexAlgebra &ProtoExpresion::LexA(void)
  { return *ptr_lex; }

//! @brief Devuelve una referencia al léxico.
LexAlgebra &ProtoExpresion::CreaLexico(void)
  {
    if(!ptr_lex)
      {
        ptr_lex= new LexAlgebra;
        assert(ptr_lex);
      }
    return *ptr_lex;
  }

bool ProtoExpresion::TieneLexico(void)
  { return (ptr_lex!=NULL); }

//! @brief Return a pointer to the variable which name
//! is being passed as parameter.
Variable *ProtoExpresion::CalcDirVar(const std::string &nmb)
  { return ptr_lex->CalcDirVar(nmb); }
//! @brief Creates the variable being passed as parameter.
NamedOperand *ProtoExpresion::NuevaVar(const Variable &v)
  { return ptr_lex->NuevaVar(v); }
//! @brief Creates the variable which name
//! is being passed as parameter.
NamedOperand *ProtoExpresion::NuevaVar(const std::string &palabra)
  { return ptr_lex->NuevaVar(Variable(palabra)); }

//! @brief Return a pointer to the signal which name
//! is being passed as parameter.
Segnal *ProtoExpresion::CalcularDireccion(const std::string &nmb)
  { return ptr_lex->CalcularDireccion(nmb); }

//! @brief Assigns to the variable named 'palabra' the value
//! being passed as parameter.
void ProtoExpresion::Asigna(const std::string &palabra,const double &d)
  { ptr_lex->Asigna(palabra,d); }
//! @brief Removes the assignment to the variable which name
//! is being passed as parameter.
void ProtoExpresion::DesAsigna(const std::string &palabra)
  { ptr_lex->DesAsigna(palabra); }
