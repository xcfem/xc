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
                                                                        
// $Revision: 1.10 $
// $Date: 2005/10/19 21:53:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/DisplacementControl.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/DisplacementControl.C
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for XC::DisplacementControl.
// DisplacementControl is an algorithmic class for perfroming a static analysis
// using the arc length scheme, that is within a load step the follwing
// constraint is enforced: dU^TdU + alpha^2*dLambda^2 = DisplacementControl^2
// where dU is change in nodal displacements for step, dLambda is
// change in applied load and XC::DisplacementControl is a control parameter.
//
// What: "@(#) DisplacementControl.C, revA"


#include <solution/analysis/integrator/static/DisplacementControl.h>
#include <solution/analysis/model/AnalysisModel.h>
#include "solution/SoluMethod.h"
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <utility/matrix/ID.h>


//! @brief Constructor.
XC::DisplacementControl::DisplacementControl(SoluMethod *owr) 
  :DispBase(owr,INTEGRATOR_TAGS_DisplacementControl,1),
   theNode(-1), theDof(-1), theIncrement(1.0),
   theDofID(0), minIncrement(1.0), maxIncrement(1.0) {}

//! @brief Constructor.
XC::DisplacementControl::DisplacementControl(SoluMethod *owr,int node, int dof, 
					 double increment, 
					 int numIncr,
					 double min, double max) 
  :DispBase(owr,INTEGRATOR_TAGS_DisplacementControl,numIncr),
   theNode(node), theDof(dof), theIncrement(increment),
   theDofID(0), minIncrement(min), maxIncrement(max) {}

XC::DisplacementControl::~DisplacementControl(void)
  {
    Domain *dom= getDomainPtr();
    if(dom)
      dom->setNodeReactionException(-1);
  }

void XC::DisplacementControl::setIncrement(const double &d)
  {
    theIncrement= d;
    //Actualizamos valores extremos.
    minIncrement= std::min(theIncrement,minIncrement);
    maxIncrement= std::max(theIncrement,maxIncrement);
  }

//! @brief Consuma los valores calculados hasta ahora.
int XC::DisplacementControl::commit(void) 
  {
    Domain *dom= getDomainPtr();
    if(dom)
      dom->setNodeReactionException(theNode);
    return DispBase::commit();
  }

//! @brief Returns a pointer to the domain.
XC::Domain *XC::DisplacementControl::getDomainPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getDomainPtr();
  }

//! @brief Returns a pointer to the domain.
const XC::Domain *XC::DisplacementControl::getDomainPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getDomainPtr();
  }

int XC::DisplacementControl::newStep(void)
  {
    // get pointers to AnalysisModel and XC::LinearSOE
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(!theModel || theLinSOE == 0)
      {
	std::cerr << "WARNING XC::DisplacementControl::newStep() ";
	std::cerr << "No XC::AnalysisModel or XC::LinearSOE has been set\n";
	return -1;
      }

    // determine increment for this iteration
    theIncrement*=factor();

    if(theIncrement < minIncrement)
      theIncrement = minIncrement;
    else if(theIncrement > maxIncrement)
      theIncrement = maxIncrement;



    // get the current load factor
    vectores.setCurrentLambda(getCurrentModelTime());

    // determine dUhat
    this->formTangent();
    vectores.determineUhat(*theLinSOE);

    const Vector &dUhat= vectores.getDeltaUhat();

    const double dUahat= dUhat(theDofID);
    if(dUahat == 0.0)
      {
	std::cerr << "WARNING XC::DisplacementControl::newStep() ";
	std::cerr << "dUahat is zero -- zero reference displacement at control node DOF\n";
	return -1;
      }
    
    // determine delta lambda(1) == dlambda    
    const double dLambda = theIncrement/dUahat;

    vectores.newStep(dLambda,vectores.getDeltaUhat());

    // update model with delta lambda and delta U
    theModel->incrDisp(vectores.getDeltaU());

    applyLoadModel(vectores.getCurrentLambda());
    if(updateModel() < 0)
      {
        std::cerr << "DisplacementControl::newStep - falló la actualización para el nuevo dU\n";
        return -1;
      }
    numIncrLastStep = 0;

    return 0;
  }

int XC::DisplacementControl::update(const Vector &dU)
  {
    if(theDofID == -1)
      {
	std::cerr << "DisplacementControl::updata(dU) - domainChanged has not been called\n";
        return -1;
      } 
    AnalysisModel *theModel= this->getAnalysisModelPtr();
    LinearSOE *theLinSOE= this->getLinearSOEPtr();    
    if(!theModel || theLinSOE == 0)
      {
	std::cerr << "WARNING XC::DisplacementControl::update() ";
	std::cerr << "No XC::AnalysisModel or XC::LinearSOE has been set\n";
	return -1;
      }

    vectores.setDeltaUbar(dU); // have to do this as the SOE is gonna change
    const double dUabar= vectores.getDeltaUbar()(theDofID);
    
    vectores.determineUhat(*theLinSOE);

    const double dUahat= vectores.getDeltaUhat()(theDofID);
    if(dUahat == 0.0)
      {
	std::cerr << "WARNING DisplacementControl::update() ";
	std::cerr << "dUahat is zero -- zero reference displacement at control node DOF\n";
	return -1;
      }
    
    // determine delta lambda(1) == dlambda    
    const double dLambda = -dUabar/dUahat;
    
    vectores.update(dLambda);

    // update the model
    theModel->incrDisp(vectores.getDeltaU());
    //applyLoadModel(currentLambda); XXX Comentado LCPT 3-07-2009 (Si se ejecuta falsea el pseudoTime).

    if(updateModel() < 0)
      {
        std::cerr << "DisplacementControl::update - falló la actualización para el nuevo dU.\n";
        return -1;
      }
	
    
    // set the X soln in linearSOE to be deltaU for convergence Test
    theLinSOE->setX(vectores.getDeltaU());

    numIncrLastStep++;

    return 0;
  }

int XC::DisplacementControl::domainChanged(void)
  {
    // we first create the Vectors needed
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();    
    if(theModel == 0 || theLinSOE == 0)
      {
	std::cerr << "WARNING DisplacementControl::domainChanged() ";
	std::cerr << "No AnalysisModel or LinearSOE has been set\n";
	return -1;
      }
    const size_t sz= theModel->getNumEqn(); // ask model in case N+1 space

    vectores.domainChanged(sz,*this,*theLinSOE);


    // lastly we determine the id of the nodal dof
    // EXTRA CODE TO DO SOME ERROR CHECKING REQUIRED
    
    Domain *dom= getDomainPtr();
    if(dom)
      {
        Node *theNodePtr= dom->getNode(theNode);
        DOF_Group *theGroup= theNodePtr->getDOF_GroupPtr();
        const ID &theID= theGroup->getID();
        theDofID = theID(theDof);
      }
    else
      std::cerr << "DisplacementControl::domainChanged; no se ha asignado el domain." << std::endl;
    
    return 0;
  }

int XC::DisplacementControl::sendSelf(CommParameters &cp)
  {
    // TO FINISH
    return 0;
  }


int XC::DisplacementControl::recvSelf(const CommParameters &cp)
  {
    // TO FINISH
    return 0;
  }

void XC::DisplacementControl::Print(std::ostream &s, int flag)
  {
    // TO FINISH    
  }

