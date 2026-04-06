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
                                                                        
//----------------------------------------------------------------------------------------------------------------------------
// Developed by:
// Nikola D. Tosic (ntosic@imk.grf.bg.ac.rs)
// Department for Materials and Structure, Faculty of Civil Engineering, University of Belgrade, Serbia
// Adam M. Knaack (adam.knaack@schaefer-inc.com) 
// Schaefer-Inc, Cincinnati, Ohio, USA
// Yahya C. Kurama (ykurama@nd.edu)
// Department of Civil and Environmental Engineering and Earth Sciences, College of Engineering, University of Notre Dame, Notre Dame, Indiana, USA
//----------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------
// Created: 2019
// Last updated: 2019
//----------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------
// Description: This file contains the source code of TDConcreteMC10NL. 
// TDConcreteMC10NL is a time-dependent concrete material model that calculates
// creep and shrinkage strains.
/*-------------------------------
! Concrete Compression - Non-linear with linear softening
! Concrete Tension - Tamai, S., Shima, H., Izumo, J., Okamura, H. 1988. Average Stress-Strain Relationship in Post Yield Range of Steel Bar in Concrete, Concrete Library of JSCE, No. 11, 117-129.
! Concrete Creep - Linear superposition of creep coefficient, Model Code 2010 time function
! Concrete Shrinkage - Model Code 2010 time function
-------------------------------*/
// Detailed descriptions of the model and its implementation can be found in the following:
// (1) Knaack, A.M., Kurama, Y.C. 2018. Modeling Time-Dependent Deformations: Application for Reinforced Concrete Beams with 
//     Recycled Concrete Aggregates. ACI Structural J. 115, 175�190. doi:10.14359/51701153
// (2) Knaack, A.M., 2013. Sustainable concrete structures using recycled concrete aggregate: short-term and long-term behavior
//     considering material variability. PhD Dissertation, Civil and Environmental Engineering and Earth Sciences, University of Notre Dame, Notre Dame, Indiana, USA, 680 pp.
// A manual describing the use of the model and sample files can be found at:
// <https://data.mendeley.com/datasets/z4gxnhchky/3>
//----------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------------
// Disclaimer: This software is provided �as is�, without any warranties, expressed or implied. In no event shall the developers be liable for any claim, damages, or liability arising from or in connection with this software.
//----------------------------------------------------------------------------------------------------------------------------



#include "TDConcreteMC10NL.h"

//! @brief Sets initial values for the concrete parameters.
void XC::TDConcreteMC10NL::setup_parameters(void)
  {
    TDConcreteMC10Base::setup_parameters();

    //Change inputs into the proper sign convention: ntosic: changed
    fcu= -fabs(fcu);
    epscu= -fabs(epscu);
  }

XC::TDConcreteMC10NL::TDConcreteMC10NL(int tag)
  : TDConcreteMC10Base(tag, MAT_TAG_TDConcreteMC10NL)
  {}

//! @brief Constructor.
//! @param _fc: cylinder compressive strength (this is a dummy parameter since compression behavior is linear).
//! @param _fcu: stress at ultimate (crushing) strain.
//! @param _ft: the tensile strength (splitting or axial tensile strength should be input, rather than the flexural).
//! @param _Ets : tension softening stiffness (absolute value) (slope of the linear tension softening branch).
//! @param _Ec: modulus of elasticity (preferably at time of loading if there is a single loading age).
//! @param _Ecm: 28-day modulus, necessary for normalizing creep coefficient.
//! @param _beta: tension softening parameter.
//! @param _age: analysis time at initiation of drying (in days).
//! @param _tcast: analysis time corresponding to concrete casting in days (note: concrete will not be able to take on loads until the age of 2 days).
//! @param csp: concrete shrinkage parameters according to Model Code 10.
XC::TDConcreteMC10NL::TDConcreteMC10NL(int tag, double _fc, double _fcu, double _epscu, double _ft, double _Ets, double _Ec, double _Ecm, double _beta, double _age, double _tcast, const MC10CreepShrinkageParameters &csp): 
  TDConcreteMC10Base(tag, MAT_TAG_TDConcreteMC10NL, _fc, _ft, _Ets, _Ec, _Ecm, _beta, _age, _tcast, csp), fcu(_fcu)
  {
    epscu= _epscu;
    setup_parameters();
  }


XC::TDConcreteMC10NL::~TDConcreteMC10NL(void)
  {
  // Does nothing
  }

XC:: UniaxialMaterial *XC::TDConcreteMC10NL::getCopy(void) const
  { return new TDConcreteMC10NL(*this); }

//! @brief Get stress at ultimate (crushing) strain.
double XC::TDConcreteMC10NL::getFcu(void) const
  { return fcu; }

//! @brief Set stress at ultimate (crushing) strain.
void XC::TDConcreteMC10NL::setFcu(const double &d)
  { fcu= d; }

int XC::TDConcreteMC10NL::setTrialStrain(double trialStrain, double strainRate)
  {
    const double t= this->getCurrentTime();
    //double tol = 1.e-4; // 9/13
    //double test = 10.0; // 9/13
    //double sigI = 0.0;  // 9/13
    //int niter = 500;  // 9/13
	
    //std::cerr<<"\n trialStrain = "<<trialStrain;
    
	// Need to initialize count and initial stress/strain:
    /*
    if(creepControl == 0)
      { count= 0; }
    else if (creepControl==1 && count == 0)
      {	count= 1; }
    resize(count);
    */
	
    // Check casting age:
    if(creepShrinkageState.getCastingAge(t)<(2.0-0.0001))
      { //Assumed that concrete can only carry load once hardened at 2 days following casting
	creepShrinkageState.set_eps_cr_sh_m_total(0.0, 0.0, 0.0, 0.0, 0.0, trialStrain);
      }
    else
      { // Concrete has hardened and is ready to accept load
	// Initialize total strain:
	creepShrinkageState.set_total_strain(trialStrain); // Total strain.
	
	// Calculate shrinkage Strain:
	if (iter < 1)
	  {
	    creepShrinkageState.setShrinkBasic(this->setShrinkBasic(t)); //ntosic
	    creepShrinkageState.setShrinkDrying(this->setShrinkDrying(t)); //ntosic
	  }

    	// Calculate creep and mechanical strain, assuming stress remains constant in a time step:
	if(creepSteps.isCreepOn())
	  {
	    if (fabs(t-creepSteps.getLastTime()) <= 0.0001)
	      { //If t = t(i-1), use creep/shrinkage from last calculated time step
		creepShrinkageState.use_creep_shrinkage_from_last_commit();
		hstv.sig = setStress(creepShrinkageState.getMech(), hstv.e);
	      }
	    else
	      { // if the current calculation is a new time step
		//if (crackP_flag == 1 && hstv.sig >= 0.0) { //if cracking occurs and previous stress is positive, 
		// creep strain should not increase
		//	eps_cr = epsP_cr;
		//} else {
		//	eps_cr = setCreepStrain(t,hstv.sig);
		//}
		//if (t < tcast) {
		//std::cerr << "\nWARNING: TDConcrete loaded before tcast, creep and shrinkage not calculated" << std::endl;
		//	eps_sh = epsP_sh;
		//	eps_cr = epsP_cr;
		//	eps_m = eps_total - eps_cr - eps_sh;
		//	hstv.sig = setStress(eps_m, hstv.e);
		//} else {
		if(iter < 1)
		  {
                    creepShrinkageState.setCreepBasic(this->setCreepBasicStrain(t, hstv.sig)); 
		    creepShrinkageState.setCreepDrying(this->setCreepDryingStrain(t, hstv.sig));
		  }
		creepShrinkageState.update_mech_strain(); //ntosic
		hstv.sig = setStress(creepShrinkageState.getMech(), hstv.e);
		//}
	      }
	  }
	else
	  { //Static Analysis using previously converged time-dependent strains
	    creepShrinkageState.use_creep_shrinkage_from_last_commit();
	    hstv.sig = setStress(creepShrinkageState.getMech(), hstv.e);
	  }
	//
	//std::cerr<<"\n   eps_cr = "<<eps_cr;
	//std::cerr<<"\n   eps_sh = "<<eps_sh;
	//std::cerr<<"\n   eps_m = "<<eps_m;
	//std::cerr<<"\n   sig = "<<hstv.sig;
      }
    iter ++;
    return 0;
  }

double XC::TDConcreteMC10NL::setStress(double strain, double &stiff)
  {
// Determine proper load path (comp load, comp unload, tens load, tens unload):
    double stress=0.0;
    creepShrinkageState.revertCrackFlag();
    hstv.ecmin = hstvP.ecmin; //Initialized as ecmin = 0; ecmin should never be positive
    // 23/03/2026 changed by LP BEGIN.
    // Without this change the tests test_tdconcrete_mc10_fiber_section2d_02.py
    // test_composite_structure_01.py and test_composite_structure_02.py don't
    // converge.
    hstv.ecmax= std::max(hstv.ecmax, hstvP.ecmax); //Initialized as ecmax = 0; ecmax should never be negative
    // 23/03/2026 changed by LP END.
    
    if(strain <= hstv.ecmin)
      { // Concrete in compression loading
        this->Compr_Envlp(strain,stress,stiff);
        hstv.ecmin= strain; // reset ecmin
        creepShrinkageState.setCrackFlag(0); // concrete in compression, no cracking
      }
    else
      { // Concrete in either: Comp Unload, Tens Load, or Tens Unload/reload
    	if(strain < 0.0)
	  { // Compression Unloading
    		//stiff = Ec;
    		//stress = strain * stiff;
	    this->Compr_Envlp(strain,stress,stiff);
	  }
	else
	  { // either Tens Load, Tens Unload, or Tens reload
	    const double et0 = ft/Ec;
	    if(strain >= hstv.ecmax)
	      { //Tens Load or reload if strain is larger than before
    		//Need to check whether cracking has occurred or not
    		//If cracked, then reloading occurs along Et
    		//If not cracked, then loading occurs according to Tens_Envlp
		hstv.ecmax = strain; // reset ecmax
		this->Tens_Envlp(strain, stress, stiff);
		if (strain >= et0)
		  {//cracking has occurred, set cracking flag
		    creepShrinkageState.setCrackFlag(1);
		  }
	      }
	    else
	      { //Tens Unload or Tens Reload
		if (strain<=et0 && hstv.ecmax<=et0)
		  { //Linear unloading/reloading, i.e, not cracked
		    this->Tens_Envlp(strain,stress,stiff);
		  }
		else
		  { // Nonlinear unloading/reloading, i.e., cracked
		    const double &Et= creepShrinkageState.getEt();
		    stress = Et*strain;
		    stiff = Et;
		  }
	      }
	  }
      }
    return stress;
  }

int XC::TDConcreteMC10NL::commitState(void)
  {
    iter = 0;
    hstvP.ecmin= hstv.ecmin;
    hstvP.ecmax= hstv.ecmax;
    hstvP.dept= hstv.dept;

    // dsig_i[count]= hstv.sig- hstvP.sig; NOT USED.
    /* 5/8/2013: commented the following lines so that the DSIG_i[count+1]=sig-hstvP.sig;*/
    //if (crack_flag == 1) {// DSIG_i will be different depending on how the fiber is cracked
    //	if (sig < 0 && hstvP.sig > 0) { //if current step puts concrete from tension to compression, DSIG_i will be only the comp. stress
    //		DSIG_i[count+1] = hstv.sig;
    //	}
    //	if (hstv.sig > 0) {// Concrete should not creep when crack is opened
    //		DSIG_i[count+1] = 0.0;
    //	}
    //	if (hstv.sig > 0 && hstvP.sig < 0) {//if current step goes from compression to tension, DSIG_i will be the stress difference
    //		DSIG_i[count+1] = hstv.sig-hstvP.sig;
    //	}
    //} else { //concrete is uncracked, DSIG = hstv.sig - hstvP.sig
    //	DSIG_i[count+1] = hstv.sig-hstvP.sig;
    //}
    const double currentTime= this->getCurrentTime();
    const double eps_mech= creepShrinkageState.getMech();
    creepSteps.assignNextStep(this->hstv, this->hstvP, this->Ec, eps_mech, currentTime);

    hstvP.e= hstv.e;
    hstvP.sig= hstv.sig;
    hstvP.eps= hstv.eps;

   //Added by AMK:
    const int count= this->creepSteps.getCount();
    creepShrinkageState.commit_state(count, this->hstvP.sig, currentTime);
    //ntosic: strain compression limit changed to 0.4fpc/Ec; Include nonlinear creep coefficient?
    if(eps_mech < 0 && fabs(eps_mech)>0.40*fabs(fpc/Ec))
      {
	double s = fabs(eps_mech/fpc)*Ec;
	s = 0.4*fabs(fpc/Ec);
	std::cerr << "Strain Compression Limit Exceeded: "
		  << eps_mech << ' ' << -s << std::endl;
      }
    
    //if (creepControl==1) {
    //	count++;
    //    resize()
    //}
    creepSteps.next();
    return 0;
  }

void XC::TDConcreteMC10NL::Print(std::ostream &s, int flag)
  {
    s << "TDConcreteMC10NL:(strain, stress, tangent) "
      << hstv.eps << " " << hstv.sig << " " << hstv.e
      << std::endl;
  }




//! @brief Monotonic envelope of concrete in tension (positive envelope).
//! @param epsc[in]: concrete strain.
//! @param sigc[out]: concrete stress.
//! @param Ect[out]: tangent concrete modulus.
void XC::TDConcreteMC10NL::Tens_Envlp(double epsc, double &sigc, double &Ect)
  {
    /*-----------------------------------------------------------------------
    ! monotonic envelope of concrete in tension (positive envelope)
    !
    !   ft    = concrete tensile strength
    !   Ec0   = initial tangent modulus of concrete 
    !   Ets   = tension softening modulus
    !   eps   = strain
    !
    !   returned variables
    !    sigc  = stress corresponding to eps
    !    Ect  = tangent concrete modulus
    !-----------------------------------------------------------------------*/
  
    const double Ec0= Ec;
    const double eps0= ft / Ec0;
    //double epsu = ft * (1.0 / Ets + 1.0 / Ec0);
    const double b= beta;
    // USE THIS ONE
    if (epsc <= eps0)
      {
	sigc = epsc * Ec0;
	Ect = Ec0;
      }
    else
      {
	Ect = -b * eps0*ft / pow(epsc, 2)*pow(eps0 / epsc, b - 1.0); //original
	sigc = ft * pow(eps0 / epsc, b);
      }
   
  //THiS IS FOR TESTING LINEAR
  //sigc = epsc*Ec0;
  //Ect = Ec0;
   /*
    if (epsc<=epsu) {
      Ect  = -Ets;
      sigc = ft-Ets*(epsc-eps0);
    } else {
      Ect  = 1.0e-10;
      sigc = 1.0e-10;
    }
    */
    
  return;
}

//ntosic  
void XC::TDConcreteMC10NL::Compr_Envlp(double epsc, double &sigc, double &Ect)
  {
    //Linear
    //Ect = Ec;
    //sigc = Ect*epsc;
    //Non-linear with linear softening
    /*-----------------------------------------------------------------------
    ! monotonic envelope of concrete in compression (negative envelope)
    !
    !   fpc    = concrete compressive strength
    !   fcu   = stress at ultimate (crushing) strain
    !   epscu = ultimate (crushing) strain
    !   Ec0   = initial concrete tangent modulus
    !   epsc  = strain
    !
    !   returned variables
    !   sigc  = current stress
    !   Ect   = tangent concrete modulus
    -----------------------------------------------------------------------*/

    const double &Ec0= Ec; //ntosic
    const double epsc0 = 2.0*fpc / Ec0; //ntosic

    const double ratLocal = epsc / epsc0;
    if(epsc >= epsc0)
      {
	    sigc = fpc * ratLocal*(2.0 - ratLocal);
	    Ect = Ec0 * (1.0 - ratLocal);
      }
    else
      {
	//   linear descending branch between epsc0 and epscu
	if(epsc > epscu)
	  {
		sigc = (fcu - fpc)*(epsc - epsc0) / (epscu - epsc0) + fpc;
		Ect = (fcu - fpc) / (epscu - epsc0);
	  }
	else
	  {

	    // flat friction branch for strains larger than epscu

	    sigc = fcu;
	    Ect = 1.0e-10;
	    //       Ect  = 0.0  
	    return;
	  }
      }
  }

