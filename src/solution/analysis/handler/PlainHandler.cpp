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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
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
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/PlainHandler.cpp,v $
                                                                        
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the implementation of XC::PlainHandler.
//
// What: "@(#) PlainHandler.C, revA"

#include <solution/analysis/handler/PlainHandler.h>
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
#include "utility/matrix/Matrix.h"
#include "domain/domain/subdomain/Subdomain.h"

//! @brief Constructor.
XC::PlainHandler::PlainHandler(ModelWrapper *owr)
  :ConstraintHandler(owr,HANDLER_TAG_PlainHandler) {}

//! @brief Constructor virtual.
XC::ConstraintHandler *XC::PlainHandler::getCopy(void) const
  { return new PlainHandler(*this); }

int XC::PlainHandler::handle(const ID *nodesLast)
  {
    // first check links exist to a XC::Domain and an XC::AnalysisModel object
    Domain *theDomain = this->getDomainPtr();
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    Integrator *theIntegrator = this->getIntegratorPtr();    
    
    if((theDomain == 0) || (theModel == 0) || (theIntegrator == 0))
      {
        std::cerr << "WARNING PlainHandler::handle() - ";
        std::cerr << " no se ha asignado dominio, modelo o integrador.\n";
        return -1;
      }


    // initialse the DOF_Groups and add them to the AnalysisModel.
    //    : must of course set the initial IDs
    NodeIter &theNod = theDomain->getNodes();
    Node *nodPtr= nullptr;
    SP_Constraint *spPtr= nullptr;
    DOF_Group *dofPtr= nullptr;

    int numDOF = 0;
    int count3 = 0;
    int countDOF =0;
    while((nodPtr = theNod()) != 0)
      {
        dofPtr= theModel->createDOF_Group(numDOF++, nodPtr);
        // initially set all the ID value to -2
        countDOF+= dofPtr->inicID(-2);

        // loop through the SP_Constraints to see if any of the
        // DOFs are constrained, if so set initial XC::ID value to -1
        int nodeID = nodPtr->getTag();
        SP_ConstraintIter &theSPs = theDomain->getConstraints().getDomainAndLoadPatternSPs();
        while((spPtr = theSPs()) != 0)
            if(spPtr->getNodeTag() == nodeID) {
                if(spPtr->isHomogeneous() == false) {
                    std::cerr << "WARNING XC::PlainHandler::handle() - ";
                    std::cerr << " non-homogeneos constraint";
                    std::cerr << " for node " << spPtr->getNodeTag();
                    std::cerr << " homo assumed\n";
                }
                const ID &id = dofPtr->getID();
                int dof = spPtr->getDOF_Number();                
                if(id(dof) == -2) {
                        dofPtr->setID(spPtr->getDOF_Number(),-1);
                        countDOF--;        
                } else {
                    std::cerr << "WARNING XC::PlainHandler::handle() - ";
                    std::cerr << " multiple single pointconstraints at DOF " << dof;
                    std::cerr << " for node " << spPtr->getNodeTag() << std::endl;
                }
            }

        // loop through the MP_Constraints to see if any of the
        // DOFs are constrained, note constraint matrix must be diagonal
        // with 1's on the diagonal
        MP_ConstraintIter &theMPs = theDomain->getConstraints().getMPs();
        MP_Constraint *mpPtr;
        while((mpPtr = theMPs()) != 0)
          {
            if(mpPtr->getNodeConstrained() == nodeID)
              {
                if(mpPtr->isTimeVarying() == true)
                  {
                    std::cerr << "WARNING XC::PlainHandler::handle() - ";
                    std::cerr << " time-varying constraint";
                    std::cerr << " for node " << nodeID;
                    std::cerr << " non-varyng assumed\n";
                  }
                const Matrix &C = mpPtr->getConstraint();
                int numRows = C.noRows();
                int numCols = C.noCols();
                if(numRows != numCols)
                  {
                    std::cerr << "WARNING PlainHandler::handle() - ";
                        std::cerr << " constraint matrix not diagonal, ignoring constraint";
                        std::cerr << " for node " << nodeID << std::endl;
                        std::cerr << " non-varyng assumed\n";
                  }
                else
                  {
                    int ok = 0;
                    for(int i=0; i<numRows; i++)
                      {
                        if(C(i,i) != 1.0) ok = 1;
                        for(int j=0; j<numRows; j++)
                          if(i != j)
                            if(C(i,j) != 0.0)
                          ok = 1;
		      }
                    if(ok != 0)
                      {
                        std::cerr << "WARNING XC::PlainHandler::handle() - ";
                        std::cerr << " constraint matrix not identity, ignoring constraint";
                        std::cerr << " for node " << nodeID << std::endl;
                        std::cerr << " non-varyng assumed\n";
                      }
                    else
                      {
                        const ID &dofs = mpPtr->getConstrainedDOFs();
                        const ID &id = dofPtr->getID();                                
                        for(int i=0; i<dofs.Size(); i++)
                          {
                            int dof = dofs(i);        
                            if(id(dof) == -2)
                              {
                                dofPtr->setID(dof,-4);
                                countDOF--;        
                              }
                            else
                              {
                                std::cerr << "WARNING XC::PlainHandler::handle() - ";
                                std::cerr << " constraint at dof " << dof << " already specified for constrained node";
                                std::cerr << " in MP_Constraint at node " << nodeID << std::endl;
                              }
                                        
			  }
		      }
		  }
	      }
	  }
        // loop through the MP_Constraints to see if any of the
        // DOFs are constrained, note constraint matrix must be diagonal
        // with 1's on the diagonal
        MRMP_ConstraintIter &theMRMPs = theDomain->getConstraints().getMRMPs();
        MRMP_Constraint *mrmpPtr;
        while((mrmpPtr = theMRMPs()) != 0)
          {
	    std::cerr << "PlainHandler::handle loop through the MRMP_Constraints." << std::endl;
          }
      }

    // set the number of eqn in the model
    theModel->setNumEqn(countDOF);

    // now see if we have to set any of the dof's to -3
    //    int numLast = 0;
    if(nodesLast != 0) 
        for (int i=0; i<nodesLast->Size(); i++) {
            int nodeID = (*nodesLast)(i);
            Node *nodPtr = theDomain->getNode(nodeID);
            if(nodPtr != 0) {
                dofPtr = nodPtr->getDOF_GroupPtr();
                
                const XC::ID &id = dofPtr->getID();
                // set all the dof values to -3
                for (int j=0; j < id.Size(); j++) 
                    if(id(j) == -2) {
                        dofPtr->setID(j,-3);
                        count3++;
                    } else {
                        std::cerr << "WARNING XC::PlainHandler::handle() ";
                        std::cerr << " - boundary sp constraint in subdomain";
                        std::cerr << " this should not be - results suspect \n";
                    }
            }
        }
    
    // initialise the FE_Elements and add to the XC::AnalysisModel.
    ElementIter &theEle = theDomain->getElements();
    Element *elePtr;

    int numFe = 0;    
    FE_Element *fePtr;
    while((elePtr = theEle()) != 0)
      { fePtr= theModel->createFE_Element(numFe++, elePtr); }
    return count3;
  }


int XC::PlainHandler::sendSelf(CommParameters &cp)
  { return 0; }

int XC::PlainHandler::recvSelf(const CommParameters &cp)
  { return 0; }
