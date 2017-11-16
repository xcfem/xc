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
                                                                        
// $Revision: 1.7 $
// $Date: 2004/11/24 00:48:29 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/SeriesMaterial.cpp,v $

// Written: MHS
// Created: Sept 2000
//
// Description: This file contains the class definition for 
// SeriesModel. SeriesModel is an aggregation
// of XC::UniaxialMaterial objects all considered acting in Series.
// Uses the same state determination as the beam elements.
// b = [1 1 ... 1]^

#include "SeriesMaterial.h"
#include <utility/matrix/ID.h>
#include <utility/matrix/Vector.h>
#include <cstdlib>
#include <utility/recorder/response/MaterialResponse.h>
#include "boost/lexical_cast.hpp"

XC::SeriesMaterial::SeriesMaterial(int tag,const DqUniaxialMaterial &theMaterialModels,int maxIter, double tol)
  :ConnectedMaterial(tag,MAT_TAG_SeriesMaterial,theMaterialModels),
   Tstrain(0.0), Cstrain(0.0), Tstress(0.0), Cstress(0.0),
   Ttangent(0.0), Ctangent(0.0), maxIterations(maxIter), tolerance(tol),
   stress(theMaterialModels.size()), flex(theMaterialModels.size()), strain(theMaterialModels.size()), initialFlag(false)
  {
    const size_t numMaterials= theMaterialModels.size();
    for(register size_t i = 0; i < numMaterials; i++)
      {
        strain[i] = 0.0;
        stress[i] = 0.0;
        flex[i] = 0.0;
      }
  }

XC::SeriesMaterial::SeriesMaterial(int tag)
  :ConnectedMaterial(tag,MAT_TAG_SeriesMaterial),
   Tstrain(0.0), Cstrain(0.0), Tstress(0.0), Cstress(0.0),
   Ttangent(0.0), Ctangent(0.0), 
   maxIterations(0), tolerance(0.0),
   stress(0), flex(0), strain(0), initialFlag(false)
  {}

int XC::SeriesMaterial::setTrialStrain(double newStrain, double strainRate)
  {
    // Using the incremental iterative strain
    double dv = newStrain-Tstrain;

    Tstrain = newStrain;

    // Stress increment using tangent at last iteration
    double dq = Ttangent*dv;

    // Update stress 
    Tstress += dq;

    for(int j = 0; j < maxIterations; j++)
      {
        // Set to zero for integration
        double f = 0.0;
        double vr = 0.0;
        const size_t numMaterials= theModels.size();
        for(size_t i = 0; i < numMaterials; i++)
          {
            // Stress unbalance in material i
            double ds = Tstress - stress[i];
            // Strain increment
            double de = flex[i]*ds;
            if(initialFlag == true)
              strain[i] += de;

            // Update material i
            theModels[i]->setTrialStrain(strain[i]);
            // Get updated stress from material i
            stress[i]= theModels[i]->getStress();

            // Get updated flexibility from material i
            flex[i] = theModels[i]->getTangent();
            if(fabs(flex[i]) > 1.0e-12)
              flex[i] = 1.0/flex[i];
            else
              flex[i] = (flex[i] < 0.0) ? -1.0e12 : 1.0e12;

            // Stress unbalance in material i
            ds = Tstress - stress[i];
            // Residual strain in material i
            de = flex[i]*ds;

            // Integrate flexibility ...
            f+= flex[i];

            // ... and integrate residual strain
            vr += strain[i] + de;
          }
        // Updated series tangent
        if(fabs(f) > 1.0e-12)
          Ttangent = 1.0/f;
        else
          Ttangent = (f < 0.0) ? -1.0e12 : 1.0e12;

        // Residual deformation
        dv = Tstrain - vr;
        // Stress increment
        dq = Ttangent*dv;
        if(fabs(dq*dv) < tolerance)
          break;
      }
    // Updated stress
    Tstress += dq;
    initialFlag = true;
    return 0;
  }

double XC::SeriesMaterial::getStrain(void) const
  { return Tstrain; }

double XC::SeriesMaterial::getStress(void) const
  { return Tstress; }

double XC::SeriesMaterial::getTangent(void) const
  { return Ttangent; }


double XC::SeriesMaterial::getInitialTangent(void) const
  {
    double kf = 0.0;
    double k  = 0.0;

    const size_t numMaterials= theModels.size();
    if(numMaterials != 0)
    kf = theModels[0]->getInitialTangent();
    for(size_t i=1; i<numMaterials; i++)
      {
        k = theModels[i]->getInitialTangent();
        if((kf + k) != 0.0)
          kf = kf*k/(kf+k);
        else
          return 0.0;
      }
    return kf;
  }


int XC::SeriesMaterial::commitState(void)
  {
    Cstrain = Tstrain;
    Cstress = Tstress;
    Ctangent = Ttangent;

    int err= theModels.commitState();
    // Commented out for the same reason it was taken
    // out of the NLBeamColumn commitState() -- MHS
    //initialFlag = false;
    return err;
  }

int XC::SeriesMaterial::revertToLastCommit(void)
  {
    Tstrain = Cstrain;
    Tstress = Cstress;
    Ttangent = Ctangent;
    
    int err= theModels.revertToLastCommit();
    const size_t numMaterials= theModels.size();
    for(size_t i = 0;i<numMaterials; i++)
      {
        strain[i] = theModels[i]->getStrain();
        stress[i] = theModels[i]->getStress();
        flex[i] = theModels[i]->getTangent();
        if(fabs(flex[i]) > 1.0e-12)
          flex[i] = 1.0/flex[i];
        else
          flex[i] = (flex[i] < 0.0) ? -1.0e12 : 1.0e12;
      }
    initialFlag = false;
    return err;
  }


int XC::SeriesMaterial::revertToStart(void)
  {
    Cstrain = 0.0;
    Cstress = 0.0;
    Ctangent = 0.0;
    int err= theModels.revertToStart();
    const size_t numMaterials= theModels.size();
    for(size_t i = 0; i < numMaterials; i++)
      {
         strain[i] = 0.0;
         stress[i] = 0.0;
         flex[i] = 0.0;
      }
    return err;    
  }



XC::UniaxialMaterial *XC::SeriesMaterial::getCopy(void) const
  { return new SeriesMaterial(*this); }

//! @brief Send its members through the channel being passed as parameter.
int XC::SeriesMaterial::sendData(CommParameters &cp)
  {
    int res= ConnectedMaterial::sendData(cp);
    res+= cp.sendDoubles(Tstrain,Cstrain,Tstress,Cstress,Ttangent,Ctangent,getDbTagData(),CommMetaData(3));
    res+= cp.sendInts(maxIterations,initialFlag,getDbTagData(),CommMetaData(4));
    res+= cp.sendDouble(tolerance,getDbTagData(),CommMetaData(5));
    res+= cp.sendVector(stress,getDbTagData(),CommMetaData(6));
    res+= cp.sendVector(flex,getDbTagData(),CommMetaData(7));
    res+= cp.sendVector(strain,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives its members through the channel being passed as parameter.
int XC::SeriesMaterial::recvData(const CommParameters &cp)
  {
    int res= ConnectedMaterial::recvData(cp);
    res+= cp.receiveDoubles(Tstrain,Cstrain,Tstress,Cstress,Ttangent,Ctangent,getDbTagData(),CommMetaData(3));
    int iFlag;
    res+= cp.receiveInts(maxIterations,iFlag,getDbTagData(),CommMetaData(4));
    initialFlag= iFlag;
    res+= cp.receiveDouble(tolerance,getDbTagData(),CommMetaData(5));
    res+= cp.receiveVector(stress,getDbTagData(),CommMetaData(6));
    res+= cp.receiveVector(flex,getDbTagData(),CommMetaData(7));
    res+= cp.receiveVector(strain,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::SeriesMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(9);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::SeriesMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(9);
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


void XC::SeriesMaterial::Print(std::ostream &s, int flag)
  {
    s << "\nSeriesMaterial, tag: " << this->getTag() << std::endl;
    s << "\tUniaxial Componenets" << std::endl;
    const size_t numMaterials= theModels.size();
    for(size_t i = 0; i < numMaterials; i++)
      s << "\t\tUniaxial XC::Material, tag: " << theModels[i]->getTag() << std::endl;
  }

XC::Response* XC::SeriesMaterial::setResponse(const std::vector<std::string> &argv, Information &info)
  {
    // See if the response is one of the defaults
    Response *res = XC::UniaxialMaterial::setResponse(argv, info);
    if(res != 0)
      return res;

    const size_t numMaterials= theModels.size();
    const size_t argc= argv.size();
    if(argv[0] == "strains")
      return new MaterialResponse(this, 100, XC::Vector(numMaterials));
    else if(argv[0] == "material" || argv[0] == "component")
      {
        if(argc > 1)
          {
            size_t matNum= boost::lexical_cast<size_t>(argv[1]) - 1;
            if(matNum >= 0 && matNum < numMaterials)
              {
                std::vector<std::string> argvMat= argv;
                argvMat.erase(argvMat.begin());
                argvMat.erase(argvMat.begin());
                return theModels[matNum]->setResponse(argvMat,info);
              }
            else
              return nullptr;
          }
        else
          return nullptr;
      }
    else
      return UniaxialMaterial::setResponse(argv, info);
  }

int XC::SeriesMaterial::getResponse(int responseID, Information &info)
  {
    Vector strains(strain);
    switch (responseID)
      {
      case 100:
        return info.setVector(strains);
      default:
        return this->UniaxialMaterial::getResponse(responseID, info);
      }
  }
