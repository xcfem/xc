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
// $Source: /OpenSees/SRC/material/nD/ContactMaterial3D.cpp,v $
                                                                        
// Written: Kathryn Petek
// Created: February 2004
// Modified: Chris McGann
//           November 2010 -> changes for incorporation into main source code
// Modified: Chris McGann
//           Jan 2011 -> added update for frictional state

// Description: This file contains the implementation for the ContactMaterial3D class.
//

#include "ContactMaterial3D.h"
#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <domain/component/Parameter.h>

//full constructor
XC::ContactMaterial3D::ContactMaterial3D(int tag, double mu, double Gmod, double c, double t)
  : ContactMaterialBase(tag,ND_TAG_ContactMaterial3D, 4),
   s_e_n(2),
   s_e_nplus1(2),
   r_nplus1(2),
   g(2,2), G(2,2)
  {
#ifdef DEBUG
    std::cerr << "XC::ContactMaterial3D::ContactMaterial3D(...)" << std::endl;
#endif
    this->zero();
  }

//null constructor
XC::ContactMaterial3D::ContactMaterial3D(int tag)
  : ContactMaterialBase(0, ND_TAG_ContactMaterial3D, 4),
    s_e_n(2),
    s_e_nplus1(2),
    r_nplus1(2),
    g(2,2), G(2,2)
  {
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::ContactMaterial3D()" << std::endl;
#endif
  }

//zero internal variables
void XC::ContactMaterial3D::zero(void)
{
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::zero( )" << std::endl;
#endif
  s_e_n.Zero();         // elastic slip from previous increment
  s_e_nplus1.Zero();    // elastic slip after current increment
       
  r_nplus1.Zero();      // direction of plastic slip

  inSlip = false;    
  mFlag = 1;

  stress_vec.Zero();
  strain_vec.Zero();
  tangent_matrix.Zero();
}


int XC::ContactMaterial3D::setTrialStrain (const Vector &strain_from_element)
{
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::setTrialStrain (const Vector &strain_from_element)" << std::endl;
#endif
  Vector t_s(2);          // tangential contact force
  double t_n;             // normal contact force
  double f_nplus1_trial;  // trial slip condition

  double gap;             // current gap
  Vector slip(2);         // incremental slip

  double t_s_norm;        // norm of tangential contact force
       
  strain_vec = strain_from_element;

  gap        = strain_vec(0);
  slip(0)    = strain_vec(1);
  slip(1)    = strain_vec(2);
  t_n        = strain_vec(3);    

  Vector zeroVec = slip;  
  zeroVec.Zero();

  // update frictional status
  this->UpdateFrictionalState();

  // trial state (elastic predictor step) -> assume sticking
  inSlip = false;
       
  s_e_nplus1 = (t_n > -tensileStrength) ?  s_e_n + slip : zeroVec; // ctv

  t_s        = stiffness * g * s_e_nplus1; // cov

  // Norm(s_e_nplus1) = sqrt( s_e_nplus1' * g * s_e_nplus1 )
  s_e_nplus1_norm = sqrt( s_e_nplus1(0) * g(0,0) * s_e_nplus1(0)
                          + s_e_nplus1(1) * g(1,0) * s_e_nplus1(0) * 2.0
                          + s_e_nplus1(1) * g(1,1) * s_e_nplus1(1) );

  // Norm(t_s) = sqrt( t_s' * g * t_s )
  //t_s_norm = sqrt( t_s(0) * G(0,0) * t_s(0)
  //                      + t_s(1) * G(1,0) * t_s(0) * 2.0
  //                      + t_s(1) * G(1,1) * t_s(1) );

  //Norm(t_s) = k*Norm(s_e_nplus1)  -- yields same result as above
  t_s_norm = stiffness * s_e_nplus1_norm;

  // slip condition
  f_nplus1_trial = t_s_norm - frictionCoeff*t_n - cohesion;

  // if (f_nplus1_trial > 0.0) {
  // if ( (f_nplus1_trial > 0.0) && (t_n > -cohesion/frictionCoeff) &&  (slip.Norm() > 1e-12) ) {
  if ( (f_nplus1_trial > 0.0) && (t_n > -tensileStrength) && (s_e_nplus1_norm > 1e-12) ) {
 
    // plastic corrector step -> sliding
    inSlip = true;

    gamma = f_nplus1_trial / stiffness * 0.999999999999 ;

    r_nplus1 = s_e_nplus1 / s_e_nplus1_norm; // ctv

    // s_p_nplus1 = s_p_n + gamma * r_nplus1
    // s_e_nplus1 = s_nplus1 - s_p_nplus1
    //        = (s_nplus1 - s_p_n) - gamma * r_nplus1
    //        = (s_n + slip - s_p_n) - gamma * r_nplus1
    //        = (s_e_n + slip) - gamma * r_nplus1
    //        = s_e_nplus1_trial - gamma * r_nplus1
    double scale = (1.0 - gamma/s_e_nplus1_norm);

    s_e_nplus1 = scale * s_e_nplus1; // ctv
    t_s        = scale * t_s;        // cov

  }

#ifdef DEBUG
  if (DEBUG_LEVEL > 1) {
    if (inSlip) {
      std::cerr << "   ** SLIDING (material)" << std::endl;
    } else {
      std::cerr << "   ** STICKING (material)" << std::endl;
	}
  }
#endif

  //update stress and strain values
  stress_vec(0) = t_n;
  stress_vec(1) = t_s(0);
  stress_vec(2) = t_s(1);
  stress_vec(3) = gap;
       
  return 0;
}


//unused trial strain functions
int XC::ContactMaterial3D::setTrialStrain (const Vector &v, const Vector &r)
{
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::setTrialStrain (const Vector &v, const Vector &r)" << std::endl;
#endif
  std::cerr << "YOU SHOULD NOT SEE THIS: XC::ContactMaterial3D::setTrialStrain (const Vector &v, const Vector &r)" << std::endl;
  return this->setTrialStrain (v);
}


const XC::Matrix &XC::ContactMaterial3D::getTangent(void) const
  {
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::getTangent()\n";
#endif

  double C_nl;
  Matrix C_ss(2,2);
  Vector C_sl(2);

  double t_n = strain_vec(3);

  C_nl = 1.0;

  if (t_n < - tensileStrength) {
    C_ss.Zero();
    C_sl.Zero();
               
  } else if (inSlip) {
    // sliding coefficients
    Matrix r_dyadic_r(2,2);

    Vector R_nplus1 = g * r_nplus1;

    r_dyadic_r(0,0) = R_nplus1(0)*R_nplus1(0);
    r_dyadic_r(0,1) = R_nplus1(0)*R_nplus1(1);
    r_dyadic_r(1,1) = R_nplus1(1)*R_nplus1(1);
    r_dyadic_r(1,0) = r_dyadic_r(0,1);
           
    double scale = (1.0 - gamma/s_e_nplus1_norm);
    C_ss = stiffness * scale * (g - r_dyadic_r);
    C_sl = R_nplus1*frictionCoeff;

  } else {
    // sticking coefficients
    C_ss = stiffness * g;
    C_sl.Zero();
  }

#ifdef DEBUG
  if (DEBUG_LEVEL > 1) {
    std::cerr << "   strain_vec = " << strain_vec;
    std::cerr << "   is sliding? " << inSlip << std::endl;
  }
  if (DEBUG_LEVEL > 2) {
    std::cerr << "   C_nl = " << C_nl
           << "   C_ss = " << C_ss
           << "   C_sl = " << C_sl
           << std::endl;
    std::cerr << "   stiffness: " << stiffness
           << "   mu: " << frictionCoeff << std::endl;
 }
#endif

  //tangent matrix was zeroed initially
  tangent_matrix(0,3) = 1;
  tangent_matrix(1,1) = C_ss(0,0);
  tangent_matrix(1,2) = C_ss(0,1);
  tangent_matrix(2,1) = C_ss(1,0);
  tangent_matrix(2,2) = C_ss(1,1);

  tangent_matrix(1,3) = C_sl(0);
  tangent_matrix(2,3) = C_sl(1);
  tangent_matrix(3,0) = 1;
       
  return tangent_matrix;          
}


void XC::ContactMaterial3D::setMetricTensor(const Matrix &v)
{
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::setMetricTensor(Matrix &v)" << std::endl;
#endif
  g = v;
 
  // dual basis metric tensor G = inv(g)
  double det = g(0,0)*g(1,1) - g(0,1)*g(1,0);
  G(0,0) =  g(1,1);
  G(1,0) = -g(1,0);
  G(0,1) = -g(0,1);
  G(1,1) =  g(0,0);
  G = G / det;
}

int XC::ContactMaterial3D::commitState(void)
  {
#ifdef DEBUG
  std::cerr << getClassName() << "::" << __FUNCTION__
         << std::endl;
#endif

        s_e_n = s_e_nplus1;
  
        return 0;
  }

int XC::ContactMaterial3D::revertToStart(void)
{
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::revertToStart(void)" << std::endl;
#endif

  this->zero();

  return 0;
}

XC::NDMaterial *XC::ContactMaterial3D::getCopy(void) const
{
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::getCopy (void)" << std::endl;
#endif
  return new ContactMaterial3D(*this);
}

XC::NDMaterial *XC::ContactMaterial3D::getCopy(const std::string &code) const
{
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::getCopy (const char *code)" << std::endl;
#endif
  ContactMaterial3D *retval(nullptr);
  if (code=="ContactMaterial3D")
    {
      retval= new ContactMaterial3D(*this);
    }
  return retval;
}

const std::string &XC::ContactMaterial3D::getType(void) const
  {
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::getType (void) const" << std::endl;
#endif
    static const std::string tmp= "ThreeDimensional";
    return tmp;
  }

int XC::ContactMaterial3D::getOrder(void) const
{
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::getOrder (void) const" << std::endl;
#endif
  return 6;
}

void XC::ContactMaterial3D::Print(std::ostream &s, int flag )
{
#ifdef DEBUG
  std::cerr << "XC::ContactMaterial3D::Print(OPS_Stream &s, int flag )" << std::endl;
#endif
  s << "ContactMaterial3D: " << std::endl;
}

int XC::ContactMaterial3D::updateParameter(int responseID, Information &info)
  {
    return XC::ContactMaterialBase::updateParameter(responseID, info);
  }

//! @brief Send material.
int XC::ContactMaterial3D::sendData(Communicator &comm)
  {
    int res= ContactMaterialBase::sendData(comm);
    res+= comm.sendVector(s_e_n, getDbTagData(), CommMetaData(8));
    res+= comm.sendVector(s_e_nplus1, getDbTagData(),CommMetaData(9));
    res+= comm.sendVector(r_nplus1, getDbTagData(),CommMetaData(10));
    res+= comm.sendDoubles(gamma, s_e_nplus1_norm, getDbTagData(), CommMetaData(11));
    res+= comm.sendMatrix(g, getDbTagData(), CommMetaData(12));
    res+= comm.sendMatrix(G, getDbTagData(), CommMetaData(13));
    return res;
  }
  
  
//! @brief Receive material.
int XC::ContactMaterial3D::recvData(const Communicator &comm)
  {
    int res= ContactMaterialBase::recvData(comm);
    res+= comm.receiveVector(s_e_n, getDbTagData(),CommMetaData(8));
    res+= comm.receiveVector(s_e_nplus1, getDbTagData(),CommMetaData(9));
    res+= comm.receiveVector(r_nplus1, getDbTagData(),CommMetaData(10));
    res+= comm.receiveDoubles(gamma, s_e_nplus1_norm, getDbTagData(), CommMetaData(11));
    res+= comm.receiveMatrix(g, getDbTagData(), CommMetaData(12));
    res+= comm.receiveMatrix(G, getDbTagData(), CommMetaData(13));
    return res;
  }

int XC::ContactMaterial3D::sendSelf(Communicator &comm)
  {
    inicComm(14);
    int res= sendData(comm);
    const int dataTag= getDbTag(comm);
    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data.\n";    
    return res;
  }

int XC::ContactMaterial3D::recvSelf(const Communicator &comm)
  {
    inicComm(14);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << ";  data could not be received.\n" ;
    else
      res+= recvData(comm);
    return res;
  }
