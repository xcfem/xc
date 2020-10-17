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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/01/27 04:32:07 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/CentralDifferenceAlternative.cpp,v $

// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the implementation of the XC::CentralDifferenceAlternative 
// class.
//
// What: "@(#) CentralDifferenceAlternative.C, revA"

#include <solution/analysis/integrator/transient/CentralDifferenceAlternative.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/DOF_GrpIter.h>
#include <solution/analysis/model/AnalysisModel.h>

//! @brief Constructor.
XC::CentralDifferenceAlternative::CentralDifferenceAlternative(SolutionStrategy *owr)
:CentralDifferenceBase(owr,INTEGRATOR_TAGS_CentralDifferenceAlternative),
 Ut(0), Utp1(0) {}

int XC::CentralDifferenceAlternative::domainChanged(void)
  {
    AnalysisModel *myModel = this->getAnalysisModelPtr();
    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    const Vector &x = theLinSOE->getX();
    const int size= x.Size();
  
    if(Ut.Size()!=size)
      {
        Ut.resize(size);
        Utp1.resize(size);
        Udot.resize(size);
      }
    // now go through and populate U and Udot by iterating through
    // the DOF_Groups and getting the last committed velocity and accel

    DOF_GrpIter &theDOFGroups = myModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr = theDOFGroups()) != nullptr)
      {
        const Vector &disp = dofGroupPtr->getCommittedDisp();	
        const Vector &vel = dofGroupPtr->getCommittedVel();	
        const ID &id = dofGroupPtr->getID();
        for(int i=0; i < id.Size(); i++)
          {
            int loc = id(i);
            if(loc >= 0)
              {
 	        Ut(loc) = disp(i);		
	        Udot(loc) = vel(i);		
              }
          }
      }
    return 0;
  }


int XC::CentralDifferenceAlternative::update(const Vector &X)
  {
    updateCount++;
    if(updateCount > 1)
      {
        std::cerr << "ERROR XC::CentralDifferenceAlternative::update() - called more than once -";
        std::cerr << " Central Difference integraion schemes require a LINEAR solution algorithm\n";
        return -1;
      }
  
    AnalysisModel *theModel= this->getAnalysisModelPtr();

    if(!theModel)
      {
        std::cerr << "ERROR XC::CentralDifferenceAlternative::update() - no XC::AnalysisModel set\n";
        return -2;
      }	
  
    // check domainChanged() has been called, i.e. Ut will not be zero
    if(Ut.isEmpty())
      {
        std::cerr << "WARNING CentralDifferenceAlternative::update() - domainChange() failed or not called\n";
        return -2;
      }	

    // check deltaU is of correct size
    if(X.Size() != Ut.Size())
      {
        std::cerr << "WARNING CentralDifferenceAlternative::update() - Vectors of incompatible size ";
        std::cerr << " expecting " << Ut.Size() << " obtained " << X.Size() << std::endl;
        return -3;
      }


    //  determine the displacement at t+delta t 
    Utp1.addVector(0.0, X, deltaT * deltaT);
    Utp1+= Ut;
    Utp1.addVector(1.0, Udot, deltaT);

    //  determine the vel at t+ 0.5 * delta t 
    Udot= Utp1;
    Udot-= Ut;
    Udot*= (1.0/deltaT);

    // update the disp & responses at the DOFs
    theModel->setDisp(Utp1);
    theModel->setVel(Udot);
    updateModel();

    return 0;
  }    

int XC::CentralDifferenceAlternative::commit(void)
  {
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(!theModel)
      {
        std::cerr << "WARNING XC::CentralDifferenceAlternative::commit() - no XC::AnalysisModel set\n";
        return -1;
      }
  
    Ut= Utp1;
  
    // update time in XC::Domain to T + deltaT & commit the domain
    double time = getCurrentModelTime() + deltaT;
    setCurrentModelTime(time);

    return commitModel();
  }

int XC::CentralDifferenceAlternative::sendSelf(Communicator &comm)
  { return 0; }

int XC::CentralDifferenceAlternative::recvSelf(const Communicator &comm)
  { return 0; }


