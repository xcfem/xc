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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/10/27 23:05:30 $
// $Source: /usr/local/cvs/OpenSees/SRC/reliability/FEsensitivity/NewmarkSensitivityIntegrator.cpp,v $


//
// Written by Terje Haukaas (haukaas@ce.berkeley.edu)
//

#include <reliability/FEsensitivity/SensitivityIntegrator.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <reliability/FEsensitivity/NewmarkSensitivityIntegrator.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/mesh/element/utils/Information.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <domain/load/pattern/LoadPattern.h>
#include <domain/domain/Domain.h>
#include <domain/load/pattern/LoadPatternIter.h>
#include <domain/mesh/node/Node.h>
#include "domain/mesh/node/NodeIter.h"
#include "solution/system_of_eqn/linearSOE/LinearSOE.h"
#include "domain/component/Parameter.h"


XC::NewmarkSensitivityIntegrator::NewmarkSensitivityIntegrator(SolutionStrategy *owr)
:SensitivityIntegrator(),Newmark(owr),parameterID(0),sensitivityFlag(0),gradNumber(0)
  { assemblyFlag = 0; }

XC::NewmarkSensitivityIntegrator::NewmarkSensitivityIntegrator(SolutionStrategy *owr,int passedAssemblyFlag, double theGamma, double theBeta, bool dispFlag)
  :SensitivityIntegrator(),Newmark(owr,theGamma, theBeta, dispFlag),
parameterID(0),sensitivityFlag(0),gradNumber(0)
  { assemblyFlag = passedAssemblyFlag; }

XC::NewmarkSensitivityIntegrator::NewmarkSensitivityIntegrator(SolutionStrategy *owr,int passedAssemblyFlag, double theGamma, double theBeta, const RayleighDampingFactors &rF,bool dispFlag)
  :SensitivityIntegrator(),Newmark(owr,theGamma, theBeta, rF, dispFlag), 
parameterID(0),sensitivityFlag(0),gradNumber(0)
  { assemblyFlag = passedAssemblyFlag; }

int XC::NewmarkSensitivityIntegrator::formEleResidual(FE_Element *theEle)
  {

    if(sensitivityFlag == 0)
      {  // NO SENSITIVITY ANALYSIS
            this->XC::Newmark::formEleResidual(theEle);
      }
    else
      {  // (ASSEMBLE ALL TERMS)

            theEle->zeroResidual();

            // Compute the time-stepping parameters on the form
            // udotdot = a1*ui+1 + a2*ui + a3*udoti + a4*udotdoti
            // udot    = a5*ui+1 + a6*ui + a7*udoti + a8*udotdoti
            // (see p. 166 of Chopra)

            // The constants are:
            // a1 = 1.0/(beta*dt*dt)
            // a2 = -1.0/(beta*dt*dt)
            // a3 = -1.0/beta*dt
            // a4 = 1.0 - 1.0/(2.0*beta)
            // a5 = gamma/(beta*dt)
            // a6 = -gamma/(beta*dt)
            // a7 = 1.0 - gamma/beta
            // a8 = 1.0 - gamma/(2.0*beta)

            // We can make use of the data members c2 and c3 of this class. 
            // As long as disp==true, they are defined as:
            // c2 = gamma/(beta*dt)
            // c3 = 1.0/(beta*dt*dt)

            // So, the constants can be computed as follows:
            if(displ==false)
              {
                std::cerr << "ERROR: XC::Newmark::formEleResidual() -- the implemented"
                    << " scheme only works if the displ variable is set to true." << std::endl;
              }
            double a2 = -c3;
            double a3 = -c2/gamma;
            double a4 = 1.0 - 1.0/(2.0*beta);
            double a6 = -c2;
            double a7 = 1.0 - gamma/beta;
            double dt = gamma/(beta*c2);
            double a8 = dt*(1.0 - gamma/(2.0*beta));


            // Obtain sensitivity vectors from previous step
            int vectorSize = U.get().Size();
            Vector V(vectorSize);
            Vector Vdot(vectorSize);
            Vector Vdotdot(vectorSize);
            int i, loc;

            AnalysisModel *myModel = this->getAnalysisModelPtr();
            DOF_GrpIter &theDOFGroups = myModel->getDOFGroups();
            DOF_Group *dofGroupPtr= nullptr;
            while((dofGroupPtr = theDOFGroups()) != 0)
              {
                const ID &id = dofGroupPtr->getID();
                int idSize = id.Size();
                const Vector &dispSens = dofGroupPtr->getDispSensitivity(gradNumber);        
                for(i=0; i < idSize; i++)
                  {
                    loc = id(i);
                    if(loc >= 0)
                      { V(loc) = dispSens(i); }
                  }

                const Vector &velSens = dofGroupPtr->getVelSensitivity(gradNumber);
                for(i=0; i < idSize; i++)
                  {
                    loc = id(i);
                    if(loc >= 0) 
                      { Vdot(loc) = velSens(i); }
                  }

                const Vector &accelSens = dofGroupPtr->getAccSensitivity(gradNumber);        
                for(i=0; i < idSize; i++)
                  {
                    loc = id(i);
                    if(loc >= 0)
                      { Vdotdot(loc) = accelSens(i); }
                  }
            }


            // Pre-compute the vectors involving a2, a3, etc.
            Vector tmp1= V*a2 + Vdot*a3 + Vdotdot*a4;
            Vector tmp2= V*a6 + Vdot*a7 + Vdotdot*a8;

            massMatrixMultiplicator= tmp1;
            dampingMatrixMultiplicator= tmp2;


            // Now we're ready to make calls to the FE XC::Element:

            // The term -dPint/dh|u fixed
            theEle->addResistingForceSensitivity(gradNumber); 

            // The term -dM/dh*acc
            theEle->addM_ForceSensitivity(gradNumber, U.getDotDot(), -1.0);

            // The term -M*(a2*v + a3*vdot + a4*vdotdot)
            theEle->addM_Force(massMatrixMultiplicator,-1.0);

            // The term -C*(a6*v + a7*vdot + a8*vdotdot)
            theEle->addD_Force(dampingMatrixMultiplicator,-1.0);

            // The term -dC/dh*vel
            theEle->addD_ForceSensitivity(gradNumber, U.getDot(),-1.0);
            
    }

    return 0;
}



int XC::NewmarkSensitivityIntegrator::formNodUnbalance(DOF_Group *theDof)
  {

    if(sensitivityFlag == 0) {  // NO SENSITIVITY ANALYSIS

            this->Newmark::formNodUnbalance(theDof);

    }
    else {  // ASSEMBLE ALL TERMS

            theDof->zeroUnbalance();


            // The term -M*(a2*v + a3*vdot + a4*vdotdot)
            theDof->addM_Force(massMatrixMultiplicator,-1.0);


            // The term -dM/dh*acc
            theDof->addM_ForceSensitivity(U.getDotDot(), -1.0);


            // The term -C*(a6*v + a7*vdot + a8*vdotdot)
            theDof->addD_Force(dampingMatrixMultiplicator,-1.0);


            // The term -dC/dh*vel
            theDof->addD_ForceSensitivity(U.getDot(),-1.0);


            // In case of random loads (have already been formed by 'applyLoadSensitivity')
            theDof->addPtoUnbalance();

    }


    return 0;
}


int XC::NewmarkSensitivityIntegrator::formSensitivityRHS(int passedGradNumber)
  {
    sensitivityFlag = 1;


    // Set a couple of data members
    gradNumber = passedGradNumber;

    // Get pointer to the SOE
    LinearSOE *theSOE = this->getLinearSOEPtr();


    // Possibly set the independent part of the RHS
    if(assemblyFlag != 0)
      {        theSOE->setB(independentRHS); }

    // Get the analysis model
    AnalysisModel *theModel= this->getAnalysisModelPtr();



    // Randomness in external load (including randomness in time series)
    // Get domain
    Domain *theDomain = theModel->getDomainPtr();

    // Loop through nodes to zero the unbalaced load
    Node *nodePtr;
    NodeIter &theNodeIter = theDomain->getNodes();
    while((nodePtr = theNodeIter()) != 0)
    nodePtr->zeroUnbalancedLoad();


    // Loop through load patterns to add external load sensitivity
    double time;
    std::map<int,LoadPattern *> &theLoadPatterns = theDomain->getConstraints().getLoadPatterns();
    for(std::map<int,LoadPattern *>::iterator loadPatternPtr= theLoadPatterns.begin();
        loadPatternPtr!= theLoadPatterns.end();loadPatternPtr++)
      {
            time = theDomain->getTimeTracker().getCurrentTime();
            loadPatternPtr->second->applyLoadSensitivity(time);
      }


    // Randomness in element/material contributions
    // Loop through FE elements
    FE_Element *elePtr;
    FE_EleIter &theEles = theModel->getFEs();    
    while((elePtr = theEles()) != 0)
      {        theSOE->addB(  elePtr->getResidual(this),  elePtr->getID()); }


    // Loop through DOF groups (IT IS IMPORTANT THAT THIS IS DONE LAST!)
    DOF_Group *dofGroupPtr;
    DOF_GrpIter &theDOFGroups = theModel->getDOFGroups();
    while((dofGroupPtr = theDOFGroups()) != 0)
      {
        theSOE->addB(  dofGroupPtr->getUnbalance(this),  dofGroupPtr->getID()  );
      }


    // Reset the sensitivity flag
    sensitivityFlag = 0;

    return 0;
}
            





int 
XC::NewmarkSensitivityIntegrator::formIndependentSensitivityRHS()
{
    // For now; don't use this
/*
    sensitivityFlag = 2; // Tell subsequent methods what to be assembled

    // Get pointer to the SOE
    LinearSOE *theSOE = this->getLinearSOEPtr();


    // Get the analysis model
    AnalysisModel *theModel = this->getAnalysisModelPtr();

    
    // Loop through FE elements
    FE_Element *elePtr;
    FE_EleIter &theEles = theModel->getFEs();    
    while((elePtr = theEles()) != 0) {
            theSOE->addB(  elePtr->getResidual(this),  elePtr->getID()  );
    }


    // Loop through DOF groups (IT IS IMPORTANT THAT THIS IS DONE LAST!)
    DOF_Group *dofGroupPtr;
    DOF_GrpIter &theDOFGroups = theModel->getDOFGroups();
    while((dofGroupPtr = theDOFGroups()) != 0)
      {
        theSOE->addB(  dofGroupPtr->getUnbalance(this),  dofGroupPtr->getID()  );
      }


    // Set the data member of this class
    independentRHS = theSOE->getB();


    // Reset the sensitivity flag
    sensitivityFlag = 0;
*/

    return 0;
}
            




int XC::NewmarkSensitivityIntegrator::saveSensitivity(const XC::Vector & vNew,int gradNum,int numGrads)
  {

    // Compute XC::Newmark parameters in general notation
    double a1 = c3;
    double a2 = -c3;
    double a3 = -c2/gamma;
    double a4 = 1.0 - 1.0/(2.0*beta);
    double a5 = c2;
    double a6 = -c2;
    double a7 = 1.0 - gamma/beta;
    double dt = gamma/(beta*c2);
    double a8 = dt*(1.0 - gamma/(2.0*beta));


    // Recover sensitivity results from previous step
    int vectorSize = U.get().Size();
    Vector V(vectorSize);
    Vector Vdot(vectorSize);
    Vector Vdotdot(vectorSize);
    int i, loc;

    AnalysisModel *myModel = this->getAnalysisModelPtr();
    DOF_GrpIter &theDOFs = myModel->getDOFGroups();
    DOF_Group *dofGroupPtr;
    while((dofGroupPtr = theDOFs()) != 0) {

            const ID &id = dofGroupPtr->getID();
            int idSize = id.Size();
            const Vector &dispSens = dofGroupPtr->getDispSensitivity(gradNumber);        
            for(i=0; i < idSize; i++)
              {
                    loc = id(i);
                    if(loc >= 0) {
                            V(loc) = dispSens(i);                
                    }
            }

            const XC::Vector &velSens = dofGroupPtr->getVelSensitivity(gradNumber);
            for(i=0; i < idSize; i++) {
                    loc = id(i);
                    if(loc >= 0) {
                            Vdot(loc) = velSens(i);
                    }
            }

            const XC::Vector &accelSens = dofGroupPtr->getAccSensitivity(gradNumber);        
            for(i=0; i < idSize; i++)
          {
                loc = id(i);
                if(loc >= 0)
              {        Vdotdot(loc) = accelSens(i); }
              }
    }


    // Compute new acceleration and velocity vectors:
    Vector *vNewPtr = new Vector(vectorSize);
    Vector *vdotNewPtr = new Vector(vectorSize);
    Vector *vdotdotNewPtr = new Vector(vectorSize);
    (*vdotdotNewPtr) = vNew*a1 + V*a2 + Vdot*a3 + Vdotdot*a4;
    (*vdotNewPtr) = vNew*a5 + V*a6 + Vdot*a7 + Vdotdot*a8;
    (*vNewPtr) = vNew;


    // Now we can save vNew, vdotNew and vdotdotNew
    DOF_GrpIter &theDOFGrps = myModel->getDOFGroups();
    DOF_Group *dofGroupPtr1= nullptr;
    while( (dofGroupPtr1 = theDOFGrps() ) != 0)
      {  dofGroupPtr1->saveSensitivity(vNewPtr,vdotNewPtr,vdotdotNewPtr,gradNum,numGrads); }

    delete vNewPtr;
    delete vdotNewPtr;
    delete vdotdotNewPtr;

    return 0;
  }



int XC::NewmarkSensitivityIntegrator::commitSensitivity(int gradNum, int numGrads)
  {

    // Loop through the FE_Elements and set unconditional sensitivities
    AnalysisModel *theAnalysisModel = this->getAnalysisModelPtr();
    FE_Element *elePtr;
    FE_EleIter &theEles = theAnalysisModel->getFEs();    
    while((elePtr = theEles()) != 0)
      {        elePtr->commitSensitivity(gradNum, numGrads); }
    return 0;
  }




int XC::NewmarkSensitivityIntegrator::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv[0] == "alphaM")
      return param.addObject(1, this);
    else if(argv[0] == "betaK")
      return param.addObject(1, this);
    // otherwise parameter is unknown
    else
      {
        std::cerr << "ERROR: Unknown random parameter in XC::Newmark::setParameter()" << std::endl;
        return -1;
      }
  }

int XC::NewmarkSensitivityIntegrator::updateParameter(int parameterID, Information &info)
  { return rayFactors.updateParameter(parameterID,info); }

int XC::NewmarkSensitivityIntegrator::activateParameter (int passedParameterID)
  {
    parameterID = passedParameterID;
    return 0;
  }









