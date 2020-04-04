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
//InternalParamsLRIn.h

#ifndef INTERNALPARAMSLRIn_H
#define INTERNALPARAMSLRIn_H

#include "InternalParamsLR.h"

namespace XC {
//! @ingroup PYMat
//
//! @brief Internal parameters for the NearField rigid-plastic component.
class InternalParamsLRIn: public InternalParamsLR
  {
  protected:
    double RLeft; //!< p at start of current plastic loading cycle - left side
    double RRight; //!< p at start of current plastic loading cycle - right side
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    InternalParamsLRIn(const double &V= 0.0,const double &r= 0.0,const double &t= 0.0,const double &yl= 0.0,const double &yr= 0.0,const double &pl= 0.0,const double &pr= 0.0);
    inline const double &yleftIn(void) const //Para PySimple1
      { return vLeft; }
    inline double &yleftIn(void) //Para PySimple1
      { return vLeft; }
    inline const double &yrightIn(void) const //Para PySimple1
      { return vRight; }
    inline double &yrightIn(void) //Para PySimple1
      { return vRight; }
    inline const double &zleftIn(void) const //Para QzSimple1
      { return vLeft; }
    inline double &zleftIn(void) //Para QzSimple1
      { return vLeft; }
    inline const double &zrightIn(void) const //Para QzSimple1
      { return vRight; }
    inline double &zrightIn(void) //Para QzSimple1
      { return vRight; }
    inline const double &PleftIn(void) const //Para PySimple1
      { return RLeft; }
    inline double &PleftIn(void) //Para PySimple1
      { return RLeft; }
    inline const double &PrightIn(void) const //Para PySimple1
      { return RRight; }
    inline double &PrightIn(void) //Para PySimple1
      { return RRight; }
    inline const double &QleftIn(void) const //Para QzSimple1
      { return RLeft; }
    inline double &QleftIn(void) //Para QzSimple1
      { return RLeft; }
    inline const double &QrightIn(void) const //Para QzSimple1
      { return RRight; }
    inline double &QrightIn(void) //Para QzSimple1
      { return RRight; }
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
  };
} // end of XC namespace


#endif
