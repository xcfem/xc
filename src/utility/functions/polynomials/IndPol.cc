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
//IndPol.cc

#include "IndPol.h"

bool operator<(const IndPol &i1,const IndPol &i2)
  {
    assert(i1.size() == i2.size());
    IndPol::size_type ultimo= i1.size() - 1;
    long i;
    if (i1.Grado() < i2.Grado())
      return 1;
    else
      if (i1.Grado() > i2.Grado()) 
        return 0;
    for(i=ultimo;i>=0;i--)
      if (i1[i] < i2[i]) 
        return 1;
      else
        if (i1[i] > i2[i]) 
          return 0;
    return 0;
  }

IndPol Conv(const NmbVars &destVars,const NmbVars &srcVars, const IndPol &srcInd) 
//Convierte el indice Indpol referido al conjunto de variables
//srcVars al conjunto de variables destVars, devolviendo
//este ultimo.
  {
    if (destVars == srcVars) return srcInd;
    IndPol conv(destVars.size(),0);
    IndPol::size_type i;
    vZ_sui ind= destVars.Indices(srcVars);
    for(i=0;i<srcInd.size();i++)
      conv[ind[i]]= srcInd[i];
    return conv;
  }
