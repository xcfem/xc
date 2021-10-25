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
//# UPDATE HISTORY:    22Nov2002 small fixes, formatting...
//#
//#
//===============================================================================



#include <material/nD/elastic_isotropic/PressureDependentElastic3D.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>
#include "material/nD/NDMaterialType.h"

XC::Matrix XC::PressureDependentElastic3D::D(6,6);   // global for XC::ElasticIsotropic3D only
XC::Vector XC::PressureDependentElastic3D::sigma(6); // global for XC::ElasticIsotropic3D only


XC::PressureDependentElastic3D::PressureDependentElastic3D(int tag, double E, double nu, double rhop, double expp, double pr, double pop):
  ElasticIsotropicMaterial(tag, ND_TAG_PressureDependentElastic3D,6, E, nu, rhop),
  exp0(expp), p_ref(pr), p_cutoff(pop), Cepsilon(6), p_n(0.0), p_n1(0.0)
{}

XC::PressureDependentElastic3D::PressureDependentElastic3D(int tag)
  : ElasticIsotropicMaterial(tag, ND_TAG_PressureDependentElastic3D,6, 0.0, 0.0, 0.0), Cepsilon(6), p_n(0.0), p_n1(0.0)
  {}


//! @brief Return the material tangent stiffness.
const XC::Matrix &XC::PressureDependentElastic3D::getTangent(void) const
  {
    double p = p_n;
    if (p <= p_cutoff)
      p = p_cutoff;
    double Eo = E * pow(p/p_ref, exp0);

    double mu2 = Eo/(1.0+v);
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
    double p = p_n;
    if (p <= p_cutoff)
      p = p_cutoff;
    double Eo = E * pow(p/p_ref, exp0);

    double mu2 = Eo/(1.0+v);
    double lam = v*mu2/(1.0-2.0*v);
    double mu  = 0.50*mu2;

    mu2 += lam;

    const Vector strain= getStrain();
    const double eps0 = strain(0);
    const double eps1 = strain(1);
    const double eps2 = strain(2);

    sigma(0) = mu2*eps0 + lam*(eps1+eps2);
    sigma(1) = mu2*eps1 + lam*(eps2+eps0);
    sigma(2) = mu2*eps2 + lam*(eps0+eps1);

    p_n1 = (sigma(0)+sigma(1)+sigma(2))/3.0;

    sigma(3) = mu*strain(3);
    sigma(4) = mu*strain(4);
    sigma(5) = mu*strain(5);

    return sigma;
  }

int XC::PressureDependentElastic3D::commitState(void)
  {
    Cepsilon= epsilon;
    p_n = p_n1;
    return 0;
  }

int XC::PressureDependentElastic3D::revertToLastCommit(void)
  {
    epsilon = Cepsilon;
    return 0;
  }

int XC::PressureDependentElastic3D::revertToStart(void)
  {
    int retval= ElasticIsotropicMaterial::revertToStart();
    p_n = 0.0;
    return retval;
  }

XC::NDMaterial* XC::PressureDependentElastic3D::getCopy(void) const
  { return new PressureDependentElastic3D(*this); }

const std::string &XC::PressureDependentElastic3D::getType(void) const
  { return strTypeThreeDimensional; }

int XC::PressureDependentElastic3D::getOrder(void) const
  {
    return 6;
  }

//! @brief Send object members through the communicator argument.
int XC::PressureDependentElastic3D::sendData(Communicator &comm)
  {
    int res= ElasticIsotropicMaterial::sendData(comm);
    res+= comm.sendDoubles(exp0,p_ref,p_cutoff,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::PressureDependentElastic3D::recvData(const Communicator &comm)
  {
    int res= ElasticIsotropicMaterial::recvData(comm);
    res+= comm.receiveDoubles(exp0,p_ref,p_cutoff,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::PressureDependentElastic3D::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::PressureDependentElastic3D::recvSelf(const Communicator &comm)
   {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
		    << "; failed to receive data.\n";
      }
    return res;
  }

void XC::PressureDependentElastic3D::Print(std::ostream &s, int flag) const
  {
    s << "PressureDependentElastic3D" << std::endl;
    s << "\ttag: " << this->getTag() << std::endl;
    s << "\tE: " << E << std::endl;
    s << "\tv: " << v << std::endl;
    s << "\texp: " << exp0 << std::endl;
    s << "\tp_ref: " << p_ref << std::endl;
    s << "\tp_cutoff: " << p_cutoff << std::endl;
    //s << "\tD: " << D << std::endl;
  }


