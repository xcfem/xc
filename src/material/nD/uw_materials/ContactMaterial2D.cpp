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

// $Revision: 1.2
// $Date: 2010-11-10
// $Source: /OpenSees/SRC/material/nD/ContactMaterial2D.cpp,v $
                                                                        
// Written: Kathryn Petek
// Created: February 2004
// Modified: Chris McGann
//           November 2010 -> changes for incorporation into main source code
// Modified: Chris McGann
//           Jan 2011 -> added update for frictional state

// Description: This file contains the implementation for the ContactMaterial2D class.
//				

#include "ContactMaterial2D.h"
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <domain/component/Parameter.h>

//full constructor
XC::ContactMaterial2D::ContactMaterial2D(int tag, double mu, double G, double c, double t)
  : ContactMaterialBase(tag,ND_TAG_ContactMaterial2D, 3)
  {
#ifdef DEBUG
        opserr << "XC::ContactMaterial2D::ContactMaterial2D" << std::endl;
#endif
        this->zero();
  }
   
//null constructor
XC::ContactMaterial2D::ContactMaterial2D(int tag) 
  : ContactMaterialBase(tag, ND_TAG_ContactMaterial2D, 3)
  {}

//zero internal variables
void XC::ContactMaterial2D::zero(void)
{
#ifdef DEBUG
        opserr << "XC::ContactMaterial2D::zero()" << std::endl;
#endif

    s_e_n      = 0.0;       // elastic slip from previous increment
    s_e_nplus1 = 0.0;       // elastic slip after current increment
    r_nplus1   = 0.0;       // direction of plastic slip

    inSlip     = false;     
	mFlag = 1;

    stress_vec.Zero();
    strain_vec.Zero();
    tangent_matrix.Zero();

	

	// ensure that tensileStrength is within bounds
	if (frictionCoeff == 0.0) {
		tensileStrength = 0.0;
	}
	else if (tensileStrength > cohesion / frictionCoeff ) {
		tensileStrength = cohesion / frictionCoeff;
	}


}


int XC::ContactMaterial2D::setTrialStrain(const Vector &strain_from_element)
{
#ifdef DEBUG
        opserr << "XC::ContactMaterial2D::setTrialStrain()" << std::endl;
#endif

    double t_s;             // tangential contact force
    double t_n;             // normal contact force
    double f_nplus1_trial;  // trial slip condition
    double gamma;           // consistency parameter

    double gap;             // current gap
    double slip;            // incremental slip
   
    strain_vec = strain_from_element;

    gap    = strain_vec(0);
    slip   = strain_vec(1);
    t_n    = strain_vec(2);

	// update frictional status
	this->UpdateFrictionalState();

// trial state (elastic predictor step) -> assume sticking
        inSlip = false;

        s_e_nplus1 = (t_n > -tensileStrength) ? s_e_n + slip : 0.0;
        t_s        = stiffness * s_e_nplus1;

        // slip condition
        f_nplus1_trial = fabs(t_s) - frictionCoeff*t_n - cohesion;

        // if ((f_nplus1_trial > 0.0) ) {
        if ((f_nplus1_trial > 0.0) && (t_n > -tensileStrength) && (fabs(s_e_nplus1) > 1.0e-12))       {


// plastic corrector step -> sliding
            inSlip = true;

            gamma = f_nplus1_trial / stiffness ;

            r_nplus1 = (t_s < 0) ? -1 : 1;

            // s_p_nplus1 = s_p_n + gamma * r_nplus1
            // s_e_nplus1 = s_nplus1 - s_p_nplus1
            //        = (s_nplus1 - s_p_n) - gamma * r_nplus1
            //        = (s_n + slip - s_p_n) - gamma * r_nplus1
            //        = (s_e_n + slip) - gamma * r_nplus1
            //        = s_e_nplus1_trial - gamma * r_nplus1
            s_e_nplus1 = s_e_nplus1 - gamma * r_nplus1;

            t_s = stiffness * s_e_nplus1;
    }

#ifdef DEBUG
    if (DEBUG_LEVEL > 1) {
        if (inSlip) {
            opserr << "   ** SLIDING" << std::endl; }
        else {
            opserr << "   ** STICKING" << std::endl;}
    }
#endif

    //update stress and strain values
    stress_vec(0) = t_n;
    stress_vec(1) = t_s;
    stress_vec(2) = gap;
    
    return 0;

}


//unused trial strain functoins
int XC::ContactMaterial2D::setTrialStrain (const Vector &v, const Vector &r)
{
  return this->setTrialStrain (v);
}


const XC::Matrix & XC::ContactMaterial2D::getTangent(void) const
  {
#ifdef DEBUG
    opserr << "XC::ContactMaterial2D::getTangent()\n";
#endif

    double C_nl;
    double C_ss;
    double C_sl;

    double t_n = strain_vec(2);

    C_nl = 1.0;

//    C_ss = (inSlip || t_n < - tensileStrength) ? 0.0 : stiffness;
//    C_sl = (inSlip) ? r_nplus1*frictionCoeff : 0.0;


	if (t_n < - tensileStrength) {
		C_ss = 0.0;
		C_sl = 0.0;
		
	} else if (inSlip) {
    // sliding coefficients
		C_ss = 0.0;
		C_sl = r_nplus1*frictionCoeff; 

	} else {
	// sticking coefficients
		C_ss = stiffness;
		C_sl = 0.0;
	} 
	

#ifdef DEBUG
    if (DEBUG_LEVEL > 1) {
        opserr << "   is sliding? " << inSlip << std::endl;
        opserr << "   C_nl = " << C_nl
               << "   C_ss = " << C_ss
               << "   C_sl = " << C_sl
               << std::endl;
        opserr << "   stiffness: " << stiffness 
               << "   mu: " << frictionCoeff << std::endl;
    }
#endif

//tangent matrix was zeroed initially
    tangent_matrix(0,2) = 1;
    tangent_matrix(1,1) = C_ss;
    tangent_matrix(1,2) = C_sl;
    tangent_matrix(2,0) = 1;
    
    return tangent_matrix;      
}


int XC::ContactMaterial2D::revertToStart(void)
{
#ifdef DEBUG
        opserr << "XC::ContactMaterial2D::revertToStart()" << std::endl;
#endif
	
    this->zero();

    return 0;
}

int XC::ContactMaterial2D::commitState(void)
  {
#ifdef DEBUG
  opserr << getClassName() << "::" << __FUNCTION__
         << std::endl;
#endif

        s_e_n = s_e_nplus1;
  
        return 0;
  }
 
XC::NDMaterial *XC::ContactMaterial2D::getCopy(void) const
{
#ifdef DEBUG
        opserr << "XC::ContactMaterial2D::getCopy()" << std::endl;
#endif

  ContactMaterial2D * copy = new ContactMaterial2D(*this);
  return copy;
}


XC::NDMaterial *XC::ContactMaterial2D::getCopy(const std::string &code) const
  {
#ifdef DEBUG
    opserr << "XC::ContactMaterial2D::getCopy()" << std::endl;
#endif
    
    ContactMaterial2D *retval= nullptr;
    if(code=="ContactMaterial2D")
      {
	retval= new ContactMaterial2D(*this);
      }
    return retval;
  }



const std::string &XC::ContactMaterial2D::getType(void) const
  {
#ifdef DEBUG
    opserr << "XC::ContactMaterial2D::getType()" << std::endl;
#endif
    static const std::string tmp= "Two_Dimensional";
    return tmp;

}


int XC::ContactMaterial2D::getOrder(void) const
{
#ifdef DEBUG
        opserr << "XC::ContactMaterial2D::getOrder()" << std::endl;
#endif
    return 3;
}

void XC::ContactMaterial2D::Print(std::ostream &s, int flag )
  {
    s << "ContactMaterial2D" << std::endl;
  }


int XC::ContactMaterial2D::updateParameter(int responseID, Information &info)
  {
    if(responseID==20)
      this->frictionCoeff=info.theDouble;
    if (responseID==21)
      this->stiffness=info.theDouble;
    return XC::ContactMaterialBase::updateParameter(responseID, info);
  }

//! @brief Send material.
int XC::ContactMaterial2D::sendData(Communicator &comm)
  {
    int res= ContactMaterialBase::sendData(comm);
    res+= comm.sendDoubles(s_e_n, s_e_nplus1, r_nplus1, getDbTagData(), CommMetaData(8));
    return res;
  }

//! @brief Receive material.
int XC::ContactMaterial2D::recvData(const Communicator &comm)
  {
    int res= ContactMaterialBase::recvData(comm);
    res+= comm.receiveDoubles(s_e_n, s_e_nplus1, r_nplus1, getDbTagData(),CommMetaData(8));
    return res;
  }

int XC::ContactMaterial2D::sendSelf(Communicator &comm)
  {
    inicComm(9);
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data.\n";    
    return res;
  }

int XC::ContactMaterial2D::recvSelf(const Communicator &comm)
  {
    inicComm(9);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ";  data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;
  }
