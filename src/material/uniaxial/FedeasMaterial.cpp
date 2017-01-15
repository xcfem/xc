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
                                                                        
// $Revision: 1.20 $
// $Date: 2006/01/03 23:52:45 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/FedeasMaterial.cpp,v $
                                                                        
// Written: MHS
// Created: Jan 2001
//
// Description: This file contains the class definition for 
// FedeasMaterial. FedeasMaterial provides a FORTRAN interface
// for programming uniaxial material models, using the subroutine
// interface from the FEDEAS ML1D library, developed by F.C. Filippou.
//
// For more information visit the FEDEAS web page:
//    http://www.ce.berkeley.edu/~filippou/Research/fedeas.htm

#include <material/uniaxial/FedeasMaterial.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/ID.h>

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include "utility/actor/actor/ArrayCommMetaData.h"

XC::FedeasMaterial::FedeasMaterial(int tag, int classTag, int nhv, int ndata)
  :UniaxialMaterial(tag,classTag),
   data(nullptr), hstv(nullptr), numData(ndata), numHstv(nhv), converged(), trial()
  {
    if(numHstv < 0)
      numHstv = 0;
  
    if(numHstv > 0) {
    // Allocate history array
    hstv = new double[2*numHstv];
    if (hstv == 0) {
      std::cerr << "XC::FedeasMaterial::FedeasMaterial -- failed to allocate history array -- type " << 
        this->getClassTag() << std::endl;
      exit(-1);
    }

    // Initialize to zero
    for (int i = 0; i < 2*numHstv; i++)
      hstv[i] = 0.0;
  }
  
  if (numData < 0)
    numData = 0;
  
  if (numData > 0) {
    // Allocate material parameter array
    data = new double[numData];
    if (data == 0) {
      std::cerr << "XC::FedeasMaterial::FedeasMaterial -- failed to allocate data array -- type : " <<
        this->getClassTag() << std::endl;
      exit(-1);
    }
                            
    // Initialize to zero
    for (int i = 0; i < numData; i++)
      data[i] = 0.0;
  }
}

XC::FedeasMaterial::~FedeasMaterial(void)
  {
    if(hstv)
      delete [] hstv;
    hstv= nullptr;
    if(data)
      delete [] data;
    data= nullptr;
  }

int XC::FedeasMaterial::setTrialStrain(double strain, double strainRate)
  {
    if(fabs(strain-trial.getStrain()) > DBL_EPSILON)
      {
        // Store the strain
        trial.Strain()= strain;
    
        // Tells subroutine to do normal operations for stress and tangent
        int ist = 1;
    
        // Call the subroutine
        return this->invokeSubroutine(ist);
      }
    return 0;
  }

int XC::FedeasMaterial::setTrial(double strain, double &stress, double &stiff, double strainRate)
  {
    int res = 0;
    if(fabs(strain-trial.getStrain()) > DBL_EPSILON)
      {

        // Store the strain
        trial.Strain()= strain;
    
        // Tells subroutine to do normal operations for stress and tangent
        int ist = 1;
    
        // Call the subroutine
        res = this->invokeSubroutine(ist);
      }
    stress = trial.getStress();
    stiff = trial.getTangent();
    return res;
  }

double XC::FedeasMaterial::getStrain(void) const
  { return trial.getStrain(); }

double XC::FedeasMaterial::getStress(void) const
  { return trial.getStress(); }

double XC::FedeasMaterial::getTangent(void) const
  { return trial.getTangent(); }

int XC::FedeasMaterial::commitState(void)
  {
    // Set committed values equal to corresponding trial values
    for(int i = 0; i < numHstv; i++)
      hstv[i] = hstv[i+numHstv];
    converged= trial;
    return 0;
  }

int XC::FedeasMaterial::revertToLastCommit(void)
  {
    // Set trial values equal to corresponding committed values
    for(int i = 0; i < numHstv; i++)
      hstv[i+numHstv] = hstv[i];  
    trial= converged;
    return 0;
  }

int XC::FedeasMaterial::revertToStart(void)
  {
    // Set all trial and committed values to zero
    for(int i = 0; i < 2*numHstv; i++)
      hstv[i] = 0.0;
  
    converged.revertToStart(this->getInitialTangent());
    trial.revertToStart(this->getInitialTangent());
    return 0;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::FedeasMaterial::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    setDbTagDataPos(4,numData);
    res+= cp.sendDoublePtr(data,getDbTagData(),ArrayCommMetaData(2,3,4));
    setDbTagDataPos(7,numHstv);
    res+= cp.sendDoublePtr(hstv,getDbTagData(),ArrayCommMetaData(5,6,7));
    res+= cp.sendMovable(converged,getDbTagData(),CommMetaData(8));
    res+= cp.sendMovable(trial,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::FedeasMaterial::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    numData= getDbTagDataPos(4);
    data= cp.receiveDoublePtr(data,getDbTagData(),ArrayCommMetaData(2,3,4));
    numHstv= getDbTagDataPos(7);
    data= cp.receiveDoublePtr(hstv,getDbTagData(),ArrayCommMetaData(5,6,7));
    res+= cp.receiveMovable(converged,getDbTagData(),CommMetaData(8));
    res+= cp.receiveMovable(trial,getDbTagData(),CommMetaData(9));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::FedeasMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(10); 
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "FedeasMaterial::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::FedeasMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(10);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "FedeasMaterial::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
           std::cerr << "FedeasMaterial::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::FedeasMaterial::Print(std::ostream &s, int flag)
{
  s << "FedeasMaterial, type: ";
        
  switch (this->getClassTag()) {
  case MAT_TAG_FedeasHardening:
    s << "Hardening" << std::endl;
    break;
  case MAT_TAG_FedeasBond1:
    s << "Bond1" << std::endl;
    break;
  case MAT_TAG_FedeasBond2:
    s << "Bond2" << std::endl;
    break;
  case MAT_TAG_FedeasConcrete1:
    s << "Concrete1" << std::endl;
    break;
  case MAT_TAG_FedeasConcrete2:
    s << "Concrete2" << std::endl;
    break;
  case MAT_TAG_FedeasConcrete3:
    s << "Concrete3" << std::endl;
    break;
  case MAT_TAG_FedeasHysteretic1:
    s << "Hysteretic1" << std::endl;
    break;
  case MAT_TAG_FedeasHysteretic2:
    s << "Hysteretic2" << std::endl;
    break;
  case MAT_TAG_FedeasSteel1:
    s << "Steel1" << std::endl;
    break;
  case MAT_TAG_FedeasSteel2:
    s << "Steel2" << std::endl;
    break;
    // Add more cases as needed
    
  default:
    s << "Material identifier = " << this->getClassTag() << std::endl;
    break;
  }
}


#ifdef _pgCC
#define hard_1__ hard1_1_
#endif



#ifdef _WIN32

extern "C" int BOND_1(double *matpar, double *hstvP, double *hstv,
                               double *strainP, double *stressP, double *dStrain,
                               double *tangent, double *stress, int *ist);

extern "C" int BOND_2(double *matpar, double *hstvP, double *hstv,
                               double *strainP, double *stressP, double *dStrain,
                               double *tangent, double *stress, int *ist);

extern "C" int CONCRETE_1(double *matpar, double *hstvP, double *hstv,
                                   double *strainP, double *stressP, double *dStrain,
                                   double *tangent, double *stress, int *ist);

extern "C" int CONCRETE_2(double *matpar, double *hstvP, double *hstv,
                                   double *strainP, double *stressP, double *dStrain,
                                   double *tangent, double *stress, int *ist);

extern "C" int CONCRETE_3(double *matpar, double *hstvP, double *hstv,
                                   double *strainP, double *stressP, double *dStrain,
                                   double *tangent, double *stress, int *ist);

extern "C" int HARD_1(double *matpar, double *hstvP, double *hstv,
                               double *strainP, double *stressP, double *dStrain,
                               double *tangent, double *stress, int *ist);

extern "C" int HYSTER_1(double *matpar, double *hstvP, double *hstv,
                                 double *strainP, double *stressP, double *dStrain,
                                 double *tangent, double *stress, int *ist);

extern "C" int HYSTER_2(double *matpar, double *hstvP, double *hstv,
                                 double *strainP, double *stressP, double *dStrain,
                                 double *tangent, double *stress, int *ist);

extern "C" int STEEL_1(double *matpar, double *hstvP, double *hstv,
                                double *strainP, double *stressP, double *dStrain,
                                double *tangent, double *stress, int *ist);

extern "C" int  STEEL_2(double *matpar, double *hstvP, double *hstv,
                                double *strainP, double *stressP, double *dStrain,
                                double *tangent, double *stress, int *ist);

// Add more declarations as needed

#define bond_1__        BOND_1
#define bond_2__        BOND_2
#define concrete_1__        CONCRETE_1
#define concrete_2__        CONCRETE_2
#define concrete_3__        CONCRETE_3
#define hard_1__         HARD_1
#define hyster_1__        HYSTER_1
#define hyster_2__        HYSTER_2
#define steel_1__        STEEL_1
#define steel_2__        STEEL_2

#else

extern "C" int bond_1__(double *matpar, double *hstvP, double *hstv,
                        double *strainP, double *stressP, double *dStrain,
                        double *tangent, double *stress, int *ist);

extern "C" int bond_2__(double *matpar, double *hstvP, double *hstv,
                        double *strainP, double *stressP, double *dStrain,
                        double *tangent, double *stress, int *ist);

extern "C" int concrete_1__(double *matpar, double *hstvP, double *hstv,
                            double *strainP, double *stressP, double *dStrain,
                            double *tangent, double *stress, int *ist);

extern "C" int concrete_2__(double *matpar, double *hstvP, double *hstv,
                            double *strainP, double *stressP, double *dStrain,
                            double *tangent, double *stress, int *ist);

extern "C" int concrete_3__(double *matpar, double *hstvP, double *hstv,
                            double *strainP, double *stressP, double *dStrain,
                            double *tangent, double *stress, int *ist);

extern "C" int hard_1__(double *matpar, double *hstvP, double *hstv,
                        double *strainP, double *stressP, double *dStrain,
                        double *tangent, double *stress, int *ist);

extern "C" int hyster_1__(double *matpar, double *hstvP, double *hstv,
                          double *strainP, double *stressP, double *dStrain,
                          double *tangent, double *stress, int *ist);

extern "C" int hyster_2__(double *matpar, double *hstvP, double *hstv,
                          double *strainP, double *stressP, double *dStrain,
                          double *tangent, double *stress, int *ist);

extern "C" int steel_1__(double *matpar, double *hstvP, double *hstv,
                         double *strainP, double *stressP, double *dStrain,
                         double *tangent, double *stress, int *ist);

extern "C" int steel_2__(double *matpar, double *hstvP, double *hstv,
                         double *strainP, double *stressP, double *dStrain,
                         double *tangent, double *stress, int *ist);

// Add more declarations as needed

#endif


int XC::FedeasMaterial::invokeSubroutine(int ist)
  {
    std::cerr << "Este material sólo funciona en Windows." << std::endl;
    return 0;
  }

