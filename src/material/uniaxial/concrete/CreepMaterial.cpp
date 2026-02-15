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
 // Michael H. Scott
 //
 // Based on TDConcrete implementations by:
 // Adam M. Knaack (adam.knaack@schaefer-inc.com) 
 // Schaefer-Inc, Cincinnati, Ohio, USA
 // Nikola D. Tosic (ntosic@imk.grf.bg.ac.rs)
 // Department for Materials and Structure, Faculty of Civil Engineering, University of Belgrade, Serbia
 // Yahya C. Kurama (ykurama@nd.edu)
 // Department of Civil and Environmental Engineering and Earth Sciences, College of Engineering, University of Notre Dame, Notre Dame, Indiana, USA
 //----------------------------------------------------------------------------------------------------------------------------

 //----------------------------------------------------------------------------------------------------------------------------
 // Description: This file contains the source code of CreepMaterial. 
 // CreepMaterial is a wrapper that imposes creep and shrinkage evoluation equations
 // to any uniaxialMaterial.
 //----------------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "material/uniaxial/concrete/CreepMaterial.h"
#include <float.h>
#include "domain/mesh/element/utils/Information.h"
#include "domain/domain/Domain.h" //Added by AMK to get current Domain time;
#include "utility/recorder/response/MaterialResponse.h"

#include "material/uniaxial/concrete/Concrete02IS.h"
#include "material/uniaxial/concrete/TDConcreteBase.h"
#include "material/uniaxial/ElasticMaterial.h"
#include <boost/algorithm/string.hpp>
#include "utility/utils/misc_utils/colormod.h"

//! @brief Return a pointer to the encapsulated concrete material.
const XC::RawConcrete *XC::CreepMaterial::_get_concrete_material(void) const
  {
    const UniaxialMaterial *tmp= this->getMaterial();
    const RawConcrete *retval= dynamic_cast<const RawConcrete *>(tmp);
    if(!retval)
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; Error!, the material of type: "
		  << tmp->getClassName()
		  << " is not a concrete type material."
		  << Color::def << std::endl;
      }
    return retval;
  }

//! @brief Resize the vectors that store the creep history.
size_t XC::CreepMaterial::resize(void)
  {
    size_t newSize= 10;
    if(count<2) // restart.
      {
	PHI_i.resize(newSize);
	E_i.resize(newSize);
	DSIG_i.resize(newSize);
	TIME_i.resize(newSize);
	DTIME_i.resize(newSize);
      }
    else
      { 
	if(static_cast<size_t>(count+1)>=E_i.size())
	  {
            newSize= 2*(count+1);
	    PHI_i.resize(newSize);
	    E_i.resize(newSize);
	    DSIG_i.resize(newSize);
	    TIME_i.resize(newSize);
	    DTIME_i.resize(newSize);
	  }
      }
    return newSize;
  }

XC::CreepMaterial::CreepMaterial(int tag)
  : EncapsulatedUniaxialMaterial(tag, MAT_TAG_CreepMaterial),
    age(0.0), beta(0.0), tcast(0.0),
    count(0), epsInit(0.0), sigInit(0.0),
    eps_cr(0.0), eps_sh(0.0), eps_m(0.0), epsP_cr(00), epsP_sh(0.0), epsP_m(0.0),
    eps_total(0.0), epsP_total(0.0), t(0.0), t_load(-1.0),
    crack_flag(0), crackP_flag(0), iter(0),
    maxSize(startSize)
  {}

XC::CreepMaterial::CreepMaterial(int tag, double _fc, double _fcu, double _epscu, double _ft, double _Ec, double _beta, double _age, double _tcast, const CreepShrinkageParameters &_csParameters)
  : EncapsulatedUniaxialMaterial(tag, MAT_TAG_CreepMaterial),
    age(_age), beta(_beta), tcast(_tcast),
    creepShrinkageParameters(_csParameters),
    count(0), epsInit(0.0), sigInit(0.0),
    eps_cr(0.0), eps_sh(0.0), eps_m(0.0), epsP_cr(00), epsP_sh(0.0), epsP_m(0.0),
    eps_total(0.0), epsP_total(0.0), t(0.0), t_load(-1.0), Et(_Ec),
    crack_flag(0), crackP_flag(0), iter(0),
    maxSize(startSize)
  {
      
    Concrete02IS wrappedMaterial(0,_Ec, _fc, 2*_fc/_Ec,_fcu,_epscu);
    wrappedMaterial.setFt(_ft);
    //ElasticMaterial wrappedMaterial(0,_Ec);
    this->setMaterial(wrappedMaterial);

    //sigCr= fabs(sigCr);
    hstvP.setup_parameters(_Ec);
    hstv.setup_parameters(_Ec);

    //Change inputs into the proper sign convention:
    creepShrinkageParameters.setup_parameters();

    this->resize();

    TIME_i[0]= getCurrentTime();
  }

XC::CreepMaterial::CreepMaterial(int tag, UniaxialMaterial &matl, double _age, double _tcast, const CreepShrinkageParameters &_csParameters)
  : EncapsulatedUniaxialMaterial(tag, MAT_TAG_CreepMaterial, matl),
    age(_age), beta(0.0), tcast(_tcast),
    creepShrinkageParameters(_csParameters),
    count(0), epsInit(0.0), sigInit(0.0),
    eps_cr(0.0), eps_sh(0.0), eps_m(0.0), epsP_cr(00), epsP_sh(0.0), epsP_m(0.0),
    eps_total(0.0), epsP_total(0.0), t(0.0), t_load(-1.0),
    crack_flag(0), crackP_flag(0), iter(0),
    maxSize(startSize)
    
  {
    // Get initial tangent
    const double &_Ec= this->getMaterial()->getInitialTangent();
    
    //sigCr= fabs(sigCr);
    hstvP.setup_parameters(_Ec);
    hstv.setup_parameters(_Ec);

    Et= _Ec;
    
    //Change inputs into the proper sign convention:
    creepShrinkageParameters.setup_parameters();

    this->resize();

    TIME_i[0]= getCurrentTime();
  }

//! @brief Sets initial values for the concrete parameters.
void XC::CreepMaterial::setup_parameters(void)
  {    
    eps_cr = 0.0;
    eps_sh = 0.0;
    epsP_cr = 0.0;
    epsP_sh = 0.0; 
    epsP_m = 0.0;

    // Get initial tangent
    const double &_Ec= this->getMaterial()->getInitialTangent();
    
    //sigCr= fabs(sigCr);
    hstvP.setup_parameters(_Ec);
    hstv.setup_parameters(_Ec);

    this->Et= _Ec;
    this->count= 0; //Added by AMK
    this->resize();
    this->epsInit= 0.0; //Added by AMK
    this->sigInit= 0.0; //Added by AMK
    this->eps_total= 0.0; //Added by AMK
    this->epsP_total= 0.0; //Added by AMK
    
    this->t_load= -1.0; //Added by AMK
    this->crack_flag= 0;
    this->crackP_flag= 0; // Added by LP
    this->iter= 0;
    
    creepShrinkageParameters.setup_parameters();
    TIME_i[0]= getCurrentTime();
  }

//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::CreepMaterial::getCopy(void) const
  { return new CreepMaterial(*this); }

void XC::CreepMaterial::setCreepShrinkageParameters(const CreepShrinkageParameters &csParameters)
  { this->creepShrinkageParameters= csParameters; }

const XC::CreepShrinkageParameters &XC::CreepMaterial::getCreepShrinkageParameters(void) const
  { return this->creepShrinkageParameters; }

double XC::CreepMaterial::getInitialTangent(void) const
  {
    return this->getMaterial()->getInitialTangent(); //Added by AMK
  } 

double XC::CreepMaterial::getCurrentTime(void) const
  {
    double currentTime= 0.0;
    const Domain *theDomain= this->getDomain();
  
    if(theDomain)
      { currentTime= theDomain->getCurrentTime(); }
  
    return currentTime;
  }	

double XC::CreepMaterial::setCreepStrain(double time, double stress)
  {
    double creep;
    double runSum= 0.0;
  
    DTIME_i[count]= get_creep_dt();

    const double _Ec= this->getInitialTangent();
  
    for (int i= 1; i<=count; i++)
      {
	PHI_i[i]= setPhi(time,TIME_i[i]); //Determine PHI
	runSum += PHI_i[i]*DSIG_i[i]/_Ec; //CONSTANT STRESS within Time interval
      }
  
    phi_i= PHI_i[count];
    creep= runSum;
    return creep;
  }

double XC::CreepMaterial::setPhi(double time, double tp)
  {	
    // ACI Equation:
    //const double f1= pow((4+0.85*tp)/tp,0.5);
    const double f2= creepShrinkageParameters.getF2(time, tp);
    const double f3= (1.25*pow((tp-tcast),-0.118))/(1.25*pow(creepShrinkageParameters.getCreepRelationshipAge(),-0.118));
    const double phi= f2*f3;
    return phi;
  }

double XC::CreepMaterial::setShrink(double time)
  {
    return creepShrinkageParameters.getShrink(this->age, time);
  }

int XC::CreepMaterial::setTrialStrain(double trialStrain, double strainRate)
  {
    const double &t= getCurrentTime();
    // double tol= 1.e-4; // 9/13
    // double test= 10.0; // 9/13
    // double sigI= 0.0;  // 9/13
    // int niter= 500;  // 9/13
  
    // Check casting age:
    if (t-tcast<(2.0-0.0001))
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
	eps_total= trialStrain;
    
	// Calculate shrinkage Strain:
	if (iter < 1)
	  { eps_sh= setShrink(t); }
    
	// Calculate creep and mechanical strain, assuming stress remains constant in a time step:
	if(is_creep_on())
	  {
	    if (fabs(t-TIME_i[count]) <= 0.0001)
	      { //If t= t(i-1), use creep/shrinkage from last calculated time step
		eps_cr= epsP_cr;
		eps_sh= epsP_sh;
		eps_m= eps_total - eps_cr - eps_sh;
		//hstv.sig= setStress(eps_m, e);
		this->getMaterial()->setTrialStrain(eps_m, strainRate);
		hstv.sig= this->getMaterial()->getStress();
		hstv.e= this->getMaterial()->getTangent();
        
	      }
	    else
	      { // if the current calculation is a new time step
		if (iter < 1)
		  {
		    eps_cr= setCreepStrain(t,hstv.sig); 
		  }
		eps_m= eps_total - eps_cr - eps_sh;
		//sig= setStress(eps_m, e);
		this->getMaterial()->setTrialStrain(eps_m, strainRate);
		hstv.sig= this->getMaterial()->getStress();
		hstv.e= this->getMaterial()->getTangent();	
	      }
	  }
	else
	  { //Static Analysis using previously converged time-dependent strains
	    eps_cr= epsP_cr;
	    eps_sh= epsP_sh;
	    eps_m= eps_total-eps_cr-eps_sh;
	    //sig= setStress(eps_m, e);
	    this->getMaterial()->setTrialStrain(eps_m, strainRate);
	    hstv.sig= this->getMaterial()->getStress();
	    hstv.e= this->getMaterial()->getTangent();      
	  }
      }
    iter++;
    return 0;
  }

double XC::CreepMaterial::getStrain(void) const
  {
    return eps_total; //Added by AMK
    //return eps;
  }

double XC::CreepMaterial::getPHI_i(void) const
  { return phi_i; }

double XC::CreepMaterial::getStress(void) const
  { return hstv.sig; }

double XC::CreepMaterial::getTangent(void) const
  { return hstv.e; }

double XC::CreepMaterial::getCreep(void) const
  { return eps_cr; }

double XC::CreepMaterial::getShrink(void) const
  { return eps_sh; }

double XC::CreepMaterial::getMech(void) const
  { return eps_m; }

int  XC::CreepMaterial::commitState(void)
  {
    iter= 0;
    hstvP.ecmin= hstv.ecmin;
    hstvP.ecmax= hstv.ecmax;
    hstvP.dept= hstv.dept;

    // Make sure enough room to write into count+1 -- MHS
    this->resize();
    
    //dsig_i[count]=sig-sigP; // Unused -- MHS
    /* 5/8/2013: commented the following lines so that the DSIG_i[count+1]=sig-sigP;*/
    //if (crack_flag == 1) {// DSIG_i will be different depending on how the fiber is cracked
    //	if (sig < 0 && sigP > 0) { //if current step puts concrete from tension to compression, DSIG_i will be only the comp. stress
    //		DSIG_i[count+1]= sig;
    //	}
    //	if (sig > 0) {// Concrete should not creep when crack is opened
    //		DSIG_i[count+1]= 0.0;
    //	}
    //	if (sig > 0 && sigP < 0) {//if current step goes from compression to tension, DSIG_i will be the stress difference
    //		DSIG_i[count+1]= sig-sigP;
    //	}
    //} else { //concrete is uncracked, DSIG= sig - sigP
    //	DSIG_i[count+1]= sig-sigP;
    //}

    DSIG_i[count+1]= hstv.sig-hstvP.sig;
  
    //Secant Stiffness for determination of creep strain:
    const double &_Ec= this->getMaterial()->getInitialTangent();
    if(fabs(eps_m/hstv.sig)>_Ec)
      { E_i[count+1]= _Ec; }
    else
      { E_i[count+1]= fabs(hstv.sig/eps_m); }//ADDED 7/22
  
    if(isnan(E_i[count+1]))
      { E_i[count+1]= _Ec; }
  
  
    TIME_i[count+1]= getCurrentTime();
  
    hstvP.e= hstv.e;
    hstvP.sig= hstv.sig;
    hstvP.eps= hstv.eps;
	
    //Added by AMK:
    epsP_total= eps_total; //Added by AMK;
    epsP_sh= eps_sh;
    epsP_cr= eps_cr;
    epsP_m= eps_m;
    const RawConcrete *concrete= this->_get_concrete_material();
    if(concrete)
      {
	const double &fc= concrete->getFpc();
	const double fcOverEc= (this->getMaterial() == 0) ? fc/_Ec : 0.002; // Assumed by MHS
	if(eps_m < 0 && fabs(eps_m)>0.50*fabs(fcOverEc))
	  {
	    //double s= fabs(eps_m/fc)*_Ec;
	    double s= fabs(eps_m/fcOverEc);    
	    s= 0.5*fabs(fcOverEc);
	    // std::cerr << Color::red << "Strain Compression Limit Exceeded: "
	    // << eps_m << ' ' << -s
	    // << Color::def << std::endl;
	  }
      }
    else
      {
        std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		  << "; Error!, null pointer to concrete material."
		  << Color::def << std::endl;
	exit(-1);
      }
    //Cracking flags:
    crackP_flag= crack_flag;
  
    //cracked reloading/unloading stiffness:
    if (crackP_flag==1)
      {
	if(hstv.sig/eps_m<Et)
	  { Et= hstv.sig/eps_m; }
      }
  
    if(count==0)
      {
	epsInit= epsP_total;
	sigInit= hstvP.sig;
      }
  
    if(sigInit<0.0 && t_load<0.0)
      {
	t_load= getCurrentTime();
	sigInit= hstvP.sig;
	epsInit= epsP_m;
      }
    else if(sigInit>0.0 && hstvP.sig<0.0 && t_load<0.0)
      {
	t_load= getCurrentTime();
	sigInit= hstvP.sig;
	epsInit= epsP_m;
      }

    this->getMaterial()->commitState();
  
    //if (ops_Creep==1) {
    //	count++;
    //}
    count++;
  
    return 0;
  }

int XC::CreepMaterial::revertToLastCommit(void)
  {
    eps_total= epsP_total; //Added by AMK;
    eps_sh= epsP_sh;
    eps_cr= epsP_cr;
    eps_m= epsP_m;  

    hstv= hstvP; // revert history variables.

    this->getMaterial()->revertToLastCommit();
  
    return 0;
  }

int XC::CreepMaterial::revertToStart(void)
  {
    const double &_Ec= this->getMaterial()->getInitialTangent();
    hstvP.revertToStart(_Ec);

    hstv.setup_parameters(_Ec);
  
    if(is_creep_off())
      { count= 0; }
    else
      { count= 1; }

    this->getMaterial()->revertToStart();
  
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::CreepMaterial::sendData(Communicator &comm)
  {
    int res= EncapsulatedUniaxialMaterial::sendData(comm);
    res+= comm.sendMovable(creepShrinkageParameters, getDbTagData(),CommMetaData(4));
    res+= comm.sendDoubles(beta, age, tcast, getDbTagData(),CommMetaData(5));
    res+= comm.sendDoubles(hstvP.ecmin, hstvP.ecmax, hstvP.dept, hstvP.eps, hstvP.sig, hstvP.e, getDbTagData(),CommMetaData(6));
    res+= comm.sendDoubles(epsInit, sigInit, epsP_m, epsP_cr, epsP_sh, getDbTagData(),CommMetaData(7));
    res+= comm.sendDoubles(epsP_total, t, t_load, phi_i, Et, getDbTagData(),CommMetaData(7));
    res+= comm.sendInts(count, crackP_flag, iter, maxSize, getDbTagData(),CommMetaData(8));
    return res;
  }
//! @brief Receives object members through the communicator argument.
int XC::CreepMaterial::recvData(const Communicator &comm)
  {
    int res= EncapsulatedUniaxialMaterial::recvData(comm);
    res+= comm.receiveMovable(creepShrinkageParameters, getDbTagData(),CommMetaData(4));
    res+= comm.receiveDoubles(beta, age, tcast, getDbTagData(),CommMetaData(5));
    res+= comm.receiveDoubles(hstvP.ecmin, hstvP.ecmax, hstvP.dept, hstvP.eps, hstvP.sig, hstvP.e, getDbTagData(),CommMetaData(6));
    res+= comm.receiveDoubles(epsInit, sigInit, epsP_m, epsP_cr, epsP_sh, getDbTagData(),CommMetaData(7));
    res+= comm.receiveDoubles(epsP_total, t, t_load, phi_i, Et, getDbTagData(),CommMetaData(7));
    res+= comm.receiveInts(count, crackP_flag, iter, maxSize, getDbTagData(),CommMetaData(8));
    
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::CreepMaterial::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << Color::red << getClassName() << "::" << __FUNCTION__
		<< "; failed to send data."
		<< Color::def << std::endl;
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::CreepMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(4);
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


void XC::CreepMaterial::Print(std::ostream &s, int flag) const
  {
    s << "CreepMaterial:(strain, stress, tangent) " << hstv.eps << " " << hstv.sig << " " << hstv.e << std::endl;
  }


int XC::CreepMaterial::getVariable(const std::string &varName, Information &theInfo) const
  {
    if(varName=="ec")
      {
	const RawConcrete *concrete=  this->_get_concrete_material();
	if(concrete)
	  {
	    theInfo.theDouble= concrete->getEpsc0();
	  }
        return 0;
      }
    else
      return -1;
  }

/* Methods added by AMK: */

XC::Response *XC::CreepMaterial::setResponse(const std::vector<std::string> &argv, Information &matInfo)
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

int XC::CreepMaterial::getResponse(int responseID, Information &matInfo)
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
    else if(responseID>20000)
      {
	matInfo.setDouble(this->getStrainSensitivity(responseID-20000));
	return 0;
      }
        
    switch (responseID)
      {
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

