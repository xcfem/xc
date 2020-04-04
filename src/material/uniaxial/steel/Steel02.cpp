//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C) Luis Claudio Pérez Tato l.perez@xcingenieria.com
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
                                                                        
// $Revision: 1.2 $
// $Date: 2006/03/23 23:02:00 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Steel02.cpp,v $
                                                                      
// Written: fmk
// Created: 03/06
//
// Description: This file contains the class implementation of Steel02. 
// This Steel02 is based on an f2c of the FEDEAS material
// Steel02.f which is:
//-----------------------------------------------------------------------
// MENEGOTTO-PINTO STEEL MODEL WITH FILIPPOU ISOTROPIC HARDENING
//            written by MOHD YASSIN (1993)
//          adapted to FEDEAS material library
//    by E. Spacone, G. Monti and F.C. Filippou (1994)
//-----------------------------------------------------------------------

#include <material/uniaxial/steel/Steel02.h>
#include <utility/matrix/Vector.h>
#include <cfloat>
#include <cstdlib>
#include "utility/actor/actor/MovableVector.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/component/Parameter.h"

//! @brief Sets all history and state variables to initial values
int XC::Steel02::setup_parameters(void)
  {
    eP= E0;
    epsP= 0.0;
    sigP= 0.0;
    sig= 0.0;
    eps= 0.0;
    e= E0;

    epsmaxP= fy/E0;
    epsminP= -epsmaxP;
    epsplP= 0.0;
    epss0P= 0.0;
    sigs0P= 0.0;
    epssrP= 0.0;
    sigsrP= 0.0;

    if(sigini!=0.0)
      {
        epsP= sigini/E0;
        sigP= sigini;
      }
    return 0;
  }

XC::Steel02::Steel02(int tag, double _fy, double _E0, double _b,
                 double _R0, double _cR1, double _cR2,
                 double _a1, double _a2, double _a3, double _a4, double sigInit)
  : SteelBase(tag,MAT_TAG_Steel02,_fy,_E0,_b,_a1,_a2,_a3,_a4), 
    sigini(sigInit), R0(_R0), cR1(_cR1), cR2(_cR2), konP(0), kon(0)
  { setup_parameters(); }

XC::Steel02::Steel02(int tag, double _fy, double _E0, double _b, double _R0, double _cR1, double _cR2)
  : SteelBase(tag, MAT_TAG_Steel02,_fy,_E0,_b,0.0,1.0,0.0,1.0),
    sigini(0.0), R0(_R0), cR1(_cR1), cR2(_cR2), konP(0)
  { setup_parameters(); }

XC::Steel02::Steel02(int tag, double _fy, double _E0,double _b)
  : SteelBase(tag, MAT_TAG_Steel02,_fy,_E0,_b,0.0,1.0,0.0,1.0),
    sigini(0.0), R0(15.0), cR1(0.925), cR2(0.15),konP(0) //Default values for elastic to hardening transitions
  { setup_parameters(); }

XC::Steel02::Steel02(int tag)
  : SteelBase(tag, MAT_TAG_Steel02,0.0,0.0,0.0,0.0,1.0,0.0,1.0),
    sigini(0.0), R0(15.0), cR1(0.925), cR2(0.15), konP(0) // Default values for elastic to hardening transitions
  { setup_parameters(); }

XC::Steel02::Steel02(void)
  : SteelBase(0, MAT_TAG_Steel02),sigini(0.0), konP(0) {}

//! @brief Sets the initial stress value.
void XC::Steel02::setInitialStress(const double &d)
  {
    sigini= d;
    setup_parameters(); //Initialize history variables.
  }

XC::UniaxialMaterial *XC::Steel02::getCopy(void) const
  { return new Steel02(*this); }

int XC::Steel02::setTrialStrain(double trialStrain, double strainRate)
  {
    double Esh= b * E0;
    double epsy= fy / E0;

    // modified C-P. Lamarche 2006
    if(sigini != 0.0)
      {
        const double epsini= sigini/E0;
        eps= trialStrain+epsini;
      }
    else
        eps= trialStrain;
    // modified C-P. Lamarche 2006

    double deps= eps - epsP;


    epsmax= epsmaxP;
    epsmin= epsminP;
    epspl = epsplP;
    epss0 = epss0P;  
    sigs0 = sigs0P; 
    epsr  = epssrP;  
    sigr  = sigsrP;  
    kon= konP;

    if(kon == 0 || kon == 3) // modified C-P. Lamarche 2006
      {
        if(fabs(deps) < 10.0*DBL_EPSILON)
          {
            e= E0;
            sig= sigini; // modified C-P. Lamarche 2006
            kon= 3; // modified C-P. Lamarche 2006 flag to impose initial stess/strain
            return 0;
          }
        else
          {
            epsmax= epsy;
            epsmin= -epsy;
            if(deps < 0.0)
              {
                kon= 2;
                epss0= epsmin;
                sigs0= -fy;
                epspl= epsmin;
              }
            else
              {
                kon= 1;
                epss0= epsmax;
                sigs0= fy;
                epspl= epsmax;
              }
          }
       }
  
    // in case of load reversal from negative to positive strain increment, 
    // update the minimum previous strain, store the last load reversal 
    // point and calculate the stress and strain (sigs0 and epss0) at the 
    // new intersection between elastic and strain hardening asymptote 
    // To include isotropic strain hardening shift the strain hardening 
    // asymptote by sigsft before calculating the intersection point 
    // Constants a3 and a4 control this stress shift on the tension side 
  
    if(kon == 2 && deps > 0.0)
      {
        kon= 1;
        epsr= epsP;
        sigr= sigP;
        //epsmin= min(epsP, epsmin);
        if(epsP < epsmin)
          epsmin= epsP;
        double d1= (epsmax - epsmin) / (2.0*(a4 * epsy));
        double shft= 1.0 + a3 * pow(d1, 0.8);
        epss0= (fy * shft - Esh * epsy * shft - sigr + E0 * epsr) / (E0 - Esh);
        sigs0= fy * shft + Esh * (epss0 - epsy * shft);
        epspl= epsmax;
      }
    else if (kon == 1 && deps < 0.0)
      {
    
        // update the maximum previous strain, store the last load reversal 
        // point and calculate the stress and strain (sigs0 and epss0) at the 
        // new intersection between elastic and strain hardening asymptote 
        // To include isotropic strain hardening shift the strain hardening 
        // asymptote by sigsft before calculating the intersection point 
        // Constants a1 and a2 control this stress shift on compression side 

          kon= 2;
          epsr= epsP;
          sigr= sigP;
          //      epsmax= max(epsP, epsmax);
          if(epsP > epsmax)
            epsmax= epsP;

          double d1= (epsmax - epsmin) / (2.0*(a2 * epsy));
          double shft= 1.0 + a1 * pow(d1, 0.8);
          epss0= (-fy * shft + Esh * epsy * shft - sigr + E0 * epsr) / (E0 - Esh);
          sigs0= -fy * shft + Esh * (epss0 + epsy * shft);
          epspl= epsmin;
      }
  
    // calculate current stress sig and tangent modulus E 

    double xi    = fabs((epspl-epss0)/epsy);
    double R     = R0*(1.0 - (cR1*xi)/(cR2+xi));
    double epsrat= (eps-epsr)/(epss0-epsr);
    double dum1 = 1.0 + pow(fabs(epsrat),R);
    double dum2 = pow(dum1,(1/R));

    sig  = b*epsrat +(1.0-b)*epsrat/dum2;
    sig  = sig*(sigs0-sigr)+sigr;

    e= b + (1.0-b)/(dum1*dum2);
    e= e*(sigs0-sigr)/(epss0-epsr);
    return 0;
  }



double XC::Steel02::getStrain(void) const
  { return eps; }

double XC::Steel02::getStress(void) const
  { return sig; }

double XC::Steel02::getTangent(void) const
  { return e; }

int XC::Steel02::commitState(void)
{
  epsminP= epsmin;
  epsmaxP= epsmax;
  epsplP= epspl;
  epss0P= epss0;
  sigs0P= sigs0;
  epssrP= epsr;
  sigsrP= sigr;
  konP= kon;
  
  eP= e;
  sigP= sig;
  epsP= eps;

  return 0;
}

int XC::Steel02::revertToLastCommit(void)
  {
    epsmin= epsminP;
    epsmax= epsmaxP;
    epspl= epsplP;
    epss0= epss0P;
    sigs0= sigs0P;
    epsr= epssrP;
    sigr= sigsrP;
    kon= konP;
  
    e= eP;
    sig= sigP;
    eps= epsP;
    return 0;
  }

int XC::Steel02::revertToStart(void)
  {
    setup_parameters();
    konP= 0;
    return 0;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::Steel02::getDbTagData(void) const
  {
    static DbTagData retval(9);
    return retval;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::Steel02::sendData(Communicator &comm)
  {
    int res= SteelBase::sendData(comm);
    res+= comm.sendDoubles(sigini,R0,cR1,cR2,epsminP,epsmaxP,getDbTagData(),CommMetaData(4));
    res+= comm.sendDoubles(epsplP,epss0P,sigs0P,epssrP,sigsrP,epsP,getDbTagData(),CommMetaData(5));
    res+= comm.sendInts(konP,kon,getDbTagData(),CommMetaData(6));
    res+= comm.sendDoubles(sigP,eP,epsmin,epsmax,epspl,epss0,getDbTagData(),CommMetaData(7));
    res+= comm.sendDoubles(sigs0,epsr,sigr,sig,e,eps,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::Steel02::recvData(const Communicator &comm)
  {
    int res= SteelBase::recvData(comm);
    res+= comm.receiveDoubles(sigini,R0,cR1,cR2,epsminP,epsmaxP,getDbTagData(),CommMetaData(4));
    res+= comm.receiveDoubles(epsplP,epss0P,sigs0P,epssrP,sigsrP,epsP,getDbTagData(),CommMetaData(5));
    res+= comm.receiveInts(konP,kon,getDbTagData(),CommMetaData(6));
    res+= comm.receiveDoubles(sigP,eP,epsmin,epsmax,epspl,epss0,getDbTagData(),CommMetaData(7));
    res+= comm.receiveDoubles(sigs0,epsr,sigr,sig,e,eps,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::Steel02::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(9);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::Steel02::recvSelf(const Communicator &comm)
  {
    inicComm(9);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! @brief Print stuff.
void XC::Steel02::Print(std::ostream &s, int flag) const
  { s << "Steel02:(strain, stress, tangent) " << eps << " " << sig << " " << e << std::endl; }

// AddingSensitivity:BEGIN ///////////////////////////////////
int XC::Steel02::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {  
    const int sp= SteelBase::setParameter(argv,param);
    if(sp<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
		<< "; WARNING: could not set parameter. "
		<< std::endl;
    return sp;
  }

int XC::Steel02::updateParameter(int parameterID, Information &info)
  {
    const int up= SteelBase::updateParameter(parameterID,info);
    return up;
  }

// AddingSensitivity:END /////////////////////////////////////////////
