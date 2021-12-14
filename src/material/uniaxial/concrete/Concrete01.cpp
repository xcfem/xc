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

// $Revision: 1.11 $
// $Date: 2004/07/15 21:34:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Concrete01.cpp,v $


// File: ~/material/Concrete01.C
//
// Written: MHS
// Created: 06/99
// Revision: A
//
// Description: This file contains the class implementation for
// Concrete01.
//
// What: "@(#) Concrete01.C, revA"


#include <material/uniaxial/concrete/Concrete01.h>
#include "domain/component/Parameter.h"
#include <utility/matrix/Vector.h>

#include <domain/mesh/element/utils/Information.h>
#include <cmath>
#include <cfloat>
#include "utility/actor/actor/MatrixCommMetaData.h"

//int count= 0;

//! @brief Verifies that all concrete parameters
//! for compression are negative.
void XC::Concrete01::make_negative(void)
  {
    if(fpc > 0.0) fpc= -fpc;
    if(epsc0 > 0.0) epsc0= -epsc0;
    if(fpcu > 0.0) fpcu= -fpcu;
    if(epscu > 0.0) epscu= -epscu;
  }

//! @brief Sets initial values for the concrete parameters.
void XC::Concrete01::setup_parameters(void)
  {
    // Initial tangent
    const double Ec0= 2*fpc/epsc0;
    convergedState.Tangent()= Ec0;
    convergedHistory.UnloadSlope()= Ec0;
    trialState.Tangent()= Ec0;
    // Set trial values
    revertToLastCommit();
  }

//! @brief Constructor.
XC::Concrete01::Concrete01(int tag, double FPC, double EPSC0, double FPCU, double EPSCU)
  :ConcreteBase(tag, MAT_TAG_Concrete01,FPC,EPSC0,EPSCU), fpcu(FPCU),
   parameterID(0), SHVs()
  {
    //count++;
    //Make all concrete parameters negative
    make_negative();
    setup_parameters();
  }

//! @brief Constructor
XC::Concrete01::Concrete01(int tag)
  :ConcreteBase(tag, MAT_TAG_Concrete01,0.0,0.0,0.0), fpcu(0.0),
   parameterID(0), SHVs()
  {
    // Set trial values
    revertToLastCommit();
  }

XC::Concrete01::Concrete01(void)
  :ConcreteBase(0, MAT_TAG_Concrete01,0.0,0.0,0.0),fpcu(0.0),
   parameterID(0), SHVs()
  {
    // Set trial values
    revertToLastCommit();
  }

//! @brief Assigns concrete compressive strength.
void XC::Concrete01::setFpcu(const double &d)
  {
    fpcu= d;
    if(fpcu > 0.0)
      {
        fpcu= -fpcu;
        std::clog << "Warning!, compressive strength must be negative." << std::endl;
      }
  }

//! @brief Returns concrete compressive strength.
double XC::Concrete01::getFpcu(void) const
  { return fpcu; }

//! @brief Destructor.
XC::Concrete01::~Concrete01(void)
  {}

//! @brief Calculate the trial state given the change in strain
void XC::Concrete01::calcula_trial_state(const double &dStrain)
  {
    trialHistory.UnloadSlope()= convergedHistory.getUnloadSlope(); //Reset unload slope.
  
    const double unloadStress= convergedState.getStress() + trialHistory.getUnloadSlope()*dStrain; //Stress when unloading.
  
    if(dStrain <= 0.0) // Material goes further into compression
      {
        trialHistory.MinStrain()= convergedHistory.getMinStrain(); //Reset min strain.
        trialHistory.EndStrain()= convergedHistory.getEndStrain(); //Reset end strain.
        reload();
        if(unloadStress > trialState.getStress() )
          {
            trialState.Stress()= unloadStress;
            trialState.Tangent()= trialHistory.getUnloadSlope();
          }
      }
    else if(unloadStress <= 0.0) // Material goes TOWARD tension (dStrain>0 and unloadStress<0.0)
      {
        trialState.Stress()= unloadStress;
        trialState.Tangent()= trialHistory.getUnloadSlope();
      }
    else // Made it into tension
      {
        trialState.Stress()= 0.0;
        trialState.Tangent()= 0.0;
      }
  }
//! @brief Sets the trial strain value.
int XC::Concrete01::setTrialStrain(double strain, double strainRate)
  {
    // Reset trial state variables to last committed state
    commit_to_trial_state();

    // Determine change in strain from last converged state
    const double dStrain= strain - convergedState.getStrain();

    if(fabs(dStrain) < DBL_EPSILON)
      return 0; //If the change is very small we do nothing.

    // Set trial strain
    trialState.Strain()= strain;
  
    // check for a quick return
    if(trialState.getStrain() > 0.0)
      {
        trialState.Stress()= 0;
        trialState.Tangent()= 0;
        return 0;
      }
  
    // Calculate the trial state given the change in strain
    // determineTrialState (dStrain);
    calcula_trial_state(dStrain);
    return 0;
  }

//@ brief Set trial values.
int XC::Concrete01::setTrial(double strain, double &stress, double &tangent, double strainRate)
  {
    // Reset trial state variables to last committed state
    commit_to_trial_state();

    // Determine change in strain from last converged state
    const double dStrain= strain - convergedState.getStrain();

    if(fabs(dStrain) < DBL_EPSILON)
      {
        stress= trialState.getStress();
        tangent= trialState.getTangent();
        return 0;
      }

    // Set trial strain
    trialState.Strain()= strain;
  
    // check for a quick return
    if(trialState.getStrain() > 0.0)
      {
        trialState.Stress()= 0;
        trialState.Tangent()= 0;
        stress= 0;
        tangent= 0;
        return 0;
      }
  
  
    // Calculate the trial state given the change in strain
    // determineTrialState (dStrain);
    calcula_trial_state(dStrain);
    stress= trialState.getStress();
    tangent=  trialState.getTangent();
    return 0;
  }

//! @brief ??
void XC::Concrete01::determineTrialState(double dStrain)
  {
    trialHistory= convergedHistory;

    const double tempStress= convergedState.getStress() + trialHistory.getUnloadSlope()*dStrain;

    // Material goes further into compression
    if(trialState.getStrain() <= convergedState.getStrain())
      {
        reload();
        if(tempStress > trialState.getStress() )
          {
            trialState.Stress()= tempStress;
            trialState.Tangent()= trialHistory.getUnloadSlope();
          }
      }
    else if(tempStress <= 0.0) // Material goes TOWARD tension
      {
        trialState.Stress()= tempStress;
        trialState.Tangent()= trialHistory.getUnloadSlope();
      }
    else // Made it into tension
      {
        trialState.Stress()= 0.0;
        trialState.Tangent()= 0.0;
      }
  }

//! @brief ??
void XC::Concrete01::reload(void)
  {
    if(trialState.getStrain() <= trialHistory.MinStrain())
      {
        trialHistory.MinStrain()= trialState.getStrain();
        // Determine point on envelope
        envelope();
        unload();
      }
    else if(trialState.getStrain() <= trialHistory.EndStrain())
      {
        trialState.Tangent()= trialHistory.getUnloadSlope();
        trialState.Stress()= trialState.getTangent()*(trialState.getStrain()-trialHistory.EndStrain());
      }
    else
      {
        trialState.Stress()= 0.0;
        trialState.Tangent()= 0.0;
      }
  }

//! @brief Determine point on envelope
void XC::Concrete01::envelope(void)
  {
     if(trialState.getStrain() > epsc0)
       {
         const double eta= trialState.getStrain()/epsc0;
         trialState.Stress()= fpc*(2*eta-eta*eta);
         const double Ec0= 2.0*fpc/epsc0;
         trialState.Tangent()= Ec0*(1.0-eta);
       }
     else if(trialState.getStrain() > epscu)
       {
         trialState.Tangent()= (fpc-fpcu)/(epsc0-epscu);
         trialState.Stress()= fpc + trialState.getTangent()*(trialState.getStrain()-epsc0);
       }
     else
       {
         trialState.Stress()= fpcu;
         trialState.Tangent()= 0.0;
       }
  }

//! @brief ??
void XC::Concrete01::unload(void)
  {
    double tempStrain= trialHistory.getMinStrain();
    if(tempStrain < epscu) tempStrain= epscu;

    const double eta= tempStrain/epsc0;
    double ratio= 0.707*(eta-2.0) + 0.834;

    if(eta < 2.0) ratio= 0.145*eta*eta + 0.13*eta;

    trialHistory.EndStrain()= ratio*epsc0;

    const double temp1= trialHistory.getMinStrain() - trialHistory.getEndStrain();
    const double Ec0= 2.0*fpc/epsc0;
    const double temp2= trialState.getStress()/Ec0;

    if(temp1 > -DBL_EPSILON) // temp1 should always be negative
      { trialHistory.UnloadSlope()= Ec0; }
    else if(temp1 <= temp2)
      {
        trialHistory.EndStrain()= trialHistory.getMinStrain() - temp1;
        trialHistory.UnloadSlope()= trialState.getStress() /temp1;
      }
    else
      {
        trialHistory.EndStrain()= trialHistory.getMinStrain() - temp2;
        trialHistory.UnloadSlope()= Ec0;
      }
  }

//! @brief Commits material state.
int XC::Concrete01::commitState(void)
  {
    convergedHistory= trialHistory;// History variables
    convergedState= trialState;// State variables
    return 0;
  }

//! @brief Returns to the last committed state.
int XC::Concrete01::revertToLastCommit(void)
  {
    commit_to_trial();
    return 0;
  }

//! @brief Returns the material to its initial state.
int XC::Concrete01::revertToStart(void)
  {
    int retval= ConcreteBase::revertToStart();
    const double Ec0= 2.0*fpc/epsc0;
    convergedHistory.revertToStart(Ec0); // History variables
    convergedState.revertToStart(Ec0);// State variables

    // Reset trial variables and state
    revertToLastCommit();

    return retval;
  }

//! @brief Returns a material copy.
XC::UniaxialMaterial* XC::Concrete01::getCopy(void) const
  { return new Concrete01(*this); }

//! @brief Send object members through the communicator argument.
int XC::Concrete01::sendData(Communicator &comm)
  {
    int res= ConcreteBase::sendData(comm);
    const double PI= parameterID;
    res+= comm.sendDoubles(fpcu,PI,getDbTagData(),CommMetaData(7));
    res+= comm.sendMatrix(SHVs,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::Concrete01::recvData(const Communicator &comm)
  {
    int res= ConcreteBase::recvData(comm);
    double PI;
    res+= comm.receiveDoubles(fpcu,PI,getDbTagData(),CommMetaData(7));
    parameterID= PI;
    res+= comm.receiveMatrix(SHVs,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::Concrete01::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(12);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::Concrete01::recvSelf(const Communicator &comm)
  {
    inicComm(12);
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

//! @brief Print stuff.
void XC::Concrete01::Print(std::ostream& s, int flag) const
  {
    s << "Concrete01, tag: " << this->getTag() << std::endl;
    s << "  fpc: " << fpc << std::endl;
    s << "  epsc0: " << epsc0 << std::endl;
    s << "  fpcu: " << fpcu << std::endl;
    s << "  epscu: " << epscu << std::endl;
  }


// AddingSensitivity:BEGIN ///////////////////////////////////

int XC::Concrete01::setParameter(const std::vector<std::string> &argv, Parameter &param)
  {
    if(argv[0] == "fc")
      { return param.addObject(1, this); } // Compressive strength
    else if(argv[0] == "epsco")
      { return param.addObject(2, this); } // Strain at compressive strength
    else if(argv[0] == "fcu")
      { return param.addObject(3, this); } // Crushing strength
    else if(argv[0] == "epscu")
      { return param.addObject(4, this); } // Strain at crushing strength
    else
      {
        std::cerr << "WARNING: Could not set parameter in Concrete01! "
                  << std::endl;
        return -1;
      }
  }



int XC::Concrete01::updateParameter(int parameterID, Information &info)
  {
    switch (parameterID)
      {
      case 1:
        this->fpc= info.theDouble;
        break;
      case 2:
        this->epsc0= info.theDouble;
        break;
      case 3:
        this->fpcu= info.theDouble;
        break;
      case 4:
        this->epscu= info.theDouble;
        break;
      default:
        break;
      }

    // Make all concrete parameters negative
    make_negative();
    setup_parameters();

    return 0;
  }


int XC::Concrete01::activateParameter(int passedParameterID)
  {
    parameterID= passedParameterID;
    return 0;
  }

double XC::Concrete01::getStressSensitivity(int gradNumber, bool conditional)
  {
    // Initialize return value
    UniaxialStateVars trialStateSensitivity;
    double dktdh= 0.0;

    // Pick up sensitivity history variables
    UniaxialHistoryVars convergedHistorySensitivity;
    UniaxialStateVars convergedStateSensitivity;
    if(!SHVs.isEmpty())
      {
        convergedHistorySensitivity.MinStrain()= SHVs(0,(gradNumber-1));
        convergedHistorySensitivity.UnloadSlope()= SHVs(1,(gradNumber-1));
        convergedHistorySensitivity.EndStrain()= SHVs(2,(gradNumber-1));
        convergedStateSensitivity.Stress()= SHVs(3,(gradNumber-1));
        convergedStateSensitivity.Strain()= SHVs(4,(gradNumber-1));
      }

    // Assign values to parameter derivatives (depending on what's random)
    double fpcSensitivity= 0.0;
    double epsc0Sensitivity= 0.0;
    double fpcuSensitivity= 0.0;
    double epscuSensitivity= 0.0;

    if(parameterID == 1)
      { fpcSensitivity= 1.0; }
    else if(parameterID == 2)
      { epsc0Sensitivity= 1.0; }
    else if(parameterID == 3)
      { fpcuSensitivity= 1.0; }
    else if(parameterID == 4)
      { epscuSensitivity= 1.0; }


    // Strain increment
    const double dStrain= trialState.getStrain() - convergedState.getStrain();

    // Evaluate stress sensitivity
    if(dStrain < 0.0)
      { // applying more compression to the material
        if(trialState.getStrain() < convergedHistory.MinStrain())
          { // loading along the backbone curve
            if(trialState.getStrain() > epsc0)
              { //on the parabola

                trialStateSensitivity.Stress()= fpcSensitivity*(2.0*trialState.getStrain() /epsc0-(trialState.getStrain() /epsc0)*(trialState.getStrain() /epsc0))
                 + fpc*( (2.0*trialStateSensitivity.Strain()*epsc0-2.0*trialState.getStrain() *epsc0Sensitivity)/(epsc0*epsc0)
			 - 2.0*(trialState.getStrain() /epsc0)*(trialStateSensitivity.getStrain()*epsc0-trialState.getStrain() *epsc0Sensitivity)/(epsc0*epsc0));
                 dktdh= 2.0*((fpcSensitivity*epsc0-fpc*epsc0Sensitivity)/(epsc0*epsc0))
                 * (1.0-trialState.getStrain() /epsc0)
                 - 2.0*(fpc/epsc0)*(trialStateSensitivity.getStrain()*epsc0-trialState.getStrain() *epsc0Sensitivity) / (epsc0*epsc0);
              }
            else if(trialState.getStrain() > epscu)
              {                // on the straight inclined line
//cerr << "ON THE STRAIGHT INCLINED LINE" << endl;
                dktdh= ( (fpcSensitivity-fpcuSensitivity)
                         * (epsc0-epscu)
                         - (fpc-fpcu)
                         * (epsc0Sensitivity-epscuSensitivity) )
                         / ((epsc0-epscu)*(epsc0-epscu));

                const double kt= (fpc-fpcu)/(epsc0-epscu);
                trialStateSensitivity.Stress()= fpcSensitivity + dktdh*(trialState.getStrain() -epsc0)
                                     + kt*(trialStateSensitivity.getStrain()-epsc0Sensitivity);
              }
            else
              { // on the horizontal line
//cerr << "ON THE HORIZONTAL LINES" << endl;
                trialStateSensitivity.Stress()= fpcuSensitivity;
                dktdh= 0.0;
              }
         }
       else if(trialState.getStrain() < convergedHistory.getEndStrain())
         {        // reloading after an unloading that didn't go all the way to zero stress
//cerr << "RELOADING AFTER AN UNLOADING THAT DIDN'T GO ALL THE WAY DOWN" << endl;
           trialStateSensitivity.Stress()= convergedHistorySensitivity.UnloadSlope() * (trialState.getStrain() -convergedHistory.getEndStrain())
	     + convergedHistory.getUnloadSlope() * (trialStateSensitivity.getStrain()-convergedHistorySensitivity.getEndStrain());
           dktdh= convergedHistorySensitivity.getUnloadSlope();
         }
       else
         {
           trialStateSensitivity.Stress()= 0.0;
           dktdh= 0.0;
         }
     }
   else if(convergedState.getStress()+convergedHistory.getUnloadSlope()*dStrain<0.0)
     {// unloading, but not all the way down to zero stress
//cerr << "UNLOADING, BUT NOT ALL THE WAY DOWN" << endl;
       trialStateSensitivity.Stress()= convergedStateSensitivity.getStress()
                            + convergedHistorySensitivity.getUnloadSlope()*dStrain
	 + convergedHistory.getUnloadSlope()*(trialStateSensitivity.getStrain()-convergedStateSensitivity.getStrain());
       dktdh= convergedHistorySensitivity.getUnloadSlope();
     }
   else
     { // unloading all the way down to zero stress
//cerr << "UNLOADING ALL THE WAY DOWN" << endl;
       trialStateSensitivity.Stress()= 0.0;
       dktdh= 0.0;
     }
   return trialStateSensitivity.Stress();
  }



int XC::Concrete01::commitSensitivity(const double &strainGradient, int gradNumber, int numGrads)
  {
    // Initialize unconditaional stress sensitivity
    UniaxialStateVars trialStateSensitivity;
    trialStateSensitivity.Strain()= strainGradient;
    double dktdh= 0.0;

    // Assign values to parameter derivatives (depending on what's random)
    double fpcSensitivity= 0.0;
    double epsc0Sensitivity= 0.0;
    double fpcuSensitivity= 0.0;
    double epscuSensitivity= 0.0;

    if(parameterID == 1)
      { fpcSensitivity= 1.0; }
    else if(parameterID == 2)
      { epsc0Sensitivity= 1.0; }
    else if(parameterID == 3)
      { fpcuSensitivity= 1.0; }
    else if(parameterID == 4)
      { epscuSensitivity= 1.0; }

    // Pick up sensitivity history variables
    UniaxialHistoryVars convergedHistorySensitivity;
    UniaxialStateVars convergedStateSensitivity;

    if(SHVs.isEmpty())
      {
        SHVs= Matrix(5,numGrads);
        convergedHistorySensitivity.UnloadSlope()= (2.0*fpcSensitivity*epsc0-2.0*fpc*epsc0Sensitivity) / (epsc0*epsc0);
      }
    else
      {
        convergedHistorySensitivity.MinStrain()= SHVs(0,(gradNumber-1));
        convergedHistorySensitivity.UnloadSlope()= SHVs(1,(gradNumber-1));
        convergedHistorySensitivity.EndStrain()= SHVs(2,(gradNumber-1));
        convergedStateSensitivity.Stress()= SHVs(3,(gradNumber-1));
        convergedStateSensitivity.Strain()= SHVs(4,(gradNumber-1));
      }

    // Strain increment
    const double dStrain= trialState.getStrain() - convergedState.getStrain();

    // Evaluate stress sensitivity
    if(dStrain < 0.0)
      { // applying more compression to the material
        if(trialState.getStrain() < convergedHistory.MinStrain())
          { // loading along the backbone curve
            if(trialState.getStrain() > epsc0)
              { //on the parabola
                trialStateSensitivity.Stress()= fpcSensitivity*(2.0*trialState.getStrain() /epsc0-(trialState.getStrain() /epsc0)*(trialState.getStrain() /epsc0))
                                     + fpc*( (2.0*trialStateSensitivity.Strain()*epsc0-2.0*trialState.getStrain() *epsc0Sensitivity)/(epsc0*epsc0)
                                     - 2.0*(trialState.getStrain() /epsc0)*(trialStateSensitivity.Strain()*epsc0-trialState.getStrain() *epsc0Sensitivity)/(epsc0*epsc0));

                dktdh= 2.0*((fpcSensitivity*epsc0-fpc*epsc0Sensitivity)/(epsc0*epsc0))
                           * (1.0-trialState.getStrain() /epsc0)
                      - 2.0*(fpc/epsc0)*(trialStateSensitivity.Strain()*epsc0-trialState.getStrain() *epsc0Sensitivity)
                        / (epsc0*epsc0);
              }
            else if(trialState.getStrain() > epscu)
              { // on the straight inclined line

                dktdh= ( (fpcSensitivity-fpcuSensitivity) * (epsc0-epscu)
                        - (fpc-fpcu)
                        * (epsc0Sensitivity-epscuSensitivity) )
                        / ((epsc0-epscu)*(epsc0-epscu));
                const double kt= (fpc-fpcu)/(epsc0-epscu);
                trialStateSensitivity.Stress()= fpcSensitivity
                                     + dktdh*(trialState.getStrain() -epsc0)
                                     + kt*(trialStateSensitivity.Strain()-epsc0Sensitivity);
              }
            else
              { // on the horizontal line
                trialStateSensitivity.Stress()= fpcuSensitivity;
                dktdh= 0.0;
              }
          }
        else if(trialState.getStrain() < convergedHistory.getEndStrain())
          { // reloading after an unloading that didn't go all the way to zero stress
            trialStateSensitivity.Stress()= convergedHistorySensitivity.getUnloadSlope() * (trialState.getStrain() -convergedHistory.getEndStrain())
                                 + convergedHistory.getUnloadSlope() * (trialStateSensitivity.Strain()-convergedHistorySensitivity.EndStrain());
            dktdh= convergedHistorySensitivity.getUnloadSlope();
          }
        else
          {
            trialStateSensitivity.Stress()= 0.0;
            dktdh= 0.0;
          }
      }
    else if(convergedState.getStress()+convergedHistory.getUnloadSlope()*dStrain<0.0)
      {// unloading, but not all the way down to zero stress
	trialStateSensitivity.Stress()= convergedStateSensitivity.getStress()
	  + convergedHistorySensitivity.UnloadSlope()*dStrain
	  + convergedHistory.getUnloadSlope()*(trialStateSensitivity.getStrain()-convergedState.getStrain());
	dktdh= convergedHistorySensitivity.UnloadSlope();
      }
    else
      {// unloading all the way down to zero stress
        trialStateSensitivity.Stress()= 0.0;
        dktdh= 0.0;
      }

    // Commit some history variables
    SHVs(3,(gradNumber-1))= trialStateSensitivity.getStress();
    SHVs(4,(gradNumber-1))= trialStateSensitivity.getStrain();

    // Possibly update history variables for the three ordinary history variable derivatives
    double epsTemp, epsTempSensitivity;
    double eta, etaSensitivity;
    double ratio, ratioSensitivity;
    double temp1, temp1Sensitivity;
    double temp2, temp2Sensitivity;
    UniaxialHistoryVars trialHistorySensitivity;

    if(dStrain<0.0 && trialState.getStrain() <convergedHistory.MinStrain())
      {
        trialHistorySensitivity.MinStrain()= trialStateSensitivity.Strain();
        if(trialState.getStrain() < epscu)
          {
            epsTemp= epscu;
            epsTempSensitivity= epscuSensitivity;
          }
        else
          {
            epsTemp= trialState.getStrain() ;
            epsTempSensitivity= trialStateSensitivity.Strain();
          }
        eta= epsTemp/epsc0;
        etaSensitivity= (epsTempSensitivity*epsc0-epsTemp*epsc0Sensitivity) / (epsc0*epsc0);
        if(eta < 2.0)
          {
            ratio= 0.145 * eta*eta + 0.13*eta;
            ratioSensitivity= 0.29 * eta * etaSensitivity + 0.13 * etaSensitivity;
          }
        else
          {
            ratio= 0.707*(eta-2.0) + 0.834;
            ratioSensitivity= 0.707 * etaSensitivity;
          }
        temp1= trialState.getStrain() - ratio * epsc0;
        temp1Sensitivity= trialStateSensitivity.Strain() - ratioSensitivity * epsc0 - ratio * epsc0Sensitivity;
        temp2= trialState.getStress()  * epsc0 / (2.0*fpc);
        temp2Sensitivity= (2.0*fpc*(trialStateSensitivity.Stress()*epsc0+trialState.getStress() *epsc0Sensitivity)
                        -2.0*trialState.getStress() *epsc0*fpcSensitivity) / (4.0*fpc*fpc);
        if(temp1 == 0.0)
          {
            trialHistorySensitivity.UnloadSlope()= (2.0*fpcSensitivity*epsc0-2.0*fpc*epsc0Sensitivity) / (epsc0*epsc0);
          }
        else if(temp1 < temp2)
          {
            trialHistorySensitivity.EndStrain()= trialStateSensitivity.Strain() - temp1Sensitivity;
            trialHistorySensitivity.UnloadSlope()= (trialStateSensitivity.Stress()*temp1-trialState.getStress()*temp1Sensitivity) / (temp1*temp1);
          }
        else
          {
            trialHistorySensitivity.EndStrain()= trialStateSensitivity.Strain() - temp2Sensitivity;
            trialHistorySensitivity.UnloadSlope()= (2.0*fpcSensitivity*epsc0-2.0*fpc*epsc0Sensitivity) / (epsc0*epsc0);
          }
      }
    else
      { trialHistorySensitivity= convergedHistorySensitivity; }
    SHVs(0,(gradNumber-1))= trialHistorySensitivity.getMinStrain();
    SHVs(1,(gradNumber-1))= trialHistorySensitivity.getUnloadSlope();
    SHVs(2,(gradNumber-1))= trialHistorySensitivity.getEndStrain();
    return 0;
  }

/* THE OLD METHODS:
double XC::Concrete01::getStressSensitivity(int gradNumber, bool conditional)
{
        // (This method only works for path-independent problems for now.)

        double gradient= 0.0;

        if( parameterID == 0 ) {
                // Leave the gradient as zero if nothing is random here;
        }
        else if(trialState.getStrain() > 0.0 ) {
                gradient= 0.0;
        }
        else if(trialState.getStrain() > epsc0) {                                        // IN PARABOLIC AREA

                if( parameterID == 1 ) {                // d{sigma}d{fpc}
                        gradient= 2.0*trialState.getStrain() /epsc0-trialState.getStrain() *trialState.getStrain() /(epsc0*epsc0);
                }
                else if( parameterID == 2  ) {        // d{sigma}d{epsc0}
                        gradient= 2.0*fpc/(epsc0*epsc0)*(trialState.getStrain() *trialState.getStrain() /epsc0-trialState.getStrain() );
                }
                else if( parameterID == 3  ) {        // d{sigma}d{fpcu}
                        gradient= 0.0;
                }
                else if( parameterID == 4  ) {        // d{sigma}d{epscu}
                        gradient= 0.0;
                }
                else {
                        gradient= 0.0;
                }
        }
        else if(trialState.getStrain() > epscu) {                                        // IN LINEAR AREA

                if( parameterID == 1 ) {                // d{sigma}d{fpc}
                        gradient= (epscu-trialState.getStrain() )/(epscu-epsc0);
                }
                else if( parameterID == 2  ) {        // d{sigma}d{epsc0}
                        gradient= (fpc-fpcu)*(epscu-trialState.getStrain() )/((epscu-epsc0)*(epscu-epsc0));
                }
                else if( parameterID == 3  ) {        // d{sigma}d{fpcu}
                        gradient= (trialState.getStrain() -epsc0)/(epscu-epsc0);
                }
                else if( parameterID == 4  ) {        // d{sigma}d{epscu}
                        gradient= (trialState.getStrain() -epsc0)*(fpc-fpcu)/((epsc0-epscu)*(epsc0-epscu));
                }
                else {
                        gradient= 0.0;
                }
        }
        else {                                                                                // IN ZERO STIFFNESS AREA

                if( parameterID == 1 ) {                // d{sigma}d{fpc}
                        gradient= 0.0;
                }
                else if( parameterID == 2  ) {        // d{sigma}d{epsc0}
                        gradient= 0.0;
                }
                else if( parameterID == 3  ) {        // d{sigma}d{fpcu}
                        gradient= 1.0;
                }
                else if( parameterID == 4  ) {        // d{sigma}d{epscu}
                        gradient= 0.0;
                }
                else {
                        gradient= 0.0;
                }
        }

        return gradient;
}



int XC::Concrete01::commitSensitivity(double TstrnSens, int gradNumber, int numGrads)
  {
    // Not treated yet.
    return 0;
  }
*/
// AddingSensitivity:END /////////////////////////////////////////////
