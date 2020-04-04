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
/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 2001, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** Reliability module developed by:                                   **
**   Terje Haukaas (haukaas@ce.berkeley.edu)                          **
**   Armen Der Kiureghian (adk@ce.berkeley.edu)                       **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.3 $
// $Date: 2003/03/04 00:46:02 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/FEsensitivity/StaticSensitivityIntegrator.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/FEsensitivity/SensitivityIntegrator.h>
#include <reliability/FEsensitivity/StaticSensitivityIntegrator.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <domain/load/pattern/LoadPattern.h>
#include <domain/load/pattern/LoadPatternIter.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <classTags.h>


XC::StaticSensitivityIntegrator::StaticSensitivityIntegrator(AnalysisAggregation *owr)
  : SensitivityIntegrator(), StaticIntegrator(owr,INTEGRATOR_TAGS_StaticSensitivity) {}


int XC::StaticSensitivityIntegrator::formEleResidual(FE_Element *theEle)
  {
    theEle->zeroResidual();
    theEle->addResistingForceSensitivity(gradNumber); 
    return 0;
  }


int XC::StaticSensitivityIntegrator::formIndependentSensitivityRHS()
  {
    // For now everything is done each time in the static case
    return 0;
  }


int XC::StaticSensitivityIntegrator::formSensitivityRHS(int passedGradNumber)
  {
    // Set a couple of data members
    gradNumber = passedGradNumber;


    // Loop through elements
    FE_Element *elePtr;
    AnalysisModel *mdl= getAnalysisModelPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    FE_EleIter &theEles= mdl->getFEs();    
    while((elePtr = theEles()) != 0)
      { theSOE->addB(  elePtr->getResidual(this),  elePtr->getID()  ); }

    // Loop through the loadPatterns and add the dPext/dh contributions
    Vector oneDimVectorWithOne(1);
    oneDimVectorWithOne(0) = 1.0;
    ID oneDimID(1);
    Node *aNode;
    DOF_Group *aDofGroup;
    int nodeNumber, dofNumber, relevantID, i, sizeRandomLoads, numRandomLoads;
    Domain *theDomain = mdl->getDomainPtr();
    std::map<int,LoadPattern *> &theLoadPatterns = theDomain->getConstraints().getLoadPatterns();
    for(std::map<int,LoadPattern *>::iterator loadPatternPtr= theLoadPatterns.begin();
        loadPatternPtr!= theLoadPatterns.end();loadPatternPtr++)
      {
        const Vector &randomLoads = loadPatternPtr->second->getExternalForceSensitivity(gradNumber);
        sizeRandomLoads = randomLoads.Size();
        if(sizeRandomLoads == 1)
          {
            // No random loads in this load pattern
          }
        else
          {
            // Random loads: add contributions to the 'B' vector
            numRandomLoads = (int)(sizeRandomLoads/2);
            for(i=0; i<numRandomLoads*2; i=i+2)
              {
                nodeNumber = (int)randomLoads(i);
                dofNumber = (int)randomLoads(i+1);
                aNode = theDomain->getNode(nodeNumber);
                aDofGroup = aNode->getDOF_GroupPtr();
                relevantID = aDofGroup->getID()(dofNumber-1);
                oneDimID(0) = relevantID;
                theSOE->addB(oneDimVectorWithOne, oneDimID);
              }
          }
      }
    return 0;
  }
            



int XC::StaticSensitivityIntegrator::saveSensitivity(const XC::Vector &v, int gradNum, int numGrads)
  {
    AnalysisModel *mdl= getAnalysisModelPtr();
    DOF_GrpIter &theDOFGrps = mdl->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;

    Vector vNewPtr(v);
    while( (dofGroupPtr = theDOFGrps() ) != 0)
      { dofGroupPtr->saveSensitivity(&vNewPtr,0,0,gradNum,numGrads); }
    return 0;
  }



int XC::StaticSensitivityIntegrator::commitSensitivity(int gradNum, int numGrads)
  {

    // Loop through the FE_Elements and set unconditional sensitivities
    FE_Element *elePtr= nullptr;
    FE_EleIter &theEles= getAnalysisModelPtr()->getFEs();    
    while((elePtr = theEles()) != 0)
      { elePtr->commitSensitivity(gradNum, numGrads); }
    return 0;
  }





int XC::StaticSensitivityIntegrator::newStep(void)
  { return 0; }

int XC::StaticSensitivityIntegrator::update(const XC::Vector &deltaU)
  { return 0; }

int XC::StaticSensitivityIntegrator::setDeltaLambda(double newDeltaLambda)
  { return 0; }

int XC::StaticSensitivityIntegrator::sendSelf(Communicator &)
  { return 0; }

int XC::StaticSensitivityIntegrator::recvSelf(const Communicator &)
  { return 0; }

void XC::StaticSensitivityIntegrator::Print(std::ostream &s, int flag) const 
  {}





