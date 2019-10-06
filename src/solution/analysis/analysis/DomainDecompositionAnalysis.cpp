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
                                                                        
// $Revision: 1.6 $
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/DomainDecompositionAnalysis.cpp,v $
                                                                        
                                                                        
// File: ~/analysis/Analysis/DomainDecompositionAnalysis.C
// 
// Written: fmk 
// Created: Tue Sept 17 16:34:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// DomainDecompositionAnalysis. DomainDecompositionAnalysis is a subclass 
// of AnalysisAnalysis, it is used to perform the static condensation process
// on a subdomain.
//
// What: "@(#) DomainDecompositionAnalysis.C, revA"


#include <solution/analysis/analysis/DomainDecompositionAnalysis.h>
#include "solution/analysis/ModelWrapper.h"
#include "solution/AnalysisAggregation.h"
#include <solution/analysis/handler/ConstraintHandler.h>
#include <solution/analysis/numberer/DOF_Numberer.h>
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/algorithm/domainDecompAlgo/DomainDecompAlgo.h>
#include <solution/analysis/algorithm/equiSolnAlgo/EquiSolnAlgo.h>
#include <solution/system_of_eqn/linearSOE/DomainSolver.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include "domain/domain/subdomain/Subdomain.h"

#include <solution/analysis/model/dof_grp/DOF_Group.h>
#include <utility/matrix/ID.h>
#include <domain/mesh/node/Node.h>

//! @brief Sets los pointers tol DomainDecompSolutionAlgorithm y al subdomain.
void XC::DomainDecompositionAnalysis::set_all_links(void)
  {
    theSubdomain->setDomainDecompAnalysis(*this);
  }

//! @brief Constructor.
//!
//! A constructor that is used when creating a DomainDecompositionObject which
//! is to receive itself afterwards. Sets the links to the Subdomain. It is
//! essential that this object recvSelf() before DomainDecompositionAnalysis
//! methods are invoked as their invocation will cause segmentation faults.
//!
//! Invokes {\em setAnalysis(this)} on the Subdomain.
//! @param subDomain: Subdomain to deal with.
//! @param s: solution method to use.
XC::DomainDecompositionAnalysis::DomainDecompositionAnalysis(Subdomain &subDomain,AnalysisAggregation *s)
  : Analysis(s),
    MovableObject(DomDecompANALYSIS_TAGS_DomainDecompositionAnalysis),
    theSubdomain(&subDomain),
    theSolver(nullptr),
    numEqn(0),numExtEqn(0),tangFormed(false),tangFormedCount(0),
    domainStamp(0)
  {
    theSubdomain->setDomainDecompAnalysis(*this);
  }

//! @brief Constructor.
//!
//! @param subDomain: subdomain to deal with.
//! @param theSolvr: Solver to use.
//! @param s: solution method to use.
XC::DomainDecompositionAnalysis::DomainDecompositionAnalysis(Subdomain &subDomain,DomainSolver &theSlvr,AnalysisAggregation *s)
  : Analysis(s),
    MovableObject(DomDecompANALYSIS_TAGS_DomainDecompositionAnalysis),
    theSubdomain(&subDomain),
    theSolver(&theSlvr),
    numEqn(0),numExtEqn(0),tangFormed(false),tangFormedCount(0),
    domainStamp(0)
  {
    theSubdomain->setDomainDecompAnalysis(*this);
  }

//! @brief Constructor.
//!
//! @param clsTag: class identifier.
//! @param subDomain: subdomain to deal with.
//! @param s: solution method to use.
XC::DomainDecompositionAnalysis::DomainDecompositionAnalysis(int clsTag, Subdomain &subDomain,AnalysisAggregation *s)
  : Analysis(s),
    MovableObject(clsTag),
    theSubdomain(&subDomain),
    theSolver(nullptr), numEqn(0),numExtEqn(0),tangFormed(false),tangFormedCount(0),
    domainStamp(0) {}

//! @brief Constructor.
//!
//! @param clsTag: class identifier.
//! @param subDomain: subdomain to deal with.
//! @param theSolver: Solver to use.
//! @param s: solution method to use.
XC::DomainDecompositionAnalysis::DomainDecompositionAnalysis(int clsTag, Subdomain &theDomain,DomainSolver &theSolver,AnalysisAggregation *s)
  : Analysis(s),
    MovableObject(clsTag),
    theSubdomain(&theDomain),
    theSolver(&theSolver), numEqn(0),numExtEqn(0),tangFormed(false),tangFormedCount(0),
    domainStamp(0) {}

//! @brief Virtual constructor.
XC::Analysis *XC::DomainDecompositionAnalysis::getCopy(void) const
  { return new DomainDecompositionAnalysis(*this); }

//! @brief Clears all object members (constraint handler, analysis model,...).
void XC::DomainDecompositionAnalysis::clearAll(void)
  {
    // invoke the destructor on all the objects in the aggregation
    Analysis::clearAll();
  }

//! @brief Performs the analysis.
//!
//! @param dT: time increment.
int XC::DomainDecompositionAnalysis::analyze(double dT)
  { return 0; }

int XC::DomainDecompositionAnalysis::initialize(void)
  { return 0; }

//! @brief Returns a pointer to the DomainSolver associated with
//! this object.
const XC::DomainSolver *XC::DomainDecompositionAnalysis::getDomainSolver(void) const
  { return theSolver; }

//! @brief Returns a pointer to the DomainSolver.
XC::DomainSolver *XC::DomainDecompositionAnalysis::getDomainSolver(void)
  { return theSolver; }

//! @brief Returns a pointer to the subdomain.
const XC::Subdomain *XC::DomainDecompositionAnalysis::getSubdomain(void) const
  { return theSubdomain; }

//! @brief Returns a pointer to the subdomain.
XC::Subdomain *XC::DomainDecompositionAnalysis::getSubdomain(void)
  { return theSubdomain; }

bool XC::DomainDecompositionAnalysis::doesIndependentAnalysis(void)
  { return false; }

//! @brief Method used to inform the object that the domain has changed.
int XC::DomainDecompositionAnalysis::domainChanged(void)
  {
    // remove existing FE_elements and DOF_Groups from the Analysis
    getAnalysisModelPtr()->clearAll();
    getConstraintHandlerPtr()->clearAll();

    // now we invoke handle() on the constraint handler which
    // causes the creation of XC::FE_Element and XC::DOF_Group objects
    // and their addition to the AnalysisModel.

    numExtEqn= getConstraintHandlerPtr()->handle(&(theSubdomain->getExternalNodes()));

    // we now get a node to number last

    const XC::ID &theExtNodes= theSubdomain->getExternalNodes();
    int idSize= theExtNodes.Size();
    //    int theLastDOF= -1;

    ID theLastDOFs(1);
    int cnt= 0;

    // create an XC::ID containing the tags of the DOF_Groups that are to
    // be numbered last
    for (int i=0; i<idSize; i++) {
	int nodeTag= theExtNodes(i);
	Node *nodePtr= theSubdomain->getNode(nodeTag);
	DOF_Group *dofGrpPtr= nodePtr->getDOF_GroupPtr();
	if (dofGrpPtr != 0) {
	    const XC::ID theID= dofGrpPtr->getID();
	    int size= theID.Size();
	    for (int j=0; j<size; j++)
		if (theID(j) == -3) {
		    theLastDOFs[cnt] = dofGrpPtr->getTag();
		    cnt++;
		    j= size;
		}
	}
    }

    // we now invoke number() on the numberer which causes
    // equation numbers to be assigned to all the DOFs in the
    // AnalysisModel.    

    getDOF_NumbererPtr()->numberDOF(theLastDOFs);

    // we invoke setSize() on the LinearSOE which
    // causes that object to determine its size    
    
    getLinearSOEPtr()->setSize(getAnalysisModelPtr()->getDOFGraph());    
    numEqn= getLinearSOEPtr()->getNumEqn();

    // we invoke domainChange() on the integrator and algorithm

    getIncrementalIntegratorPtr()->domainChanged();
    getDomainDecompSolutionAlgorithmPtr()->domainChanged();        

    // now set the variables to indicate that tangent has not been formed

    tangFormed= false;
    tangFormedCount= 0;
    
    return 0;
  }

//! @brief Returns the number of external equations.
//!
//! A method to return the number of external degrees-of-freedom on the
//! Subdomain interface, this information is returned when handle()
//! is invoked on \p theConstraintHandler.
int XC::DomainDecompositionAnalysis::getNumExternalEqn(void)
  { return numExtEqn; }

//! @brief Returns the number of internal equations.
int XC::DomainDecompositionAnalysis::getNumInternalEqn(void)
  { return numEqn-numExtEqn; }


int XC::DomainDecompositionAnalysis::newStep(double dT)
  { return getIncrementalIntegratorPtr()->newStep(dT); }

//! @brief A method which invokes solveCurrentStep() on \p theAlgorithm.
int XC::DomainDecompositionAnalysis::computeInternalResponse(void)
  {  return getDomainDecompSolutionAlgorithmPtr()->solveCurrentStep(); }



//! @brief Assembles the tangent stiffness matrix.
//!
//! A method to form the condensed tangent matrix, given the current
//! number of internal dof. It first checks to see if the Subdomain has
//! changed, by  invoking hasDomainChanged() on the Subdomain; if it
//! has invokeChangeOnAnalysis() is invoked on the {\em
//! Subdomain}. It then checks to see if \p counter is equal to \f$-1\f$ or
//! not; a \f$-1\f$ indicating the tangent has already been formed in order
//! that the residual could be determined. If this is not the case {\em
//! formTangent()} is invoked on \p theIntegrator, condenseA() is
//! invoked on \p theSolver object, a flag is set to indicate that the
//! tangent has been formed, and the \p counter is incremented. Returns
//! a \f$0\f$ if successful, if either the formTangent() or {\em
//! condenseA()} method returns a negative number this number is
//! returned.  
int XC::DomainDecompositionAnalysis::formTangent(void)
  {
    int result =0;

    Domain *the_Domain= this->getDomainPtr();

    // we check to see if the domain has changed 
    int stamp= the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp= stamp;
	this->domainChanged();
      }
    
    // if tangFormed == -1 then formTangent has already been
    // called for this state by formResidual() or formTangVectProduct()
    // so we won't be doing it again.

    if(tangFormedCount != -1)
      {
	result= getIncrementalIntegratorPtr()->formTangent();
	if(result < 0)
	  return result;
	result= theSolver->condenseA(numEqn-numExtEqn);
	if(result < 0)
	  return result;
      }
	
    tangFormed= true;
    tangFormedCount++;
    
    return result;
  }


//! @brief Assembles the residuial vector.
//!
//! A method to form the condensed residual vector, given the current
//! number of internal dof. A check to see if the Subdomain has changed is
//! first made, this is done by invoking hasDomainChanged() on the Subdomain;
//! if it has been modified invokeChangeOnAnalysis() 
//! is invoked on the \p Subdomain. If the tangent has not yet been formed
//! it invokes formTangent() on itself and sets the \p counter to \f$-1\f$.
//! To form the residual formUnbalance() is invoked on \p theIntegrator and
//! {\em condenseRHS(numInt)} is invoked on \p theSolver. Returns \f$0\f$ or
//! the negative number that was returned if either formUnbalance()}
//! or {\em condenseRHS() failed.
int XC::DomainDecompositionAnalysis::formResidual(void)
  {
    int result =0;
    Domain *the_Domain= this->getDomainPtr();    
    
    // we check to see if the domain has changed 
    int stamp= the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp= stamp;
	this->domainChanged();
      }
    
    if(tangFormed == false)
      {
	result= this->formTangent();
	if(result < 0)
	  return result;
	tangFormedCount= -1; // set to minus number so tangent 
	                      // is not formed twice at same state
      }

    result= getIncrementalIntegratorPtr()->formUnbalance();

    if(result < 0)
      return result;
    return theSolver->condenseRHS(numEqn-numExtEqn);
  }


//! @brief form the product of the condensed tangent matrix times the
//! vector \f$u\f$.
//! 
//! A method to form the product of the condensed tangent matrix times the
//! vector \f$u\f$. A check to see if the Subdomain has changed is first made, 
//! this is done by invoking hasDomainChanged() on the Subdomain; if it has 
//! been modified invokeChangeOnAnalysis() 
//! is invoked on the \p Subdomain. If the tangent has not yet been formed
//! it invokes formTangent() on itself and sets the \p counter to \f$-1\f$.
//! Finally the result of invoking {\em computeCondensedMatVect(numInt, u)}
//! on {\em theSolver} is returned.
int XC::DomainDecompositionAnalysis::formTangVectProduct(Vector &u)
  {
    int result= 0;

    Domain *the_Domain= this->getDomainPtr();
    
    // we check to see if the domain has changed 
    int stamp= the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp= stamp;
	this->domainChanged();
      }
    
    if(tangFormed == false)
      {
	result= this->formTangent();
	if (result < 0)
	    return result;
	tangFormedCount= -1; // set to minus number so tangent 
	                      // is not formed twice at same state
      }    
    return theSolver->computeCondensedMatVect(numEqn-numExtEqn,u);
  }


//! @brief Return the tangent stiffness matrix.
//!
//! A method which returns the portion of A corresponding to internal
//! equation numbers. A check to see if the Subdomain has changed is first
//! made, this is done by invoking hasDomainChanged() on the Subdomain; if it
//! has been modified invokeChangeOnAnalysis() is invoked on the \p Subdomain. 
//! If the tangent has not yet been formed formTangent() is invoked. 
//! The method returns the result of invoking getCondensedA()}
//! on {\em theSolver().
const XC::Matrix &XC::DomainDecompositionAnalysis::getTangent(void)
  {
    Domain *the_Domain= this->getDomainPtr();
    
    // we check to see if the domain has changed 
    int stamp= the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp= stamp;
	this->domainChanged();
      }

    if(tangFormed == false)
      {	this->formTangent(); }
    return (theSolver->getCondensedA());
  }


//! @brief Returns the residual vector.
//!
//! A method which returns the portion of the \f$b\f$ corresponding
//! to the external equation numbers. A check to see if the Subdomain has
//! changed is first made, this is done by invoking hasDomainChanged() on the
//! Subdomain; if it has been modified invokeChangeOnAnalysis() is invoked on
//! the \p Subdomain and formResidual() is called.  The object returns the
//! Vector obtained from invoking getCondensedRHS() on the solver. 
const XC::Vector &XC::DomainDecompositionAnalysis::getResidual(void)
  {
    Domain *the_Domain= this->getDomainPtr();
    
    // we check to see if the domain has changed 
    // we check to see if the domain has changed 
    int stamp= the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp= stamp;
	this->domainChanged();
	this->formResidual();	
      }    
    theResidual= theSolver->getCondensedRHS();
    return theResidual;
  }

//! @brief Returns the result of invoking getCondensedMatVect() on the
//! solver.
//!
//! Returns the result of invoking getCondensedMatVect() on the
//! solver. A check to see if the Subdomain has changed is first made, 
//! this is done by invoking hasDomainChanged() on the Subdomain; if it has 
//! been modified invokeChangeOnAnalysis() is invoked on the \p Subdomain. 
//! The object returns the Vector obtained from invoking getCondensedMatVect() 
//! on \p theSolver.
const XC::Vector &XC::DomainDecompositionAnalysis::getTangVectProduct()
  {
    Domain *the_Domain= this->getDomainPtr();
    
    // we check to see if the domain has changed 
    int stamp= the_Domain->hasDomainChanged();
    if(stamp != domainStamp)
      {
	domainStamp= stamp;
	this->domainChanged();
      }
    return theSolver->getCondensedMatVect();
  }

//! @brief Sends the object.
int XC::DomainDecompositionAnalysis::sendSelf(CommParameters &cp)
  {
    // determine the type of each object in the aggregation,
    // store it in an ID and send the info off.
    getDbTag(cp);
    ID data(14);
    data(0)= getConstraintHandlerPtr()->getClassTag();
    data(1)= getDOF_NumbererPtr()->getClassTag();
    data(2)= getAnalysisModelPtr()->getClassTag();
    data(3)= getDomainDecompSolutionAlgorithmPtr()->getClassTag();
    data(4)= getIncrementalIntegratorPtr()->getClassTag();    
    data(5)= getLinearSOEPtr()->getClassTag();    
    data(6)= theSolver->getClassTag();        

    data(7)= getConstraintHandlerPtr()->getDbTag();
    data(8)= getDOF_NumbererPtr()->getDbTag();
    data(9)= getAnalysisModelPtr()->getDbTag();
    data(10)= getDomainDecompSolutionAlgorithmPtr()->getDbTag();
    data(11)= getIncrementalIntegratorPtr()->getDbTag();    
    data(12)= getLinearSOEPtr()->getDbTag();    
    data(13)= theSolver->getDbTag();        

    cp.sendID(data,getDbTagData(),CommMetaData(0));

    // invoke sendSelf on each object in the aggregation
    
    getConstraintHandlerPtr()->sendSelf(cp);
    getDOF_NumbererPtr()->sendSelf(cp);
    getAnalysisModelPtr()->sendSelf(cp);
    getDomainDecompSolutionAlgorithmPtr()->sendSelf(cp);
    getIncrementalIntegratorPtr()->sendSelf(cp);    
    getLinearSOEPtr()->sendSelf(cp);    
    theSolver->sendSelf(cp);            
    return 0;
  }

//! @brief Receives the object.
int XC::DomainDecompositionAnalysis::recvSelf(const CommParameters &cp)
  {
    // receive the data identifyng the objects in the aggregation
    ID data(14);
    getDbTag();
    cp.receiveID(data,getDbTagData(),CommMetaData(0));

    //
    // now ask the object broker an object of each type
    // and invoke recvSelf() on the object to init it.
    //

    brokeConstraintHandler(cp,ID(data(0)));
    if(getConstraintHandlerPtr())
      {
	getConstraintHandlerPtr()->setDbTag(data(7));
	getConstraintHandlerPtr()->recvSelf(cp);
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed to get the ConstraintHandler.\n";
	return -1;
      }

    brokeNumberer(cp,data);
    if(getDOF_NumbererPtr())
      {
	getDOF_NumbererPtr()->setDbTag(data(8));	
	getDOF_NumbererPtr()->recvSelf(cp);
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to get the DOF Numberer\n";
	return -1;
      }


    brokeAnalysisModel(cp,ID(data(2)));
    if(getAnalysisModelPtr())
      {
	getAnalysisModelPtr()->setDbTag(data(9));
	getAnalysisModelPtr()->recvSelf(cp);
      }
    else {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to get the AnalysisModel\n";
	return -1;
    }        


    brokeDomainDecompAlgo(cp,data);
    if(getDomainDecompSolutionAlgorithmPtr())
      {
	getDomainDecompSolutionAlgorithmPtr()->setDbTag(data(10));
	getDomainDecompSolutionAlgorithmPtr()->recvSelf(cp);
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to get the domain decomposition algorithm.\n";
	return -1;
      }            

    brokeIncrementalIntegrator(cp,data);
    if(getIncrementalIntegratorPtr())
      {
	getIncrementalIntegratorPtr()->setDbTag(data(11));
	getIncrementalIntegratorPtr()->recvSelf(cp);
      }
    else
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to get the IncrementalIntegrator.\n";
	return -1;
      }        	

    brokeDDLinearSOE(cp,ID(data(5)));
    theSolver= cp.getNewDomainSolver();

    if(getLinearSOEPtr() == 0 || theSolver == 0)
      {
	std::cerr << getClassName() << "::" << __FUNCTION__
	          << "; failed to get the LinearSOE or the solver.\n";
	return -1;
      }
    else
      {
	getLinearSOEPtr()->setDbTag(data(12));
	theSolver->setDbTag(data(13));
	getLinearSOEPtr()->recvSelf(cp);
	theSolver->recvSelf(cp);
      }

    // set the links in all the objects
    set_all_links();
    return 0;
  }

//! @brief Sets the solution algorithm to use in the analysis.
int XC::DomainDecompositionAnalysis::setAlgorithm(EquiSolnAlgo &theAlgorithm)
  {
    Analysis::setAlgorithm(theAlgorithm);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented\n";
    return -1;
  }

//! @brief Sets the integrator to use in the analysis.
int XC::DomainDecompositionAnalysis::setIntegrator(IncrementalIntegrator &theIntegrator) 
  {
    Analysis::setIntegrator(theIntegrator);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented\n";
    return -1;
  }


//! @brief Sets the system of equations to use in the analysis.
int XC::DomainDecompositionAnalysis::setLinearSOE(LinearSOE &theSOE)
  {
    Analysis::setLinearSOE(theSOE);
    std::cerr << getClassName() << "::" << __FUNCTION__
	      << "; not implemented\n";
    return -1;
  }

//! @brief Sets the convergence test to use in the analysis.
int XC::DomainDecompositionAnalysis::setConvergenceTest(ConvergenceTest &theTest)
  { return solution_method->setConvergenceTest(theTest); }


