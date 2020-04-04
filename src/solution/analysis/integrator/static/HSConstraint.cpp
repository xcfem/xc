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
//# PROJECT:           Object Oriented Finite Element Program
//# PURPOSE:           Hyper-spherical Constraint
//# CLASS:             HSConstraint
//#
//# VERSION:           0.61803398874989 (golden section)
//# LANGUAGE:          C++
//# TARGET OS:         all...
//# DESIGN:            Ritu Jain, Boris Jeremic
//# PROGRAMMER(S):     Ritu, Boris Jeremic
//#
//#
//# DATE:              14Mar2003
//# UPDATE HISTORY:
//#
//#
//===============================================================================



#include "HSConstraint.h"
#include <solution/analysis/model/AnalysisModel.h>
#include <solution/system_of_eqn/linearSOE/LinearSOE.h>
#include <cmath>
#include "utility/actor/actor/MatrixCommMetaData.h"

//! @brief Constructor.
XC::HSConstraint::HSConstraint(AnalysisAggregation *owr,double arcLength, double psi_u, double psi_f, double u_ref)
  :ProtoArcLength(owr,INTEGRATOR_TAGS_HSConstraint,arcLength),
   psi_u2(psi_u*psi_u), psi_f2(psi_f*psi_f), u_ref2(u_ref*u_ref) {}

//! @brief Virtual constructor.
XC::Integrator *XC::HSConstraint::getCopy(void) const
  { return new HSConstraint(*this); }

//! @brief Returns the value of dLambda for the newStep method.
double XC::HSConstraint::getDLambdaNewStep(void) const
  {
    const Vector &dUhat= vectors.getDeltaUhat();
    const Vector &f_ext= vectors.getPhat();

    // determine delta lambda(1) == dlambda
//    double retval = sqrt(arcLength2/((dUhat^dUhat)+alpha2));
// out temp BJ 
//    double retval = sqrt(arcLength2/((psi_u2/u_ref2*fabs(dUhat^dUhat))+psi_f2));
// old version with fext
    double retval= sqrt(arcLength2/( (psi_u2/u_ref2*fabs(dUhat^dUhat) ) + psi_f2*(f_ext^f_ext)  ));
    retval *= signLastDeltaLambdaStep; // base sign of load change
                                        // on what was happening last step
    return retval;
  }

//! @brief Returns the value of dLambda para el método update.
double XC::HSConstraint::getDLambdaUpdate(void) const
  {
    const double &dLStep= vectors.getDeltaLambdaStep();
    const Vector &dUhat= vectors.getDeltaUhat();
    const Vector &dUstep= vectors.getDeltaUstep();
    const Vector &dUbar= vectors.getDeltaUbar();
    const Vector &f_ext= vectors.getPhat();
    const double prod_f_ext= (f_ext^f_ext);

    // determine the coeeficients of our quadratic equation
    const double a1= psi_u2/u_ref2*(dUhat^dUhat) + psi_f2 * prod_f_ext;
    
    const double a2= 2.0 *(psi_u2/u_ref2*((dUhat^dUbar)+(dUhat^dUstep))
                      + psi_f2*dLStep * prod_f_ext);
    
    const double a3= psi_u2/u_ref2 * ((dUstep+dUbar)^(dUstep+dUbar)) - arcLength2 
                      + (dLStep*dLStep)*psi_f2 * prod_f_ext ;

    // check for a solution to quadratic
    const double b24ac = a2*a2 - a1*a3;
    if(b24ac < 0)
      {
        std::cerr << "XC::HSConstraint::update() - imaginary roots due to multiple instability";
        std::cerr << " directions - initial load increment was too large\n";
        std::cerr << "a1: " << a1 << " a2: " << a2 << " a3: " << a3 << " b24ac: " << b24ac << std::endl;
        return -1;
      }
    double retval;
    if(a1 == 0.0)
      {
        // std::cerr << "XC::HSConstraint::update() - zero denominator";
        // std::cerr << "\n";
        // return -2;
	retval= -a3/(2.0*a2);
      }
    else
      {
    	// determine the roots of the quadratic
    	const double sqrtb24ac= sqrt(b24ac);
    	const double dlambda1= (-a2 + sqrtb24ac)/a1;
    	const double dlambda2= (-a2 - sqrtb24ac)/a1;

	//Vector deltaU1 = dUbar;
	//deltaU1->addVector(1.0, dUhat,dlambda1);
	//double costheta1 = dUstep^(dUstep+deltaU1);

	//Vector deltaU2 = dUbar;
	//deltaU2->addVector(1.0, dUhat,dlambda2);
	//double costheta2 = dUstep)^(dUstep+deltaU2);

        const double val= dUhat^dUstep;
    	double costheta1= (dUstep^dUstep) + (dUbar^dUstep);
    	const double costheta2= costheta1 + dlambda2*val;

    	costheta1+= dlambda1*val;

    	// choose retval based on angle between incremental displacement before
    	// and after this step -- want positive
    	if(costheta1 > costheta2)
     	  retval= dlambda1;
    	else
      	  retval= dlambda2;
      }
    return retval;
  }


//! @brief Send object members through the communicator argument.
int XC::HSConstraint::sendData(Communicator &comm)
  {
    int res= ProtoArcLength::sendData(comm);
    res+= comm.sendDoubles(psi_u2,psi_f2,u_ref2,getDbTagData(),CommMetaData(18));
    res+= comm.sendMatrix(scalingMatrix,getDbTagData(),CommMetaData(19));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::HSConstraint::recvData(const Communicator &comm)
  {
    int res= ProtoArcLength::recvData(comm);
    res+= comm.receiveDoubles(psi_u2,psi_f2,u_ref2,getDbTagData(),CommMetaData(18));
    res+= comm.receiveMatrix(scalingMatrix,getDbTagData(),CommMetaData(19));
    return res;
  }

//! @brief Sends object data through the communicator argument.
int XC::HSConstraint::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(23);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object data through the communicator argument.
int XC::HSConstraint::recvSelf(const Communicator &comm)
  {
    inicComm(23);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::HSConstraint::Print(std::ostream &s, int flag) const
  {
    ProtoArcLength::Print(s,flag);
    s << "  HSConstraint: " << sqrt(arcLength2) /*<<  "  alpha: ";
    s << sqrt(alpha2) */ << std::endl;
  }
