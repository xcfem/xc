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
//NewmarkBase.h

#ifndef NewmarkBase_h
#define NewmarkBase_h

#include <solution/analysis/integrator/transient/DampingFactorsIntegrator.h>
#include "solution/analysis/integrator/transient/ResponseQuantities.h"

namespace XC {
class Vector;
class ID;

//! @addtogroup NewmarkIntegrator Newmark method for the numerical integration of the equation.
//! @ingroup TransientIntegrator
//
//! @brief Base class for the two parameter time-stepping method developed by
//! Nathan M. Newmark.
class NewmarkBase: public DampingFactorsIntegrator
  {
  protected:
    double gamma; //!< gamma factor for Newmark method.
    
    double c2, c3; //!< some constants we need to keep
    ResponseQuantities U; //!< response quantities at time t+deltaT = predicted + corrected

    void PopulateUs(XC::AnalysisModel *model);
    int sendData(Communicator &);
    int recvData(const Communicator &);

    NewmarkBase(SolutionStrategy *,int classTag);
    NewmarkBase(SolutionStrategy *,int classTag,double gamma);
    NewmarkBase(SolutionStrategy *,int classTag,double gamma,const RayleighDampingFactors &rF);
  };
} // end of XC namespace

#endif
