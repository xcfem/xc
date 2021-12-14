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
//giros.cc

#include "giros.h"
#include <cmath>

//En dos dimensiones.
ZMatrix<double> Giro2D(double ang)
  {
    const double c= cos(ang);
    const double s= sin(ang);
    ZMatrix<double> giro(2,2,0.0);
    giro(1,1)=c;  giro(1,2)=s;
    giro(2,1)=-s; giro(2,2)=c;
    return giro;
  }

//En tres dimensiones
ZMatrix<double> GiroX(double ang)
  {
    const double c= cos(ang);
    const double s= sin(ang);
    ZMatrix<double> giro(3,3,0.0);
    giro(1,1)= 1.0;
    giro(2,2)=c;  giro(2,3)=s;
    giro(3,2)=-s; giro(3,3)=c;
    return giro;
  }

ZMatrix<double> GiroY(double ang)
  {
    const double c= cos(ang);
    const double s= sin(ang);
    ZMatrix<double> giro(3,3,0.0);
    giro(1,1)=c;  giro(1,3)=s;
    giro(2,2)= 1.0;
    giro(3,1)=-s; giro(3,3)=c;
    return giro;
  }

ZMatrix<double> GiroZ(double ang)
  {
    const double c= cos(ang);
    const double s= sin(ang);
    ZMatrix<double> giro(3,3,0.0);
    giro(1,1)=c;  giro(1,2)=s;
    giro(2,1)=-s; giro(2,2)=c;
    giro(3,3)= 1.0;
    return giro;
  }

ZMatrix<double> GiroXYZ(double rx,double ry,double rz)
  {
    ZMatrix<double> giro(3,3,0.0);
    giro= GiroZ(rz) * GiroY(ry) * GiroX(rx);
    return giro;
  }
