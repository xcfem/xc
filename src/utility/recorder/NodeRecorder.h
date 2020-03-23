// -*-c++-*-
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
                                                                        
// $Revision: 1.12 $
// $Date: 2005/03/30 03:54:33 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/NodeRecorder.h,v $
                                                                        
#ifndef NodeRecorder_h
#define NodeRecorder_h

// Written: fmk 
// Created: 09/00
// Revision: A
//
// What: "@(#) NodeRecorder.h, revA"


#include <utility/recorder/NodeRecorderBase.h>
#include <utility/matrix/Vector.h>

namespace XC {
class Domain;
class FE_Datastore;
class DataOutputHandler;
class Node;

//! @ingroup Recorder
//
//! @brief Recording of specified nodal DOF responses for the specified nodes.
class NodeRecorder: public NodeRecorderBase
  {
  private:	
    int initialize(void);
    Vector response;

    // AddingSensitivity:BEGIN //////////////////////////////
    int gradIndex;
    // AddingSensitivity:END ////////////////////////////////

    void setup_dofs(const ID &dofs);
    void setup_nodes(const ID &nodes);
  protected:
    int sendData(CommParameters &);  
    int receiveData(const CommParameters &);

  public:
    NodeRecorder(void);
    NodeRecorder(const ID &theDof, const ID &theNodes, 
		 int sensitivity, const std::string &dataToStore,
		 Domain &theDomain, DataOutputHandler &theOutputHandler,
		 double deltaT = 0.0, bool echoTimeFlag = true); 

    void setupDataFlag(const std::string &dataToStore);
    int record(int commitTag, double timeStamp);

    int sendSelf(CommParameters &);  
    int recvSelf(const CommParameters &);
  };
} // end of XC namespace

#endif
