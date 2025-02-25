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
// ArpackSOE.h

#ifndef ArpackSOE_h
#define ArpackSOE_h

#include <solution/system_of_eqn/eigenSOE/ArpackSOEBase.h>
#include "solution/system_of_eqn/linearSOE/DistributedLinSOE.h"

namespace XC {

//! @brief <a href="http://www.caam.rice.edu/software/ARPACK/" target="_new"> Arpack++</a> based system of equations.
//! @ingroup EigenSOE
class ArpackSOE: public ArpackSOEBase, DistributedLinSOE
  {
    std::vector<double> M;
    int Msize;
    bool mDiagonal;
    AnalysisModel *theModel;
  protected:
    LinearSOE *theSOE;

    ArpackSOE(SolutionStrategy *,double shift = 0.0);
  public:
    friend class ArpackSolver;
    int checkSameInt(int);
    
    int setLinearSOE(LinearSOE &);    
    int getNumEqn(void) const;

  };
} // end of XC namespace


#endif



