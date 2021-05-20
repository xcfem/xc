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
//inversa.h

#ifndef INVERSA_H
#define INVERSA_H


#include "gauss.h"

template <class treal>
ZMatrix<treal> inversa(ZMatrix<treal> &m,int &regular)
  {
    if (!m.Cuadrada()) ZMatrixError("Not a square matrix.");
    if(m.getNumberOfRows()<2)
      {
        //invierte kii
        ZMatrix<treal> t(m);
        treal det= t(1,1)*t(2,2)-t(2,1)*t(1,2);
        if(det == 0.0)
          regular= 0;
        else
          {
            treal dum= t(1,1)/det;
            t(1,1)= t(2,2)/det;
            t(1,2)= -t(1,2)/det;
            t(2,1)= -t(2,1)/det;
            t(2,2)= dum;
          }
        return t;
      }
    else //Mejorar (ver matrix++)
      return gauss_invierte(m,regular);
  }

#endif
