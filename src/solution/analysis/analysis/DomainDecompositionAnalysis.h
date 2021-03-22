// -*-c++-*-
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
                                                                        
// $Revision: 1.4 $
// $Date: 2005/11/29 23:36:47 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/DomainDecompositionAnalysis.h,v $
                                                                        
                                                                        
// Written: fmk 
// Created: Tue Sept 17 16:34:47: 1996
// Revision: A
//
// Description: This file contains the class definition for 
// DomainDecompositionAnalysis. DomainDecompositionAnalysis is a subclass 
// of AnalysisAnalysis, it is used when performing a domain decomposition
// analysis. It provides methods which can be invoked by a subdomain to
// perform the numerical computations required.
//
// What: "@(#) DomainDecompositionAnalysis.h, revA"

#ifndef DomainDecompositionAnalysis_h
#define DomainDecompositionAnalysis_h

#include <solution/analysis/analysis/Analysis.h>
#include "utility/matrix/Vector.h"
#include <utility/actor/actor/MovableObject.h>

namespace XC {
class Subdomain;
 class Matrix;
 class DomainSolver;

//! @ingroup AnalysisType
//
//! @brief Used when performing a domain decomposition
//! analysis. It provides methods which can be invoked by a subdomain to
//! perform the numerical computations required.
//!
//! Used when performing an analysis using the domain decomposition
//! method to solve the equations. Its public member functions are all
//! virtual to allow for the generalization of the 
//! class. The following are the aggregates of such an analysis type:
//! - Subdomain: a pointer to the subdomain on which the analysis is performed. 
//! - DomainSolver: an algorithmic class specifying the
//!   sequence of operations to be performed in performing the numerical 
//!   operations.
class DomainDecompositionAnalysis: public Analysis, public MovableObject
  {
  private:
    Subdomain *theSubdomain; //!< Pointer to subdomain. 
    DomainSolver *theSolver; //!< Pointer to solver.
    Vector theResidual; //!< Residual load vector.
    int numEqn; //!< number of equations.
    int numExtEqn; //!< number of external equations.

    // the following 2 variables are used to allow formResidual()
    // and formTangVectProduct() to be called before formTangent()
    // - this must be allowed as typical elements will not have to fromTangent
    // before being asked to form Residual(). 
    bool tangFormed; //!< True if the tangent stiffness matrix is already formed.
    int tangFormedCount; //!< saves the expense of computing formTangent() for same state of Subdomain.
  protected:
    int domainStamp;
    //! @brief Returns a pointer to the subdomain.
    inline Subdomain *getSubdomainPtr(void) const
      { return theSubdomain; }

    void set_all_links(void);

    friend class SolutionProcedure;
    friend class FEM_ObjectBroker;
    DomainDecompositionAnalysis(Subdomain &theDomain,SolutionStrategy *s= nullptr);
    DomainDecompositionAnalysis(Subdomain &theDomain,DomainSolver &theSolver,SolutionStrategy *s= nullptr);
    DomainDecompositionAnalysis(int classTag, Subdomain &theDomain,SolutionStrategy *s= nullptr);
    DomainDecompositionAnalysis(int classTag, Subdomain &theDomain,DomainSolver &theSolver,SolutionStrategy *s);
    Analysis *getCopy(void) const;
  public:

    virtual void clearAll(void);	    
    virtual int initialize(void);
    virtual int domainChanged(void);

    // methods for non standard domain deomposition analysis
    virtual bool doesIndependentAnalysis(void);    
    virtual int analyze(double dT);

    // methods for standard domain deomposition analysis
    // that do some form of condensation to the tangent
    virtual int  getNumExternalEqn(void);
    virtual int  getNumInternalEqn(void);
    virtual int  newStep(double dT);
    virtual int  computeInternalResponse(void);
    virtual int  formTangent(void);
    virtual int  formResidual(void);
    virtual int  formTangVectProduct(Vector &force);
    virtual const Matrix &getTangent(void);
    virtual const Vector &getResidual(void);
    virtual const Vector &getTangVectProduct(void);
    
    virtual const DomainSolver *getDomainSolver(void) const;
    virtual DomainSolver *getDomainSolver(void);
    virtual const Subdomain *getSubdomain(void) const;
    virtual Subdomain *getSubdomain(void);

    virtual int sendSelf(Communicator &);
    virtual int recvSelf(const Communicator &);

    // methods to change the analysis aggregates
    virtual int setAlgorithm(EquiSolnAlgo &theAlgorithm);
    virtual int setIntegrator(IncrementalIntegrator &theIntegrator);
    virtual int setLinearSOE(LinearSOE &theSOE);
    virtual int setConvergenceTest(ConvergenceTest &theTest);
  };

} // end of XC namespace

#endif











