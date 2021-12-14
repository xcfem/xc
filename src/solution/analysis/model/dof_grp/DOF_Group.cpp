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

// $Revision: 1.13 $
// $Date: 2005/11/29 23:31:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/dof_grp/DOF_Group.cpp,v $


// File: ~/analysis/model/dof_grp/DOF_Group.C
//
// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::DOF_Group class interface.
//
// What: "@(#) DOF_Group.C, revA"


#include "DOF_Group.h"
#include <cstdlib>

#include "domain/mesh/node/Node.h"
#include "utility/matrix/Vector.h"
#include "utility/matrix/Matrix.h"
#include "solution/analysis/integrator/TransientIntegrator.h"

const int MAX_NUM_DOF= 256;


// static variables initialisation
XC::Matrix XC::DOF_Group::errMatrix(1,1);
XC::Vector XC::DOF_Group::errVect(1);
XC::UnbalAndTangentStorage XC::DOF_Group::unbalAndTangentArray(MAX_NUM_DOF+1);
int XC::DOF_Group::numDOF_Groups(0); // number of objects


void XC::DOF_Group::inicID(void)
  {
    const int sz= getNumDOF();
    // get number of DOF & verify valid
    if(sz <= 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; node must have at least 1 dof." << sz;
        exit(-1);
      }

    // initially set all the IDs to be -2
    for(int i=0; i<sz; i++)
      myID(i)= -2;
  }

//! @brief Constructor that take the corresponding model node.
//!
//!
//! Constructs a  DOF\_Group with an associated node given by {\em
//! theNode} and a tag given by \p tag. During construction it
//! determines the number of unknown dofs from the node, \p numDOF, and
//! creates an Vector object to hold the unbalance information and an ID
//! object to hold mapping between degrees-of-freedom and equation
//! numbers. All values of the ID are set to \f$-2\f$ initially. It creates a
//! Vector to hold the unbalance loads and an ID to 
//! hold the mapping information, both of size equal to the number of
//! unknowns. If the size of the Vector or the ID is \f$0\f$, i.e. ran out of
//! memory, a warning message is orinted and \p numDOF set to \f$0\f$.
//!
//! @param tag: object identifier.
//! @param node: node associated to the DOF group.
XC::DOF_Group::DOF_Group(int tag, Node *node)
  :TaggedObject(tag), myID(node->getNumberDOF()),
   unbalAndTangent(node->getNumberDOF(),unbalAndTangentArray), myNode(node)
   
  {
    inicID();
    numDOF_Groups++;
  }

//! @brief Constructor.
//!
//! Provided for subclasses. Constructs a  DOF\_Group with the number of
//! unknown dofs given by \p numDOF and a tag given by \p tag. No
//! Node is associated with this DOF\_Group. Creates a Vector of size {\em
//! numDOF} to hold the unbalance information and an ID object to
//! hold mapping between degrees-of-freedom and equation numbers. All
//! values of the ID are set to \f$-2\f$ initially. If
//! the size of the Vector or the ID is \f$0\f$, i.e. ran out of memory, a
//! warning message is orinted and \p numDOF set to \f$0\f$.
XC::DOF_Group::DOF_Group(int tag, int ndof)
  :TaggedObject(tag), myID(ndof),
   unbalAndTangent(ndof,unbalAndTangentArray), myNode(nullptr)
  {
    inicID();
    numDOF_Groups++;
  }

//! @brief Destructor.
XC::DOF_Group::~DOF_Group(void)
  {
    numDOF_Groups--;

    //numDOF= unbalAndTangent.getNumDOF();

    // set the pointer in the associated Node to nullptr, to stop
    // segmentation fault if node tries to use this object after destroyed
    if(myNode)
      {
        myNode->setDOF_GroupPtr(nullptr);
        myNode= nullptr;
      }
  }

//! @brief to set the corresponding index of the ID to value.
//!
//! Operation to set the equation number of the {\em dof'th} DOF in the
//! DOF\_Group to \p value. In this base class, if \p index is a
//! valid location, \f$0 < index < numDOF-1\f$ the operator {\em (index) =
//! value} is invoked on the ID object created for the mapping, otherwise
//! an error message is printed. 
void XC::DOF_Group::setID(int index, int value)
  {
    const int sz= getNumDOF(); 
    if((index >= 0) && (index < sz))
      myID(index) = value;
    else
      {
        std::cerr << "WARNING DOF_Group::setID - invalid location ";
        std::cerr << index << " in ID of size " << getNumDOF() << std::endl;
      }
  }

//! @brief Method to set the ID to be same as that given.
//! 
//! An operation to set the equation numbers of all the DOFs in the
//! DOF\_Group to be those given in the ID \p values. In this base
//! class, the operator {\em = values} is invoked on the ID object created
//! for the mapping. 
void XC::DOF_Group::setID(const ID &copy)
  { myID= copy; }

//! @brief Method to return the current ID.
//! 
//! A const member function which returns in an ID object the equation
//! numbers associated with the degrees-of-freedom in the DOF\_Group. The
//! size of the ID object is equal to the number of degrees-of-freedom
//! represented by the DOF\_Group. This base class returns the ID created
//! for the mapping. 
const XC::ID &XC::DOF_Group::getID(void) const
  { return myID; }

int XC::DOF_Group::inicID(const int &value)
  {
    const ID &id= getID();
    const int sz= id.Size();
    for(int j=0;j<sz;j++)
      setID(j,value);
    return sz;
  }

//! @brief Returns the tag of the associated node (-1 if there is no
//! associated node).
int XC::DOF_Group::getNodeTag(void) const
  {
    if(myNode)
      return myNode->getTag();
    else
      return -1;
  }

//! @brief Returns the total number of \p free DOFs.
//! 
//! Returns the total number of \p free DOFs in the DOF\_Group,
//! i.e. number of dof in the group who have not been assigned a negative
//! equation number. Determines this by looping through the ID containing
//! the mapping information.
int XC::DOF_Group::getNumFreeDOF(void) const
  {
    const int sz= getNumDOF();
    int numFreeDOF= sz;
    for(int i=0; i<sz; i++)
      if(myID(i) == -1)
        numFreeDOF--;
    return numFreeDOF;
  }

//! @brief Returns the total number of \p constrained DOFs.
//! 
//! Returns the total number of \p constrained DOFs in the DOF\_Group,
//! i.e. number of dof in the group who have been assigned a negative
//! equation number. Determines this by looping through the ID containing
//! the mapping information.
int XC::DOF_Group::getNumConstrainedDOF(void) const
  {
    int numConstr = 0;
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      if(myID(i) < 0)
        numConstr++;
    return numConstr;
  }


//! @brief Returns the current tangent matrix for the DOF\_Group.
//!
//! Returns the current tangent matrix for the DOF\_Group. If no tangent
//! matrix has been created, one of size(numDOF,numDOF) is constructed.
//! If not enough space is available for a new tangent matrix, an error
//! message is printed and an error Matrix is returned..  
const XC::Matrix &XC::DOF_Group::getTangent(Integrator *theIntegrator)
  {
    if(theIntegrator)
      theIntegrator->formNodTangent(this);
    return unbalAndTangent.getTangent();
  }

//! @brief To zero the tangent matrix.
//! 
//! To zero the tangent. If a tangent matrix has been allocated,
//! will invoke Zero() on the tangent matrix.
void XC::DOF_Group::zeroTangent(void)
  { unbalAndTangent.getTangent().Zero(); }


//! @brief To add \p fact times the nodal mass matrix to the tangent
//! matrix.
//! 
//! To add \p fact times the nodal mass matrix to the tangent
//! matrix. In this base class, if a Matrix to store the tangent has not
//! yet been created, one of size (numDOF,numDOF) is now created; if
//! construction of this matrix fails an error is printed and an error
//! Matrix is returned. Invokes {\em addMatrix(theNode-\f$>\f$getMass,fact)}
//! on the Matrix, printing a warining message if this method returns a
//! \f$negative\f$ value. If no Node is associated with the DOF\_Group an
//! error message is printed and nothing is done.
void  XC::DOF_Group::addMtoTang(double fact)
  {
    if(myNode != 0)
      {
        if(unbalAndTangent.getTangent().addMatrix(1.0, myNode->getMass(), fact) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << ";  invoking addMatrix() on the tangent failed.\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; no node associated, "
	        << "subclass should provide the method.\n";
  }


void XC::DOF_Group::addCtoTang(double fact)
  {
    if(myNode != 0)
      {
        if(unbalAndTangent.getTangent().addMatrix(1.0, myNode->getDamp(), fact) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; invoking addMatrix() on the tangent failed.\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; no node associated, "
	        << "subclass should provide the method.\n";
  }

//! @brief To zero the unbalance vector.
//! 
//! To zero the unbalance vector. Invokes Zero() on the vector
//! object used to store the unbalance information.
void XC::DOF_Group::zeroUnbalance(void)
  { unbalAndTangent.getResidual().Zero(); }

//! @brief Returns the vector holding the unbalance.
const XC::Vector &XC::DOF_Group::getUnbalance(Integrator *theIntegrator)
  {
    if(theIntegrator)
      theIntegrator->formNodUnbalance(this);
    return unbalAndTangent.getResidual();
  }

//! @brief Adds the product of the unbalanced load at the node and \p fact to
//! the unbalance vector.
//!
//! Adds the product of the unbalanced load at the node and \p fact to
//! the unbalance vector. A warning message is printed and nothing is done
//! if no node is associated with the DOF\_Group, otherwise {\em
//! addVector(theNode-\f$>\f$getUnbalancedLoad(),fact)} is invoked on the
//! unbalance vector and a warning message is printed if this method
//! returns a negative value. 
void XC::DOF_Group::addPtoUnbalance(double fact)
  {
    if(myNode != 0)
      {
        if(unbalAndTangent.getResidual().addVector(1.0, myNode->getUnbalancedLoad(), fact) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; invoking addVector() on the unbalance failed.\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; no node associated, "
	        << "subclass should provide the method.\n";
  }


void XC::DOF_Group::addPIncInertiaToUnbalance(double fact)
  {
    if(myNode)
      {
        if(unbalAndTangent.getResidual().addVector(1.0, myNode->getUnbalancedLoadIncInertia(), fact) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; invoking addVector() on the unbalance failed\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; no node associated, "
	        << "subclass should provide the method.\n";
  }


void XC::DOF_Group::addM_Force(const XC::Vector &Udotdot, double fact)
  {
    if(!myNode)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no node associated, "
		  << " subclass should not call this method \n";
        return;
      }
    else
      {
	const int sz= getNumDOF();
	Vector accel(sz);
	// get accel for the unconstrained dof
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
	      accel(i)= Udotdot(loc);
	    else
	      accel(i)= 0.0;
	  }

	if(unbalAndTangent.getResidual().addMatrixVector(1.0, myNode->getMass(), accel, fact) < 0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; invoking addMatrixVector() "
		    << " on the unbalance failed.\n";
      }
  }



const XC::Vector &XC::DOF_Group::getTangForce(const XC::Vector &Udotdot, double fact)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not yet implemented";
    return unbalAndTangent.getResidual();
  }


const XC::Vector &XC::DOF_Group::getM_Force(const XC::Vector &Udotdot, double fact)
  {
    if(!myNode)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no node associated"
		  << " subclass should not call this method \n";
        return unbalAndTangent.getResidual();
      }

    const int sz= getNumDOF();
    Vector accel(sz);
    // get accel for the unconstrained dof
    for (int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
            accel(i) = Udotdot(loc);
        else accel(i) = 0.0;
      }

    if(unbalAndTangent.getResidual().addMatrixVector(0.0, myNode->getMass(), accel, fact) < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; invoking addMatrixVector() on the unbalance failed.\n";
    return unbalAndTangent.getResidual();
  }



const XC::Vector &XC::DOF_Group::getC_Force(const XC::Vector &Udotdot, double fact)
  {
    if(!myNode)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; no XC::Node associated, "
	        << " subclass should not call this method \n";
    else
      {
	const int sz= getNumDOF();
	Vector accel(sz);
	// get accel for the unconstrained dof
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
	      accel(i) = Udotdot(loc);
	    else
	      accel(i) = 0.0;
	  }

	if(unbalAndTangent.getResidual().addMatrixVector(0.0, myNode->getDamp(), accel, fact) < 0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; invoking addMatrixVector() on the unbalance failed.\n";
      }
    return unbalAndTangent.getResidual();
  }


//! @brief To return the committed displacement at the associated node.
//! 
//! To return the committed displacement at the associated node. Returns the
//! result of invoking getDisp() on the node. If there is no associated
//! node object, an error message is printed and an error Vector is
//! returned.
const XC::Vector &XC::DOF_Group::getCommittedDisp(void)
  {
    if(!myNode)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no associated node, "
		  << " returning the error Vector.\n";
        return errVect;
      }
    else
      return myNode->getDisp();
  }

//! Return the committed velocity at the associated node.
//! 
//! To return the committed velocity at the associated node. Returns the
//! result of invoking getVel() on the node. If there is no associated node
//! object, an error message is printed and an error Vector is returned.
const XC::Vector &XC::DOF_Group::getCommittedVel(void)
  {
    if(!myNode)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no associated node "
		  << " returning the error Vector.\n";
        return errVect;
      }
    else
      return myNode->getVel();
  }

//! @brief Return the committed acceleration at the associated node.
//! 
//! Return the committed acceleration at the associated node. Returns result of
//! invoking  getAccel() on the Node. If there is no associated node
//! object, an error message is printed and an error Vector is returned.
const XC::Vector &XC::DOF_Group::getCommittedAccel(void)
  {
    if(!myNode)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; no associated node "
		  << " returning the error Vector.\n";
        return errVect;
      }
    else
      return myNode->getAccel();
  }

//! @brief Method to set the corresponding nodes displacements to the
//! values in u, components identified by myID.
//!
//! This operation sets the value of the nodal trial displacement at the
//! associated node. The vector \p u is of size equal to the number of
//! equations in the AnalysisModel (this is not checked by the DOF\_Group).
//! The DOF\_Group object accesses the contents of the Vector \p u using
//! its assigned equation numbers. If a \f$-1\f$ exists for a DOF a \f$0\f$ value
//! is set as the corresponding nodal displacement for the node. Creates a Vector
//! to store the correct components of \p u, then invokes setTrialDisp()
//! on the node object; if no node object an error message is
//! printed. CURRENTLY THERE IS NO CHECK TO SEE IF myID(i) DOES NOT OVERFLOW
//! ADMISSIBLE VALUES IN U - THIS NEES TO CHANGE
void XC::DOF_Group::setNodeDisp(const Vector &u)
  {
    if(!myNode)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; no associated node." << std::endl;
    else
      {
	Vector &disp= unbalAndTangent.getResidual();
	disp= myNode->getTrialDisp();

	// get disp for my dof out of vector u
	const int sz= getNumDOF();
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
	      disp(i) = u(loc);
	  }
	myNode->setTrialDisp(disp);
      }
  }

//! @brief Method to set the corresponding nodes velocities to the
//!  values in udot, components identified by myID.
//!
//! This operation sets the value of the nodal trial velocity at the
//! associated node. The vector {\em \f$u^{.}\f$} is of size equal to the number of
//! equations in the AnalysisModel (this is not checked by the DOF\_Group).
//! The DOF\_Group object accesses the contents of the Vector {\em \f$u^{.}\f$} using
//! its assigned equation numbers. If a \f$-1\f$ exists for a DOF a \f$0\f$ value
//! is set as the corresponding nodal velocity for the node.  Creates a Vector
//! to store the correct components of \p u, then invokes setTrialVel()
//! on the node object; if no node object an error message is printed. 
//! MUST CHANGE AS setNodeDisp
void XC::DOF_Group::setNodeVel(const Vector &udot)
  {

    if(!myNode)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; no associated node." << std::endl;
    else
      {
	 Vector &vel = unbalAndTangent.getResidual();
	 vel = myNode->getTrialVel();

	 // get vel for my dof out of vector udot
	 const int sz= getNumDOF();
	 for(int i=0; i<sz; i++)
	   {
	     const int loc= myID(i);
	     if(loc >= 0)
		 vel(i) = udot(loc);
	   }
	 myNode->setTrialVel(vel);
      }
  }



//! @brief Method to set the corresponding nodes accelerations to the
//! values in udotdot, components identified by myID.
//! 
//! This operation sets the value of the nodal trial acceleration at the
//! associated node. The vector {\em \f$u^{..}\f$} is of size equal to the number of
//! equations in the AnalysisModel (this is not checked by the DOF\_Group).
//! The DOF\_Group object accesses the contents of the Vector {\em \f$u^{..}\f$} using
//! its assigned equation numbers. If a \f$-1\f$ exists for a DOF a \f$0\f$ value
//! is set as the corresponding nodal acceleration for the node. The return
//! value is as outlined above for {\em setNodeAccel().} Creates a Vector
//! to store the correct components of \p u, then invokes setTrialAccel()
//! on the node object; if no node object an error message is printed. 
//! MUST CHANGE AS setNodeDisp
void XC::DOF_Group::setNodeAccel(const Vector &udotdot)
  {
    if(!myNode)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; no associated node." << std::endl;
    else
      {
	Vector &accel = unbalAndTangent.getResidual();;
	accel = myNode->getTrialAccel();

	// get disp for the unconstrained dof
	const int sz= getNumDOF();
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
	      accel(i) = udotdot(loc);
	  }

	myNode->setTrialAccel(accel);
      }
  }


//! @brief Method to set the corresponding nodes displacements to the
//" values in u, components identified by myID
void XC::DOF_Group::incrNodeDisp(const Vector &u)
  {
    if(!myNode)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no associated node.\n";
        exit(-1);
      }
    else
      {
	Vector &disp= unbalAndTangent.getResidual();

	if(disp.Size() == 0)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; out of space\n";
	    return;
	  }

	// get disp for my dof out of vector u
	const int sz= getNumDOF();
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
	      disp(i)= u(loc);
	    else
	      disp(i)= 0.0;
	  }
	myNode->incrTrialDisp(disp);
      }
  }


//! @brief Method to set the corresponding nodes velocities to the
//! values in udot, components identified by myID.
//!
//! These methods are similar to those three just outlined, the only
//! difference being that the trial response quantities at the nodes are
//! incremented, not set, with the values now given; this is done by
//! invoking incrTrialDisp()}, {\em incrTrialVel(), and {\em 
//! incrTrialAccel()} respectively on the nodes. MUST ALL CHANGE AS
//! setNodeDisp.
void XC::DOF_Group::incrNodeVel(const Vector &udot)
  {

    if(!myNode)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no associated node.\n";
        exit(-1);
      }
    else
      {
	Vector &vel = unbalAndTangent.getResidual();

	// get vel for my dof out of vector udot
	const int sz= getNumDOF();
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
	      vel(i) = udot(loc);  // -1 for dof labelled 1 through ndof
	    else
	      vel(i) = 0.0;
	  }
	myNode->incrTrialVel(vel);
      }
  }



// void setNodeIncrAccel(const XC::Vector &udotdot);
//        Method to set the corresponding nodes accelerations to the
//        values in udotdot, components identified by myID;

void XC::DOF_Group::incrNodeAccel(const Vector &udotdot)
  {

    if(!myNode)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no associated node.\n";
        exit(-1);
      }
    else
      {
	Vector &accel = unbalAndTangent.getResidual();

	// get disp for the unconstrained dof
	const int sz= getNumDOF();
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
	      accel(i) = udotdot(loc);
	    else accel(i) = 0.0;
	  }
	myNode->incrTrialAccel(accel);
      }
  }

//! @brief Return trial displacement.
const XC::Vector &XC::DOF_Group::getTrialDisp(void) const
  {
    if(myNode)
      return myNode->getTrialDisp();
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << ": no associated Node "
		  << " returning the error Vector\n";
	return errVect;
      }
  }

//! @brief Return trial velocity.
const XC::Vector &XC::DOF_Group::getTrialVel(void) const
  {
    if(myNode)
      return myNode->getTrialVel();
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << ": no associated Node "
		  << " returning the error Vector\n";
	return errVect;
      }
  }

//! @brief Return trial acceleration.
const XC::Vector &XC::DOF_Group::getTrialAccel(void) const
  {
    if(myNode)
      return myNode->getTrialAccel();
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << ": no associated Node "
		  << " returning the error Vector\n";
	return errVect;
      }
  }


void XC::DOF_Group::setEigenvector(int mode, const Vector &theVector)
  {

    if(!myNode)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; no associated node.\n";
        exit(-1);
      }
    else
      {
	Vector &eigenvector = unbalAndTangent.getResidual();

	// get disp for the unconstrained dof
	const int sz= getNumDOF();
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
	      eigenvector(i) = theVector(loc);
	    else
	       eigenvector(i) = 0.0;
	  }
	myNode->setEigenvector(mode, eigenvector);
      }
  }


XC::Matrix *XC::DOF_Group::getT(void)
  { return nullptr; }



void XC::DOF_Group::addLocalM_Force(const Vector &accel, double fact)
  {
    if(myNode)
      {
        if(unbalAndTangent.getResidual().addMatrixVector(1.0, myNode->getMass(), accel, fact) < 0)
          std::cerr << getClassName() << "::" << __FUNCTION__
	            << "; invoking addMatrixVector() on the unbalance failed.\n";
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no node associated, "
		<< " subclass should not call this method \n";
  }


// AddingSensitivity:BEGIN ////////////////////////////////////////
const XC::Vector &XC::DOF_Group::getDispSensitivity(int gradNumber)
  {
    Vector &result = unbalAndTangent.getResidual();
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      { result(i) = myNode->getDispSensitivity(i+1,gradNumber);        }
    return result;
  }

const XC::Vector &XC::DOF_Group::getVelSensitivity(int gradNumber)
  {
    Vector &result = unbalAndTangent.getResidual();
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      { result(i) = myNode->getVelSensitivity(i+1,gradNumber); }
    return result;
  }

const XC::Vector &XC::DOF_Group::getAccSensitivity(int gradNumber)
  {
    Vector &result = unbalAndTangent.getResidual();
    const int sz= getNumDOF();
    for(int i=0; i<sz; i++)
      { result(i) = myNode->getAccSensitivity(i+1,gradNumber); }
    return result;
  }

int XC::DOF_Group::saveSensitivity(Vector *v,Vector *vdot,Vector *vdotdot,int gradNum,int numGrads)
  {
    // Displacement sensitivities
    const int sz= getNumDOF();
    Vector myV(sz);
    for(int i=0; i<sz; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
         { myV(i) = (*v)(loc); }
        else
         { myV(i) = 0.0; }
      }


    // Vel and Acc sensitivities only if they are being delivered
    if( (vdot != 0) && (vdotdot != 0) )
      {
        Vector myVdot(sz);
        for(int i=0; i<sz; i++)
          {
            const int loc= myID(i);
            if(loc >= 0)
              { myVdot(i) = (*vdot)(loc); }
            else
              { myVdot(i) = 0.0; }
          }
        Vector myVdotdot(sz);
        for(int i=0; i<sz; i++)
          {
            const int loc= myID(i);
            if(loc >= 0)
              { myVdotdot(i) = (*vdotdot)(loc); }
            else
              { myVdotdot(i) = 0.0; }
          }
        myNode->saveSensitivity(&myV, &myVdot, &myVdotdot, gradNum, numGrads);
      }
    else
      myNode->saveSensitivity(&myV, nullptr, nullptr, gradNum, numGrads);

    return 0;
  }

void XC::DOF_Group::addM_ForceSensitivity(const XC::Vector &Udotdot, double fact)
  {
    if(!myNode)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no node associated"
		<< " subclass should not call this method \n";
    else
      {
	const int sz= getNumDOF();
	Vector accel(sz);
	// get accel for the unconstrained dof
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
		accel(i) = Udotdot(loc);
	    else accel(i) = 0.0;
	  }

	if(unbalAndTangent.getResidual().addMatrixVector(1.0, myNode->getMassSensitivity(), accel, fact) < 0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; invoking addMatrixVector() on the unbalance failed\n";
      }
  }

void XC::DOF_Group::addD_Force(const XC::Vector &Udot, double fact)
  {
    if(!myNode)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no node associated"
		<< " subclass should not call this method \n";
    else
      {
	const int sz= getNumDOF();
	Vector vel(sz);
	// get velocity for the unconstrained dof
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
	      vel(i) = Udot(loc);
	    else vel(i) = 0.0;
	  }

	if(unbalAndTangent.getResidual().addMatrixVector(1.0, myNode->getDamp(), vel, fact) < 0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; invoking addMatrixVector() on the unbalance failed\n";
      }
  }

void XC::DOF_Group::addD_ForceSensitivity(const Vector &Udot, double fact)
  {
    if(!myNode)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no node associated"
		<< " subclass should not call this method \n";
    else
      {
	const int sz= getNumDOF();
	Vector vel(sz);
	// get velocity for the unconstrained dof
	for(int i=0; i<sz; i++)
	  {
	    const int loc= myID(i);
	    if(loc >= 0)
	      vel(i) = Udot(loc);
	    else
	      vel(i) = 0.0;
	  }

	if(unbalAndTangent.getResidual().addMatrixVector(1.0, myNode->getDampSensitivity(), vel, fact) < 0)
	  std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; invoking addMatrixVector() on the unbalance failed\n";
      }
  }

// AddingSensitivity:END //////////////////////////////////////////
void XC::DOF_Group::resetNodePtr(void)
  { myNode= nullptr; }


