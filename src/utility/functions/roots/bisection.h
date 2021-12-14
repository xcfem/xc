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
//bisection.h

#ifndef BISECTION_H
#define BISECTION_H

#include "utility/utils/misc_utils/matem.h"
#include "utility/utils/misc_utils/mchne_eps.h"

template<class F>
inline bool Bisection(double low, double high, double tolerance,double &root,const double *params, F f)
  {
    if (f(low, params) * f(high, params) > 0) return false;

    while (fabs(high - low) > tolerance)
      {
        root = (low + high) / 2; //Actualizamos la aproximación
        if (f(root, params) * f(high, params) > 0)
          high = root;
        else
          low = root;
      }
    return true;
  }

class SolverBiseccion
  {
    double low;
    double high;
    double tolerance;
    bool error;

    template<class F>
    double Biseccion(const double &raiz,const double *params, F f)
      {
        double root= raiz;
        error= Bisection(low,high,tolerance,root,params,f);
        return root;
      }

  public:
    SolverBiseccion(const double &min,const double &max,const double &tol= sqrt_mchne_eps_dbl)
      : low(min), high(max), tolerance(tol), error(false) {}
    inline const bool &GetError(void) const
      { return error; }
    void Reset(const double &min,const double &max,const double &tol= sqrt_mchne_eps_dbl)
      {
        low=min;
        high= max;
        tolerance= tol;
        error= false;
      }
    template<class F>
    inline double Solve(const double &raiz,const double *params, F f)
      {
        const double low_bak= low;
        const double high_bak= high;
        const double tolerance_bak= tolerance;
        const double retval=Biseccion(raiz,params,f);
        Reset(low_bak,high_bak,tolerance_bak);
        return retval;
      }
  };

#endif
