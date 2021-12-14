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
//OpTrigonometricas.h

#ifndef OPTRIGONOMETRICAS_H
#define OPTRIGONOMETRICAS_H

#include "OpUnario.h"
#include "operator_names.h"

class OpPi : public OpUnario
  {
    
  public:
    OpPi(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpPi; }
    virtual const Operando &Opera(const Operando *v1= NULL,const Operando *v2= NULL) const;
  };

class OpSin : public OpUnario
  {
    
  public:
    OpSin(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpSin; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpCos : public OpUnario
  {
    
  public:
    OpCos(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpCos; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpTan : public OpUnario
  {
    
  public:
    OpTan(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpTan; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

class OpAtan : public OpUnario
  {
    
  public:
    OpAtan(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpAtan; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };

//! @brief Secant function sec(x)= 1/cos(x)
class OpSec : public OpUnario
  {
    
  public:
    OpSec(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpSec; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
  };

//! @brief Cosecant function cosec(x)= 1/sin(x)
class OpCosec : public OpUnario
  {
    
  public:
    OpCosec(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpCosec; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
  };

//! @brief Cotangent function cotg(x)= 1/tan(x)
class OpCotg : public OpUnario
  {
    
  public:
    OpCotg(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpCotg; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
  };

//! @brief Function that converts sexagesimal degrees into
//! radians deg2rad(x)= pi/180*x
class OpDeg2Rad : public OpUnario
  {
    
  public:
    OpDeg2Rad(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpDeg2Rad; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };


//! @brief Function that converts radians into sexagesimal degrees
//! rad2deg(x)= 180/pi*x
class OpRad2Deg : public OpUnario
  {
    
  public:
    OpRad2Deg(void) : OpUnario() {}
    inline virtual const std::string &getName(void) const
      { return nmbOpRad2Deg; }
    virtual const Operando &Opera(const Operando *,const Operando *v2= NULL) const;
    virtual Rama Diferencia(const Variable &,const Rama &r) const;
  };


#endif
