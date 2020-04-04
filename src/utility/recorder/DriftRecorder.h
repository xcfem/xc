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
                                                                        
// $Revision: 1.9 $
// $Date: 2005/03/30 20:12:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/DriftRecorder.h,v $
                                                                        
#ifndef DriftRecorder_h
#define DriftRecorder_h

// Written: MHS
// Created: Oct 2001
//
// Description: This file contains the class definition for 
// DriftRecorder. 

#include "HandlerRecorder.h"
#include <vector>
#include "utility/matrix/Vector.h"

namespace XC {
class Domain;
class DataOutputHandler;
class Node;
class ID;

//! @ingroup Recorder
//
//! @brief Records relative displacement between nodes. The drift is taken 
//! as the ratio between the prescribed relative displacement and
//! the specified distance between the nodes.
class DriftRecorder: public HandlerRecorder
  {
  private:	
    int initialize(void);

    ID *ndI;
    ID *ndJ;
    std::vector<Node *> theNodes; //!< i & j nodes
    int dof; //! degree of freedom.
    int perpDirn;
    Vector oneOverL;
    Vector data;
    int numNodes;
  protected:
    void free_nodes(void);
    int alloc_nodes(const int &,const int &);
    void free_ndIJ(void);
    void alloc_ndIJ(const int &);
    void set_ndIJ(const ID &nI,const ID &);
    void setup_ndIJ(const int &);

  public:
    DriftRecorder(void);
    DriftRecorder(int ndI, int ndJ, int dof, int perpDirn,
		Domain &theDomain, 
		DataOutputHandler &theHandler,
		bool echoTime = false);

    DriftRecorder(const ID &ndI, const ID &ndJ, int dof, int perpDirn,
		Domain &theDomain, 
		DataOutputHandler &theHandler,
		bool echoTime = false);
  
    ~DriftRecorder(void);

    int record(int commitTag, double timeStamp);
    int restart(void);    

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif
