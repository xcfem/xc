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
// $Date: 2003/03/06 20:32:01 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/TransientIntegrator.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/TransientIntegrator.C
// 
// Written: fmk 
// Created: Tue Sept 17 15:54:47: 1996
// Revision: A
//
// Description: This file contains the class definition for XC::TransientIntegrator.
// TransientIntegrator is an algorithmic class for setting up the finite element
// equations for a static analysis and for tncrementing the nodal displacements
// with the values in the soln vector to the LinearSOE object. 
//
// What: "@(#) TransientIntegrator.C, revA"

#include <solution/analysis/integrator/TransientIntegrator.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/model/FE_EleIter.h>
#include <solution/analysis/model/DOF_GrpIter.h>


//! @brief Constructor.
XC::TransientIntegrator::TransientIntegrator(SoluMethod *owr,int clasTag)
  : IncrementalIntegrator(owr,clasTag) {}

//! @brief Forma la matriz de rigidez tangente.
int XC::TransientIntegrator::formTangent(int statFlag)
  {
    int result = 0;
    statusFlag = statFlag;

    LinearSOE *theLinSOE = this->getLinearSOEPtr();
    AnalysisModel *theModel = this->getAnalysisModelPtr();
    if(theLinSOE == 0 || theModel == 0)
      {
	std::cerr << "WARNING XC::TransientIntegrator::formTangent() ";
	std::cerr << "no XC::LinearSOE or XC::AnalysisModel has been set\n";
	return -1;
      }
    
    // the loops to form and add the tangents are broken into two for 
    // efficiency when performing parallel computations
    
    theLinSOE->zeroA();

    // loop through the DOF_Groups and add the unbalance
    DOF_GrpIter &theDOFGroups = theModel->getDOFGroups();
    DOF_Group *dofGroupPtr= nullptr;
    
    while((dofGroupPtr = theDOFGroups()) != 0)
      {
	if(theLinSOE->addA(dofGroupPtr->getTangent(this),dofGroupPtr->getID()) <0)
          {
	    std::cerr << "XC::TransientIntegrator::formTangent() - failed to addA:dof\n";
	    result = -1;
	  }
      }    

    // loop through the FE_Elements getting them to add the tangent    
    FE_EleIter &theEles2 = theModel->getFEs();    
    FE_Element *elePtr;    
    while((elePtr = theEles2()) != 0)
      {
	if(theLinSOE->addA(elePtr->getTangent(this),elePtr->getID()) < 0)
          {
	    std::cerr << "XC::TransientIntegrator::formTangent() - failed to addA:ele\n";
	    result = -2;
	  }
      }
    return result;
  }

//! @brief Forma el vector residuo del elemento que se pasa como parámetro.    
int XC::TransientIntegrator::formEleResidual(FE_Element *theEle)
  {
    theEle->zeroResidual();
    theEle->addRIncInertiaToResidual();
    return 0;
  }

//! @brief Forma el vector de cargas desequilibradas del nodo que se pasa como parámetro.
int XC::TransientIntegrator::formNodUnbalance(DOF_Group *theDof)
  {
    theDof->zeroUnbalance();
    theDof->addPIncInertiaToUnbalance();
    return 0;
  }

