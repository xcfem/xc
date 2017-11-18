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
//InternalParamsLR.h

#ifndef INTERNALPARAMSLR_H
#define INTERNALPARAMSLR_H

#include <material/uniaxial/PY/InternalParamsA.h>

namespace XC {
//! @ingroup PYMat
//
//! @brief Internal parameters for a p-y material.
class InternalParamsLR: public InternalParamsA
  {
  protected:
    double vLeft; //!< left reference point
    double vRight; //!< right reference point
    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    InternalParamsLR(const double &V= 0.0,const double &r= 0.0,const double &t= 0.0,const double &yl= 0.0,const double &yr= 0.0);
    inline const double &yleft(void) const // for PySimple1
      { return vLeft; }
    inline double &yleft(void) // for PySimple1
      { return vLeft; }
    inline const double &yright(void) const // for PySimple1
      { return vRight; }
    inline double &yright(void) // for PySimple1
      { return vRight; }
    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace


#endif
