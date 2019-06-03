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
                                                                        
#ifndef KEigenAlgo_h
#define KEigenAlgo_h

#include <solution/analysis/algorithm/eigenAlgo/EigenAlgorithm.h>

namespace XC {
class KEigenIntegrator;

//! @ingroup EigenAlgo
//
//! @brief Algorithm for ill-conditioning analysis.
class KEigenAlgo : public EigenAlgorithm
  {
  protected:
    KEigenIntegrator *getKEigenIntegrator(void);
    virtual void eigen_to_model(int numModes);

    friend class AnalysisAggregation;
    KEigenAlgo(AnalysisAggregation *);
    virtual SolutionAlgorithm *getCopy(void) const;
  public:
    virtual int solveCurrentStep(int numModes);
    virtual void Print(std::ostream &s, int flag = 0);
    virtual int sendSelf(CommParameters &);
    virtual int recvSelf(const CommParameters &);
  };
inline SolutionAlgorithm *KEigenAlgo::getCopy(void) const
  { return new KEigenAlgo(*this); }
} // end of XC namespace

#endif


