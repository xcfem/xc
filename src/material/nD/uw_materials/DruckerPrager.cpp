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
// $Date: 2010-02-04 20:50:27 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/DruckerPrager.cpp,v $
                                                                      
// Written: Kathryn Petek, Peter Mackenzie-Helnwein, and Pedro Arduino
// Created: 12/04
// Modified: 06/09
//
// Description: This file contains the implementation for the DruckerPrager class.
// In its original version this file was called DruckerPragerTensionCutoff.
//				
//		This implementation of Drucker-Prager allows for non-associative flow 
//		through the use of the rho_bar parameter.  It also includes linear 
//		kinematic hardening and linear and nonlinear isotropic hardening.
//

#include "DruckerPrager.h"
#include "DruckerPrager3D.h"
#include "DruckerPragerPlaneStrain.h"

#include <domain/mesh/element/utils/Information.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <domain/component/Parameter.h>



// #include <elementAPI.h>
// #define OPS_Export 

// static int numDruckerPragerMaterials = 0;


//! @brief Compute derived quantities.
void XC::DruckerPrager::computeMTo(void)
  {
    mK= mKref; // bulk modulus.
    mG= mGref;
    if(mrho == 0.0)
      { mTo= 1e10; }
    else
      { mTo= root23*msigma_y/mrho; }
  }

//full constructor
XC::DruckerPrager::DruckerPrager(int tag, int classTag, double bulk, double shear, double s_y, double r,
			                                        double r_bar, double Kinfinity, double Kinit, double d1,
			                                        double d2, double H, double t, double mDen, double atm)
  : NDMaterial(tag,classTag),
    mEpsilon(6), mEpsilon_n_p(6), mEpsilon_n1_p(6), mSigma(6),
    mBeta_n(6), mBeta_n1(6), mCe(6,6), mCep(6,6),
    mI1(6), mIIvol(6,6), mIIdev(6,6), mState(5)
  {
    massDen= mDen; // density.
    mPatm= atm; // reference stress first invariant (pressure)
    mK=  bulk; // bulk modulus.
    mG=  shear;
    msigma_y=  s_y; // yield stress.
    mrho= r; // volumetric term (failure surface and associativity)
    mrho_bar=  r_bar; // nonassociative flow term (failure surface and associativity)
    mKinf= Kinfinity; // isotropic hardening
    mKo= Kinit; // isotropic hardening
    mdelta1=  d1; // isotropic hardening
    mdelta2=  d2;
    mHard=  H;
    mtheta=  t;
    setup(2);
  }


//null constructor
XC::DruckerPrager::DruckerPrager(int tag, int classTag) 
  : NDMaterial(tag, classTag),
    mEpsilon(6),  mEpsilon_n_p(6), mEpsilon_n1_p(6),
    mSigma(6), mBeta_n(6), mBeta_n1(6),
    mCe(6,6), mCep(6,6), mI1(6),
    mIIvol(6,6), mIIdev(6,6),mState(5)
  {
    massDen=  0.0;
    mKref=  0.0;
    mGref=  0.0;
    mPatm=  101.325e3; // one atmosphere of pressure.
    mK=  0.0;
    mG=  0.0;
    msigma_y=  1e+10;
    mrho=  0.0;
    mrho_bar=  0.0;
    mKinf=  0.0;
    mKo=  0.0;
    mdelta1=  0.0;
    mdelta2=  0.0;
    mHard=  0.0;
    mtheta=  0.0;
    
    mTo=  0.0;
    mElastFlag= 2;
    this->initialize();
  }

//! @brief Destructor.
XC::DruckerPrager::~DruckerPrager(void)
  {}

//zero internal variables
void XC::DruckerPrager::initialize(void)
  {
    mEpsilon.Zero();
    mEpsilon_n_p.Zero();
    mEpsilon_n1_p.Zero();

    mSigma.Zero();
    mBeta_n.Zero();
    mBeta_n1.Zero();

    mAlpha1_n= 0.0;
    mAlpha1_n1= 0.0;
    mAlpha2_n= 0.0;
    mAlpha2_n1= 0.0;
    mFlag= 1;

    mHprime= (1-mtheta)*mHard;

    // 2nd order Identity Tensor
    mI1.Zero();
    mI1(0)= 1;
    mI1(1)= 1;
    mI1(2)= 1;

    // 4th order Volumetric Tensor
    // IIvol= I1 tensor I1
    mIIvol.Zero();
    mIIvol(0,0)= 1;
    mIIvol(0,1)= 1;
    mIIvol(0,2)= 1;
    mIIvol(1,0)= 1;
    mIIvol(1,1)= 1;
    mIIvol(1,2)= 1;
    mIIvol(2,0)= 1;
    mIIvol(2,1)= 1;
    mIIvol(2,2)= 1;

    // 4th order Deviatoric Tensor
    // Note:  this is the contravariant form!
    //        usable for s^a= 2G * IIdev^ab * epsilon_b
    // (Need a different form for s^a= IIdev ^a_b * sigma^a)
    mIIdev.Zero();
    mIIdev(0,0)= two3;
    mIIdev(0,1)= -one3;
    mIIdev(0,2)= -one3;
    mIIdev(1,0)= -one3;
    mIIdev(1,1)= two3;
    mIIdev(1,2)= -one3;
    mIIdev(2,0)= -one3;
    mIIdev(2,1)= -one3;
    mIIdev(2,2)= two3;
    mIIdev(3,3)= 0.5;
    mIIdev(4,4)= 0.5;
    mIIdev(5,5)= 0.5;

    mCe= mK * mIIvol + 2*mG*mIIdev;
    mCep= mCe;
    mState.Zero();
  }

void XC::DruckerPrager::setup(const int &elastFlag)
  {
    computeMTo();
    // set the elastic flag
    //  0= elastic+no param update; 1= elastic+param update; 2= elastoplastic+no param update (default)
    mElastFlag= elastFlag;

    // Use these values to deactivate yield surface 1 - Create Pure Tension Cutoff
    //msigma_y= 1e10;
    //mTo= 100;
    this->initialize();    
  } 

XC::NDMaterial *XC::DruckerPrager::getCopy(const std::string &type) const
  {
    if((type=="PlaneStrain2D") || (type=="PlaneStrain"))
      {
	DruckerPragerPlaneStrain *clone= new DruckerPragerPlaneStrain(this->getTag(), mK, mG, msigma_y, mrho, mrho_bar, mKinf, mKo,mdelta1, mdelta2, mHard, mtheta, massDen, mPatm);
	return clone;
      }
    else if((type=="ThreeDimensional") || (type=="3D"))
      {  
	DruckerPrager3D *clone= new DruckerPrager3D(this->getTag(),mK, mG, msigma_y, mrho, mrho_bar, mKinf, mKo,mdelta1, mdelta2, mHard, mtheta, massDen, mPatm);
        return clone;
      }
    else
      {
  	std::cerr << "DruckerPrager::getCopy failed to get copy: " << type << std::endl;
  	return nullptr;
      }
  }

//! @brief get density.
double XC::DruckerPrager::getRho(void) const
  { return massDen; }

//! @brief set density.
void XC::DruckerPrager::setRho(const double &r)
  { massDen= r; }  

//! @brief get bulk modulus.
double XC::DruckerPrager::getBulkModulus(void) const
  { return mKref; }

//! @brief set bulk modulus.
void XC::DruckerPrager::setBulkModulus(const double &d)
  { mKref= d; }

//! @brief get shear modulus.
double XC::DruckerPrager::getShearModulus(void) const
  { return mGref; }

//! @brief set shear modulus.
void XC::DruckerPrager::setShearModulus(const double &d)
  { mGref= d; }

//! @brief get reference pressure.
double XC::DruckerPrager::getReferencePressure(void) const
  { return mPatm; }

//! @brief set reference pressure.
void XC::DruckerPrager::setReferencePressure(const double &d)
  { mPatm= d; }

//! @brief get yield stress.
double XC::DruckerPrager::getYieldStress(void) const
  { return msigma_y; }

//! @brief set yield stress.
void XC::DruckerPrager::setYieldStress(const double &d)
  { msigma_y= d; }

//! @brief Failure surface and associativity. Get volumetric term.
double XC::DruckerPrager::getFailureSurfaceRho(void) const
  { return mrho; }

//! @brief Failure surface and associativity. Set volumetric term.
void XC::DruckerPrager::setFailureSurfaceRho(const double &d)
  { mrho= d; }

//! @brief Failure surface and associativity. Get nonassociative flow term.
double XC::DruckerPrager::getFailureSurfaceRhoBar(void) const
  { return mrho_bar; }

//! @brief Failure surface and associativity. Set nonassociative flow term.
void XC::DruckerPrager::setFailureSurfaceRhoBar(const double &d)
  { mrho_bar= d; }

//! @brief Isotropic hardening. Get Kinf.
double XC::DruckerPrager::getIsotropicHardeningKinf(void) const
  { return mKinf; }
//! @brief Isotropic hardening. Set Kinf.
void XC::DruckerPrager::setIsotropicHardeningKinf(const double &d)
  { mKinf= d; }
//! @brief Isotropic hardening. Get Ko.
double XC::DruckerPrager::getIsotropicHardeningKo(void) const
  { return mKo; }
//! @brief Isotropic hardening. Set Ko.
void XC::DruckerPrager::setIsotropicHardeningKo(const double &d)
  { mKo= d; }
//! @brief Isotropic hardening. Get delta1.
double XC::DruckerPrager::getIsotropicHardeningDelta(void) const
  { return mdelta1; }
//! @brief Isotropic hardening. Set delsta1.
void XC::DruckerPrager::setIsotropicHardeningDelta(const double &d)
  { mdelta1= d; }

//! @brief Kinematic hardening. Get H.
double XC::DruckerPrager::getKinematicHardeningH(void) const
  { return mHard; }
//! @brief Kinematic hardening. Set H.
void XC::DruckerPrager::setKinematicHardeningH(const double &d)
  { mHard= d; }
//! @brief Kinematic hardening. Get theta.
double XC::DruckerPrager::getKinematicHardeningTheta(void) const
  { return mtheta; }
//! @brief Kinematic hardening. Set theta.
void XC::DruckerPrager::setKinematicHardeningTheta(const double &d)
  { mtheta= d; }

//! @brief Get tension softening delta.
double XC::DruckerPrager::getTensionSofteningDelta(void) const
  { return mdelta2; }
//! @brief Set tension softening delta.
void XC::DruckerPrager::setTensionSofteningDelta(const double &d)
  { mdelta2= d; }

//! @brief Get mTo.
double XC::DruckerPrager::getMTo(void) const
  { return mTo; }
//! @brief Set tension softening delta.
void XC::DruckerPrager::setMTo(const double &d)
  { mTo= d; }

//! @brief Commit the state of the material.
int XC::DruckerPrager::commitState(void)
  {
    mEpsilon_n_p= mEpsilon_n1_p;
    mAlpha1_n= mAlpha1_n1; 
    mAlpha2_n= mAlpha2_n1; 
    mBeta_n= mBeta_n1;

    return 0;
  }
 
int XC::DruckerPrager::revertToLastCommit (void)
  { return 0; }

//! @brief Revert the material to its initial state.
int XC::DruckerPrager::revertToStart(void)
  {
    int retval= NDMaterial::revertToStart();
    // if(ops_InitialStateAnalysis)
    //   {
    // 		// do nothing, keep state variables from last step
    //   }
    // else
    //   {
	// normal call for revertToStart (not initialStateAnalysis)
    	this->initialize();
      // }
    return retval;
  }

XC::NDMaterial *XC::DruckerPrager::getCopy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << std::endl; 
    exit(-1);
    return 0;
  }

const std::string &XC::DruckerPrager::getType(void) const
  {
    static std::string retval= "";
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << std::endl; 
    exit(-1);
    return retval;
  }

// ! @brief Return the order of the material.
int XC::DruckerPrager::getOrder(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; not implemented yet."
              << std::endl; 
    exit(-1);
    return 0;
  }

//! @brief Return the initial strain.
const XC::Vector &XC::DruckerPrager::getInitialGeneralizedStrain(void) const
  {
    static Vector retval(6,0.0); //No initial strain implemented yet.
    return retval;
  }

void XC::DruckerPrager::setInitialGeneralizedStrain(const Vector &eps)
  {
    // Initial strain not implemented yet, so:
    if(eps.NormInf()>1e-8) // if not zero issue an error message.
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; this material doesn't implement intitial strain."
                << std::endl; 
  }

//! @brief Plasticity integration routine
void XC::DruckerPrager::plastic_integrator(void) 
  {
		
    // set trial state:

    // epsilon_n1_p_trial= ..._n1_p= ..._n_p
    mEpsilon_n1_p= mEpsilon_n_p;

    // alpha1_n+1_trial
    mAlpha1_n1= mAlpha1_n;
    // alpha2_n+1_trial
    mAlpha2_n1= mAlpha2_n;

    // beta_n+1_trial
    mBeta_n1= mBeta_n;

    // epsilon_elastic= epsilon_n+1 - epsilon_n_p
    Vector epsilon_e= mEpsilon - mEpsilon_n1_p;

    // trial stress
    mSigma= mCe*epsilon_e;

    // deviator stress tensor: s= 2G * IIdev * epsilon_e
    //I1_trial
    double Invariant_1= ( mSigma(0) + mSigma(1) + mSigma(2) );
    

    // s_n+1_trial
    Vector s= mSigma - (Invariant_1/3.0)*mI1;

    //eta_trial= s_n+1_trial - beta_n;
    Vector eta= s - mBeta_n;
		
    // compute yield function value (contravariant norm)
    double norm_eta= sqrt(eta(0)*eta(0) + eta(1)*eta(1) + eta(2)*eta(2) + 2*(eta(3)*eta(3) + eta(4)*eta(4) + eta(5)*eta(5)));

    // f1_n+1_trial
    double f1= norm_eta + mrho*Invariant_1 - root23*Kiso(mAlpha1_n1);

    // f2_n+1_trial
    double f2= Invariant_1 - T(mAlpha2_n1);

    // update elastic bulk and shear moduli 
    this->updateElasticParam();

    // check trial state
    bool okay;	// boolean variable to ensure satisfaction of multisurface kuhn tucker conditions

    int count= 1;
    Vector Jact(2);
    double Invariant_ep;
    double norm_ep, norm_dev_ep;
    const double fTOL= 0.0;
    const double gTOL= -1.0e-10;
    if(((f1<=fTOL) && (f2<=fTOL)) || (mElastFlag < 2))
      {
	okay= true;
	// trial state= elastic state - don't need to do any updates.
	mCep= mCe;
	count= 0;

	// set state variables for recorders
        Invariant_ep= mEpsilon_n1_p(0)+mEpsilon_n1_p(1)+mEpsilon_n1_p(2);

	norm_ep= sqrt(mEpsilon_n1_p(0)*mEpsilon_n1_p(0) + mEpsilon_n1_p(1)*mEpsilon_n1_p(1) + mEpsilon_n1_p(2)*mEpsilon_n1_p(2) + 0.5*(mEpsilon_n1_p(3)*mEpsilon_n1_p(3) + mEpsilon_n1_p(4)*mEpsilon_n1_p(4) + mEpsilon_n1_p(5)*mEpsilon_n1_p(5)));

        Vector dev_ep= mEpsilon_n1_p - one3*Invariant_ep*mI1;

        norm_dev_ep= sqrt(dev_ep(0)*dev_ep(0) + dev_ep(1)*dev_ep(1) + dev_ep(2)*dev_ep(2) + 0.5*(dev_ep(3)*dev_ep(3) + dev_ep(4)*dev_ep(4) + dev_ep(5)*dev_ep(5)));

	mState(0)= Invariant_1;
	mState(1)= norm_eta;
        mState(2)= Invariant_ep;
        mState(3)= norm_dev_ep;
	mState(4)= norm_ep;
	return;
      }
    else
      {
	// plastic correction required
	okay= false;

	// determine number of active surfaces.  size & fill Jact
	if( (f1 > fTOL ) && (f2 <= fTOL) )
	  {
	    // f1 surface only
	    Jact(0)= 1;
	    Jact(1)= 0;
	  }
	else if( (f1 <= fTOL ) && (f2 > fTOL) )
	  {
	    // f2 surface only
	    Jact(0)= 0;
	    Jact(1)= 1;
	  }
	else if( (f1 > fTOL ) && (f2 > fTOL) )
	  {
	    // both surfaces active
	    Jact(0)= 1;
	    Jact(1)= 1;
	  }
      } 

    int loopCounter= 0;
    double alpha1= mAlpha1_n; // hardening parameter for DP surface
    double alpha2= mAlpha2_n; // hardening parameter for tension cut-off
    Matrix g(2,2); // jacobian of the corner region (return map)
    Matrix g_contra(2,2); // inverse of jacobian of the corner region
    Vector n(6); // normal to the yield surface in strain space
    Vector R(2); // residual vector    
    Vector gamma(2); // vector of consistency parameters
    Vector dgamma(2);	// incremental vector of consistency parameters
    //-----------------MultiSurface Placity Return Map-------------------------
    while(!okay)
      {
	//  n= eta / norm_eta;  (contravaraint)
	if(norm_eta < 1.0e-13)
	  { n.Zero(); }
	else
	  { n= eta/norm_eta; }

	// initialize R, gamma1, gamma2, dgamma1, dgamma2= 0
	R.Zero();  
	gamma.Zero(); 
	dgamma.Zero();
	// initialize g such that det(g)= 1
	g(0,0)= 1; g(1,0)= 0;
	g(0,1)= 0; g(1,1)= 1; 

	// Newton procedure to compute nonlinear gamma1 and gamma2
	//initialize terms
	for(int i= 0; i < 2; i++)
	  {
	    if(Jact(i)== 1)
	      {
	        R(0)= norm_eta - (2*mG + two3*mHprime)*gamma(0) + mrho*Invariant_1 - 9*mK*mrho*mrho_bar*gamma(0) - 9*mK*mrho*gamma(1) - root23*Kiso(alpha1);
		g(0,0)= -2*mG - two3*(mHprime + Kisoprime(alpha1)) - 9*mK*mrho*mrho_bar;
	      }
	    else if(Jact(i)== 2)
	      {
	        R(1)= Invariant_1 - 9*mK*mrho_bar*gamma(0) - 9*mK*gamma(1) - T(alpha2);
		g(1,1)= -9*mK + mdelta2*T(alpha2);
	      }
	  }
	if(Jact(0)== 1 && Jact(1)== 1)
	  {
		g(0,1)= -9*mK*mrho;
		g(1,0)= mrho_bar*(-9*mK + mdelta2*T(alpha2));
	  } 
	g.Invert(g_contra);

	// iteration counter
	int m= 0;
	//iterate
	while ((fabs(R.Norm()) > 1e-10) && (m < 10))
	  {
	    dgamma= -1*g_contra * R;
	    gamma += dgamma;

	    //update alpha1 and alpha2
	    alpha1= mAlpha1_n + root23*gamma(0);
	    alpha2= mAlpha2_n + mrho_bar*gamma(0) + gamma(1);

	    // reset g & R matrices
	    g(0,0)= 1;
	    g(1,1)= 1;
	    g(1,0)= 0;
	    g(0,1)= 0;
	    R.Zero();
	    for(int i= 0; i < 2; i++)
	      {
		if(Jact(i)== 1)
		  {
		    R(0)= norm_eta - (2*mG + two3*mHprime)*gamma(0) + mrho*Invariant_1 - 9*mK*mrho*mrho_bar*gamma(0) - 9*mK*mrho*gamma(1) - root23*Kiso(alpha1);
		    g(0,0)= -2*mG - two3*(mHprime + Kisoprime(alpha1)) - 9*mK*mrho*mrho_bar;
		  }
		else if(Jact(i)== 2)
		  {
		    R(1)= Invariant_1 - 9*mK*mrho_bar*gamma(0) - 9*mK*gamma(1) - T(alpha2);
		    g(1,1)= -9*mK + mdelta2*T(alpha2);
		  }
	      }
	    if(Jact(0)== 1 && Jact(1)== 1)
	      {
		g(0,1)= -9*mK*mrho;
		g(1,0)= mrho_bar*(-9*mK + mdelta2*T(alpha2));
	      } 
	    g.Invert(g_contra);
	    m++;
	  } // end while m<10

	// check maintain Kuhn-Tucker conditions
	f1= norm_eta - (2*mG + two3*mHprime)*gamma(0) + mrho*Invariant_1 
					-9*mK*mrho*mrho_bar*gamma(0) - 9*mK*mrho*gamma(1) - root23*Kiso(alpha1);
	f2= Invariant_1 - 9*mK*mrho_bar*gamma(0) - 9*mK*gamma(1) - T(alpha2);

	if( count > 100 )
	  {
	    okay= true;
            break;
	  }

	// check active surfaces
	if((Jact(0)== 1) && (Jact(1)== 0))
	  {
	    // f2 may be > or < f2_tr because of softening of f2 related to alpha1
	    if(f2 >= fTOL)
	      {
		// okay= false;
		Jact(0)= 1;
		Jact(1)= 1;
		count += 1;

	      }
	    else
	      { okay= true; }
	  }
	else if((Jact(0)== 0) && (Jact(1)== 1))
	  {
	    // f1 will always be less than f1_tr
	    okay= true;
	  }
	else if((Jact(0)== 1) && (Jact(1)== 1))
	  {
	    if((gamma(0) <= gTOL) && (gamma(1) > gTOL))
	      {
		// okay= false;
		Jact(0)= 0;
		Jact(1)= 1;
		count += 1;
	      }
	    else if((gamma(0) > gTOL) && (gamma(1) <= gTOL))
	      {
		// okay= false;
		Jact(0)= 1;
		Jact(1)= 0;
		count += 1;
	      }
	    else if((gamma(0) > gTOL) && (gamma(1) > gTOL))
	      { okay= true; }
	  }
	if( (count > 3) && (!okay) )
	  {
		Jact(0)= 1;
		Jact(1)= 1;
		count += 100;
	  }

	if( count > 3 )
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; Jact= " << Jact
		      << "count= " << count << std::endl;
	  }
	loopCounter++;
	if(loopCounter>100)
	  {
	    std::cerr << getClassName() << "::" << __FUNCTION__
		      << "; too much loops. Something went wrong. Exiting"
		      << "count= " << count
		      << "loop counter: " << loopCounter << std::endl;
	    exit(-1);
	  }
      } // end of while(!okay) loop

    //update everything and exit!

    // update alpha1 and alpha2
    mAlpha1_n1= alpha1;
    mAlpha2_n1= alpha2;

    //update epsilon_n1_p
    //first calculate n_covar
    // n_a= G_ab * n^b= covariant
    Vector n_covar(6);
    n_covar(0)= n(0);
    n_covar(1)= n(1);
    n_covar(2)= n(2);
    n_covar(3)= 2*n(3);
    n_covar(4)= 2*n(4);
    n_covar(5)= 2*n(5);
    mEpsilon_n1_p= mEpsilon_n_p + (mrho_bar*gamma(0) + gamma(1))*mI1 + gamma(0)*n_covar;

           
    Invariant_ep= mEpsilon_n1_p(0)+mEpsilon_n1_p(1)+mEpsilon_n1_p(2);

    norm_ep= sqrt(mEpsilon_n1_p(0)*mEpsilon_n1_p(0) + mEpsilon_n1_p(1)*mEpsilon_n1_p(1) + mEpsilon_n1_p(2)*mEpsilon_n1_p(2)
	       + 0.5*(mEpsilon_n1_p(3)*mEpsilon_n1_p(3) + mEpsilon_n1_p(4)*mEpsilon_n1_p(4) + mEpsilon_n1_p(5)*mEpsilon_n1_p(5)));

    Vector dev_ep= mEpsilon_n1_p - one3*Invariant_ep*mI1;

    norm_dev_ep= sqrt(dev_ep(0)*dev_ep(0) + dev_ep(1)*dev_ep(1) + dev_ep(2)*dev_ep(2) + 0.5*(dev_ep(3)*dev_ep(3) + dev_ep(4)*dev_ep(4) + dev_ep(5)*dev_ep(5)));

    // update sigma
    mSigma-= (3*mK*mrho_bar*gamma(0) + 3*mK*gamma(1))*mI1 + 2*mG*gamma(0)*n;

    s-= 2*mG*gamma(0) * n;
    Invariant_1-= 9*mK*mrho_bar*gamma(0) + 9*mK*gamma(1);
    //mSigma= s + Invariant_1/3.0 * mI1;

    //update beta_n1
    mBeta_n1= mBeta_n - (two3*mHprime*gamma(0))*n;

    //eta_n+1= s_n+1 - beta_n+1;
    eta= s - mBeta_n1;
    norm_eta= sqrt(eta(0)*eta(0) + eta(1)*eta(1) + eta(2)*eta(2) + 2*(eta(3)*eta(3) + eta(4)*eta(4) + eta(5)*eta(5)));
			
    // update Cep
    // note: Cep is contravariant
    Vector b1(6);
    Vector b2(6);
    if((Jact(0)== 1) && (Jact(1)== 0))
      {
	b1= 2*mG*n + 3*mK*mrho*mI1;
	b2.Zero();
      }
    else if((Jact(0)== 0) && (Jact(1)== 1))
      {
	b1.Zero();
	b2= 3*mK*mI1;
      }
    else if((Jact(0)== 1) && (Jact(1)== 1))
      {
	b1= 2*mG*n + 3*mK*mrho*mI1;
	b2= 3*mK*mI1;
      }

    const Vector temp1= g_contra(0,0)*b1 + g_contra(0,1)*b2;  
    const Vector temp2= mrho_bar*temp1 + g_contra(1,0)*b1 + g_contra(1,1)*b2;

    // Avoid zero division error.
    double gamma0_norm_eta= gamma(0); 
    if(norm_eta==0)
      {
	if(gamma0_norm_eta<gTOL)
	  { gamma0_norm_eta= 0.0; }
	else
	  { gamma0_norm_eta*= 6.023e23; } // Very big number.
      }
    else
      gamma0_norm_eta/=norm_eta;
    double NormCep= 0.0;
    for(int i= 0; i < 6; i++)
      {
	for(int j= 0; j < 6; j++)
	  {
	    mCep(i,j)= mCe(i,j)
			      + 3*mK * mI1(i)*temp2(j)  
			      + 2*mG * n(i)*temp1(j)
			      - 4*mG*mG*gamma0_norm_eta * (mIIdev(i,j) - n(i)*n(j));
	    NormCep += mCep(i,j)*mCep(i,j);
	  }
      }
    if( NormCep < 1e-10)
      {
	mCep= 1.0e-3 * mCe;
	std::cerr << "NormCep= " << NormCep << std::endl;
      }
    mState(0)= Invariant_1;
    mState(1)= norm_eta;
    mState(2)= Invariant_ep;
    mState(3)= norm_dev_ep;
    mState(4)= norm_ep;
  }

int XC::DruckerPrager::updateElasticParam(void)
  {
    double Sigma_mean= 0.0;
    if(mElastFlag == 1 && mFlag == 1)
      {
        Sigma_mean= -one3*(mSigma(0)+mSigma(1)+mSigma(2));
        if(Sigma_mean < 0.0)
	  Sigma_mean= 0.0;  // prevents modulus update for cases where tension exists 
        mK= mKref * pow(1+(Sigma_mean/mPatm), 0.5);
        mG= mGref * pow(1+(Sigma_mean/mPatm), 0.5);
	mCe= mK * mIIvol + 2*mG*mIIdev;
        mFlag= 0;
	    //std::cerr << "Plastic Integrator -->" << "K= " << mK  << "  G=" << mG << std::endl;
      }
    else if( mElastFlag != 1 )
      { mFlag= 1; }
    return 0;
  }

double XC::DruckerPrager::Kiso(double alpha1)
  {
    return msigma_y + mtheta * mHard * alpha1 + (mKinf - mKo) * (1 - exp(-mdelta1 * alpha1));
}


double XC::DruckerPrager::Kisoprime(double alpha1)
  {
    return mtheta * mHard + (mKinf - mKo) * mdelta1*  exp(-mdelta1 * alpha1);
  }

double XC::DruckerPrager:: T(double alpha2) 
  {
    return mTo * exp(-mdelta2 * alpha2);
  }


double XC::DruckerPrager::deltaH(double dGamma)
  { return mHprime * root23 * dGamma; }

//! @brief Return vector of state variables for output
const XC::Vector &XC::DruckerPrager::getState(void) const
  { return mState; }

//! @brief Returns material response.
//! @param cod: name of the requested value.
//! @param silent: if true don't complain about non-existen property.
XC::Matrix XC::DruckerPrager::getValues(const std::string &cod, bool silent) const
  {
    Matrix retval;
    if(cod == "state")
      {
	const Vector state= this->getState();
	const size_t sz= state.Size();
	retval.resize(1,sz);
	retval.putRow(0,state);
      }
    else if(cod == "stage")
      {
	const int stage= this->getMaterialStage();
	retval.resize(1,1);
	retval(0,0)= static_cast<double>(stage);
      }
    else
      retval= NDMaterial::getValues(cod, silent);
    return retval;
  }

XC::Response *XC::DruckerPrager::setResponse(const std::vector<std::string> &argv, Information &matInfo)
  {
    if(argv[0] == "stress" || argv[0] == "stresses")
      return new MaterialResponse(this, 1, this->getStress());
    else if(argv[0] == "strain" || argv[0] == "strains")
      return new MaterialResponse(this, 2, this->getStrain());
    else if(argv[0] == "state")
      return new MaterialResponse(this, 3, this->getState());
    else
      return nullptr;
  }

int XC::DruckerPrager::getResponse(int responseID, Information &matInfo)
  {
    switch (responseID)
      {
      case -1:
	      return -1;
      case 1:
	      if(matInfo.theVector != 0)
		      *(matInfo.theVector)= getStress();
	      return 0;
      case 2:
	      if(matInfo.theVector != 0)
		      *(matInfo.theVector)= getStrain();
	      return 0;
      case 3:
	      if(matInfo.theVector != 0)
		      *(matInfo.theVector)= getState();
	      return 0;
      default:
	      return -1;
    }
  }

int XC::DruckerPrager::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv[0]=="materialState")
      {
        // switch elastic/plastic state
	return param.addObject(5,this);
      }
    else if(argv[0]=="frictionalStrength")
      {
        // update rho parameter
	return param.addObject(7,this);
      }
    else if(argv[0]=="nonassociativeTerm")
      {
        // update nonassociative rho_bar parameter
	return param.addObject(8,this);
      }
    else if(argv[0]=="cohesiveIntercept")
      {
        // update zero confinement yield strength
	return param.addObject(9,this);
      }
    else if(argv[0]=="shearModulus")
      {
        // update shear modulus
        return param.addObject(10,this);
      }
    else if(argv[0]=="bulkModulus")
      {
        // update bulk modulus
	return param.addObject(11,this);
      }
    else if(argv[0]=="updateMaterialStage")
      { return -1; }
    else
      {
        // invalid parameter type
	std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING: invalid parameter command for DruckerPrager nDMaterial with tag: "
		  << this->getTag() << std::endl;
        return -1;
      }
    return -1;
  }


//! @brief return the material stage (0:elastic 1:plastic).
int XC::DruckerPrager::getMaterialStage(void) const
  { return mElastFlag; }

//! @brief Update material stage.
//! @param stage: 0= elastic+no param update; 1= elastic+param update; 2= elastoplastic+no param update (default).
void XC::DruckerPrager::updateMaterialStage(int stage)
  {
    if(stage !=0 && stage !=1 && stage !=2)
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; WARNING stage must be 0, 1 or 2."
		  << std::endl;
        exit(-1);
      }
    mElastFlag= stage;
  }

int XC::DruckerPrager::updateParameter(int responseID, Information &info)
  {
    if(responseID == 5)
      {
        // materialState called - update mElasticFlag
	updateMaterialStage(static_cast<int>(info.theDouble));
      }
    else if(responseID == 7)
      {
        // frictionalStrength called - update rho and tension cutoff
	mrho= info.theDouble;
	if(mrho == 0.0)
	  { mTo= 1e10; }
	else
	  { mTo= root23*msigma_y/mrho;	}
      }
    else if(responseID == 8)
      {
        // nonassociativeTerm called - update rho_bar
	mrho_bar= info.theDouble;
      }
    else if(responseID == 9)
      {
        // cohesiveIntercept called - update sigma_y and tension cutoff
	msigma_y= info.theDouble;
	if(mrho == 0.0)
	  { mTo= 1e10; }
	else
	  { mTo= root23*msigma_y/mrho; }
      }
    else if(responseID == 10)
      {
        // shearModulus called - update G and Ce
        mG= info.theDouble;
	mCe= mK*mIIvol + 2*mG*mIIdev;
      }
    else if(responseID == 11)
      {
        // bulkModulus called - update K and Ce
        mK= info.theDouble;
	mCe= mK*mIIvol + 2*mG*mIIdev;
      }
    return 0;
  }

int XC::DruckerPrager::sendSelf(Communicator &)
  {
    std::cerr << "DruckerPrager::sendSelf not yet implemented" << std::endl;
// 	int res= 0;

//     // place data in a vector
// 	static Vector data(45);
// 	data(0)= this->getTag();
//     data(1)= mKref;
//     data(2)= mGref;
//     data(3)= mK;
//     data(4)= mG;
//     data(5)= msigma_y;
//     data(6)= mrho;
//     data(7)= mrho_bar;
//     data(8)= mKinf;
//     data(9)= mKo;
//     data(10)= mdelta1;
//     data(11)= mdelta2;
//     data(12)= mHard;
//     data(13)= mtheta;
//     data(14)= massDen;
// 	data(15)= mPatm;
// 	data(16)= mTo;
// 	data(17)= mHprime;	
//     data(18)= mAlpha1_n;
//     data(19)= mAlpha2_n;
// 	data(20)= mElastFlag;
// 	data(21)= mFlag;

// 	data(22)= mEpsilon(0);
// 	data(23)= mEpsilon(1);
// 	data(24)= mEpsilon(2);
// 	data(25)= mEpsilon(3);
// 	data(26)= mEpsilon(4);
// 	data(27)= mEpsilon(5);

// 	data(28)= mEpsilon_n_p(0);
// 	data(29)= mEpsilon_n_p(1);
// 	data(30)= mEpsilon_n_p(2);
// 	data(31)= mEpsilon_n_p(3);
// 	data(32)= mEpsilon_n_p(4);
// 	data(33)= mEpsilon_n_p(5);

// 	data(34)= mBeta_n(0);
// 	data(35)= mBeta_n(1);
// 	data(36)= mBeta_n(2);
// 	data(37)= mBeta_n(3);
// 	data(38)= mBeta_n(4);
// 	data(39)= mBeta_n(5);

// 	data(40)= mState(0);
// 	data(41)= mState(1);
// 	data(42)= mState(2);
// 	data(43)= mState(3);
// 	data(44)= mState(4);
	
// 	res= comm.sendVector(this->getDbTag(), commitTag, data);
// 	if(res < 0) {
// 		std::cerr << "WARNING: XC::DruckerPrager::sendSelf - failed to send vector to communicator" << std::endl;
// 		return -1;
//     }
    
     return 0;
 }

int XC::DruckerPrager::recvSelf(const Communicator &)    
 {
    std::cerr << "DruckerPrager::sendSelf not yet implemented" << std::endl;
// 	int res= 0;

// 	// receive data
// 	static Vector data(45);
// 	res= comm.recvVector(this->getDbTag(), commitTag, data);
// 	if(res < 0) {
// 		std::cerr << "WARNING: XC::DruckerPrager::recvSelf - failed to receive vector from communicator" << std::endl;
// 		return -1;
// 	}

//     // set member variables
// 	this->setTag((int)data(0));
// 	mKref= data(1);
//     mGref= data(2);
//     mK= data(3);
//     mG= data(4);
//     msigma_y= data(5);
//     mrho= data(6);
//     mrho_bar= data(7);
//     mKinf= data(8);
//     mKo= data(9);
//     mdelta1= data(10);
//     mdelta2= data(11);
//     mHard= data(12);
//     mtheta= data(13);
//     massDen= data(14);
// 	mPatm= data(15);
// 	mTo= data(16);
// 	mHprime= data(17);
//     mAlpha1_n= data(18);
//     mAlpha2_n= data(19);
// 	mElastFlag= (int)data(20);
// 	mFlag= (int)data(21);

// 	mEpsilon(0)= data(22);
// 	mEpsilon(1)= data(23);
// 	mEpsilon(2)= data(24);
// 	mEpsilon(3)= data(25);
// 	mEpsilon(4)= data(26);
// 	mEpsilon(5)= data(27);

// 	mEpsilon_n_p(0)= data(28);
// 	mEpsilon_n_p(1)= data(29);
// 	mEpsilon_n_p(2)= data(30);
// 	mEpsilon_n_p(3)= data(31);
// 	mEpsilon_n_p(4)= data(32);
// 	mEpsilon_n_p(5)= data(33);

// 	mBeta_n(0)= data(34);
// 	mBeta_n(1)= data(35);
// 	mBeta_n(2)= data(36);
// 	mBeta_n(3)= data(37);
// 	mBeta_n(4)= data(38);
// 	mBeta_n(5)= data(39);

// 	mState(0)= data(40);
// 	mState(1)= data(41);
// 	mState(2)= data(42);
// 	mState(3)= data(43);
// 	mState(4)= data(44);

// 	mCe= mK*mIIvol + 2*mG*mIIdev;
//     mCep= mCe;

     return 0;
  }

void XC::DruckerPrager::Print(std::ostream &s, int flag ) const
  {
    s << "DruckerPrager" << std::endl;
  }

