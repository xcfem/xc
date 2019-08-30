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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/eigenAlgo/StandardEigenAlgo.cpp,v $
                                                                        
// Written: MHS
// Created: Oct 2001
//
// Description: This file contains the class definition of XC::StandardEigenAlgo.
// StandardEigenAlgo is a class which performs a eigen solution algorithm
// to solve standard eigenvalue equations. It is not expected that 
// this class will have subclasses.

#include <solution/analysis/algorithm/eigenAlgo/StandardEigenAlgo.h>
#include <solution/analysis/analysis/EigenAnalysis.h>
#include <solution/analysis/integrator/EigenIntegrator.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>


//! @brief Constructor.
XC::StandardEigenAlgo::StandardEigenAlgo(AnalysisAggregation *owr)
  :EigenAlgorithm(owr,EigenALGORITHM_TAGS_Standard) {}

//! @brief Calculate the eigenvalues for the current step.
int XC::StandardEigenAlgo::solveCurrentStep(int numModes)
  {
    AnalysisModel *theModel= getAnalysisModelPtr();
    EigenIntegrator *theIntegrator= getEigenIntegratorPtr();
    EigenSOE *theSOE = getEigenSOEPtr();
    if((theModel == 0) || (theIntegrator == 0) || (theSOE == 0))
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; domain, model or integrator not assigned.\n";
        return -1;
      }
  
    if(theIntegrator->formK() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the Integrator failed in formK()\n";
        return -2;
      }
  
    if(theSOE->solve(numModes) < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; the EigenSOE failed in solve()\n";
        return -4;
      }
  
    // now set the eigenvalues and eigenvectors in the model
    eigen_to_model(numModes);
    return 0;
  }

//! @brief Send.
int XC::StandardEigenAlgo::sendSelf(CommParameters &cp)
  { return 0; }

//! @brief Receive.
int XC::StandardEigenAlgo::recvSelf(const CommParameters &cp)
  { return 0; }

//! @brief Print.
void XC::StandardEigenAlgo::Print(std::ostream &s, int flag)
  {
    s << "\tStandardEigenAlgo\n";
  }
