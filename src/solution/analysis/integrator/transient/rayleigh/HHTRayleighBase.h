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
//HHTRayleighBase.h


#ifndef HHTRayleighBase_h
#define HHTRayleighBase_h

#include <solution/analysis/integrator/transient/RayleighBase.h>
#include "solution/analysis/integrator/transient/ResponseQuantities.h"

namespace XC {

//! @ingroup RayleighIntegrator
//
//! @brief HHTRayleighBase is a base for the
//! classes performing a transient analysis
//! using the Hilber-Hughes-Taylor integration scheme.
class HHTRayleighBase: public RayleighBase
  {
  protected:
    double alpha; //!< \alpha should be between 0.67 and 1.0
                  //! (alpha factor corresponds to Newmark method. 
    double gamma; //!< \gamma factor.
    
    double c2, c3;              // some constants we need to keep
    ResponseQuantities Ut; //!< response quantities at time t
    ResponseQuantities U; //!< response quantities at time t + delta t
    ResponseQuantities Ualpha; //!< response quantities at time t+alpha delta t

    int sendData(Communicator &);
    int recvData(const Communicator &);

    HHTRayleighBase(SolutionStrategy *,int classTag);
    HHTRayleighBase(SolutionStrategy *,int classTag,double alpha);
    HHTRayleighBase(SolutionStrategy *,int classTag,double alpha,const RayleighDampingFactors &rF);
    HHTRayleighBase(SolutionStrategy *,int classTag,double alpha, double gamma);
    HHTRayleighBase(SolutionStrategy *,int classTag,double alpha, double gamma,const RayleighDampingFactors &rF);
    
  };
} // end of XC namespace

#endif
