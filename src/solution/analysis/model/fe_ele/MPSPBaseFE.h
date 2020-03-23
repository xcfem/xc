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
//MPSPBaseFE.h
                                                                        
                                                                        
#ifndef MPSPBaseFE_h
#define MPSPBaseFE_h

#include <solution/analysis/model/fe_ele/FE_Element.h>
#include "utility/matrix/Matrix.h"
#include "utility/matrix/Vector.h"

namespace XC {

//! @ingroup AnalysisFE
//
//! @brief Base class for MFreedom_FE y SFreedom_FE.
class MPSPBaseFE: public FE_Element
  {
  protected:
    double alpha;
    Matrix tang;
    Vector resid;

    MPSPBaseFE(int tag, int numDOF_Group, int ndof,const double &alpha= 1.0);
  };
} // end of XC namespace

#endif


