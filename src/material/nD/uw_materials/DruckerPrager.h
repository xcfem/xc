// -*-c++-*-
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
**                                                                    **
**                                                                    **
**                                                                    **
** ****************************************************************** */

#ifndef DruckerPrager_h
#define DruckerPrager_h

// $Revision: 1.1 $
// $Date: 2010-02-04 00:44:04 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/DruckerPrager.h,v $
                                                                      
// Written: Kathryn Petek, Peter Mackenzie-Helnwein, and Pedro Arduino
// Created: 12/04
//
// Description: This file contains the class definition for DruckerPrager. 
//

#include <material/nD/NDMaterial.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>


namespace XC {
//! @ingroup NDMat
//!
//! @defgroup UWMat Elastic isotropic n-dimensional material.
//
//! @ingroup UWMat
//
//! @brief Drucker-Prager material.
class DruckerPrager: public NDMaterial
  {
  protected:

    //material parameters
    double mKref; //!< reference Bulk Modulus 
    double mGref; //!< reference Shear Modulus
    double mPatm; //!< reference stress first invariant (pressure)
    double mK; //!< bulk modulus 
    double mG; //!< shear modulus
    double msigma_y; //!< yield stress 
    double mrho; //!< volumetric term (failure surface and associativity)
    double mrho_bar; //!< nonassociative flow term (failure surface and associativity)
    double mKinf; //!< nonlinear isotropic hardening term
    double mKo; //!< nonlinear isotropic hardening term
    double mdelta1; //!< exponential hardening term for Drucker-Prager surface
    double mdelta2; //!< Tension softening. Exponential hardening term for tension cutoff surface
    double mHard; //!< isotropic hardening constant
    double mtheta; //!< isotropic hardening constant
    double mTo; //!< initial tension cutoff strength

    double massDen; //!< density

    //internal variables
    Vector mEpsilon;
    Vector mEpsilon_n_p; //!< plastic strain vector at step n, trail e_p
    Vector mEpsilon_n1_p; //!< plastic strain vector at step n+1 
    Vector mSigma;

    Vector mBeta_n; //!< backstress at step n, beta_np1_trial = beta_n
    Vector mBeta_n1; //!< backstress at step n+1

    double mHprime; //!< derivative of linear kinematic hardening term 

    double mAlpha1_n; //!< alpha1_n
    double mAlpha1_n1; //!< alpha1_n+1
    double mAlpha2_n; //!< alpha2_n
    double mAlpha2_n1; //!< alpha2_n+1

    int mElastFlag; //!< Flag to determine elastic behavior 0 = elastic+no param update; 1 = elastic+param update; 2 = elastoplastic+no param update (default)
    int mFlag;

    Matrix mCe; //!< elastic tangent stiffness matrix
    Matrix mCep; //!< elastoplastic tangent stiffness matrix
    Vector mI1; //!< 2nd Order Identity Tensor 
    Matrix mIIvol; //!< IIvol = I1 tensor I1  
    Matrix mIIdev; //!< 4th Order Deviatoric Tensor

    Vector mState; //!< state vector for output

    //functions
    int  updateElasticParam(void); //updated Elastic Parameters based on mean stress 

    //plasticity integration routine
    void plastic_integrator(void);

    double Kiso(double alpha1);  // isotropic hardening function
    double Kisoprime(double alpha1); //
    double T(double alpha2);
    double deltaH(double dGamma);


    //parameters
    const double one3= 1.0 / 3.0 ;
    const double two3= 2.0 / 3.0 ;
    const double root23= sqrt(2.0/3.0) ;
  public:
    DruckerPrager(int tag, int classTag);
    DruckerPrager(int tag, int classTag, double bulk, double shear,
		  double s_y, double r, double r_bar, double Kinfinity, double Kinit, 
		  double d1, double d2, double H, double t, double massDen = 0.0, double atm = 101.325e3);

    // Elastic Constructor
    //	  DruckerPrager(int tag, double bulk, double shear);

    ~DruckerPrager();

    NDMaterial *getCopy(const std::string &type) const;

    int getMaterialStage(void) const;
    void updateMaterialStage(int);
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);

    NDMaterial *getCopy(void) const;
    const std::string &getType(void) const;
    int getOrder(void) const;

    Response *setResponse(const std::vector<std::string> &argv, Information &matInfo);
    int getResponse (int responseID, Information &matInformation);
    const Vector &getState(void) const;
    Matrix getValues(const std::string &, bool silent= false) const;

    int sendSelf(Communicator &);  
    int recvSelf(const Communicator &); 

    void Print(std::ostream &, int flag =0) const;

    int setParameter(const std::vector<std::string> &, Parameter &);
    int updateParameter(int parameterID, Information &info);

    void computeMTo(void);
    void initialize(); // initializes variables
    void setup(const int &elastFlag= 2);
    
    // Density.
    double getRho(void) const;
    void setRho(const double &r);
    // Bulk modulus.
    double getBulkModulus(void) const;
    void setBulkModulus(const double &);
    // Shear modulus.
    double getShearModulus(void) const;
    void setShearModulus(const double &);
    // Reference pressure.
    double getReferencePressure(void) const;
    void setReferencePressure(const double &);
    // Yield stress.
    double getYieldStress(void) const;
    void setYieldStress(const double &);
    // Failure surface and associativity. Volumetric term. 
    double getFailureSurfaceRho(void) const;
    void setFailureSurfaceRho(const double &);
    // Failure surface and associativity. Nonassociative flow term.
    double getFailureSurfaceRhoBar(void) const;
    void setFailureSurfaceRhoBar(const double &);
    // Isotropic hardening.
    double getIsotropicHardeningKinf(void) const;
    void setIsotropicHardeningKinf(const double &);
    double getIsotropicHardeningKo(void) const;
    void setIsotropicHardeningKo(const double &);
    double getIsotropicHardeningDelta(void) const;
    void setIsotropicHardeningDelta(const double &);
    // Kinematic hardening.
    double getKinematicHardeningH(void) const;
    void setKinematicHardeningH(const double &);
    double getKinematicHardeningTheta(void) const;
    void setKinematicHardeningTheta(const double &);
    // Tension softening.
    double getTensionSofteningDelta(void) const;
    void setTensionSofteningDelta(const double &);     
    double getMTo(void) const;
    void setMTo(const double &);     
  };
} // end XC namespace


#endif
