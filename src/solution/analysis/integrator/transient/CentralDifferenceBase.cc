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
//CentralDifferenceBase.cc

#include <solution/analysis/integrator/transient/CentralDifferenceBase.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

//! @brief Constructor.
XC::CentralDifferenceBase::CentralDifferenceBase(SoluMethod *owr,int classTag)
:TransientIntegrator(owr,classTag), updateCount(0), deltaT(0) {}

int XC::CentralDifferenceBase::newStep(double _deltaT)
  {
    updateCount = 0;
    deltaT = _deltaT;

    if(deltaT <= 0.0)
      {
        std::cerr << "XC::CentralDifferenceBase::newStep() - error in variable\n";
        std::cerr << "dT = " << deltaT << std::endl;
        return -2;	
      }

    //AnalysisModel *theModel = this->getAnalysisModelPtr();
    double time = getCurrentModelTime();
    applyLoadModel(time);
    return 0;
  }

int XC::CentralDifferenceBase::formEleTangent(FE_Element *theEle)
  {
    theEle->zeroTangent();
    theEle->addMtoTang();
    return 0;
  }    

int XC::CentralDifferenceBase::formNodTangent(DOF_Group *theDof)
  {
    theDof->zeroTangent();
    theDof->addMtoTang();
    return(0);
  }


