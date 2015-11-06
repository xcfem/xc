//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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


XC::StaticSensitivityIntegrator::StaticSensitivityIntegrator(SoluMethod *owr)
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
                const XC::ID &anID = aDofGroup->getID();
                relevantID = anID(dofNumber-1);
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

int XC::StaticSensitivityIntegrator::sendSelf(CommParameters &)
  { return 0; }

int XC::StaticSensitivityIntegrator::recvSelf(const CommParameters &)
  { return 0; }

void XC::StaticSensitivityIntegrator::Print(std::ostream &s, int flag)  
  {}





