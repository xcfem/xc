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

// $Revision: 1.3 $
// $Date: 2005/12/15 00:30:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/mesh/element/utils/coordTransformation/CrdTransf.cpp,v $


// File: ~/crdTransf/CrdTransf.C
//
// Written: Remo Magalhaes de Souza (rmsouza@ce.berkeley.edu)
// Created: 04/2000
// Revision: A
//
// Description: This file contains the implementation for the XC::CrdTransf class.
// CrdTransf provides the abstraction of a frame
// coordinate transformation. It is an abstract base class and
// thus no objects of  it's type can be instatiated.

#include "CrdTransf.h"
#include <domain/mesh/node/Node.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>


#include "utility/geom/pos_vec/Pos3dArray.h"
#include "utility/matrix/ID.h"
#include "utility/actor/actor/MovableVector.h"
#include "utility/actor/actor/CommMetaData.h"
#include "preprocessor/prep_handlers/TransfCooHandler.h"


//! @brief Constructor.
XC::CrdTransf::CrdTransf(int tag, int classTag, int dim_joint_offset)
  : TaggedObject(tag), MovableObject(classTag),
    nodeIPtr(nullptr), nodeJPtr(nullptr),L(0.0),nodeIOffset(dim_joint_offset), nodeJOffset(dim_joint_offset), initialDispChecked(false) {}


//! @brief Destructor virtual.
XC::CrdTransf::~CrdTransf(void)
  {}

//! @brief Returns (if possible) a pointer to the coordinate transformation handler (owner).
const XC::TransfCooHandler *XC::CrdTransf::GetTransfCooHandler(void) const
  {
    const XC::TransfCooHandler *retval= dynamic_cast<const TransfCooHandler *>(Owner());
    if(!retval)
      std::cerr << "CrdTransf::" << __FUNCTION__
		<< "; coordinate transformation handler not defined."
		<< std::endl;
    return retval;
  }

//! @brief Returns a pointer to the coordinate transformation handler (if possible).
XC::TransfCooHandler *XC::CrdTransf::GetTransfCooHandler(void)
  {
    XC::TransfCooHandler *retval= dynamic_cast<TransfCooHandler *>(Owner());
    if(!retval)
      std::cerr << "CrdTransf::" << __FUNCTION__
		<< "; coordinate transformation handler not defined."
		<< std::endl;
    return retval;
  }

//! @brief Return the name of the coordinate transformation.
std::string XC::CrdTransf::getName(void) const
  {
    const TransfCooHandler *cthandler= GetTransfCooHandler();
    return cthandler->getName(getTag());
  }


//! @brief Asigna los pointers to node dorsal y frontal.
int XC::CrdTransf::set_node_ptrs(Node *nodeIPointer, Node *nodeJPointer)
  {
    nodeIPtr = nodeIPointer;
    nodeJPtr = nodeJPointer;

    if ((!nodeIPtr) || (!nodeJPtr))
      {
        std::cerr << "CrdTransf::" << __FUNCTION__
		  << "; invalid pointers to the element nodes\n";
        return -1;
      }
    return 0;
  }

//@brief Returns element length.
double XC::CrdTransf::getLength(bool initialGeometry) const
  {
    if(initialGeometry)
      return getInitialLength();
    else
      return getDeformedLength();
  }

const XC::Matrix &XC::CrdTransf::getPointsGlobalCoordFromLocal(const Matrix &localCoords) const
  {
    static Matrix retval;
    const size_t numPts= localCoords.noRows(); //Number of points to transform.
    const size_t dim= localCoords.noCols(); //Space dimension.
    retval.resize(numPts,dim);
    Vector xl(dim);
    Vector xg(dim);
    for(size_t i= 0;i<numPts;i++)
      {
        for(size_t j= 0;j<dim;j++)
          xl[j]= localCoords(i,j);
        xg= getPointGlobalCoordFromLocal(xl);
        for(size_t j= 0;j<dim;j++)
          retval(i,j)= xg[j];
      }
    return retval;
  }

double XC::CrdTransf::getPointBasicCoordFromGlobal(const Vector &globalCoords) const
  {
    Vector cl= getPointLocalCoordFromGlobal(globalCoords);
    return cl[0]/L;
  }

const XC::Vector &XC::CrdTransf::getBasicDisplSensitivity(int gradNumber)
  {
    std::cerr << "CrdTransf::" << __FUNCTION__
	      << "; WARNING - this method "
              << " should not be called." << std::endl;

    static XC::Vector dummy(1);
    return dummy;
  }

const XC::Vector &XC::CrdTransf::getGlobalResistingForceShapeSensitivity(const Vector &pb, const Vector &p0, int gradNumber)
  {
    std::cerr << "CrdTransf::" << __FUNCTION__
	      << "; ERROR - has not been"
              << " implemented yet for the chosen transformation."
	      << std::endl;

    static XC::Vector dummy(1);
    return dummy;
  }

const XC::Vector &XC::CrdTransf::getGlobalResistingForceShapeSensitivity(const XC::Vector &pb, const XC::Vector &p0)
  {
    std::cerr << "CrdTransf::" << __FUNCTION__
	      << "; ERROR - has not been"
              << " implemented yet for the chosen transformation."
	      << std::endl;

    static XC::Vector dummy(1);
    return dummy;
  }

const XC::Vector &XC::CrdTransf::getBasicTrialDispShapeSensitivity(void)
  {
    std::cerr << "CrdTransf::" << __FUNCTION__
	      << "; ERROR  has not been"
              << " implemented yet for the chosen transformation."
	      << std::endl;

    static Vector dummy(1);
    return dummy;
  }

//! @brief Sends object members through the communicator argument.
int XC::CrdTransf::sendData(Communicator &comm)
  {
    //data(0)= getTag();
    int res= comm.sendDouble(L,getDbTagData(),CommMetaData(1));
    res+= comm.sendVector(nodeIOffset,getDbTagData(),CommMetaData(2));
    res+= comm.sendVector(nodeJOffset,getDbTagData(),CommMetaData(3));
    res+= comm.sendVector(nodeIInitialDisp,getDbTagData(),CommMetaData(4));
    res+= comm.sendVector(nodeJInitialDisp,getDbTagData(),CommMetaData(5));
    res+= comm.sendBool(initialDispChecked,getDbTagData(),CommMetaData(6));
    if(res<0)
      std::cerr << "CrdTransf::" << __FUNCTION__
		<< "; failed to send data.\n";
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::CrdTransf::recvData(const Communicator &comm)
  {
    //setTag(data(0));
    int res= comm.receiveDouble(L,getDbTagData(),CommMetaData(1));
    res+= comm.receiveVector(nodeIOffset,getDbTagData(),CommMetaData(2));
    res+= comm.receiveVector(nodeJOffset,getDbTagData(),CommMetaData(3));
    res+= comm.receiveVector(nodeIInitialDisp,getDbTagData(),CommMetaData(4));
    res+= comm.receiveVector(nodeJInitialDisp,getDbTagData(),CommMetaData(5));
    res+= comm.receiveBool(initialDispChecked,getDbTagData(),CommMetaData(6));
    if(res<0)
      std::cerr << "CrdTransf::" << __FUNCTION__
		<< "; failed to receive data.\n";
    return res;    
  }

