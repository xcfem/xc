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

// $Revision: 1.15 $
// $Date: 2008-11-19 23:42:25 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/handler/AutoConstraintHandler.cpp,v $


// Written: Massimo Petracca
// Created: June 2024.
// Revision: A
//
// What: "@(#) AutoConstraintHandler.C, revA"

#include "AutoConstraintHandler.h"
#include <cstdlib>

#include "solution/analysis/model/AnalysisModel.h"
#include "domain/domain/Domain.h"
#include "solution/analysis/model/fe_ele/FE_Element.h"
#include "solution/analysis/model/fe_ele/penalty/PenaltyMFreedom_FE.h"
#include "solution/analysis/model/dof_grp/DOF_Group.h"
#include "domain/mesh/node/Node.h"
#include "domain/mesh/element/Element.h"
#include "domain/mesh/node/NodeIter.h"
#include "domain/mesh/element/ElementIter.h"
#include "domain/constraints/SFreedom_ConstraintIter.h"
#include "domain/constraints/SFreedom_Constraint.h"
#include "domain/constraints/MFreedom_ConstraintIter.h"
#include "domain/constraints/MFreedom_Constraint.h"
#include "domain/constraints/MRMFreedom_ConstraintIter.h"
#include "domain/constraints/MRMFreedom_Constraint.h"
#include "solution/analysis/integrator/Integrator.h"
#include "utility/matrix/ID.h"
#include "utility/matrix/Matrix.h"
#include "domain/domain/subdomain/Subdomain.h"
#include "solution/analysis/model/dof_grp/TransformationDOF_Group.h"
#include "domain/mesh/element/utils/NodePtrsWithIDs.h"

#include <cmath>
#include <algorithm>
#include <utility>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <iomanip>


namespace {

	/**
	A utility class to compute, for each MFreedom_Constraint, the penalty value 
        according to the (approximate) max stiffness value at the retained 
        and constrained nodes.
	*/
	class PenaltyEvaluator
	  {
	  public:
	    PenaltyEvaluator(void) = delete;
	    /**
	       Full constructor.

	       @param domain The domain
	       @param mps The list of MFreedom_Constraints used for the auto-computation of penalty values
	       @param penalty_oom The order-of-magnitude to be added to the stiffness order-of-magnitude to obtain the penalty value
	    */
	    PenaltyEvaluator(XC::Domain* domain, const std::vector<XC::MFreedom_Constraint*> mps, double penalty_oom)
	      {
		// store nodes involved in the input mps and init penalty to 0
		for (auto* mp : mps)
		  {
		    if (mp)
		      {
			m_node_penalty[mp->getNodeRetained()] = 0.0;
			m_node_penalty[mp->getNodeConstrained()] = 0.0;
		      }
		  }
		// eval elements connected to those nodes
		XC::Element* elePtr= nullptr;
		XC::ElementIter& theEles = domain->getElements();
		while((elePtr = theEles()) != 0)
		  {
		    if(!elePtr->isSubdomain())
		      {
			// element nodes. process only if at least one node is involved
			const XC::ID &elenodes = elePtr->getNodePtrs().getExternalNodes();
			bool found = false;
			for (int i = 0; i < elenodes.Size(); ++i)
			  {
			    if (m_node_penalty.count(elenodes(i)) > 0)
			      {
				found = true;
				break;
			      }
			  }
			if(!found)
			  continue;
			// eval max diagonal entry for this element
			const XC::Matrix &K= elePtr->getInitialStiff();
			double kmax = 0.0;
			for (int i = 0; i < K.noRows(); ++i)
			  {
			    double ki = std::abs(K(i, i));
			    kmax = std::max(kmax, ki);
			  }
			// and accumulte the same to each node (it's just an approximation)
			for(int i = 0; i < elenodes.Size(); ++i)
			  {
			    auto iter = m_node_penalty.find(elenodes(i));
			    if (iter != m_node_penalty.end())
			      iter->second += kmax;
			  }
		      }
		  }
		// now m_node_penalty contains the accumulated max diagonal stiffness entry
		// at each node from each element connected to that node.
		// now we can convert the stiffness value to the penalty value
		for (auto& item : m_node_penalty)
		  {
		    // the accumulated stiffness at this node
		    double kmax = item.second;
		    // its order of magnitude
		    double koom = std::round(std::log10(kmax));
		    // compute the penalty value
		    item.second = std::pow(10.0, koom + penalty_oom);
		  }
	      }
	    /**
	       returns the penalty value for the input mp constraint
	    */
	    inline double getPenaltyValue(const XC::MFreedom_Constraint* mp) const
	      {
		double value = 0.0;
		if (mp == nullptr) return value;
		auto itr = m_node_penalty.find(mp->getNodeRetained());
		if (itr != m_node_penalty.end()) value = std::max(value, itr->second);
		auto itc = m_node_penalty.find(mp->getNodeConstrained());
		if (itc != m_node_penalty.end()) value = std::max(value, itc->second);
		return value;
	      }
	public:
	    // maps a node id to the penalty value at that node
	    std::unordered_map<int, double> m_node_penalty;
	};
  } //End namespace.

//! @brief Constructor.
XC::AutoConstraintHandler::AutoConstraintHandler(ModelWrapper *owr)
  : ConstraintHandler(owr, HANDLER_TAG_AutoConstraintHandler)
  {}

//! @brief Constructor.
XC::AutoConstraintHandler::AutoConstraintHandler(ModelWrapper *owr,
	bool _verbose,
	bool _auto_penalty, 
	double _auto_penalty_oom,
	double _user_penalty)
  : ConstraintHandler(owr, HANDLER_TAG_AutoConstraintHandler)
  , verbose(_verbose)
  , auto_penalty(_auto_penalty)
  , auto_penalty_oom(_auto_penalty_oom)
  , user_penalty(_user_penalty)
  {}

//! @brief Virtual constructor.
XC::ConstraintHandler *XC::AutoConstraintHandler::getCopy(void) const
  { return new AutoConstraintHandler(*this); }

XC::AutoConstraintHandler::~AutoConstraintHandler()
  {}

int XC::AutoConstraintHandler::handle(const ID* nodesLast)
  {
    // first check links exist to a Domain and an AnalysisModel object
    Domain *theDomain = this->getDomainPtr ();
    AnalysisModel *theModel = this->getAnalysisModelPtr ();
    Integrator *theIntegrator = this->getIntegratorPtr ();
    if ((theDomain == 0) || (theModel == 0) || (theIntegrator == 0))
      {
	std::cerr << "WARNING AutoConstraintHandler::handle() - ";
	std::cerr << " setLinks() has not been called\n";
	return -1;
      }

    // create a multimap (key = NodeID, value = SFreedom_Constraint).
    // multimap allows for duplicate keys because we may have multiple SP
    // constraints on the same node.
    // in this way we have :
    // a) a fast access to the key (to tell whether a node is constrained or not)
    // b) and a range of SFreedom_Constraints acting on that node
    std::unordered_multimap<int, SFreedom_Constraint *> sp_map;
    {
      SFreedom_ConstraintIter &theSPs = theDomain->getConstraints().getDomainAndLoadPatternSPs();
      SFreedom_Constraint *theSP;
      while ((theSP = theSPs ()) != 0)
	sp_map.emplace (std::make_pair (theSP->getNodeTag (), theSP));
    }

    // create a DOF_Group for each Node and add it to the AnalysisModel
    // create a TrasformationDOF_Group for nodes constrained in SFreedom_Constraints
    int countDOF = 0;
    {
      NodeIter &theNodes = theDomain->getNodes ();
      Node *nodPtr;
      int numDofGrp = 0;
      while ((nodPtr = theNodes ()) != 0)
	{

	  // process this node
	  DOF_Group *dofPtr = 0;
	  int nodeTag = nodPtr->getTag ();

	  // if the node is constrained in an SP constraint
	  // handle it with the transformation method
	  auto it_range = sp_map.equal_range (nodeTag);
	  if (it_range.first != it_range.second)
	    {

	      // this node is constrained in 1 or more SFreedom_Constraints.
	      // handle it with a TransformationDOF_Group.
	      // note: the TransformationConstraintHandler pointer is useless in
	      // TDOF...
	      TransformationDOF_Group *tDofPtr
		  = new TransformationDOF_Group (numDofGrp++, nodPtr, nullptr);
	      dofPtr = tDofPtr;

	      // count and add all SFreedom_Constraints.
	      // if verbose, keep track of conflicting constraints
	      int numSPs = 0;
	      for (auto it = it_range.first; it != it_range.second; it++)
		{
		  tDofPtr->addSFreedom_Constraint (*(it->second));
		  numSPs++;
		}

	      // add the DOF to the array
	      theDOFs.push_back (tDofPtr);
	      // count only free DOFs
	      countDOF += nodPtr->getNumberDOF () - numSPs;
	    }
	  else
	    {

	      // free node, create an ordinary DOF_Group
	      dofPtr = new DOF_Group (numDofGrp++, nodPtr);
	      // count all DOFs
	      countDOF += nodPtr->getNumberDOF ();
	    }

	  // add the DOF Group to the model
	  nodPtr->setDOF_GroupPtr (dofPtr);
	  theModel->addDOF_Group (dofPtr);
	}
    }

    // set the number of equations
    theModel->setNumEqn (countDOF);

    // now see if we have to set any of the dof's to -3
    int count3 = 0;
    if (nodesLast != 0)
      {
	for (int i = 0; i < nodesLast->Size (); i++)
	  {
	    int nodeID = (*nodesLast) (i);
	    Node *nodPtr = theDomain->getNode (nodeID);
	    if (nodPtr != 0)
	      {
		DOF_Group *dofPtr = nodPtr->getDOF_GroupPtr ();
		const ID &id = dofPtr->getID ();
		// set all the dof values to -3
		for (int j = 0; j < id.Size (); j++)
		  {
		    if (id (j) == -2)
		      {
			dofPtr->setID (j, -3);
			count3++;
		      }
		    else
		      {
			std::cerr << "WARNING AutoConstraintHandler::handle()"
				     " - boundary sp constraint in subdomain "
				     "this should not be "
				     "- results suspect \n";
		      }
		  }
	      }
	  }
      }

    // create the FE_Elements for the Elements and add to the AnalysisModel
    ElementIter &theEle = theDomain->getElements ();
    Element *elePtr;
    int numFeEle = 0;
    FE_Element *fePtr;

    // first standard elements
    while ((elePtr = theEle ()) != 0)
      {
	// only create an FE_Element for a subdomain element if it does not
	// do independent analysis .. then subdomain part of this analysis so
	// create an FE_element & set subdomain to point to it.
	if (elePtr->isSubdomain () == true)
	  {
	    Subdomain *theSub = (Subdomain *)elePtr;
	    if (theSub->doesIndependentAnalysis () == false)
	      {
		fePtr = new FE_Element (numFeEle++, elePtr);
		theModel->addFE_Element (fePtr);
		theSub->setFE_ElementPtr (fePtr);
	      }
	  }
	else
	  {
	    // just a regular element .. create an FE_Element for it & add to
	    // AnalysisModel
	    fePtr = new FE_Element (numFeEle++, elePtr);
	    theModel->addFE_Element (fePtr);
	  }
      }

    // then MP Penalty elements
    // Note: now we get all MPs and handle them with penalty.
    // In the future we may split valid MPs and conflicting MPs. valid one will
    // be handled by the transformation method
    std::vector<MFreedom_Constraint *> mps_penalty;
    {
      MFreedom_ConstraintIter &theMPs = theDomain->getConstraints().getMPs();
      MFreedom_Constraint *mpPtr;
      while ((mpPtr = theMPs ()) != 0)
	mps_penalty.push_back (mpPtr);
    }
    std::shared_ptr<PenaltyEvaluator> peval;
    if (auto_penalty)
      peval = std::make_shared<PenaltyEvaluator> (theDomain, mps_penalty,
						  auto_penalty_oom);
    for(MFreedom_Constraint *mp : mps_penalty)
      {
	const double penalty
	    = auto_penalty ? peval->getPenaltyValue (mp) : user_penalty;
	fePtr= new PenaltyMFreedom_FE(numFeEle, *theDomain, *mp, penalty);
	theModel->addFE_Element(fePtr);
	numFeEle++;
      }

    // give some info
    if (verbose)
      {
	std::stringstream ss;
	ss << "AutoConstraint Handler report:\n";
	ss << "+ SP Constraints:\n";
	ss << "   + " << sp_map.size ()
	   << " constraints handled with the Transformation method\n";
	if (sp_map.size () > 0)
	  {
	    NodeIter &theNodes = theDomain->getNodes ();
	    Node *nodPtr;
	    while ((nodPtr = theNodes ()) != 0)
	      {
		auto it_range = sp_map.equal_range (nodPtr->getTag ());
		if (it_range.first == it_range.second)
		  continue;
		std::vector<int> sp_per_dof (
		    static_cast<std::size_t> (nodPtr->getNumberDOF ()), 0);
		for (auto it = it_range.first; it != it_range.second; it++)
		  {
		    SFreedom_Constraint *sp = it->second;
		    sp_per_dof[static_cast<std::size_t> (sp->getDOF_Number ())]
			+= 1;
		  }
		ss << "      - Node: " << nodPtr->getTag () << " [ ";
		for (auto i : sp_per_dof)
		  ss << i << " ";
		ss << "]\n";
		for (auto it = it_range.first; it != it_range.second; it++)
		  {
		    SFreedom_Constraint *sp = it->second;
		    int how_many = sp_per_dof[static_cast<std::size_t> (
			sp->getDOF_Number ())];
		    if (how_many > 1)
		      {
			ss << "         ! Warning: @DOF " << sp->getDOF_Number ()
			   << ". Found " << how_many
			   << " SP constraints. Only the SP (tag = "
			   << sp->getTag () << ", value = " << sp->getValue ()
			   << ") will be imposed\n";
		      }
		  }
	      }
	  }
	ss << "+ MP Constraints:\n";
	ss << "   + " << mps_penalty.size ()
	   << " constraints handled with the Penalty method\n";
	if (auto_penalty)
	  {
	    ss << "   + Automatic Penalty values for each MP Constraint:\n";
	    for (MFreedom_Constraint *mp : mps_penalty)
	      ss << "      + MP(tag = " << mp->getTag ()
		 << ") = " << std::scientific << peval->getPenaltyValue (mp)
		 << "\n";
	  }
	else
	  {
	    ss << "   + Uniform User-Defined penalty = " << std::scientific
	       << user_penalty << "\n";
	  }
	std::string ss_value = ss.str ();
	std::cerr << ss_value.c_str ();
      }

    // done
    return count3;
  }

void XC::AutoConstraintHandler::clearAll(void)
  {
    // reset the TransformationDOF_Group vector
    // don't delete the pointer inside the vector (owned by the model)
    theDOFs.clear();

    // for the nodes reset the DOF_Group pointers to 0
    Domain* theDomain= this->getDomainPtr();
    if (theDomain == 0)
      return;
    NodeIter& theNod = theDomain->getNodes();
    Node* nodPtr= nullptr;
    while ((nodPtr = theNod()) != 0)
      nodPtr->setDOF_GroupPtr(0);
  }

//! @brief Sends object members through the communicator argument.
int XC::AutoConstraintHandler::sendData(Communicator &comm)
  {
    int res= ConstraintHandler::sendData(comm);
    res+= comm.sendBools(verbose, auto_penalty, getDbTagData(),CommMetaData(2));
    res+= comm.sendDoubles(auto_penalty_oom, user_penalty, getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::AutoConstraintHandler::recvData(const Communicator &comm)
  {
    int res= ConstraintHandler::recvData(comm);
    res+= comm.receiveBools(verbose, auto_penalty, getDbTagData(),CommMetaData(2));
    res+= comm.receiveDoubles(auto_penalty_oom, user_penalty, getDbTagData(),CommMetaData(3));
    return res;
  }
//! @brief Sends object through the communicator argument.
int XC::AutoConstraintHandler::sendSelf(Communicator &comm)
  {
    inicComm(4);

    int res= sendData(comm);

    const int dataTag= getDbTag();
    res += comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send ID data.\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::AutoConstraintHandler::recvSelf(const Communicator &comm)
  {
    inicComm(4);

    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ID data.\n";
    else
      res+= recvData(comm);
    return res;
  }

int XC::AutoConstraintHandler::applyLoad(void)
  {
    // enforse SP constraints
    // is there a reason for a backward loop?
    std::size_t numDOF = theDOFs.size();
    for (std::size_t i = 1; i <= numDOF; i++)
      theDOFs[numDOF - i]->enforceSPs(1);
    for (std::size_t i = 1; i <= numDOF; i++)
      theDOFs[numDOF - i]->enforceSPs(0);
    return 0;
  }

int XC::AutoConstraintHandler::doneNumberingDOF(void)
  {
    // iterate through the DOF_Groups telling them that their ID has now been set
    DOF_GrpIter& theDOFS = this->getAnalysisModelPtr()->getDOFGroups();
    DOF_Group* dofPtr;
    while ((dofPtr = theDOFS()) != 0)
      dofPtr->doneID();
    
    // iterate through the FE_Element getting them to set their IDs
    // done using base class implementation
    return ConstraintHandler::doneNumberingDOF();
  }
