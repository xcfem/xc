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
//HHTBase.h,v $


#ifndef HHTBase_h
#define HHTBase_h

// Written: Andreas Schellenberg (andreas.schellenberg@gmx.net)
// Created: 02/05
// Revision: A
//
// Description: This file contains the class definition for HHTBase.
// HHTBase is an algorithmic class for performing a transient analysis
// using the HHTBase integration scheme.
//
// What: "@(#) HHTBase.h, revA"

#include "solution/analysis/integrator/transient/rayleigh/HHTRayleighBase.h"

namespace XC {

//! @ingroup RayleighIntegrator
//
//! @brief HHTBase is an algorithmic class
//! for performing a transient analysis
//! using the HHTBase integration scheme.
class HHTBase: public HHTRayleighBase
  {
  protected:
    double beta; //! \beta parameter
    double c1;   //!< some constants we need to keep

    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    HHTBase(AnalysisAggregation *,int classTag);
    HHTBase(AnalysisAggregation *,int classTag,double alpha);
    HHTBase(AnalysisAggregation *,int classTag,double alpha,const RayleighDampingFactors &rF);
    HHTBase(AnalysisAggregation *,int classTag,double alpha, double beta, double gamma);
    HHTBase(AnalysisAggregation *,int classTag,double alpha, double beta, double gamma,const RayleighDampingFactors &rF);    
  };
} // end of XC namespace

#endif
