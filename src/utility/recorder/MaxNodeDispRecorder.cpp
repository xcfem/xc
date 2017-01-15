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
// $Date: 2004/11/24 22:45:28 $
// $Source: /usr/local/cvs/OpenSees/SRC/recorder/MaxNodeDispRecorder.cpp,v $
                                                                        
                                                                        
// File: ~/recorder/MaxNodeDispRecorder.C
//
// Written: fmk 
// Created: 11/98
// Revision: A
//
// Description: This file contains the class definition for XC::MaxNodeDispRecorder.
// A XC::MaxNodeDispRecorder is used to determine the max nodal displacement
// at a collection of nodes over an analysis. (between commitTag of 0 and
// last commitTag).
//
// What: "@(#) MaxNodeDispRecorder.C, revA"

#include <utility/recorder/MaxNodeDispRecorder.h>
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>

XC::MaxNodeDispRecorder::MaxNodeDispRecorder(void)
  :DomainRecorderBase(RECORDER_TAGS_MaxNodeDispRecorder,nullptr), theNodes(), maxDisp(),dof(-1)
  {}

XC::MaxNodeDispRecorder::MaxNodeDispRecorder(int theDof,const XC::ID &nodes, Domain &theDom)
  :DomainRecorderBase(RECORDER_TAGS_MaxNodeDispRecorder,&theDom), theNodes(nodes), maxDisp(nodes.Size()), 
 dof(theDof)
  { if (dof < 0) dof = 0; }

int XC::MaxNodeDispRecorder::record(int commitTag, double timeStamp)
  {
    for (int i=0; i<theNodes.Size(); i++) {
	Node *theNode = theDomain->getNode(theNodes(i));
	if (theNode != 0) {
	    const XC::Vector &theDisp = theNode->getTrialDisp();
	    if (theDisp.Size() > dof) {
		double disp = theDisp(dof);
		if (disp > 0 && disp > maxDisp(i))
		    maxDisp(i) = disp;
		else if (disp < 0 && -disp > maxDisp(i))
		    maxDisp(i) = -disp;
	    }
	}
    }
    return 0;
  }


int XC::MaxNodeDispRecorder::playback(int commitTag)
  {
    std::cerr << "Max Recorded Displacement: " << maxDisp << std::endl;
    return 0;
  }


int XC::MaxNodeDispRecorder::restart(void)
  {
    maxDisp.Zero();
    return 0;
  }

