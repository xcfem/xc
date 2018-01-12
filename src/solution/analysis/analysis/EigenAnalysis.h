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
                                                                        
// $Revision: 1.1.1.1 $
// $Date: 2000/09/15 08:23:16 $
// $Source: /usr/local/cvs/OpenSees/SRC/analysis/analysis/EigenAnalysis.h,v $
                                                                        
                                                                        
// File: ~/analysis/analysis/eigenAnalysis/EigenAnalysis.h
//
// Written: Jun Peng
// Created: Wed Jan 27, 1999
// Revision: A
//
// Description: This file contains the class definition of EigenAnalysis.
// EigenAnalysis is a subclass of Analysis, it is used to perform the 
// eigen value analysis on the FE_Model.
//
// This class is inheritanted from the base class of Analysis
// which was created by fmk (Frank).


#ifndef EigenAnalysis_h
#define EigenAnalysis_h

#include <solution/analysis/analysis/Analysis.h>

namespace XC {
  class Vector;
  class Matrix;

//! @ingroup AnalysisType
//
//! @brief Eigenproblem analysis.
class EigenAnalysis : public Analysis
  {
  protected:
    int domainStamp;

    friend class ProcSolu;
    EigenAnalysis(AnalysisAggregation *analysis_aggregation);
    Analysis *getCopy(void) const;
  public:     
    virtual int analyze(int numModes);
    void clearAll(void);
    virtual int domainChanged(void);
     
    virtual int setAlgorithm(EigenAlgorithm &theAlgo);
    virtual int setIntegrator(EigenIntegrator &theIntegrator);
    virtual int setEigenSOE(EigenSOE &theSOE);  

    //Autovectores.
    virtual const Vector &getEigenvector(int mode) const;
    Vector getNormalizedEigenvector(int mode) const;
    Matrix getEigenvectors(void) const;
    Matrix getNormalizedEigenvectors(void) const;

    //Eigenvalues.
    virtual const double &getEigenvalue(int mode) const;
    double getAngularFrequency(int mode) const;
    double getPeriodo(int mode) const;
    double getFrecuencia(int mode) const;
    Vector getEigenvalues(void) const;
    Vector getAngularFrequencies(void) const;
    Vector getPeriodos(void) const;
    Vector getFrecuencias(void) const;
    int getNumModes(void) const;

    //Modal participation factors.
    virtual double getModalParticipationFactor(int mode) const;
    Vector getModalParticipationFactors(void) const;

    //Distribution factors.
    Vector getDistributionFactor(int mode) const;
    Matrix getDistributionFactors(void) const;

    //Effective modal masses.
    double getEffectiveModalMass(int mode) const;
    Vector getEffectiveModalMasses(void) const;
    double getTotalMass(void) const;

    //Equivalent static load.
    Vector getEquivalentStaticLoad(int mode,const double &) const;
  };

} // end of XC namespace

#endif

