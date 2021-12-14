// -*-c++-*-
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
//IllConditioningAnalysis.h


#ifndef IllConditioningAnalysis_h
#define IllConditioningAnalysis_h

#include <solution/analysis/analysis/EigenAnalysis.h>

namespace XC {
  class Vector;
  class KEigenAlgo;
  class EigenSOE;
  class KEigenIntegrator;

//! @ingroup AnalysisType
//
//! @brief Linear buckling analysis (used inside an StaticAnalysis).
class IllConditioningAnalysis: public EigenAnalysis
  {
  protected:
    friend class SolutionProcedure;
    friend class KEigenAnalysis;
    IllConditioningAnalysis(SolutionStrategy *analysis_aggregation);
    Analysis *getCopy(void) const;
  public:
     
    virtual int setAlgorithm(KEigenAlgo &theAlgo);
    virtual int setIntegrator(KEigenIntegrator &theIntegrator);
    virtual int setEigenSOE(EigenSOE &theSOE);
  };
inline Analysis *IllConditioningAnalysis::getCopy(void) const
  { return new IllConditioningAnalysis(*this); }
} // end of XC namespace

#endif

