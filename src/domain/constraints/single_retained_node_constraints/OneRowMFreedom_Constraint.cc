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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/12/22 00:35:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/single_retained_node_constraints/OneRowMFreedom_Constraint.cpp,v $
                                                                        
                                                                        
// File: ~/domain/constraints//OneRowMFreedom_Constraint.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of class XC::OneRowMFreedom_Constraint.
//
// The class XC::OneRowMFreedom_Constraint interface:
//

#include <domain/constraints/single_retained_node_constraints/OneRowMFreedom_Constraint.h>

#include <cstdlib>
#include <utility/matrix/ID.h>
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>
#include <boost/any.hpp>

#include "utility/matrices/m_int.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/actor/CommMetaData.h"
#include "vtkCellType.h"

//! !@brief Constructor. //Arash
//!
//! @param tag: constraint identifier.
//! @param classTag: constraint class identifier.
XC::OneRowMFreedom_Constraint::OneRowMFreedom_Constraint(int tag,int classTag)		
 : MFreedom_Constraint(tag,0,classTag),
   retainedNodeTag(0) {}


//! Constructor to be called from subclasses.
//!
//! @param tag: multi-freedom constraint identifier.
//! @param nodeRetain: identifier of the retained node.
//! @param nodeConstr: identifier of the constrained node.
//! @param classTag: constraint class identifier.
XC::OneRowMFreedom_Constraint::OneRowMFreedom_Constraint(int tag, int nodeRetain, int nodeConstr, int classTag)
 : MFreedom_Constraint(tag, nodeConstr, classTag),
   retainedNodeTag(nodeRetain)
   {}

//! Constructor to be called from subclasses.
//!
//! @param tag: identifier for the multi-freedom constraint.
//! @param nodeRetain: identifier of the retained node.
//! @param nodeConstr: identifier of the constrained node.
//! @param constrainedDOF: constrained degrees of freedom.
//! @param retainedDOF: retained degrees of freedom.
XC::OneRowMFreedom_Constraint::OneRowMFreedom_Constraint(int tag, int nodeRetain, int nodeConstr,const ID &constrainedDOF,const ID &retainedDOF, int classTag)
  : MFreedom_Constraint(tag, nodeConstr, constrainedDOF, retainedDOF, classTag),
    retainedNodeTag(nodeRetain)
    {}


//! @brief Constructor.
//!
//! @param tag: identifier for the multi-freedom constraint.
//! @param nodeRetain: identifier of the retained node.
//! @param nodeConstr: identifier of the constrained node.
//! @param constr: constraint matrix.
//! @param constrainedDOF: constrained degrees of freedom.
//! @param retainedDOF: retained degrees of freedom.
XC::OneRowMFreedom_Constraint::OneRowMFreedom_Constraint(int tag, int nodeRetain, int nodeConstr, Matrix &constr, ID &constrainedDOF, ID &retainedDOF, int classTag)
  : MFreedom_Constraint(tag, nodeConstr, constr, constrainedDOF, retainedDOF, classTag),
  retainedNodeTag(nodeRetain)
  {}

//! @brief Sends data through the communicator argument.
int XC::OneRowMFreedom_Constraint::sendData(Communicator &comm)
  {
    int res= MFreedom_Constraint::sendData(comm);
    res+= comm.sendInt(retainedNodeTag,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives data through the communicator argument.
int XC::OneRowMFreedom_Constraint::recvData(const Communicator &comm)
  {
    int res= MFreedom_Constraint::recvData(comm);
    res+= comm.receiveInt(retainedNodeTag,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Return a Python dictionary with the object members values.
boost::python::dict XC::OneRowMFreedom_Constraint::getPyDict(void) const
  {
    boost::python::dict retval= MFreedom_Constraint::getPyDict();
    retval["retainedNodeTag"]= this->retainedNodeTag;
    return retval;
  }
//! @brief Set the values of the object members from a Python dictionary.
void XC::OneRowMFreedom_Constraint::setPyDict(const boost::python::dict &d)
  {
    MFreedom_Constraint::setPyDict(d);
    this->retainedNodeTag= boost::python::extract<int>(d["retainedNodeTag"]);
  }

//! @brief Sends object through the communicator argument.
int XC::OneRowMFreedom_Constraint::sendSelf(Communicator &comm)
  {
    inicComm(10);
    int result= sendData(comm);
    const int dataTag= getDbTag();
    result = comm.sendIdData(getDbTagData(),dataTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; WARNING - error sending ID data\n";
    return result;
  }


//! @brief Receives object through the communicator argument.
int XC::OneRowMFreedom_Constraint::recvSelf(const Communicator &comm)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;
  }

//! @brief Printing.
void XC::OneRowMFreedom_Constraint::Print(std::ostream &s, int flag) const
  {     
    s << "MFreedom_Constraint: " << this->getTag() << "\n";
    s << "\tNode Constrained: " << getNodeConstrained();
    s << " node Retained: " << getNodeRetained() ;
    s << " constrained dof: " << constrDOF;    
    s << " retained dof: " << retainDOF;        
    s << " constraint matrix: " << constraintMatrix << "\n";
  }



