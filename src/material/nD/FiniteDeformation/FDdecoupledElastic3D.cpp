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

#include <material/nD/FiniteDeformation/FDdecoupledElastic3D.h>
#include <utility/matrix/Vector.h>
#include "material/nD/FiniteDeformation/WEnergy/WEnergy.h"
#include "material/nD/TipoMaterialND.h"


void XC::FDdecoupledElastic3D::libera(void)
  {
    if(W) delete W;
    W= nullptr;
  }

void XC::FDdecoupledElastic3D::alloc(const WEnergy *w)
  {
    if(w)
      {
        libera();
        W= w->getCopy();
      }
    else
      {
        std::cerr << "FDdecoupledElastic3D::alloc pasó a null pointer.\n";
        exit(-1);
      }
  }

//! @brief Constructor.
XC::FDdecoupledElastic3D::FDdecoupledElastic3D(int tag,int classTag,WEnergy *wEnergy_in,double rho_in= 0.0)
:FiniteDeformationElastic3D(tag, classTag, rho_in), W(nullptr)
  { alloc(wEnergy_in); }

//! @brief Constructor.
XC::FDdecoupledElastic3D::FDdecoupledElastic3D(int tag,WEnergy *wEnergy_in,double rho_in = 0.0)
:FiniteDeformationElastic3D(tag, ND_TAG_FDdecoupledElastic3D, rho_in), W(nullptr)
  { alloc(wEnergy_in); }


//! @brief Constructor.
XC::FDdecoupledElastic3D::FDdecoupledElastic3D(int tag,WEnergy *wEnergy_in)
  :FiniteDeformationElastic3D(tag, ND_TAG_FDdecoupledElastic3D, 0.0), W(nullptr)
  { alloc(wEnergy_in); }

XC::FDdecoupledElastic3D::FDdecoupledElastic3D(int tag)
  :FiniteDeformationElastic3D(tag, ND_TAG_FDdecoupledElastic3D, 0.0), W(nullptr)
  {}

//! @brief Constructor de copia.
XC::FDdecoupledElastic3D::FDdecoupledElastic3D(const FDdecoupledElastic3D &fde3d)
  :FiniteDeformationElastic3D(fde3d), W(nullptr)
  {
    alloc(fde3d.W);
  }

//! @brief 
XC::FDdecoupledElastic3D::~FDdecoupledElastic3D(void)
  { libera(); }

//! @brief 
double XC::FDdecoupledElastic3D::getRho(void) const
  { return rho; }
//! @brief 
const XC::WEnergy *XC::FDdecoupledElastic3D::getWEnergy(void) const
  { return W; }
//! @brief 
int XC::FDdecoupledElastic3D::setTrialF(const XC::straintensor &f)
{
   FromForC = 0;
   F = f;
   C = F("ki")*F("kj");   C.null_indices();
   return this->ComputeTrials();
}
//! @brief 
int XC::FDdecoupledElastic3D::setTrialFIncr(const XC::straintensor &df)
  { return this->setTrialF(this->getF() + df); }
//! @brief 
int XC::FDdecoupledElastic3D::setTrialC(const XC::straintensor &c)
  {
    FromForC = 1;
    C = c;
    return this->ComputeTrials();
  }
//! @brief 
int XC::FDdecoupledElastic3D::setTrialCIncr(const XC::straintensor &dc)
  { return this->setTrialC(this->getC() + dc); }
//! @brief 
const XC::straintensor &XC::FDdecoupledElastic3D::getF(void) const
  { return F; }
//! @brief 
const XC::straintensor &XC::FDdecoupledElastic3D::getC(void) const
  { return C; }
//! @brief 
const double XC::FDdecoupledElastic3D::getJ(void) const
  { return J; }
//! @brief 
const XC::Vector XC::FDdecoupledElastic3D::getlambda(void) const
  {
    XC::Vector lambda(3);

    lambda(0) = lambda1;
    lambda(1) = lambda2;
    lambda(2) = lambda3;

    return lambda;
  }
//! @brief 
const XC::Vector XC::FDdecoupledElastic3D::getlambda_wave(void) const
{
  XC::Vector lambda_wave(3);
  lambda_wave(0) = lambda_wave1;
  lambda_wave(1) = lambda_wave2;
  lambda_wave(2) = lambda_wave3;

  return lambda_wave;
}
//! @brief 
const XC::Vector XC::FDdecoupledElastic3D::wa(void)
{
  XC::Vector Wa(3);
  XC::Vector lambda_wave(3);
  lambda_wave = this->getlambda_wave();
  XC::Vector disowOverlambda = W->disowOdlambda(lambda_wave);
  double temp = disowOverlambda(0) * lambda_wave(0) +
                disowOverlambda(1) * lambda_wave(1) +
                disowOverlambda(2) * lambda_wave(2) ;
  temp = temp * (-0.3333333333333333333333333333);
  Wa(0) = temp + disowOverlambda(0) * lambda_wave(0);
  Wa(1) = temp + disowOverlambda(1) * lambda_wave(1);
  Wa(2) = temp + disowOverlambda(2) * lambda_wave(2);
  return Wa;
}
//! @brief 
const XC::Tensor XC::FDdecoupledElastic3D::Yab(void)
  {
    Tensor Y(2, def_dim_2, 0.0);
    Tensor I_ij("I", 2, def_dim_2);
    Vector lambda_wave(3);
    lambda_wave = this->getlambda_wave();
    Tensor  d2 = W->d2isowOdlambda1dlambda2(lambda_wave);
    Vector  d1 = W->disowOdlambda(lambda_wave);
    Vector  d11 = W->d2isowOdlambda2(lambda_wave);
    d2.val(1,1) = d11(0);
    d2.val(2,2) = d11(1);
    d2.val(3,3) = d11(2);
    Vector tempi(3);
    double tempd = d1(0)*lambda_wave(0) + d1(1)*lambda_wave(1) + d1(2)*lambda_wave(2) ;
    double tempcd = 0.0;
    for(int i=0; i<3; i++)
      {
        tempi(i) = 0.0;
        for(int j=0; j<3; j++)
          {
            tempi(i) += d2.cval(i+1,j+1) * lambda_wave(i) * lambda_wave(j);
            tempcd   += d2.cval(i+1,j+1) * lambda_wave(i) * lambda_wave(j);
          }
      }
    for(int a=1; a<=3; a++)
      {
        for(int b=1; b<=3; b++)
          {
            Y.val(a,b) = d1(a-1)*I_ij.cval(a,b)*lambda_wave(b-1) + d2.cval(a,b)*lambda_wave(a-1)*lambda_wave(b-1) -
                          (  tempi(a-1) + tempi(b-1) + d1(a-1)*lambda_wave(a-1) + d1(b-1)*lambda_wave(b-1) ) / 3.0 +
                           ( tempcd + tempd ) / 9.0;
          }
      }
    return Y;
  }

//! @brief 
const XC::Tensor XC::FDdecoupledElastic3D::FDisoStiffness(void)
{
  Tensor I_ij("I", 2, def_dim_2);
  Tensor I_ijkl( 4, def_dim_4, 0.0 );
  I_ijkl = I_ij("ij") * I_ij("kl");
  I_ij.null_indices();
  Tensor I_ikjl( 4, def_dim_4, 0.0 );
  I_ikjl = I_ijkl.transpose0110();
  Tensor I_iljk( 4, def_dim_4, 0.0 );
  I_iljk = I_ijkl.transpose0111();
  Tensor I4s = (I_ikjl+I_iljk)*0.5;
  Tensor  tempI = I4s - I_ijkl;

  Tensor CinvCinv = Cinv("ij") * Cinv("kl");
  CinvCinv.null_indices(); Cinv.null_indices(); 

  Tensor ICinv = ( CinvCinv.transpose0110() + CinvCinv.transpose0111() ) * (0.5);

  Tensor CinvCinv_ICinv = CinvCinv - ICinv;

  double I1 = lambda1*lambda1 + lambda2*lambda2 + lambda3*lambda3;

  Vector Wa = this->wa();
  Tensor yab = this->Yab();

  Tensor L_iso(2,def_dim_2,0.0);

  if(caseIndex == 0)
    {
      double d1 = (lambda1+lambda2)*(lambda1+lambda3)*(lambda1-lambda2)*(lambda1-lambda3);
      double d2 = (lambda2+lambda3)*(lambda2+lambda1)*(lambda2-lambda3)*(lambda2-lambda1);
      double d3 = (lambda3+lambda1)*(lambda3+lambda2)*(lambda3-lambda1)*(lambda3-lambda2);

      Tensor M1 = ( C - I_ij*(I1-lambda1*lambda1) + Cinv*(J*J/(lambda1*lambda1)) ) * (1.0/d1);
      Tensor M2 = ( C - I_ij*(I1-lambda2*lambda2) + Cinv*(J*J/(lambda2*lambda2)) ) * (1.0/d2);
      Tensor M3 = ( C - I_ij*(I1-lambda3*lambda3) + Cinv*(J*J/(lambda3*lambda3)) ) * (1.0/d3);

    double d1p = 4.0 *lambda1*lambda1*lambda1*lambda1 - I1*lambda1*lambda1 - J*J /(lambda1*lambda1);
    double d2p = 4.0 *lambda2*lambda2*lambda2*lambda2 - I1*lambda2*lambda2 - J*J /(lambda2*lambda2);
    double d3p = 4.0 *lambda3*lambda3*lambda3*lambda3 - I1*lambda3*lambda3 - J*J /(lambda3*lambda3);

    XC::Tensor Cm1M1M1Cm1 = Cinv("ij")*M1("kl") + M1("ij")*Cinv("kl");
    Cinv.null_indices(); M1.null_indices(); Cm1M1M1Cm1.null_indices();

    XC::Tensor Cm1M2M2Cm1 = Cinv("ij")*M2("kl") + M2("ij")*Cinv("kl");
    Cinv.null_indices(); M2.null_indices(); Cm1M2M2Cm1.null_indices();

    XC::Tensor Cm1M3M3Cm1 = Cinv("ij")*M3("kl") + M3("ij")*Cinv("kl");
    Cinv.null_indices(); M3.null_indices(); Cm1M3M3Cm1.null_indices();


    XC::Tensor dM1M1d = I_ij("ij")*M1("kl") + M1("ij")*I_ij("kl");
    I_ij.null_indices(); M1.null_indices(); dM1M1d.null_indices();

    XC::Tensor dM2M2d = I_ij("ij")*M2("kl") + M2("ij")*I_ij("kl");
    I_ij.null_indices(); M2.null_indices(); dM2M2d.null_indices();

    XC::Tensor dM3M3d = I_ij("ij")*M3("kl") + M3("ij")*I_ij("kl");
    I_ij.null_indices(); M3.null_indices(); dM3M3d.null_indices();

    XC::Tensor M1M1 = M1("ij") * M1("kl");
    M1.null_indices(); M1M1.null_indices();
    XC::Tensor M2M2 = M2("ij") * M2("kl");
    M2.null_indices(); M2M2.null_indices();
    XC::Tensor M3M3 = M3("ij") * M3("kl");
    M3.null_indices(); M3M3.null_indices();

    XC::Tensor calM1 = ( tempI + (CinvCinv_ICinv -Cm1M1M1Cm1)*(J*J/(lambda1*lambda1)) + dM1M1d*(lambda1*lambda1) - M1M1*d1p ) *(1.0/d1);
    XC::Tensor calM2 = ( tempI + (CinvCinv_ICinv -Cm1M2M2Cm1)*(J*J/(lambda2*lambda2)) + dM2M2d*(lambda2*lambda2) - M2M2*d2p ) *(1.0/d2);
    XC::Tensor calM3 = ( tempI + (CinvCinv_ICinv -Cm1M3M3Cm1)*(J*J/(lambda3*lambda3)) + dM3M3d*(lambda3*lambda3) - M3M3*d3p ) *(1.0/d3);

    XC::Tensor L_iso_1 = ( calM1*Wa(0) + calM2*Wa(1) + calM3*Wa(2) ) * 2.0;
    XC::Tensor L_iso_2 =  M1("ij") * M1("kl") * yab.cval(1,1)  + M1("ij") * M2("kl") * yab.cval(1,2)  + M1("ij") * M3("kl") * yab.cval(1,3)  +
                      M2("ij") * M1("kl") * yab.cval(2,1)  + M2("ij") * M2("kl") * yab.cval(2,2)  + M2("ij") * M3("kl") * yab.cval(2,3)  +
                      M3("ij") * M1("kl") * yab.cval(3,1)  + M3("ij") * M2("kl") * yab.cval(3,2)  + M3("ij") * M3("kl") * yab.cval(3,3);
    L_iso = L_iso_1 + L_iso_2 ;
  }

  if(caseIndex == 11)
  {
    double d1 = (lambda1+lambda2)*(lambda1+lambda3)*(lambda1-lambda2)*(lambda1-lambda3);
    XC::Tensor M1 = (I_ij - Cinv * (lambda2*lambda2)) * (1.0/(lambda1+lambda2)/(lambda1-lambda2));
    XC::Tensor Mr = Cinv - M1;
    double d1p = 4.0 *lambda1*lambda1*lambda1*lambda1 - I1*lambda1*lambda1 - J*J /(lambda1*lambda1);
    XC::Tensor Cm1M1M1Cm1 = Cinv("ij")*M1("kl") + M1("ij")*Cinv("kl");
    Cinv.null_indices(); M1.null_indices(); Cm1M1M1Cm1.null_indices();
    XC::Tensor dM1M1d = I_ij("ij")*M1("kl") + M1("ij")*I_ij("kl");
    I_ij.null_indices(); M1.null_indices(); dM1M1d.null_indices();
    XC::Tensor M1M1 = M1("ij") * M1("kl");
    M1.null_indices(); M1M1.null_indices();
    XC::Tensor calM1 = ( tempI + (CinvCinv_ICinv -Cm1M1M1Cm1)*(J*J/(lambda1*lambda1)) + dM1M1d*(lambda1*lambda1) - M1M1*d1p ) *(1.0/d1);
    XC::Tensor calMr = (ICinv + calM1) * (-1.0);
    XC::Tensor L_iso_1 = ( calM1*Wa(0) + calMr*Wa(2) ) * 2.0;
    XC::Tensor L_iso_2 =  M1("ij") * M1("kl") * yab.cval(1,1)  + M1("ij") * Mr("kl") * yab.cval(1,3)  +
                      Mr("ij") * M1("kl") * yab.cval(3,1)  + Mr("ij") * Mr("kl") * yab.cval(3,3);
    L_iso = L_iso_1 + L_iso_2 ;
  }

  if(caseIndex == 13)
  {
    double d3 = (lambda3+lambda1)*(lambda3+lambda2)*(lambda3-lambda1)*(lambda3-lambda2);
    XC::Tensor M3 = (I_ij - Cinv * (lambda2*lambda2)) * (1.0/(lambda3+lambda2)/(lambda3-lambda2));
    XC::Tensor Mr = Cinv - M3;
    double d3p = 4.0 *lambda3*lambda3*lambda3*lambda3 - I1*lambda3*lambda3 - J*J /(lambda3*lambda3);
    XC::Tensor Cm1M3M3Cm1 = Cinv("ij")*M3("kl") + M3("ij")*Cinv("kl");
    Cinv.null_indices(); M3.null_indices(); Cm1M3M3Cm1.null_indices();
    XC::Tensor dM3M3d = I_ij("ij")*M3("kl") + M3("ij")*I_ij("kl");
    I_ij.null_indices(); M3.null_indices(); dM3M3d.null_indices();
    XC::Tensor M3M3 = M3("ij") * M3("kl");
    M3.null_indices(); M3M3.null_indices();
    XC::Tensor calM3 = ( tempI + (CinvCinv_ICinv -Cm1M3M3Cm1)*(J*J/(lambda3*lambda3)) + dM3M3d*(lambda3*lambda3) - M3M3*d3p ) *(1.0/d3);
    XC::Tensor calMr = (ICinv + calM3) * (-1.0);
    XC::Tensor L_iso_1 = ( calM3*Wa(2) + calMr*Wa(0) ) * 2.0;
    XC::Tensor L_iso_2 =  M3("ij") * M3("kl") * yab.cval(3,3)  + M3("ij") * Mr("kl") * yab.cval(3,1)  +
                      Mr("ij") * M3("kl") * yab.cval(1,3)  + Mr("ij") * Mr("kl") * yab.cval(1,1);
    L_iso = L_iso_1 + L_iso_2 ;
  }

    if(caseIndex == 2)
  {
    XC::Vector lambda_wave(3);
    lambda_wave = this->getlambda_wave();
    XC::Vector  d11 = W->d2isowOdlambda2(lambda_wave);
    XC::Vector  d1 = W->disowOdlambda(lambda_wave);
    double G2linear = d11(1)*lambda_wave2*lambda_wave2 + d1(1)*lambda_wave2;

    L_iso = ( ICinv - CinvCinv * (1.0/3.0) ) * G2linear;
  }

    return L_iso;
}
//! @brief 
const XC::Tensor XC::FDdecoupledElastic3D::FDvolStiffness(void)
{
   XC::Tensor CinvCinv = Cinv("ij")*Cinv("kl") ;
   Cinv.null_indices(); CinvCinv.null_indices();
   XC::Tensor ICinv = ( CinvCinv.transpose0110() + CinvCinv.transpose0111() ) * (0.5);
   double dWdJ = W->dvolwOdJ(J);
   double d2WdJ2 = W->d2volwOdJ2(J);
   double wj = d2WdJ2*J*J + J*dWdJ;

   XC::Tensor L_vol = CinvCinv*wj - ICinv *2.0*J*dWdJ  ;

   return L_vol;
}
//! @brief 
const XC::Tensor& XC::FDdecoupledElastic3D::getTangentTensor(void) const
  { return Stiffness; }
//! @brief 
const XC::Tensor &XC::FDdecoupledElastic3D::getInitialTangentTensor(void) const
  {
    //BJtensor I2("I", 2, def_dim_2);
    //BJtensor I_ijkl = I2("ij")*I2("kl");
    //BJtensor I_ikjl = I_ijkl.transpose0110();
    //BJtensor I_iljk = I_ijkl.transpose0111();
    //BJtensor I4s = (I_ikjl+I_iljk)*0.5;
    //static XC::BJtensor L0;
    //L0 = I_ijkl*( E*nu / ( (1.0+nu)*(1.0 - 2.0*nu) ) ) + I4s*( E / (1.0 + nu) );

    //return L0;
    return this->getTangentTensor();
  }
//! @brief 
const XC::straintensor &XC::FDdecoupledElastic3D::getStrainTensor(void) const
  { return thisGreenStrain; }
//! @brief 
const XC::stresstensor &XC::FDdecoupledElastic3D::getStressTensor(void) const
  { return thisPK2Stress; }
//! @brief 
const XC::stresstensor XC::FDdecoupledElastic3D::getPK1StressTensor(void)
{
   XC::stresstensor thisSPKStress;
   XC::stresstensor thisFPKStress;

   if( FromForC == 0 ) {
    thisSPKStress = this->getStressTensor();
    thisFPKStress = thisSPKStress("ij") * (F.transpose11())("jk") ;
   }

   if( FromForC == 1 ) {
    std::cerr << "FDdecoupledElastic3D: unknown deformation gradient - cannot compute PK1 stress" << "\n";
    exit(-1);
   }

    return thisFPKStress;
}
//! @brief 
const XC::stresstensor XC::FDdecoupledElastic3D::getCauchyStressTensor(void)
{
   XC::stresstensor thisSPKStress;
   XC::stresstensor thisCauchyStress;

   if( FromForC == 0 ) {
    thisSPKStress = this->getStressTensor();
    thisCauchyStress = F("ij") * thisSPKStress("jk") * (F.transpose11())("kl") * (1.0/J);
   }

   if( FromForC == 1 ) {
    std::cerr << "FDdecoupledElastic3D: unknown deformation gradient - cannot compute Cauchy stress" << "\n";
    exit(-1);
   }

    return thisCauchyStress;
}
//! @brief 
int XC::FDdecoupledElastic3D::commitState(void)
  { return 0; }
//! @brief 
int XC::FDdecoupledElastic3D::revertToLastCommit(void)
  { return 0; }
//! @brief 
int XC::FDdecoupledElastic3D::revertToStart(void)
{
   XC::Tensor F0("I", 2, def_dim_2);
   F = F0;
   C = F0;
   Cinv = F0;

   XC::Tensor ss_zero(2,def_dim_2,0.0);
   thisPK2Stress = ss_zero;
   thisGreenStrain = ss_zero;
   Stiffness = getInitialTangentTensor();

   J = 1.0;
   lambda1 = 1.0;
   lambda2 = 1.0;
   lambda3 = 1.0;
   lambda_wave1 = 1.0;
   lambda_wave2 = 1.0;
   lambda_wave3 = 1.0;

   caseIndex = 0;

   return 0;
}
//! @brief 
XC::NDMaterial *XC::FDdecoupledElastic3D::getCopy(void) const
  { return new FDdecoupledElastic3D(*this); }

//! @brief 
XC::NDMaterial *XC::FDdecoupledElastic3D::getCopy(const std::string &type) const
  {

    std::cerr << "FDdecoupledElastic3D::getCopy(const std::string &) - not yet implemented\n";
    return 0;
  }
//! @brief 
const std::string &XC::FDdecoupledElastic3D::getType(void) const
  { return strTipoThreeDimensionalFD; }
//! @brief 
int XC::FDdecoupledElastic3D::getOrder(void) const
  { return 6; }

//! @brief Send object members through the channel being passed as parameter.
int XC::FDdecoupledElastic3D::sendData(CommParameters &cp)
  {
    int res= FiniteDeformationElastic3D::sendData(cp);
    //XXX Falta enviar pointer to WEnergy (W). Position 2 reserved;
    res+= cp.sendTensor(F,getDbTagData(),CommMetaData(3));
    res+= cp.sendTensor(C,getDbTagData(),CommMetaData(4));
    res+= cp.sendTensor(Cinv,getDbTagData(),CommMetaData(5));
    res+= cp.sendDoubles(J,lambda1,lambda2,lambda3,getDbTagData(),CommMetaData(6));
    res+= cp.sendDoubles(lambda_wave1, lambda_wave2, lambda_wave3,getDbTagData(),CommMetaData(7));
    res+= cp.sendInts(caseIndex,FromForC,getDbTagData(),CommMetaData(8));
    res+= cp.sendTensor(Stiffness,getDbTagData(),CommMetaData(9));
    res+= cp.sendTensor(thisGreenStrain,getDbTagData(),CommMetaData(10));
    res+= cp.sendTensor(thisPK2Stress,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::FDdecoupledElastic3D::recvData(const CommParameters &cp)
  {
    int res= FiniteDeformationElastic3D::recvData(cp);
    //XXX Falta recibir pointer to WEnergy (W). Position 2 reserved;
    res+= cp.receiveTensor(F,getDbTagData(),CommMetaData(3));
    res+= cp.receiveTensor(C,getDbTagData(),CommMetaData(4));
    res+= cp.receiveTensor(Cinv,getDbTagData(),CommMetaData(5));
    res+= cp.receiveDoubles(J,lambda1,lambda2,lambda3,getDbTagData(),CommMetaData(6));
    res+= cp.receiveDoubles(lambda_wave1, lambda_wave2, lambda_wave3,getDbTagData(),CommMetaData(7));
    res+= cp.receiveInts(caseIndex,FromForC,getDbTagData(),CommMetaData(8));
    res+= cp.receiveTensor(Stiffness,getDbTagData(),CommMetaData(9));
    res+= cp.receiveTensor(thisGreenStrain,getDbTagData(),CommMetaData(10));
    res+= cp.receiveTensor(thisPK2Stress,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::FDdecoupledElastic3D::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::FDdecoupledElastic3D::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief 
void XC::FDdecoupledElastic3D::Print(std::ostream &s, int flag)
  {
    s << "Finite Deformation Elastic 3D model" << std::endl;
    s << "\trho: " << rho << std::endl;
    return;
  }
//! @brief 
//int XC::FDdecoupledElastic3D::setParameter(const std::vector<std::string> &argv, Parameter &param)
//{
//   return -1;
//}
//! @brief 
//int XC::FDdecoupledElastic3D::updateParameter(int parameterID, Information &info)
//{
//   return -1;
//}
//! @brief 
int XC::FDdecoupledElastic3D::ComputeTrials()
{
   // Cinv:
   Cinv = C.inverse();
   Cinv.symmetrize11();

   // J:
   J = sqrt(C.determinant());

   // lambda:
   BJtensor eigtensor(C.eigenvalues());
   lambda1 = sqrt(eigtensor.cval(1));
   lambda2 = sqrt(eigtensor.cval(2));
   lambda3 = sqrt(eigtensor.cval(3));

   // lambda_wave
   double JJJ = pow(J, -0.33333333333333333333333333333);
   lambda_wave1 = lambda1 *JJJ;
   lambda_wave2 = lambda2 *JJJ;
   lambda_wave3 = lambda3 *JJJ;

   // caseIndex, note lambda1 >= lambda2 >= lambda3 implied by C.eigenvalues()
   double diff12 = fabs(lambda1-lambda2);
   double diff23 = fabs(lambda2-lambda3);
   double perturbation = pow( d_macheps(), (0.4) );
   if( diff12 >= perturbation && diff23 >= perturbation )
    caseIndex = 0;
   else if(diff12 >= perturbation && diff23 < perturbation )
    caseIndex = 11;
   else if(diff12 < perturbation && diff23 >= perturbation )
    caseIndex = 13;
   else if(diff12 < perturbation &&  diff23 < perturbation )
    caseIndex = 2;
   else   {std::cerr << "FDdecoupledElastic3D::getCaseIndex -- unknown case! \n";
    exit(-1);}

   XC::Tensor I_ij("I", 2, def_dim_2);

   XC::Tensor isoPK2Stress(2, def_dim_2, 0.0);

   XC::Vector Wa = this->wa();

   double I1 = lambda1*lambda1+lambda2*lambda2+lambda3*lambda3;

   if(caseIndex == 0)
   {
     double d1 = (lambda1+lambda2)*(lambda1+lambda3)*(lambda1-lambda2)*(lambda1-lambda3);
     double d2 = (lambda2+lambda3)*(lambda2+lambda1)*(lambda2-lambda3)*(lambda2-lambda1);
     double d3 = (lambda3+lambda1)*(lambda3+lambda2)*(lambda3-lambda1)*(lambda3-lambda2);

     XC::Tensor M1 = ( C - I_ij*(I1-lambda1*lambda1) + Cinv *(J*J/(lambda1*lambda1)) ) * (1.0/d1);
     XC::Tensor M2 = ( C - I_ij*(I1-lambda2*lambda2) + Cinv *(J*J/(lambda2*lambda2)) ) * (1.0/d2);
     XC::Tensor M3 = ( C - I_ij*(I1-lambda3*lambda3) + Cinv *(J*J/(lambda3*lambda3)) ) * (1.0/d3);

     isoPK2Stress = M1*Wa(0) + M2*Wa(1) + M3*Wa(2);
   }

   if(caseIndex == 11)
   {
     XC::Tensor M1 = (I_ij - Cinv * (lambda2*lambda2)) * (1.0/(lambda1+lambda2)/(lambda1-lambda2));
     XC::Tensor Mr = Cinv - M1;
     isoPK2Stress = Mr*Wa(2) + M1*Wa(0);
   }

   if(caseIndex == 13)
   {
     XC::Tensor M3 = (I_ij - Cinv * (lambda2*lambda2)) * (1.0/(lambda3+lambda2)/(lambda3-lambda2));
     XC::Tensor Mr = Cinv - M3;
     isoPK2Stress = Mr*Wa(0) + M3*Wa(2);
   }

   if(caseIndex == 2)
   {

   }

   double dWdJ = W->dvolwOdJ(J);
   XC::Tensor volPK2Stress = Cinv * J * dWdJ;

   thisPK2Stress = volPK2Stress + isoPK2Stress; // This is PK2Stress

   thisGreenStrain = (C - I_ij) * 0.5; // This is Green Strain

   XC::Tensor L_iso = this->FDisoStiffness();
   XC::Tensor L_vol = this->FDvolStiffness();
   Stiffness = L_iso + L_vol; // This is Langrangian Tangent Stiffness

   return 0;
}
//! @brief 
int XC::FDdecoupledElastic3D::getCaseIndex(void) const
  { return caseIndex; }


