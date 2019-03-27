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

// $Revision: 1.5 $
// $Date: 2005/11/29 22:42:42 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/algorithm/equiSolnAlgo/NewtonLineSearch.cpp,v $

// Written: fmk
// Created: 11/96
// Modified: Ed "C++" Love 10/00 to perform the line search
//
// Description: This file contains the implementation for XC::NewtonLineSearch.
//
// What: "@(#)NewtonLineSearch.h, revA"

#include <solution/analysis/algorithm/equiSolnAlgo/NewtonLineSearch.h>
//Line search.
#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/LineSearch.h>
#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/BisectionLineSearch.h>
#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/InitialInterpolatedLineSearch.h>
#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/RegulaFalsiLineSearch.h>
#include <solution/analysis/algorithm/equiSolnAlgo/lineSearch/SecantLineSearch.h>

#include <solution/analysis/model/AnalysisModel.h>
#include <solution/analysis/analysis/StaticAnalysis.h>
#include <solution/analysis/integrator/IncrementalIntegrator.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <solution/analysis/convergenceTest/ConvergenceTest.h>
#include <utility/matrix/ID.h>
#include "solution/AnalysisAggregation.h"


//! @brief Null Constructor
XC::NewtonLineSearch::NewtonLineSearch(AnalysisAggregation *owr)
  :EquiSolnAlgo(owr,EquiALGORITHM_TAGS_NewtonLineSearch), theLineSearch(nullptr) {}

XC::NewtonLineSearch::NewtonLineSearch(const NewtonLineSearch &other)
  : EquiSolnAlgo(other), theLineSearch(nullptr)
  { if(other.theLineSearch) copy(other.theLineSearch); }

XC::NewtonLineSearch &XC::NewtonLineSearch::operator=(const NewtonLineSearch &other)
  {
    EquiSolnAlgo::operator=(other);
    if(other.theLineSearch) copy(other.theLineSearch);
    return *this;
  }

void XC::NewtonLineSearch::free_mem(void)
  {
    if(theLineSearch)
      {
        delete theLineSearch;
        theLineSearch= nullptr;
      }
  }

bool XC::NewtonLineSearch::alloc(const std::string &nmb)
  {
    free_mem();
    if(nmb=="bisection_line_search")
      theLineSearch=new BisectionLineSearch();
    else if(nmb=="initial_interpolated_line_search")
      theLineSearch=new InitialInterpolatedLineSearch();
    else if(nmb=="regula_falsi_line_search")
      theLineSearch=new RegulaFalsiLineSearch();
    else if(nmb=="secant_line_search")
      theLineSearch=new SecantLineSearch();
    if(theLineSearch)
      theLineSearch->set_owner(this);
    return (theLineSearch!=nullptr);
  }

void XC::NewtonLineSearch::copy(LineSearch *ptr)
  {
    if(ptr)
      {
        free_mem();
        theLineSearch= ptr->getCopy();
        theLineSearch->set_owner(this);
      }
    else
     std::cerr << "NewtonLineSearch::copy; pointer to line search is null." << std::endl;
  }

XC::NewtonLineSearch::~NewtonLineSearch(void)
  { free_mem(); }

//! @brief resuelve el paso actual.
int XC::NewtonLineSearch::solveCurrentStep(void)
  {
    // set up some pointers tond check they are valid
    // NOTE this could be taken away if we set Ptrs as protecetd in superclass
    AnalysisModel *theAnaModel= getAnalysisModelPtr();
    IncrementalIntegrator *theIntegrator= getIncrementalIntegratorPtr();
    LinearSOE *theSOE= getLinearSOEPtr();
    ConvergenceTest *theTest= getConvergenceTestPtr();

    if((!theAnaModel) || (!theIntegrator) || (!theSOE) || (!theTest))
      {
        std::cerr << "WARNING XC::NewtonLineSearch::solveCurrentStep() - setLinks() has";
        std::cerr << "no se ha asignado modelo, integrator o system of equations.\n";
        return -5;
      }

    theLineSearch->newStep(*theSOE);

    // set itself as the XC::ConvergenceTest objects XC::EquiSolnAlgo
    theTest->set_owner(getAnalysisAggregation());
    if(theTest->start() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
        std::cerr << "the convergence test object failed in start()\n";
        return -3;
      }

    if(theIntegrator->formUnbalance() < 0)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
        std::cerr << "the integrator failed in formUnbalance()\n";
        return -2;
      }

    int result = -1;
    do
      {
        //residual at this iteration before next solve
        const Vector &Resid0 = theSOE->getB() ;

        //form the tangent
        if(theIntegrator->formTangent() < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
            std::cerr << "the integrator failed in formTangent()\n";
            return -1;
          }

        //solve
        if(theSOE->solve() < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
            std::cerr << "the LinearSysOfEqn failed in solve()\n";
            return -3;
          }

        //line search direction
        const Vector &dx0 = theSOE->getX() ;

        //intial value of s
        double s0= - (dx0 ^ Resid0) ;

       if(theIntegrator->update(theSOE->getX()) < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
            std::cerr << "the integrator failed in update()\n";
            return -4;
          }

        if(theIntegrator->formUnbalance() < 0)
          {
            std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
            std::cerr << "the integrator failed in formUnbalance()\n";
            return -2;
          }

        //new unbalanced vector
        const Vector &Resid = theSOE->getB() ;

        //new valur for s
        const double s= - ( dx0 ^ Resid );

        if(theLineSearch)
          theLineSearch->search(s0, s, *theSOE, *theIntegrator);

        this->record(0); //Calls record method for all the recorders.
        result = theTest->test();
      }
    while (result == -1);

    if(result == -2)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__ << "; ";
        std::cerr << "the convergence test object failed in test()\n"
                  << "convergence test message: "
		  << theTest->getStatusMsg(1) << std::endl;

        return -3;
      }
    // note - if positive result we are returning what the convergence test returned
    // which should be the number of iterations
    return result;
  }

int XC::NewtonLineSearch::sendSelf(CommParameters &cp)
  { return -1; }

int XC::NewtonLineSearch::recvSelf(const CommParameters &cp)
  { return -1; }

void XC::NewtonLineSearch::Print(std::ostream &s, int flag)
  {
    if(flag == 0)
      s << "NewtonLineSearch\n";
    if(theLineSearch != 0)
      theLineSearch->Print(s, flag);
  }









