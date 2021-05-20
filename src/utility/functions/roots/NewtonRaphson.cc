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
//NewtonRaphson.cc

#include "NewtonRaphson.h"

bool norma_inf_nula(const m_double &x,const double &tolerance)
  {
    size_t sz= x.getNumberOfRows();
    for(size_t i= 1;i<= sz;i++)
      if(fabs(x(i)) > tolerance) return false;
    return true;
  }

double dot_trn(const m_double &v1,const m_double &v2)
    //Producto escalar de dos vectores COLUMNA
     //Sirve para evitar calcular la traspuesta de v1.
      {
        const size_t n_rows= v1.getNumberOfRows();
        double retval(v1(1,1)*v2(1,1));
        for(m_double::size_type i=2;i<=n_rows;i++)
          { retval+= v1(i,1) * v2(i,1); }
        return retval;
      }
