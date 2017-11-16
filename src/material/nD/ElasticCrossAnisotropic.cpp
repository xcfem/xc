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
//# PURPOSE:           Elastic Cross Anisotropic XC::Material implementation:
//# CLASS:             ElasticIsotropic3D
//#
//# VERSION:           0.61803398874989 (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhaohui Yang, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhaohui Yang, Yi Bian, Boris Jeremic
//#
//#
//# DATE:              10Oct2002 Yi Bian
//# UPDATE HISTORY:    March 20, 2003 Revised by Joey Yang & Boris Jeremic, UC Davis
//#
//#
//===============================================================================
//


#include <material/nD/ElasticCrossAnisotropic.h>

#include <utility/matrix/Matrix.h>
#include "material/nD/TipoMaterialND.h"

//XC::Tensor XC::ElasticCrossAnisotropic::rank2 (2, def_dim_2, 0.0 ) ;
//XC::Tensor XC::ElasticCrossAnisotropic::rank4 (2, def_dim_2, 0.0 ) ;

XC::Vector XC::ElasticCrossAnisotropic::sigma(6);
XC::Matrix XC::ElasticCrossAnisotropic::D(6, 6);

///////////////////////////////////////////////////////////////////////////////
XC::ElasticCrossAnisotropic::ElasticCrossAnisotropic(int tag,double Ehp, double Evp, 
                                                 double nuhvp,double nuhhp,double Ghvp, 
                                                 double rhop):
XC::NDMaterial(tag, ND_TAG_ElasticCrossAnisotropic3D),
Tepsilon(6), Cepsilon(6),
Eh(Ehp), Ev(Evp), 
nuhv(nuhvp), nuhh(nuhhp),
Ghv(Ghvp),
rho(rhop)
{
   D.Zero();
   Dt = BJtensor( 4, def_dim_4, 0.0 );
   this->convertD2TensorEijkl();
}

//! @brief Constructor.
XC::ElasticCrossAnisotropic::ElasticCrossAnisotropic(int tag)
  : XC::NDMaterial(tag, ND_TAG_ElasticCrossAnisotropic3D), Tepsilon(6), Cepsilon(6)
  {
    D.Zero();
  }

///////////////////////////////////////////////////////////////////////////////
XC::ElasticCrossAnisotropic::ElasticCrossAnisotropic(void)
  : Tepsilon(6), Cepsilon(6)
  {
    D.Zero();
  }

///////////////////////////////////////////////////////////////////////////////
double XC::ElasticCrossAnisotropic::getrho()
{
  return rho;
}

///////////////////////////////////////////////////////////////////////////////
XC::NDMaterial* XC::ElasticCrossAnisotropic::getCopy(const std::string &type) const
  {
    ElasticCrossAnisotropic *theModel= nullptr;
    if((type==strTipoThreeDimensional))
      {
        theModel = new ElasticCrossAnisotropic(this->getTag(), Eh, Ev, nuhv, nuhh, Ghv, rho);
            // This function should only be called during element instantiation, so
            // no state determination is performed on the material model object
            // prior to copying the material model(calling this function)
      }
    else
      std::cerr <<"ElasticCrossAnisotropic::getModel failed to get model " << type << "\n";
    return theModel;
  }

///////////////////////////////////////////////////////////////////////////////
int XC::ElasticCrossAnisotropic::setTrialStrain(const XC::Vector &strain)
{
  Tepsilon = strain;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
int XC::ElasticCrossAnisotropic::setTrialStrain(const XC::Vector &strain, const XC::Vector &rate)
{
  Tepsilon = strain;

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
int XC::ElasticCrossAnisotropic::setTrialStrainIncr(const XC::Vector &strain)
{
  //epsilon += strain;
  Tepsilon = Cepsilon;
  Tepsilon.addVector(1.0, strain, 1.0);

  return 0;
}

///////////////////////////////////////////////////////////////////////////////
int XC::ElasticCrossAnisotropic::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
{
  //epsilon += strain;
  Tepsilon = Cepsilon;
  Tepsilon.addVector(1.0, strain, 1.0);

  return 0;
}

const XC::Matrix &XC::ElasticCrossAnisotropic::getTangent(void) const
  {
    ElasticCrossAnisotropic *this_no_const=const_cast<ElasticCrossAnisotropic *>(this);
    this_no_const->setInitElasticStiffness();
    return D;
  }

const XC::Vector &XC::ElasticCrossAnisotropic::getStress(void) const
{
    double eps0 = Tepsilon(0);
    double eps1 = Tepsilon(1);
    double eps2 = Tepsilon(2);
    double eps3 = Tepsilon(3);
    double eps4 = Tepsilon(4);
    double eps5 = Tepsilon(5);

    //sigma = D*epsilon;
    sigma(0) = D(0,0) * eps0 + D(0,1) * eps1 + D(0,2) * eps2;
    sigma(1) = D(1,0) * eps0 + D(1,1) * eps1 + D(1,2) * eps2;
    sigma(2) = D(2,0) * eps0 + D(2,1) * eps1 + D(2,2) * eps2;
    sigma(3) = D(3,3) * eps3;
    sigma(4) = D(4,4) * eps4;
    sigma(5) = D(5,5) * eps5;

    return sigma;
}

const XC::Vector &XC::ElasticCrossAnisotropic::getStrain(void) const
{
    return Tepsilon;
}

int
XC::ElasticCrossAnisotropic::setTrialStrain(const XC::Tensor &v)
{
    Strain = v;
    return 0;
}

int
XC::ElasticCrossAnisotropic::setTrialStrain(const XC::Tensor &v, const XC::Tensor &r)
{
    Strain = v;
    return 0;
}

int
XC::ElasticCrossAnisotropic::setTrialStrainIncr(const XC::Tensor &v)
{
    Strain = Strain + v;
    return 0;
}

int XC::ElasticCrossAnisotropic::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r)
  {
    Strain = Strain + v;
    return 0;
  }

const XC::Tensor &XC::ElasticCrossAnisotropic::getTangentTensor(void) const
  { return Dt; }

const XC::stresstensor &XC::ElasticCrossAnisotropic::getStressTensor(void) const
  {
    Stress = Dt("ijkl") * Strain("kl");
    return Stress;
  }

const XC::straintensor &XC::ElasticCrossAnisotropic::getStrainTensor(void) const
  { return Strain; }

const XC::straintensor &XC::ElasticCrossAnisotropic::getPlasticStrainTensor(void) const
  {
     //Return zero XC::straintensor
     static XC::straintensor t;
     return t;
  }

int XC::ElasticCrossAnisotropic::commitState(void)
{
  //sigma = D * epsilon;
  Cepsilon = Tepsilon;

        return 0;
}

int
XC::ElasticCrossAnisotropic::revertToLastCommit(void)
{
  Tepsilon = Cepsilon;
  return 0;
}

int XC::ElasticCrossAnisotropic::revertToStart(void)
  {
    Cepsilon.Zero();
    return 0;
  }

XC::NDMaterial *XC::ElasticCrossAnisotropic::getCopy(void) const
  { return new ElasticCrossAnisotropic(*this); }

const std::string &XC::ElasticCrossAnisotropic::getType(void) const
  { return strTipoThreeDimensional; }

int XC::ElasticCrossAnisotropic::getOrder(void) const
  { return 6; }

//! @brief Send object members through the channel being passed as parameter.
int XC::ElasticCrossAnisotropic::sendData(CommParameters &cp)
  {
    int res= NDMaterial::sendData(cp);
    res+= cp.sendVector(Tepsilon,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(Cepsilon,getDbTagData(),CommMetaData(2));
    res+= cp.sendTensor(Dt,getDbTagData(),CommMetaData(3));
    res+= cp.sendTensor(Stress,getDbTagData(),CommMetaData(4));
    res+= cp.sendTensor(Strain,getDbTagData(),CommMetaData(5));
    res+= cp.sendDoubles(Eh,Ev,nuhv,nuhh,Ghv,rho,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::ElasticCrossAnisotropic::recvData(const CommParameters &cp)
  {
    int res= NDMaterial::recvData(cp);
    res+= cp.receiveVector(Tepsilon,getDbTagData(),CommMetaData(1));
    res+= cp.receiveVector(Cepsilon,getDbTagData(),CommMetaData(2));
    res+= cp.receiveTensor(Dt,getDbTagData(),CommMetaData(3));
    res+= cp.receiveTensor(Stress,getDbTagData(),CommMetaData(4));
    res+= cp.receiveTensor(Strain,getDbTagData(),CommMetaData(5));
    res+= cp.receiveDoubles(Eh,Ev,nuhv,nuhh,Ghv,rho,getDbTagData(),CommMetaData(6));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ElasticCrossAnisotropic::sendSelf(CommParameters &cp)
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
int XC::ElasticCrossAnisotropic::recvSelf(const CommParameters &cp)
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

void XC::ElasticCrossAnisotropic::Print(std::ostream &s, int flag)
{
  s << "Elastic Cross-Anisotropic XC::Material Model\n";
  s << "\tEh:  " << Eh << "\tEv:  " << Ev << "\n";
  s << "\tnuhv:  " << nuhv << "\tnuhh:  " << nuhh << "\n";
  s << "\tGhv:  " << Ghv << "\trho:  " << rho << "\n";
  return;
}

//int XC::ElasticCrossAnisotropic::setParameter(char **argv, Parameter &param)
//{
//  return -1;
//}

/*int
XC::ElasticCrossAnisotropic::updateParameter(int parameterID, Information &info)
{
  return -1;
}
*/

void XC::ElasticCrossAnisotropic::setInitElasticStiffness(void)
{
   //Old codes from Yi Bian
   //double A = 1/((1 + nuhv) * (1 - 2 * nuhv));
   //D(0, 0) = A * (1 - nuhv) * Ev;
   //D(1, 1) = D(2, 2) = A * (1 - nuhv) * Eh;
   //D(0, 1) = D(0, 2) = D(1, 0) = D(2, 0) = A * Eh * nuhh;
   //D(3, 3) = Eh/(1 + nuhv);
   //D(4, 4) = D(5, 5) = 2 * Ghv;

//  Compliance matrix C Refer to Thor C. Heimdahl and Andrew Drescher
//  "Elastic Anisotropy of Tire Shreds", {ASCE} Journal of Geotechnical
//  and Geoenvironmental Engineering, 1999, pp. 383-389
//  |Sxx|    | 1/Eh     -nuhh/Eh -nuhv/Ev     0       0     0    |
//  |Syy|    |-nuhh/Eh    1/Eh   -nuhv/Ev     0       0     0    |
//  |Szz|    |-nuhv/Ev  -nuhv/Ev   1/Ev       0       0     0    |
//  |Sxy| C= |   0         0       0  2(1+nuhh)/Eh     0     0    |
//  |Sxz|    |   0         0       0         0     1/(Ghv)   0    |
//  |Syz|    |   0         0       0         0         0   1/(Ghv)|
//
   // Form compliance matrix D
   double A = 1.0/Eh;
   double B = 1.0/Ev;
   //std::cerr << "A " << A << " B " << B;
   D(0,0) = D(1,1) = A;
   D(2,2) = B;
   D(0,1) = D(1,0) = -nuhh*A;
   D(0,2) = D(2,0) = D(1,2) = D(2,1) = -nuhv*B;
   D(3,3) = 2*(1.0+nuhh)*A;
   //D(4,4) = D(5,5) = 0.5/Ghv;
   D(4,4) = D(5,5) = 1.0/Ghv;
   //std::cerr << " C " << D;

   // Do invertion once to get Elastic matrix D
   D.Invert( D );
   //std::cerr << " D " << D;

   return;

}

void XC::ElasticCrossAnisotropic::convertD2TensorEijkl(void)
{
   this->setInitElasticStiffness();

   //Convert Matric D to 4th order Elastic constants XC::BJtensor Dt;
   Dt.val(1,1,1,1) = D(0,0); 
   Dt.val(1,1,2,2) = D(0,1); 
   Dt.val(1,1,3,3) = D(0,2); // --> Sigma_xx

   Dt.val(1,2,1,2) = D(3,3); 
   Dt.val(1,2,2,1) = D(3,3); // --> Sigma_xy

   Dt.val(1,3,1,3) = D(4,4); 
   Dt.val(1,3,3,1) = D(4,4); // --> Sigma_xz

   Dt.val(2,1,1,2) = D(3,3); 
   Dt.val(2,1,2,1) = D(3,3); // --> Sigma_yx

   Dt.val(2,2,1,1) = D(1,0); 
   Dt.val(2,2,2,2) = D(1,1); 
   Dt.val(2,2,3,3) = D(1,2); // --> Sigma_yy

   Dt.val(2,3,2,3) = D(5,5); 
   Dt.val(2,3,3,2) = D(5,5); // --> Sigma_yz

   Dt.val(3,1,1,3) = D(4,4); 
   Dt.val(3,1,3,1) = D(4,4); // --> Sigma_zx

   Dt.val(3,2,2,3) = D(5,5); 
   Dt.val(3,2,3,2) = D(5,5); // --> Sigma_zy

   Dt.val(3,3,1,1) = D(2,0); 
   Dt.val(3,3,2,2) = D(2,1); 
   Dt.val(3,3,3,3) = D(2,2); // --> Sigma_zz

   return;
}
