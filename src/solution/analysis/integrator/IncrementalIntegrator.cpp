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
                                                                        
// $Revision: 1.7 $
// $Date: 2005/12/19 22:43:36 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/IncrementalIntegrator.cpp,v $
                                                                        
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the implementation of XC::IncrementalIntegrator.
//
// What: "@(#) IncrementalIntegrator.C, revA"

#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include "solution/SoluMethod.h"
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/DOF_GrpIter.h>


//! @brief Constructor.
XC::IncrementalIntegrator::IncrementalIntegrator(SoluMethod *owr,int clasTag)
  : Integrator(owr,clasTag), statusFlag(CURRENT_TANGENT) {}


//! @brief Forma la matriz de rigidez tangente.
int XC::IncrementalIntegrator::formTangent(int statFlag)
  {
    int result = 0;
    statusFlag = statFlag;
    AnalysisModel *mdl= getAnalysisModelPtr();
    LinearSOE *theSOE= getLinearSOEPtr();

    if(!mdl || !theSOE)
      {
	std::cerr << "WARNING XC::IncrementalIntegrator::formTangent() -";
	std::cerr << " no AnalysisModel or LinearSOE have been set\n";
	return -1;
      }

    theSOE->zeroA(); //Anula los elementos de la matriz.
    
    // the loops to form and add the tangents are broken into two for 
    // efficiency when performing parallel computations - CHANGE

    // loop through the FE_Elements adding their contributions to the tangent
    FE_Element *elePtr;
    FE_EleIter &theEles2= mdl->getFEs();    
    while((elePtr = theEles2()) != 0)     
      if(theSOE->addA(elePtr->getTangent(this),elePtr->getID()) < 0)
        {
	  std::cerr << "WARNING XC::IncrementalIntegrator::formTangent -";
	  std::cerr << " failed in addA for ID " << elePtr->getID();	    
	  result = -3;
	}
    return result;
  }

//! @brief Forma el load vector no equilibradas (el lado derecho de la ecuación).
int XC::IncrementalIntegrator::formUnbalance(void)
  {
    AnalysisModel *mdl= getAnalysisModelPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    if((!mdl) || (!theSOE))
      {
	std::cerr << "WARNING IncrementalIntegrator::formUnbalance -";
	std::cerr << " no AnalysisModel or LinearSOE has been set\n";
	return -1;
      }
    
    theSOE->zeroB();
    
    if(formElementResidual() < 0)
      {
	std::cerr << "WARNING IncrementalIntegrator::formUnbalance ";
	std::cerr << " - this->formElementResidual failed\n";
	return -1;
      }
    
    if(formNodalUnbalance() < 0)
      {
	std::cerr << "WARNING XC::IncrementalIntegrator::formUnbalance ";
	std::cerr << " - this->formNodalUnbalance failed\n";
	return -2;
      }
    return 0;
  }
  
//! @brief Returns the response on the DOFs being passed as parameter.
//! @param id: dofs identifiers on wich the response is requested.
//! @param result: respones values.
int XC::IncrementalIntegrator::getLastResponse(Vector &result, const XC::ID &id)
  {
    LinearSOE *theSOE= getLinearSOEPtr();
    if(!theSOE)
      {
	std::cerr << "WARNING XC::IncrementalIntegrator::getLastResponse() -";
	std::cerr << "no LineaerSOE object associated with this object\n";	
	return -1;
      }

    int res = 0; 
    int size = theSOE->getNumEqn() -1;
    const Vector &X = theSOE->getX();
    for(int i=0; i<id.Size(); i++)
      {
	int loc = id(i);
	if(loc < 0 )
	  result(i) = 0.0;
	else if(loc <= size) //El GDL existe.
          { result(i) = X(loc);	}
	else
          {
	    std::cerr << "WARNING XC::IncrementalIntegrator::getLastResponse() -";
	    std::cerr << "location " << loc << "in XC::ID ouside bounds ";
	    std::cerr << size << "\n";	
	    res = -2;
	  }
      }	    
    return res;
  }

//! @brief ??
int XC::IncrementalIntegrator::newStep(double deltaT)
  { return 0; }


int XC::IncrementalIntegrator::initialize(void)
  { return 0; }

//! @brief Consuma los valores calculados hasta ahora.
int XC::IncrementalIntegrator::commit(void) 
  {
    int retval= -1;
    AnalysisModel *mdl= getAnalysisModelPtr();
    if(!mdl)
      {
	std::cerr << "WARNING XC::IncrementalIntegrator::commit() -";
	std::cerr << "no AnalysisModel object associated with this object\n";	
      }
    else
      retval= commitModel();
    return retval;
  }


int XC::IncrementalIntegrator::revertToLastStep(void) 
  { return 0; }   

int XC::IncrementalIntegrator::revertToStart()
  {
    std::cerr << "ERROR: revertToStart() method not yet implemented " << std::endl
	      << " for the chosen type of integrator. " << std::endl;
    return 0;
  }

//! @brief Returns a pointer to the system of equations de eigenvalues.
XC::LinearSOE *XC::IncrementalIntegrator::getLinearSOEPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Returns a pointer to the system of equations de eigenvalues.
const XC::LinearSOE *XC::IncrementalIntegrator::getLinearSOEPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Forma el load vector no equilibradas en los nodos.
int XC::IncrementalIntegrator::formNodalUnbalance(void)
  {
    // loop through the DOF_Groups and add the unbalance
    LinearSOE *theSOE= getLinearSOEPtr();
    AnalysisModel *mdl= getAnalysisModelPtr();
    DOF_GrpIter &theDOFGroups= mdl->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    int res = 0;

    while((dofGroupPtr = theDOFGroups()) != 0)
      { 
        //      std::cerr << "NODPTR: " << dofGroupPtr->getUnbalance(this);
	if(theSOE->addB(dofGroupPtr->getUnbalance(this),dofGroupPtr->getID()) <0)
          {
	    std::cerr << "WARNING IncrementalIntegrator::formNodalUnbalance -";
	    std::cerr << " failed in addB for XC::ID " << dofGroupPtr->getID();
	    res = -2;
	  }
      }
    return res;
  }

//! @brief Forma el vector residuo de los elementos.
int XC::IncrementalIntegrator::formElementResidual(void)
  {
    // loop through the FE_Elements and add the residual
    FE_Element *elePtr;

    int res = 0;    

    LinearSOE *theSOE= getLinearSOEPtr();
    AnalysisModel *mdl= getAnalysisModelPtr();
    FE_EleIter &theEles2 = mdl->getFEs();
    while((elePtr= theEles2()) != nullptr)
      {
	if(theSOE->addB(elePtr->getResidual(this),elePtr->getID()) <0)
          {
	    std::cerr << "WARNING IncrementalIntegrator::formElementResidual -";
	    std::cerr << " failed in addB for XC::ID " << elePtr->getID();
	    res = -2;
	  }
      }
    return res;	    
  }

