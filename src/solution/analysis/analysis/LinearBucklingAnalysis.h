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
//LinearBucklingAnalysis.h
                                                                        
                                                                        
#ifndef LinearBucklingAnalysis_h
#define LinearBucklingAnalysis_h


// Description: This file contains the interface for the LinearBucklingAnalysis
// class. LinearBucklingAnalysis is a subclass of StaticAnalysis, it is used to perform 
// a linear buckling analysis on the FE\_Model.

#include <solution/analysis/analysis/StaticAnalysis.h>
#include "LinearBucklingEigenAnalysis.h"


namespace XC {
class ConvergenceTest;
class EigenAnalysis;
class LinearBucklingAlgo;
class LinearBucklingIntegrator;
class Vector;
class ArpackSOE;
class ArpackSolver;

//! @ingroup AnalysisType
//
//! @brief Linear buckling analysis.
class LinearBucklingAnalysis: public StaticAnalysis
  {
  private:
    SoluMethod *eigen_solu; //!< Solutio strategy for the eigenvalue problem.
    LinearBucklingEigenAnalysis linearBucklingEigenAnalysis;
    int numModes;
    int linear_buckling_analysis_step; //!Step in which the linear buckling analysis is started.
  protected:
    friend class ProcSolu;
    LinearBucklingAnalysis(SoluMethod *metodo,SoluMethod *eigen_solu);
    Analysis *getCopy(void) const;
  public:
    void clearAll(void);	    
    
    int analyze(int numSteps);
    int domainChanged(void);

    int getNumModes(void) const
      { return numModes; }
    void setNumModes(const int &nm)
      { numModes= nm; }

    int setLinearBucklingAlgorithm(LinearBucklingAlgo &);
    int setLinearBucklingIntegrator(LinearBucklingIntegrator &);
    int setArpackSOE(ArpackSOE &theSOE);
    virtual const Vector &getEigenvector(int mode);
    virtual const double &getEigenvalue(int mode) const;
  };

//! @brief Virtual constructor.
inline Analysis *LinearBucklingAnalysis::getCopy(void) const
  { return new LinearBucklingAnalysis(*this); }
} // end of XC namespace

#endif
