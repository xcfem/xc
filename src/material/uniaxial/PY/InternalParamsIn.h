// -*-c++-*-
//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
//InternalParamsIn.h

#ifndef INTERNALPARAMSIN_H
#define INTERNALPARAMSIN_H

#include "InternalParamsA.h"

namespace XC {
//! @ingroup PYMat
//
//! @brief Internal parameters for a p-y material.
class InternalParamsIn: public InternalParamsA
  {
  protected:
    double Rin; //!<  R at start of current plastic loading cycle
    double vin; //!<  v (y or z) at start of current plastic loading cycle
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    InternalParamsIn(const double &V= 0.0,const double &r= 0.0,const double &t= 0.0,const double &Vin= 0.0,const double &rin= 0.0);
    inline const double &yin(void) const //Para PySimple1
      { return vin; }
    inline double &yin(void) //Para PySimple1
      { return vin; }
    inline double &zin(void) //Para QzSimple1
      { return vin; }
    inline const double &zin(void) const //Para QzSimple1
      { return vin; }
    inline const double &Pin(void) const //Para PySimple1
      { return Rin; }
    inline double &Pin(void) //Para PySimple1
      { return Rin; }
    inline const double &Qin(void) const //Para QzSimple1
      { return Rin; }
    inline double &Qin(void) //Para QzSimple1
      { return Rin; }
    inline const double &Tin(void) const //Para TzSimple1
      { return Rin; }
    inline double &Tin(void) //Para TzSimple1
      { return Rin; }
    void revertToStart(const double &tg);
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
  };
} // end of XC namespace


#endif
