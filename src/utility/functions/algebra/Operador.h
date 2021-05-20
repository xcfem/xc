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
//Operador.h

#ifndef OPERADOR_H
#define OPERADOR_H

#include "Operando.h"
#include "operator_names.h"

//! @brief Clase base de los operadores unarios y binarios.
class Operador : public Operando
  {
  protected:
    int prioridad;
  public:
    friend class Lexico;
    Operador(void) : Operando()
      { Put(-1); }
    Operador(const int &p)
      : Operando(), prioridad(p) {}
    Operador(const Operador &otro)
      : Operando(otro), prioridad(otro.prioridad) {}
    Operador &operator=(const Operador &otro);
    void Put(const int &pv)
      {
        Operando::Put(0.0);
        prioridad= pv;
      }
    virtual int GetPrioridad(void) const
      { return prioridad; }
    virtual const std::string &GetFullString(void) const;
  };

class ParentIzdo : public Operador
  {
  public:
    ParentIzdo(void) : Operador(-1) {}
    inline virtual Clase GetClase(void) const
      { return parent_izdo; }
    inline virtual const std::string &getName(void) const
      { return nmbParentIzdo; }
  };

class ParentDcho : public Operador
  {
  public:
    ParentDcho(void) : Operador(-1) {}
    inline virtual Clase GetClase(void) const
      { return parent_dcho; }
    inline virtual const std::string &getName(void) const
      { return nmbParentDcho; }
  };


#endif



