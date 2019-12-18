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

// $Revision: 1.4 $
// $Date: 2005/01/08 01:22:41 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/RigidRod.cpp,v $


// File: ~/model/constraints/RigidRod.C
//
// Written: fmk 12/99
// Revised:
//
// Purpose: This file contains the class implementation for XC::RigidRod.
#include "RigidRod.h"

#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/ID.h>

//! @brief Object setup.
void XC::RigidRod::setup(Domain *theDomain)
  {
    RigidBase::setDomain(theDomain);

    //get the coordinates of the two nodes - check dimensions are the same
    const Vector &crdR = nodeR->getCrds();
    const Vector &crdC = nodeC->getCrds();
    int dimR = crdR.Size();
    int dimC = crdC.Size();
    if(dimR != dimC)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; mismatch in dimension "
		  << "between constrained node " <<  getNodeConstrained()
		  <<  " and Retained node " << getNodeRetained() << std::endl;
        return;
      }

    // check the number of dof at each node is the same
    const int numDOF = nodeR->getNumberDOF();
    if(numDOF != nodeC->getNumberDOF())
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; mismatch in numDOF "
		  << "between constrained node " <<  getNodeConstrained()
		  <<  " and retained node " << getNodeRetained() << std::endl;
        return;
      }

    // check the number of dof at the nodes >= dimension of problem
    if(numDOF < dimR)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << ";  - numDOF at nodes " << getNodeRetained()
		  << " and " << getNodeConstrained()
		  << "must be >= dimension of problem\n";
        return;
      }


    // create the ID to identify the constrained dof
    ID id(dimR);

    // construct the transformation matrix Ccr, where  Uc = Ccr Ur & set the diag
    Matrix mat(dimR,dimR);
    mat.Zero();

    // set the values
    for(int i=0; i<dimR; i++)
      {
        mat(i,i) = 1.0;
        id(i) = i;
      }

    set_constraint(mat);
    set_constrained_retained_dofs(id,id);
  }

//! @brief Constructor.
XC::RigidRod::RigidRod(int mPtag)
  : RigidBase(mPtag) {}


//! @brief Constructor.
XC::RigidRod::RigidRod(int mPtag,const int &nm, const int &ns)
  : RigidBase(mPtag,nm,ns,0) {}

