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

// $Revision: 0 $
// $Date: May 2015 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/BilinearOilDamper.h,v $
                                                                        
// Written: Sarven Akcelyan and Dimitrios G. Lignos, PhD, McGill University
// Created: May 2015
// Updated: May 2015
// Revision: A

// Description: This file contains the class interface for 
// Oil Damper Model Relationship of the form  before relief valve ==> F = K*u_s = C*V_d  after relief valve ==> F= K*u_s = Fr + p*C*(V_d-Fr/C)
//
// References: 
// Akcelyan, S., and Lignos, D.G. (2015), “Adaptive Numerical Method Algorithms for Nonlinear Viscous and Bilinear Oil Damper Models Under Random Vibrations”, ASCE Journal of Engineering Mechanics, (under review)
// Kasai, K., Takahashi, O., and Sekiguchi, Y. (2004). "JSSI manual for building passive control technology part-10 time-history analysis model for nonlinear oil dampers." Proc., The 13th World Conference on Earthquake Engineering, Vancouver, B.C., Canada.

// Variables:
// $K: Elastic stiffness of linear spring (to model the axial flexibility of a viscous damper (brace and damper portion)
// $C: Viscous damping coefficient of the damper
// $Fr: Relief load
// $p: post-relief viscous damping coefficient ratio, (p=C2/C)
// $LGap: gap length to simulate the gap length due to the pin tolerance
// $NM:	Employed adaptive numerical algorithm (default value NM = 1; 1 = Dormand-Prince54, 2 = Finite differences)
// $RelTol: Tolerance for absolute relative error control of the adaptive iterative algorithm (default value 10^-6)
// $AbsTol: Tolerance for absolute error control of adaptive iterative algorithm (default value 10^-10)
// $MaxHalf: Maximum number of sub-step iterations within an integration step (default value 15)

#include "material/uniaxial/damper/BilinearOilDamper.h"
#include "utility/matrix/Vector.h"
#include "domain/domain/Domain.h"
#include "domain/mesh/element/utils/Information.h"
#include "domain/component/Parameter.h"
#include "utility/utils/misc_utils/colormod.h"

//! @brief Default constructor.
XC::BilinearOilDamper::BilinearOilDamper(int tag)
 :DamperBase(tag, MAT_TAG_BilinearOilDamper),
  Fr(1.0), p(1.0)
  {
    this->setup();
  }

XC::BilinearOilDamper::BilinearOilDamper(int tag, double k, double c, double fr, double pp, double lgap, double nm, double reltol, double abstol, double maxhalf)
  :DamperBase(tag,MAT_TAG_BilinearOilDamper, k, c, lgap, nm, reltol, abstol, maxhalf),
   Fr(fr), p(pp)
  {
    if (p < 0.0)
      {
	std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; p < 0.0, setting to 0.0"
		  << Color::def << std::endl;
	p = 0.0;
      }
  
    //initialize variables
    this->setup();
  }

int XC::BilinearOilDamper::setTrialStrain(double strain, double strainRate)
  {
    //all variables to the last commit
    this->revertToLastCommit();
  
  
    const double ops_Dt= this->getDomain()->getDt(); //time step
    // Determine the strain rate and acceleration 
    // const double dStrain = (strain - Tstrain);
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
    double smin = pow(0.5,MaxHalf);
    double s = 1.0;
    double stot = 0.0;
    double it = 0.0;
  
    // Method1 : Dormand Pronce Method (ODE Solver)
    if (NM == 1.0)
      { 
	double h, yt, eps, error;
	vel0 = TVel;  // Velocity of the previous step.
	fd0 = Tstress; 
    
	while (it < 1.0)
	  { //iteration
	    h = s * ops_Dt; // Time step 
	    vel1 = vel0 + acc * h; // Velocity at the time step h
    
	    this->DormandPrince(vel0, vel1, fd0, h, yt, eps, error);
      
      
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
		  {
		    s=smin;
		  }
	      }
      
	    if (stot == 1.0)
	      { // The total internal stepsize reached dt
		it=1.0;
	      }
	  }
    
	if (p == 0)
	  {
	    if (fabs(fd0) > Fr)
	      {
		fd0 = sgn(fd0)*Fr;
	      }
	  }
	
      }
  
  // Method2 : Numerical Integration
  if (NM == 2.0)
    { 
      double h= 0.0, fdk1= 0.0, fdk2= 0.0, eps= 0.0, error= 0.0;
    
    
      while (it < 1.0)
	{ //iteration
	  for ( int k = 1; k < 3 ; k = k + 1)
	    {
	      if (k == 1)
		{
		  h = s * ops_Dt; // Time step 
		}
	      else
		{
		  h = s/(s+1) * ops_Dt; // Time step 
		}
	
	      vel0 = TVel;  // Velocity of the previous step.
	      fd0 = Tstress;
	      for ( int j = 1; j < (ops_Dt/h + 1) ; j = j + 1)
		{
		  vel1 = vel0 + acc * h; // Velocity at the time step h
		  double fd1 = (K*vel1*h+fd0)/(1+K*h/C);
	  
		  if (fd1 > Fr)
		    { // if the force exceeds the relief force
		      if (p == 0.)
			{ fd1 = Fr; }
		      else
			{
			  fd1 = (K*vel1*h+Fr*(1-p)*K*h/(p*C)+fd0)/(1+K*h/(p*C));
			}
		    }
	  
		  if (fd1 < -Fr)
		    { // if the force exceeds the relief force
		      if (p == 0.)
			{ fd1 = -Fr; }
		      else
			{
			  fd1 = (K*vel1*h-Fr*(1-p)*K*h/(p*C)+fd0)/(1+K*h/(p*C));
			}
		    }
	  
		  fd0 = fd1;
		  vel0 = vel1;
		}
	      if(k == 1)
		{
		  fdk1 = fd0; // Solution for h = s * ops_Dt; 
		}
	      else
		{
		  fdk2 = fd0; // Solution for  h = s/(s+1) * ops_Dt;
		} 
	      error = fdk2 - fdk1;
	      eps = fabs(error/fdk2);
	    }
	  // Error check: Adaptive Step Size
	  if ((eps <= RelTol) || (s == smin) || (fabs(error) <= AbsTol))
	    {
	      it = 1;
	    }
	  else
	    {
	      if (s > smin)
		{
		  s= 0.5*s; // step gets smaller -now try this step again.
		}
	      else
		{
		  s= smin;
		}
	    }
	}
    }
  
  // End of Methods
  
  
  // Effect of gap start 
  
  if (LGap > 0.)
    {
      double dStrain = (strain - Tstrain);
    
      if ((fd0 > 0) && (Tstress < 0))
	{  //from negative to positive
      
	  Tpugr = Tstrain + dStrain * fabs(fd0)/fabs(fd0 - Tstress);  // approximate displacement for gap initiation
	  Tnugr = 0.;
      
	  if (fabs(strain-Tpugr) < LGap)
	    {fd0 = 0.; }
	}  
    
    if ((fd0 < 0) && (Tstress > 0))
      {  //from positive to negative
      
	Tnugr = Tstrain + dStrain * fabs(fd0)/fabs(fd0 - Tstress);  // approximate displacement for gap initiation
	Tpugr = 0.;
      
	if (fabs(strain-Tnugr) < LGap)
	  { fd0 = 0.; }
      }
    
    // After gap initiation
    
    if((fabs(Tpugr) > 0.) && (Tstress == 0))
      {   //from negative to positive
      
	if ((strain > Tpugr) && ((strain-Tpugr) < LGap))
	  { fd0 = 0.; }
      }
    
    
    
    if((fabs(Tnugr) > 0.) && (Tstress == 0))
      {   //from positive to negative
      
	if ((strain < Tnugr) && ((strain-Tnugr) > -LGap))
	  {
	    fd0 = 0.;
	  }
      }
    
    }
  // Effect of gap end 
  
  
  
  Tstress = fd0; // Stress 
  TVel = Vel;
  Tstrain = strain;
  
  return 0;
}

double XC::BilinearOilDamper::f(double v, double fd) const
  {
    if ((fabs(fd) < Fr) || (p == 0))
      {
	return ( v - (fd/C) )*K;
      }
    else
      {
	return ( v - ((sgn(fd)*(p-1.0)*Fr+fd)/(p*C)))*K;
      }
  }

//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::BilinearOilDamper::getCopy(void) const
  { return new BilinearOilDamper(*this); }

//! @brief Send object members through the given communicator.
int XC::BilinearOilDamper::sendData(Communicator &comm)
  {
    int res = DamperBase::sendData(comm);
    res+= comm.sendDoubles(Fr, p, getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receive object members through the given communicator.
int XC::BilinearOilDamper::recvData(const Communicator &comm)
  {
    int res = DamperBase::recvData(comm);
    res+= comm.receiveDoubles(Fr, p, getDbTagData(),CommMetaData(5));
    return res;
  }
//! @brief Sends object through the communicator argument.
int XC::BilinearOilDamper::sendSelf(Communicator &comm)
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
int XC::BilinearOilDamper::recvSelf(const Communicator &comm)
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

void XC::BilinearOilDamper::Print(std::ostream &s, int flag) const
  {
    s << "BilinearOilDamper tag: " << this->getTag() << std::endl;
    s << "  K: " << K << std::endl; 
    s << "  C: " << C << std::endl;
    s << "  Fr: " << Fr << std::endl;
	s << "  p: " << p << std::endl;
    s << "  LGap: " << LGap << std::endl; 
	s << "  NM: " << NM << std::endl; 
    s << "  RelTol: " << RelTol << std::endl;
	s << "  AbsTol: " << AbsTol << std::endl;
    s << "  MaxHalf: " << MaxHalf << std::endl;    
  }


int XC::BilinearOilDamper::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    int retval= -1;
    if((argv[0]=="Fr"))
      {
	param.setValue(Fr);
	retval= param.addObject(3, this);
      }
    else if((argv[0]=="p"))
      {
	param.setValue(p);
	retval= param.addObject(4, this);
      }
    else
      { retval= DamperBase::setParameter(argv, param); }
    return retval;
  }


int XC::BilinearOilDamper::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
      case 3:
	Fr = info.theDouble;
	return 0;
      case 4:
	p = info.theDouble;
	return 0;
      default:
	return DamperBase::updateParameter(parameterID, info);
      }
  }
