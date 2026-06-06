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

// $Revision: 1.9 $
// $Date: 2005/11/29 22:42:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/HallFactorsNewton.cpp,v $


// File: ~/OOP/analysis/algorithm/HallFactorsNewton.C
//
// Written: fmk
// Created: Sun Sept 15 15:06:47: 1996
// Revision: A
//

// Description: This file contains the class definition for
// HallFactorsNewton. HallFactorsNewton is a class which uses the
// Newton-Raphson solution algorithm
// to solve the equations. No member functions are declared as virtual as
// it is not expected that this class will be subclassed.
//
// What: "@(#)HallFactorsNewton.C, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/HallFactorsNewton.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <utility/matrix/ID.h>
#include "solution/SolutionStrategy.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Constructor
//!
//! @param SolutionStrategy: object the object which is used to access the
//! object that compound the analysis and that, at the end of each iteration,
//! are used to determine if convergence has been obtained.
XC::HallFactorsNewton::HallFactorsNewton(SolutionStrategy *owr,int classTag, int theTangentToUse, double i_factor, double c_factor, int fact_once)
  : NewtonBased(owr, classTag),
  numIterations(0), factorOnce(fact_once), iFactor(i_factor), cFactor(c_factor)
  {}

//! @brief Instruct the solver to use the model’s initial stiffness matrix for
//! every iteration within every time step.
void XC::HallFactorsNewton::useInitialTangent(void)
  {
    this->tangent= INITIAL_TANGENT;
    this->factorOnce= 1; // initial implies factor-once
    this->iFactor= 0;
    this->cFactor= 1;
  }

//! @brief Instruct the solver to use the model’s secant stiffness matrix for
//! every iteration within every time step.
void XC::HallFactorsNewton::useSecantTangent(int factOnce)
  {
    this->tangent= CURRENT_SECANT;
    this->factorOnce= factOnce;
    this->iFactor= 0;
    this->cFactor= 1;
  }

//! @brief Instruct the solver to use the model’s hall stiffness matrix for
//! every iteration within every time step.
void XC::HallFactorsNewton::useHallTangent(double iFactor, double cFactor, int factOnce)
  {
    this->tangent= HALL_TANGENT;
    this->factorOnce= factOnce;
    this->iFactor= iFactor;
    this->cFactor= cFactor;
  }

//! @brief Instruct the solver to use the model’s currentstiffness matrix for
//! every iteration within every time step.
void XC::HallFactorsNewton::useCurrentTangent(int factOnce)
  {
    this->tangent= INITIAL_TANGENT;
    this->factorOnce= factOnce;
    this->iFactor= 0;
    this->cFactor= 1;
  }

//! @brief Send object members through the communicator argument.
int XC::HallFactorsNewton::sendData(Communicator &comm)
  {
    int res= NewtonBased::sendData(comm);
    res+= comm.sendInts(this->numIterations, this->factorOnce, getDbTagData(), CommMetaData(3));
    res+= comm.sendDoubles(iFactor, cFactor, getDbTagData(), CommMetaData(4));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::HallFactorsNewton::recvData(const Communicator &comm)
  {
    int res= NewtonBased::recvData(comm);
    res+= comm.receiveInts(this->numIterations, this->factorOnce, getDbTagData(),CommMetaData(3));
    res+= comm.receiveDoubles(iFactor, cFactor, getDbTagData(), CommMetaData(4));
    return res;
  }

//! @brief Send object through the communicator argument.
int XC::HallFactorsNewton::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(5);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receive object through the communicator argument.
int XC::HallFactorsNewton::recvSelf(const Communicator &comm)
  {
    inicComm(5);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids."
	        << Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data."
		    << Color::def << std::endl;
      }
    return res;
  }

//! Sends the string 'HallFactorsNewton' to the stream if
//! \p flag equals \f$0\f$.
void XC::HallFactorsNewton::Print(std::ostream &s, int flag) const
  {
    s << getClassName() << std::endl
      << "numIterations= " << this->numIterations << std::endl
      << "iFactor= " << this->iFactor << std::endl
      << "cFactor= " << this->cFactor << std::endl
      << "factorOnce= " << this->factorOnce << std::endl;
  }









