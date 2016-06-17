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
                                                                        
// $Revision: 1.4 $
// $Date: 2003/02/14 23:00:49 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/StaticIntegrator.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/integrator/StaticIntegrator.C
// 
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Description: This file contains the class definition for XC::StaticIntegrator.
// StaticIntegrator is an algorithmic class for setting up the finite element
// equations for a static analysis and for Incrementing the nodal displacements
// with the values in the soln vector to the XC::LinearSOE object. 
//
// What: "@(#) StaticIntegrator.C, revA"

#include <solution/analysis/integrator/StaticIntegrator.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Vector.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>

XC::StaticIntegrator::StaticIntegrator(SoluMethod *owr,int clasTag)
  :IncrementalIntegrator(owr,clasTag) {}

//! @brief Solicita al elemento being passed as parameter
//! que forme su matriz de rigidez.
int XC::StaticIntegrator::formEleTangent(FE_Element *theEle)
  {
    // only elements stiffness matrix needed

    if(statusFlag == CURRENT_TANGENT)
      {
        theEle->zeroTangent();
        theEle->addKtToTang();
      }
    else if(statusFlag == INITIAL_TANGENT)
      {
        theEle->zeroTangent();
        theEle->addKiToTang();
      }
    return 0;
  }

int XC::StaticIntegrator::formEleResidual(FE_Element *theEle)
  {
    // only elements residual needed
    theEle->zeroResidual();
    theEle->addRtoResidual();
    return 0;
  }    

int XC::StaticIntegrator::formNodTangent(DOF_Group *theDof)
  {
    // should never be called
    std::cerr << "StaticIntegrator::formNodTangent() -";
    std::cerr << " this method should never have been called!\n";
    return -1;
  }    

int XC::StaticIntegrator::formNodUnbalance(DOF_Group *theDof)
  {
    // only nodes unbalance need be added
    theDof->zeroUnbalance();
    theDof->addPtoUnbalance();
    return 0;
  }    

