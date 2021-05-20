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
//Macaulay.h

#ifndef MACAULAY_H
#define MACAULAY_H

#include "OpBinario.h"
#include "operator_names.h"

class Rama;

//! @brief Macaulay function F0 (See Mecánica de materiales Gere-Timoshenko paragraph 7.9).
//! se define como:
//! <x-a>= 0 si x<=a.
//! <x-a>= 1 si x>=a.
class OpMacaulay0: public OpBinario
  {
  protected:
    virtual double get_val(const double &x,const double &a) const;
  public:
    OpMacaulay0(void) : OpBinario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpMacaulay0; }
    virtual int GetPrioridad(void) const
      { return 4; }
    inline virtual Posicion GetPosicion(void) const
      { return prefija; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };


//! @brief Macaulay function Fn (Ver Mecánica de materiales Gere-Timoshenko paragraph 7.9).
//! se define como:
//! <x-a>= 0 si x<=a.
//! <x-a>= (x-a)^n si x>=a.
class OpMacaulayN: public OpMacaulay0
  {
    size_t N;
    std::string nmbOpMacaulayN;
  protected:
    virtual double get_val(const double &x,const double &a) const;
  public:
    OpMacaulayN(size_t n=1);
    inline virtual const std::string &getName(void) const
      { return nmbOpMacaulayN; }
    virtual ~OpMacaulayN(void)
      {}
  };

#endif
