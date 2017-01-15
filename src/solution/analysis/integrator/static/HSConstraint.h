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
//# PROJECT:           Object Oriented Finite Element Program
//# PURPOSE:           Hyper-spherical Constraint 
//# CLASS:             HSConstraint
//#
//# VERSION:           0.61803398874989 (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Ritu Jain, Boris Jeremic
//# PROGRAMMER(S):     Ritu, Boris Jeremic
//#
//#
//# DATE:              14Mar2003
//# UPDATE HISTORY:   
//#
//#
//===============================================================================
#ifndef HSConstraint_h
#define HSConstraint_h

#include "solution/analysis/integrator/static/ProtoArcLength.h"
#include "utility/matrix/Matrix.h"

namespace XC {
class LinearSOE;
class AnalysisModel;
class FE_Element;
class Vector;
class Matrix;

//! @ingroup StaticIntegrator
//
//! @brief ??.
class HSConstraint: public ProtoArcLength
  {
  private:
    double psi_u2;
    double psi_f2;
    double u_ref2;
    Matrix scalingMatrix;
  protected:
    double getDLambdaNewStep(void) const;
    double getDLambdaUpdate(void) const;
    int sendData(CommParameters &);
    int recvData(const CommParameters &);

    friend class SoluMethod;
    HSConstraint(SoluMethod *,double arcLength, double psi_u=1.0, double psi_f=1.0, double u_ref=1.0);
    Integrator *getCopy(void) const;
  public:

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag =0);    
  };
inline Integrator *HSConstraint::getCopy(void) const
  { return new HSConstraint(*this); }
} // end of XC namespace

#endif

