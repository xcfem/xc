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
                                                                        
// $Revision: 1.10 $
// $Date: 2004/11/24 00:48:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ParallelMaterial.cpp,v $
                                                                        
                                                                        
// File: ~/material/ParallelModel.C
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class definition for 
// ParallelModel. ParallelModel is an aggregation
// of XC::UniaxialMaterial objects all considered acting in parallel.
//
// What: "@(#) ParallelModel.C, revA"

#include "ParallelMaterial.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <cstdlib>
#include <utility/recorder/response/MaterialResponse.h>
#include "boost/lexical_cast.hpp"

XC::ParallelMaterial::ParallelMaterial(int tag, const XC::DqUniaxialMaterial &theMaterialModels)
  :ConnectedMaterial(tag,MAT_TAG_ParallelMaterial, theMaterialModels), trialStrain(0.0), trialStrainRate(0.0)
  {}

XC::ParallelMaterial::ParallelMaterial(int tag)
  :ConnectedMaterial(tag,MAT_TAG_ParallelMaterial), trialStrain(0.0), trialStrainRate(0.0)
  {}

// this constructor is used for a ParallelMaterailModel object that
// needs to be constructed in a remote actor process. recvSelf() needs
// to be called on this object
XC::ParallelMaterial::ParallelMaterial(void)
  :ConnectedMaterial(0,MAT_TAG_ParallelMaterial), trialStrain(0.0), trialStrainRate(0.0)
  {}


int XC::ParallelMaterial::setTrialStrain(double strain, double strainRate)
  {
    // set the trialStrain and the trialStrain in each of the
    // local MaterialModel objects 
    trialStrain = strain;
    trialStrainRate = strainRate;
    return theModels.setTrialStrain(strain, strainRate);
  }


double XC::ParallelMaterial::getStrain(void) const
  { return trialStrain; }

double XC::ParallelMaterial::getStrainRate(void) const
  { return trialStrainRate; }

double XC::ParallelMaterial::getStress(void) const
  {
    // get the stress = sum of stress in all local MaterialModel objects
    double stress= 0.0;
    const size_t numMaterials= theModels.size();
    for(size_t i=0; i<numMaterials; i++)
      stress +=theModels[i]->getStress();
    return stress;
  }

//! @brief Return the material tangent stiffness.
double XC::ParallelMaterial::getTangent(void) const
  {
    // get the tangent = sum of tangents in all local MaterialModel objects    
    double E = 0.0;
    const size_t numMaterials= theModels.size();
    for(size_t i=0; i<numMaterials; i++)
      E +=theModels[i]->getTangent();    
    return E;
  }

//! @brief Return the material initial stiffness.
double XC::ParallelMaterial::getInitialTangent(void) const
  {
    // get the tangent = sum of tangents in all local MaterialModel objects    
    double E = 0.0;
    const size_t numMaterials= theModels.size();
    for(size_t i=0; i<numMaterials; i++)
      E +=theModels[i]->getInitialTangent();    
    return E;
  }

double XC::ParallelMaterial::getDampTangent(void) const
  {
    // get the damp tangent = sum of damp tangents in all local MaterialModel objects    
    double eta = 0.0;
    const size_t numMaterials= theModels.size();
    for(size_t i=0; i<numMaterials; i++)
      eta +=theModels[i]->getDampTangent();    
    return eta;
  }

//! @brief Commit the state of the material.
int XC::ParallelMaterial::commitState(void)
  {

    // invoke commitState() on each of local MaterialModel objects
    return theModels.commitState();
  }

int XC::ParallelMaterial::revertToLastCommit(void)
  {
    // invoke commitState() on each of local MaterialModel objects
    return theModels.revertToLastCommit();
  }

//! @brief Revert the material to its initial state.
int XC::ParallelMaterial::revertToStart(void)
  {
    int retval= ConnectedMaterial::revertToStart();
    trialStrain = 0.0;
    trialStrainRate = 0.0;
    retval+= theModels.revertToStart();
    return retval;
  }


XC::UniaxialMaterial * XC::ParallelMaterial::getCopy(void) const
  { return new ParallelMaterial(*this); }

//! @brief Send its members through the communicator argument.
int XC::ParallelMaterial::sendData(Communicator &comm)
  {
    int res= ConnectedMaterial::sendData(comm);
    res+= comm.sendDoubles(trialStrain,trialStrainRate,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Receives its members through the communicator argument.
int XC::ParallelMaterial::recvData(const Communicator &comm)
  {
    int res= ConnectedMaterial::recvData(comm);
    res+= comm.receiveDoubles(trialStrain,trialStrainRate,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::ParallelMaterial::sendSelf(Communicator &comm)
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
int XC::ParallelMaterial::recvSelf(const Communicator &comm)
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

void XC::ParallelMaterial::Print(std::ostream &s, int flag) const
  {
    s << "Parallel tag: " << this->getTag() << std::endl;
    for(size_t i=0; i<theModels.size(); i++)
      {
        s << " ";
        theModels[i]->Print(s, flag);
      }
  }

XC::Response *XC::ParallelMaterial::setResponse(const std::vector<std::string> &argv,Information &info)
  {
    // See if the response is one of the defaults
    Response *res= ConnectedMaterial::setResponse(argv, info);
    if(res != 0) return res;

    const size_t numMaterials= theModels.size();
    const size_t argc= argv.size();
    if(argv[0] == "stresses")
      return new MaterialResponse(this, 100, XC::Vector(numMaterials));
    else if(argv[0] == "material" || argv[0] == "component")
      {
        if(argc > 1)
          {
            size_t matNum = boost::lexical_cast<size_t>(argv[1]) - 1;
            if(matNum >= 0 && matNum < numMaterials)
              {
                std::vector<std::string> argvMat= argv;
                argvMat.erase(argvMat.begin());
                argvMat.erase(argvMat.begin());
                return theModels[matNum]->setResponse(argvMat, info);
              }
            else
              return 0;
          }
        else
          return 0;
      }
    else
      return this->ConnectedMaterial::setResponse(argv, info);
  }

int XC::ParallelMaterial::getResponse(int responseID, Information &info)
  {
    const size_t numMaterials= theModels.size();
    XC::Vector stresses(numMaterials);
    switch (responseID)
      {
      case 100:
        for(size_t i = 0; i < numMaterials; i++)
          stresses(i) = theModels[i]->getStress();
        return info.setVector(stresses);
      default:
        return ConnectedMaterial::getResponse(responseID, info);
      }
  }
