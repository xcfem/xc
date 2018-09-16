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
//PQyzBase.h

#ifndef PQYZBASE_H
#define PQYZBASE_H

#include "PYBase.h"

namespace XC {
//! @ingroup PYMat
//
//! @brief Base class for PySimple1 y QzSimple1.
class PQyzBase: public PYBase
  {
  protected:
    // Generated parameters or constants (not user input)
    double NFkrig; //!< stiffness of the "rigid" portion of Near Field spring
    
    InternalParamsLRIn CNF; //!< Committed internal parameters for the NearField rigid-plastic component
    InternalParamsLRIn TNF; //!< Trial internal parameters for the NearField rigid-plastic component
    
    InternalParamsA CGap; //!< Committed internal parameters for the Gap (Drag + Closure)
    InternalParamsA TGap; //!< Trial internal parameters for the Gap (Drag + Closure) 

    // Material parameters
    double Elast; //!< (p/matCapacity) when yielding first occurs in virgin loading
    double nd; //!< exponent for hardening shape of drag component

    int sendData(CommParameters &);
    int recvData(const CommParameters &);
  public:
    PQyzBase(int tag, int classtag, int soilType, double rult, double v50,double dashpot);
    PQyzBase(int tag, int classtag);
    PQyzBase(void);

    int commitState(void);
    int revertToLastCommit(void);

  };
} // end of XC namespace


#endif
