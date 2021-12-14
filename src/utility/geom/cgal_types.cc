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
//cgal_types.cc

#include "cgal_types.h"
#include "CGAL/to_rational.h"

GEOM_FT double_to_FT(const double &d)
  {
    //return CGAL::to_rational<GEOM_FT>(d);
    return d;
  }

GEOM_FT sqrt_FT(const GEOM_FT &n)
  { 
    const double tmp= sqrt(n);
    return double_to_FT(tmp);
  }

GEOM_FT abs_FT(const GEOM_FT &n)
  { return fabs(n); }

CGPoint_2 Point_2_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y)
  {
//     const GEOM_RT wsx = x.numerator() * y.denominator(); 
//     const GEOM_RT wsy = y.numerator() * x.denominator(); 
//     const GEOM_RT wsh  = x.denominator() * y.denominator(); 
//     return CGPoint_2(wsx, wsy, wsh);
    return CGPoint_2(x, y, 1.0);
  }
CGDirection_2 Dir_2_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y)
  {
//     const GEOM_RT wsx = x.numerator() * y.denominator(); 
//     const GEOM_RT wsy = y.numerator() * x.denominator(); 
//     return CGDirection_2(wsx, wsy);
    return CGDirection_2(x, y);
  }
CGVector_2 Vector_2_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y)
  {
//     const GEOM_RT wsx = x.numerator() * y.denominator(); 
//     const GEOM_RT wsy = y.numerator() * x.denominator(); 
//     const GEOM_RT wsh  = x.denominator() * y.denominator(); 
//     assert(wsh!=0);
//     return CGVector_2(wsx, wsy, wsh);
    return CGVector_2(x, y, 1.0);
  }
CGPoint_2 Point_2_from_doubles(const double &x,const double &y)
  {
//     const GEOM_FT p_q_x(double_to_FT(x));
//     const GEOM_FT p_q_y(double_to_FT(y));
//     return Point_2_from_cartesianas(p_q_x,p_q_y);
    return Point_2_from_cartesianas(x,y);
  }
CGVector_2 Vector_2_from_doubles(const double &x,const double &y)
  {
//     const GEOM_FT p_q_x(double_to_FT(x));
//     const GEOM_FT p_q_y(double_to_FT(y));
//     return Vector_2_from_cartesianas(p_q_x,p_q_y);
    return Vector_2_from_cartesianas(x,y);
  }

CGPoint_3 Point_3_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y,const GEOM_FT &z)
  {
//     const GEOM_RT wsx = x.numerator() * y.denominator() * z.denominator(); 
//     const GEOM_RT wsy = y.numerator() * x.denominator() * z.denominator(); 
//     const GEOM_RT wsz = z.numerator() * x.denominator() * y.denominator(); 
//     const GEOM_RT wsh  = x.denominator() * y.denominator() * z.denominator(); 
//     return CGPoint_3(wsx, wsy, wsz, wsh);
    return CGPoint_3(x, y, z, 1.0);
  }
CGDirection_3 Dir_3_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y,const GEOM_FT &z)
  {
//     const GEOM_RT wsx = x.numerator() * y.denominator() * z.denominator(); 
//     const GEOM_RT wsy = y.numerator() * x.denominator() * z.denominator(); 
//     const GEOM_RT wsz = z.numerator() * x.denominator() * y.denominator(); 
//     return CGDirection_3(wsx, wsy,wsz);
    return CGDirection_3(x, y,z);
  }
CGVector_3 Vector_3_from_cartesianas(const GEOM_FT &x,const GEOM_FT &y,const GEOM_FT &z)
  {
//     const GEOM_RT wsx = x.numerator() * y.denominator() * z.denominator(); 
//     const GEOM_RT wsy = y.numerator() * x.denominator() * z.denominator(); 
//     const GEOM_RT wsz = z.numerator() * x.denominator() * y.denominator(); 
//     const GEOM_RT wsh  = x.denominator() * y.denominator() * z.denominator(); 
//     assert(wsh!=0);
//     return CGVector_3(wsx, wsy, wsz, wsh);
    return CGVector_3(x, y, z, 1.0);
  }
CGPoint_3 Point_3_from_doubles(const double &x,const double &y,const double &z)
  {
//     const GEOM_FT p_q_x(double_to_FT(x));
//     const GEOM_FT p_q_y(double_to_FT(y));
//     const GEOM_FT p_q_z(double_to_FT(z));
//     return Point_3_from_cartesianas(p_q_x,p_q_y,p_q_z);
    return Point_3_from_cartesianas(x,y,z);
  }
CGVector_3 Vector_3_from_doubles(const double &x,const double &y,const double &z)
  {
//     const GEOM_FT p_q_x(double_to_FT(x));
//     const GEOM_FT p_q_y(double_to_FT(y));
//     const GEOM_FT p_q_z(double_to_FT(z));
//     return Vector_3_from_cartesianas(p_q_x,p_q_y,p_q_z);
    return Vector_3_from_cartesianas(x,y,z);
  }

