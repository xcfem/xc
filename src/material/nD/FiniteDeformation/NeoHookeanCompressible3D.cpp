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

// the traditional neo-Hookean hyperelasticity:
// w = 0.5*lambda*(lnJ)^2 - G*(lnJ) + 0.5*G*(trace(C)-3)

#include "material/nD/FiniteDeformation/NeoHookeanCompressible3D.h"
#include "material/nD/NDMaterialType.h"

//-----------------------------------------------------------------------------------------------------------------------------------------------
XC::NeoHookeanCompressible3D::NeoHookeanCompressible3D(int tag,
                                                   int classTag,
                                                   double K_in,
                                                   double G_in,
                                                   double rho_in = 0.0)
:FiniteDeformationElastic3D(tag, classTag, rho_in), K(K_in), G(G_in)
{}

//-----------------------------------------------------------------------------------------------------------------------------------------------
XC::NeoHookeanCompressible3D::NeoHookeanCompressible3D(int tag,
                                               double K_in,
                                               double G_in,
                                                   double rho_in = 0.0)
:FiniteDeformationElastic3D(tag, ND_TAG_NeoHookeanCompressible3D, rho_in), K(K_in), G(G_in)
{}

XC::NeoHookeanCompressible3D::NeoHookeanCompressible3D(int tag)
  :FiniteDeformationElastic3D(tag, ND_TAG_NeoHookeanCompressible3D, 0.0), K(0.0), G(0.0)
  {}

//------------------------------------------------------------------------------------------------------------------------------------------------
XC::NeoHookeanCompressible3D::NeoHookeanCompressible3D( )
:FiniteDeformationElastic3D(0, 0, 0.0), K(0.0), G(0.0)
  {}


//--------------------------------------------------------------------------------------------------------------------------------------------------
int XC::NeoHookeanCompressible3D::setTrialF(const XC::straintensor &f)
{
   FromForC = 0;
   F = f;
   C = F("ki")*F("kj");   C.null_indices();
   return this->ComputeTrials();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int XC::NeoHookeanCompressible3D::setTrialFIncr(const XC::straintensor &df)
{
   return this->setTrialF(this->getF() + df);
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int XC::NeoHookeanCompressible3D::setTrialC(const XC::straintensor &c)
{
   FromForC = 1;
   C = c;
   return this->ComputeTrials();
}

//---------------------------------------------------------------------------------------------------------------------------------------------------
int XC::NeoHookeanCompressible3D::setTrialCIncr(const XC::straintensor &dc)
{
   return this->setTrialC(this->getC() + dc);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------
const XC::straintensor &XC::NeoHookeanCompressible3D::getF(void) const
  { return F; }

//-----------------------------------------------------------------------------------------------------------------------------------------------------
const XC::straintensor &XC::NeoHookeanCompressible3D::getC(void) const
  { return C; }

////------------------------------------------------------------------------------------------------------------------------------------------------------
//const double XC::NeoHookeanCompressible3D::getJ(void)
//{
//   return J;
//}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::Tensor& XC::NeoHookeanCompressible3D::getTangentTensor(void) const
  { return Stiffness; }

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::Tensor &XC::NeoHookeanCompressible3D::getInitialTangentTensor(void) const
  { return this->getTangentTensor(); }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::straintensor &XC::NeoHookeanCompressible3D::getStrainTensor(void) const
  { return thisGreenStrain; }

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::stresstensor &XC::NeoHookeanCompressible3D::getStressTensor(void) const
  { return thisPK2Stress; }

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::stresstensor XC::NeoHookeanCompressible3D::getPK1StressTensor(void) const
  {
    stresstensor thisSPKStress;
    stresstensor thisFPKStress;

    if( FromForC == 0 )
      {
        thisSPKStress = this->getStressTensor();
        thisFPKStress = thisSPKStress("ij") * (F.transpose11())("jk") ;
      }

    if( FromForC == 1 )
      {
        std::cerr << "NeoHookeanCompressible3D: unknown deformation gradient - cannot compute PK1 stress" << "\n";
        exit(-1);
      }
    return thisFPKStress;
  }

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const XC::stresstensor XC::NeoHookeanCompressible3D::getCauchyStressTensor(void) const
{
   XC::stresstensor thisSPKStress;
   XC::stresstensor thisCauchyStress;

   if( FromForC == 0 ) {
    thisSPKStress = this->getStressTensor();
    thisCauchyStress = F("ij") * thisSPKStress("jk") * (F.transpose11())("kl") * (1.0/J);
   }

   if( FromForC == 1 ) {
    std::cerr << "NeoHookeanCompressible3D: unknown deformation gradient - cannot compute Cauchy stress" << "\n";
    exit(-1);
   }

    return thisCauchyStress;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::NeoHookeanCompressible3D::commitState(void)
{
   return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::NeoHookeanCompressible3D::revertToLastCommit(void)
{
   return 0;
}

//! @brief Revert the material to its initial state.
int XC::NeoHookeanCompressible3D::revertToStart(void)
  {
    int retval= FiniteDeformationElastic3D::revertToStart();
    BJtensor F0("I", 2, def_dim_2);
    F = F0;
    C = F0;
    Cinv = F0;
    J = 1.0;

    BJtensor ss_zero(2,def_dim_2,0.0);
    thisPK2Stress = ss_zero;
    thisGreenStrain = ss_zero;

    Stiffness = getInitialTangentTensor();

    return retval;
  }

//! @brief Virtual constructor.
XC::NDMaterial * XC::NeoHookeanCompressible3D::getCopy(void) const
  { return new NeoHookeanCompressible3D(*this); }

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
XC::NDMaterial * XC::NeoHookeanCompressible3D::getCopy(const std::string &type) const
  {
    std::cerr << "NeoHookeanCompressible3D::getCopy(const std::string &) - not yet implemented\n";
    return nullptr;
  }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const std::string &XC::NeoHookeanCompressible3D::getType(void) const
  { return strTypeThreeDimensionalFD; }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::NeoHookeanCompressible3D::getOrder(void) const
{
   return 6;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::NeoHookeanCompressible3D::sendSelf(Communicator &comm)
{
   int res = 0;
   // not yet implemented
   return res;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::NeoHookeanCompressible3D::recvSelf(const Communicator &comm)
{
   int res = 0;
   // not yet implemented
   return res;
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void XC::NeoHookeanCompressible3D::Print(std::ostream &s, int flag) const
{
   s << "Finite Deformation Elastic 3D model" << "\n";
   s << "\trho: " << rho << "\n";
   s << "\tK: " << K << "\n";
   s << "\tG: " << G << "\n";
   return;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//int XC::NeoHookeanCompressible3D::setParameter(char **argv, Parameter &param)
//{
//   return -1;
//}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//int XC::NeoHookeanCompressible3D::updateParameter(int parameterID, Information &info)
//{
//   return -1;
//}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int XC::NeoHookeanCompressible3D::ComputeTrials()
{   
   BJtensor tensorI2("I", 2, def_dim_2);
   BJtensor tsr1;
   BJtensor tsr2;

   // Cinv:
   Cinv = C.inverse();
   Cinv.symmetrize11();

   // J:
   J = sqrt(C.determinant());

   // lame constants:
   double lambda = K - 2.0*G/3.0;
   double mu = G - lambda*log(J);

   // Pk2Stress:
   thisPK2Stress = (tensorI2-Cinv)*G + Cinv*lambda*log(J);
   
   // Green Strain:
   thisGreenStrain = (C - tensorI2) * 0.5; 
   
   // Langrangian Tangent Stiffness:
   tsr1 = Cinv("ij")*Cinv("kl");
     tsr1.null_indices();
   tsr2 = tsr1.transpose0110() + tsr1.transpose0111();
   Stiffness = tsr1*lambda + tsr2*mu;

   return 0;
}

