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

XC::TDConcreteMC10NL::TDConcreteMC10NL(int tag)
  : TDConcreteMC10Base(tag, MAT_TAG_TDConcreteMC10NL)
  {}

XC::TDConcreteMC10NL::TDConcreteMC10NL(int tag, double _fc, double _fcu, double _epscu, double _ft, double _Ec, double _Ecm, double _beta, double _age, double _epsba, double _epsbb, double _epsda, double _epsdb, double _phiba, double _phibb, double _phida, double _phidb, double _tcast, double _cem): 
  TDConcreteMC10Base(tag, MAT_TAG_TDConcreteMC10NL, _fc, _ft, _Ec, _Ecm, _beta, _age, _epsba, _epsbb, _epsda, _epsdb, _phiba, _phibb, _phida, _phidb, _tcast, _cem)
  {
    // setup_parameters(); Called in the constructors of derived classes.
  }


XC::TDConcreteMC10NL::~TDConcreteMC10NL(void)
  {
  // Does nothing
  }

XC:: UniaxialMaterial *XC::TDConcreteMC10NL::getCopy(void) const
  { return new TDConcreteMC10NL(*this); }


//ntosic
double XC::TDConcreteMC10NL::setCreepBasicStrain(double time, double stress)
  {
    double creepBasic;
    double runSum = 0.0;
    
    DTIME_i[count] = creepDt;
 
	for(int i = 1; i<=count; i++)
	  {
                PHIB_i[i] = setPhiBasic(time,TIME_i[i]); //Determine PHI //ntosic: PHIB
                runSum += PHIB_i[i]*DSIG_i[i]/Ecm; //CONSTANT STRESS within Time interval //ntosic: changed to Ecm from Ec (according to Model Code formulation of phi basic)
      }
    
    phib_i = PHIB_i[count];
    creepBasic = runSum;
    return creepBasic;  
  }

//ntosic
double XC::TDConcreteMC10NL::setCreepDryingStrain(double time, double stress)
  {
	double creepDrying;
	double runSum = 0.0;

	DTIME_i[count] = creepDt;

	for (int i = 1; i <= count; i++) {
		PHID_i[i] = setPhiDrying(time, TIME_i[i]); //Determine PHI //ntosic: PHID
		runSum += PHID_i[i] * DSIG_i[i] / Ecm; //CONSTANT STRESS within Time interval //ntosic: changed to Ecm from Ec (according to Model Code formulation of phi drying)
	}

	phid_i = PHID_i[count];
	creepDrying = runSum;
	return creepDrying;

  }

int XC::TDConcreteMC10NL::setTrialStrain(double trialStrain, double strainRate)
  {
    double t = getCurrentTime();
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
	if (t-tcast<(2.0-0.0001)) { //Assumed that concrete can only carry load once hardened at 2 days following casting
		eps_crb = 0.0; //ntosic
		eps_crd = 0.0; //ntosic
		eps_shb = 0.0; //ntosic
		eps_shd = 0.0; //ntosic
		eps_m = 0.0;
		eps_total = trialStrain;
		sig = 0.0;
	} else { // Concrete has hardened and is ready to accept load
		// Initialize total strain:
        	eps_total = trialStrain;
	
		// Calculate shrinkage Strain:
            if (iter < 1) {
                eps_shb = setShrinkBasic(t); //ntosic
				eps_shd = setShrinkDrying(t); //ntosic
            }

    	// Calculate creep and mechanical strain, assuming stress remains constant in a time step:
    	if (creepControl == 1) {
        	if (fabs(t-TIME_i[count]) <= 0.0001) { //If t = t(i-1), use creep/shrinkage from last calculated time step
            	eps_crb = epsP_crb; //ntosic
				eps_crd = epsP_crd; //ntosic
            	eps_shb = epsP_shb; //ntosic
				eps_shd = epsP_shd; //ntosic
            	eps_m = eps_total - eps_crb - eps_crd - eps_shb - eps_shd; //ntosic
            	sig = setStress(eps_m, e);
            
        	} else { // if the current calculation is a new time step
        		//if (crackP_flag == 1 && sigP >= 0.0) { //if cracking occurs and previous stress is positive, 
        		// creep strain should not increase
        		//	eps_cr = epsP_cr;
        		//} else {
        		//	eps_cr = setCreepStrain(t,sig);
        		//}
        		//if (t < tcast) {
        		//std::cerr << "\nWARNING: TDConcrete loaded before tcast, creep and shrinkage not calculated" << std::endl;
        		//	eps_sh = epsP_sh;
        		//	eps_cr = epsP_cr;
        		//	eps_m = eps_total - eps_cr - eps_sh;
        		//	sig = setStress(eps_m, e);
        		//} else {
				if (iter < 1) {
                    eps_crb = setCreepBasicStrain(t,sig); 
					eps_crd = setCreepDryingStrain(t, sig);
				}
        		eps_m = eps_total - eps_crb - eps_crd - eps_shb - eps_shd; //ntosic
        		sig = setStress(eps_m, e);
				//}
        	}
    	} else { //Static Analysis using previously converged time-dependent strains
        	    eps_crb = epsP_crb; //ntosic
				eps_crd = epsP_crd; //ntosic
            	eps_shb = epsP_shb; //ntosic
				eps_shd = epsP_shd; //ntosic
            	eps_m = eps_total - eps_crb - eps_crd - eps_shb - eps_shd; //ntosic
    	        sig = setStress(eps_m, e);
    	}
		//
		//std::cerr<<"\n   eps_cr = "<<eps_cr;
		//std::cerr<<"\n   eps_sh = "<<eps_sh;
		//std::cerr<<"\n   eps_m = "<<eps_m;
		//std::cerr<<"\n   sig = "<<sig;
	}
    iter ++;
	return 0;
}

double XC::TDConcreteMC10NL::setStress(double strain, double &stiff)
{
// Determine proper load path (comp load, comp unload, tens load, tens unload):
    double stress=0.0;
    crack_flag = crackP_flag;
    ecmin = ecminP; //Initialized as ecmin = 0; ecmin should never be positive
    ecmax = ecmaxP; //Initialized as ecmax = 0; ecmax should never be negative
    
    if (strain <= ecmin) { // Concrete in compression loading
        this->Compr_Envlp(strain,stress,stiff);
        ecmin = strain;			// reset ecmin
        crack_flag = 0;			// concrete in compression, no cracking
    } else { // Concrete in either: Comp Unload, Tens Load, or Tens Unload/reload
    	if (strain < 0.0) { // Compression Unloading
    		//stiff = Ec;
    		//stress = strain * stiff;
    		this->Compr_Envlp(strain,stress,stiff);
    	} else { // either Tens Load, Tens Unload, or Tens reload
    		double et0 = ft/Ec;
    		if (strain >= ecmax) { //Tens Load or reload if strain is larger than before
    		//Need to check whether cracking has occurred or not
    		//If cracked, then reloading occurs along Et
    		//If not cracked, then loading occurs according to Tens_Envlp
    			ecmax = strain; // reset ecmax
    			this->Tens_Envlp(strain, stress, stiff);
    			if (strain >= et0) {//cracking has occurred, set cracking flag
    				crack_flag = 1;
    			}
    		} else { //Tens Unload or Tens Reload
    			if (strain<=et0 && ecmax<=et0) { //Linear unloading/reloading, i.e, not cracked
    					this->Tens_Envlp(strain,stress,stiff);
    			} else { // Nonlinear unloading/reloading, i.e., cracked
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
    ecminP = ecmin;
    ecmaxP = ecmax;
    deptP = dept;

    dsig_i[count]=sig-sigP;
    /* 5/8/2013: commented the following lines so that the DSIG_i[count+1]=sig-sigP;*/
    //if (crack_flag == 1) {// DSIG_i will be different depending on how the fiber is cracked
    //	if (sig < 0 && sigP > 0) { //if current step puts concrete from tension to compression, DSIG_i will be only the comp. stress
    //		DSIG_i[count+1] = sig;
    //	}
    //	if (sig > 0) {// Concrete should not creep when crack is opened
    //		DSIG_i[count+1] = 0.0;
    //	}
    //	if (sig > 0 && sigP < 0) {//if current step goes from compression to tension, DSIG_i will be the stress difference
    //		DSIG_i[count+1] = sig-sigP;
    //	}
    //} else { //concrete is uncracked, DSIG = sig - sigP
    //	DSIG_i[count+1] = sig-sigP;
    //}
    DSIG_i[count+1] = sig-sigP;

    //Secant Stiffness for determination of creep strain:
	if (fabs(eps_m/sig)>Ec) {  //ntosic: originally was eps_m/sig
	    E_i[count+1] = Ec;
	} else {
	    E_i[count+1] = fabs(sig/eps_m); //ADDED 7/22
	}

	if (isnan(E_i[count+1])) {
	    E_i[count+1] = Ec;
	}


    TIME_i[count+1] = getCurrentTime();

    eP = e;
    sigP = sig;
    epsP = eps;

   //Added by AMK:
	  epsP_total = eps_total; //Added by AMK;
      epsP_shb = eps_shb; //ntosic
	  epsP_shd = eps_shd; //ntosic
	  epsP_crb = eps_crb; //ntosic
	  epsP_crd = eps_crd; //ntosic
	  epsP_m = eps_m;
	  //ntosic: strain compression limit changed to 0.4fc/Ec; Include nonlinear creep coefficient?
      if (eps_m < 0 && fabs(eps_m)>0.40*fabs(fc/Ec)) {
	  double s = fabs(eps_m/fc)*Ec;
	  s = 0.4*fabs(fc/Ec);
	  std::cerr << "Strain Compression Limit Exceeded: " << eps_m << ' ' << -s << std::endl;
      }
		  //Cracking flags:
		  crackP_flag = crack_flag;

	  //cracked reloading/unloading stiffness:
	  if (crackP_flag==1) {
		  if (sig/eps_m<Et) {
			  Et = sig/eps_m;
		  }
	  }

	  if(count==0)
	    {
		  epsInit = epsP_total;
		  sigInit = sigP;
	    }

	  if (sigInit<0.0 && t_load<0.0)
	    {
		  t_load = getCurrentTime();
		  sigInit = sigP;
		  epsInit = epsP_m;
	  } else if (sigInit>0.0 && sigP<0.0 && t_load<0.0) {
		  t_load = getCurrentTime();
		  sigInit = sigP;
		  epsInit = epsP_m;
	  }

	  //if (creepControl==1) {
	  //	count++;
	  //    resize()
	  //}
	  count++;
	  resize();

    return 0;
  }

void XC::TDConcreteMC10NL::Print(std::ostream &s, int flag)
  {
    s << "TDConcreteMC10NL:(strain, stress, tangent) " << eps << " " << sig << " " << e << std::endl;
  }




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
  
	double Ec0 = Ec;
	double eps0 = ft / Ec0;
	//double epsu = ft * (1.0 / Ets + 1.0 / Ec0);
	double b = beta;
	// USE THIS ONE
	if (epsc <= eps0) {
		sigc = epsc * Ec0;
		Ect = Ec0;
	}
	else {
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
    !   fc    = concrete compressive strength
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
    const double epsc0 = 2.0*fc / Ec0; //ntosic

    const double ratLocal = epsc / epsc0;
    if(epsc >= epsc0)
      {
	    sigc = fc * ratLocal*(2.0 - ratLocal);
	    Ect = Ec0 * (1.0 - ratLocal);
      }
    else
      {
	//   linear descending branch between epsc0 and epscu
	if(epsc > epscu)
	  {
		sigc = (fcu - fc)*(epsc - epsc0) / (epscu - epsc0) + fc;
		Ect = (fcu - fc) / (epscu - epsc0);
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

