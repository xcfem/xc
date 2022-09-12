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
// File: ~/system_of_eqn/eigenSOE/ArpackSOE.cc
//
// Written: Jun Peng 
// Created: February 1999
// Revision: A
//
// Description: This file contains the class definition for XC::ArpackSOE
// ArpackSOE is a subclass of XC::EigenSOE. It uses the LAPACK storage
// scheme to store the components of the K, M matrix, which is a full matrix.
// It uses the ARPACK to do eigenvalue analysis.

#include "ArpackSOE.h"
#include "utility/actor/channel/Channel.h"
#include "solution/system_of_eqn/linearSOE/LinearSOE.h"


//! @brief Constructor.
XC::ArpackSOE::ArpackSOE(SolutionStrategy *owr, double theShift)
  :ArpackSOEBase(owr,theShift), theSOE(nullptr)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; Work in progres. Not yet ready." << std::endl;
  }


//! @brief Assigns the value of the linear SOE.
int XC::ArpackSOE::setLinearSOE(LinearSOE &theLinearSOE)
  {
    theSOE = &theLinearSOE;
    return 0;
  }

//! @brief Return the number of equations of the linear SOE.
int XC::ArpackSOE::getNumEqn(void) const
  {
    if (theSOE != 0)
      return theSOE->getNumEqn();
    else 
      return 0;
  }

int XC::ArpackSOE::checkSameInt(int value)
  {
	if (processID == -1)
		return 1;

	static ID idData(1);
    if (processID != 0) {
    
		Channel *theChannel = theChannels[0];
	    idData(0) = value;
		theChannel->sendID(0, 0, idData);
		theChannel->recvID(0, 0, idData);
		if (idData(0) == 1)
			return 1;
		else
			return 0;
	} 

	else {
        int ok = 1;
		// receive B
	const int numChannels= getNumChannels();
		for (int j=0; j<numChannels; j++) {
		// get X & add
			Channel *theChannel = theChannels[j];
			theChannel->recvID(0, 0, idData);
			if (idData(0) != value)
				ok = 0;
		}

		// send results back
		idData(0) = ok;
		for (int j=0; j<numChannels; j++) {
			Channel *theChannel = theChannels[j];
			theChannel->sendID(0, 0, idData);
		}
		return ok;
    }
}
