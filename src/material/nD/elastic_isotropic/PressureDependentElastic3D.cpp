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
//# PURPOSE:           Pressure dependent elastic isotropic material implementation:
//# CLASS:             PressureDependentElastic3D
//#
//# VERSION:           0.61803398874989 (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhaohui Yang, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhaohui Yang, Boris Jeremic
//#
//#
//# DATE:              07July2001
//# UPDATE HISTORY:    22Nov2002 small fixes, formating...
//#
//#
//===============================================================================



#include <material/nD/elastic_isotropic/PressureDependentElastic3D.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>

#include <utility/matrix/nDarray/straint.h>
#include <utility/matrix/nDarray/stresst.h>
#include <utility/matrix/nDarray/Tensor.h>

#include "material/nD/TipoMaterialND.h"

XC::Matrix XC::PressureDependentElastic3D::D(6,6);   // global for XC::ElasticIsotropic3D only
XC::Vector XC::PressureDependentElastic3D::sigma(6); // global for XC::ElasticIsotropic3D only


XC::PressureDependentElastic3D::PressureDependentElastic3D(int tag, double E, double nu, double rhop, double expp, double pr, double pop):
  ElasticIsotropicMaterial(tag, ND_TAG_PressureDependentElastic3D,6, E, nu, rhop),
  exp(expp), p_ref(pr), p_cutoff(pop)
{
  // Set up the elastic constant matrix for 3D elastic isotropic
  Dt = BJtensor( 4, def_dim_4, 0.0 );
  ComputeElasticStiffness();
}

XC::PressureDependentElastic3D::PressureDependentElastic3D(int tag)
  : ElasticIsotropicMaterial(tag, ND_TAG_PressureDependentElastic3D,6, 0.0, 0.0, 0.0)
  { Dt = BJtensor( 4, def_dim_4, 0.0 ); }

XC::PressureDependentElastic3D::PressureDependentElastic3D()
  : ElasticIsotropicMaterial(0, ND_TAG_PressureDependentElastic3D,6, 0.0, 0.0, 0.0)
  {
    Dt = BJtensor( 4, def_dim_4, 0.0 );
  }

int XC::PressureDependentElastic3D::setTrialStrainIncr(const XC::Vector &v)
  {
    epsilon += v;
    return 0;
  }

int XC::PressureDependentElastic3D::setTrialStrainIncr(const XC::Vector &v, const XC::Vector &r)
  {
    epsilon += v;
    return 0;
  }

const XC::Matrix &XC::PressureDependentElastic3D::getTangent(void) const
  {
    //Update E
    Stress = getStressTensor();
    double p = Stress.p_hydrostatic();

    if(p <= p_cutoff)
      p = p_cutoff;
    double Ec = E * pow(p/p_ref, exp);

    double mu2 = Ec/(1.0+v);
    double lam = v*mu2/(1.0-2.0*v);
    double mu  = 0.50*mu2;

    mu2 += lam;

    D(0,0) = D(1,1) = D(2,2) = mu2;
    D(0,1) = D(1,0) = lam;
    D(0,2) = D(2,0) = lam;
    D(1,2) = D(2,1) = lam;
    D(3,3) = mu;
    D(4,4) = mu;
    D(5,5) = mu;

    return D;
  }

const XC::Vector& XC::PressureDependentElastic3D::getStress(void) const
  {
    //Update E
    Stress = getStressTensor();
    double p = Stress.p_hydrostatic();

    if(p <= p_cutoff)
    p = p_cutoff;
    double Ec = E * pow(p/p_ref, exp);

    double mu2 = Ec/(1.0+v);
    double lam = v*mu2/(1.0-2.0*v);
    double mu  = 0.50*mu2;

    mu2 += lam;

    double eps0 = epsilon(0);
    double eps1 = epsilon(1);
    double eps2 = epsilon(2);

    sigma(0) = mu2*eps0 + lam*(eps1+eps2);
    sigma(1) = mu2*eps1 + lam*(eps2+eps0);
    sigma(2) = mu2*eps2 + lam*(eps0+eps1);

    sigma(3) = mu*epsilon(3);
    sigma(4) = mu*epsilon(4);
    sigma(5) = mu*epsilon(5);

    return sigma;
    //sigma = D*epsilon;
    //return sigma;
  }

int XC::PressureDependentElastic3D::setTrialStrain(const XC::Tensor &v)
  {
    Strain = v;
    return 0;
  }

int XC::PressureDependentElastic3D::setTrialStrain(const XC::Tensor &v, const XC::Tensor &r)
  {
    Strain = v;
    return 0;
  }

int XC::PressureDependentElastic3D::setTrialStrainIncr(const XC::Tensor &v)
  {
    //std::cerr << " before set Tri St Incr " << Strain;
    //std::cerr << " Strain Incr " << v << std::endl;n;
    Strain = Strain + v;
    //std::cerr << " after setTrialStrainIncr  " << Strain << std::endl;n;
    return 0;
  }

int XC::PressureDependentElastic3D::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r)
  {
    Strain = Strain + v;
    return 0;
  }

const XC::Tensor& XC::PressureDependentElastic3D::getTangentTensor(void) const
  {
    ComputeElasticStiffness();
    return Dt;
  }

const XC::stresstensor &XC::PressureDependentElastic3D::getStressTensor(void) const
  {
    //First time? then return zero stress
    if( Dt.rank() == 0 )
      return Stress;
    else
      {
        Stress = Dt("ijkl") * Strain("kl");
        return Stress;
      }
  }

const XC::straintensor &XC::PressureDependentElastic3D::getStrainTensor(void) const
  { return Strain; }

const XC::straintensor &XC::PressureDependentElastic3D::getPlasticStrainTensor(void) const
  {
    //Return zero XC::straintensor
    static XC::straintensor t;
    return t;
  }

int XC::PressureDependentElastic3D::commitState(void)
  {
    //Set the new Elastic constants
    BJtensor ret( 4, def_dim_4, 0.0 );

    // Kronecker delta XC::BJtensor
    BJtensor I2("I", 2, def_dim_2);

    BJtensor I_ijkl = I2("ij")*I2("kl");


    //I_ijkl.null_indices();
    BJtensor I_ikjl = I_ijkl.transpose0110();
    BJtensor I_iljk = I_ijkl.transpose0111();
    BJtensor I4s = (I_ikjl+I_iljk)*0.5;

    //Update E according to hydrostatic stress
    Stress = this->getStressTensor();
    //Dt("ijkl") * Strain("kl");
    double p = Stress.p_hydrostatic();
    //std::cerr << " p = " <<  p;

    //Cut-off pressure
    if(p <= p_cutoff)
      p = p_cutoff;

    double Ec = E * pow(p/p_ref, exp);
    //std::cerr << " Eo = " << E << " Ec = " << Ec << " Exp:" << exp<< " p_ref:" << p_ref << " po: " << po<< std::endl;
    //std::cerr << " coef = " << Ec/E << std::endl;

    // Building elasticity XC::BJtensor
    ret = I_ijkl*( Ec*v / ( (1.0+v)*(1.0 - 2.0*v) ) ) + I4s*( Ec / (1.0 + v) );

    //ret.print();
    Dt = ret;

    return 0;
  }

int XC::PressureDependentElastic3D::revertToLastCommit(void)
  {
    // Nothing was previously committed
    return 0;
  }

int XC::PressureDependentElastic3D::revertToStart(void)
  {
    // Nothing was previously committed
    return 0;
  }

XC::NDMaterial* XC::PressureDependentElastic3D::getCopy(void) const
  { return new PressureDependentElastic3D(*this); }

const std::string &XC::PressureDependentElastic3D::getType(void) const
  { return strTipoThreeDimensional; }

int XC::PressureDependentElastic3D::getOrder(void) const
  {
    return 6;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::PressureDependentElastic3D::sendData(CommParameters &cp)
  {
    int res= ElasticIsotropicMaterial::sendData(cp);
    res+= cp.sendDoubles(exp,p_ref,p_cutoff,getDbTagData(),CommMetaData(3));
    res+= cp.sendTensor(Dt,getDbTagData(),CommMetaData(4));
    res+= cp.sendTensor(Stress,getDbTagData(),CommMetaData(5));
    res+= cp.sendTensor(Strain,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::PressureDependentElastic3D::recvData(const CommParameters &cp)
  {
    int res= ElasticIsotropicMaterial::recvData(cp);
    res+= cp.receiveDoubles(exp,p_ref,p_cutoff,getDbTagData(),CommMetaData(3));
    res+= cp.receiveTensor(Dt,getDbTagData(),CommMetaData(4));
    res+= cp.receiveTensor(Stress,getDbTagData(),CommMetaData(5));
    res+= cp.receiveTensor(Strain,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PressureDependentElastic3D::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(7);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::PressureDependentElastic3D::recvSelf(const CommParameters &cp)
   {
    inicComm(7);
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

void XC::PressureDependentElastic3D::Print(std::ostream &s, int flag)
  {
    s << "PressureDependentElastic3D" << std::endl;
    s << "\ttag: " << this->getTag() << std::endl;
    s << "\tE: " << E << std::endl;
    s << "\tv: " << v << std::endl;
    s << "\texp: " << exp << std::endl;
    s << "\tp_ref: " << p_ref << std::endl;
    s << "\tp_cutoff: " << p_cutoff << std::endl;
    //s << "\tD: " << D << std::endl;
  }


//================================================================================
void XC::PressureDependentElastic3D::ComputeElasticStiffness(void) const
  {
    BJtensor ret( 4, def_dim_4, 0.0 );
    // Kronecker delta XC::BJtensor
    BJtensor I2("I", 2, def_dim_2);
    BJtensor I_ijkl = I2("ij")*I2("kl");
    //I_ijkl.null_indices();
    BJtensor I_ikjl = I_ijkl.transpose0110();
    BJtensor I_iljk = I_ijkl.transpose0111();
    BJtensor I4s = (I_ikjl+I_iljk)*0.5;
    //Initialize E according to initial pressure in the gauss point
    Stress = getStressTensor();
    //Dt("ijkl") * Strain("kl");
    double p = Stress.p_hydrostatic();
    if(p <= p_cutoff)
      p = p_cutoff;
    double Eo = E * pow(p/p_ref, exp);
    //std::cerr << " p_ref = " <<  p_ref << " p = " << p << std::endl;
    //std::cerr << " coef = " << Eo/E << std::endl;
    //std::cerr << " E@ref = " << E << " Eo = " << Eo << std::endl;

    // Building elasticity XC::BJtensor
    ret = I_ijkl*( Eo*v / ( (1.0+v)*(1.0 - 2.0*v) ) ) + I4s*( Eo / (1.0 + v) );
    //ret.print();
    Dt = ret;
    return;
  }
