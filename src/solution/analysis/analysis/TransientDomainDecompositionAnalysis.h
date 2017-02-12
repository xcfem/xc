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
                                                                        
// $Revision: 1.1 $
// $Date: 2005/12/01 00:07:57 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/TransientDomainDecompositionAnalysis.h,v $
                                                                        
// Written: fmk 
// Revision: A
//
// Description: This file contains the class definition for 
// TransientDomainDecompositionAnalysis.TransientDomainDecompositionAnalysis is a subclass 
// of DomainDecompositionAnalysis used to perform a static analysis step on a subdomain.

//
// What: "@(#) TransientDomainDecompositionAnalysis.h, revA"

#ifndef TransientDomainDecompositionAnalysis_h
#define TransientDomainDecompositionAnalysis_h

#include <solution/analysis/analysis/DomainDecompositionAnalysis.h>

namespace XC {
class Subdomain;

//! @ingroup AnalysisType
//
//! @brief Transient analysis witn domain decomposition.
class TransientDomainDecompositionAnalysis: public DomainDecompositionAnalysis
  {
    friend class ProcSolu;
    TransientDomainDecompositionAnalysis(Subdomain &theDomain,SoluMethod *s= NULL);
    Analysis *getCopy(void) const;
  public:
    ~TransientDomainDecompositionAnalysis(void);

    void clearAll(void);
    int initialize(void);
    int domainChanged(void);

    // methods for non standard domain deomposition analysis
    int analyze(double dT);
    bool doesIndependentAnalysis(void);    

    // methods for standard domain deomposition analysis
    // that do some form of condensation to the tangent
    int getNumExternalEqn(void);
    int getNumInternalEqn(void);
    int newStep(double dT);
    int computeInternalResponse(void);
    int formTangent(void);
    int formResidual(void);
    int formTangVectProduct(Vector &force);
    const Matrix &getTangent(void);
    const Vector &getResidual(void);
    const Vector &getTangVectProduct(void);

    // methods to change the analysis aggregates
    int setAlgorithm(EquiSolnAlgo &theAlgorithm);
    int setIntegrator(IncrementalIntegrator &theIntegrator);
    int setLinearSOE(LinearSOE &theSOE);

    // methods to send/receive
    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);
  };
inline Analysis *TransientDomainDecompositionAnalysis::getCopy(void) const
  { return new TransientDomainDecompositionAnalysis(*this); }
} // end of XC namespace

#endif











