// -*-c++-*-
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
//NamedOperand.h

#ifndef NAMEDOPERAND_H
#define NAMEDOPERAND_H

#include "Operando.h"

class NamedOperand : public Operando
  {
  protected:
    std::string name;
  public:
    NamedOperand(void) : Operando(), name() {}
    NamedOperand(const std::string &nmb,const double &d=0.0): Operando(d), name(nmb) {}
    NamedOperand(const NamedOperand &otro): Operando(otro), name(otro.name) {}
    NamedOperand &operator=(const NamedOperand &otro)
      {
        Operando::operator =(otro);
        name= otro.name;
        return *this;
      }
    void Put(const std::string &nmb, const double &v)
      {
	Operando::Put(v);
        name= nmb;
      }
    inline virtual const std::string &getName(void) const
      { return name; }
    inline virtual const std::string &GetFullString(void) const
      { return name; }
    friend std::istream &operator >>(std::istream &is, NamedOperand &s);
    virtual int GetPrioridad(void) const
      { return 8; }
    virtual ~NamedOperand(void)
      {}
  };


#endif
