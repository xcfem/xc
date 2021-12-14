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

//! @brief Default constructor.
XC::MaxNodeDispRecorder::MaxNodeDispRecorder(void)
  :DomainRecorderBase(RECORDER_TAGS_MaxNodeDispRecorder,nullptr), nodeTags(), maxDisp(),dof(-1)
  {}

//! @brief Constructor.
//!
//! @param theDof: index of the degree of freedom to follow.
//! @param nTags: identifiers of the nodes to follow.
//! @param theDom: domain of the nodes.
XC::MaxNodeDispRecorder::MaxNodeDispRecorder(int theDof,const ID &nTags, Domain &theDom)
  :DomainRecorderBase(RECORDER_TAGS_MaxNodeDispRecorder,&theDom), nodeTags(nTags), maxDisp(nTags.Size()), dof(theDof)
  { if (dof < 0) dof = 0; }

//! For each node in \p nodeTags the value of the \p dof'th committed
//! displacement is obtained. If the absolute value of this is greater
//! than the value currently stored in the Vector of max displacements,
//! the value in the Vector is updated. If no Node exists in {\em
//! theDomain} with the tag or the Node does not have a \p dof'th
//! degree-of-freedom associated with it \f$0\f$ is entered in the
//! Vector. Returns \f$0\f$.
int XC::MaxNodeDispRecorder::record(int commitTag, double timeStamp)
  {
    for (int i=0; i<nodeTags.Size(); i++)
      {
	Node *theNode = theDomain->getNode(nodeTags(i));
	if (theNode != 0)
	  {
	    const Vector &theDisp = theNode->getTrialDisp();
	    if (theDisp.Size() > dof)
	      {
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


//! @brief Prints to clog the vector containing the maximum absolute nodal
//! displacements. Note, at the end of the analysis, what is printed is
//! independent of \p commitTag. Returns \f$0\f$.
int XC::MaxNodeDispRecorder::playback(int commitTag)
  {
    std::clog << "Max Recorded Displacement: " << maxDisp << std::endl;
    return 0;
  }


//! @brief Zeros the Vector of maximum nodal displacements.
int XC::MaxNodeDispRecorder::restart(void)
  {
    maxDisp.Zero();
    return 0;
  }

