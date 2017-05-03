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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/eigenAlgo/EigenAlgorithm.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/algorithm/eigenAlgo/EigenAlgorithm.C
//
// Written: Jun Peng
// Created: Wed Jan 27, 1999
// Revision: A
//
// Description: This file contains the class definition of XC::EigenAlgorithm.
// EigenAlgorithm is a class which performs a eigen solution algorithm
// to solve the equations. 
//
// This class is inheritanted from the base class of XC::SolutionAlgorithm
// which was created by fmk (Frank).


#include <solution/analysis/algorithm/eigenAlgo/EigenAlgorithm.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/integrator/EigenIntegrator.h>
#include <solution/system_of_eqn/eigenSOE/EigenSOE.h>
#include <utility/matrix/Vector.h>

//! @brief Constructor.
XC::EigenAlgorithm::EigenAlgorithm(SoluMethod *owr,int classTag)
  :SolutionAlgorithm(owr,classTag) {}

//! @brief Return a pointer to the integrator.
XC::EigenIntegrator *XC::EigenAlgorithm::getEigenIntegratorPtr()
  { return dynamic_cast<EigenIntegrator *>(getIntegratorPtr()); }

//! @brief Dump the eigenvalues into the model.
void XC::EigenAlgorithm::eigen_to_model(int numModes)
  {
    AnalysisModel *theModel= getAnalysisModelPtr();
    assert(theModel);
    theModel->setNumEigenvectors(numModes);
    Vector theEigenvalues(numModes);
    Vector modalParticipationFactors(numModes);
    EigenSOE *theSOE= getEigenSOEPtr();
    assert(theSOE);
    for(int i= 1;i<=numModes;i++)
      {
        theEigenvalues[i-1] = theSOE->getEigenvalue(i);
        modalParticipationFactors[i-1] = theSOE->getModalParticipationFactor(i);
        theModel->setEigenvector(i, theSOE->getEigenvector(i));
      }
    theModel->setEigenvalues(theEigenvalues);
    theModel->setModalParticipationFactors(modalParticipationFactors);
  }
