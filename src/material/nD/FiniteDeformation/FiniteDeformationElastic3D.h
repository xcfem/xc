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
//# DATE:              19AUg2003
//# UPDATE HISTORY:    Sept 2003
//#		       May28, 2004
//#		       
//===============================================================================

#ifndef FiniteDeformationElastic3D_h
#define FiniteDeformationElastic3D_h

#include "FiniteDeformationMaterial.h"

namespace XC {
//! @ingroup FDNDMat
//
//! @brief Base class for finite deformation elastic materials.
class FiniteDeformationElastic3D: public FiniteDeformationMaterial
  {
  protected:
    double rho; //!< Material density.
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    
    FiniteDeformationElastic3D(int tag, int classTag, double );
    FiniteDeformationElastic3D(int tag);      
    FiniteDeformationElastic3D();      
    
    virtual double getRho(void) const;

    virtual int setTrialF(const straintensor &f);
    virtual int setTrialFIncr(const straintensor &df);
    virtual int setTrialC(const straintensor &c);
    virtual int setTrialCIncr(const straintensor &dc);

    virtual const Tensor& getTangentTensor(void) const;	  // Default Lagrangian Tangent Tensor
    virtual const Tensor& getInitialTangentTensor(void) const;

    virtual const straintensor &getStrainTensor(void) const;   // Default Green Lagrangian Strain
    virtual const stresstensor &getStressTensor(void) const;   // Default 2nd Piola Kirchhoff Stress
    virtual const straintensor &getF(void) const;
    virtual const straintensor &getC(void) const;

    virtual int commitState(void);
    virtual int revertToLastCommit(void);
    virtual int revertToStart(void);

    virtual NDMaterial *getCopy(void) const;
    virtual NDMaterial *getCopy(const std::string &) const;

    virtual const std::string &getType(void) const;
    virtual int getOrder (void) const;

    virtual void Print(std::ostream &s, int flag = 0) const;

    virtual int setParameter(const std::vector<std::string> &argv, Parameter &param);
    virtual int updateParameter(int parameterID, Information &info);

    const  stresstensor getPK1StressTensor(void) const;
    const  stresstensor getCauchyStressTensor(void) const;
  };
} // end of XC namespace

#endif

