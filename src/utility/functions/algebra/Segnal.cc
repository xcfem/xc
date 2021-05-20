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
//Segnal.cc

#include "Segnal.h"
#include <iostream>

std::ostream &Segnal::Imprime(std::ostream &os) const
  {
    os << getName();
    return os;
  }

const std::string &Segnal::StrClase(void) const
  {
    static std::string s= "";
    switch(GetClase())
      {
        case fin_exp:
          s= "fin_exp";
          break;
        case parent_izdo:
          s= "parent_izdo";
          break;
        case parent_dcho:
          s= "parent_dcho";
          break;
        case operando:
          s= "operando";
          break;
	    case op_unitario:
          s= "op_unitario";
          break;
        case op_binario:
          s= "op_binario";
          break;
	    default:
          s= "desconocida";
      }
    return s;
  }

std::ostream &operator<<(std::ostream &stream, const Segnal &s)
  { return s.Imprime(stream); }
