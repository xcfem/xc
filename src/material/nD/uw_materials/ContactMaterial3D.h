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

#ifndef ContactMaterial3D_h
#define ContactMaterial3D_h

// $Revision: 1.2
// $Date: 2010-11-10
// $Source: /OpenSees/SRC/material/nD/ContactMaterial3D.h,v $
                                                                        
// Written: Kathryn Petek
// Created: February 2004
// Modified: Chris McGann
//           November 2010 -> changes for incorporation into main source code
// Modified: Chris McGann
//           Jan 2011 -> added update for frictional state

// Description: This file contains the class prototype for ContactMaterial3D.
//

#include "ContactMaterialBase.h"


namespace XC {

//! @brief 2D contact material.
//! @ingroup UWMat
class ContactMaterial3D : public ContactMaterialBase
  {
  private:
  
    // state variables
    Vector s_e_n;           // elastic slip from previous increment
    Vector s_e_nplus1;      // elastic slip after current increment
  
    Vector r_nplus1;        // sliding direction
  
    double gamma;           // consistency parameter
    double s_e_nplus1_norm;	// norm of trial slip
  
  
    Matrix g;				// metric tensor
    Matrix G;				// dual basis metric tensor
  
  // static vectors and matrices declared in ContactMaterialBase
  // strain_vec(0) -> gap     ... current gap distance
  // strain_vec(1) -> slip(1) ... incremental slip in xi direction
  // strain_vec(2) -> slip(2) ... incremental slip in eta direciton
  // strain_vec(3) -> lambda  ... lagrangean multiplier -> t_n
  
  // stress_vec(0) -> t_n     ... normal contact force
  // stress_vec(1) -> t_s(1)  ... tangential contact force in xi dir.
  // stress_vec(2) -> t_s(2) ... tangentail contact force in eta dir.
  // stress_vec(3) -> gap     ... current gap
  
  protected:
    void zero();
    int sendData(Communicator &);  
    int recvData(const Communicator &);
  public:
    // Null constructor
    ContactMaterial3D(int tag= 0);
    // Full constructor
    ContactMaterial3D(int tag, double mu, double G, double c, double t);
  
    // Sets the values of the trial strain tensor.
    int setTrialStrain(const Vector &strain_from_element);
  
    // Unused trialStrain functions
    int setTrialStrain(const Vector &v, const Vector &r);
  
    // Calculates current tangent stiffness.
    const Matrix &getTangent(void) const;
  
    // get metric tensor for material class
    void setMetricTensor(const Matrix &m);
  
    // Accepts the current trial strain values as being on the
    // solution path, and updates all model parameters related
    // to stress/strain states. Return 0 on success.
    int commitState(void);
    int revertToStart();
  
    // Return an exact copy of itself.
    NDMaterial *getCopy(void) const;
  
    // Return a copy of itself if "code"="ContactMaterial3D", 
    // otherwise return null.
    NDMaterial *getCopy(const std::string &code) const;
  
    // Return the string "ContactMaterial3D".
    const std::string &getType(void) const;
  
    // Return ndm.
    int getOrder(void) const;
  
    void Print(std::ostream &, int flag =0);
  
    // public methods for material stage update
    int updateParameter(int responseID, Information &eleInformation);  
    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &); 
  };
} // end XC namespace

#endif

