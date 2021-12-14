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

// $Revision: 1.17 $
// $Date: 2005/11/29 22:46:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/fe_ele/FE_Element.cpp,v $


// Written: fmk
// Created: 11/96
// Revision: A
//
// Purpose: This file contains the code for implementing the methods
// of the XC::FE_Element class interface.

#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <cstdlib>

#include "domain/mesh/element/Element.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"
#include <domain/domain/Domain.h>
#include <domain/mesh/node/Node.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <solution/analysis/integrator/Integrator.h>
#include "domain/domain/subdomain/Subdomain.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>

const int MAX_NUM_DOF= 64;

// static variables initialisation
XC::Matrix XC::FE_Element::errMatrix(1,1);
XC::Vector XC::FE_Element::errVector(1);
XC::UnbalAndTangentStorage XC::FE_Element::unbalAndTangentArray(MAX_NUM_DOF+1);
int XC::FE_Element::numFEs(0);           // number of objects


//! @brief set the pointers for the tangent and residual
void XC::FE_Element::set_pointers(void)
  {
    if(myEle->isSubdomain() == false)
      {
        unbalAndTangent= UnbalAndTangent(numDOF,unbalAndTangentArray);
      }
    else
      {

        // as subdomains have own matrix for tangent and residual don't need
        // to set matrix and vector pointers to these objects
        //theResidual= Vector(numDOF);
         // invoke setFE_ElementPtr() method on Subdomain
        Subdomain *theSub= dynamic_cast<Subdomain *>(myEle);
        theSub->setFE_ElementPtr(this);
      }
  }

//! @brief Construictor that take the corresponding model element.
//!
//! Constructs an empty FE\_Element with an associated element given by {\em
//! theElement}.  During construction it determines the number of unknown
//! dofs from the element. Constructs an ID for the mapping between dof's of
//! the element and equation numbers in the system of equation and an ID
//! to hold the tag of the DOF\_Group objects associated with each Node of
//! the element. If the result of invoking {\em
//! theElementPtr-\f$>\f$isSubdomain()} is \p true invokes {\em
//! setFE\_Element(this)} on the Subdomain; if false creates a Matrix 
//! for the tangent and a Vector for the residual to be stored. An error
//! message is printed and the program is terminated if no Domain object
//! is associated with the Element, a Node of the Element does not exist
//! in the Domain, each Node has not yet been associated with a DOF\_Group
//! object, or there is not enough memory for the Vectors and Matrices
//! required.
XC::FE_Element::FE_Element(int tag, Element *ele)
  :TaggedObject(tag),numDOF(ele->getNumDOF()),unbalAndTangent(0,unbalAndTangentArray),
   theModel(nullptr), myEle(ele), theIntegrator(nullptr),
   myDOF_Groups((ele->getNodePtrs().getExternalNodes()).Size()), myID(ele->getNumDOF())
  {
    if(numDOF<=0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "element must have at least 1 dof " << *ele;
        exit(-1);
      }

    // get elements domain & check it is valid
    Domain *theDomain= ele->getDomain();
    if(!theDomain)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; FATAL - element has no domain "<< *ele;
        exit(-1);
      }

    // keep a pointer to all DOF_Groups
    int numGroups= ele->getNumExternalNodes();
    const ID &nodes= ele->getNodePtrs().getExternalNodes();

    for(int i=0; i<numGroups; i++)
      {
        Node *nodePtr =theDomain->getNode(nodes(i));
        if(nodePtr == 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; FATAL - node: "
		      <<  nodes(i) <<  "does not exist in the domain.\n";
            std::cerr << *ele;
            exit(-1);
          }

        DOF_Group *dofGrpPtr= nodePtr->getDOF_GroupPtr();
        if(dofGrpPtr != 0)
          myDOF_Groups(i)= dofGrpPtr->getTag();
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; FATAL node: "
		      <<  *nodePtr
		      <<  " has no DOF_Group associated with it.\n";
            exit(-1);
          }
      }

    set_pointers();
    // increment number of FE_Elements by 1
    numFEs++;
  }

//! Constructor.
//!
//! Provided for subclasses. Constructs an empty FE\_Element with the
//! number of unknown dofs given by \p numDOF and the number of
//! associated DOF\_Group objects given by \p numDOFGroup, two empty IDs
//! are constructed to hold the mapping and the tags of the
//! DOF\_Groups. The subclass must fill in the ID for the tags of the
//! DOF\_Groups in order that setID() will work. No element is
//! associated with this FE\_Element. No space is allocated for the
//! tangent and residual matrix and vector, this is the responsibility of
//! the subclass.
XC::FE_Element::FE_Element(int tag, int numDOF_Group, int ndof)
  :TaggedObject(tag),numDOF(ndof),unbalAndTangent(ndof,unbalAndTangentArray),
    theModel(nullptr), myEle(nullptr), theIntegrator(nullptr),
    myDOF_Groups(numDOF_Group), myID(ndof)
  {
    // this is for a subtype, the subtype must set the myDOF_Groups ID array
    numFEs++;

    // as subtypes have no access to the tangent or residual we don't set them
    // this way we can detect if subclass does not provide all methods it should
  }


//! @brief Destructor.
XC::FE_Element::~FE_Element(void)
  {
    // decrement number of FE_Elements
    numFEs--;
    theModel= nullptr;
    myEle= nullptr;
    theIntegrator= nullptr; 
  }

//! @brief Returns the tags of the associated DOF_Groups.
//! 
//! Returns a const ID containing the unique tag number of the
//! DOF\_Group objects associated with that FE\_Element. For this base class,
//! these are obtained from the DOF\_Groups associated with the Node objects
//! that are associated with the Element object passed in the constructor. This
//! ID is computed only once, during the creation of the object.
const XC::ID &XC::FE_Element::getDOFtags(void) const
  { return myDOF_Groups; }


//! @brief Method to return the ID containing equation numbers.
//! 
//! Returns a const ID containing the equation numbers associated with its
//! matrices and vectors. This ID will contain \f$0\f$'s unless the {\em
//! setID()} method has been called.
const XC::ID &XC::FE_Element::getID(void) const
  { return myID; }

//! @brief Sets the link with the analysis model.
//!
//! To set a link to the AnalysisModel in which the FE\_element
//! resides; this link is needed in setID(). Is invoked by the
//! AnalysisModel when the FE\_element is added to the AnalysisModel.
void XC::FE_Element::setAnalysisModel(AnalysisModel &theAnalysisModel)
  { theModel= &theAnalysisModel; }

//! @brief Method to set the corresponding index of the ID to value.
//! 
//! Causes the FE\_Element to determine the mapping between it's equation
//! numbers and the degrees-of-freedom. The \f$i-1\f$ component of the ID
//! contains the equation number that is associated with \f$i\f$'th
//! degree-of-freedom (a consequence of C indexing for IDs). The method is
//! to be invoked by the DOF\_Numberer after the DOF\_Groups have been assigned
//! their equation numbers. The base class uses the ID containing the tags of
//! the DOF\_Group objects to determine this by looping over the
//! DOF\_Group objects (identified in the DOF\_Group ID and obtained from
//! the AnalysisModel) and getting their mapping by invoking {\em
//! getID()}. Returns \f$0\f$ if successful, a warning message and a
//! negative number is returned if an error occurs:
//! \f$-1\f$ returned if no AnalysisModel link has been set, \f$-2\f$ if a
//! DOF\_Group object does not exist in the AnalysisModel and a \f$-3\f$ if
//! there are more dof's in the DOF\_Groups than dof's identified for the
//! FE\_Element.
int XC::FE_Element::setID(void)
  {
    if(!theModel)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; WARNING - no AnalysisModel set.\n";
        return -1;
      }

    int numGrps= myDOF_Groups.Size();
    int current= 0;
    for(int i=0;i<numGrps;i++)
      {
        int tag= myDOF_Groups(i);
        DOF_Group *dofPtr= theModel->getDOF_GroupPtr(tag);
        if(!dofPtr)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING null DOF_Group pointer.\n";
            return -2;
          }

        const ID &theDOFid= dofPtr->getID();

        for(int j=0; j<theDOFid.Size(); j++)
          if(current < numDOF)
            myID(current++)= theDOFid(j);
          else
            {
              std::cerr << getClassName() << "::" << __FUNCTION__
			<< "; WARNING -  numDOF and"
			<< " number of dof at the DOF_Groups\n";
              return -3;
            }
      }
    return 0;
  }

//! @brief Computes and returns the tangent stiffness matrix.
//!
//! Causes the FE\_Element to determine it's contribution to the tangent
//! matrix and to return this matrix. If the Element is a
//! Subdomain it invokes computeTangent()} and {\em getTang() on the
//! Subdomain. Otherwise {\em formEleTangent(this)} is invoked on {\em
//! theIntegrator} and the new tangent matrix is returned.
//! Subclasses must provide their own implementation. If no Element is
//! passed in the constructor, a warning message is
//! printed and an error Matrix of size 1X1 is returned.
const XC::Matrix &XC::FE_Element::getTangent(Integrator *theNewIntegrator)
  {
    theIntegrator= theNewIntegrator;

    if(myEle == nullptr)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; FATAL - no Element *given "
		  << "- subclasses must provide implementation - "
		  << " - a 1x1 error matrix will be returned.\n";
        exit(-1);
      }

    if(myEle->isSubdomain() == false)
      {
        if(theNewIntegrator)
          theNewIntegrator->formEleTangent(this);
	const Matrix &retval= unbalAndTangent.getTangent();
        return retval;
      }
    else
      {
        Subdomain *theSub= dynamic_cast<Subdomain *>(myEle);
        theSub->computeTang();
        const Matrix &retval= theSub->getTang();
	return retval;
      }
  }

//! @brief Computes and returns the residual vector.
//!
//! Causes the FE\_Element to determine it's contribution to the residual
//! vector and to return this vector. If the Element is a Subdomain it invokes
//! computeResidual()} and {\em getResistingForce() on the Subdomain.
//! Otherwise {\em formEleResidual(this)} is invoked on \p theIntegrator
//! and the resuting residual vector is returned. 
//! Subclasses must provide their own implementation. If no Element is
//! passed in the constructor, a warning message and an error vector is
//! returned.
const XC::Vector &XC::FE_Element::getResidual(Integrator *theNewIntegrator)
  {
    theIntegrator= theNewIntegrator;

    if(!theIntegrator)
      return unbalAndTangent.getResidual();

    if(!myEle)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; FATAL - no element *given "
		  << "- subclasses must provide implementation - "
		  << " - an error Vector of order 1 will be returned.\n";
        exit(-1);
      }
    else
      {
        if(myEle->isSubdomain() == false)
          {
            theNewIntegrator->formEleResidual(this);
            return unbalAndTangent.getResidual();
          }
        else
          {
            Subdomain *theSub= dynamic_cast<Subdomain *>(myEle);
            theSub->computeResidual();
            return theSub->getResistingForce();
          }
      }
  }


//! @brief Zeros the tangent matrix.
//!
//! Zeros the tangent matrix. If the Element is not a Subdomain invokes
//! Zero() on the tangent matrix. Subclasses must provide their own
//! implementation. Nothing is done and a warning message is printed if no
//! Element was passed in the constructor or the Element passed was a
//! Subdomain. 
void XC::FE_Element::zeroTangent(void)
  {
    if(myEle)
      {
        if(myEle->isSubdomain() == false)
          unbalAndTangent.getTangent().Zero();
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
	        << ", subclasses must provide implementation.\n";
  }

//! @brief Adds the product of \p fact times the element's tangent stiffness
//! matrix to the tangent.
//! 
//! Adds the product of \p fact times the element's tangent stiffness
//! matrix to the tangent. If no element is associated with the
//! FE\_Element nothing is added, if the element is not a Subdomain {\em
//! addMatrix(theEle-\f$>\f$getTangentStiff(),fact} is invoked on the tangent
//! matrix. Nothing is done and a warning message is printed if no Element
//! was passed in the constructor or the Element passed was a
//! Subdomain.  
void XC::FE_Element::addKtToTang(double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        else if(myEle->isSubdomain() == false)
          unbalAndTangent.getTangent().addMatrix(1.0, myEle->getTangentStiff(),fact);
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else 
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
	        << ", subclasses must provide implementation.\n";
  }

//! @brief  Adds the product of \p fact times the element's damping
//! matrix to the tangent.
//! 
//! Adds the product of \p fact times the element's damping
//! matrix to the tangent. If no element is associated with the
//! FE\_Element nothing is added, if the element is not a Subdomain 
//! {\em addMatrix(theEle-\f$>\f$getDamp(),fact} is invoked on the tangent
//! matrix. Nothing is done and a warning message is printed if no Element was
//! passed in the constructor or the Element passed was a Subdomain.  
void XC::FE_Element::addCtoTang(double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        else if(myEle->isSubdomain() == false)
          unbalAndTangent.getTangent().addMatrix(1.0, myEle->getDamp(),fact);
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
	        << ", subclasses must provide implementation.\n";
  }

//! @brief Adds the product of \p fact times the element's mass
//! matrix to the tangent.
//! 
//! Adds the product of \p fact times the element's mass
//! matrix to the tangent. If no element is associated with the
//! FE\_Element nothing is added, if the element is not a Subdomain 
//! {\em addMatrix(theEle-\f$>\f$getMass(),fact} is invoked on the tangent
//! matrix. Nothing is done and a warning message is printed if no Element was
//! passed in the constructor or the Element passed was a Subdomain.  
void XC::FE_Element::addMtoTang(double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        else if(myEle->isSubdomain() == false)
          {
            const Matrix &masas= myEle->getMass();
            unbalAndTangent.getTangent().addMatrix(1.0, masas,fact);
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
	        << ", subclasses must provide implementation.\n";
  }


//! Adds the product of \p fact times the element's initial stiffness
//! matrix to the tangent. If no element is associated with the
//! FE\_Element nothing is added, if the element is not a Subdomain {\em
//! addMatrix(theEle-\f$>\f$getInitialStiff(),fact} is invoked on the tangent
//! matrix. Nothing is done and a warning message is printed if no element
//! was passed in the constructor or the element passed was a
//! Subdomain.
void XC::FE_Element::addKiToTang(double fact)
  {
    if(myEle)
      {
	// check for a quick return
	if(fact == 0.0)
	  return;
	else if(myEle->isSubdomain() == false)
	  unbalAndTangent.getTangent().addMatrix(1.0, myEle->getInitialStiff(), fact);
	else
	  {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
	  }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
	        << ", subclasses must provide implementation.\n";
  }


//! @brief Zeros the residual vector.
//!
//! Zeros the residual vector. If the Element is not a Subdomain invokes
//! Zero() on the residual vector. Subclasses must provide their own
//! implementation. Nothing is done and a warning message is printed if no
//! Element was passed in the constructor or the Element passed was a
//! Subdomain. 
void XC::FE_Element::zeroResidual(void)
  {
    if(myEle)
      {
        if(myEle->isSubdomain() == false)
          unbalAndTangent.getResidual().Zero();
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
	        << ", subclasses must provide implementation.\n";
  }

//! Adds to the residual vector the product of the elements residual load
//! vector and \p fact.
//!
//! Adds to the residual vector the product of the elements residual load
//! vector and \p fact. If no element is associated with the
//! FE\_Element nothing is added, if the associated element is not a
//! Subdomain {\em addVector(myEle-\f$>\f$getResistingForce(),fact)} is
//! invoked on the residual. Nothing is done and a warning message is
//! printed if no Element was passed in the constructor or the Element
//! passed was a Subdomain.   
void XC::FE_Element::addRtoResidual(double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
            return;
        else if(myEle->isSubdomain() == false)
          {
            const Vector &eleResisting= myEle->getResistingForce();
            unbalAndTangent.getResidual().addVector(1.0, eleResisting, -fact);
          }
        else 
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
	        << ", subclasses must provide implementation.\n";
  }

//! Adds to the residual vector the product of the elements residual load
//! vector with inertia forces included and \p fact.
//!
//! Adds to the residual vector the product of the elements residual load
//! vector with inertia forces included and \p fact. If no element is
//! associated with the FE\_Element nothing is added, if the associated
//! element is not a Subdomain {\em
//! addVector(myEle-\f$>\f$getResistingForceIncInertia(),fact)} is 
//! invoked on the residual. Nothing is done and a warning message is
//! printed if no Element was passed in the constructor or the Element
//! passed was a Subdomain.   
void XC::FE_Element::addRIncInertiaToResidual(double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        else if(myEle->isSubdomain() == false)
          {
            const Vector &eleResisting= myEle->getResistingForceIncInertia();
            unbalAndTangent.getResidual().addVector(1.0, eleResisting, -fact);
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
	        << ", subclasses must provide implementation.\n";
  }


//! Returns the product of FE\_Elements current tangent matrix
//! and a Vector whose values are obtained by taking the product of {\em
//! fact} and those elements of the Vector \p disp associated with 
//! the FE\_Elements equation numbers. If the element associated with the
//! FE\_Element is a subdomain, the tangent is obtained by invoking {\em
//! getTang()} on the subdomain, otherwise the tangent is formed by
//! invoking {\em formEleTang(this)} on the integrator object last used in
//! a getTangent()} or {\em getResidual().
//! If no element is associated with the
//! FE\_Element a zero vector is returned and an error message is
//! printed. An error message is also printed if invoking
//! addMatrixVector() on the force vector returns \f$< 0\f$.
const XC::Vector &XC::FE_Element::getTangForce(const Vector &disp, double fact)
  {
    if(myEle)
      {

        // zero out the force vector
        unbalAndTangent.getResidual().Zero();

        // check for a quick return
        if(fact == 0.0)
          return unbalAndTangent.getResidual();

        // get the components we need out of the vector
        // and place in a temporary vector
        Vector tmp(numDOF);
        for(int i=0; i<numDOF; i++)
          {
            int dof= myID(i);
            if(dof >= 0)
              tmp(i)= disp(myID(i));
            else
              tmp(i)= 0.0;
          }

        if(myEle->isSubdomain() == false)
          {
            // form the tangent again and then add the force
            theIntegrator->formEleTangent(this);
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, unbalAndTangent.getTangent(),tmp,fact) < 0) 
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
		          << "; WARNING - "
			  << "addMatrixVector returned error.\n";
              }
          }
        else
          {
	    Subdomain *theSub= dynamic_cast<Subdomain *>(myEle);
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, theSub->getTang(),tmp,fact) < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
		          << "; WARNING - "
			  << "addMatrixVector returned error.\n";
              }
          }
        return unbalAndTangent.getResidual();
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; WARNING - no element *given "
                  << ", subclasses must provide implementation.\n";
        return errVector;
      }
  }



const XC::Vector &XC::FE_Element::getK_Force(const Vector &disp, double fact)
  {
    if(myEle != 0)
      {
        // zero out the force vector
        unbalAndTangent.getResidual().Zero();

        // check for a quick return
        if(fact == 0.0)
            return unbalAndTangent.getResidual();

        // get the components we need out of the vector
        // and place in a temporary vector
        Vector tmp(numDOF);
        for(int i=0; i<numDOF; i++)
          {
            int dof= myID(i);
            if(dof >= 0)
              tmp(i)= disp(myID(i));
            else
              tmp(i)= 0.0;
          }

        if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getTangentStiff(), tmp, fact) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
                      << "; WARNING - "
	              << "addMatrixVector returned error.\n";
          }
        return unbalAndTangent.getResidual();
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; WARNING - no element *given "
                  << ", subclasses must provide implementation.\n";
        return errVector;
      }
  }

//! Returns the product of elements current mass matrix
//! and a Vector whose values are obtained by taking the product of {\em
//! fact} and those elements of the Vector \p accel associated with 
//! the FE\_Elements equation numbers. If no element is associated with the
//! FE\_Element or the associated element is a Subdomain a warning message
//! is printed and an error Vector is returned.
const XC::Vector &XC::FE_Element::getM_Force(const Vector &disp, double fact)
  {

    if(myEle)
      {
        // zero out the force vector
        unbalAndTangent.getResidual().Zero();

        // check for a quick return
        if(fact == 0.0)
            return unbalAndTangent.getResidual();

        // get the components we need out of the vector
        // and place in a temporary vector
        Vector tmp(numDOF);
        for(int i=0; i<numDOF; i++)
          {
            int dof= myID(i);
            if(dof >= 0)
              tmp(i)= disp(myID(i));
            else
              tmp(i)= 0.0;
          }
        if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getMass(), tmp, fact) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
                      << "; WARNING - "
	              << "addMatrixVector returned error.\n";
          }
        return unbalAndTangent.getResidual();
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; WARNING - no element *given "
                  << ", subclasses must provide implementation.\n";
        return errVector;
      }
  }

const XC::Vector &XC::FE_Element::getC_Force(const XC::Vector &disp, double fact)
  {
    if(myEle)
      {
        // zero out the force vector
        unbalAndTangent.getResidual().Zero();

        // check for a quick return
        if(fact == 0.0)
          return unbalAndTangent.getResidual();

        // get the components we need out of the vector
        // and place in a temporary vector
        Vector tmp(numDOF);
        for(int i=0; i<numDOF; i++)
          {
            const int dof= myID(i);
            if(dof >= 0)
              tmp(i)= disp(myID(i));
            else
              tmp(i)= 0.0;
          }
        if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getDamp(), tmp, fact) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
                      << "; WARNING - "
	              << "addMatrixVector returned error.\n";
          }
        return unbalAndTangent.getResidual();
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; WARNING - no element *given "
                  << ", subclasses must provide implementation.\n";
        return errVector;
      }
  }


//! @brief Method which returns the last integrator supplied in a 
//! formTangent() or a formResidual() invocation.
XC::Integrator *XC::FE_Element::getLastIntegrator(void)
  { return theIntegrator; }

//! @brief A method which invokes getLastResponse() on the Integrator
//! object that was last passed as an argument to any of the routines.
//! 
//! A method which invokes getLastResponse() on the Integrator
//! object that was last passed as an argument to any of the routines.
//! The FE\_Elements ID and the force Vector object is passed as arguments.
//! Returns the force Vector object if successful. If no element is
//! associated with the FE\_Element or no integrator has yet to be passed,
//! a warning message is printed and an error Vector is returned.
const XC::Vector &XC::FE_Element::getLastResponse(void)
  {
    if(myEle)
      {
        if(theIntegrator)
          {
            if(theIntegrator->getLastResponse(unbalAndTangent.getResidual(),myID) < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
			  << " - the integrator had problems with"
		          << " getLastResponse().\n";
              }
          }
        else
          {
            unbalAndTangent.getResidual().Zero();
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - no integrator yet passed\n";
          }
        Vector &result= unbalAndTangent.getResidual();
        return result;
      }
    else
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING - no element passed in constructor\n";
        return errVector;
      }
  }

//! Adds to the residual the product of elements current mass matrix
//! and a Vector whose values are obtained by taking the product of {\em
//! fact} and those elements of the Vector \p accel associated with 
//! the FE\_Elements equation numbers. If no element is associated with the
//! FE\_Element or the Element is a Subdomain nothing is added and an
//! warning message is printed. An error message is also printed if invoking
//! addMatrixVector() on the residual vector returns \f$< 0\f$.
void XC::FE_Element::addM_Force(const XC::Vector &accel, double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            // get the components we need out of the vector
            // and place in a temporary vector
            Vector tmp(numDOF);
            for(int i=0; i<numDOF; i++)
              {
                const int loc= myID(i);
                if(loc >= 0)
                  tmp(i)= accel(loc);
                else
                  tmp(i)= 0.0;
              }
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getMass(), tmp, fact) < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
                          << "; WARNING - "
	                  << "addMatrixVector returned error.\n";
              }
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
                << ", subclasses must provide implementation.\n";
  }

//! Adds to the residual the product of elements current damping matrix
//! and a Vector whose values are obtained by taking the product of {\em
//! fact} and those elements of the Vector \p vel associated with 
//! the FE\_Elements equation numbers. If no element is associated with the
//! FE\_Element or the Element is a Subdomain nothing is added and an
//! warning message is printed. An error message is also printed if invoking
//! addMatrixVector() on the residual vector returns \f$< 0\f$.
void XC::FE_Element::addD_Force(const XC::Vector &accel, double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            // get the components we need out of the vector
            // and place in a temporary vector
            Vector tmp(numDOF);
            for(int i=0; i<numDOF; i++)
              {
                const int loc= myID(i);
                if(loc >= 0)
                  tmp(i)= accel(loc);
                else
                  tmp(i)= 0.0;
              }
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getDamp(), tmp, fact) < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
                          << "; WARNING - "
	                  << "addMatrixVector returned error.\n";
              }
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
                << ", subclasses must provide implementation.\n";
  }

void XC::FE_Element::addLocalM_Force(const XC::Vector &accel, double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getMass(),accel, fact) < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
                          << "; WARNING - "
	                  << "addMatrixVector returned error.\n";
              }
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
                << ", subclasses must provide implementation.\n";
  }

void XC::FE_Element::addLocalD_Force(const XC::Vector &accel, double fact)
  {
    if(myEle != 0)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getDamp(),accel, fact) < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
                          << "; WARNING - "
	                  << "addMatrixVector returned error.\n";
              }
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
                << ", subclasses must provide implementation.\n";
  }

//! @brief Returns a pointer to the associated element.
XC::Element *XC::FE_Element::getElement(void)
  { return myEle; }


// AddingSensitivity:BEGIN /////////////////////////////////
void XC::FE_Element::addResistingForceSensitivity(int gradNumber, double fact)
  { unbalAndTangent.getResidual().addVector(1.0, myEle->getResistingForceSensitivity(gradNumber), -fact); }

void XC::FE_Element::addM_ForceSensitivity(int gradNumber, const XC::Vector &vect, double fact)
  {
    // Get the components we need out of the vector
    // and place in a temporary vector
    Vector tmp(numDOF);
    for(int i=0; i<numDOF; i++)
      {
        const int loc= myID(i);
        if(loc >= 0)
          { tmp(i)= vect(loc); }
        else
          { tmp(i)= 0.0; }
      }
    if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getMassSensitivity(gradNumber),tmp,fact) < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
                  << "; WARNING - "
                  << "addMatrixVector returned error.\n";
      }
  }

void XC::FE_Element::addD_ForceSensitivity(int gradNumber, const XC::Vector &vect, double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            // get the components we need out of the vector
            // and place in a temporary vector
            Vector tmp(numDOF);
            for(int i=0; i<numDOF; i++)
              {
                int loc= myID(i);
                if(loc >= 0)
                  tmp(i)= vect(loc);
                else
                  tmp(i)= 0.0;
              }
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getDampSensitivity(gradNumber), tmp, fact) < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
                          << "; WARNING - "
                          << "addMatrixVector returned error.\n";
              }
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
                << ", subclasses must provide implementation.\n";
  }

void XC::FE_Element::addLocalD_ForceSensitivity(int gradNumber, const XC::Vector &accel, double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getDampSensitivity(gradNumber),accel, fact) < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
                          << "; WARNING - "
                          << "addMatrixVector returned error.\n";
              }
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
                << ", subclasses must provide implementation.\n";
  }

void XC::FE_Element::addLocalM_ForceSensitivity(int gradNumber, const XC::Vector &accel, double fact)
  {
    if(myEle)
      {
        // check for a quick return
        if(fact == 0.0)
          return;
        if(myEle->isSubdomain() == false)
          {
            if(unbalAndTangent.getResidual().addMatrixVector(1.0, myEle->getMassSensitivity(gradNumber),accel, fact) < 0)
              {
                std::cerr << getClassName() << "::" << __FUNCTION__
                          << "; WARNING - "
                          << "addMatrixVector returned error.\n";
              }
          }
        else
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING - "
		      << "this should not be called on a subdomain!\n";
          }
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; WARNING - no element *given "
                << ", subclasses must provide implementation.\n";
  }

int XC::FE_Element::commitSensitivity(int gradNum, int numGrads)
  {
    myEle->commitSensitivity(gradNum, numGrads);
    return 0;
  }

// AddingSensitivity:END ////////////////////////////////////


int XC::FE_Element::updateElement(void)
  {
    if(myEle)
      return myEle->update();
    else
      return 0;
  }
