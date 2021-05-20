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
//OpBinario.h

#ifndef OPBINARIO_H
#define OPBINARIO_H

#include "Operador.h"
#include "operator_names.h"

class Rama;

//! @brief Operador binario.
class OpBinario : public Operador
  {
  public:
    OpBinario(void) : Operador() {}
    OpBinario(const int &p): Operador(p) {}
    OpBinario(const OpBinario &otro): Operador(otro) {}
    OpBinario &operator=(const OpBinario &otro)
      {
        Operador::operator =(otro);
        return *this;
      }
    inline virtual Clase GetClase(void) const
      { return op_binario; }
    inline virtual Posicion GetPosicion(void) const
      { return infija; }
    Rama *operator()(Rama *r1,Rama *r2) const;
  };

class OpSuma : public OpBinario
  {
    
  public:
    OpSuma(void) : OpBinario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpSuma; }
    virtual int GetPrioridad(void) const
      { return 4; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
    virtual Rama *Simplifica(Rama *r) const;
  };

class OpResta : public OpBinario
  {
    
  public:
    OpResta(void) : OpBinario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpResta; }
    virtual int GetPrioridad(void) const
      { return 4; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpProd : public OpBinario
  {
    
  public:
    OpProd(void) : OpBinario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpProd; }
    virtual int GetPrioridad(void) const
      { return 5; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
    virtual Rama *Simplifica(Rama *r) const;
    Rama *DistribDcha(Rama *raiz) const;
    Rama *DistribIzda(Rama *raiz) const;
    Rama *Distrib(Rama *r) const;
  };

class OpDiv : public OpBinario
  {
    
  public:
    OpDiv(void) : OpBinario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpDiv; }
    virtual int GetPrioridad(void) const
      { return 5; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpDivEnt : public OpBinario
  {
    
  public:
    OpDivEnt(void) : OpBinario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpDivEnt; }
    virtual int GetPrioridad(void) const
      { return 5; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

class OpMod : public OpBinario
  {
    
  public:
    OpMod(void) : OpBinario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpMod; }
    virtual int GetPrioridad(void) const
      { return 5; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

class OpPot : public OpBinario
  {
    
  public:
    OpPot(void) : OpBinario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpPot; }
    virtual int GetPrioridad(void) const
      { return 6; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

//! @brief Operador max.
//! Devuelve el máximo de los valores izquierdo y derecho
class OpMax : public OpBinario
  {
    
  public:
    OpMax(void) : OpBinario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpMax; }
    virtual int GetPrioridad(void) const
      { return 4; }
    inline virtual Posicion GetPosicion(void) const
      { return prefija; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

//! @brief Operador min.
//! Devuelve el mínimo de los valores izquierdo y derecho
class OpMin : public OpBinario
  {
    
  public:
    OpMin(void) : OpBinario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpMin; }
    virtual int GetPrioridad(void) const
      { return 4; }
    inline virtual Posicion GetPosicion(void) const
      { return prefija; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

#endif
