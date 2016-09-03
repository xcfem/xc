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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/02/22 22:21:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/CentralDifferenceNoDamping.cpp,v $

// Written: fmk 
// Created: 11/98
//
// Description: This file contains the implementation of the XC::CentralDifferenceNoDamping 
// class.
//
// What: "@(#) CentralDifferenceNoDamping.C, revA"

#include <solution/analysis/integrator/transient/CentralDifferenceNoDamping.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>

//! @brief Constructor.
XC::CentralDifferenceNoDamping::CentralDifferenceNoDamping(SoluMethod *owr)
:CentralDifferenceBase(owr,INTEGRATOR_TAGS_CentralDifferenceNoDamping) {}

int XC::CentralDifferenceNoDamping::formEleResidual(FE_Element *theEle)
  {
    theEle->zeroResidual();
    theEle->addRtoResidual();
    return 0;
  }

int XC::CentralDifferenceNoDamping::formNodUnbalance(DOF_Group *theDof)
  {
    theDof->zeroUnbalance();
    theDof->addPtoUnbalance();
    return 0;
  }


int XC::CentralDifferenceNoDamping::domainChanged(void)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const Vector &x = theLinSOE->getX();
    const int size = x.Size();
  
    if(U.get().Size() != size)
      U.resize(size);
    
    // now go through and populate U and Udot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel

    DOF_GrpIter &theDOFGroups = myModel->getDOFGroups();
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
      }    
    return 0;
  }

int XC::CentralDifferenceNoDamping::update(const XC::Vector &X)
  {
    updateCount++;
    if(updateCount > 1)
      {
        std::cerr << "ERROR XC::CentralDifferenceNoDamping::update() - called more than once -";
        std::cerr << " Central Difference integraion schemes require a LINEAR solution algorithm\n";
        return -1;
      }
  
    AnalysisModel *theModel = this->getAnalysisModelPtr();

    if(!theModel)
      {
        std::cerr << "ERROR XC::CentralDifferenceNoDamping::update() - no XC::AnalysisModel set\n";
        return -2;
      }	
  
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(U.get().Size()==0)
      {
        std::cerr << "WARNING XC::CentralDifferenceNoDamping::update() - domainChange() failed or not called\n";
        return -2;
      }	

    // check deltaU is of correct size
    if(X.Size() != U.get().Size())
      {
        std::cerr << "WARNING XC::CentralDifferenceNoDamping::update() - Vectors of incompatable size ";
        std::cerr << " expecting " << U.get().Size() << " obtained " << X.Size() << std::endl;
        return -3;
      }

    //  determine the acceleration at time t 
    (U.getDotDot()) = X;

    //  determine the vel at t+ 0.5 * delta t 
    U.getDot().addVector(1.0, X, deltaT);
  
    //  determine the displacement at t+delta t 
    U.get().addVector(1.0, U.getDot(), deltaT);

    // update the disp & responses at the DOFs
    theModel->setDisp(U.get());
    updateModel();
    return 0;
  }    

int XC::CentralDifferenceNoDamping::commit(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(!theModel)
      {
        std::cerr << "WARNING XC::CentralDifferenceNoDamping::commit() - no XC::AnalysisModel set\n";
        return -1;
      }	  
  
    // update time in XC::Domain to T + deltaT & commit the domain
    double time = getCurrentModelTime() + deltaT;
    setCurrentModelTime(time);

    return commitModel();
  }

int XC::CentralDifferenceNoDamping::sendSelf(CommParameters &cp)
  { return 0; }

int XC::CentralDifferenceNoDamping::recvSelf(const CommParameters &cp)
  { return 0; }

