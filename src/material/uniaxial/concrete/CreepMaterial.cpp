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


#include "CreepMaterial.h"
#include <float.h>
#include <Channel.h>
#include <Information.h>
#include <elementAPI.h>
#include <Domain.h>
#include <MaterialResponse.h>
#include <Vector.h>
#include <ID.h>
#include <FEM_ObjectBroker.h>

#include "material/uniaxial/concrete/Concrete02IS.h"
#include "material/uniaxial/ElasticMaterial.h"

static Concrete02IS defaultConcrete(0,Ec,fc,2*fc/Ec,fcu,epscu);


XC::CreepMaterial::CreepMaterial(int tag, double _fc, double _fcu, double _epscu, double _ft, double _Ec, double _beta, double _age, double _epsshu, double _epssha, double _tcr, double _epscru, double _epscra, double _epscrd, double _tcast) : EncapsulatedUniaxialMaterial(tag, MAT_TAG_CreepMaterial, defaultConcrete.getCopy()), fc(_fc), fcu(_fcu), epscu(_epscu), ft(_ft), Ec(_Ec), beta(_beta), age(_age), epsshu(_epsshu), epssha(_epssha), tcr(_tcr), epscru(_epscru), epscra(_epscra), epscrd(_epscrd), tcast(_tcast), maxSize(startSize),
  PHI_i(0), E_i(0), DSIG_i(0), TIME_i(0), DTIME_i(0)
    {
      
      Concrete02IS wrappedMaterial(0,Ec,fc,2*fc/Ec,fcu,epscu);
      //ElasticMaterial wrappedMaterial(0,Ec);
      this->setMaterial(wrappedMaterial);

      ecminP= 0.0;
      ecmaxP= 0.0;
      deptP= 0.0;

      //sigCr= fabs(sigCr);
      hstvP.setup_parameters(Ec);
      hstv.setup_parameters(Ec);

      Et= Ec;
      count= 0; //Added by AMK
      epsInit= 0.0; //Added by AMK
      sigInit= 0.0; //Added by AMK
      eps_total= 0.0; //Added by AMK
      epsP_total= 0.0; //Added by AMK

      eps_m= 0.0; //Added by AMK
      eps_cr= 0.0; //Added by AMK
      eps_sh= 0.0;
      epsP_cr= 0.0; //Added by AMK
      epsP_sh= 0.0; 
      epsP_m= 0.0; //Added by AMK

      t_load= -1.0; //Added by AMK
      crack_flag= 0;
      iter= 0;

      //Change inputs into the proper sign convention:
      fc= -fabs(fc); 
      epsshu= -fabs(epsshu);
      epscru= fabs(epscru);

      this->resize();

      TIME_i[0]= getCurrentTime();
    }

//! @brief Resize the vectors that store the creep history.
void XC::CreepMaterial::resize()
  {
    if(count+1 >= maxSize)
      {
	maxSize+= growSize;
	PHI_i.resize(maxSize);
	E_i.resize(maxSize);
	DSIG_i.resize(maxSize);
	TIME_i.resize(maxSize);
	DTIME_i.resize(maxSize);
      }
  }

XC::CreepMaterial::CreepMaterial(int tag, UniaxialMaterial &matl, double _age, double _epsshu, double _epssha, double _tcr, double _epscru, double _epscra, double _epscrd, double _tcast): 
  EncapsulatedUniaxialMaterial(tag, MAT_TAG_CreepMaterial), wrappedMaterial(0),
  age(_age), epsshu(_epsshu), epssha(_epssha), tcr(_tcr), epscru(_epscru), epscra(_epscra), epscrd(_epscrd), tcast(_tcast), maxSize(startSize),
  PHI_i(0), E_i(0), DSIG_i(0), TIME_i(0), DTIME_i(0)  
  {
    wrappedMaterial= matl.getCopy();
    if(wrappedMaterial == 0)
      {
	std::cerr << "XC::CreepMaterial::CreepMaterial - failed to get copy of material" << endln;
	exit(-1);
      }  

    // Set initial tangent
    Ec= this->getMaterial()->getInitialTangent();
    
    ecminP= 0.0;
    deptP= 0.0;
  
    //sigCr= fabs(sigCr);
    hstvP.setup_parameters(Ec);
    hstv.setup_parameters(Ec);

    Et= Ec;
    count= 0; //Added by AMK
    epsInit= 0.0; //Added by AMK
    sigInit= 0.0; //Added by AMK
    eps_total= 0.0; //Added by AMK
    epsP_total= 0.0; //Added by AMK
  
    eps_m= 0.0; //Added by AMK
    eps_cr= 0.0; //Added by AMK
    eps_sh= 0.0;
    epsP_cr= 0.0; //Added by AMK
    epsP_sh= 0.0; 
    epsP_m= 0.0; //Added by AMK
  
    t_load= -1.0; //Added by AMK
    crack_flag= 0;
    iter= 0;
  
    //Change inputs into the proper sign convention:
    epsshu= -fabs(epsshu);
    epscru= fabs(epscru);

    this->resize();

    TIME_i[0]= getCurrentTime();
  }

XC::CreepMaterial::CreepMaterial(int tag):
  EncapsulatedUniaxialMaterial(tag, MAT_TAG_CreepMaterial), wrappedMaterial(0), maxSize(startSize)
  {}

//! @brief Virtual constructor.
UniaxialMaterial *XC::CreepMaterial::getCopy(void)
  { return new CreepMaterial(*this); }

double XC::CreepMaterial::getInitialTangent(void)
  {
    return Ec; //Added by AMK
  } 

double XC::CreepMaterial::getCurrentTime(void)
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
  
    DTIME_i[count]= ops_Dt;
  
    for (int i= 1; i<=count; i++)
      {
	PHI_i[i]= setPhi(time,TIME_i[i]); //Determine PHI
	runSum += PHI_i[i]*DSIG_i[i]/Ec; //CONSTANT STRESS within Time interval
      }
  
    phi_i= PHI_i[count];
    creep= runSum;
    return creep;
  }

double XC::CreepMaterial::setPhi(double time, double tp)
  {	
    // ACI Equation:
    const double tmtp= time-tp;
    const double f1= pow((4+0.85*tp)/tp,0.5);
    const double f2= pow(tmtp,epscra)/(epscrd+pow(tmtp,epscra))*epscru;
    const double f3= (1.25*pow((tp-tcast),-0.118))/(1.25*pow(tcr,-0.118));
    const double phi= f2*f3;
    return phi;
  }

double XC::CreepMaterial::setShrink(double time)
  {
    const double &tD= age; //Age at initiation of drying
    double shrink= 0.0;
    if (time-(tD) < 0)
      {	shrink= 0.0; }
    else
      { shrink= (time-(tD)) / (epssha + (time - (tD))) * epsshu; }
    return shrink;
  }

int XC::CreepMaterial::setTrialStrain(double trialStrain, double strainRate)
  {
    const double &t= getCurrentTime();
    double tol= 1.e-4; // 9/13
    double test= 10.0; // 9/13
    double sigI= 0.0;  // 9/13
    int niter= 500;  // 9/13
  
    // Check casting age:
    if (t-tcast<(2.0-0.0001))
      { //Assumed that concrete can only carry load once hardened at 2 days following casting
	eps_cr= 0.0;
	eps_sh= 0.0;
	eps_m= 0.0;
	eps_total= trialStrain;
	sig= 0.0;
      }
    else
      { // Concrete has hardened and is ready to accept load
	// Initialize total strain:
	eps_total= trialStrain;
    
	// Calculate shrinkage Strain:
	if (iter < 1)
	  { eps_sh= setShrink(t); }
    
	// Calculate creep and mechanical strain, assuming stress remains constant in a time step:
	if (ops_Creep == 1)
	  {
	    if (fabs(t-TIME_i[count]) <= 0.0001)
	      { //If t= t(i-1), use creep/shrinkage from last calculated time step
		eps_cr= epsP_cr;
		eps_sh= epsP_sh;
		eps_m= eps_total - eps_cr - eps_sh;
		//sig= setStress(eps_m, e);
		this->getMaterial()->setTrialStrain(eps_m, strainRate);
		sig= this->getMaterial()->getStress();
		e= this->getMaterial()->getTangent();
        
      } else { // if the current calculation is a new time step
	if (iter < 1) {
	  eps_cr= setCreepStrain(t,sig); 
	}
	eps_m= eps_total - eps_cr - eps_sh;
	//sig= setStress(eps_m, e);
	this->getMaterial()->setTrialStrain(eps_m, strainRate);
	sig= this->getMaterial()->getStress();
	e= this->getMaterial()->getTangent();	
      }
    } else { //Static Analysis using previously converged time-dependent strains
      eps_cr= epsP_cr;
      eps_sh= epsP_sh;
      eps_m= eps_total-eps_cr-eps_sh;
      //sig= setStress(eps_m, e);
      this->getMaterial()->setTrialStrain(eps_m, strainRate);
      sig= this->getMaterial()->getStress();
      e= this->getMaterial()->getTangent();      
    }
  }
  iter ++;
  return 0;
}

double XC::CreepMaterial::getStrain(void)
  {
    return eps_total; //Added by AMK
    //return eps;
  }

double XC::CreepMaterial::getPHI_i(void)
  { return phi_i; }

double XC::CreepMaterial::getStress(void)
  { return sig; }

double XC::CreepMaterial::getTangent(void)
  { return e; }

double XC::CreepMaterial::getCreep(void)
  { return eps_cr; }

double XC::CreepMaterial::getShrink(void)
  { return eps_sh; }

double XC::CreepMaterial::getMech(void)
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
    if(fabs(eps_m/sig)>Ec)
      { E_i[count+1]= Ec; }
    else
      { E_i[count+1]= fabs(hstv.sig/eps_m); }//ADDED 7/22
  
    if(isnan(E_i[count+1]))
      { E_i[count+1]= Ec; }
  
  
    TIME_i[count+1]= getCurrentTime();
  
    hstvP.e= hstv.e;
    hstvP.sig= hstv.sig;
    hstvP.eps= hstv.eps;
	
    //Added by AMK:
    epsP_total= eps_total; //Added by AMK;
    epsP_sh= eps_sh;
    epsP_cr= eps_cr;
    epsP_m= eps_m;
    const double fcOverEc= (this->getMaterial() == 0) ? fc/Ec : 0.002; // Assumed by MHS
    if(eps_m < 0 && fabs(eps_m)>0.50*fabs(fcOverEc))
      {
	//double s= fabs(eps_m/fc)*Ec;
	double s= fabs(eps_m/fcOverEc);    
	s= 0.5*fabs(fcOverEc);
	//std::cerr << "Strain Compression Limit Exceeded: " << eps_m << ' ' << -s << endln;
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
	sigInit= sigP;
      }
  
    if(sigInit<0.0 && t_load<0.0)
      {
	t_load= getCurrentTime();
	sigInit= sigP;
	epsInit= epsP_m;
      }
    else if(sigInit>0.0 && sigP<0.0 && t_load<0.0)
      {
	t_load= getCurrentTime();
	sigInit= sigP;
	epsInit= epsP_m;
      }

    this->getMaterial()->commitState();
  
  //if (ops_Creep==1) {
  //	count++;
  //}
  count++;
  
  return 0;
}

int 
XC::CreepMaterial::revertToLastCommit(void)
{
  eps_total= epsP_total; //Added by AMK;
  eps_sh= epsP_sh;
  eps_cr= epsP_cr;
  eps_m= epsP_m;  
  
  ecmin= ecminP;;
  dept= deptP;
  
  e= eP;
  sig= sigP;
  eps= epsP;

  this->getMaterial()->revertToLastCommit();
  
  return 0;
}

int 
XC::CreepMaterial::revertToStart(void)
{
  ecminP= 0.0;
  deptP= 0.0;
  
  eP= Ec;
  epsP= 0.0;
  sigP= 0.0;
  eps= 0.0;
  sig= 0.0;
  e= Ec;
  
  if (ops_Creep==0) {
    count= 0;
  } else {
    count= 1;
  }

  this->getMaterial()->revertToStart();
  
  return 0;
}

int 
XC::CreepMaterial::sendSelf(int commitTag, Channel &theChannel)
{
  int res= 0;

  int dbTag= this->getDbTag();

  static ID classTags(4);

  classTags(0)= this->getMaterial()->getClassTag();

  int matDbTag= this->getMaterial()->getDbTag();
  if (matDbTag == 0) {
    matDbTag= theChannel.getDbTag();
    if (matDbTag != 0)
      this->getMaterial()->setDbTag(matDbTag);
  }
  classTags(1)= matDbTag;
  classTags(2)= this->getTag();
  classTags(3)= maxSize;
  
  res= theChannel.sendID(dbTag, commitTag, classTags);
  if (res < 0)
    {
      std::cerr << "XC::CreepMaterial::sendSelf -- could not send ID" << endln;
      return res;
    }

  Vector data(16 + 6 + 21 + maxSize*5);
  data(0)= fc;    
  data(1)= epsc0; 
  data(2)= fcu;   
  data(3)= epscu; 
  data(4)= tcr;   
  data(5)= ft;    
  data(6)= Ets;   
  data(7)= Ec;
  data(8)= age; 
  data(9)= epsshu;
  data(10)= epssha;
  data(11)= epscra;
  data(12)= epscru;
  data(13)= beta;
  data(14)= epscrd;
  data(15)= tcast;

  data(16)= ecminP;
  data(17)= ecmaxP;
  data(18)= deptP;
  data(19)= epsP;
  data(20)= sigP;
  data(21)= eP;

  data(22)= count;
  data(23)= epsInit;
  data(24)= sigInit;
  data(25)= eps_cr;
  data(26)= eps_sh;
  data(27)= eps_T;
  data(28)= eps_m;
  data(29)= epsP_m;    
  data(30)= epsP_cr;
  data(31)= epsP_sh;
  data(32)= eps_total;
  data(33)= epsP_total;
  data(34)= e_total;
  data(35)= eP_total;
  data(36)= t;
  data(37)= t_load;
  data(38)= phi_i;
  data(39)= Et;
  data(40)= crack_flag;
  data(41)= crackP_flag;
  data(42)= iter;
  
  for (int i= 0; i < maxSize; i++) {
    data(43             + i)= PHI_i[i];
    data(43 +   maxSize + i)= E_i[i];
    data(43 + 2*maxSize + i)= DSIG_i[i];
    data(43 + 3*maxSize + i)= TIME_i[i];
    data(43 + 4*maxSize + i)= DTIME_i[i];            
  }
  
  res= theChannel.sendVector(this->getDbTag(), commitTag, data);
  if (res < 0) {
    std::cerr << "XC::CreepMaterial::sendSelf - failed to send Vector" << endln;
    return res;
  }
  
  res= this->getMaterial()->sendSelf(commitTag, theChannel);
  if (res < 0) {
    std::cerr << "XC::CreepMaterial::sendSelf -- could not send UniaxialMaterial" << endln;
    return res;
  }
	
  return res;
}

int 
XC::CreepMaterial::recvSelf(int commitTag, Channel &theChannel, 
			FEM_ObjectBroker &theBroker)
{
  int res= 0;

  static ID idata(4);
  int dbTag= this->getDbTag();

  res= theChannel.recvID(dbTag, commitTag, idata);
  if (res < 0) {
    std::cerr << "XC::CreepMaterial::recvSelf() - failed to receive data\n";
    return res;
  }

  this->setTag(idata(2));  
  maxSize= idata(3);
  Vector data(16 + 6 + 21 + maxSize*5);

  if (theChannel.recvVector(this->getDbTag(), commitTag, data) < 0) {
    std::cerr << "XC::CreepMaterial::recvSelf() - failed to recvSelf\n";
    return -1;
  }
  
  fc= data(0);
  epsc0= data(1);
  fcu= data(2);
  epscu= data(3);
  tcr= data(4);
  ft= data(5);
  Ets= data(6);
  Ec= data(7);
  age= data(8);
  epsshu= data(9);
  epssha= data(10);
  epscra= data(11);
  epscru= data(12);
  beta= data(13);
  epscrd= data(14);
  tcast= data(15);

  ecminP= data(16);
  ecmaxP= data(17);
  deptP= data(18);
  epsP= data(19);
  sigP= data(20);
  eP= data(21);

  count= data(22);
  epsInit= data(23);
  sigInit= data(24);
  eps_cr= data(25);
  eps_sh= data(26);
  eps_T= data(27);
  eps_m= data(28);
  epsP_m= data(29);
  epsP_cr= data(30);
  epsP_sh= data(31);
  eps_total= data(32);
  epsP_total= data(33);
  e_total= data(34);
  eP_total= data(35);
  t= data(36);
  t_load= data(37);
  phi_i= data(38);
  Et= data(39);
  crack_flag= data(40);
  crackP_flag= data(41);
  iter= data(42);

  if (PHI_i != 0) delete [] PHI_i;
  PHI_i= new float [maxSize];
  if (E_i != 0) delete [] E_i;
  E_i= new float [maxSize];
  if (DSIG_i != 0) delete [] DSIG_i;
  DSIG_i= new float [maxSize];
  if (TIME_i != 0) delete [] TIME_i;
  TIME_i= new float [maxSize];
  if (DTIME_i != 0) delete [] DTIME_i;
  DTIME_i= new float [maxSize];  
  
  for (int i= 0; i < maxSize; i++) {
    PHI_i[i]  = data(43             + i);
    E_i[i]    = data(43 +   maxSize + i);
    DSIG_i[i] = data(43 + 2*maxSize + i);
    TIME_i[i] = data(43 + 3*maxSize + i);
    DTIME_i[i]= data(43 + 4*maxSize + i);
  }

  e= eP;
  sig= sigP;
  eps= epsP;
  
  

  int matClassTag= idata(0);
  
  // Check if material is null
  if (this->getMaterial() == 0) {
    wrappedMaterial= theBroker.getNewUniaxialMaterial(matClassTag);
    if (this->getMaterial() == 0) {
      std::cerr << "XC::CreepMaterial::recvSelf -- could not get a UniaxialMaterial" << endln;
      return -1;
    }
  }

  dbTag= idata(1);
  if (this->getMaterial()->getClassTag() != matClassTag) {
    delete wrappedMaterial;
    wrappedMaterial= theBroker.getNewUniaxialMaterial(matClassTag);
    if (this->getMaterial() == 0) {
      std::cerr << "XC::CreepMaterial::recvSelf -- could not get a UniaxialMaterial" << endln;
      return -1;
    }
  }
  
  this->getMaterial()->setDbTag(dbTag);
  res= this->getMaterial()->recvSelf(commitTag, theChannel, theBroker);
  if (res < 0) {
    std::cerr << "XC::CreepMaterial::recvSelf -- count not receive Uniaxialmaterial" << endln;
    return res;
  }
  
  return res;
}

void XC::CreepMaterial::Print(std::ostram &s, int flag)
  {
    s << "CreepMaterial:(strain, stress, tangent) " << hstv.eps << " " << hstv.sig << " " << hstv.e << std::endl;
  }


int
XC::CreepMaterial::getVariable(const char *varName, Information &theInfo)
{
  if (strcmp(varName,"ec") == 0) {
    theInfo.theDouble= epsc0;
    return 0;
  } else
    return -1;
}

/* Methods added by AMK: */

Response* 
XC::CreepMaterial::setResponse(const char **argv, int argc,
							  OPS_Stream &theOutput)
{	
  Response *theResponse= 0;
  
  theOutput.tag("UniaxialMaterialOutput");
  theOutput.attr("matType", this->getClassType());
  theOutput.attr("matTag", this->getTag());
  
  // stress
  if (strcmp(argv[0],"stress") == 0) {
    theOutput.tag("ResponseType", "sigma11");
    theResponse=  new MaterialResponse(this, 1, this->getStress());
  }  
  // tangent
  else if (strcmp(argv[0],"tangent") == 0) {
    theOutput.tag("ResponseType", "C11");
    theResponse=  new MaterialResponse(this, 2, this->getTangent());
  }
  
  // strain
  else if (strcmp(argv[0],"strain") == 0) {
    theOutput.tag("ResponseType", "eps11");
    theResponse=  new MaterialResponse(this, 3, this->getStrain());
  }
  
  // strain
  else if ((strcmp(argv[0],"stressStrain") == 0) || 
	   (strcmp(argv[0],"stressANDstrain") == 0) ||
	   (strcmp(argv[0],"stressAndStrain") == 0)) {
    theOutput.tag("ResponseType", "sig11");
    theOutput.tag("ResponseType", "eps11");
    theResponse=  new MaterialResponse(this, 4, Vector(2));
  }
  
  else if (strcmp(argv[0],"CreepStressStrainTangent")==0) {
    theOutput.tag("ResponseType", "sig11");
    theOutput.tag("ResponseType", "eps11");
    theOutput.tag("ResponseType", "C11");
    theOutput.tag("ResponseType", "CreepStrain");
    theOutput.tag("ResponseType", "MechStrain");
    theOutput.tag("ResponseType", "ShrinkStrain");
    theOutput.tag("ResponseType", "t_load");
    theResponse= new MaterialResponse(this, 6, Vector(6));
  }
  
  else if ((strcmp(argv[0],"stressStrainTangent") == 0) || 
	   (strcmp(argv[0],"stressANDstrainANDtangent") == 0)) {
    theOutput.tag("ResponseType", "sig11");
    theOutput.tag("ResponseType", "eps11");
    theOutput.tag("ResponseType", "C11");
    theResponse=  new MaterialResponse(this, 5, Vector(3));
  }
  
  // stress sensitivity for local sensitivity recorder purpose.  Quan 2009
  // limit:  no more than 10000 random variables/sensitivity parameters
  else if (strstr(argv[0],"stressSensitivity") != 0) {
    char *token= strtok((char *) argv[0], " ");
    if (token != NULL) token= strtok(NULL, " ");
    int gradient= atoi(token);
    theOutput.tag("ResponseType", "sigsens11");
    theResponse=  new MaterialResponse(this, gradient+10000, this->getStress());
  }
  // strain sensivitiy
  else if (strstr(argv[0],"strainSensitivity") != 0) {
    char *token= strtok((char *) argv[0], " ");
    if (token != NULL) token= strtok(NULL, " ");
    int gradient= atoi(token);
    theOutput.tag("ResponseType", "epssens11");
    theResponse=  new MaterialResponse(this, gradient+20000, this->getStrain());
  }
  
  
  theOutput.endTag();
  return theResponse;
}

int XC::CreepMaterial::getResponse(int responseID, Information &matInfo)
{
  static Vector stressStrain(2);
  static Vector stressStrainTangent(3);
  static Vector CreepStressStrainTangent(6); //Added by AMK
  // each subclass must implement its own stuff   
  
  // added for sensitivity recorder. Quan 2009
  if ((responseID>10000)&&(responseID<20000)){
    matInfo.setDouble(this->getStressSensitivity(responseID-10000,false));
    return 0;
  }
  else if (responseID>20000){
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
    stressStrain(0)= this->getStress();
    stressStrain(1)= this->getStrain();
    matInfo.setVector(stressStrain);
    return 0;
    
  case 5:
    stressStrainTangent(0)= this->getStress();
    stressStrainTangent(1)= this->getStrain();
    stressStrainTangent(2)= this->getTangent();
    matInfo.setVector(stressStrainTangent);
    return 0;
    
  case 6:
    CreepStressStrainTangent(0)= this->getStress();
    CreepStressStrainTangent(1)= this->getStrain();
    CreepStressStrainTangent(2)= this->getTangent();
    CreepStressStrainTangent(3)= this->getCreep();
    CreepStressStrainTangent(4)= this->getMech();
    CreepStressStrainTangent(5)= this->getShrink();
    matInfo.setVector(CreepStressStrainTangent);
    return 0;
    
  default:      
    return -1;
  }
}
