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
                                                                        
// $Revision: 1.5 $
// $Date: 2005/10/19 21:53:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/integrator/BaseControl.cpp,v $
                                                                        
                                                                        
// 
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for XC::BaseControl.
// BaseControl is an algorithmic class for performing a static analysis
// using a load control integration scheme.
//
// What: "@(#) BaseControl.h, revA"



#include <solution/analysis/integrator/static/BaseControl.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <utility/matrix/Vector.h>

//! @brief Constructor.
//!
//! @param owr: set of objects used to perform the analysis.
//! @param classTag: class identifier.
//! @param numIncr: number of increments.
XC::BaseControl::BaseControl(AnalysisAggregation *owr,int classTag,int numIncr)
:StaticIntegrator(owr,classTag), specNumIncrStep(numIncr), numIncrLastStep(numIncr)
  { setup_numIncr(numIncr); }

void XC::BaseControl::setup_numIncr(const int &numIncr)
  {
    // to avoid divide-by-zero error on first update() ensure numIncr != 0
    if(numIncr == 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; numIncr set to 0, 1 assumed\n";
        specNumIncrStep= 1.0;
        numIncrLastStep= 1.0;
      }
    else
      {
        specNumIncrStep= static_cast<double>(numIncr);
        numIncrLastStep= static_cast<double>(numIncr);
      }
  }

//! @brief Send object members through the communicator argument.
int XC::BaseControl::sendData(Communicator &comm)
  {
    int res= StaticIntegrator::sendData(comm);
    res+= comm.sendDoubles(specNumIncrStep,numIncrLastStep,getDbTagData(),CommMetaData(1));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::BaseControl::recvData(const Communicator &comm)
  {
    int res= StaticIntegrator::recvData(comm);
    res+= comm.receiveDoubles(specNumIncrStep,numIncrLastStep,getDbTagData(),CommMetaData(1));
    return res;
  }
