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
//ProtoArcLength.h

#ifndef ProtoArcLength_h
#define ProtoArcLength_h

#include "solution/analysis/integrator/StaticIntegrator.h"
#include "IntegratorVectors.h"
#include "utility/matrix/Vector.h"

namespace XC {
class LinearSOE;
class AnalysisModel;
class FE_Element;

//! @ingroup StaticIntegrator
//
//! @brief Base class for arc length integrators.
class ProtoArcLength: public StaticIntegrator
  {
  protected:
    double arcLength2; //!< Squared value of the arc length.
    IntegratorVectors vectors;
    int signLastDeltaLambdaStep;

    virtual double getDLambdaNewStep(void) const= 0;
    virtual double getDLambdaUpdate(void) const= 0;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    ProtoArcLength(AnalysisAggregation *,int classTag,double ArcLength);
  public:

    int newStep(void);
    int update(const Vector &deltaU);
    int domainChanged(void);

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif

