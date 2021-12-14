//----------------------------------------------------------------------------
//  xc utils library bilioteca de comandos para el intérprete del lenguaje
//  de entrada de datos.
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
//GaussPoints.cc
#include "gssleg.h"

#include "GaussPoints.h"

//! Generate the Gauss points between x1 y x2 for n ordinates.
GaussPoints::GaussPoints(double x1, double x2, int n) : ConjPG(n)
  {
    v_double w(n); //pesos.
    v_double x(n); //abcisas.
    int i;

    gss_leg(x1, x2, x, w);
    for(i=0;i<n;i++)
      {
        (*this)[i].w = w[i];
        (*this)[i].x = x[i];
      }
  }
std::ostream& operator<<(std::ostream& o, const GaussPoints& gpts)
  {
    GaussPoints::const_iterator i;
    for(i=gpts.begin();i!=gpts.end();i++)
      o << (*i) << ", ";
    return o;
  }
