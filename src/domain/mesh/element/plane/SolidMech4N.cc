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

// $Revision: 1.24 $
// $Date: 2003/10/07 21:18:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/element/fourNodeQuad/SolidMech4N.cpp,v $

// Written: MHS
// Created: Feb 2000
// Revised: Dec 2000 for efficiency
//
// Description: This file contains the class definition for XC::SolidMech4N.

#include "SolidMech4N.h"
#include <domain/mesh/node/Node.h>
#include <material/nD/NDMaterial.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include <domain/domain/Domain.h>

//! @brief Default constructor.
XC::SolidMech4N::SolidMech4N(int tag, int classTag, const SolidMech2D &pp)
  :QuadBase4N<SolidMech2D>(tag, classTag, pp) {}

//! @brief Constructor.
//!
//! @param nd1, nd2, nd3, nd3: four nodes defining element boundaries,
//!                            input in counterclockwise order around
//!                            the element.
//! @param t: element thickness.
//! @param type: string representing material behavior. Valid options
//!              depend on the NDMaterial object and its available material
//!              formulations. The type parameter can be either "PlaneStrain"
//!              or "PlaneStress."
//! @param m: element material.
XC::SolidMech4N::SolidMech4N(int tag, int classTag, int nd1, int nd2, int nd3, int nd4, const SolidMech2D &pp)
  : QuadBase4N<SolidMech2D>(tag, classTag, nd1, nd2, nd3, nd4, pp) {}

//! @brief Checks the material type.
bool XC::SolidMech4N::check_material_type(const std::string &type) const
  { return physicalProperties.check_material_elast_plana(type); }

//! @brief Check the element.
void XC::SolidMech4N::checkElem(void)
  {
    QuadBase4N<SolidMech2D>::checkElem();

    // Check if nodes are ordered counterclockwise
    if(!this->getNodePtrs().hasNull())
      {
	const Pos3d vPoint(0,0,100);
        if(this->clockwise(vPoint, false)) // if not reverse them.
	  { this->getNodePtrs().reverse(); }
      }
  }

//! @brief Send object members through the communicator argument.
int XC::SolidMech4N::sendData(Communicator &comm)
  {
    int res=  QuadBase4N<SolidMech2D>::sendData(comm);
    res+= comm.sendMatrix(Ki,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::SolidMech4N::recvData(const Communicator &comm)
  {
    int res=  QuadBase4N<SolidMech2D>::recvData(comm);
    res+= comm.receiveMatrix(Ki,getDbTagData(),CommMetaData(8));
    return res;
  }
