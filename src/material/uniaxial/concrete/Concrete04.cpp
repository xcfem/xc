//----------------------------------------------------------------------------
//  XC program; finite element analysis code
//  for structural analysis and design.
//
//  Copyright (C)  Luis Claudio Pérez Tato
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
                                                                        
// $Revision: 1.5 $
// $Date: 2006/08/04 17:34:58 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Concrete04.cpp,v $
                                                                        
// Written: N.Mitra (nmitra@u.washington.edu) 
// Created: 09/04
// Revision: A
//
// Description: This file contains the class implementation for 
// Concrete04 based on Popovics pre and post compression curve 
// for concrete. 
//
// What: "@(#) Concrete04.C, revA"
// Revision 1. Adding in exponential tensile envelope for concrete
// Dt. 05-16-05


#include <material/uniaxial/concrete/Concrete04.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/Matrix.h>

#include <domain/mesh/element/utils/Information.h>
#include <cmath>
#include <cfloat>

//! @brief Sets initial values of concrete parameters.
void XC::Concrete04::setup_parameters(void)
  {
    // Make all concrete parameters negative
    if(fpc > 0.0 || epsc0 > 0.0 || epscu > 0.0)
      {
        std::cerr << "error: negative values required for concrete stress-strain model"
                  << std::endl;
      }
  
    if(fct < 0.0)
      {
        fct= 0.0;
        std::cerr << "warning: fct less than 0.0 so the tensile response part is being set to 0"
                  << std::endl;
      }
    convergedState.Tangent()= Ec0;
    convergedHistory.UnloadSlope()= Ec0;
    CUtenSlope= Ec0;
  
    // Set trial values
    this->revertToLastCommit();
  }

//! @brief Constructor.
XC::Concrete04::Concrete04(int tag, double FPC, double EPSC0, double EPSCU, double EC0, double FCT, double ETU)
  :ConcreteBase(tag, MAT_TAG_Concrete04,FPC,EPSC0,EPSCU),
   Ec0(EC0), fct(FCT), etu(ETU), beta(0.1),
   CmaxStrain(0.0), CcompStrain(0.0), CUtenStress(FCT)
  { setup_parameters(); }

//! @brief Constructor.
XC::Concrete04::Concrete04(int tag, double FPC, double EPSC0, double EPSCU, double EC0, double FCT, double ETU, double BETA)
  :ConcreteBase(tag, MAT_TAG_Concrete04,FPC,EPSC0,EPSCU),
   Ec0(EC0), fct(FCT), etu(ETU), beta(BETA),CmaxStrain(0.0), CcompStrain(0.0), CUtenStress(FCT)
  { setup_parameters(); }

//! @brief Constructor.
XC::Concrete04::Concrete04(int tag, double FPC, double EPSC0, double EPSCU, double EC0)
  :ConcreteBase(tag, MAT_TAG_Concrete04,FPC,EPSC0,EPSCU),
   Ec0(EC0), fct(0.0), etu(0.0), beta(0.0),
   CmaxStrain(0.0), CcompStrain(0.0), CUtenStress(0.0)
  { setup_parameters(); }


//! @brief Constructor.
XC::Concrete04::Concrete04(int tag)
  :ConcreteBase(tag, MAT_TAG_Concrete04,0.0,0.0,0.0),
   Ec0(0.0), fct(0.0), etu(0.0), beta(0.0),
   CmaxStrain(0.0), CcompStrain(0.0), CUtenStress(0.0), CUtenSlope(0.0)
  { setup_parameters(); }

//! @brief Set the trial stress.
int XC::Concrete04::setTrialStrain(double strain, double strainRate)
  {
    // Reset trial history variables to last committed state
    trialHistory= convergedHistory;
    TmaxStrain= CmaxStrain;   
    TUtenSlope= CUtenSlope;
    trialState= convergedState;   


    /* // Set trial strain*/  
    if(fct == 0.0 && strain > 0.0)
      {    
        trialState.Strain()= strain;
        trialState.Stress()= 0.0;    
        trialState.Tangent()= 0.0;    
        TUtenSlope= 0.0;    
        return 0;  
      }

    /*// Determine change in strain from last converged state*/  
    const double dStrain= strain - convergedState.getStrain();

    if(fabs(dStrain) < DBL_EPSILON)       
      return 0;

    trialState.Strain()= strain;  

  /*// Calculate the trial state given the change in strain  // determineTrialState (dStrain);*/
     trialHistory.UnloadSlope()= convergedHistory.getUnloadSlope();  
  TUtenSlope= CUtenSlope;
  if (dStrain <= 0.0) {	  /*// Material can be either in Compression-Reloading	  // or Tension-Unloading state.*/
    if (trialState.getStrain() > 0.0) {         
      /*// Material is in Tension-Unloading State*/		  
      trialState.Tangent()= TUtenSlope;		  
      trialState.Stress()= trialState.getStrain() * TUtenSlope; 	  
    } else {
      // Material is in Compression-Reloading State
      trialHistory= convergedHistory;      
      CompReload();
    }
  } else {
    /*// Material can be either in Compression-Unloading	  // or Tension-Reloading State.*/
    if (trialState.getStrain() >= 0.0) {    /*// Material is in Tension-Reloading State*/      
      TmaxStrain= CmaxStrain;                  
      if (trialState.getStrain() < TmaxStrain) {        
	trialState.Stress()= trialState.getStrain() * CUtenSlope;        
	trialState.Tangent()= CUtenSlope;        
	TUtenSlope= CUtenSlope;      
      } else {        
	TmaxStrain= trialState.getStrain();        
	TensEnvelope();        
	setTenUnload();      
      }        
    } else {
       if(trialState.getStrain() <= trialHistory.getEndStrain()) {          
	trialState.Tangent()= trialHistory.getUnloadSlope();          
	trialState.Stress()= trialState.getTangent() * (trialState.getStrain() - trialHistory.getEndStrain());        
      } else {          
	trialState.Stress()= 0.0;          
	trialState.Tangent()= 0.0;        
      }        
    }  
  }    
  return 0;
}

void XC::Concrete04::CompReload()
{
  if (trialState.getStrain() <= trialHistory.getMinStrain()) {
    
    trialHistory.MinStrain()= trialState.getStrain();
    
    /*// Determine point on envelope*/
    CompEnvelope ();
    setCompUnloadEnv ();
    
  }
  else if (trialState.getStrain() < trialHistory.getEndStrain()) {
    trialState.Tangent()= trialHistory.getUnloadSlope();
    trialState.Stress()= trialState.getTangent()*(trialState.getStrain()-trialHistory.getEndStrain());
  }
  else if (trialState.getStrain() <= 0.0) {
    trialState.Stress()= 0.0;
    trialState.Tangent()= 0.0;
  }
}

void XC::Concrete04::CompEnvelope()
{
  if (trialState.getStrain() >= epscu) {
    double Esec= fpc/epsc0;
    double r= 0.0;
    if (Esec >= Ec0) {
      r= 400.0;
    } else {
      r= Ec0/(Ec0-Esec);
    }
    double eta= trialState.getStrain()/epsc0;
    trialState.Stress()= fpc*eta*r/(r-1+pow(eta,r));
    trialState.Tangent()= fpc*r*(r-1)*(1-pow(eta,r))/(pow((r-1+pow(eta,r)),2)*epsc0);
  } else {
    trialState.Stress()= 0.0;
    trialState.Tangent()= 0.0;
  }
  
}

void XC::Concrete04::setCompUnloadEnv()
  {
    double tempStrain= trialHistory.getMinStrain();
  
  if (tempStrain < epscu)
    tempStrain= epscu;
  
  double eta= tempStrain/epsc0;
  
  double ratio= 0.707*(eta-2.0) + 0.834; // unloading parameter as per Karsan-Jirsa
  
  if (eta < 2.0)
    ratio= 0.145*eta*eta + 0.13*eta;
  
  trialHistory.EndStrain()= ratio*epsc0;
  
  double temp1= trialHistory.getMinStrain() - trialHistory.getEndStrain();
  
  double temp2= trialState.getStress()/Ec0;
  
  if (temp1 > -DBL_EPSILON) {	// temp1 should always be negative
    trialHistory.UnloadSlope()= Ec0;
  }
  else if (temp1 <= temp2) {
    trialHistory.EndStrain()= trialHistory.getMinStrain() - temp1;
    trialHistory.UnloadSlope()= trialState.getStress()/temp1;
  }
  else {
    trialHistory.EndStrain()= trialHistory.getMinStrain() - temp2;
    trialHistory.UnloadSlope()= Ec0;
  }
  
  
  if (trialState.getStrain() >= 0.0) {
    /*std::cerr << "actually made it in here" << std::endl;*/
    /*trialHistory.UnloadSlope()= Ec0;*/
  }
  
}

void XC::Concrete04::TensReload()
{  TensEnvelope();  setTenUnload();}

void XC::Concrete04::TensEnvelope()
{  double ect= fct / Ec0;    
  if (trialState.getStrain() <= ect) {    
    trialState.Stress()= trialState.getStrain() * Ec0;    
    trialState.Tangent()= Ec0;
  } else if (trialState.getStrain() > etu) {    
    trialState.Stress()= 0.0;    
    trialState.Tangent()= 0.0;  
  } else {    
    trialState.Stress()= fct * pow(beta, (trialState.getStrain() - ect) / (etu - ect));    
    trialState.Tangent()= fct * pow(beta, (trialState.getStrain() - ect) / (etu - ect)) * log(beta) / (etu - ect);  
  }
}

void XC::Concrete04::setTenUnload(){
  TUtenStress= trialState.getStress();
  TUtenSlope= trialState.getStress() / trialState.getStrain();
}

int XC::Concrete04::commitState(void)
  {
    convergedHistory= trialHistory; 
    CmaxStrain= TmaxStrain;   
    CUtenSlope= TUtenSlope;
    convergedState= trialState;
    return 0;
  }

int XC::Concrete04::revertToLastCommit(void)
  {
    trialHistory= convergedHistory;
    TmaxStrain= CmaxStrain;   
    TUtenSlope= CUtenSlope;  
    trialState= convergedState;   
    return 0;
  }

int XC::Concrete04::revertToStart(void)
  {
    // History variables
    convergedHistory.revertToStart(Ec0); // History variables
    CmaxStrain= 0.0;   
    CUtenSlope= Ec0;
    // State variables
    convergedState.revertToStart(Ec0);
    revertToLastCommit();
    return 0;
  }

XC::UniaxialMaterial *XC::Concrete04::getCopy(void) const
  { return new Concrete04(*this); }

int XC::Concrete04::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::Concrete04::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::Concrete04::Print (std::ostream& s, int flag)
{
  s << "Concrete04, tag: " << this->getTag() << std::endl;
  s << "  fpc: " << fpc << std::endl;
  s << "  epsc0: " << epsc0 << std::endl;
  s << "  fct: " << fct << std::endl;
  s << "  epscu: " << epscu << std::endl;
  s << "  Ec0:  " << Ec0 << std::endl;
  s << "  etu:  " << etu << std::endl;
  s << "  beta: " << beta << std::endl;
}

/*// LOWES: add functions for variable hinge-length model*/
int XC::Concrete04::getMaterialType()
{
	return 0;
}
/*// LOWES: end*/
