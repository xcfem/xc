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
//max_min.cc

#include "max_min.h"

m_double extremo_calcx(const double &a,const double &b,const double &ab)
  {
    const double r= (sqrt(5.0)-1)/2.0; //0.61803399...

    m_double x(2,2);
    x(ant+1,exterior)= a;  x(ant+1,interior)= b - ab * r;
    x(post+1,exterior)= b; x(post+1,interior)= a + ab * r;
    return x;
  }

m_double extremo_cx( m_double &x,const bool &cambia,
                            const long &interior,const long &exterior)
  {
    x(cambia+1,exterior)= x(cambia+1,interior);
    x(cambia+1,interior)= x(!cambia+1,interior);
    x(!cambia+1,interior)= x(ant+1,exterior) + 
                           x(post+1,exterior) - 
                           x(cambia+1,interior);
    return x;
  }
