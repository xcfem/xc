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
 // Adam M. Knaack (adam.knaack@schaefer-inc.com) 
 // Schaefer-Inc, Cincinnati, Ohio, USA
 // Nikola D. Tosic (ntosic@imk.grf.bg.ac.rs)
 // Department for Materials and Structure, Faculty of Civil Engineering, University of Belgrade, Serbia
 // Yahya C. Kurama (ykurama@nd.edu)
 // Department of Civil and Environmental Engineering and Earth Sciences, College of Engineering, University of Notre Dame, Notre Dame, Indiana, USA
 //----------------------------------------------------------------------------------------------------------------------------

 //----------------------------------------------------------------------------------------------------------------------------
 // Created: 2012
 // Last updated: 2019
 //----------------------------------------------------------------------------------------------------------------------------

 //----------------------------------------------------------------------------------------------------------------------------
 // Description: This file contains the source code of TDConcrete. 
 // TDConcrete is a time-dependent concrete material model that calculates
 // creep and shrinkage strains.
 /*-------------------------------
 ! Concrete Compression - Linear
 ! Concrete Tension - Tamai, S., Shima, H., Izumo, J., Okamura, H. 1988. Average Stress-Strain Relationship in Post Yield Range of Steel Bar in Concrete, Concrete Library of JSCE, No. 11, 117-129.
 ! Concrete Creep - Linear superposition of creep coefficient, ACI 209 time function
 ! Concrete Shrinkage - ACI 209 time function
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


#include "material/uniaxial/concrete/TDConcrete.h" //Changed by AMK
#include "domain/mesh/element/utils/Information.h"
#include <float.h>
#include "domain/domain/Domain.h" //Added by AMK to get current Domain time;
#include "utility/recorder/response/MaterialResponse.h"
#include <cmath>
#include <boost/algorithm/string.hpp>
#include "domain/mesh/element/Element.h"

//! @brief Sets initial values for the concrete parameters.
void XC::TDConcrete::setup_parameters(void)
  {
    TDConcreteBase::setup_parameters();
    
    eps_cr = 0.0; //Added by AMK
    eps_sh = 0.0;
    epsP_cr = 0.0; //Added by AMK
    epsP_sh = 0.0; 
    epsP_m = 0.0; //Added by AMK

    //Change inputs into the proper sign convention:
    epsshu = -fabs(epsshu);
    epscru = fabs(epscru); 
  }

size_t XC::TDConcrete::resize(void)
  {
    const size_t newSize= TDConcreteBase::resize();
    if(newSize!=PHI_i.size()) // restart.
      { PHI_i.resize(newSize); }
    return newSize;
  }

//! @brief Constructor.
XC::TDConcrete::TDConcrete(int tag)
  : TDConcreteBase(tag, MAT_TAG_TDConcrete)
  {}

//! @brief Constructor.
//! @param _fpc: cylinder compressive strength (this is a dummy parameter since compression behavior is linear).
//! @param _ft: the tensile strength (splitting or axial tensile strength should be input, rather than the flexural).
//! @param _Ec: modulus of elasticity (preferably at time of loading if there is a single loading age).
//! @param _beta: tension softening parameter.
//! @param _age: analysis time at initiation of drying (in days).
//! @param _epsshu: ultimate shrinkage strain εsh,u, as per ACI 209R-92.
//! @param _epssha: fitting parameter within the shrinkage time evolution function as per ACI 209R-92.
//! @param _tcr: creep model age in days.
//! @param _epscru: ultimate creep coefficient φu, as per ACI 209R-92.
//! @param _epscra: fitting constant within the creep time evolution function as per ACI 209R-92.
//! @param _epscrd: fitting constant within the creep time evolution function as per ACI 209R-92.
//! @param _tcast: analysis time corresponding to concrete casting in days (note: concrete will not be able to take on loads until the age of 2 days).
XC::TDConcrete::TDConcrete(int tag, double _fpc, double _ft, double _Ec, double _beta, double _age, double _epsshu, double _epssha, double _tcr, double _epscru, double _epscra, double _epscrd, double _tcast)
  : TDConcreteBase(tag, MAT_TAG_TDConcrete, _fpc, _ft, _Ec, _beta, _age, _tcast),
    tcr(_tcr), epsshu(_epsshu), epssha(_epssha), epscru(_epscru), epscra(_epscra), epscrd(_epscrd)
  {
    setup_parameters();
  }

XC::TDConcrete::~TDConcrete(void)
  {
    // Does nothing
  }

XC::UniaxialMaterial *XC::TDConcrete::getCopy(void) const
  { return new TDConcrete(*this); }

double XC::TDConcrete::setCreepStrain(double time, double stress)
  {
    double creep;
    double runSum = 0.0;
    
    DTIME_i[count]= creepDt;
    
    for (int i = 1; i<=count; i++)
      {
        PHI_i[i] = setPhi(time,TIME_i[i]); //Determine PHI
        runSum += PHI_i[i]*DSIG_i[i]/Ec; //CONSTANT STRESS within Time interval
      }
    
    phi_i = PHI_i[count];
    creep = runSum;
    return creep;
    
}

double XC::TDConcrete::setPhi(double time, double tp)
  {        
    // ACI Equation:
    const double tmtp = time-tp;
    //double f1 = pow((4+0.85*tp)/tp,0.5);
    const double f2= pow(tmtp,epscra)/(epscrd+pow(tmtp,epscra))*epscru;
    const double f3= (1.25*pow((tp-tcast),-0.118))/(1.25*pow(tcr,-0.118));
    const double phi= f2*f3;
    return phi;
  }

double XC::TDConcrete::setShrink(double time)
  {
    const double &tD= this->age; //Age at initiation of drying
    double shrink = 0.0;
    if(time-(tD) < 0)
      {
        shrink = 0.0;
      }
    else
      { shrink = (time-(tD)) / (epssha + (time - (tD))) * epsshu; }
    return shrink;
  }

int XC::TDConcrete::setTrialStrain(double trialStrain, double strainRate)
  {
    const double t= getCurrentTime();
    //double tol = 1.e-4; // 9/13
    //double test = 10.0; // 9/13
    //double sigI = 0.0;  // 9/13
    //int niter = 500;  // 9/13
        
    //std::cerr<<"\n trialStrain = "<<trialStrain;
    
        // Need to initialize count and initial stress/strain:
    /*
    if(creep == 0)
      { count= 0; }
    else if(creepControl==1 && count == 0)
      { count= 1; }
    resize();
    */
        
        // Check casting age:
        if(t-tcast<(2.0-0.0001))
	  { //Assumed that concrete can only carry load once hardened at 2 days following casting
                eps_cr= 0.0;
                eps_sh= 0.0;
                eps_m= 0.0;
                eps_total= trialStrain;
                hstv.sig= 0.0;
          }
	else
	  { // Concrete has hardened and is ready to accept load
            // Initialize total strain:
            eps_total = trialStrain;
        
            // Calculate shrinkage Strain:
            if(iter < 1)
	      { eps_sh = setShrink(t); }
            // Calculate creep and mechanical strain, assuming stress remains constant in a time step:
            if(creepControl == 1)
	      {
                if(fabs(t-TIME_i[count]) <= 0.0001)
		  { //If t = t(i-1), use creep/shrinkage from last calculated time step
                    eps_cr = epsP_cr;
                    eps_sh = epsP_sh;
                    eps_m = eps_total - eps_cr - eps_sh;
                    hstv.sig= setStress(eps_m, hstv.e);
            
                  }
		else
		  { // if the current calculation is a new time step
                        //if(crackP_flag == 1 && hstvP.sig >= 0.0) { //if cracking occurs and previous stress is positive, 
                        // creep strain should not increase
                        //        eps_cr = epsP_cr;
                        //} else {
                        //        eps_cr = setCreepStrain(t,hstv.sig);
                        //}
                        //if(t < tcast) {
                        //std::cerr << "\nWARNING: TDConcrete loaded before tcast, creep and shrinkage not calculated" << std::endl;
                        //        eps_sh = epsP_sh;
                        //        eps_cr = epsP_cr;
                        //        eps_m = eps_total - eps_cr - eps_sh;
                        //        sig = setStress(eps_m, e);
                        //} else {        
                    if(iter < 1)
		      { eps_cr = setCreepStrain(t,hstv.sig); }
                    eps_m = eps_total - eps_cr - eps_sh;
                    hstv.sig= setStress(eps_m, hstv.e);
                        //}
                  }
              }
	    else // no more creep no more shrinkage.
	      { //Static Analysis using previously converged time-dependent strains
                eps_cr = epsP_cr;
                eps_sh = epsP_sh;
                eps_m = eps_total-eps_cr-eps_sh;
                hstv.sig= setStress(eps_m, hstv.e);
	      }
                //
                //std::cerr<<"\n   eps_cr = "<<eps_cr;
                //std::cerr<<"\n   eps_sh = "<<eps_sh;
                //std::cerr<<"\n   eps_m = "<<eps_m;
                //std::cerr<<"\n   sig = "<<sig;
        }
    iter++;
    return 0;
  }


double XC::TDConcrete::setStress(double strain, double &stiff)
  {
    // Determine proper load path (comp load, comp unload, tens load,
    // tens unload):
    double stress=0.0;
    crack_flag = crackP_flag;
    hstv.ecmin= hstvP.ecmin; //Initialized as ecmin = 0; ecmin should never be positive
    hstv.ecmax= hstvP.ecmax; //Initialized as ecmax = 0; ecmax should never be negative
    
    if(strain <= hstv.ecmin)
      { // Concrete in compression loading
        this->Compr_Envlp(strain,stress,stiff);
        hstv.ecmin= strain;                        // reset ecmin
        crack_flag= 0;                        // concrete in compression, no cracking
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
	    double et0 = ft/Ec;
	    if(strain >= hstv.ecmax)
	      { //Tens Load or reload if strain is larger than before
		//Need to check whether cracking has occurred or not
		//If cracked, then reloading occurs along Et
		//If not cracked, then loading occurs according to Tens_Envlp
		hstv.ecmax= strain; // reset ecmax
		this->Tens_Envlp(strain, stress, stiff);
		if(strain >= et0)
		  {//cracking has occurred, set cracking flag
		    crack_flag = 1;
		  }
	      }
	    else
	      { //Tens Unload or Tens Reload
		if(strain<=et0 && hstv.ecmax<=et0)
		  { //Linear unloading/reloading, i.e, not cracked
		    this->Tens_Envlp(strain,stress,stiff);
		  }
		else
		  { // Nonlinear unloading/reloading, i.e., cracked
		    stress = Et*strain;
		    stiff= Et;
		  }
	      }
	  }
      }
    return stress;
  }

double XC::TDConcrete::getPHI_i(void) const
  {
    return phi_i;
  }

double XC::TDConcrete::getCreep(void) const
  { return eps_cr; }

double XC::TDConcrete::getShrink(void) const
  {
    return eps_sh;
  }

//! @brief Assigns ultimate shrinkage.
void XC::TDConcrete::setUltimateShrinkage(const double &d)
  { epsshu= d; }

//! @brief Return ultimate shrinkage.
double XC::TDConcrete::getUltimateShrinkage(void) const
  { return epsshu; }

//! @brief Assigns shrinkage parameter.
void XC::TDConcrete::setShrinkageParameter(const double &d)
  { epssha= d; }

//! @brief Return shrinkage parameter.
double XC::TDConcrete::getShrinkageParameter(void) const
  { return epssha; }

//! @brief Assigns creep relationship age.
void XC::TDConcrete::setCreepRelationshipAge(const double &d)
  { tcr= d; }

//! @brief Return creep relationship age.
double XC::TDConcrete::getCreepRelationshipAge(void) const
  { return tcr; }

//! @brief Assigns creep exponent parameter.
void XC::TDConcrete::setUltimateConcreteCreep(const double &d)
  { epscru= d; }

//! @brief Return creep exponent parameter.
double XC::TDConcrete::getUltimateConcreteCreep(void) const
  { return epscru; }

//! @brief Assigns creep exponent parameter.
void XC::TDConcrete::setCreepExponentParameter(const double &d)
  { epscra= d; }

//! @brief Return creep exponent parameter.
double XC::TDConcrete::getCreepExponentParameter(void) const
  { return epscra; }

//! @brief Assigns creep exponent parameter.
void XC::TDConcrete::setCreepDParameter(const double &d)
  { epscrd= d; }

//! @brief Return creep exponent parameter.
double XC::TDConcrete::getCreepDParameter(void) const
  { return epscrd; }

int XC::TDConcrete::commitState(void)
  {
    iter = 0;
    hstvP.ecmin= hstv.ecmin;
    hstvP.ecmax= hstv.ecmax;
    hstvP.dept= hstv.dept;

    dsig_i[count]=hstv.sig-hstvP.sig;
    /* 5/8/2013: commented the following lines so that the DSIG_i[count+1]=sig-hstvP.sig;*/
    //if(crack_flag == 1) {// DSIG_i will be different depending on how the fiber is cracked
    //        if(sig < 0 && hstvP.sig > 0) { //if current step puts concrete from tension to compression, DSIG_i will be only the comp. stress
    //                DSIG_i[count+1] = sig;
    //        }
    //        if(sig > 0) {// Concrete should not creep when crack is opened
    //                DSIG_i[count+1] = 0.0;
    //        }
    //        if(sig > 0 && hstvP.sig < 0) {//if current step goes from compression to tension, DSIG_i will be the stress difference
    //                DSIG_i[count+1] = hstv.sig-hstvP.sig;
    //        }
    //} else { //concrete is uncracked, DSIG = hstv.sig - hstvP.sig
    //        DSIG_i[count+1] = hstv.sig-hstvP.sig;
    //}
    DSIG_i[count+1] = hstv.sig-hstvP.sig;

    //Secant Stiffness for determination of creep strain:
    if(fabs(eps_m/hstv.sig)>Ec)
      { E_i[count+1] = Ec; }
    else
      { E_i[count+1] = fabs(hstv.sig/eps_m); } //ADDED 7/22

    if(isnan(E_i[count+1]))
      { E_i[count+1] = Ec; }

    TIME_i[count+1] = getCurrentTime();

    hstvP.e= hstv.e;
    hstvP.sig= hstv.sig;
    hstvP.eps= hstv.eps;

    //Added by AMK:
    epsP_total = eps_total; //Added by AMK;
    epsP_sh = eps_sh;
    epsP_cr = eps_cr;
    epsP_m = eps_m;
    if(eps_m < 0 && fabs(eps_m)>0.50*fabs(fpc/Ec))
      {
	double s = fabs(eps_m/fpc)*Ec;
	s = 0.5*fabs(fpc/Ec);
	std::cerr << "Strain Compression Limit Exceeded: "
		  << eps_m << ' ' << -s << std::endl;
      }

    //Cracking flags:
    crackP_flag = crack_flag;

    //cracked reloading/unloading stiffness:
    if(crackP_flag==1)
      {
	if(hstv.sig/eps_m<Et)
	  { Et = hstv.sig/eps_m; }
      }
    if(count==0)
      {
        epsInit = epsP_total;
	sigInit = hstvP.sig;
      }

    if(sigInit<0.0 && t_load<0.0)
      {
        t_load = getCurrentTime();
        sigInit = hstvP.sig;
	epsInit = epsP_m;

      }
    else if(sigInit>0.0 && hstvP.sig<0.0 && t_load<0.0)
      {
        t_load = getCurrentTime();
        sigInit= hstvP.sig;
        epsInit= epsP_m;
      }

    //if(creepControl==1) {
    //   count++;
    //   resize();
    //}
    count++;
    resize();

    return 0;
  }

int XC::TDConcrete::revertToLastCommit(void)
  {
    eps_total = epsP_total; //Added by AMK;
    eps_sh = epsP_sh;
    eps_cr = epsP_cr;
    eps_m = epsP_m;  

    hstv= hstvP; // revert history variables.

    return 0;
  }

int XC::TDConcrete::revertToStart(void)
  {
    hstvP.revertToStart(Ec);

    hstv.setup_parameters(Ec);

    if(creepControl==0)
      { count= 0; }
    else
      { count= 1; }
    resize();

    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::TDConcrete::sendData(Communicator &comm)
  {
    int res= TDConcreteBase::sendData(comm);
    res+= comm.sendDoubles(epsshu, epssha, tcr, epscru, epscra, epscrd,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::TDConcrete::recvData(const Communicator &comm)
  {
    int res= TDConcreteBase::recvData(comm);
    res+= comm.receiveDoubles(epsshu, epssha, tcr, epscru, epscra, epscrd,getDbTagData(),CommMetaData(3));
    return res;
  }
//! @brief Sends object through the communicator argument.
int XC::TDConcrete::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::TDConcrete::recvSelf(const Communicator &comm)
  {
    inicComm(4);
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

void XC::TDConcrete::Print(std::ostream &s, int flag) const
  {
    s << "TDConcrete:(strain, stress, tangent) " << hstv.eps << " " << hstv.sig << " " << hstv.e << std::endl;
  }




void XC::TDConcrete::Tens_Envlp (double epsc, double &sigc, double &Ect)
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
  double eps0 = ft/Ec0;
  //double epsu = ft*(1.0/Ets+1.0/Ec0);
  double b = beta;
  // USE THIS ONE
  if(epsc<=eps0) {
    sigc = epsc*Ec0;
    Ect  = Ec0;
  } else {
    Ect = -b*eps0*ft/pow(epsc,2)*pow(eps0/epsc,b-1.0);
    sigc = ft*pow(eps0/epsc,b);
  }
   
  
  //THiS IS FOR TESTING LINEAR
  //sigc = epsc*Ec0;
  //Ect = Ec0;
    
    /*
    if(epsc<=epsu) {
      Ect  = -Ets;
      sigc = ft-Ets*(epsc-eps0);
    } else {
      Ect  = 1.0e-10;
      sigc = 1.0e-10;
    }
    */
    
  return;
}

  
void XC::TDConcrete::Compr_Envlp(double epsc, double &sigc, double &Ect) 
{
//Linear
Ect = Ec;
sigc = Ect*epsc;
  return;
}

int XC::TDConcrete::getVariable(const std::string &varName, Information &theInfo) const
  {
    if(varName=="ec")
      {
        theInfo.theDouble = epsc0;
        return 0;
      }
    else
      return -1;
  }

/* Methods added by AMK: */

XC::Response *XC::TDConcrete::setResponse(const std::vector<std::string> &argv, Information &matInfo)
  {        
    Response *theResponse = nullptr;

    //theOutput.tag("UniaxialMaterialOutput");
    //theOutput.attr("matType", this->getClassType());
    //theOutput.attr("matTag", this->getTag());

    // stress
    if(argv[0]=="stress")
      {
            //theOutput.tag("ResponseType", "sigma11");
            theResponse =  new MaterialResponse(this, 1, this->getStress());
      }  
    // tangent
    else if(argv[0]=="tangent")
      {
            //theOutput.tag("ResponseType", "C11");
            theResponse =  new MaterialResponse(this, 2, this->getTangent());
      }

    // strain
    else if(argv[0]=="strain") {
            //theOutput.tag("ResponseType", "eps11");
            theResponse =  new MaterialResponse(this, 3, this->getStrain());
    }

    // strain
    else if((argv[0]=="stressStrain") || 
                     (argv[0]=="stressANDstrain") ||
                     (argv[0]=="stressAndStrain")) {
            //theOutput.tag("ResponseType", "sig11");
            //theOutput.tag("ResponseType", "eps11");
            theResponse =  new MaterialResponse(this, 4, Vector(2));
    }

    else if(argv[0]=="CreepStressStrainTangent")
      {
            //theOutput.tag("ResponseType", "sig11");
            //theOutput.tag("ResponseType", "eps11");
            //theOutput.tag("ResponseType", "C11");
            //theOutput.tag("ResponseType", "CreepStrain");
            //theOutput.tag("ResponseType", "MechStrain");
            //theOutput.tag("ResponseType", "ShrinkStrain");
            //theOutput.tag("ResponseType", "t_load");
            theResponse = new MaterialResponse(this, 6, Vector(6));
      }
    else if((argv[0]=="stressStrainTangent") || 
                     (argv[0]=="stressANDstrainANDtangent"))
      {
            //theOutput.tag("ResponseType", "sig11");
            //theOutput.tag("ResponseType", "eps11");
            //theOutput.tag("ResponseType", "C11");
            theResponse =  new MaterialResponse(this, 5, Vector(3));
      }

    // stress sensitivity for local sensitivity recorder purpose.  Quan 2009
    // limit:  no more than 10000 random variables/sensitivity parameters
    else if(argv[0].find("stressSensitivity") != std::string::npos)
      {
	std::vector<std::string> tokens;
	boost::split(tokens, argv[0], boost::is_any_of(" "));
        const int gradient= atoi(tokens[1]);
        //theOutput.tag("ResponseType", "sigsens11");
        theResponse =  new MaterialResponse(this, gradient+10000, this->getStress());
      }
    // strain sensivitiy
    else if(argv[0].find("strainSensitivity") != std::string::npos)
      {
	std::vector<std::string> tokens;
	boost::split(tokens, argv[0], boost::is_any_of(" "));
        const int gradient= atoi(tokens[1]);
        //theOutput.tag("ResponseType", "epssens11");
        theResponse =  new MaterialResponse(this, gradient+20000, this->getStrain());
      }
    //theOutput.endTag();
    return theResponse;
  }

int XC::TDConcrete::getResponse(int responseID, Information &matInfo)
  {
        static Vector stressStrain(2);
        static Vector stressStrainTangent(3);
        static Vector CreepStressStrainTangent(6); //Added by AMK
        // each subclass must implement its own stuff   
        
        // added for sensitivity recorder. Quan 2009
        if((responseID>10000)&&(responseID<20000))
	  {
            matInfo.setDouble(this->getStressSensitivity(responseID-10000,false));
                return 0;
          }
        else if(responseID>20000){
                matInfo.setDouble(this->getStrainSensitivity(responseID-20000));
                return 0;
        }
        
        switch (responseID) {
                case 1:
                        matInfo.setDouble(this->getStress());
                        return 0;
                        
                case 2:
                        matInfo.setDouble(this->getTangent());
                        return 0;      
                        
                case 3:
                        matInfo.setDouble(this->getStrain());
                        return 0;      
                        
                case 4:
                        stressStrain(0) = this->getStress();
                        stressStrain(1) = this->getStrain();
                        matInfo.setVector(stressStrain);
                        return 0;
                        
                case 5:
                        stressStrainTangent(0) = this->getStress();
                        stressStrainTangent(1) = this->getStrain();
                        stressStrainTangent(2) = this->getTangent();
                        matInfo.setVector(stressStrainTangent);
                        return 0;
                
                case 6:
                        CreepStressStrainTangent(0) = this->getStress();
                        CreepStressStrainTangent(1) = this->getStrain();
                        CreepStressStrainTangent(2) = this->getTangent();
                        CreepStressStrainTangent(3) = this->getCreep();
                        CreepStressStrainTangent(4) = this->getMech();
                        CreepStressStrainTangent(5) = this->getShrink();
                        matInfo.setVector(CreepStressStrainTangent);
                        return 0;
                        
                default:      
                        return -1;
        }
  }


