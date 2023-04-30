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
#include "domain/load/plane/QuadRawLoad.h"

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
//! @brief Defines a load over the element from a vector of nodal loads
//! in local coordinates.
//!
//! @param nLoads: loads on each element node.
const XC::QuadRawLoad *XC::SolidMech4N::vector2dRawLoadLocal(const std::vector<Vector> &nLoads)
  {
    QuadRawLoad *retval= nullptr;
    Preprocessor *preprocessor= getPreprocessor();
    if(preprocessor)
      {
        MapLoadPatterns &lPatterns= preprocessor->getLoadHandler().getLoadPatterns();
        static ID eTags(1);
        eTags[0]= getTag(); //Load for this element.
        const int &loadTag= lPatterns.getCurrentElementLoadTag(); //Load identifier.

        const size_t sz= nLoads.size();
	const size_t nn= getNumExternalNodes();
        if(sz==nn)
          {
            LoadPattern *lp= lPatterns.getCurrentLoadPatternPtr();
            if(lp)
              {
                retval= new QuadRawLoad(loadTag,nLoads,eTags);
                lp->addElementalLoad(retval);
                lPatterns.setCurrentElementLoadTag(loadTag+1);
              }
            else
	      std::cerr << getClassName() << "::" << __FUNCTION__
                        << " there is no current load pattern."
                        << " Load ignored." << std::endl; 
          }
        else
          std::cerr << getClassName() << "::" << __FUNCTION__
                    << "; a vector of dimension " << nn
	            << " was expected." << std::endl;
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; modeler not defined." << std::endl;
    return retval;
  }

//! @brief Defines a load over the element from a vector of nodal loads
//! in global coordinates.
//!
//! @param nLoads: loads on each element node.
const XC::QuadRawLoad *XC::SolidMech4N::vector2dRawLoadGlobal(const std::vector<Vector> &nLoads)
  {
    const QuadRawLoad *retval= nullptr;
    const size_t sz= nLoads.size();
    const size_t nn= getNumExternalNodes();
    if(sz==nn)
      {
        assert(theCoordTransf);
	std::vector<Vector> tmp(nn);
	for(size_t i= 0;i<nn;i++)
	  {
	    tmp[i]= nLoads[i]; // No transformation needed.
	  }
        retval= vector2dRawLoadLocal(tmp);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; a vector of dimension " << nn
                << " was expected." << std::endl;
    return retval;
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
