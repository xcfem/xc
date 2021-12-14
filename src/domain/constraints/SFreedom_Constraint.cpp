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
// $Date: 2005/12/22 00:35:08 $
// $Source: /usr/local/cvs/OpenSees/SRC/domain/constraints/SFreedom_Constraint.cpp,v $
                                                                        
                                                                        
// File: ~/domain/constraints/SFreedom_Constraint.C
//
// Written: fmk 
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the implementation of class XC::SFreedom_Constraint.

#include <domain/constraints/SFreedom_Constraint.h>
#include <utility/matrix/Vector.h>
#include "utility/matrix/ID.h"
#include <utility/actor/objectBroker/FEM_ObjectBroker.h>

#include "utility/matrices/m_int.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/node/Node.h"
#include "utility/actor/actor/MovableVector.h"
#include "vtkCellType.h"

//! @brief Default constructor.
//! 
//! Provided for the FEM\_ObjectBroker to be able to instantiate an
//! object; the data for this object will be read from a Channel object
//! when recvSelf() is invoked.
//!
//! @param classTag: class identifier.
XC::SFreedom_Constraint::SFreedom_Constraint(int classTag)
  : Constraint(classTag), dofNumber(0), valueR(0.0), valueC(0.0), isConstant(true), loadPatternTag(-1) {}

//! @brief Constructor.
//!
//! @param tag: Constraint identifier.
//! @param nodeTag: identifier of the node to constraint tag.
XC::SFreedom_Constraint::SFreedom_Constraint(int tag, int nodeTag)
  : Constraint(tag, nodeTag,CNSTRNT_TAG_SFreedom_Constraint), dofNumber(0), valueR(0.0), valueC(0.0),
    isConstant(true), loadPatternTag(-1) {}

//! @brief Constructor for a subclass to use.
//! 
//! Provided for subclasses to use. The subclasses can vary the value of the
//! imposed displacement when getValue() is invoked. If this
//! constructor is used the isHomogeneous() method will always
//! return \p false. The integer value \p tag is used to identify
//! the SFreedom\_Constraint among all other SFreedom\_Constraints.
//!
//! @param tag: Constraint identifier.
//! @param nodeTag: identifier of the node to constraint.
//! @param ndof: DOF number to constraint.
//! @param classTag: class identifier.
XC::SFreedom_Constraint::SFreedom_Constraint(int tag, int nodeTag, int ndof, int classTag)
  :Constraint(tag, nodeTag,classTag), dofNumber(ndof), valueR(0.0), valueC(0.0), isConstant(true), 
   loadPatternTag(-1)
 // valueC is set to 1.0 so that homo will be false when recvSelf() invoked
 // should be ok as valueC cannot be used by subclasses and subclasses should
 // not be used if it is a homogeneous constraint.
  {}

//! @brief Constructor.
//!
//! To construct a single point constraint to constrain the trial
//! displacement of the \p ndof'th dof at node \p node to the value
//! given by \p value. The integer value \p tag is used to identify
//! the SFreedom\_Constraint among all other SFreedom\_Constraints. If
//! \p value is equal to \f$0.0\f$ the method isHomogeneous() will
//! always return \p true, otherwise \p false. 
//!
//! @param tag: Constraint identifier.
//! @param nodeTag: identifier of the node to constraint.
//! @param ndof: DOF number to constraint.
//! @param value: prescribed value for DOF.
//! @param ISconstant: True if the prescribed value is constant in time.
XC::SFreedom_Constraint::SFreedom_Constraint(int tag, int nodeTag, int ndof, double value, bool ISconstant)
  :Constraint(tag, nodeTag, CNSTRNT_TAG_SFreedom_Constraint), dofNumber(ndof), valueR(value), valueC(value), isConstant(ISconstant),
 loadPatternTag(-1) {}

//! @brief Virtual constructor.
XC::SFreedom_Constraint *XC::SFreedom_Constraint::getCopy(void) const
  { return new SFreedom_Constraint(*this); }

//! @brief Returns a copy of the constraint with a different tag,
//! it's used in BoundaryCondHandler::addSFreedom_Constraint(nodeTag,seed).
XC::SFreedom_Constraint *XC::SFreedom_Constraint::getCopy(const int &spTag) const
  { 
    SFreedom_Constraint *retval= new SFreedom_Constraint(*this);
    retval->setTag(spTag);
    return retval;
  }

//! @brief Returns the constrained DOF number.
int XC::SFreedom_Constraint::getDOF_Number(void) const
  { return dofNumber; }

//! @brief Returns true if the constraint affects the node and DOF arguments.
bool XC::SFreedom_Constraint::affectsNodeAndDOF(int nodeTag, int theDOF) const
  {
    bool retval= false;
    if(affectsNode(nodeTag))
      retval= (theDOF==dofNumber);
    return retval;
  }

//! @brief Returns the value of the prescribed displacement.
//! 
//! To return the value of the constraint determined in the last call to
//! applyConstraint(). This base class returns \p value passed in
//! the constructor. 
double XC::SFreedom_Constraint::getValue(void) const
  { return valueC; }

//! @brief Applies the constraint with the load factor
//! being passed as parameter.
//!
//! To set the value of the constraint for the load factor given by {\em
//! loadFactor}. The constraint is set equal to \p loadFactor * {\em
//! value} if the constraint is not constant, or \p value if the
//! constraint was identified as constant in the constructor.
int XC::SFreedom_Constraint::applyConstraint(double loadFactor)
  {
    // as SFreedom_Constraint objects are time invariant nothing is done
    if(isConstant == false)
      valueC= loadFactor*valueR;
    return 0;
  }

//! @brief Returns true if it's an homogeneous constraint
//! (prescribed value for the DOF is zero).
//!
//! To return a boolean indicating whether or not the constraint is a
//! homogeneous constraint. A homogeneous constraint is one where the value
//! of the constraint, \p value, is always \f$0\f$. This information can be
//! used by the ConstraintHandler to reduce the number of equations in the
//! system.
bool XC::SFreedom_Constraint::isHomogeneous(void) const
  {
    if(valueR == 0.0)
      return true;
    else
      return false;
  }

//! @brief Sets the load pattern tag for the constraint.
//!
//! @param tag: load pattern identifier.
void XC::SFreedom_Constraint::setLoadPatternTag(int tag)
  { loadPatternTag = tag; }

//! @brief Return the load pattern tag of the constraint.
int XC::SFreedom_Constraint::getLoadPatternTag(void) const
  { return loadPatternTag;  }

//! @brief Returns a vector to store the dbTags of the object members.
XC::DbTagData &XC::SFreedom_Constraint::getDbTagData(void) const
  {
    static DbTagData retval(7);
    return retval;
  }

//! @brief Sends data through the communicator argument.
int XC::SFreedom_Constraint::sendData(Communicator &comm)
  {
    int res= Constraint::sendData(comm);
    res+= comm.sendInts(dofNumber,loadPatternTag,getDbTagData(),CommMetaData(3));
    res+= comm.sendDoubles(valueC,valueR,getDbTagData(),CommMetaData(4));
    res+= comm.sendBool(isConstant,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives data through the communicator argument.
int XC::SFreedom_Constraint::recvData(const Communicator &comm)
  {
    int res= Constraint::recvData(comm);
    res+= comm.receiveInts(dofNumber,loadPatternTag,getDbTagData(),CommMetaData(3));
    res+= comm.receiveDoubles(valueC,valueR,getDbTagData(),CommMetaData(4));
    res+= comm.receiveBool(isConstant,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::SFreedom_Constraint::sendSelf(Communicator &comm)
  {
    inicComm(7);
    int result= sendData(comm);
    const int dbTag= getDbTag();
    result+= comm.sendIdData(getDbTagData(),dbTag);
    if(result < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send extra data.\n";
    return result;
  }

//! @brief Receives object through the communicator argument.
int XC::SFreedom_Constraint::recvSelf(const Communicator &comm)
  {
    inicComm(7);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;
  }

//! @brief Returns the cell type to be used to represent
//! the object in VTK graphics.
int XC::SFreedom_Constraint::getVtkCellType(void) const
  { return VTK_VERTEX; }


//! @brief Prints constraint information.
void XC::SFreedom_Constraint::Print(std::ostream &s, int flag) const
  {
    Constraint::Print(s,flag);
    s << " DOF: " << dofNumber;
    s << " ref value: " << valueR << " current value: " << valueC << std::endl;
  }

