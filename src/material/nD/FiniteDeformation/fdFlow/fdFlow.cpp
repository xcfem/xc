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
//===============================================================================
//# COPYRIGHT (C): Woody's license (by BJ):
//                 ``This    source  code is Copyrighted in
//                 U.S.,  for  an  indefinite  period,  and anybody
//                 caught  using it without our permission, will be
//                 mighty good friends of ourn, cause we don't give
//                 a  darn.  Hack it. Compile it. Debug it. Run it.
//                 Yodel  it.  Enjoy it. We wrote it, that's all we
//                 wanted to do.''
//
//# PROJECT:           Object Oriented Finite XC::Element Program
//# PURPOSE:           Finite Deformation Hyper-Elastic classes
//# CLASS:
//#
//# VERSION:           0.6_(1803398874989) (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhao Cheng, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhao Cheng, Boris Jeremic
//#
//#
//# DATE:              July 2004
//# UPDATE HISTORY:
//#
//===============================================================================

#ifndef fdFlow_CPP
#define fdFlow_CPP

#include "material/nD/FiniteDeformation/fdFlow/fdFlow.h"
#include <utility/matrix/nDarray/stresst.h>

XC::fdFlow::fdFlow()
{ 

}

double XC::fdFlow::dFodq(const XC::stresstensor &sts,  const XC::FDEPState &fdepstate ) const
{ 
     return 0.0;
}

XC::stresstensor XC::fdFlow::dFoda(const XC::stresstensor &sts,  const XC::FDEPState &fdepstate ) const
{ 
     XC::stresstensor Z2;
     return Z2;
}

XC::BJtensor XC::fdFlow::d2Fodsds(const XC::stresstensor &sts,  const XC::FDEPState &fdepstate ) const
{    
     BJtensor Z4(4, def_dim_4, 0.0);
     return Z4;
}

XC::stresstensor XC::fdFlow::d2Fodsdq(const XC::stresstensor &sts,  const XC::FDEPState &fdepstate ) const
{ 
     XC::stresstensor Z2;
     return Z2;
}

XC::BJtensor XC::fdFlow::d2Fodsda(const XC::stresstensor &sts,  const XC::FDEPState &fdepstate ) const
{    
     BJtensor Z4(4, def_dim_4, 0.0);
     return Z4;
}

double XC::fdFlow::d2Fodqdq(const XC::stresstensor &sts,  const XC::FDEPState &fdepstate ) const
{ 
     return 0.0;
}

XC::stresstensor XC::fdFlow::d2Fodqda(const XC::stresstensor &sts,  const XC::FDEPState &fdepstate ) const
{ 
     XC::stresstensor Z2;
     return Z2;
}

XC::BJtensor XC::fdFlow::d2Fodada(const XC::stresstensor &sts,  const XC::FDEPState &fdepstate ) const
{    
     BJtensor Z4(4, def_dim_4, 0.0);
     return Z4;
}

std::ostream& operator<<(std::ostream &os, const XC::fdFlow &fdfl)
{
     os << "fdFlow Parameters: " << "\n";
     return os;
}


#endif

