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
//FunctionFromPointsR_R.h

#ifndef FUNCTIONFROMPOINTSR_R_H
#define FUNCTIONFROMPOINTSR_R_H

#include "FunctionFromPointsR_T.h"
#include <deque>

typedef std::deque<double> dq_double;

//! @brief f:R->R function defined on a set of points.
//!
//! Function that for each point x from the real domain returns a 
//! real value. The correspondence is established by points,
//! that is, by pairs (x1,y1), (x2,y2), ...
class FunctionFromPointsR_R: public FunctionFromPointsR_T<double>
  {
  public:
    FunctionFromPointsR_R::const_iterator PosMax(void) const;
    FunctionFromPointsR_R &operator=(const FunctionFromPointsR_R &);
    FunctionFromPointsR_R &operator=(const double &);
    //! @brief Return the maximum value de las ordenadas definidas.
    double Max(void) const
      { return (*PosMax()).second; }
    //! @brief Return the abcisa correspondiente al valor máximo de las ordenadas definidas.
    double AbcisaMax(void) const
      { return (*PosMax()).first; }
    FunctionFromPointsR_R::const_iterator PosMin(void) const;
    //! @brief Return el valor mínimo de las ordenadas definidas.
    double Min(void) const
      { return (*PosMin()).second; }
    //! @brief Return the abcisa correspondiente al valor mínimo de las ordenadas definidas.
    double AbcisaMin(void) const
      { return (*PosMin()).first; }
    double getArea(void) const;
    double firstX(void) const;
    double firstY(void) const;
    double lastX(void) const;
    double lastY(void) const;
    dq_double Dominio(void) const;
    dq_double Abcisas(void) const;
    dq_double Ordenadas(void) const;
    dq_double Ceros(void) const;

    FunctionFromPointsR_R sub(const double &x0,const double &x1) const;
    FunctionFromPointsR_R getNeg(void) const;
    FunctionFromPointsR_R &Neg(void);
    FunctionFromPointsR_R &operator+=(const double &);
    FunctionFromPointsR_R &operator+=(const FunctionFromPointsR_R &);
    FunctionFromPointsR_R &operator-=(const double &);
    FunctionFromPointsR_R &operator-=(const FunctionFromPointsR_R &);
    FunctionFromPointsR_R &operator*=(const double &);
    FunctionFromPointsR_R &operator*=(const FunctionFromPointsR_R &);
    FunctionFromPointsR_R &operator/=(const double &);
    FunctionFromPointsR_R &operator/=(const FunctionFromPointsR_R &);
    FunctionFromPointsR_R integra(const double &x0,const double &x1,const double &delta= -1.0) const;

  };

FunctionFromPointsR_R asigna(const FunctionFromPointsR_R &,const double &);
FunctionFromPointsR_R operator+(const FunctionFromPointsR_R &,const double &);
FunctionFromPointsR_R operator+(const FunctionFromPointsR_R &,const FunctionFromPointsR_R &);
inline FunctionFromPointsR_R operator+(const double &c,const FunctionFromPointsR_R &f)
  { return f+c;}
inline FunctionFromPointsR_R operator-(const FunctionFromPointsR_R &f,const double &c)
  { return f+(-c); }
inline FunctionFromPointsR_R operator-(const double &c,const FunctionFromPointsR_R &f)
  { return c+f.getNeg();}
FunctionFromPointsR_R operator-(const FunctionFromPointsR_R &,const FunctionFromPointsR_R &);
FunctionFromPointsR_R operator*(const double &,const FunctionFromPointsR_R &);
inline FunctionFromPointsR_R operator*(const FunctionFromPointsR_R &f,const double &c)
  { return c*f; }
FunctionFromPointsR_R operator*(const FunctionFromPointsR_R &,const FunctionFromPointsR_R &);
inline FunctionFromPointsR_R operator/(const FunctionFromPointsR_R &f,const double &c)
  { return (1.0/c)*f; }
FunctionFromPointsR_R operator/(const FunctionFromPointsR_R &,const FunctionFromPointsR_R &);

#endif
