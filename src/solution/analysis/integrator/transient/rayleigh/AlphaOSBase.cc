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
//AlphaOSBase.cpp

#include <solution/analysis/integrator/transient/rayleigh/AlphaOSBase.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/FE_EleIter.h>

//! @brief Constructor.
XC::AlphaOSBase::AlphaOSBase(SolutionStrategy *owr,int classTag)
    : HHTBase(owr,classTag) {}

//! @brief Constructor.
XC::AlphaOSBase::AlphaOSBase(SolutionStrategy *owr,int classTag, double _beta, double _gamma)
  : HHTBase(owr,classTag,0,_beta,_gamma) {}

//! @brief Constructor.
XC::AlphaOSBase::AlphaOSBase(SolutionStrategy *owr,int classTag, double _beta, double _gamma,const RayleighDampingFactors &rF)
  : HHTBase(owr,classTag,0,_beta,_gamma,rF) {}

int XC::AlphaOSBase::newStep(double _deltaT)
  {
    updateCount = 0;

    deltaT = _deltaT;
    if(beta == 0 || gamma == 0 )
      {
        std::cerr << "XC::AlphaOSBase::newStep() - error in variable\n";
        std::cerr << "gamma = " << gamma << " beta = " << beta << std::endl;
        return -1;
     }
    
    if (deltaT <= 0.0)  {
        std::cerr << "XC::AlphaOSBase::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << "\n";
        return -2;	
    }
    
    // set the constants
    c1 = 1.0;
    c2 = gamma/(beta*deltaT);
    c3 = 1.0/(beta*deltaT*deltaT);
    
    if(U.get().Size() == 0)
      {
        std::cerr << "XC::AlphaOSBase::newStep() - domainChange() failed or hasn't been called\n";
        return -3;	
      }
    
    // set response at t to be that at t+deltaT of previous step
    Ut= U;  
    
    // determine new_ displacements and velocities at time t+deltaT
    U.get().addVector(1.0, Ut.getDot(), deltaT);
    const double a1 = (0.5 - beta)*deltaT*deltaT;
    U.get().addVector(1.0, Ut.getDotDot(), a1);
    
    const double a2 = deltaT*(1.0 - gamma);
    U.getDot().addVector(1.0, Ut.getDotDot(), a2);
    return 0;
  }


int XC::AlphaOSBase::revertToLastStep(void)
  {
    // set response at t+deltaT to be that at t .. for next step
    if(U.get().Size()>0)
      U= Ut;
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::AlphaOSBase::sendData(Communicator &comm)
  {
    int res= HHTBase::sendData(comm);
    res+= comm.sendMovable(Upt,getDbTagData(),CommMetaData(9));
    res+= comm.sendInt(updateCount,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::AlphaOSBase::recvData(const Communicator &comm)
  {
    int res= HHTBase::recvData(comm);
    res+= comm.receiveMovable(Upt,getDbTagData(),CommMetaData(9));
    res+= comm.receiveInt(updateCount,getDbTagData(),CommMetaData(10));
    return res;
  }
