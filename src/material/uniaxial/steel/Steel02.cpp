//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C) Luis C. Pérez Tato l.perez@xcingenieria.com
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
    stvP.setup_parameters(this->E0, this->sigini);
    stv.setup_parameters(this->E0, 0.0);

    hstvP.setup_parameters(this->E0, this->fy);

    return 0;
  }

//! @brief Default constructor.
XC::Steel02::Steel02(int tag)
  : SteelBase(tag, MAT_TAG_Steel02,0.0,0.0,0.0,0.0,1.0,0.0,1.0),
    sigini(0.0), R0(15.0), cR1(0.925), cR2(0.15) // Default values for elastic to hardening transitions
  { setup_parameters(); }

XC::Steel02::Steel02(int tag, double _fy, double _E0, double _b,
                 double _R0, double _cR1, double _cR2,
                 double _a1, double _a2, double _a3, double _a4,
		     const double &sigInit, const double &epsInit)
  : SteelBase(tag,MAT_TAG_Steel02,_fy,_E0,_b,_a1,_a2,_a3,_a4, epsInit), 
    sigini(sigInit), R0(_R0), cR1(_cR1), cR2(_cR2)
  { setup_parameters(); }

XC::Steel02::Steel02(int tag, double _fy, double _E0, double _b, double _R0, double _cR1, double _cR2)
  : SteelBase(tag, MAT_TAG_Steel02,_fy,_E0,_b,0.0,1.0,0.0,1.0),
    sigini(0.0), R0(_R0), cR1(_cR1), cR2(_cR2)
  { setup_parameters(); }

XC::Steel02::Steel02(int tag, double _fy, double _E0,double _b)
  : SteelBase(tag, MAT_TAG_Steel02,_fy,_E0,_b,0.0,1.0,0.0,1.0),
    sigini(0.0), R0(15.0), cR1(0.925), cR2(0.15) //Default values for elastic to hardening transitions
  { setup_parameters(); }

//! @brief Get the first parameter that controls the transition from elastic to plastic branches.
double XC::Steel02::getR0(void) const
  { return R0; }
//! @brief Set the first parameter that controls the transition from elastic to plastic branches.
void XC::Steel02::setR0(const double &d)
  { R0= d;}

//! @brief Get second parameter that controls the transition from elastic to plastic branches.
double XC::Steel02::getCR1(void) const
  { return cR1; }
//! @brief Set second parameter that controls the transition from elastic to plastic branches.
void XC::Steel02::setCR1(const double &d)
  { cR1= d; }

//! @brief Get third parameter that controls the transition from elastic to plastic branches.
double XC::Steel02::getCR2(void) const
  { return cR2; }
//! @brief Set third parameter that controls the transition from elastic to plastic branches.
void XC::Steel02::setCR2(const double &d)
  { cR2= d; }

//! @brief Set coefficients for isotropic hardening [a1, a2, a3, a4].
void XC::Steel02::setParams(const std::vector<double> &params)
  {
    const size_t sz= params.size();
    if(sz>0)
      setR0(params[0]);
    if(sz>1)
      setCR1(params[1]);
    if(sz>2)
      setCR2(params[2]);
  }
//! @brief Set coefficients for isotropic hardening [a1, a2, a3, a4].
void XC::Steel02::setParamsPy(const boost::python::list &params)
  {
    const size_t sz= boost::python::len(params);
    std::vector<double> tmp(sz);
    for(size_t i= 0;i<sz;i++)
      {
	tmp[i]= boost::python::extract<double>(params[i]);
      }
    setParams(tmp);
  }

//! @brief Sets initial strain.
int XC::Steel02::setInitialStrain(const double &strain)
  {
    ezero= strain;
    setInitialStress(sigini-ezero*E0);
    return 0;
  }

//! @brief Increments initial strain.
//! @param strainIncrement: value of the strain increment.
int XC::Steel02::incrementInitialStrain(const double &strainIncrement)
  {
    ezero+= strainIncrement;
    setInitialStress(sigini-strainIncrement*E0);
    return 0;
  }

//! @brief Zeroes the initial strain.
void XC::Steel02::zeroInitialStrain(void)
  { incrementInitialStrain(-ezero); }

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
    const double Esh= b*E0;
    const double epsy= fy/E0;

    // modified C-P. Lamarche 2006
    if(sigini != 0.0)
      {
        const double epsini= sigini/E0;
        stv.eps= trialStrain+epsini;
      }
    else
        stv.eps= trialStrain;
    // modified C-P. Lamarche 2006

    const double deps= stv.eps - stvP.eps;

    hstv.partial_revert_to_last_commit(hstvP);
    
    const double tol= 10.0*DBL_EPSILON;

    if(hstv.kon == 0 || hstv.kon == 3) // modified C-P. Lamarche 2006
      {
        if(fabs(deps) < tol)
          {
            stv.e= E0;
            stv.sig= sigini; // modified C-P. Lamarche 2006
            hstv.kon= 3; // modified C-P. Lamarche 2006 flag to impose initial stress/strain
            return 0;
          }
        else
          {
            hstv.epsmax= epsy;
            hstv.epsmin= -epsy;
            if(deps < 0.0)
              {
                hstv.kon= 2;
                hstv.epss0= hstv.epsmin;
                hstv.sigs0= -fy;
                hstv.epspl= hstv.epsmin;
              }
            else
              {
                hstv.kon= 1;
                hstv.epss0= hstv.epsmax;
                hstv.sigs0= fy;
                hstv.epspl= hstv.epsmax;
              }
          }
      }
    if(hstv.epss0 == 0.0) //Set epps0 after setup_parameters
      {
        if(hstv.kon == 2 && deps<=0.0)
	  {
	    const double d1= (hstv.epsmax - hstv.epsmin) / (2.0*(a2 * epsy));
	    const double shft= 1.0 + a1 * pow(d1, 0.8);
	    hstv.epss0= (-fy * shft + Esh * epsy * shft - hstv.sigsr + E0 * hstv.epssr) / (E0 - Esh);
	    hstv.sigs0= -fy * shft + Esh * (hstv.epss0 + epsy * shft);
	    hstv.epspl= hstv.epsmin;
	  }
	else if(hstv.kon == 1 && deps>=0.0)
	  {
	    const double d1= (hstv.epsmax - hstv.epsmin) / (2.0*(a4 * epsy));
	    const double shft= 1.0 + a3 * pow(d1, 0.8);
	    hstv.epss0= (fy * shft - Esh * epsy * shft - hstv.sigsr + E0 * hstv.epssr) / (E0 - Esh);
	    hstv.sigs0= fy * shft + Esh * (hstv.epss0 - epsy * shft);
	    hstv.epspl= hstv.epsmax;
	  }
      }
  
    // in case of load reversal from negative to positive strain increment, 
    // update the minimum previous strain, store the last load reversal 
    // point and calculate the stress and strain (hstv.sigs0 and hstv.epss0) at the 
    // new intersection between elastic and strain hardening asymptote 
    // To include isotropic strain hardening shift the strain hardening 
    // asymptote by sigsft before calculating the intersection point 
    // Constants a3 and a4 control this stress shift on the tension side 
  
    if(hstv.kon == 2 && deps > 0.0)
      {
        hstv.kon= 1;
        hstv.epssr= stvP.eps;
        hstv.sigsr= stvP.sig;
        //hstv.epsmin= min(stvP.eps, hstv.epsmin);
        if(stvP.eps < hstv.epsmin)
          hstv.epsmin= stvP.eps;
        const double d1= (hstv.epsmax - hstv.epsmin) / (2.0*(a4 * epsy));
        const double shft= 1.0 + a3 * pow(d1, 0.8);
        hstv.epss0= (fy * shft - Esh * epsy * shft - hstv.sigsr + E0 * hstv.epssr) / (E0 - Esh);
        hstv.sigs0= fy * shft + Esh * (hstv.epss0 - epsy * shft);
        hstv.epspl= hstv.epsmax;
      }
    else if (hstv.kon == 1 && deps < 0.0)
      {
    
        // update the maximum previous strain, store the last load reversal 
        // point and calculate the stress and strain (hstv.sigs0 and hstv.epss0) at the 
        // new intersection between elastic and strain hardening asymptote 
        // To include isotropic strain hardening shift the strain hardening 
        // asymptote by sigsft before calculating the intersection point 
        // Constants a1 and a2 control this stress shift on compression side 

          hstv.kon= 2;
          hstv.epssr= stvP.eps;
          hstv.sigsr= stvP.sig;
          //      hstv.epsmax= max(stvP.eps, hstv.epsmax);
          if(stvP.eps > hstv.epsmax)
            hstv.epsmax= stvP.eps;

          const double d1= (hstv.epsmax - hstv.epsmin) / (2.0*(a2 * epsy));
          const double shft= 1.0 + a1 * pow(d1, 0.8);
          hstv.epss0= (-fy * shft + Esh * epsy * shft - hstv.sigsr + E0 * hstv.epssr) / (E0 - Esh);
          hstv.sigs0= -fy * shft + Esh * (hstv.epss0 + epsy * shft);
          hstv.epspl= hstv.epsmin;
      }
  
    // calculate current stress sig and tangent modulus E 

    const double xi= fabs((hstv.epspl-hstv.epss0)/epsy);
    const double R= R0*(1.0 - (cR1*xi)/(cR2+xi));
    const double epsdif= (hstv.epss0-hstv.epssr);
    const double epsrat= (stv.eps-hstv.epssr)/epsdif;
    const double dum1= 1.0 + pow(fabs(epsrat),R);
    const double dum2= pow(dum1,(1.0/R));

    stv.sig= b*epsrat +(1.0-b)*epsrat/dum2;
    stv.sig= stv.sig*(hstv.sigs0-hstv.sigsr)+hstv.sigsr;

    stv.e= b + (1.0-b)/(dum1*dum2);
    const double factor= (hstv.sigs0-hstv.sigsr)/epsdif;
    if(!std::isnan(factor))
      stv.e*= factor;
    return 0;
  }

//! @brief Return material strain
double XC::Steel02::getStrain(void) const
  { return stv.eps; }

//! @brief Return material stress.
double XC::Steel02::getStress(void) const
  { return stv.sig; }

//! @brief Return tangent stiffness.
double XC::Steel02::getTangent(void) const
  { return stv.e; }

//! @brief Commit material state.
int XC::Steel02::commitState(void)
  {
    hstvP= hstv;

    stvP= stv;

    return 0;
  }

//! @brief Revert the material to its last commited state.
int XC::Steel02::revertToLastCommit(void)
  {
    hstv= hstvP;
  
    stv= stvP;
    return 0;
  }

//! @brief Revert the material to its initial state.
int XC::Steel02::revertToStart(void)
  {
    int retval= SteelBase::revertToStart();
    setup_parameters();
    hstvP.kon= 0;
    return retval;
  }

//! @brief Returns a vector to store the dbTags
//! of the class members.
XC::DbTagData &XC::Steel02::getDbTagData(void) const
  {
    static DbTagData retval(9);
    return retval;
  }

//! @brief Send object members through the communicator argument.
int XC::Steel02::sendData(Communicator &comm)
  {
    int res= SteelBase::sendData(comm);
    res+= comm.sendDoubles(sigini,R0,cR1,cR2,hstvP.epsmin,hstvP.epsmax,getDbTagData(),CommMetaData(4));
    res+= comm.sendDoubles(hstvP.epspl,hstvP.epss0,hstvP.sigs0,hstvP.epssr,hstvP.sigsr,stvP.eps,getDbTagData(),CommMetaData(5));
    res+= comm.sendInts(hstvP.kon,hstv.kon,getDbTagData(),CommMetaData(6));
    res+= comm.sendDoubles(stvP.sig,stvP.e,hstv.epsmin,hstv.epsmax,hstv.epspl,hstv.epss0,getDbTagData(),CommMetaData(7));
    res+= comm.sendDoubles(hstv.sigs0,hstv.epssr,hstv.sigsr,stv.sig,stv.e,stv.eps,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::Steel02::recvData(const Communicator &comm)
  {
    int res= SteelBase::recvData(comm);
    res+= comm.receiveDoubles(sigini,R0,cR1,cR2,hstvP.epsmin,hstvP.epsmax,getDbTagData(),CommMetaData(4));
    res+= comm.receiveDoubles(hstvP.epspl,hstvP.epss0,hstvP.sigs0,hstvP.epssr,hstvP.sigsr,stvP.eps,getDbTagData(),CommMetaData(5));
    res+= comm.receiveInts(hstvP.kon,hstv.kon,getDbTagData(),CommMetaData(6));
    res+= comm.receiveDoubles(stvP.sig,stvP.e,hstv.epsmin,hstv.epsmax,hstv.epspl,hstv.epss0,getDbTagData(),CommMetaData(7));
    res+= comm.receiveDoubles(hstv.sigs0,hstv.epssr,hstv.sigsr,stv.sig,stv.e,stv.eps,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Sends object through the communicator argument.
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

//! @brief Receives object through the communicator argument.
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
  { s << "Steel02:(strain, stress, tangent) " << stv << std::endl; }

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
