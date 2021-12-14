//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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

//! @brief Constructor.
//!
//! @param owr: set of objects used to perform the analysis.
XC::StaticIntegrator::StaticIntegrator(SolutionStrategy *owr,int clasTag)
  :IncrementalIntegrator(owr,clasTag) {}

//! @brief Asks the element  being passed as parameter to build
//! its tangent stiffness matrix.
//!
//! To form the tangent matrix of the FE\_Element, \p theEle, is
//! instructed to zero this matrix and then add it's \f$K\f$ matrix to the
//! tangent, i.e. it performs the following: 
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theEle-\f$>\f$zeroTang()
//! theEle-\f$>\f$addKtoTang() 
//! \end{tabbing}
//! \noindent The method returns \f$0\f$.
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

//! To form the residual vector of the FE\_Element.
//! 
//! To form the residual vector of the FE\_Element, \p theEle, is
//! instructed to zero the vector and then add it's \f$R\f$ vector to
//! the residual, i.e. it performs the following: 
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theEle-\f$>\f$zeroResidual()
//! theEle-\f$>\f$addRtoResid() 
//! \end{tabbing}
//! \noindent The method returns \f$0\f$.
int XC::StaticIntegrator::formEleResidual(FE_Element *theEle)
  {
    // only elements residual needed
    theEle->zeroResidual();
    theEle->addRtoResidual();
    return 0;
  }    

//! @brief This should never be called in a static analysis. An error message is
//! printed if it is. Returns -1.
int XC::StaticIntegrator::formNodTangent(DOF_Group *theDof)
  {
    // should never be called
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; this method should never have been called!\n";
    return -1;
  }    

//! Form the unbalance vector for the DOF\_Group.
//! 
//! To form the unbalance vector of the DOF\_Group, \p theDof, is
//! instructed to zero the vector and then add it's \f$P\f$ vector to the
//! unbalance, i.e. it performs the following: 
//! \begin{tabbing}
//! while \= \+ while \= while \= \kill
//! theDof-\f$>\f$zeroUnbalance()
//! theDof-\f$>\f$addPtoUnbal() 
//! \end{tabbing}
//! \noindent The method returns \f$0\f$.
int XC::StaticIntegrator::formNodUnbalance(DOF_Group *theDof)
  {
    // only nodes unbalance need be added
    theDof->zeroUnbalance();
    theDof->addPtoUnbalance();
    return 0;
  }    

