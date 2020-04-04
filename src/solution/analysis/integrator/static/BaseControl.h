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
//BaseControl.h                                                      
                                                                        
#ifndef BaseControl_h
#define BaseControl_h

#include <solution/analysis/integrator/StaticIntegrator.h>

namespace XC {

//! @ingroup StaticIntegrator
//
//! @brief Base class for load and displacement control static integrators .
class BaseControl: public StaticIntegrator
  {
  protected:
    double specNumIncrStep; //!< Jd factor relating load increment at subsequent time steps. (optional, default: 1.0)
    double numIncrLastStep; //!< J(i-1). 

    int sendData(Communicator &);
    int recvData(const Communicator &);

    void setup_numIncr(const int &);

    BaseControl(AnalysisAggregation *,int classTag,int numIncr= 1);
  public:

    inline double factor(void) const
      { return specNumIncrStep/numIncrLastStep; }
  };
} // end of XC namespace

#endif

