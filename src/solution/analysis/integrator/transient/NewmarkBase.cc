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
//NewmarkBase.cpp

#include <solution/analysis/integrator/transient/NewmarkBase.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Default constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
XC::NewmarkBase::NewmarkBase(AnalysisAggregation *owr,int classTag)
 : DampingFactorsIntegrator(owr,classTag),gamma(0.0), c2(0.0), c3(0.0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param _gamma: gamma factor for Newmark method.
XC::NewmarkBase::NewmarkBase(AnalysisAggregation *owr,int classTag, double _gamma)
 : DampingFactorsIntegrator(owr,INTEGRATOR_TAGS_Newmark), gamma(_gamma), c2(0.0), c3(0.0) {}

//! @brief Constructor.
//!
//! @param owr: analysis aggregation that will own this object.
//! @param classTag: class identifier.
//! @param _gamma: gamma factor for Newmark method.
//! @param fF: Rayleigh damping factors.
XC::NewmarkBase::NewmarkBase(AnalysisAggregation *owr,int classTag,double _gamma,const RayleighDampingFactors &rF)
  : DampingFactorsIntegrator(owr,classTag,rF),  gamma(_gamma),  c2(0.0), c3(0.0) {}

//! @brief Populate U, Udot and Udotdot by iterating through the DOF_Groups and 
//! getting the last committed displacement, velocity and accel
void XC::NewmarkBase::PopulateUs(AnalysisModel *model)
  {
    DOF_GrpIter &theDOFGroups = model->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr = theDOFGroups()) != 0)
      {
        const ID &id = dofGroupPtr->getID();
        
        const Vector &disp = dofGroupPtr->getCommittedDisp();	
        U.setDisp(id,disp);
        const Vector &vel = dofGroupPtr->getCommittedVel();
        U.setVel(id,vel);
        const Vector &accel = dofGroupPtr->getCommittedAccel();
        U.setAccel(id,accel);
        // NOTE WE CAN't DO TOGETHER BECAUSE DOF_GROUPS USING SINGLE VECTOR
//         for (int i=0; i < id.Size(); i++) {
//           int loc = id(i);
//           if (loc >= 0) {
//  	    (*U)(loc) = disp(i);		
//   	    (U.getDot())(loc) = vel(i);
//  	    (U.getDotDot())(loc) = accel(i);
//           }
//         }

      }
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::NewmarkBase::sendData(Communicator &comm)
  {
    int res= DampingFactorsIntegrator::sendData(comm);
    res+= comm.sendDoubles(gamma,c2,c3,getDbTagData(),CommMetaData(3));
    res+= comm.sendMovable(U,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::NewmarkBase::recvData(const Communicator &comm)
  {
    int res= DampingFactorsIntegrator::recvData(comm);
    res+= comm.receiveDoubles(gamma,c2,c3,getDbTagData(),CommMetaData(3));
    res+= comm.receiveMovable(U,getDbTagData(),CommMetaData(4));
    return res;
  }

