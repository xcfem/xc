/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */

// $Revision: 1.3 $
// $Date: 2002-12-05 22:49:09 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/J2PlateFibre.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: Elastic isotropic model where stress 
// components 22, 33, 13, and 23 are condensed out.

#include "J2PlateFibre.h"           
#include "domain/mesh/element/utils/Information.h"
#include "domain/component/Parameter.h"
#include "material/nD/NDMaterialType.h"

const double XC::J2PlateFibre::one3= (1.0/3.0);
const double XC::J2PlateFibre::two3= (2.0/3.0);
const double XC::J2PlateFibre::root23= sqrt( 2.0 / 3.0 );
XC::Vector XC::J2PlateFibre::sigma(order);
XC::Matrix XC::J2PlateFibre::D(order,order);

void XC::J2PlateFibre::init(void)
  {
    epsPn[0]= 0.0;
    epsPn[1]= 0.0;
    epsPn[2]= 0.0;
    epsPn[3]= 0.0;
    epsPn[4]= 0.0;

    epsPn1[0]= 0.0;
    epsPn1[1]= 0.0;
    epsPn1[2]= 0.0;
    epsPn1[3]= 0.0;
    epsPn1[4]= 0.0;

    alphan= 0.0;
    alphan1= 0.0;
  }

//! @brief Default constructor.
XC::J2PlateFibre::J2PlateFibre(int tag):
  NDMaterial(tag, ND_TAG_J2PlateFibre),
  E(0.0), nu(0.0), sigmaY(0.0), Hiso(0.0), Hkin(0.0), rho(0.0),
  parameterID(0), SHVs(), Tepsilon(order), Tepsilon0(order), dg_n1(0.0)
  { init(); }

XC::J2PlateFibre::J2PlateFibre(int tag, double e, double g, double sy, double hi, double hk)
  : NDMaterial(tag, ND_TAG_J2PlateFibre), E(e), nu(g), sigmaY(sy),
    Hiso(hi), Hkin(hk), rho(0.0), parameterID(0), SHVs(),
    Tepsilon(order), Tepsilon0(order), dg_n1(0.0)
  { init(); }


XC::J2PlateFibre::~J2PlateFibre(void)
  {}

//! @brief Return material density
double XC::J2PlateFibre::getRho(void) const
  { return rho; }

//! @brief Set density
//! @param r: density.
void XC::J2PlateFibre::setRho(const double &r)
  { rho= r; }

//! @brief Return elastic modulus
double XC::J2PlateFibre::getE(void) const
  { return E; }

//! @brief Set elastic modulus
//! @param e: elastic modulus.
void XC::J2PlateFibre::setE(const double &e)
  { E= e; }

//! @brief Return the material Poisson's ratio.
double XC::J2PlateFibre::getnu(void) const
  { return nu; }

//! @brief Set the material Poisson's ratio.
//! @param g: Poisson's ratio.
void XC::J2PlateFibre::setnu(const double &g)
  { nu= g; }

//! @brief Return the material yield stress.
double XC::J2PlateFibre::getYieldStress(void) const
  { return sigmaY; }

//! @brief Set the material yield stress.
//! @param g: yield stress.
void XC::J2PlateFibre::setYieldStress(const double &sgY)
  { sigmaY= sgY; }

//! @brief Return the material isotropic hardening parameter.
double XC::J2PlateFibre::getHiso(void) const
  { return Hiso; }

//! @brief Set the material isotropic hardening parameter.
//! @param hiso: material isotropic hardening parameter.
void XC::J2PlateFibre::setHiso(const double &hiso)
  { Hiso= hiso; }

//! @brief Return the material kinematic hardening parameter.
double XC::J2PlateFibre::getHkin(void) const
  { return Hkin; }

//! @brief Set the material kinematic hardening parameter.
//! @param hkin: material kinematic hardening parameter.
void XC::J2PlateFibre::setHkin(const double &hkin)
  { Hkin= hkin; }

//! @brief Sets the initial strain value.
int XC::J2PlateFibre::setInitialStrain(const Vector &eps)
  {
    Tepsilon0= eps;
    return 0;
  }

//! @brief Returns the value of the initial strain.
const XC::Vector &XC::J2PlateFibre::getInitialStrain(void) const
  { return Tepsilon0; }

//! @brief Set the material trial strain.
int XC::J2PlateFibre::setTrialStrain(const Vector &strain)
  {
    Tepsilon= strain;
    return 0;
  }

int XC::J2PlateFibre::setTrialStrain(const Vector &strain, const Vector &rate)
  {
    Tepsilon= strain;
    return 0;
  }

int XC::J2PlateFibre::setTrialStrainIncr(const Vector &strain)
  { return 0; }

int XC::J2PlateFibre::setTrialStrainIncr(const Vector &strain, const Vector &rate)
  { return 0; }

const XC::Matrix &XC::J2PlateFibre::getTangent(void) const
  {
    double twoG= E/(1+nu);
    double G= 0.5*twoG;
    double C00= E/(1-nu*nu); double C11= C00;
    double C01= nu*C00; double C10= C01;

    double sig[order];
    const Vector Cepsilon= getStrain();
    sig[0]= C00*(Cepsilon(0)-epsPn[0]) + C01*(Cepsilon(1)-epsPn[1]);
    sig[1]= C01*(Cepsilon(0)-epsPn[0]) + C00*(Cepsilon(1)-epsPn[1]);
    sig[2]= G*(Cepsilon(2)-epsPn[2]);
    sig[3]= G*(Cepsilon(3)-epsPn[3]);
    sig[4]= G*(Cepsilon(4)-epsPn[4]);

    const double two3Hkin= two3*Hkin;

    double xsi[order];
    xsi[0]= sig[0] - two3Hkin*(2*epsPn[0]+epsPn[1]);
    xsi[1]= sig[1] - two3Hkin*(2*epsPn[1]+epsPn[0]);
    xsi[2]= sig[2] - one3*Hkin*epsPn[2];
    xsi[3]= sig[3] - one3*Hkin*epsPn[3];
    xsi[4]= sig[4] - one3*Hkin*epsPn[4];

    double q= sqrt(two3*(xsi[0]*xsi[0] + xsi[1]*xsi[1] - xsi[0]*xsi[1]) +
		    2.0*(xsi[2]*xsi[2] + xsi[3]*xsi[3] + xsi[4]*xsi[4]));
    const double F= q - root23*(sigmaY + Hiso*alphan);

    if(F < -100*DBL_EPSILON) {
      D.Zero();
      D(0,0)= C00; D(0,1)= C01;
      D(1,0)= C01; D(1,1)= C00;
      D(2,2)= G;
      D(3,3)= G;
      D(4,4)= G;

      epsPn1[0]= epsPn[0];
      epsPn1[1]= epsPn[1];
      epsPn1[2]= epsPn[2];
      epsPn1[3]= epsPn[3];
      epsPn1[4]= epsPn[4];
    }
    else {

      // Solve for dg
      double dg= 0.0;

      static Vector R(6);
      static Vector x(6);
      x(0)= xsi[0]; R(0)= 0.0;
      x(1)= xsi[1]; R(1)= 0.0;
      x(2)= xsi[2]; R(2)= 0.0;
      x(3)= xsi[3]; R(3)= 0.0;
      x(4)= xsi[4]; R(4)= 0.0;
      x(5)= dg;     R(5)= F;

      static Matrix J(6,6);
      static Vector dx(6);

      int iter= 0; int maxIter= 25;
      while(iter < maxIter && R.Norm() > 1.0e-14)
	{
	iter++;

	J(0,0)= 1.0 + dg*(two3*C00-one3*C01+two3Hkin); J(0,1)= dg*(two3*C01-one3*C00);
	J(1,0)= dg*(two3*C10-one3*C11); J(1,1)= 1.0 + dg*(two3*C11-one3*C10+two3Hkin);
	J(2,2)= 1.0 + dg*(twoG+two3Hkin);
	J(3,3)= 1.0 + dg*(twoG+two3Hkin);
	J(4,4)= 1.0 + dg*(twoG+two3Hkin);

	J(0,5)= (two3*C00-one3*C01+two3Hkin)*x(0) + (two3*C01-one3*C00)*x(1);
	J(1,5)= (two3*C10-one3*C11)*x(0) + (two3*C11-one3*C10+two3Hkin)*x(1);
	J(2,5)= (twoG+two3Hkin)*x(2);
	J(3,5)= (twoG+two3Hkin)*x(3);
	J(4,5)= (twoG+two3Hkin)*x(4);

	J(5,0)= (1.0-two3*Hiso*dg)*(two3*x(0)-one3*x(1))/q;
	J(5,1)= (1.0-two3*Hiso*dg)*(two3*x(1)-one3*x(0))/q;
	J(5,2)= (1.0-two3*Hiso*dg)*2.0*x(2)/q;
	J(5,3)= (1.0-two3*Hiso*dg)*2.0*x(3)/q;
	J(5,4)= (1.0-two3*Hiso*dg)*2.0*x(4)/q;

	J(5,5)= -two3*Hiso*q;

	J.Solve(R, dx);
	x.addVector(1.0, dx, -1.0);

	dg= x(5);
	dg_n1= dg;

	q= sqrt(two3*(x(0)*x(0) + x(1)*x(1) - x(0)*x(1)) + 2.0*(x(2)*x(2) + x(3)*x(3) + x(4)*x(4)));

	R(0)= x(0) - xsi[0] + dg*((two3*C00-one3*C01+two3Hkin)*x(0) + (two3*C01-one3*C00)*x(1));
	R(1)= x(1) - xsi[1] + dg*((two3*C10-one3*C11)*x(0) + (two3*C11-one3*C10+two3Hkin)*x(1));
	R(2)= x(2) - xsi[2] + dg*(twoG+two3Hkin)*x(2);
	R(3)= x(3) - xsi[3] + dg*(twoG+two3Hkin)*x(3);
	R(4)= x(4) - xsi[4] + dg*(twoG+two3Hkin)*x(4);
	R(5)= q - root23*(sigmaY + Hiso*(alphan+dg*root23*q));
      }

      if(iter == maxIter) {
	//std::cerr << getClassName() << "::" << __FUNCTION__ << "; maxIter reached " << R.Norm() << std::endl;
      }

      alphan1= alphan + dg*root23*q;

      epsPn1[0]= epsPn[0] + dg*(two3*x(0)-one3*x(1));
      epsPn1[1]= epsPn[1] + dg*(two3*x(1)-one3*x(0));
      epsPn1[2]= epsPn[2] + dg*2.0*x(2);
      epsPn1[3]= epsPn[3] + dg*2.0*x(3);
      epsPn1[4]= epsPn[4] + dg*2.0*x(4);

      double beta= 1.0+dg*two3Hkin;

      //J.Zero();
      J(0,0)= 1.0 + dg*(two3*C00-one3*C01)/beta; J(0,1)= dg*(two3*C01-one3*C00)/beta;
      J(1,0)= dg*(two3*C10-one3*C11)/beta; J(1,1)= 1.0 + dg*(two3*C11-one3*C10)/beta;
      J(2,2)= 1.0 + dg*twoG/beta;
      J(3,3)= 1.0 + dg*twoG/beta;
      J(4,4)= 1.0 + dg*twoG/beta;

      //J(0,5)= ((two3*C00-one3*C01) - dg*(two3*C00-one3*C01)/beta*two3Hkin)*x(0) + 
      //  ((two3*C01-one3*C00) - dg*(two3*C01-one3*C00)/beta*two3Hkin)*x(1);
      //J(1,5)= ((two3*C10-one3*C11) - dg*(two3*C10-one3*C11)/beta*two3Hkin)*x(0) + 
      //  ((two3*C11-one3*C10) - dg*(two3*C11-one3*C10)/beta*two3Hkin)*x(1);
      J(0,5)= (two3*C00-one3*C01)*(1.0-dg/beta*two3Hkin)*x(0) + (two3*C01-one3*C00)*(1.0-dg/beta*two3Hkin)*x(1);
      J(1,5)= (two3*C10-one3*C11)*(1.0-dg/beta*two3Hkin)*x(0) + (two3*C11-one3*C10)*(1.0-dg/beta*two3Hkin)*x(1);
      J(2,5)= (twoG - dg*twoG/beta*two3Hkin)*x(2);
      J(3,5)= (twoG - dg*twoG/beta*two3Hkin)*x(3);
      J(4,5)= (twoG - dg*twoG/beta*two3Hkin)*x(4);

      J(5,0)= (1.0-two3*Hiso*dg)*(two3*x(0)-one3*x(1))/q/beta;
      J(5,1)= (1.0-two3*Hiso*dg)*(two3*x(1)-one3*x(0))/q/beta;
      J(5,2)= (1.0-two3*Hiso*dg)*x(2)/q*2.0/beta;
      J(5,3)= (1.0-two3*Hiso*dg)*x(3)/q*2.0/beta;
      J(5,4)= (1.0-two3*Hiso*dg)*x(4)/q*2.0/beta;

      J(5,5)= -q*two3Hkin/beta - two3*Hiso*q;

      static Matrix invJ(6,6);
      J.Invert(invJ);

      D(0,0)= invJ(0,0)*C00 + invJ(0,1)*C10;
      D(1,0)= invJ(1,0)*C00 + invJ(1,1)*C10;
      D(2,0)= invJ(2,0)*C00 + invJ(2,1)*C10;
      D(3,0)= invJ(3,0)*C00 + invJ(3,1)*C10;
      D(4,0)= invJ(4,0)*C00 + invJ(4,1)*C10;

      D(0,1)= invJ(0,0)*C01 + invJ(0,1)*C11;
      D(1,1)= invJ(1,0)*C01 + invJ(1,1)*C11;
      D(2,1)= invJ(2,0)*C01 + invJ(2,1)*C11;
      D(3,1)= invJ(3,0)*C01 + invJ(3,1)*C11;
      D(4,1)= invJ(4,0)*C01 + invJ(4,1)*C11;

      D(0,2)= invJ(0,2)*G;
      D(1,2)= invJ(1,2)*G;
      D(2,2)= invJ(2,2)*G;
      D(3,2)= invJ(3,2)*G;
      D(4,2)= invJ(4,2)*G;

      D(0,3)= invJ(0,3)*G;
      D(1,3)= invJ(1,3)*G;
      D(2,3)= invJ(2,3)*G;
      D(3,3)= invJ(3,3)*G;
      D(4,3)= invJ(4,3)*G;

      D(0,4)= invJ(0,4)*G;
      D(1,4)= invJ(1,4)*G;
      D(2,4)= invJ(2,4)*G;
      D(3,4)= invJ(3,4)*G;
      D(4,4)= invJ(4,4)*G;
    }

    return D;
  }

const XC::Matrix &XC::J2PlateFibre::getInitialTangent(void) const
  {
    const double G= 0.5*E/(1+nu);
    const double C00= E/(1-nu*nu);
    const double C01= nu*C00;

    D.Zero();
    D(0,0)= C00; D(0,1)= C01;
    D(1,0)= C01; D(1,1)= C00;
    D(2,2)= G;
    D(3,3)= G;
    D(4,4)= G;

    return D;
  }

const XC::Vector &XC::J2PlateFibre::getStress(void) const
  {
    const double twoG= E/(1+nu);
    const double G= 0.5*twoG;
    const double C00= E/(1-nu*nu); double C11= C00;
    const double C01= nu*C00; double C10= C01;

    const Vector Cepsilon= getStrain();
    sigma(0)= C00*(Cepsilon(0)-epsPn[0]) + C01*(Cepsilon(1)-epsPn[1]);
    sigma(1)= C01*(Cepsilon(0)-epsPn[0]) + C00*(Cepsilon(1)-epsPn[1]);
    sigma(2)= G*(Cepsilon(2)-epsPn[2]);
    sigma(3)= G*(Cepsilon(3)-epsPn[3]);
    sigma(4)= G*(Cepsilon(4)-epsPn[4]);

    const double two3Hkin= two3*Hkin;

    double xsi[order];
    xsi[0]= sigma(0) - two3Hkin*(2*epsPn[0]+epsPn[1]);
    xsi[1]= sigma(1) - two3Hkin*(2*epsPn[1]+epsPn[0]);
    xsi[2]= sigma(2) - one3*Hkin*epsPn[2];
    xsi[3]= sigma(3) - one3*Hkin*epsPn[3];
    xsi[4]= sigma(4) - one3*Hkin*epsPn[4];

    double q= sqrt(two3*(xsi[0]*xsi[0] + xsi[1]*xsi[1] - xsi[0]*xsi[1]) +
		    2.0*(xsi[2]*xsi[2] + xsi[3]*xsi[3] + xsi[4]*xsi[4]));
    const double F= q - root23*(sigmaY + Hiso*alphan);

    if(F < -100*DBL_EPSILON)
      {
	epsPn1[0]= epsPn[0];
	epsPn1[1]= epsPn[1];
	epsPn1[2]= epsPn[2];
	epsPn1[3]= epsPn[3];
	epsPn1[4]= epsPn[4];
      }
    else
      {
      // Solve for dg
      double dg= 0.0;

      static Vector R(6);
      static Vector x(6);
      x(0)= xsi[0]; R(0)= 0.0;
      x(1)= xsi[1]; R(1)= 0.0;
      x(2)= xsi[2]; R(2)= 0.0;
      x(3)= xsi[3]; R(3)= 0.0;
      x(4)= xsi[4]; R(4)= 0.0;
      x(5)= dg;     R(5)= F;

      static Matrix J(6,6);
      static Vector dx(6);

      int iter= 0; int maxIter= 25;
      while(iter < maxIter && R.Norm() > 1.0e-14)
	{
	  iter++;

	  J(0,0)= 1.0 + dg*(two3*C00-one3*C01+two3Hkin); J(0,1)= dg*(two3*C01-one3*C00);
	  J(1,0)= dg*(two3*C10-one3*C11); J(1,1)= 1.0 + dg*(two3*C11-one3*C10+two3Hkin);
	  J(2,2)= 1.0 + dg*(twoG+two3Hkin);
	  J(3,3)= 1.0 + dg*(twoG+two3Hkin);
	  J(4,4)= 1.0 + dg*(twoG+two3Hkin);

	  J(0,5)= (two3*C00-one3*C01+two3Hkin)*x(0) + (two3*C01-one3*C00)*x(1);
	  J(1,5)= (two3*C10-one3*C11)*x(0) + (two3*C11-one3*C10+two3Hkin)*x(1);
	  J(2,5)= (twoG+two3Hkin)*x(2);
	  J(3,5)= (twoG+two3Hkin)*x(3);
	  J(4,5)= (twoG+two3Hkin)*x(4);

	  J(5,0)= (1.0-two3*Hiso*dg)*(two3*x(0)-one3*x(1))/q;
	  J(5,1)= (1.0-two3*Hiso*dg)*(two3*x(1)-one3*x(0))/q;
	  J(5,2)= (1.0-two3*Hiso*dg)*2.0*x(2)/q;
	  J(5,3)= (1.0-two3*Hiso*dg)*2.0*x(3)/q;
	  J(5,4)= (1.0-two3*Hiso*dg)*2.0*x(4)/q;

	  J(5,5)= -two3*Hiso*q;

	  J.Solve(R, dx);
	  x.addVector(1.0, dx, -1.0);

	  dg= x(5);
	  dg_n1= dg;

	  q= sqrt(two3*(x(0)*x(0) + x(1)*x(1) - x(0)*x(1)) + 2.0*(x(2)*x(2) + x(3)*x(3) + x(4)*x(4)));

	  R(0)= x(0) - xsi[0] + dg*((two3*C00-one3*C01+two3Hkin)*x(0) + (two3*C01-one3*C00)*x(1));
	  R(1)= x(1) - xsi[1] + dg*((two3*C10-one3*C11)*x(0) + (two3*C11-one3*C10+two3Hkin)*x(1));
	  R(2)= x(2) - xsi[2] + dg*(twoG+two3Hkin)*x(2);
	  R(3)= x(3) - xsi[3] + dg*(twoG+two3Hkin)*x(3);
	  R(4)= x(4) - xsi[4] + dg*(twoG+two3Hkin)*x(4);
	  R(5)= q - root23*(sigmaY + Hiso*(alphan+dg*root23*q));
	}
      if(iter == maxIter)
	{
	  //std::cerr << getClassName() << "::" << __FUNCTION__ << ";maxIter reached " << R.Norm() << std::endl;
        }

      alphan1= alphan + dg*root23*q;

      epsPn1[0]= epsPn[0] + dg*(two3*x(0)-one3*x(1));
      epsPn1[1]= epsPn[1] + dg*(two3*x(1)-one3*x(0));
      epsPn1[2]= epsPn[2] + dg*2.0*x(2);
      epsPn1[3]= epsPn[3] + dg*2.0*x(3);
      epsPn1[4]= epsPn[4] + dg*2.0*x(4);

      sigma(0)= x(0) + two3Hkin*(2.0*epsPn1[0]+epsPn1[1]);
      sigma(1)= x(1) + two3Hkin*(2.0*epsPn1[1]+epsPn1[0]);
      sigma(2)= x(2) + one3*Hkin*epsPn1[2];
      sigma(3)= x(3) + one3*Hkin*epsPn1[3];
      sigma(4)= x(4) + one3*Hkin*epsPn1[4];
    }

    return sigma;
  }

//! @brief return the Von Mises equivalent stress.
//!
//! <a href="https://en.wikipedia.org/wiki/Von_Mises_yield_criterion"> Von Mises yield criterion.</a>
double XC::J2PlateFibre::getVonMisesStress(void) const
  {
    double retval= 0.0;
    const Vector sg= getStress();
    const size_t sz= sg.Size();
    //NDmaterial stress order = 11, 22, 33, 12, 23, 31 
    //PlateFiberMaterial stress order= 11, 22, 12, 23, 31, (33) 
    if(sz==5) // plate fiber material
      {
	const double sg11= sg[0]; //11
	const double sg22= sg[1]; //22
	const double sg12= sg[2]; //12
	const double sg23= sg[3]; //23
	const double sg31= sg[4]; //31
	
	retval= sqrt(0.5*(pow(sg11-sg22,2)+(sg22*sg22)+(sg11*sg11)+6*(sg12*sg12+sg23+sg23+sg31+sg31)));
      }
    else
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ", wrong stress vector size (" << sz
	        << ")." << std::endl;
    return retval;
  }

const XC::Vector &XC::J2PlateFibre::getStrain(void) const
  {
    static Vector retval;
    retval= Tepsilon-Tepsilon0;
    return retval;
  }

int XC::J2PlateFibre::commitState(void)
  {
    epsPn[0]= epsPn1[0];
    epsPn[1]= epsPn1[1];
    epsPn[2]= epsPn1[2];
    epsPn[3]= epsPn1[3];
    epsPn[4]= epsPn1[4];

    alphan= alphan1;

    return 0;
  }

int XC::J2PlateFibre::revertToLastCommit(void)
  {
    epsPn1[0]= epsPn[0];
    epsPn1[1]= epsPn[1];
    epsPn1[2]= epsPn[2];
    epsPn1[3]= epsPn[3];
    epsPn1[4]= epsPn[4];
    alphan1= alphan;
    return 0;
  }

int XC::J2PlateFibre::revertToStart(void)
  {
    Tepsilon.Zero();
    init();
    dg_n1= 0.0;
    SHVs.Zero();
    return 0;
  }

XC::NDMaterial *XC::J2PlateFibre::getCopy(void) const
  { return new J2PlateFibre(*this); }

XC::NDMaterial *XC::J2PlateFibre::getCopy(const std::string &type) const
  {
    NDMaterial *retval= nullptr;
    if(type== this->getType())
      retval= this->getCopy();
    return retval;
  }

const std::string &XC::J2PlateFibre::getType(void) const
  { return strTypePlateFibre; }

int XC::J2PlateFibre::getOrder(void) const
  { return order; }

const XC::Vector &XC::J2PlateFibre::getStressSensitivity(int gradIndex, bool conditional)
  {
    sigma(0)= 0.0;
    sigma(1)= 0.0;
    sigma(2)= 0.0;
    sigma(3)= 0.0;
    sigma(4)= 0.0;

    const double twoG= E/(1+nu);
    const double G= 0.5*twoG;
    const double C00= E/(1-nu*nu); double C11= C00;
    const double C01= nu*C00; double C10= C01;

    double dEdh= 0.0;
    double dsigmaYdh= 0.0;
    double dHkindh= 0.0;
    double dHisodh= 0.0;
    double dGdh= 0.0;
    double dC00dh= 0.0;
    double dC01dh= 0.0;

    if(parameterID == 1) { // E
      dEdh= 1.0;
      dGdh= 0.5/(1+nu);
      //double C00= E/(1-nu*nu);
      //double C01= nu*C00;
      dC00dh= 1.0/(1-nu*nu);
      dC01dh= nu*dC00dh;
    }
    if(parameterID == 2) { // G(nu)
      dGdh= 1.0;
      dGdh= -0.5*E/(1.0 + 2*nu + nu*nu);
      //double C00= E/(1-nu*nu);
      //double C01= nu*C00;
      dC00dh= -E/((1-nu*nu)*(1-nu*nu))*(-2*nu);
      dC01dh= nu*dC00dh + C00;
    }
    if(parameterID == 5) {
      dsigmaYdh= 1.0;
    }
    if(parameterID == 6) {
      dHkindh= 1.0;
    }
    if(parameterID == 7) {
      dHisodh= 1.0;
    }

    double depsPdh[order]; 
    depsPdh[0]= 0.0; 
    depsPdh[1]= 0.0;
    depsPdh[2]= 0.0;
    depsPdh[3]= 0.0;
    depsPdh[4]= 0.0;
    double dalphadh= 0.0;
    if(!SHVs.isEmpty())
      {
	depsPdh[0]= (SHVs)(0,gradIndex);
	depsPdh[1]= (SHVs)(1,gradIndex);
	depsPdh[2]= (SHVs)(2,gradIndex);
	depsPdh[3]= (SHVs)(3,gradIndex);
	depsPdh[4]= (SHVs)(4,gradIndex);
	dalphadh= (SHVs)(5,gradIndex);
      }

    double two3Hkin= two3*Hkin;

    double xsi[order];
    const Vector Cepsilon= getStrain();
    xsi[0]= C00*(Cepsilon(0)-epsPn[0]) + C01*(Cepsilon(1)-epsPn[1]);
    xsi[1]= C01*(Cepsilon(0)-epsPn[0]) + C00*(Cepsilon(1)-epsPn[1]);
    xsi[2]= G*(Cepsilon(2)-epsPn[2]);
    xsi[3]= G*(Cepsilon(3)-epsPn[3]);
    xsi[4]= G*(Cepsilon(4)-epsPn[4]);

    xsi[0] -= two3Hkin*(2*epsPn[0]+epsPn[1]); 
    xsi[1] -= two3Hkin*(2*epsPn[1]+epsPn[0]); 
    xsi[2] -= one3*Hkin*epsPn[2]; 
    xsi[3] -= one3*Hkin*epsPn[3]; 
    xsi[4] -= one3*Hkin*epsPn[4];

    double dbetadh[order]; 
    dbetadh[0]= two3*dHkindh*(2*epsPn[0]+epsPn[1]) + two3Hkin*(2*depsPdh[0]+depsPdh[1]);
    dbetadh[1]= two3*dHkindh*(2*epsPn[1]+epsPn[0]) + two3Hkin*(2*depsPdh[1]+depsPdh[0]);
    dbetadh[2]= one3*(dHkindh*epsPn[2] + Hkin*depsPdh[2]);
    dbetadh[3]= one3*(dHkindh*epsPn[3] + Hkin*depsPdh[3]);
    dbetadh[4]= one3*(dHkindh*epsPn[4] + Hkin*depsPdh[4]);

    double dxsidh[order];
    dxsidh[0]= -C00*depsPdh[0] - C01*depsPdh[1] + dC00dh*(Cepsilon(0)-epsPn[0]) + dC01dh*(Cepsilon(1)-epsPn[1]) - dbetadh[0];
    dxsidh[1]= -C01*depsPdh[0] - C00*depsPdh[1] + dC01dh*(Cepsilon(0)-epsPn[0]) + dC00dh*(Cepsilon(1)-epsPn[1]) - dbetadh[1];
    dxsidh[2]= -G*depsPdh[2] + dGdh*(Cepsilon(2)-epsPn[2]) - dbetadh[2];
    dxsidh[3]= -G*depsPdh[3] + dGdh*(Cepsilon(3)-epsPn[3]) - dbetadh[3];
    dxsidh[4]= -G*depsPdh[4] + dGdh*(Cepsilon(4)-epsPn[4]) - dbetadh[4];

    double q= two3*(xsi[0]*xsi[0] + xsi[1]*xsi[1] - xsi[0]*xsi[1]) + 
      2.0*(xsi[2]*xsi[2] + xsi[3]*xsi[3] + xsi[4]*xsi[4]);
    double F= q - root23*(sigmaY + Hiso*alphan1);

    if(F <= -100*DBL_EPSILON) {
      sigma(0)= dC00dh*(Cepsilon(0)-epsPn[0]) + dC01dh*(Cepsilon(1)-epsPn[1]) - C00*depsPdh[0] - C01*depsPdh[1];
      sigma(1)= dC01dh*(Cepsilon(0)-epsPn[0]) + dC00dh*(Cepsilon(1)-epsPn[1]) - C01*depsPdh[0] - C00*depsPdh[1];
      sigma(2)= dGdh*(Cepsilon(2)-epsPn1[2]) - G*depsPdh[2];
      sigma(3)= dGdh*(Cepsilon(3)-epsPn1[3]) - G*depsPdh[3];
      sigma(4)= dGdh*(Cepsilon(4)-epsPn1[4]) - G*depsPdh[4];
    }
    else {
      static Matrix J(6,6);
      static Vector b(6);
      static Vector dx(6);

      double dg= dg_n1;

      J(0,0)= 1.0 + dg*(two3*C00-one3*C01+two3Hkin); J(0,1)= dg*(two3*C01-one3*C00);
      J(1,0)= dg*(two3*C10-one3*C11); J(1,1)= 1.0 + dg*(two3*C11-one3*C10+two3Hkin);
      J(2,2)= 1.0 + dg*(twoG+two3Hkin);
      J(3,3)= 1.0 + dg*(twoG+two3Hkin);
      J(4,4)= 1.0 + dg*(twoG+two3Hkin);

      J(0,5)= (two3*C00-one3*C01+two3Hkin)*xsi[0] + (two3*C01-one3*C00)*xsi[1];
      J(1,5)= (two3*C10-one3*C11)*xsi[0] + (two3*C11-one3*C10+two3Hkin)*xsi[1];
      J(2,5)= (twoG+two3Hkin)*xsi[2];
      J(3,5)= (twoG+two3Hkin)*xsi[3];
      J(4,5)= (twoG+two3Hkin)*xsi[4];

      J(5,0)= (1.0-two3*Hiso*dg)*(two3*xsi[0]-one3*xsi[1])/q;
      J(5,1)= (1.0-two3*Hiso*dg)*(two3*xsi[1]-one3*xsi[0])/q;
      J(5,2)= (1.0-two3*Hiso*dg)*2.0*xsi[2]/q;
      J(5,3)= (1.0-two3*Hiso*dg)*2.0*xsi[3]/q;
      J(5,4)= (1.0-two3*Hiso*dg)*2.0*xsi[4]/q;

      J(5,5)= -two3*Hiso*q;

      J.Solve(b, dx);
    }

    return sigma;
  }

int XC::J2PlateFibre::commitSensitivity(const Vector &depsdh, int gradIndex, int numGrads)
  {
    if(SHVs.isEmpty())
      { SHVs= Matrix(6,numGrads); }

    if(gradIndex >= SHVs.noCols())
      {
	//std::cerr << getClassName() << "::" << __FUNCTION__ << gradIndex << ' ' << SHVs.noCols() << std::endl;
	return 0;
      }

    const double twoG= E/(1+nu);
    const double G= 0.5*twoG;
    const double C00= E/(1-nu*nu); double C11= C00;
    const double C01= nu*C00; double C10= C01;

    double dEdh= 0.0;
    double dsigmaYdh= 0.0;
    double dHkindh= 0.0;
    double dHisodh= 0.0;
    double dGdh= 0.0;
    double dC00dh= 0.0;
    double dC01dh= 0.0;

    if(parameterID == 1) { // E
      dEdh= 1.0;
      dGdh= 0.5/(1+nu);
      //double C00= E/(1-nu*nu);
      //double C01= nu*C00;
      dC00dh= 1.0/(1-nu*nu);
      dC01dh= nu*dC00dh;
    }
    if(parameterID == 2) { // G(nu)
      dGdh= 1.0;
      dGdh= -0.5*E/(1.0 + 2*nu + nu*nu);
      //double C00= E/(1-nu*nu);
      //double C01= nu*C00;
      dC00dh= -E/((1-nu*nu)*(1-nu*nu))*(-2*nu);
      dC01dh= nu*dC00dh + C00;
    }
    if(parameterID == 5) {
      dsigmaYdh= 1.0;
    }
    if(parameterID == 6) {
      dHkindh= 1.0;
    }
    if(parameterID == 7) {
      dHisodh= 1.0;
    }

    double depsPdh[order];
    depsPdh[0]= 0.0;
    depsPdh[1]= 0.0;
    depsPdh[2]= 0.0;
    depsPdh[3]= 0.0;
    depsPdh[4]= 0.0;
    double dalphadh= 0.0;
    if(!SHVs.isEmpty())
      {
	depsPdh[0]= (SHVs)(0,gradIndex);
	depsPdh[1]= (SHVs)(1,gradIndex);
	depsPdh[2]= (SHVs)(2,gradIndex);
	depsPdh[3]= (SHVs)(3,gradIndex);
	depsPdh[4]= (SHVs)(4,gradIndex);
	dalphadh= (SHVs)(5,gradIndex);
      }

    double two3Hkin= two3*Hkin;

    double xsi[order];
    const Vector Cepsilon= getStrain();
    xsi[0]= C00*(Cepsilon(0)-epsPn[0]) + C01*(Cepsilon(1)-epsPn[1]);
    xsi[1]= C01*(Cepsilon(0)-epsPn[0]) + C00*(Cepsilon(1)-epsPn[1]);
    xsi[2]= G*(Cepsilon(2)-epsPn[2]);
    xsi[3]= G*(Cepsilon(3)-epsPn[3]);
    xsi[4]= G*(Cepsilon(4)-epsPn[4]);

    xsi[0] -= two3Hkin*(2*epsPn[0]+epsPn[1]);
    xsi[1] -= two3Hkin*(2*epsPn[1]+epsPn[0]);
    xsi[2] -= one3*Hkin*epsPn[2];
    xsi[3] -= one3*Hkin*epsPn[3];
    xsi[4] -= one3*Hkin*epsPn[4];

    double q= two3*(xsi[0]*xsi[0] + xsi[1]*xsi[1] - xsi[0]*xsi[1]) +
      2.0*(xsi[2]*xsi[2] + xsi[3]*xsi[3] + xsi[4]*xsi[4]); 
    double F= q - root23*(sigmaY + Hiso*alphan1);

    if(F <= -100*DBL_EPSILON) {
      // Do nothing
    }
    else {
      static Matrix J(6,6);
      static Vector b(6);
      static Vector dx(6);

      double dg= dg_n1;

      J(0,0)= 1.0 + dg*(two3*C00-one3*C01+two3Hkin); J(0,1)= dg*(two3*C01-one3*C00);
      J(1,0)= dg*(two3*C10-one3*C11); J(1,1)= 1.0 + dg*(two3*C11-one3*C10+two3Hkin);
      J(2,2)= 1.0 + dg*(twoG+two3Hkin);
      J(3,3)= 1.0 + dg*(twoG+two3Hkin);
      J(4,4)= 1.0 + dg*(twoG+two3Hkin);

      J(0,5)= (two3*C00-one3*C01+two3Hkin)*xsi[0] + (two3*C01-one3*C00)*xsi[1];
      J(1,5)= (two3*C10-one3*C11)*xsi[0] + (two3*C11-one3*C10+two3Hkin)*xsi[1];
      J(2,5)= (twoG+two3Hkin)*xsi[2];
      J(3,5)= (twoG+two3Hkin)*xsi[3];
      J(4,5)= (twoG+two3Hkin)*xsi[4];

      J(5,0)= (1.0-two3*Hiso*dg)*(two3*xsi[0]-one3*xsi[1])/q;
      J(5,1)= (1.0-two3*Hiso*dg)*(two3*xsi[1]-one3*xsi[0])/q;
      J(5,2)= (1.0-two3*Hiso*dg)*2.0*xsi[2]/q;
      J(5,3)= (1.0-two3*Hiso*dg)*2.0*xsi[3]/q;
      J(5,4)= (1.0-two3*Hiso*dg)*2.0*xsi[4]/q;

      J(5,5)= -two3*Hiso*q;

      J.Solve(b, dx);
    }

    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::J2PlateFibre::sendData(Communicator &comm)
  {
    int res= NDMaterial::sendData(comm);
    res+= comm.sendDoubles(E,nu,sigmaY,Hiso,Hkin,getDbTagData(),CommMetaData(1));
    res+= comm.sendInt(parameterID,getDbTagData(),CommMetaData(2));
    res+= comm.sendMatrix(SHVs,getDbTagData(),CommMetaData(3));
    res+= comm.sendVector(Tepsilon,getDbTagData(),CommMetaData(4));
    res+= comm.sendVector(Tepsilon0,getDbTagData(),CommMetaData(5));
    res+= comm.sendDoubles(alphan, alphan1, dg_n1,getDbTagData(),CommMetaData(6));
    size_t conta= 7;
    for(size_t i=0;i<order;i++)
      res+= comm.sendDoubles(epsPn[i],epsPn1[i],getDbTagData(),CommMetaData(conta++));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::J2PlateFibre::recvData(const Communicator &comm)
  {
    int res= NDMaterial::recvData(comm);
    res+= comm.receiveDoubles(E,nu,sigmaY,Hiso,Hkin,getDbTagData(),CommMetaData(1));
    res+= comm.receiveInt(parameterID,getDbTagData(),CommMetaData(2));
    res+= comm.receiveMatrix(SHVs,getDbTagData(),CommMetaData(3));
    res+= comm.receiveVector(Tepsilon,getDbTagData(),CommMetaData(4));
    res+= comm.receiveVector(Tepsilon0,getDbTagData(),CommMetaData(5));
    res+= comm.receiveDoubles(alphan, alphan1, getDbTagData(),CommMetaData(6));
    size_t conta= 7;
    for(size_t i=0;i<order;i++)
      res+= comm.receiveDoubles(epsPn[i],epsPn1[i],getDbTagData(),CommMetaData(conta++));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::J2PlateFibre::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(7+order);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::J2PlateFibre::recvSelf(const Communicator &comm)
  {
    inicComm(7+order);
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

void XC::J2PlateFibre::Print(std::ostream &s, int flag)
  {
    s << "J2 Plate Fibre Material Model" << std::endl;
    s << "\tE:  " << E << std::endl;
    s << "\tnu:  " << nu << std::endl;
    s << "\tsigmaY:  " << sigmaY << std::endl;
    s << "\tHiso:  " << Hiso << std::endl;
    s << "\tHkin:  " << Hkin << std::endl;

    return;
  }

int XC::J2PlateFibre::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    if(argv[0]=="E")
      retval= param.addObject(1, this);
    else if(argv[0]=="nu")
      retval= param.addObject(2, this);  
    else if((argv[0]=="sigmaY") == 0 || (argv[0]=="fy"))
      retval= param.addObject(5, this);
    else if(argv[0]=="Hkin")
      retval= param.addObject(6, this);
    else if(argv[0]=="Hiso")
      retval= param.addObject(7, this);
    return retval;
  }

int XC::J2PlateFibre::updateParameter(int parameterID, Information &info)
  { 
    switch(parameterID)
      {
      case 1:
	E= info.theDouble;
	return 0;
      case 2:
	nu= info.theDouble;
	return 0;
      case 5:
	sigmaY= info.theDouble;
	return 0;
      case 6:
	Hkin= info.theDouble;
	return 0;
      case 7:
	Hiso= info.theDouble;
	return 0;
      default:
	return -1;
      }
  }

int XC::J2PlateFibre::activateParameter(int paramID)
  {
    parameterID= paramID;
    return 0;
  }
