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
//NewtonBased.h                                                                        
                                                                        
#ifndef NewtonBased_h
#define NewtonBased_h

#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include "solution/analysis/integrator/IncrementalIntegrator.h"

namespace XC {
class ConvergenceTest;

//! @ingroup EQSolAlgo
//
//! @brief Base class for algorithms based on Newton method.
//
//! Those method use  the tangent stiffness matrix on each
//! iteration (with or without updating) until convergence is achieved.
class NewtonBased: public EquiSolnAlgo
  {
  protected:
    int tangent;
    int sendData(Communicator &);
    int recvData(const Communicator &);

    NewtonBased(AnalysisAggregation *,int classTag,int tangent = CURRENT_TANGENT);
  public:
    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif


