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
//MRMFreedom_FE.h
                                                                        
                                                                        
#ifndef MRMFreedom_FE_h
#define MRMFreedom_FE_h

#include "MPBase_FE.h"
#include <vector>

namespace XC {
class Node;
class MRMFreedom_Constraint;

//! @ingroup AnalysisFE
//
//! @brief ??
class MRMFreedom_FE: public MPBase_FE
  {
  protected:
    MRMFreedom_Constraint *theMRMP;
    std::vector<Node *> theRetainedNodes;    

    MRMFreedom_FE(int tag, int numDOF_Group, int ndof, MRMFreedom_Constraint &,const double &alpha= 1.0);
    int determineConstrainedDOFsIDs(const int &);
    int determineRetainedDOFsIDs(const int &);
    int determineRetainedNodesDofGrpPtr(Domain &,const int &);
  };
} // end of XC namespace

#endif


