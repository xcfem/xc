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
//gssleg.cc

#include "gssleg.h"
#include <iostream>

#define	TOLERANCE	3.0e-11 /* value used in NRiC */

void gss_leg(double x1, double x2,v_double &x,v_double &w)
  {
    size_t n= x.size();
    if(n!=w.size())
      {
        std::cerr << "Error en gss_leg" << std::endl;
        return;
      }
    double xavg = (x2+x1)/2.0, dx = (x2-x1);
    double zi, oldzi;
    size_t n2 = (n+1)/2;
    double p[3], dp;

    // loop over lower half the points
    for(size_t i=1;i<=n2;i++)
      {
        //initial guess at root
        zi = cos(M_PI*(i-.25)/(n+.5));
        do
          {
            // Calculate value of nth polynomial.  This will be used
            // to calculate the derivative of the polynomial later on.
            // the derivative in turn is used to improve the guess of the
            // root
            p[1] = 0.0;
            p[2] = 1.0;
            for(size_t j=1;j<=n;j++)
              {			
                p[0] = p[1];
                p[1] = p[2];
                p[2] = ((2*j-1)*zi*p[1]-(j-1.0)*p[0])/j;
              }

            // Newton-Raphson refinement - Continue until the
            // iteration doesn't improve much
            oldzi = zi;
            dp = n*(zi*p[2]-p[1])/(zi*zi-1.0);
            zi = oldzi - p[2]/dp;
          }
        while(fabs(zi-oldzi) > TOLERANCE);
		
        // Refinement succeeded, now transform from (-1,1) to (x1,x2)
        x[i-1] = xavg-(dx/2.0)*zi;
        w[i-1] = dx/((1.0-zi*zi)*dp*dp);
        //Mirror image values
        x[n-i] = xavg+(dx/2.0)*zi;
        w[n-i] = w[i-1];
      }
  }
