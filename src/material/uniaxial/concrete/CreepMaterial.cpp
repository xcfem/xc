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

XC::CreepMaterial::CreepMaterial(int tag)
  : EncapsulatedUniaxialMaterial(tag, MAT_TAG_CreepMaterial),
    beta(0.4),
    creepShrinkageState(),
    iter(0),
    creepShrinkageParameters()
  {}

XC::CreepMaterial::CreepMaterial(int tag, double _fc, double _fcu, double _epscu, double _ft, double _Ec, double _beta, double _age, double _tcast, const ACICreepShrinkageParameters &_csParameters)
  : EncapsulatedUniaxialMaterial(tag, MAT_TAG_CreepMaterial),
    beta(_beta),
    creepShrinkageState(_age, _tcast, _Ec),
    iter(0),
    creepShrinkageParameters(_csParameters)
  {
      
    Concrete02IS wrappedMaterial(0,_Ec, _fc, 2*_fc/_Ec,_fcu,_epscu);
    wrappedMaterial.setFt(_ft);
    //ElasticMaterial wrappedMaterial(0,_Ec);
    this->setMaterial(wrappedMaterial);

    //sigCr= fabs(sigCr);
    hstvP.setup_parameters(_Ec);
    hstv.setup_parameters(_Ec);

    creepShrinkageParameters.setup_parameters();

    creepSteps.initTime(getCurrentTime());
  }

XC::CreepMaterial::CreepMaterial(int tag, UniaxialMaterial &matl, double _age, double _tcast, const ACICreepShrinkageParameters &_csParameters)
  : EncapsulatedUniaxialMaterial(tag, MAT_TAG_CreepMaterial, matl),
    beta(0.0),
    creepShrinkageState(_age, _tcast, 0.0),
    iter(0),
    creepShrinkageParameters(_csParameters)
  {
    // Get initial tangent
    const double &_Ec= this->getInitialTangent();

    creepShrinkageState.setup_parameters(_Ec);
    //sigCr= fabs(sigCr);
    hstvP.setup_parameters(_Ec);
    hstv.setup_parameters(_Ec);
    
    //Change inputs into the proper sign convention:
    creepShrinkageParameters.setup_parameters();

    creepSteps.initTime(getCurrentTime());
  }

//! @brief Sets initial values for the concrete parameters.
void XC::CreepMaterial::setup_parameters(void)
  {
    // Get initial tangent
    const double &_Ec= this->getInitialTangent();
    
    this->creepShrinkageState.setup_parameters(_Ec);
    //sigCr= fabs(sigCr);
    this->hstvP.setup_parameters(_Ec);
    this->hstv.setup_parameters(_Ec);

    this->iter= 0;
    
    creepShrinkageParameters.setup_parameters();
    creepSteps.initTime(getCurrentTime());
  }

//! @brief Virtual constructor.
XC::UniaxialMaterial *XC::CreepMaterial::getCopy(void) const
  { return new CreepMaterial(*this); }

void XC::CreepMaterial::setCreepShrinkageParameters(const ACICreepShrinkageParameters &csParameters)
  { this->creepShrinkageParameters= csParameters; }

const XC::ACICreepShrinkageParameters &XC::CreepMaterial::getCreepShrinkageParameters(void) const
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
    const double _Ec= this->getTangent();
    
    const double retval= creepSteps.computePhi(*this, _Ec, time);
    phi_i= creepSteps.getLastPhi();
    return retval;    
  }

double XC::CreepMaterial::setPhi(double time, double tp) const
  {	
    // ACI Equation:
    //const double f1= pow((4+0.85*tp)/tp,0.5);
    const double f2= creepShrinkageParameters.getF2(time, tp);
    const double f3= (1.25*pow((creepShrinkageState.getCastingAge(tp)),-0.118))/(1.25*pow(creepShrinkageParameters.getCreepRelationshipAge(),-0.118));
    const double phi= f2*f3;
    return phi;
  }

double XC::CreepMaterial::setShrink(double time)
  {
    return creepShrinkageParameters.getShrink(creepShrinkageState.getAge(), time);
  }

int XC::CreepMaterial::setTrialStrain(double trialStrain, double strainRate)
  {
    const double &t= getCurrentTime();
    // double tol= 1.e-4; // 9/13
    // double test= 10.0; // 9/13
    // double sigI= 0.0;  // 9/13
    // int niter= 500;  // 9/13
  
    // Check casting age:
    if (creepShrinkageState.getCastingAge(t)<(2.0-0.0001))
      { //Assumed that concrete can only carry load once hardened at 2 days following casting
	creepShrinkageState.set_eps_cr_sh_m_total(0.0, 0.0, 0.0, trialStrain);
	hstv.sig= 0.0;
      }
    else
      { // Concrete has hardened and is ready to accept load
	// Initialize total strain:
	creepShrinkageState.set_total_strain(trialStrain); // Total strain.
    
	// Calculate shrinkage Strain:
	if(this->iter < 1)
	  { creepShrinkageState.setShrink(setShrink(t)); }
    
	// Calculate creep and mechanical strain, assuming stress remains constant in a time step:
	UniaxialMaterial *mat= this->getMaterial();
	if(creepSteps.isCreepOn())
	  {
	    if (fabs(t-creepSteps.getLastTime()) <= 0.0001)
	      { //If t= t(i-1), use creep/shrinkage from last calculated time step
		creepShrinkageState.use_creep_shrinkage_from_last_commit();
		//hstv.sig= setStress(creepShrinkageState.getMech(), e);
		mat->setTrialStrain(creepShrinkageState.getMech(), strainRate);
		hstv.sig= mat->getStress();
		hstv.e= mat->getTangent();
	      }
	    else
	      { // if the current calculation is a new time step
		if(this->iter < 1)
		  {
		    creepShrinkageState.setCreep(setCreepStrain(t,hstv.sig)); // Creep strain.
		  }
		creepShrinkageState.update_mech_strain();
		//sig= setStress(creepShrinkageState.getMech(), e);
		mat->setTrialStrain(creepShrinkageState.getMech(), strainRate);
		hstv.sig= mat->getStress();
		hstv.e= mat->getTangent();
	      }
	  }
	else
	  { //Static Analysis using previously converged time-dependent strains
	    creepShrinkageState.use_creep_shrinkage_from_last_commit();
	    //sig= setStress(creepShrinkageState.getMech(), e);
	    mat->setTrialStrain(creepShrinkageState.getMech(), strainRate);
	    hstv.sig= mat->getStress();
	    hstv.e= mat->getTangent();
	  }
      }
    iter++;
    return 0;
  }

double XC::CreepMaterial::getStrain(void) const
  {
    return creepShrinkageState.getStrain(); //Added by AMK
    //return eps;
  }

double XC::CreepMaterial::getPHI_i(void) const
  { return phi_i; }

double XC::CreepMaterial::getStress(void) const
  { return hstv.sig; }

double XC::CreepMaterial::getTangent(void) const
  { return hstv.e; }

double XC::CreepMaterial::getCreep(void) const
  { return creepShrinkageState.getCreep(); }

double XC::CreepMaterial::getShrink(void) const
  { return creepShrinkageState.getShrink(); }

double XC::CreepMaterial::getMech(void) const
  { return creepShrinkageState.getMech(); }

int XC::CreepMaterial::commitState(void)
  {
    this->iter= 0;
    hstvP.ecmin= hstv.ecmin;
    hstvP.ecmax= hstv.ecmax;
    hstvP.dept= hstv.dept;

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
    const double _Ec= this->getTangent();
    const double currentTime= this->getCurrentTime();
    const double eps_mech= creepShrinkageState.getMech();
    creepSteps.assignNextStep(this->hstv, this->hstvP, _Ec, eps_mech, currentTime);
  
    hstvP.e= hstv.e;
    hstvP.sig= hstv.sig;
    hstvP.eps= hstv.eps;
	
    //Added by AMK:
    const int count= this->creepSteps.getCount();
    creepShrinkageState.commit_state(count, this->hstvP.sig, currentTime);
    
    const RawConcrete *concrete= this->_get_concrete_material();
    if(concrete)
      {
	const double &fc= concrete->getFpc();
	const double fcOverEc= (this->getMaterial() == 0) ? fc/_Ec : 0.002; // Assumed by MHS
	if(eps_mech < 0 && fabs(eps_mech)>0.50*fabs(fcOverEc))
	  {
	    //double s= fabs(eps_m/fc)*_Ec;
	    double s= fabs(eps_mech/fcOverEc);    
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
    
    this->getMaterial()->commitState();
  
    //if (ops_Creep==1) {
    //	count++;
    //}
    creepSteps.next();
  
    return 0;
  }

int XC::CreepMaterial::revertToLastCommit(void)
  {
    creepShrinkageState.revert_to_last_commit();

    hstv= hstvP; // revert history variables.

    this->getMaterial()->revertToLastCommit();
  
    return 0;
  }

int XC::CreepMaterial::revertToStart(void)
  {
    const double &_Ec= this->getInitialTangent();
    
    creepShrinkageState.revert_to_start(_Ec);
    
    hstvP.revertToStart(_Ec);

    hstv.setup_parameters(_Ec);
  
    creepSteps.revertToStart();

    this->getMaterial()->revertToStart();
  
    return 0;
  }

//! @brief Send object members through the communicator argument.
int XC::CreepMaterial::sendData(Communicator &comm)
  {
    int res= EncapsulatedUniaxialMaterial::sendData(comm);
    res+= comm.sendMovable(creepShrinkageParameters, getDbTagData(),CommMetaData(4));
    res+= comm.sendMovable(creepShrinkageState, getDbTagData(),CommMetaData(5));
    res+= comm.sendDoubles(hstvP.ecmin, hstvP.ecmax, hstvP.dept, hstvP.eps, hstvP.sig, hstvP.e, getDbTagData(),CommMetaData(6));
    res+= comm.sendDoubles(beta, phi_i, getDbTagData(),CommMetaData(7));
    res+= comm.sendInt(iter, getDbTagData(),CommMetaData(8));
    return res;
  }
//! @brief Receives object members through the communicator argument.
int XC::CreepMaterial::recvData(const Communicator &comm)
  {
    int res= EncapsulatedUniaxialMaterial::recvData(comm);
    res+= comm.receiveMovable(creepShrinkageParameters, getDbTagData(),CommMetaData(4));
    res+= comm.receiveMovable(creepShrinkageState, getDbTagData(),CommMetaData(5));
    res+= comm.receiveDoubles(hstvP.ecmin, hstvP.ecmax, hstvP.dept, hstvP.eps, hstvP.sig, hstvP.e, getDbTagData(),CommMetaData(6));
    res+= comm.receiveDoubles(beta, phi_i, getDbTagData(),CommMetaData(7));
    res+= comm.receiveInt(iter, getDbTagData(),CommMetaData(8));
    
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::CreepMaterial::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(9);
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
    inicComm(9);
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
    s << "CreepMaterial:(strain, stress, tangent) "
      << hstv.eps << " "
      << hstv.sig << " "
      << hstv.e
      << std::endl;
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

