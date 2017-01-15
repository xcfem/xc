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

