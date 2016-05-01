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

//! @brief Forma el vector de cargas no equilibradas (el lado derecho de la ecuación).
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
  
//! @brief Devuelve la respusta correspondiente a los grados de libertad de id en
//! el vector result que se pasa como parámetro.  
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

//! @brief Devuelve un puntero al sistema de ecuaciones de autovalores.
XC::LinearSOE *XC::IncrementalIntegrator::getLinearSOEPtr(void)
  {
    SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Devuelve un puntero al sistema de ecuaciones de autovalores.
const XC::LinearSOE *XC::IncrementalIntegrator::getLinearSOEPtr(void) const
  {
    const SoluMethod *sm= getSoluMethod();
    assert(sm);
    return sm->getLinearSOEPtr();
  }

//! @brief Forma el vector de cargas no equilibradas en los nodos.
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

