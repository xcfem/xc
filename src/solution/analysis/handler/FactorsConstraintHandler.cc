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
// $Date: 2005/11/29 22:04:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/FactorsConstraintHandler.cpp,v $

// Written: fmk
// Created: May 1998
// Revision: A
//
// What: "@(#) FactorsConstraintHandler.C, revA"

#include <solution/analysis/handler/FactorsConstraintHandler.h>
#include <utility/matrix/Vector.h>


//! @brief Constructor.
//! @param owr: pointer to the model wrapper that owns the handler.
//! @param classTag: identifier of the class.
//! @param sp: factor to be used with the single freedom constraints.
//! @param mp: factor to be used with the multi-freedom constraints.
XC::FactorsConstraintHandler::FactorsConstraintHandler(ModelWrapper *owr,int classTag,const double &sp,const double &mp)
  :ConstraintHandler(owr,classTag), alphaSP(sp), alphaMP(mp) {}

//! @brief Send object members through the communicator argument.
int XC::FactorsConstraintHandler::sendData(Communicator &comm)
  {
    int res= ConstraintHandler::sendData(comm);
    res+= comm.sendDoubles(alphaSP,alphaMP,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receive object members through the communicator argument.
int XC::FactorsConstraintHandler::recvData(const Communicator &comm)
  {
    int res= ConstraintHandler::recvData(comm);
    res+= comm.receiveDoubles(alphaSP,alphaMP,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Send object through the communicator argument.
int XC::FactorsConstraintHandler::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << __FUNCTION__
                << "; failed to send data\n";
    return res;
  }

//! @brief Receive object through the communicator argument.
int XC::FactorsConstraintHandler::recvSelf(const Communicator &comm)  
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << __FUNCTION__
	        << "; failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << __FUNCTION__
                    << "; failed to receive data.\n";
      }
    return res;
  }

