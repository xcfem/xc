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
//Max_Min.h
// Computation of the extreme of a function that is
// unimodal on the interval (a,b) using the golden section
// method.

#ifndef MAX_MIN_H
#define MAX_MIN_H

#include "utility/matrices/m_double.h"
#include "utility/utils/misc_utils/mchne_eps.h"
#include "Parameterize.h"

typedef enum {interior=1, exterior=2} situ;   // interior, exterior

const bool ant= false;   // extremo anterior
const bool post= true;   // extremo posterior

m_double extremo_calcx(const double &a,const double &b,const double &ab);

m_double extremo_cx( m_double &x,const bool &cambia,const long &interior,const long &exterior);

template<class F>
inline m_double extremo_cy( const F &f,m_double &y,const m_double &x,
                            const bool &cambia,
                            const long &interior,const long &exterior)
  {
    y(cambia+1,(long)exterior)= y(cambia+1,(long)interior);
    y(cambia+1,(long)interior)= y(!cambia+1,(long)interior);
    y(!cambia+1,(long)interior)= f(x(!cambia+1,(long)interior));
    return y;
  }

template <class F>
double extremo( const double &a,const double &b,bool minimo,
                const F &f,const double &dx= sqrt_mchne_eps_dbl)
  {
    bool crece, cambia;

    double ab = b - a;
    m_double x= extremo_calcx(a,b,ab);
    m_double y= matrix_argument(f,x);

    while (ab > dx)
      {
        crece = (y(ant+1,(long)interior) < y(post+1,(long)interior));
        cambia= (crece == minimo);   /* extremo que cambia */
        x= extremo_cx(x,cambia,(long)interior,(long)exterior);
        y= extremo_cy(f,y,x,cambia,(long)interior,(long)exterior);
        ab = x(post+1,(long)exterior) - x(ant+1,(long)exterior);
      }
    return (0.5 * (x(1,(long)exterior) + x(2,(long)exterior)));
  }

template <class F,size_t i>
double extremo( const double &a,const double &b,bool minimo,
                F f, const m_double &x0,
                const double &dx= sqrt_mchne_eps_dbl)
  {
    Parameterize<F,i> pf(f,x0);
    return extremo(a, b, minimo, pf);
  }

#endif
