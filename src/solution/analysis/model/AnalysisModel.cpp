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
** file 'COPYRIGHT' in  main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.12 $
// $Date: 2005/11/28 21:23:50 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/model/AnalysisModel.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/model/AnalysisModel.C
//
// Written: fmk 
// Created: Fri Sep 20 15:27:47: 1996
// Revision: A
//
// Purpose: This file contains the class definition for XC::AnalysisModel
// AnalysisModel is a container class. The class is responsible for holding
// and providing access to the Elements, Nodes, LoadCases, SFreedom_Constraints 
// and MFreedom_Constraints. These objects are all added to the XC::AnalysisModel by a 
// ModelBuilder.
//
// What: "@(#) AnalysisModel.C, revA"

#include <cstdlib>

#include "AnalysisModel.h"
#include "solution/analysis/ModelWrapper.h"
#include <utility/tagged/storage/ArrayOfTaggedObjects.h>
#include <domain/domain/Domain.h>
#include <solution/analysis/model/fe_ele/FE_Element.h>
#include <solution/analysis/model/fe_ele/lagrange/LagrangeSFreedom_FE.h>
#include <solution/analysis/model/fe_ele/lagrange/LagrangeMFreedom_FE.h>
#include <solution/analysis/model/fe_ele/lagrange/LagrangeMRMFreedom_FE.h>
#include <solution/analysis/model/fe_ele/penalty/PenaltySFreedom_FE.h>
#include <solution/analysis/model/fe_ele/penalty/PenaltyMFreedom_FE.h>
#include <solution/analysis/model/fe_ele/penalty/PenaltyMRMFreedom_FE.h>
#include <solution/analysis/model/fe_ele/transformation/TransformationFE.h>
#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include "solution/analysis/model/dof_grp/LagrangeDOF_Group.h"
#include "solution/analysis/model/dof_grp/TransformationDOF_Group.h"
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/domain/subdomain/Subdomain.h"
#include "domain/mesh/node/NodeIter.h"
#include "solution/analysis/handler/ConstraintHandler.h"
#include "solution/analysis/handler/TransformationConstraintHandler.h"

//! @brief Constructor.
//! 
//! Constructs an empty AnalysisModel. The constructor allocates
//! space for two arrays of 256 pointers to FE\_Elements and DOF\_Groups.
//! If not enough memory is available for these arrays, an error message
//! is printed and the program is terminated. Note these arrays grow
//! automatically if the problem needs it.
XC::AnalysisModel::AnalysisModel(ModelWrapper *owr)
  :MovableObject(AnaMODEL_TAGS_AnalysisModel), CommandEntity(owr),
   numFE_Ele(0), numDOF_Grp(0), numEqn(0),
   theFEs(this,256,"FEs"), theDOFGroups(this,256,"DOFs"), theFEiter(&theFEs), theDOFGroupiter(&theDOFGroups),
   theFEconst_iter(&theFEs), theDOFGroupconst_iter(&theDOFGroups),
   myDOFGraph(*this), myGroupGraph(*this), updateGraphs(false) {}

//! @brief Constructor.
//!
//! Provided for subclasses to be used. The storage of the FE\_Elements
//! and DOF\_Groups and iters to access them must be provided by the
//! subclass.
XC::AnalysisModel::AnalysisModel(int theClassTag,CommandEntity *owr)
  :MovableObject(theClassTag), CommandEntity(owr),
   numFE_Ele(0), numDOF_Grp(0), numEqn(0),
   theFEs(this,1024,"FEs"), theDOFGroups(this,1024,"DOFs"),theFEiter(&theFEs), theDOFGroupiter(&theDOFGroups),
   theFEconst_iter(&theFEs), theDOFGroupconst_iter(&theDOFGroups),
   myDOFGraph(*this), myGroupGraph(*this), updateGraphs(false) {}

//! @brief Copy constructor.
XC::AnalysisModel::AnalysisModel(const AnalysisModel &other)
  : MovableObject(other), CommandEntity(other),
   numFE_Ele(other.numFE_Ele), numDOF_Grp(other.numDOF_Grp), numEqn(other.numEqn),
   theFEs(other.theFEs), theDOFGroups(other.theDOFGroups),theFEiter(&theFEs), theDOFGroupiter(&theDOFGroups),
   theFEconst_iter(&theFEs), theDOFGroupconst_iter(&theDOFGroups),
   myDOFGraph(*this), myGroupGraph(*this), updateGraphs(false) {}

//! @brief Assignment operator.
XC::AnalysisModel &XC::AnalysisModel::operator=(const AnalysisModel &other)
  { 
    MovableObject::operator=(other);
    CommandEntity::operator=(other);
    numFE_Ele= other.numFE_Ele;
    numDOF_Grp= other.numDOF_Grp;
    numEqn= other.numEqn;
    theFEs= other.theFEs;
    theDOFGroups= other.theDOFGroups;
    myDOFGraph= DOF_Graph(*this);
    myGroupGraph= DOF_GroupGraph(*this);
    updateGraphs= false; //Update just finished
    return *this;
  }

//! @brief Virtual constructor.
XC::AnalysisModel *XC::AnalysisModel::getCopy(void) const
  { return new AnalysisModel(*this); }

//! @brief Destructor.
//!
//! Is responsible for returning to memory the arrays used for storing
//! pointers to the FE\_Element and DOF\_Groups which have been added to
//! the AnalysisModel. It is not responsible for deleting the individual
//! DOF\_Group and FE\_Element objects, that is the responsibility of the
//! ConstraintHandler. If the Graphs have been requested their destructor
//! is invoked.
XC::AnalysisModel::~AnalysisModel(void)
  { clearAll(); }    

//! @brief Adds the FE\_Element pointed to by \p theElement to the model.
//!
//! Adds the FE\_Element pointed to by \p theElement to the domain and
//! invokes {\em setAnalysisModel(*this)} on the FE\_Element. If the
//! array for the FE\_Elements is large enough, it adds this pointer to
//! the array and increments the number of FE\_Elements in the array. If
//! the array is not large enough, a new one double in size is
//! constructed, all the old pointers are copied to this new array and the
//! new pointer is then added. If not enough room is available for this
//! array, an error message is printed and the program is
//! terminated. Returns \p true, otherwise \p false if {\em
//! theElement} is \f$0\f$ or derived class used which does not implement the
//! method. 
bool XC::AnalysisModel::addFE_Element(FE_Element *theElement)
  {
    // check we don't add a null pointer or this is a subclass
    // trying to use this method when it shouldn't
    bool retval= false;
    if(theElement)
      {
	// check if an Element with a similar tag already exists in the Domain
	int tag= theElement->getTag();
	TaggedObject *other= theFEs.getComponentPtr(tag);
	if(other)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; element with tag " << tag
		      << " already exists in model\n"; 
	    retval= false;
	  }
        else
	  {
	    // add the element to the container object for the elements
	    retval= theFEs.addComponent(theElement);
	    if(retval) // o.k.
	      {
		theElement->setAnalysisModel(*this);
		numFE_Ele++;
		updateGraphs= true;
	      }
	  }
      }
    return retval;
  }

//! @brief Creates a FE_Element and appends it to the model.
XC::FE_Element *XC::AnalysisModel::createFE_Element(const int &tag, Element *elePtr)
  {
    FE_Element *retval= nullptr;
    // only create an FE_Element for a subdomain element if it does not
    // do independent analysis .. then subdomain part of this analysis so create
    // an FE_element & set subdomain to point to it.
    if(elePtr->isSubdomain() == true)
      {
        Subdomain *theSub= dynamic_cast<Subdomain *>(elePtr);
        if(theSub->doesIndependentAnalysis() == false)
          {
            retval=new FE_Element(tag, elePtr);
            if(retval) theSub->setFE_ElementPtr(retval);
          }
      }
    else
      retval= new FE_Element(tag, elePtr);

    if(retval)
      addFE_Element(retval);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating FE_Element: " << tag << std::endl;
    return retval;
  }

//! @brief Create an LagrangeSFreedom_FE object and append it to the model.
XC::LagrangeSFreedom_FE *XC::AnalysisModel::createLagrangeSFreedom_FE(const int &tag, SFreedom_Constraint &theSP, DOF_Group &theDofGrp,const double &alpha)
  {
    LagrangeSFreedom_FE *retval= new LagrangeSFreedom_FE(tag,*getDomainPtr(),theSP,theDofGrp,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating LagrangeFE_Element: " << tag << std::endl;
    return retval;    
  }

//! @brief Method to create a LagrangeMFreedom_FE object and append it
//! to the model.
XC::LagrangeMFreedom_FE *XC::AnalysisModel::createLagrangeMFreedom_FE(const int &tag,MFreedom_Constraint &theMP,DOF_Group &theDofGrp,const double &alpha)
  {
    LagrangeMFreedom_FE *retval= new LagrangeMFreedom_FE(tag,*getDomainPtr(),theMP,theDofGrp,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating LagrangeFE_Element: " << tag << std::endl;
    return retval;    
  }

//! @brief Method to create a LagrangeMRMFreedom_FE object and append it
//! to the model.
XC::LagrangeMRMFreedom_FE *XC::AnalysisModel::createLagrangeMRMFreedom_FE(const int &tag,MRMFreedom_Constraint &theMRMP,DOF_Group &theDofGrp,const double &alpha)
  {
    LagrangeMRMFreedom_FE *retval= new LagrangeMRMFreedom_FE(tag,*getDomainPtr(),theMRMP,theDofGrp,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating LagrangeFE_Element: " << tag << std::endl;
    return retval;
  }

//! @brief Method to create a PenaltySFreedom_FE object and append it
//! to the model.
XC::PenaltySFreedom_FE *XC::AnalysisModel::createPenaltySFreedom_FE(const int &tag, SFreedom_Constraint &theSP, const double &alpha)
  {
    PenaltySFreedom_FE *retval=new PenaltySFreedom_FE(tag,*getDomainPtr(),theSP,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating PenaltyFE_Element: " << tag << std::endl;
    return retval;    
  }

//! @brief Create a PenaltyMFreedom_FE object and append it to the model.
XC::PenaltyMFreedom_FE *XC::AnalysisModel::createPenaltyMFreedom_FE(const int &tag, MFreedom_Constraint &theMP, const double &alpha)
  {
    PenaltyMFreedom_FE *retval=new PenaltyMFreedom_FE(tag,*getDomainPtr(),theMP,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating PenaltyFE_Element: " << tag << std::endl;
    return retval;    
  }

//! @brief Create a PenaltyMRMFreedom_FE object and append it to the model.
XC::PenaltyMRMFreedom_FE *XC::AnalysisModel::createPenaltyMRMFreedom_FE(const int &tag, MRMFreedom_Constraint &theMRMP, const double &alpha)
  {
    PenaltyMRMFreedom_FE *retval=new PenaltyMRMFreedom_FE(tag,*getDomainPtr(),theMRMP,alpha);
    if(retval)
      addFE_Element(retval);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating PenaltyFE_Element: " << tag << std::endl;
    return retval;    
  }

//! @brief Create a TransformationFE_Element object and append it to the model.
XC::FE_Element *XC::AnalysisModel::createTransformationFE(const int &tag, Element *elePtr, const std::set<int> &transformedEle, std::set<FE_Element *> &theFEset)
  {
    FE_Element *retval= nullptr;
    const bool transformado= transformedEle.find(tag) != transformedEle.end();
    // only create an FE_Element for a subdomain element if it does not
    // do independent analysis .. then subdomain part of this analysis so create
    // an FE_element & set subdomain to point to it.
    if(elePtr->isSubdomain() == true)
      {
        Subdomain *theSub= dynamic_cast<Subdomain *>(elePtr);
        if(theSub->doesIndependentAnalysis() == false)
          {
            if(!transformado)
              { retval= new FE_Element(tag, elePtr); }
            else
              {
                retval= new TransformationFE(tag, elePtr);
                theFEset.insert(retval);
              }
            if(retval) theSub->setFE_ElementPtr(retval);
          }
      }
    else
      if(!transformado)
        { retval=new FE_Element(tag, elePtr); }
      else
        {
          retval=new TransformationFE(tag, elePtr);
          theFEset.insert(retval); 
        }

    if(retval)
      addFE_Element(retval);
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; WARNING - ran out of memory"
                << " creating FE_Element: " << tag << std::endl;
    return retval;
  }

//! @brief Method to add an DOF to the model.
//! 
//! Adds the DOF\_Group pointed to by \p theGroup to the domain. If the
//! array for the DOF\_Groups is large enough, it adds this pointer to
//! the array and increments the number of DOF\_Groups in the array. If
//! the array is not large enough, a new one double in size is
//! constructed, all the old pointers are copied to this new array and the
//! new pointer is then added. If not enough room is available for this
//! array, an error message is printed and the program is
//! terminated. Returns \p true, otherwise \p false if {\em
//! theGroup} is \f$0\f$ or derived class used which does not implement the
//! method. 
bool XC::AnalysisModel::addDOF_Group(DOF_Group *theGroup)
  {

    // check we don't add a null pointer or this is a subclass trying
    // to use a method it shouldn't be using
    if(theGroup == 0)
      return false;
  
    // check if an Node with a similar tag already exists in the Domain
    int tag= theGroup->getTag();
    TaggedObject *other= theDOFGroups.getComponentPtr(tag);
    if(other)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; group with tag " << tag
		  << " already exists in model\n"; 
        return false;
      }

    // add the element to the container object for the elements
    bool result= theDOFGroups.addComponent(theGroup);
    if(result == true)
      {
        numDOF_Grp++;
        updateGraphs= true;
        return true;  // o.k.
      }
    else
      return false;
  }

// @brief Method to create an add a DOF_Group to the model.
XC::DOF_Group *XC::AnalysisModel::createDOF_Group(const int &tag, Node *myNode)
  {
    DOF_Group *dofPtr=new DOF_Group(tag,myNode);
    if(dofPtr)
      {
        myNode->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating DOF_Group " << tag << std::endl;

    return dofPtr;
  }

//! @brief Appends to the model the Lagrange DOFs for the single freedom 
//! constraint being passed as parameter.
//! @param tag: identifier for the new Lagrange DOFs group.
//! @param spPtr: single freedom constraint to introduce.
XC::LagrangeDOF_Group *XC::AnalysisModel::createLagrangeDOF_Group(const int &tag, SFreedom_Constraint *spPtr)
  {
    assert(spPtr);
    LagrangeDOF_Group *dofPtr=new LagrangeDOF_Group(tag,*spPtr);
    if(dofPtr)
      {
        //spPtr->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }

//! @brief Appends to the model the Lagrange DOFs for the multi-freedom 
//! constraint being passed as parameter.
//! @param tag: identifier for the new Lagrange DOFs group.
//! @param mpPtr: multi-freedom constraint to introduce.
XC::LagrangeDOF_Group *XC::AnalysisModel::createLagrangeDOF_Group(const int &tag, MFreedom_Constraint *mpPtr)
  {
    assert(mpPtr);
    LagrangeDOF_Group *dofPtr=new LagrangeDOF_Group(tag,*mpPtr);
    if(dofPtr)
      {
        //mpPtr->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }


//! @brief Appends to the model the Lagrange DOFs for the multi-freedom 
//! constraint being passed as parameter.
//! @param tag: identifier for the new Lagrange DOFs group.
//! @param mrmpPtr: multi-row, multi-freedom constraint to introduce.
XC::LagrangeDOF_Group *XC::AnalysisModel::createLagrangeDOF_Group(const int &tag, MRMFreedom_Constraint *mrmpPtr)
  {
    assert(mrmpPtr);
    LagrangeDOF_Group *dofPtr=new LagrangeDOF_Group(tag,*mrmpPtr);
    if(dofPtr)
      {
        //mpPtr->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }

XC::TransformationDOF_Group *XC::AnalysisModel::createTransformationDOF_Group(const int &tag, Node *myNode, MFreedom_ConstraintBase *mp, TransformationConstraintHandler *handler)
  {
    assert(mp);
    assert(handler);
    TransformationDOF_Group *dofPtr=new TransformationDOF_Group(tag,myNode,mp,handler);
    if(dofPtr)
      {
        myNode->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }

XC::TransformationDOF_Group *XC::AnalysisModel::createTransformationDOF_Group(const int &tag, Node *myNode, TransformationConstraintHandler *handler)
  {
    assert(handler);
    TransformationDOF_Group *dofPtr=new TransformationDOF_Group(tag,myNode,handler);
    if(dofPtr)
      {
        myNode->setDOF_GroupPtr(dofPtr);
        addDOF_Group(dofPtr);
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; ran out of memory"
                << " creating DOF_Group " << tag << std::endl;
    return dofPtr;
  }

//! Clears from the model all FE\_Element and DOF\_Group objects.
//! 
//! Clears from the model all FE\_Element and DOF\_Group objects that have
//! been added to the analysis model using the above two methods. It does
//! this by setting the components in the two arrays of pointers equal to
//! \f$0\f$ and setting the number of components to \f$0\f$. If the Graphs have
//! been created their destructor is invoked. Also sets \p numEqn to \f$0\f$.
void XC::AnalysisModel::clearAll(void) 
  {
    //XXX LA LÍNEA SIGUIENTE FALLA CUANDO EL MODELO NO CONTIENE ELEMENTOS.
    theFEs.clearAll();
    theDOFGroups.clearAll();

    numFE_Ele=0;
    numDOF_Grp= 0;
    numEqn= 0;    
    updateGraphs= true;
  }



//! @brief Returns the umber of DOF_Group objects added to the model.
int XC::AnalysisModel::getNumDOF_Groups(void) const
  { return numDOF_Grp; }


//! @brief Returns a pointer to the DEF group with the tag
//! beign passed as parameter.
//!
//! Returns a pointer to the DOF\_Group object whose tag is given by {\em
//! tag}.  It first checks to see if the DOF\_Group object is at the
//! location in the array given by \p tag; if not it searches through
//! the array to find the DOF\_Group object. Returns a pointer to the
//! object if found, otherwise \f$0\f$ is returned.
XC::DOF_Group *XC::AnalysisModel::getDOF_GroupPtr(int tag)
  {
    DOF_Group *result= nullptr;
    TaggedObject *other= theDOFGroups.getComponentPtr(tag);
    if(other)
      result= dynamic_cast<DOF_Group *>(other);
    updateGraphs= true;
    return result;
  }

//! @brief Returns a pointer to the DOF group which the tag
//! being passed as parameter.
const XC::DOF_Group *XC::AnalysisModel::getDOF_GroupPtr(int tag) const
  {
    const DOF_Group *result= nullptr;
    const TaggedObject *other= theDOFGroups.getComponentPtr(tag);
    if(other)
      result= dynamic_cast<const DOF_Group *>(other);
    return result;
  }

//! @brief Returns an {\em FE\_EleIter} for the FE\_Elements of the model.
XC::FE_EleIter &XC::AnalysisModel::getFEs()
  {
    theFEiter.reset();
    updateGraphs= true;
    return theFEiter;
  }

//! @brief Returns a {\em FE\_EleConstIter} for the FE\_Elements of the model.
XC::FE_EleConstIter &XC::AnalysisModel::getConstFEs() const
  {
    theFEconst_iter.reset();
    return theFEconst_iter;
  }

XC::DOF_GrpIter &XC::AnalysisModel::getDOFGroups()
  {
    theDOFGroupiter.reset();
    updateGraphs= true;
    return theDOFGroupiter;
  }

XC::DOF_GrpConstIter &XC::AnalysisModel::getConstDOFs() const
  {
    theDOFGroupconst_iter.reset();
    return theDOFGroupconst_iter;
  }

//! @brief Sets the value of the number of equations in the model.
//! Invoked by the DOF\_Numberer when it is numbering the dofs.
void XC::AnalysisModel::setNumEqn(int theNumEqn)
  { numEqn= theNumEqn; }

//! @brief Returns the number of DOFs in the model which have been assigned
//! an equation number.
//! 
//! Returns the number of DOFs in the model which have been assigned
//! an equation number. Returns the value passed in setNumEqn(),
//! if setNumEqn() was not invoked \f$0\f$ is returned.
int XC::AnalysisModel::getNumEqn(void) const
  { return numEqn; }


XC::Graph &XC::AnalysisModel::getDOFGraph(void)
  {
    if(updateGraphs)
      {
        myDOFGraph= DOF_Graph(*this);
        updateGraphs= false;
      }
    return myDOFGraph;
  }

//! @brief Returns the connectivity of the DOF\_Group objects in the model.
//! 
//! Returns the connectivity of the DOF\_Group objects in the model. 
//! This graph is used by the DOF\_Numberer to assign equation numbers to
//! the dofs. If no graph has yet been constructed it creates
//! a new DOF\_GroupGraph object using itself as the argument, otherwise it
//! returns a pointer to this graph. AGAIN WILL CHANGE.
XC::Graph &XC::AnalysisModel::getDOFGroupGraph(void)
  {
    if(updateGraphs)
      {
        myGroupGraph= DOF_GroupGraph(*this);
        updateGraphs= false;
      }
    return myGroupGraph;
  }

//! Returns the DOF connectivity graph for the individual dofs in the
//! model. This graph is used by the system of equation object to
//! determine its size. If no graph has yet been constructed it creates
//! a new DOF\_Graph object using itself as the argument, otherwise it
//! returns a pointer to this graph. THIS WILL CHANGE WHEN I REMOVE
//! DOF\_Graph CLASS - will go through and construct the Graph.
const XC::Graph &XC::AnalysisModel::getDOFGraph(void) const
  {
    if(updateGraphs)
      {
        myDOFGraph= DOF_Graph(*this);    
        updateGraphs= false;
      }
    return myDOFGraph;
  }

const XC::Graph &XC::AnalysisModel::getDOFGroupGraph(void) const
  {
    if(updateGraphs)
      {
        myGroupGraph= DOF_GroupGraph(*this);
        updateGraphs= false;
      }
    return myGroupGraph;
  }

//! @brief Sets the values of the displacement, velocity and acceleration of
//! the nodes.
//! 
//! The model is responsible for invoking {\em setNodeDisp(disp)}, {\em
//! setNodeVel(vel)} and {\em setNodeAccel(accel)} on each DOF\_Group in the
//! model. It does this by iterating over the DOF\_Group objects using the
//! iter.
void XC::AnalysisModel::setResponse(const Vector &disp, const Vector &vel, const Vector &accel)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while((dofPtr= theDOFGrps()) != 0)
      {
        dofPtr->setNodeDisp(disp);
        dofPtr->setNodeVel(vel);
        dofPtr->setNodeAccel(accel);        
      }
  }
        
//! @brief Sets the values of the displacement of the nodes.
//!
//! The model is responsible for invoking {\em setDisp(disp)} on each
//! DOF\_Group in the model. It does this by getting an iter to the
//! DOF\_Group objects and iterating through them invoking {\em
//! setNodeDisp(disp)} on each DOF\_Group.
void XC::AnalysisModel::setDisp(const Vector &disp)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
        dofPtr->setNodeDisp(disp);
  }        
        
//! @brief Sets the values of the velocity of the nodes.
//!
//! The model is responsible for invoking {\em setVel(vel)} on each
//! DOF\_Group in the model. It does this by getting an iter to the
//! DOF\_Group objects and iterating through them invoking {\em
//! setNodeVel(vel)} on each DOF\_Group.
void XC::AnalysisModel::setVel(const Vector &vel)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
        dofPtr->setNodeVel(vel);
  }        
        
//! @brief Sets the values of the acceleration of the nodes.
//!
//! The model is responsible for invoking {\em setAccel(accel)} on each
//! DOF\_Group in the model. It does this by getting an iter to the
//! DOF\_Group objects and iterating through them invoking {\em
//! setNodeAccel(accel)} on each DOF\_Group.
void XC::AnalysisModel::setAccel(const Vector &accel)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
        dofPtr->setNodeAccel(accel);        
  }

//! @brief Sets the values of the displacement increment of the nodes.
//!
//! The model is responsible for invoking {\em incrNodeDisp(disp)} on each
//! DOF\_Group in the model. It does this by getting an iter to the
//! DOF\_Group objects and iterating through them invoking {\em
//! incrNodeDisp(disp)} on each DOF\_Group.
void XC::AnalysisModel::incrDisp(const Vector &disp)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
        dofPtr->incrNodeDisp(disp);
  }
        
//! @brief Sets the values of the velocity increment of the nodes.
//!
//! The model is responsible for invoking {\em incrNodeVel(vel)} on each
//! DOF\_Group in the model. It does this by getting an iter to the
//! DOF\_Group objects and iterating through them invoking {\em
//! incrNodeVel(vel)} on each DOF\_Group.
void XC::AnalysisModel::incrVel(const Vector &vel)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
        dofPtr->incrNodeVel(vel);
  }        
        
//! @brief Sets the values of the acceleration increment of the nodes.
//!
//! The model is responsible for invoking {\em incrNodeAccel(accel)} on each
//! DOF\_Group in the model. It does this by getting an iter to the
//! DOF\_Group objects and iterating through them invoking {\em
//! incrNodeAccel(accel)} on each DOF\_Group.
void XC::AnalysisModel::incrAccel(const Vector &accel)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr= nullptr;  
    while((dofPtr= theDOFGrps()) != 0) 
        dofPtr->incrNodeAccel(accel);        
  }        


void XC::AnalysisModel::setNumEigenvectors(int numEigenvectors)
  {
    Node *theNode= nullptr;
    NodeIter &theNodes= getDomainPtr()->getNodes();
    while((theNode= theNodes()) != 0)
        theNode->setNumEigenvectors(numEigenvectors);
  }

//! @brief Informs the domain of the obtained eigenvalues.
void XC::AnalysisModel::setEigenvalues(const Vector &eigenvalues)
  { getDomainPtr()->setEigenvalues(eigenvalues); }        

//! @brief Informs the domain of the obtained modal participation factors.
void XC::AnalysisModel::setModalParticipationFactors(const Vector &values)
  { getDomainPtr()->setModalParticipationFactors(values); }        

void XC::AnalysisModel::setEigenvector(int mode, const Vector &eigenvalue)
  {
    DOF_GrpIter &theDOFGrps= this->getDOFGroups();
    DOF_Group *dofPtr=nullptr;
    while ((dofPtr= theDOFGrps()) != 0) 
      dofPtr->setEigenvector(mode, eigenvalue);        
  }      

//! @brief Method which invokes {\em applyLoad(timeStep, loadFactor)} on the
//! domain.
//! 
//! Method which invokes {\em applyLoad(timeStep, loadFactor)} on the
//! domain. This method causes the domain ask the loads in the currently
//! set to apply themselves. If no Domain has been set nothing is done and an
//! error message is printed. 
void XC::AnalysisModel::applyLoadDomain(double pseudoTime)
  {
    Domain *dom= getDomainPtr();
    if(dom)
      {
        dom->applyLoad(pseudoTime);
        getHandlerPtr()->applyLoad();
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; WARNING no domain linked.\n";
  }

//! @brief Method which invokes update() on the domain. If no Domain has
//! been set nothing is done and an error message is printed. 
int XC::AnalysisModel::updateDomain(void)
  {
    // check to see there is a XC::Domain linked to the Model
    int res= 0;
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; WARNING no domain linked.\n";
    else
      {
        // invoke the method
        res= dom->update();
        if(res==0)
          res+= getHandlerPtr()->update();
      }
    return res;
  }


int XC::AnalysisModel::updateDomain(double newTime, double dT)
  {
    // check to see there is a domain linked to the Model
    int res= 0;
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; WARNING: no domain linked.\n";
    else
      {
        // invoke the method
        dom->applyLoad(newTime);
        res= getHandlerPtr()->applyLoad();
        if(res==0)
          res= dom->update();
        if(res==0)
          res= getHandlerPtr()->update();
      }
    return res;
  }


int XC::AnalysisModel::newStepDomain(double dT)
  {
    // check to see there is a domain linked to the Model
    int retval= -1;
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no domain linked."
                << std::endl;
    else
      retval= dom->newStep(dT); // invoke the domain method
    return retval;
  }


//! @brief Commits domain state.
//! 
//! Method which invokes commit() on the domain: this is an
//! operation which causes all nodes in the domain to take the current
//! values of response quantities and copy them into the accepted values. 
//! Returns \f$0\f$ if successful , a negative number if not: \f$-1\f$ if no
//! Domain has been set and \f$-2\f$ if commit() fails on the Domain.
int XC::AnalysisModel::commitDomain(void)
  {
    // check to see there is a domain linked to the Model
    int retval= -1;
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no domain linked."
                << std::endl;
    else
      {
        retval= dom->commit();
        if(retval<0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; WARNING: Domain::commit() failed\n";
            retval= -2;
          }
      }
    return retval;
  }

//! @brief Returns to the last committed state.
//!
//! Method which invokes revertToLastCommit() on the domain: this is an
//! operation which causes all nodes in the domain to set the trial
//! response quantities equal to the last committed response quantities.
//! Returns \f$0\f$ if successful , a negative number if not: \f$-1\f$ if no
//! Domain has been set and \f$-2\f$ if \p revertToLastCommit() fails on
//! the Domain. 
int XC::AnalysisModel::revertDomainToLastCommit(void)
  {
    // check to see there is a XC::Domain linked to the Model
    int retval= -1;
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no domain linked."
                << std::endl;
    else if(dom->revertToLastCommit()<0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed." << std::endl;
        retval= -2;
      }        
    return retval;
  }

//! To get the current time in the Domain.
//!
//! To get the current time in the Domain. If no Domain has been set a
//! warning message is printed and \f$0.0\f$ is returned, otherwise the result
//! of invoking getCurrentTime() on the Domain is returned.
double XC::AnalysisModel::getCurrentDomainTime(void) const
  {
    // check to see there is a Domain linked to the Model
    double retval= 0.0;
    const Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no domain linked."
                << std::endl;
    else
      retval= dom->getTimeTracker().getCurrentTime();
    return retval;
  }


//! To set the current time in the Domain to be \p newTime.
//! 
//! To set the current time in the Domain to be \p newTime. If no
//! Domain has been set a warning message is printed, otherwise 
//! {\em setCurrentTime(newTime)} is invoked on the Domain.
void XC::AnalysisModel::setCurrentDomainTime(double newTime)
  {
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no domain linked."
                << std::endl;
    else
      dom->setCurrentTime(newTime);
  }



void XC::AnalysisModel::setRayleighDampingFactors(const RayleighDampingFactors &rF)
  {
    Domain *dom= getDomainPtr();
    if(!dom)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; no domain linked."
                << std::endl;
    else
      dom->setRayleighDampingFactors(rF); // invoke the method
  }


XC::ModelWrapper *XC::AnalysisModel::getModelWrapper(void)
  { return dynamic_cast<ModelWrapper *>(Owner()); }


const XC::ModelWrapper *XC::AnalysisModel::getModelWrapper(void) const
  { return dynamic_cast<const ModelWrapper *>(Owner()); }


//! @brief Returns a const pointer to the associated Domain, that is the Domain
//! set when the links were set. 
const XC::Domain *XC::AnalysisModel::getDomainPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getDomainPtr();
  }

//! @brief Returns a pointer to the associated Domain, that is the Domain
//! set when the links were set. 
XC::Domain *XC::AnalysisModel::getDomainPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getDomainPtr();
  }

const XC::ConstraintHandler *XC::AnalysisModel::getHandlerPtr(void) const
  {
    const ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getConstraintHandlerPtr();
  }

XC::ConstraintHandler *XC::AnalysisModel::getHandlerPtr(void)
  {
    ModelWrapper *sm= getModelWrapper();
    assert(sm);
    return sm->getConstraintHandlerPtr();
  }


//! Returns \f$0\f$. Note the FE\_Elements and DOF\_Group objects are not sent
//! as they are not MovableObjects. AnalysisModel objects are only sent
//! when setting up a DomainDecompAnalysis on a remote process; only type
//! info and whatever subclasses might need need to be sent.
int XC::AnalysisModel::sendSelf(CommParameters &cp)
  { return 0; }

//! Returns \f$0\f$.
int XC::AnalysisModel::recvSelf(const CommParameters &cp) 
  { return 0; }

