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
//CentralDifferenceBase.h
                                                                        
#ifndef CentralDifferenceBase_h
#define CentralDifferenceBase_h

#include "solution/analysis/integrator/TransientIntegrator.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup TransientIntegrator
//
//! @brief Approximates velocity and acceleration by centered finite differences of displacement.
class CentralDifferenceBase : public TransientIntegrator
  {
  protected:
    int updateCount; //!< method should only have one update per step
    Vector Udot; //!< vel response quantity at time t-1/2 delta t
    double deltaT;

    CentralDifferenceBase(AnalysisAggregation *,int classTag);
  public:
    // methods which define what the FE_Element and DOF_Groups add
    // to the system of equation object.
    int formEleTangent(FE_Element *theEle);
    int formNodTangent(DOF_Group *theDof);

    inline const Vector &getVel(void) const
      { return Udot; }
    int newStep(double deltaT);
  };
} // end of XC namespace

#endif

