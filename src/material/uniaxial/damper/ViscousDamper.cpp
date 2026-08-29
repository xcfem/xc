//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis C. Pérez Tato
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
                                                                        
// $Revision: C $
// $Date: May 2015 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ViscousDamper.cpp,v $
                                                                        
// Written: Sarven Akcelyan and Dimitrios G. Lignos, PhD, McGill University
// Created: January 2013
// Updated: May 2015
// Revision: C
//
// Description: This file contains the class interface for 
// Viscous Damper Model Relationship of the form F = K*u_s = C*pow(V_d,alpha)
// Reference: 
// Akcelyan, S., and Lignos, D.G. (2015), “Adaptive Numerical Method Algorithms for Nonlinear Viscous and Bilinear Oil Damper Models Under Random Vibrations”, ASCE Journal of Engineering Mechanics, (under review)
// Kasai K, Oohara K. (2001). “Algorithm and Computer Code To Simulate Response of Nonlinear Viscous Damper”. Proceedings Passively Controlled Structure Symposium 2001, Yokohama, Japan.

// Variables:
// $K: Elastic stiffness of linear spring (to model the axial flexibility of a viscous damper (brace and damper portion)
// $C: Viscous damping coefficient of the damper
// $Alpha: Viscous damper exponent
// $LGap: gap length to simulate the gap length due to the pin tolerance
// $NM:	Employed adaptive numerical algorithm (default value NM = 1; 1 = Dormand-Prince54, 2=6th order Adams-Bashforth-Moulton, 3=modified Rosenbrock Triple)
// $RelTol:	Tolerance for absolute relative error control of the adaptive iterative algorithm (default value 10^-6)
// $AbsTol:	Tolerance for absolute error control of adaptive iterative algorithm (default value 10^-6)
// $MaxHalf: Maximum number of sub-step iterations within an integration step, h=dt*(0.5)^MaxHalf (default value 15)


#include "material/uniaxial/damper/ViscousDamper.h"
#include "utility/matrix/Vector.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Default constructor.
XC::ViscousDamper::ViscousDamper(int tag)
  : DamperBase(tag,MAT_TAG_ViscousDamper),
    Alpha(0.0)
  { this->setup(); }

XC::ViscousDamper::ViscousDamper(int tag, double k, double c, double a, double lgap, double nm, double reltol, double abstol, double maxhalf)
  : DamperBase(tag,MAT_TAG_ViscousDamper, k, c, lgap, nm, reltol, abstol, maxhalf),
    Alpha(a)
  {
    if (Alpha < 0.0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; Alpha < 0.0, setting to 1.0"
		  << Color::def << std::endl;
	Alpha = 1.0;
      }
        
    //initialize variables
    this->setup();
  }


int XC::ViscousDamper::setTrialStrain(double strain, double strainRate)
  {
    //all variables to the last commit
    this->revertToLastCommit();
  
    const double ops_Dt= this->getDomain()->getDt(); //time step
    // Determine the strain rate and acceleration   
    double Vel= 0.0, fd0= 0.0, acc= 0.0, vel1= 0.0, vel0= 0.0;
    if (fabs(strainRate) == 0.0)
      { //static analysis
	Vel = 0.0;
	acc = 0.0;
      }
    else
      { 
	Vel = strainRate;
	acc = (Vel - TVel)/ops_Dt;
      }
  
    const double smin = pow(0.5,MaxHalf);
    double s = 1.0;
    double stot = 0.0;
    double it = 0.0;
    fd0 = Tstress; 

    double h, yt, eps, error;
    vel0 = TVel;  // Velocity of the previous step.


    while (it < 1.0)
      { //iteration
	h = s * ops_Dt; // Time step 
	vel1 = vel0 + acc * h; // Velocity at the time step h
    
    
	// Selection of Numerical Method to solve the ODE
	if (NM == 1.0)
	  {
	    DormandPrince(vel0, vel1, fd0, h, yt, eps, error);
	  }
	if (NM == 2.0)
	  {
	    ABM6(vel0, vel1, fd0, h, yt, eps, error);
	  }
	if (NM == 3.0)
	  {
	    ROS(vel0, vel1, fd0, h, yt, eps, error);
	  }
    
	// Error check: Adaptive Step Size
	if ((eps <= RelTol) || (s == smin) || (fabs(error) <= AbsTol))
	  {
	    vel0 = vel1;
	    fd0 = yt;
	    stot = stot+s;
	  }
	else
	  {
	    if (s > smin)
	      {
		s=0.5*s; // step gets smaller -now try this step again.
	      }
	    else
	      { s=smin; }
	  }
    
	if (stot == 1.0)
	  { // The total internal stepsize reached dt
	    it=1.0;
	  }
      }

    // Effect of gap start 
  
    if (LGap > 0.)
      {
	const double dStrain = (strain - Tstrain);
    
	if ((fd0 > 0) && (Tstress < 0))
	  {  //from negative to positive
	    Tpugr = Tstrain + dStrain * fabs(fd0)/fabs(fd0 - Tstress);  // approximate displacement for gap initiation
	    Tnugr = 0.;
      
	    if (fabs(strain-Tpugr) < LGap)
	      {
		fd0 = 0.;
	      }
	  }  
    
	if ((fd0 < 0) && (Tstress > 0))
	  {  //from positive to negative
      
	    Tnugr = Tstrain + dStrain * fabs(fd0)/fabs(fd0 - Tstress);  // approximate displacement for gap initiation
	    Tpugr = 0.;
      
	    if (fabs(strain-Tnugr) < LGap)
	      { fd0 = 0.; }
	  }
    
	// After gap inititon
	if  ((fabs(Tpugr) > 0.) && (Tstress == 0))
	  {   //from negative to positive
	    if ((strain > Tpugr) && ((strain-Tpugr) < LGap))
	      { fd0 = 0.; }
	  }
    
	if((fabs(Tnugr) > 0.) && (Tstress == 0))
	  { //from positive to negative
	    if ((strain < Tnugr) && ((strain-Tnugr) > -LGap))
	      { fd0 = 0.; }
	  }
    
      }
    // Effect of gap end 
  
  
    Tstress = fd0; // Stress 
    TVel = Vel;
    Tstrain = strain;
    
    return 0;
  }




int XC::ViscousDamper::ABM6(double vel0, double vel1, double y0, double h, double& y6, double& eps, double& error) const
  {
    h= h/6.0;

    const double f0= f((vel1 - vel0)*(0./6.) + vel0, y0);
    double y11= y0 + h*f0; //predictor
    const double f1= f((vel1 - vel0)*(1./6.) + vel0, y11);
    y11= y0 + h*f1; //corrector
    double y2= y11 + 0.5*h*(3.*f1 - 1.*f0); //predictor
    const double f2= f((vel1 - vel0)*(2./6.) + vel0, y2);
    y2= y11 + 0.5*h*(f2 + f1);  //corrector
    double y3= y2 + h/12.*(23.*f2 - 16.*f1 + 5.*f0); //predictor
    const double f3= f((vel1 - vel0)*(3./6.) + vel0, y3);
    y3= y2 + h/12.*(5.*f3 + 8.*f2 - 1.*f1); //corrector
    double y4= y3 + h/24.*(55.*f3 - 59.*f2 + 37.*f1 - 9.*f0); //predictor
    const double f4= f((vel1 - vel0)*(4./6.) + vel0, y4);
    y4= y3 + h/24.*(9.*f4 + 19.*f3 -5.*f2 + f1); // corrector
    double y5= y4 + h/720.*(1901.*f4 - 2774.*f3 + 2616.*f2 - 1274.*f1 + 251.*f0); // predictor
    const double f5= f((vel1 - vel0)*(5./6.) + vel0, y5);
    y5= y4 + h/720.*(251.*f5 + 646.*f4 -264.*f3 + 106.*f2 -19.*f1); // corrector
    double yp6= y5 + h/1440.*(4277.*f5 -7923.*f4 +9982.*f3 -7298.*f2 + 2877.*f1 -475.*f0); // predictor
    const double f6= f((vel1 - vel0)*(6./6.) + vel0, yp6);
    y6= y5 + (h/1440.)*(475.*f6 +1427.*f5  -798.*f4 + 482.*f3 -173.*f2 + 27.*f1); // corrector

    error= (yp6-y6);

    eps= fabs(error/y6);

    return 0;
  }

int XC::ViscousDamper::ROS(double vel0, double vel1, double y0, double h, double& y2, double& eps, double& error) const
  {
    const double J= -K / (Alpha*C);
    const double T= K;
    const double d= 1. / (2. + sqrt(2.));
    const double e32= 6. + sqrt(2.);
    const double W= 1. - h*d*J;
    const double f0= f(vel0, y0);
    const double k1= (f0 + h*d*T)/W;
    const double f1= f((vel1 - vel0)*(0.5) + vel0, y0 + (0.5)*k1*h);
    const double k2= (f1 - k1)/W + k1;
    y2 = y0 + h*k2;
    const double f2= f(vel1, y2);
    const double k3= 1./W *(f2 - e32*(k2 - f1) - 2.*(k1 - f0) + h*d*T);
    error= h/6.*(k1 -2.*k2 + k3);
    const double y3= y2 + error;
    eps= fabs(error/(y3));

    return 0;
  }


double XC::ViscousDamper::f(double v, double fd) const
  { return (v - sgn(fd) * pow(fabs(fd)/C,1.0/Alpha))*K; }
//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::ViscousDamper::getCopy(void) const
  { return new ViscousDamper(*this); }

//! @brief Send object members through the given communicator.
int XC::ViscousDamper::sendData(Communicator &comm)
  {
    int res = DamperBase::sendData(comm);
    res+= comm.sendDouble(Alpha, getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receive object members through the given communicator.
int XC::ViscousDamper::recvData(const Communicator &comm)
  {
    int res = DamperBase::recvData(comm);
    res+= comm.receiveDouble(Alpha, getDbTagData(),CommMetaData(5));
    return res;
  }
//! @brief Sends object through the communicator argument.
int XC::ViscousDamper::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(6);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data."
	        << Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ViscousDamper::recvSelf(const Communicator &comm)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to receive ids."
	        << Color::def << std::endl;
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
           std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		     << "; failed to receive data."
	             << Color::def << std::endl;
      }
    return res;
  }

int XC::ViscousDamper::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    if ((argv[0]=="Alpha") || (argv[0]=="alpha"))
      {
	param.setValue(this->Alpha);
	retval= param.addObject(1, this);
      }
    else
      {
	retval= DamperBase::setParameter(argv, param);
      }
    return retval;
  }

int XC::ViscousDamper::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
      case 6:
	this->Alpha= info.theDouble;
	return 0;
      default:
	return DamperBase::updateParameter(parameterID, info);
      }
  }

void XC::ViscousDamper::Print(std::ostream &s, int flag) const
  {
    s << "ViscousDamper tag: " << this->getTag() << std::endl;
    s << "  K: " << K << std::endl; 
    s << "  C: " << C << std::endl;
    s << "  Alpha: " << Alpha << std::endl;
	s << "  LGap: " << LGap << std::endl; 
	s << "  NM: " << NM << std::endl; 
    s << "  RelTol: " << RelTol << std::endl;
	s << "  AbsTol: " << AbsTol << std::endl;
    s << "  MaxHalf: " << MaxHalf << std::endl;
        
  }
