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
//MFreedom_FE.h
                                                                        
                                                                        
#ifndef MFreedom_FE_h
#define MFreedom_FE_h

#include "MPBase_FE.h"

namespace XC {
class Node;
class MFreedom_Constraint;

//! @ingroup AnalysisFE
//
//! @brief ??
class MFreedom_FE: public MPBase_FE
  {
  protected:
    MFreedom_Constraint *theMFreedom;
    Node *theRetainedNode;    

    MFreedom_FE(int tag, int numDOF_Group, int ndof, MFreedom_Constraint &theMFreedom,const double &alpha= 1.0);
    int determineConstrainedDOFsIDs(const int &);
    int determineRetainedDOFsIDs(const int &);
    DOF_Group *determineRetainedNodeDofGrpPtr(void);
  };
} // end of XC namespace

#endif


