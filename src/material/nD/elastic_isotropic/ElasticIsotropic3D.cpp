//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
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
//# PURPOSE:           Elastic Isotropic XC::Material implementation:
//# CLASS:             ElasticIsotropic3D
//#
//# VERSION:           0.61803398874989 (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Zhaohui Yang, Boris Jeremic (jeremic@ucdavis.edu)
//# PROGRAMMER(S):     Zhaohui Yang, Boris Jeremic
//#
//#
//# DATE:              10Oct2000
//# UPDATE HISTORY:    22Nov2002 small fixes 
//#
//#
//===============================================================================
                                                                        
#include <material/nD/elastic_isotropic/ElasticIsotropic3D.h>

#include "utility/matrix/Matrix.h"
#include "material/nD/TipoMaterialND.h"

XC::Matrix XC::ElasticIsotropic3D::D(6,6);	  // global for XC::ElasticIsotropic3D only
XC::Vector XC::ElasticIsotropic3D::sigma(6);	 // global for XC::ElasticIsotropic3D onyl
XC::stresstensor XC::ElasticIsotropic3D::Stress;

XC::ElasticIsotropic3D::ElasticIsotropic3D(int tag, double E, double nu, double rho):
  ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropic3D,6, E, nu, rho),Dt(0)
{
	// Set up the elastic constant matrix for 3D elastic isotropic 
	D.Zero();
}

XC::ElasticIsotropic3D::ElasticIsotropic3D(int tag)
  : ElasticIsotropicMaterial(tag, ND_TAG_ElasticIsotropic3D,6, 0.0, 0.0, 0.0), Dt(0)
  {}

XC::ElasticIsotropic3D::ElasticIsotropic3D()
  : ElasticIsotropicMaterial(0, ND_TAG_ElasticIsotropic3D,6, 0.0, 0.0, 0.0), Dt(0)
  {}

XC::ElasticIsotropic3D::~ElasticIsotropic3D()
  {
    if(Dt) delete Dt;
  }

int XC::ElasticIsotropic3D::setTrialStrainIncr(const XC::Vector &v)
{
	epsilon += v;

	return 0;
}

int XC::ElasticIsotropic3D::setTrialStrainIncr(const XC::Vector &v, const XC::Vector &r)
  {
    epsilon += v;
    return 0;
  }

const XC::Matrix &XC::ElasticIsotropic3D::getTangent(void) const
  {
    double mu2 = E/(1.0+v);
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

const XC::Matrix &XC::ElasticIsotropic3D::getInitialTangent(void) const
  {
    double mu2 = E/(1.0+v);
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

const XC::Vector &XC::ElasticIsotropic3D::getStress(void) const
  {
       double mu2 = E/(1.0+v);
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
  }

int XC::ElasticIsotropic3D::setTrialStrain(const XC::Tensor &v)
{
    Strain = v;
    return 0;
}

int XC::ElasticIsotropic3D::setTrialStrain(const XC::Tensor &v, const XC::Tensor &r)
{
    Strain = v;
    return 0;
}

int XC::ElasticIsotropic3D::setTrialStrainIncr(const XC::Tensor &v)
{
    //std::cerr << " before set Tri St Incr " << Strain;
    //std::cerr << " Strain Incr " << v << std::endl;n;
    Strain = Strain + v;
    //std::cerr << " after setTrialStrainIncr  " << Strain << std::endl;n;
    return 0;
}

int XC::ElasticIsotropic3D::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r)
{
    Strain = Strain + v;
    return 0;
}

const XC::Tensor &XC::ElasticIsotropic3D::getTangentTensor(void) const
  {
    if(Dt == 0)
      {
        Dt= new XC::BJtensor( 4, def_dim_4, 0.0 ); 
        setInitElasticStiffness();
      }
    return *Dt;
  }

const XC::stresstensor &XC::ElasticIsotropic3D::getStressTensor(void) const
  {
    if(Dt == 0)
      {
        Dt = new XC::BJtensor( 4, def_dim_4, 0.0 ); 
        setInitElasticStiffness();
      }
    Stress = (*Dt)("ijkl") * Strain("kl");
    return Stress;
  }

const XC::straintensor &XC::ElasticIsotropic3D::getStrainTensor(void) const
  { return Strain; }

const XC::straintensor &XC::ElasticIsotropic3D::getPlasticStrainTensor(void) const
  {
    //Return zero XC::straintensor
    static XC::straintensor t;
    return t;
  }

int XC::ElasticIsotropic3D::commitState(void)
  { return 0; }

int XC::ElasticIsotropic3D::revertToLastCommit(void)
  { return 0; }

int XC::ElasticIsotropic3D::revertToStart(void)
  { return 0; }

XC::NDMaterial *XC::ElasticIsotropic3D::getCopy(void) const
  { return new ElasticIsotropic3D(*this); }

const std::string &XC::ElasticIsotropic3D::getType(void) const
  { return strTipoThreeDimensional; }

int XC::ElasticIsotropic3D::getOrder(void) const
  { return 6; }

void XC::ElasticIsotropic3D::Print(std::ostream &s, int flag)
  {
	s << strTipoElasticIsotropic3D << std::endl;
	s << "\ttag: " << this->getTag() << std::endl;
	s << "\tE: " << E << std::endl;
	s << "\tv: " << v << std::endl;
	s << "\trho: " << rho << std::endl;
  }


//================================================================================
void XC::ElasticIsotropic3D::setInitElasticStiffness(void) const
  {    
    BJtensor ret( 4, def_dim_4, 0.0 );
    				       
    // Kronecker delta XC::BJtensor
    BJtensor I2("I", 2, def_dim_2);

    BJtensor I_ijkl = I2("ij")*I2("kl");


    //I_ijkl.null_indices();
    BJtensor I_ikjl = I_ijkl.transpose0110();
    BJtensor I_iljk = I_ijkl.transpose0111();
    BJtensor I4s = (I_ikjl+I_iljk)*0.5;
    
    // Building elasticity XC::BJtensor
    ret = I_ijkl*( E*v / ( (1.0+v)*(1.0 - 2.0*v) ) ) + I4s*( E / (1.0 + v) );
    
    //ret.print();
    *Dt = ret;

    return;
  }


