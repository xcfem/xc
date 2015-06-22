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
XC::CentralDifferenceAlternative::CentralDifferenceAlternative(SoluMethod *owr)
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
    if(Ut.Nulo())
      {
        std::cerr << "WARNING CentralDifferenceAlternative::update() - domainChange() failed or not called\n";
        return -2;
      }	

    // check deltaU is of correct size
    if(X.Size() != Ut.Size())
      {
        std::cerr << "WARNING CentralDifferenceAlternative::update() - Vectors of incompatable size ";
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

int XC::CentralDifferenceAlternative::sendSelf(CommParameters &cp)
  { return 0; }

int XC::CentralDifferenceAlternative::recvSelf(const CommParameters &cp)
  { return 0; }


