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
#ifndef FiniteDeformationEP3D_H
#define FiniteDefornationEP3D_H

#include <material/nD/NDMaterial.h>

namespace XC {
  class FDEPState;
  class fdYield;
  class fdFlow;
  class fdEvolution_S;
  class fdEvolution_T;
//! @ingroup NDMat
//
//! @defgroup FDNDMat Finite deformation materials.
//
//! @ingroup FDNDMat
//
//! @brief Finite deformation elasto-plastic 3D material.
class FiniteDeformationEP3D: public NDMaterial
  {

  private:
    NDMaterial *fde3d;
    fdYield *fdy;
    fdFlow *fdf;
    fdEvolution_S *fdEvolutionS;
    fdEvolution_T *fdEvolutionT;

    //material input
    straintensor F;

    //material response
    straintensor iniGreen;
    stresstensor iniPK2;
    BJtensor iniTangent;
  
    stresstensor B_PK2;
    straintensor Fe;
    stresstensor cauchystress;
  
    FDEPState *fdeps;
  private:

    NDMaterial *getFDE3D() const;
    fdYield *getFDY() const;
    fdFlow *getFDF() const;
    fdEvolution_S *getFDEvolutionS() const;
    fdEvolution_T *getFDEvolutionT() const;  
    FDEPState *getFDEPState() const; 

    int ImplicitAlgorithm();
    int SemiImplicitAlgorithm();
  public:
    FiniteDeformationEP3D(int tag);
    // Constructor 00
    FiniteDeformationEP3D( );
    // Constructor 01
    FiniteDeformationEP3D(int tag,
                          NDMaterial *fde3d_in,
			fdYield *fdy_in,
			fdFlow *fdf_in,
			fdEvolution_S *fdEvolutionS_in,
			fdEvolution_T *fdEvolutionT_in);
    // Constructor 02
    FiniteDeformationEP3D(int tag,
                          NDMaterial *fde3d_in,
			fdYield *fdy_in,
			fdFlow *fdf_in,
			fdEvolution_S *fdEvolutionS_in);
    // Constructor 03
    FiniteDeformationEP3D(int tag,
                          NDMaterial *fde3d_in,
			fdYield *fdy_in,
			fdFlow *fdf_in,
			fdEvolution_T *fdEvolutionT_in);
    // Constructor 04
    FiniteDeformationEP3D(int tag,
                          NDMaterial *fde3d_in,
			fdYield *fdy_in,
			fdFlow *fdf_in);
    // Destructor
    virtual ~FiniteDeformationEP3D( );
    
    double getRho(void) const;  

    int setTrialF(const straintensor &f);
    int setTrialFIncr(const straintensor &df);

    const Tensor &getTangentTensor(void) const;
    const straintensor &getStrainTensor(void) const;  // Default Green Strain
    const stresstensor &getStressTensor(void) const ;  // Default 2nd Piola Kirchhoff Stress
    const straintensor &getF(void) const;
    const straintensor getFp(void);

    int commitState(void) ;
    int revertToLastCommit(void) ;
    int revertToStart(void) ;

    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;

    const std::string &getType(void) const;
    int getOrder (void) const;

    int sendSelf(CommParameters &);
    int recvSelf(const CommParameters &);

    void Print(std::ostream &s, int flag);

    const  stresstensor getCauchyStressTensor(void);
  };
} // end of XC namespace

#endif
