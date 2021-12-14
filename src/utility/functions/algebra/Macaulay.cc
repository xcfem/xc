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
//Macaulay.cc
#include "Macaulay.h"
#include "Lexico.h"
#include "boost/lexical_cast.hpp"
#include <cmath>

double OpMacaulay0::get_val(const double &x,const double &a) const
  {
    if(x<=a)
      return 0.0;
    else
      return 1.0;
  }

const Operando &OpMacaulay0::Opera(const Operando *v1,const Operando *v2) const
  {
    const double &x= v1->GetValor();
    const double &a= v2->GetValor();
    return *Lex().NuevoLiteral(get_val(x,a));
  }


OpMacaulayN::OpMacaulayN(size_t n)
  : OpMacaulay0(), N(n), nmbOpMacaulayN("Fmac"+boost::lexical_cast<std::string>(N)) {}

double OpMacaulayN::get_val(const double &x,const double &a) const
  {
    const double v0= OpMacaulay0::get_val(x,a);
    if(x<=a)
      return v0;
    else
      return pow(x-a,N)*v0;
  }
