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
** (C) Copyright 1999, The Regents of the University of California    **
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
** ****************************************************************** */

// $Revision: 1.6 $
// $Date: 2005/11/29 22:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/PenaltyConstraintHandler.cpp,v $


// Written: fmk
// Created: May 1998
// Revision: A
//
// What: "@(#) PenaltyConstraintHandler.C, revA"

#include <solution/analysis/handler/PenaltyConstraintHandler.h>
#include <cstdlib>
#include <solution/analysis/model/AnalysisModel.h>
#include <domain/domain/Domain.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <domain/mesh/node/Node.h>
#include <domain/mesh/element/Element.h>
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/element/ElementIter.h"
#include <domain/constraints/SP_ConstraintIter.h>
#include <domain/constraints/SP_Constraint.h>
#include <domain/constraints/MP_ConstraintIter.h>
#include <domain/constraints/MP_Constraint.h>
#include <domain/constraints/MRMP_ConstraintIter.h>
#include <domain/constraints/MRMP_Constraint.h>
#include <solution/analysis/integrator/Integrator.h>
#include <utility/matrix/ID.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/fe_ele/penalty/PenaltySP_FE.h>
#include <solution/analysis/model/fe_ele/penalty/PenaltyMP_FE.h>
#include <solution/analysis/model/fe_ele/penalty/PenaltyMRMP_FE.h>


//! @brief Constructor.
XC::PenaltyConstraintHandler::PenaltyConstraintHandler(ModelWrapper *owr,const double &sp, const double &mp)
  :FactorsConstraintHandler(owr,HANDLER_TAG_PenaltyConstraintHandler,sp,mp) {}

//! @brief Constructor virtual.
XC::ConstraintHandler *XC::PenaltyConstraintHandler::getCopy(void) const
  { return new PenaltyConstraintHandler(*this); }

int XC::PenaltyConstraintHandler::handle(const ID *nodesLast)
  {
    // first check links exist to a XC::Domain and an XC::AnalysisModel object
    Domain *theDomain = this->getDomainPtr();
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    Integrator *theIntegrator = this->getIntegratorPtr();

    if((theDomain == 0) || (theModel == 0) || (theIntegrator == 0))
      {
        std::cerr << "WARNING PenaltyConstraintHandler::handle() - ";
        std::cerr << " no se ha asignado dominio, modelo o integrador.\n";
        return -1;
      }


    // get number ofelements and nodes in the domain
    // and init the theFEs and theDOFs arrays
    int numSPs = 0;
    SP_ConstraintIter &theSPs = theDomain->getConstraints().getDomainAndLoadPatternSPs();
    SP_Constraint *spPtr;
    while((spPtr = theSPs()) != 0)
      numSPs++;

    // initialse the DOF_Groups and add them to the XC::AnalysisModel.
    //    : must of course set the initial IDs
    NodeIter &theNod = theDomain->getNodes();
    Node *nodPtr;
    DOF_Group *dofPtr;

    int numDofGrp = 0;
    int count3 = 0;
    int countDOF =0;
    while((nodPtr = theNod()) != 0)
      {
        dofPtr= theModel->createDOF_Group(numDofGrp++, nodPtr);
        // initially set all the ID value to -2
        countDOF+= dofPtr->inicID(-2);
      }

    theModel->setNumEqn(countDOF);

    // set the number of eqn in the model
    // now see if we have to set any of the dof's to -3
    //    int numLast = 0;
    if(nodesLast != 0)
      for(int i=0; i<nodesLast->Size(); i++)
        {
          int nodeID = (*nodesLast)(i);
          Node *nodPtr = theDomain->getNode(nodeID);
          if(nodPtr != 0)
            {
              dofPtr = nodPtr->getDOF_GroupPtr();

              const ID &id = dofPtr->getID();
              // set all the dof values to -3
              for(int j=0; j < id.Size(); j++)
                if(id(j) == -2)
                  {
                    dofPtr->setID(j,-3);
                    count3++;
                  }
                else
                  {
                    std::cerr << "WARNING XC::PenaltyConstraintHandler::handle() ";
                    std::cerr << " - boundary sp constraint in subdomain";
                    std::cerr << " this should not be - results suspect \n";
                  }
            }
        }

    // create the FE_Elements for the Elements and add to the XC::AnalysisModel
    ElementIter &theEle = theDomain->getElements();
    Element *elePtr;

    int numFeEle = 0;
    FE_Element *fePtr;
    while((elePtr = theEle()) != 0)
      { fePtr= theModel->createFE_Element(numFeEle++, elePtr); }

    // create the PenaltySP_FE for the SP_Constraints and
    // add to the AnalysisModel
    SP_ConstraintIter &theSPss = theDomain->getConstraints().getDomainAndLoadPatternSPs();
    while((spPtr = theSPss()) != 0)
      {
        fePtr= theModel->createPenaltySP_FE(numFeEle, *spPtr, alphaSP);
        numFeEle++;
      }

    // create the PenaltyMP_FE for the MP_Constraints and
    // add to the AnalysisModel
    MP_ConstraintIter &theMPs = theDomain->getConstraints().getMPs();
    MP_Constraint *mpPtr;
    while((mpPtr = theMPs()) != 0)
      {
	fePtr= theModel->createPenaltyMP_FE(numFeEle, *mpPtr, alphaMP);
        numFeEle++;
      }

    // create the PenaltyMRMP_FE for the MRMP_Constraints and
    // add to the AnalysisModel
    MRMP_ConstraintIter &theMRMPs = theDomain->getConstraints().getMRMPs();
    MRMP_Constraint *mrmpPtr;
    while((mrmpPtr = theMRMPs()) != 0)
      {
	fePtr= theModel->createPenaltyMRMP_FE(numFeEle, *mrmpPtr, alphaMP);
        numFeEle++;
      }
    return count3;
  }


