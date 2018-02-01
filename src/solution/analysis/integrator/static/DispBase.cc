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
//DisplacementControl.cpp

#include <solution/analysis/integrator/static/DispBase.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <utility/matrix/ID.h>

#include "utility/actor/actor/ArrayCommMetaData.h"

//! @brief Constructor.
//!
//! @param owr: set of objects used to perform the analysis.
//! @param classTag: class identifier.
//! @param numIncr: number of increments.
XC::DispBase::DispBase(AnalysisAggregation *owr,int classTag,const int &numIncr) 
  :BaseControl(owr,classTag,numIncr){}

//! @brief Send object members through the channel being passed as parameter.
int XC::DispBase::sendData(CommParameters &cp)
  {
    int res= BaseControl::sendData(cp);
    res+= cp.sendMovable(vectors,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::DispBase::recvData(const CommParameters &cp)
  {
    int res= BaseControl::recvData(cp);
    res+= cp.receiveMovable(vectors,getDbTagData(),CommMetaData(1));
    return res;
  }

