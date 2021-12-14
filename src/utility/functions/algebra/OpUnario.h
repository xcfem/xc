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
//OpUnario.h

#ifndef OPUNARIO_H
#define OPUNARIO_H

#include "Operador.h"
#include "operator_names.h"

class OpUnario: public Operador
  {
  public:
    OpUnario(void) : Operador() {}
    OpUnario(const int &p): Operador(p) {}
    OpUnario(const OpUnario &otro): Operador(otro) {}
    OpUnario &operator=(const OpUnario &otro)
      {
        Operador::operator =(otro);
        return *this;
      }
    inline virtual Clase GetClase(void) const
      { return op_unitario; }
    inline virtual Posicion GetPosicion(void) const
      { return prefija; }
    virtual int GetPrioridad(void) const
      { return 6; }
    Rama *operator()(Rama *r) const;
  };

class OpNeg : public OpUnario
  {
    
  public:
    OpNeg(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpNeg; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpInv : public OpUnario
  {
    
  public:
    OpInv(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpInv; }
    virtual const Operando &Opera(const Operando *,const Operando *v2=NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpSign : public OpUnario
  {
    
  public:
    OpSign(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpSign; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &) const;
  };

class OpAbs : public OpUnario
  {
    
  public:
    OpAbs(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpAbs; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &) const;
  };

class OpSqr : public OpUnario
  {
    
  public:
    friend class Lexico;
    OpSqr(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpSqr; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpSqrt : public OpUnario
  {
    
  public:
    OpSqrt(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpSqrt; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpExp : public OpUnario
  {
    
  public:
    OpExp(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpExp; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

//! @brief Logaritmo neperiano.
class OpLn : public OpUnario
  {
    
  public:
    OpLn(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpLn; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpLg : public OpUnario
  {
    
  public:
    OpLg(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpLg; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpRound : public OpUnario
  {
    
  public:
    OpRound(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpRound; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
  };

class OpTrunc : public OpUnario
  {
    
  public:
    OpTrunc(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpTrunc; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
  };

class OpToNum : public OpUnario
  {
    
  public:
    OpToNum(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpToNum; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
  };
#endif










