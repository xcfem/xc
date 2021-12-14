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
//# PURPOSE:           Elastic Cross Anisotropic Material implementation:
//# CLASS:             ElasticIsotropic3D
//#
//# VERSION:           0.61803398874989 (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhaohui Yang, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhaohui Yang, Yi Bian, Boris Jeremic
//#
//#
//# DATE:              10Oct2002
//# UPDATE HISTORY:    March 20, 2003 Re-activated Joey Yang
//#
//#
//===============================================================================

#ifndef ElasticCrossAnisotropic_h
#define ElasticCrossAnisotropic_h

#include <utility/matrix/Vector.h>
#include <material/nD/NDMaterial.h>
#include <utility/matrix/nDarray/Tensor.h>
#include <utility/matrix/nDarray/straint.h>
#include <utility/matrix/nDarray/stresst.h>

namespace XC {
//! @ingroup NDMat
//
//! @brief Base class for elastic anisotropic materials.
class ElasticCrossAnisotropic: public NDMaterial
  {
  private:
    static Vector sigma; //!< Stress vector ... class-wide for returns
    static Matrix D; //!< Elastic constants
    Vector Tepsilon; //!< Trial strain vector
    Vector Cepsilon; //!< Committed strains

    Tensor Dt; //!< Elastic constants tensor
//Tensor Dt_commit; //!< last-step Elastic constants tensor
    mutable stresstensor Stress; //!< Stress tensor
    mutable straintensor Strain; //!< Strain tensor

// all the directions are relative so we call them "horizontal" and "vertical", take that
// horizontal is one plane of anisotropy while vertical is the axes perpendicular to that plane.
    double Eh; //!< Eh: Young's modulus in any horizontal direction.
    double Ev; //!< Ev: Young's modulus in a vertical direction.
    double nuhv; //!< nuhv: Poisson's ratio for strain in the vertical direction due to a horizontal direct stress.
    double nuhh; //!< nvhh: Poisoon's ratio for strain in any horizontal direction due to a horizontal direct stress at right angles.
    double Ghv; //!< Ghv: Modulus of shear deformation in a vertical plane.
    double rho; //!< Mass density

    void setInitElasticStiffness(void);
    void convertD2TensorEijkl(void);
  protected:
    int sendData(Communicator &);
    int recvData(const Communicator &);
  public:
    ElasticCrossAnisotropic(int tag, double Ehp, double Evp, double nuhvp,double nuhhp, double Ghvp, double rhop = 0.0);
    ElasticCrossAnisotropic(int tag);
    ElasticCrossAnisotropic(void);

    double getrho();
    int setTrialStrain(const Vector &v);
    int setTrialStrain(const Vector &v, const Vector &r);
    int setTrialStrainIncr(const Vector &v);
    int setTrialStrainIncr(const Vector &v, const Vector &r);

    const Matrix &getTangent(void) const;
    const Vector &getStress(void) const;
    const Vector &getStrain(void) const;

    int setTrialStrain(const Tensor &v);
    int setTrialStrain(const Tensor &v, const Tensor &r);
    int setTrialStrainIncr(const Tensor &v);
    int setTrialStrainIncr(const Tensor &v, const Tensor &r);

    const Tensor &getTangentTensor(void) const;
    const stresstensor &getStressTensor(void) const;
    const straintensor &getStrainTensor(void) const;
    const straintensor &getPlasticStrainTensor(void) const;

    int commitState (void);
    int revertToLastCommit (void);
    int revertToStart (void);

    NDMaterial *getCopy(void) const;
    NDMaterial *getCopy(const std::string &) const;
    const std::string &getType(void) const;
    int getOrder(void) const;

    void Print(std::ostream &s, int flag = 0) const;

//int setParameter(const std::vector<std::string> &argv, Parameter &param);
//int updateParameter (int parameterID, Information &info);

    int sendSelf(Communicator &);
    int recvSelf(const Communicator &);
  };
} // end of XC namespace

#endif

