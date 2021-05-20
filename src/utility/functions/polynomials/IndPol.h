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
//IndPol.h
#ifndef INDPOL_H
#define INDPOL_H

#include <iostream>
#include "NmbVars.h"
#include "utility/matrices/vectorZ.h"

// Integer vector used to store the powers of the polynomial variables
// as follows:

//    x2 y3 z8 -----> [2,3,8]
typedef vectorZ<short unsigned int> vZ_sui; 

template std::ostream& operator<<(std::ostream &stream,const vZ_sui &v);

class IndPol : public vZ_sui
  {
  public:
    IndPol(void) : vZ_sui(1,(unsigned short int)0) {}
    IndPol(size_type n,const short unsigned int &value = 0) : vZ_sui(n,value) {}
    IndPol(const IndPol &otro) : vZ_sui(otro) {}
    IndPol(const vZ_sui &otro) : vZ_sui(otro) {}
    IndPol &operator =(const IndPol &otro)
      {
        vZ_sui::operator =(otro);
        return *this;
      }
    IndPol &operator =(const vZ_sui &otro)
      {
        vZ_sui::operator =(otro);
        return *this;
      }
    friend IndPol Conv(const NmbVars &dest,const NmbVars &srcVars, const IndPol &srcInd);
    friend IndPol operator +(const IndPol &i1,const IndPol &i2)
    //Devuelve la suma de las potencias.
      {
        assert(i1.size() == i2.size());
        IndPol suma(i1.size());
        suma.Suma(i1,i2);
        return suma;
      }
    friend IndPol operator -(const IndPol &i1,const IndPol &i2)
    //Devuelve la diferencia de las potencias.
      {
        assert(i1.size() == i2.size());      
        IndPol resta(i1.size());
        resta.Resta(i1,i2);
        return resta;
      }
    short unsigned int Grado(void) const
    //Devuelve el grado del termino que tiene por potencias las especificadas
    //en el IndPol.
      { return Sumatorio(0,size()); }
    short unsigned int Grado(size_type j) const
    //Devuelve el grado del termino en la varieble aue tiene por indice j.
      { return (*this)[j]; }
    friend bool operator < (const IndPol &i1,const IndPol &i2);
  };


bool operator < (const IndPol &i1,const IndPol &i2);

IndPol Conv(const NmbVars &destVars,const NmbVars &srcVars, const IndPol &srcInd);
//Convierte el indice Indpol referido al conjunto de variables
//srcVars al conjunto de variables destVars, devolviendo
//este ultimo.

#endif
