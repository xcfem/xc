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
//BFBRoydenBase.h
                                                                        
#ifndef BFBRoydenBase_h
#define BFBRoydenBase_h

#include "EquiSolnConvAlgo.h"
#include "utility/matrix/Vector.h"
#include <solution/analysis/integrator/IncrementalIntegrator.h>

namespace XC {

//! @ingroup EQSolAlgo
//
//! @brief Base class for Broyden's method solution algorithms.
//!
//! In numerical analysis, Broyden's method is a quasi-Newton method for
//! finding roots in k variables. It was originally described by C. G.
//! Broyden in 1965.
//! Newton's method for solving \f$f(x) = 0\f$ uses the Jacobian matrix, J, at
//! every iteration. However, computing this Jacobian is a difficult and
//! expensive operation. The idea behind Broyden's method is to compute
//! the whole Jacobian only at the first iteration and to do rank-one
//! updates at other iterations. 
class BFBRoydenBase: public EquiSolnConvAlgo
  {
    void free_mem(void);
    void alloc(const ConvergenceTest *);
  protected:
    ConvergenceTest *localTest;
    int tangent;
    int numberLoops; //!< number of iterations
    std::vector<Vector> s; //!< displacement increments
    std::vector<Vector> z; //!< displacement increments
    Vector residOld; //!< residuals
    Vector residNew; //!< residuals
    Vector du; //!< displacement increment
    Vector temp; //!< temporary vector

    BFBRoydenBase(AnalysisAggregation *,int classTag,int tangent = CURRENT_TANGENT, int n = 10);    
    BFBRoydenBase(AnalysisAggregation *,int classTag,ConvergenceTest &theTest, int tangent= CURRENT_TANGENT, int n=10);
  public:
    ~BFBRoydenBase(void);

    int setConvergenceTest(ConvergenceTest *theNewTest);
  };
} // end of XC namespace

#endif


