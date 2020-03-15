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
                                                                        
// $Revision: 1.12 $
// $Date: 2004/10/20 20:04:43 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/HystereticMaterial.cpp,v $

// Written: MHS
// Created: July 2000
//
// Description: This file contains the implementation of 
// HystereticMaterial.  HystereticMaterial is
// a one-dimensional hysteretic model with pinching of both
// force and deformation, damage due to deformation and energy, and
// degraded unloading stiffness based on maximum ductility.  This
// is a modified implementation of Hyster2.f90 by Filippou.
#include <cstdlib>
#include <material/uniaxial/HystereticMaterial.h>
#include <cmath>
#include <cfloat>
#include "utility/matrix/Vector.h"


XC::HystereticMaterial::HystereticMaterial(int tag,
                        double m1p, double r1p, double m2p, double r2p, double m3p, double r3p,
                        double m1n, double r1n, double m2n, double r2n, double m3n, double r3n,
                        double px, double py, double d1, double d2, double b):
 XC::UniaxialMaterial(tag, MAT_TAG_Hysteretic),
pinchX(px), pinchY(py), damfc1(d1), damfc2(d2), beta(b),
mom1p(m1p), rot1p(r1p), mom2p(m2p), rot2p(r2p), mom3p(m3p), rot3p(r3p),
mom1n(m1n), rot1n(r1n), mom2n(m2n), rot2n(r2n), mom3n(m3n), rot3n(r3n)
  {
    bool error = false;
       
    // Positive backbone parameters
    if(rot1p <= 0.0) error = true;
    if(rot2p <= rot1p) error = true;
    if(rot3p <= rot2p) error = true;

    // Negative backbone parameters
    if(rot1n >= 0.0) error = true;
    if(rot2n >= rot1n) error = true;
    if(rot3n >= rot2n) error = true;
        
    if(error)
      {
        std::cerr << "XC::HystereticMaterial::HystereticMaterial -- input backbone is not unique (one-to-one)\n";
        exit(-1);
      }                

    energyA = 0.5 * (rot1p*mom1p + (rot2p-rot1p)*(mom2p+mom1p) + (rot3p-rot2p)*(mom3p+mom2p) +
                rot1n*mom1n + (rot2n-rot1n)*(mom2n+mom1n) + (rot3n-rot2n)*(mom3n+mom2n));

    // Set envelope slopes
    this->setEnvelope();

    // Initialize history variables
    this->revertToStart();
    this->revertToLastCommit();
  }

XC::HystereticMaterial::HystereticMaterial(int tag,
                        double m1p, double r1p, double m2p, double r2p,
                        double m1n, double r1n, double m2n, double r2n,
                        double px, double py, double d1, double d2, double b):
 XC::UniaxialMaterial(tag, MAT_TAG_Hysteretic),
pinchX(px), pinchY(py), damfc1(d1), damfc2(d2), beta(b),
mom1p(m1p), rot1p(r1p), mom3p(m2p), rot3p(r2p),
mom1n(m1n), rot1n(r1n), mom3n(m2n), rot3n(r2n)
  {
    bool error = false;
        
    // Positive backbone parameters
    if(rot1p <= 0.0) error = true;
    if(rot3p <= rot1p) error = true;

    // Negative backbone parameters
    if(rot1n >= 0.0) error = true;
    if(rot3n >= rot1n) error = true;

    if(error)
      {
        std::cerr << "XC::HystereticMaterial::HystereticMaterial -- input backbone is not unique (one-to-one)\n";
        exit(-1);
      }

                                      
    energyA = 0.5 * (rot1p*mom1p + (rot3p-rot1p)*(mom3p+mom1p) +
             rot1n*mom1n + (rot3n-rot1n)*(mom3n+mom1n));

    mom2p = 0.5*(mom1p+mom3p);
    mom2n = 0.5*(mom1n+mom3n);

    rot2p = 0.5*(rot1p+rot3p);
    rot2n = 0.5*(rot1n+rot3n);

    // Set envelope slopes
    this->setEnvelope();

    // Initialize history variables
    this->revertToStart();
    this->revertToLastCommit();
  }

XC::HystereticMaterial::HystereticMaterial(int tag)
  : UniaxialMaterial(tag, MAT_TAG_Hysteretic),
pinchX(0.0), pinchY(0.0), damfc1(0.0), damfc2(0.0), beta(0.0),
mom1p(0.0), rot1p(0.0), mom2p(0.0), rot2p(0.0), mom3p(0.0), rot3p(0.0),
mom1n(0.0), rot1n(0.0), mom2n(0.0), rot2n(0.0), mom3n(0.0), rot3n(0.0)
  {}

XC::HystereticMaterial::HystereticMaterial()
  : UniaxialMaterial(0, MAT_TAG_Hysteretic),
pinchX(0.0), pinchY(0.0), damfc1(0.0), damfc2(0.0), beta(0.0),
mom1p(0.0), rot1p(0.0), mom2p(0.0), rot2p(0.0), mom3p(0.0), rot3p(0.0),
mom1n(0.0), rot1n(0.0), mom2n(0.0), rot2n(0.0), mom3n(0.0), rot3n(0.0)
  {}

int XC::HystereticMaterial::setTrialStrain(double strain, double strainRate)
  {
    TrotMax = CrotMax;
    TrotMin = CrotMin;
    TenergyD = CenergyD;
    TrotPu = CrotPu;
    TrotNu = CrotNu;

        trial.Strain() = strain;
        double dStrain = trial.getStrain() - converged.getStrain();

        TloadIndicator = CloadIndicator;
        
        if(TloadIndicator == 0)
                TloadIndicator = (dStrain < 0.0) ? 2 : 1;

        if(trial.getStrain() >= CrotMax) {
                TrotMax = trial.getStrain();
                trial.Tangent()= posEnvlpTangent(trial.getStrain());
                trial.Stress()= posEnvlpStress(trial.getStrain());
        }
        else if(trial.getStrain() <= CrotMin) {
                TrotMin = trial.getStrain();
                trial.Tangent()= negEnvlpTangent(trial.getStrain());
                trial.Stress()= negEnvlpStress(trial.getStrain());
        }
        else {
          if(dStrain < 0.0)
            negativeIncrement(dStrain);
          else if(dStrain > 0.0)
            positiveIncrement(dStrain);
        }

        TenergyD = CenergyD + 0.5*(converged.getStress()+trial.getStress())*dStrain;

        return 0;
}


double XC::HystereticMaterial::getStrain(void) const
  { return trial.getStrain(); }

double XC::HystereticMaterial::getStress(void) const
  { return trial.getStress(); }

double XC::HystereticMaterial::getTangent(void) const
  { return trial.getTangent(); }

void XC::HystereticMaterial::positiveIncrement(double dStrain)
  {
        double kn = pow(CrotMin/rot1n,beta);
        kn = (kn < 1.0) ? 1.0 : 1.0/kn;
        double kp = pow(CrotMax/rot1p,beta);
        kp = (kp < 1.0) ? 1.0 : 1.0/kp;

        if(TloadIndicator == 2) {
                TloadIndicator = 1;
                if(converged.getStress() <= 0.0) {
                        TrotNu = converged.getStrain() - converged.getStress()/(E1n*kn);
                        double energy = CenergyD - 0.5*converged.getStress()/(E1n*kn)*converged.getStress();
                        double damfc = 0.0;
                        if(CrotMin < rot1n) {
                                damfc = damfc2*energy/energyA;
                                damfc += damfc1*(CrotMin-rot1n)/rot1n;
                        }

                        TrotMax = CrotMax*(1.0+damfc);
                }
        }

  TloadIndicator = 1;

        TrotMax = (TrotMax > rot1p) ? TrotMax : rot1p;

        double maxmom = posEnvlpStress(TrotMax);
        double rotlim = negEnvlpRotlim(CrotMin);
        double rotrel = (rotlim > TrotNu) ? rotlim : TrotNu;
        rotrel = TrotNu;
        if(negEnvlpStress(CrotMin) >= 0.0)
          rotrel = rotlim;

        double rotmp1 = rotrel + pinchY*(TrotMax-rotrel);
        double rotmp2 = TrotMax - (1.0-pinchY)*maxmom/(E1p*kp);
        double rotch = rotmp1 + (rotmp2-rotmp1)*pinchX;

        double tmpmo1;
        double tmpmo2;

        if(trial.getStrain() < TrotNu) {
                trial.Tangent()= E1n*kn;
                trial.Stress() = converged.getStress() + trial.getTangent()*dStrain;
                if(trial.getStress() >= 0.0)
                  {
                        trial.Stress() = 0.0;
                        trial.Tangent()= E1n*1.0e-9;
                }
        }

        else if(trial.getStrain() >= TrotNu && trial.getStrain() < rotch) {
                if(trial.getStrain() <= rotrel) {
                        trial.Stress()= 0.0;
                        trial.Tangent()= E1p*1.0e-9;
                }
                else {
                        trial.Tangent()= maxmom*pinchY/(rotch-rotrel);
                        tmpmo1 = converged.getStress() + E1p*kp*dStrain;
                        tmpmo2 = (trial.getStrain()-rotrel)*trial.getTangent();
                        if(tmpmo1 < tmpmo2) {
                                trial.Stress()= tmpmo1;
                                trial.Tangent()= E1p*kp;
                        }
                        else
                                trial.Stress()= tmpmo2;
                }
        }

        else {
                trial.Tangent()= (1.0-pinchY)*maxmom/(TrotMax-rotch);
                tmpmo1 = converged.getStress() + E1p*kp*dStrain;
                tmpmo2 = pinchY*maxmom + (trial.getStrain()-rotch)*trial.getTangent();
                if(tmpmo1 < tmpmo2) {
                        trial.Stress()= tmpmo1;
                        trial.Tangent()= E1p*kp;
                }
                else
                        trial.Stress()= tmpmo2;
        }
}

void XC::HystereticMaterial::negativeIncrement(double dStrain)
{
        double kn = pow(CrotMin/rot1n,beta);
        kn = (kn < 1.0) ? 1.0 : 1.0/kn;
        double kp = pow(CrotMax/rot1p,beta);
        kp = (kp < 1.0) ? 1.0 : 1.0/kp;

        if(TloadIndicator == 1) {
                TloadIndicator = 2;
                if(converged.getStress() >= 0.0) {
                        TrotPu = converged.getStrain() - converged.getStress()/(E1p*kp);
                        double energy = CenergyD - 0.5*converged.getStress()/(E1p*kp)*converged.getStress();
                        double damfc = 0.0;
                        if(CrotMax > rot1p) {
                                damfc = damfc2*energy/energyA;
                                damfc += damfc1*(CrotMax-rot1p)/rot1p;
                        }

                        TrotMin = CrotMin*(1.0+damfc);
                }
        }

  TloadIndicator = 2;

        TrotMin = (TrotMin < rot1n) ? TrotMin : rot1n;

        double minmom = negEnvlpStress(TrotMin);
        double rotlim = posEnvlpRotlim(CrotMax);
        double rotrel = (rotlim < TrotPu) ? rotlim : TrotPu;
        rotrel = TrotPu;
        if(posEnvlpStress(CrotMax) <= 0.0)
          rotrel = rotlim;

        double rotmp1 = rotrel + pinchY*(TrotMin-rotrel);
        double rotmp2 = TrotMin - (1.0-pinchY)*minmom/(E1n*kn);
        double rotch = rotmp1 + (rotmp2-rotmp1)*pinchX;

        double tmpmo1;
        double tmpmo2;

        if(trial.getStrain() > TrotPu) {
                trial.Tangent()= E1p*kp;
                trial.Stress()= converged.getStress() + trial.getTangent()*dStrain;
                if(trial.getStress() <= 0.0) {
                        trial.Stress()= 0.0;
                        trial.Tangent()= E1p*1.0e-9;
                }
        }

        else if(trial.getStrain() <= TrotPu && trial.getStrain() > rotch) {
                if(trial.getStrain() >= rotrel) {
                        trial.Stress()= 0.0;
                        trial.Tangent()= E1n*1.0e-9;
                }
                else {
                        trial.Tangent()= minmom*pinchY/(rotch-rotrel);
                        tmpmo1 = converged.getStress() + E1n*kn*dStrain;
                        tmpmo2 = (trial.getStrain()-rotrel)*trial.getTangent();
                        if(tmpmo1 > tmpmo2) {
                                trial.Stress()= tmpmo1;
                                trial.Tangent()= E1n*kn;
                        }
                        else
                                trial.Stress()= tmpmo2;
                }
        }

        else {
                trial.Tangent()= (1.0-pinchY)*minmom/(TrotMin-rotch);
                tmpmo1 = converged.getStress() + E1n*kn*dStrain;
                tmpmo2 = pinchY*minmom + (trial.getStrain()-rotch)*trial.getTangent();
                if(tmpmo1 > tmpmo2) {
                        trial.Stress()= tmpmo1;
                        trial.Tangent()= E1n*kn;
                }
                else
                        trial.Stress()= tmpmo2;
        }
}

int XC::HystereticMaterial::commitState(void)
  {
    CrotMax = TrotMax;
    CrotMin = TrotMin;
    CrotPu = TrotPu;
    CrotNu = TrotNu;
    CenergyD = TenergyD;
    CloadIndicator = TloadIndicator;
        
    converged= trial;
    return 0;
  }

int XC::HystereticMaterial::revertToLastCommit(void)
  {
    TrotMax = CrotMax;
    TrotMin = CrotMin;
    TrotPu = CrotPu;
    TrotNu = CrotNu;
    TenergyD = CenergyD;
    TloadIndicator = CloadIndicator;

    trial= converged;
    return 0;
  }

int XC::HystereticMaterial::revertToStart(void)
  {
    CrotMax = 0.0;
    CrotMin = 0.0;
    CrotPu = 0.0;
    CrotNu = 0.0;
    CenergyD = 0.0;
    CloadIndicator = 0;

    converged.revertToStart(E1p);
    trial.revertToStart(E1p);
    return 0;
  }

XC::UniaxialMaterial *XC::HystereticMaterial::getCopy(void) const
  { return new HystereticMaterial(*this); }

//! @brief Send object members through the channel being passed as parameter.
int XC::HystereticMaterial::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendDoubles(pinchX,pinchY,damfc1,damfc2,beta,energyA,getDbTagData(),CommMetaData(2));
    res+= cp.sendDoubles(mom1p,rot1p,mom2p,rot2p,mom3p,rot3p,getDbTagData(),CommMetaData(3));
    res+= cp.sendDoubles(mom1n,rot1n,mom2n,rot2n,mom3n,rot3n,getDbTagData(),CommMetaData(4));
    res+= cp.sendDoubles(CrotMax,CrotMin,CrotPu,CrotNu,CenergyD,getDbTagData(),CommMetaData(5));
    res+= cp.sendDoubles(TrotMax,TrotMin,TrotPu,TrotNu,TenergyD,getDbTagData(),CommMetaData(6));
    res+= cp.sendMovable(converged,getDbTagData(),CommMetaData(7));
    res+= cp.sendMovable(trial,getDbTagData(),CommMetaData(8));
    res+= cp.sendDoubles(E1p,E1n,E2p,E2n,E3p,E3n,getDbTagData(),CommMetaData(9));
    res+= cp.sendInts(CloadIndicator,TloadIndicator,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::HystereticMaterial::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveDoubles(pinchX,pinchY,damfc1,damfc2,beta,energyA,getDbTagData(),CommMetaData(2));
    res+= cp.receiveDoubles(mom1p,rot1p,mom2p,rot2p,mom3p,rot3p,getDbTagData(),CommMetaData(3));
    res+= cp.receiveDoubles(mom1n,rot1n,mom2n,rot2n,mom3n,rot3n,getDbTagData(),CommMetaData(4));
    res+= cp.receiveDoubles(CrotMax,CrotMin,CrotPu,CrotNu,CenergyD,getDbTagData(),CommMetaData(5));
    res+= cp.receiveDoubles(TrotMax,TrotMin,TrotPu,TrotNu,TenergyD,getDbTagData(),CommMetaData(6));
    res+= cp.receiveMovable(converged,getDbTagData(),CommMetaData(7));
    res+= cp.receiveMovable(trial,getDbTagData(),CommMetaData(8));
    res+= cp.receiveDoubles(E1p,E1n,E2p,E2n,E3p,E3n,getDbTagData(),CommMetaData(9));
    res+= cp.receiveInts(CloadIndicator,TloadIndicator,getDbTagData(),CommMetaData(10));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::HystereticMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(12); 
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0) 
      std::cerr << "HystereticMaterial::sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::HystereticMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(12);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "HystereticMaterial::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << "HystereticMaterial::recvSelf - failed to receive data.\n";
      }
    return res;
  }
    
void XC::HystereticMaterial::Print(std::ostream &s, int flag) const
{
        s << "Hysteretic XC::Material, tag: " << this->getTag() << std::endl;
        s << "mom1p: " << mom1p << std::endl;
        s << "rot1p: " << rot1p << std::endl;
        s << "E1p: " << E1p << std::endl;
        s << "mom2p: " << mom2p << std::endl;
        s << "rot2p: " << rot2p << std::endl;
        s << "E2p: " << E2p << std::endl;
        s << "mom3p: " << mom3p << std::endl;
        s << "rot3p: " << rot3p << std::endl;
        s << "E3p: " << E3p << std::endl;

        s << "mom1n: " << mom1n << std::endl;
        s << "rot1n: " << rot1n << std::endl;
        s << "E1n: " << E1n << std::endl;
        s << "mom2n: " << mom2n << std::endl;
        s << "rot2n: " << rot2n << std::endl;
        s << "E2n: " << E2n << std::endl;
        s << "mom3n: " << mom3n << std::endl;
        s << "rot3n: " << rot3n << std::endl;
        s << "E3n: " << E3n << std::endl;

        s << "pinchX: " << pinchX << std::endl;
        s << "pinchY: " << pinchY << std::endl;
        s << "damfc1: " << damfc1 << std::endl;
        s << "damfc2: " << damfc2 << std::endl;
        s << "energyA: " << energyA << std::endl;
        s << "beta: " << beta << std::endl;
}

void XC::HystereticMaterial::setEnvelope(void)
  {
    E1p = mom1p/rot1p;
    E2p = (mom2p-mom1p)/(rot2p-rot1p);
    E3p = (mom3p-mom2p)/(rot3p-rot2p);

    E1n = mom1n/rot1n;
    E2n = (mom2n-mom1n)/(rot2n-rot1n);
    E3n = (mom3n-mom2n)/(rot3n-rot2n);
  }

double XC::HystereticMaterial::posEnvlpStress(double strain)
  {
        if(strain <= 0.0)
                return 0.0;
        else if(strain <= rot1p)
                return E1p*strain;
        else if(strain <= rot2p)
                return mom1p + E2p*(strain-rot1p);
        else if(strain <= rot3p || E3p > 0.0)
                return mom2p + E3p*(strain-rot2p);
        else
                return mom3p;
  }

double XC::HystereticMaterial::negEnvlpStress(double strain)
  {
        if(strain >= 0.0)
                return 0.0;
        else if(strain >= rot1n)
                return E1n*strain;
        else if(strain >= rot2n)
                return mom1n + E2n*(strain-rot1n);
        else if(strain >= rot3n || E3n > 0.0)
                return mom2n + E3n*(strain-rot2n);
        else
                return mom3n;
  }

double XC::HystereticMaterial::posEnvlpTangent(double strain)
  {
        if(strain < 0.0)
                return E1p*1.0e-9;
        else if(strain <= rot1p)
                return E1p;
        else if(strain <= rot2p)
                return E2p;
        else if(strain <= rot3p || E3p > 0.0)
                return E3p;
        else
                return E1p*1.0e-9;
  }

double XC::HystereticMaterial::negEnvlpTangent(double strain)
  {
        if(strain > 0.0)
                return E1n*1.0e-9;
        else if(strain >= rot1n)
                return E1n;
        else if(strain >= rot2n)
                return E2n;
        else if(strain >= rot3n || E3n > 0.0)
                return E3n;
        else
                return E1n*1.0e-9;
  }

double XC::HystereticMaterial::posEnvlpRotlim(double strain)
  {
  double strainLimit = POS_INF_STRAIN;

  if(strain <= rot1p)
    return POS_INF_STRAIN;
  if(strain > rot1p && strain <= rot2p && E2p < 0.0)
    strainLimit = rot1p - mom1p/E2p;
  if(strain > rot2p && E3p < 0.0)
    strainLimit = rot2p - mom2p/E3p;

  if(strainLimit == POS_INF_STRAIN)
    return POS_INF_STRAIN;
  else if(posEnvlpStress(strainLimit) > 0)
    return POS_INF_STRAIN;
  else
    return strainLimit;
}

double XC::HystereticMaterial::negEnvlpRotlim(double strain)
{
  double strainLimit = NEG_INF_STRAIN;

  if(strain >= rot1n)
    return NEG_INF_STRAIN;
  if(strain < rot1n && strain >= rot2n && E2n < 0.0)
    strainLimit = rot1n - mom1n/E2n;
  if(strain < rot2n && E3n < 0.0)
    strainLimit = rot2n - mom2n/E3n;

  if(strainLimit == NEG_INF_STRAIN)
    return NEG_INF_STRAIN;
  else if(negEnvlpStress(strainLimit) < 0)
    return NEG_INF_STRAIN;
  else
    return strainLimit;
}
