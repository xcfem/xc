// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  This program derives from OpenSees <http://opensees.berkeley.edu>
//  developed by the  «Pacific earthquake engineering research center».
//
//  Except for the restrictions that may arise from the copyright
//  of the original program (see copyright_opensees.txt)
//  XC is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or 
//  (at your option) any later version.
//
//  This software is distributed in the hope that it will be useful, but 
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details. 
//
//
// You should have received a copy of the GNU General Public License 
// along with this program.
// If not, see <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
//InternalParamsA.h

#ifndef INTERNALPARAMSA_H
#define INTERNALPARAMSA_H

#include "utility/actor/actor/MovableObject.h"

namespace XC {
//! @ingroup PYMat
//
//! @brief Internal parameters for the Gap (Drag + Closure).
class InternalParamsA: public MovableObject
  {
  protected:
    double v; //!< Displacement.
    double R; //!< Force.
    double Tang; //!< Combined tangent

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    InternalParamsA(const double &V= 0.0,const double &r= 0.0,const double &t= 0.0);
    inline const double &y(void) const //For PySimple1
      { return v; }
    inline double &y(void) //For PySimple1
      { return v; }
    inline double &z(void) //For QzSimple1
      { return v; }
    inline const double &z(void) const //For QzSimple1
      { return v; }
    inline const double &P(void) const //For PySimple1
      { return R; }
    inline double &P(void) //For PySimple1
      { return R; }
    inline const double &Q(void) const //For QzSimple1
      { return R; }
    inline double &Q(void) //For QzSimple1
      { return R; }
    inline const double &T(void) const //For TzSimple1
      { return R; }
    inline double &T(void) //For TzSimple1
      { return R; }
    inline const double &tang(void) const
      { return Tang; }
    inline double &tang(void)
      { return Tang; }

    void getField(const double &V);
    const double &updateField(const double &Tp);
    inline const double unbalance(const double &Tp) const
      { return (Tp - R); }
    inline const double residual(const double &Tp) const
      { return unbalance(Tp)/Tang; }

    void revertToStart(const double &tg);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace


#endif
