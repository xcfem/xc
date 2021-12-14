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
                                                                        
// $Revision: 1.2 $
// $Date: 2003/02/14 23:00:59 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/load/pattern/CFactorSeries.cpp,v $

// File: ~/domain/load/pattern/CFactorSeries.C
//
// Written: fmk 
// Created: 07/99
// Revision: A
//
// Purpose: This file contains the class definition for XC::CFactorSeries.
// CFactorSeries is a concrete class. A XC::CFactorSeries object provides
// a linear time series. the factor is given by the pseudoTime and 
// a constant factor provided in the constructor.
//
// What: "@(#) CFactorSeries.C, revA"


#include "CFactorSeries.h"

#include "utility/actor/actor/MovableVector.h"
#include "utility/matrix/ID.h"

//! @brief Constructor.
//!
//! @param theFactor: factor used in the relation.
XC::CFactorSeries::CFactorSeries(int classTag,double theFactor)
  :TimeSeries(classTag), cFactor(theFactor) {}

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::CFactorSeries::getDbTagData(void) const
  {
    static DbTagData retval(1);
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::CFactorSeries::sendData(Communicator &comm)
  { return comm.sendDouble(cFactor,getDbTagData(),CommMetaData(0)); }

//! @brief Receives object members through the communicator argument.
int XC::CFactorSeries::recvData(const Communicator &comm)
  { return comm.receiveDouble(cFactor,getDbTagData(),CommMetaData(0)); }

//! @brief Sends object through the communicator argument.
int XC::CFactorSeries::sendSelf(Communicator &comm)
  {
    inicComm(1);
    int result= sendData(comm);

    const int dataTag= getDbTag(comm);
    result+= comm.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; ch failed to send data.\n";
    return result;
  }

//! @brief Receives object through the communicator argument.
int XC::CFactorSeries::recvSelf(const Communicator &comm)
  {
    inicComm(1);

    const int dataTag = this->getDbTag();  
    int result = comm.receiveIdData(getDbTagData(),dataTag);
    if(result<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ch failed to receive data.\n";
    else
      result+= recvData(comm);
    return result;    
  }
