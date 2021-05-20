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
//ProtoExpresion.h

#ifndef PROTOEXPRESION_H
#define PROTOEXPRESION_H

#include <string>

class Lexico;
class LexAlgebra;
class Operando;
class NamedOperand;
class Segnal;
class Variable;

class ProtoExpresion
  {
  protected:
    static LexAlgebra *ptr_lex;

    static Segnal *CalcularDireccion(const std::string &nmb);
  public:
    ProtoExpresion(void);
    static LexAlgebra &CreaLexico(void);
    static Lexico &Lex(void);
    static LexAlgebra &LexA(void);
    static bool TieneLexico(void);
    static NamedOperand *NuevaVar(const Variable &v);
    static NamedOperand *NuevaVar(const std::string &palabra);
    static Variable *CalcDirVar(const std::string &nmb);
    static void Asigna(const std::string &palabra,const double &d);
    static void DesAsigna(const std::string &palabra);
  };

#endif
