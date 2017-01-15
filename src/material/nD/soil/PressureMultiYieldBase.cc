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
//PressureMultiYieldBase.cpp

#include "PressureMultiYieldBase.h"
#include <cmath>
#include <cstdlib>
#include <domain/mesh/element/Information.h>
#include <utility/matrix/ID.h>
#include <utility/recorder/response/MaterialResponse.h>

#include <material/nD/soil/MultiYieldSurface.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/nDarray/Tensor.h>
#include <utility/matrix/nDarray/stresst.h>
#include "material/nD/TipoMaterialND.h"

int XC::PressureMultiYieldBase::matCount=0;
std::vector<int> XC::PressureMultiYieldBase::loadStagex;  //=0 if elastic; =1 if plastic
std::vector<int> XC::PressureMultiYieldBase::ndmx;  //num of dimensions (2 or 3)
std::vector<double> XC::PressureMultiYieldBase::rhox;
std::vector<double> XC::PressureMultiYieldBase::frictionAnglex;
std::vector<double> XC::PressureMultiYieldBase::peakShearStrainx;
std::vector<double> XC::PressureMultiYieldBase::refPressurex;
std::vector<double> XC::PressureMultiYieldBase::cohesionx;
std::vector<double> XC::PressureMultiYieldBase::pressDependCoeffx;
std::vector<int> XC::PressureMultiYieldBase::numOfSurfacesx;
std::vector<double> XC::PressureMultiYieldBase::residualPressx;
std::vector<double> XC::PressureMultiYieldBase::stressRatioPTx;

XC::Matrix XC::PressureMultiYieldBase::theTangent(6,6);
XC::T2Vector XC::PressureMultiYieldBase::subStrainRate;

const double pi= 3.14159265358979;

void XC::PressureMultiYieldBase::setup(int nd, double r, double frictionAng,double peakShearStra, double refPress, double pressDependCoe, double cohesi,int numberOfYieldSurf)
  {
    if(nd !=2 && nd !=3)
      {
        std::cerr << "FATAL:XC::PressureMultiYieldBase:: dimension error" << std::endl;
        std::cerr << "Dimension has to be 2 or 3, you give nd= " << nd << std::endl;
        exit(-1);
      }

  if(cohesi < 0)
    {
      std::cerr << "WARNING:XC::PressureMultiYieldBase:: cohesion < 0" << std::endl;
      std::cerr << "Will reset cohesion to zero." << std::endl;
      cohesi = 0.;
    } 
  if(peakShearStra <= 0)
    {
      std::cerr << "FATAL:XC::PressureMultiYieldBase:: peakShearStra <= 0" << std::endl;
     exit(-1);
    }
  if(refPress <= 0)
    {
      std::cerr << "FATAL:XC::PressureMultiYieldBase:: refPress <= 0" << std::endl;
      exit(-1);
    }
  if(pressDependCoe < 0)
    {
      std::cerr << "WARNING:XC::PressureMultiYieldBase:: pressDependCoe < 0" << std::endl;
      std::cerr << "Will reset pressDependCoe to zero." << std::endl;
      pressDependCoe = 0.;
    }
  if(numberOfYieldSurf <= 0)
    {
      std::cerr << "WARNING:XC::PressureMultiYieldBase:: numberOfSurfaces <= 0" << std::endl;
      std::cerr << "Will use 10 yield surfaces." << std::endl;
      numberOfYieldSurf = 10;
    }
  if(numberOfYieldSurf > 100)
    {
      std::cerr << "WARNING:XC::PressureMultiYieldBase::PressureMultiYieldBase: numberOfSurfaces > 40" << std::endl;
      std::cerr << "Will use 100 yield surfaces." << std::endl;
      numberOfYieldSurf = 100;
    }
  if(r < 0)
    {
      std::cerr << "FATAL:XC::PressureMultiYieldBase:: rho <= 0" << std::endl;
     exit(-1);
    }

  if(matCount%20 == 0)
    {
       loadStagex.resize(matCount+20);
       ndmx.resize(matCount+20);
       rhox.resize(matCount+20);
       frictionAnglex.resize(matCount+20);
       peakShearStrainx.resize(matCount+20);
       refPressurex.resize(matCount+20);
       cohesionx.resize(matCount+20);
       pressDependCoeffx.resize(matCount+20);
       numOfSurfacesx.resize(matCount+20);
       residualPressx.resize(matCount+20);
       stressRatioPTx.resize(matCount+20);
      }

    ndmx[matCount] = nd;
    loadStagex[matCount] = 0;   //default
    frictionAnglex[matCount]= frictionAng;
    peakShearStrainx[matCount]= peakShearStra;
    refPressurex[matCount]= -refPress;  //compression is negative
    cohesionx[matCount]= cohesi;
    pressDependCoeffx[matCount] = pressDependCoe;
    numOfSurfacesx[matCount] = numberOfYieldSurf;
    rhox[matCount]= r;
  
    matN = matCount;
    matCount++;

    int numOfSurfaces = numOfSurfacesx[matN];

    e2p = committedActiveSurf = activeSurfaceNum = 0; 
  
    theSurfaces.resize(numOfSurfaces+1); //first surface not used
    committedSurfaces.resize(numOfSurfaces+1); 
  }

XC::PressureMultiYieldBase::PressureMultiYieldBase(int tag, int classTag, int nd, double r, double frictionAng,double peakShearStra, double refPress, double pressDependCoe, double cohesi,int numberOfYieldSurf)
 :XC::NDMaterial(tag,classTag), theSurfaces(), committedSurfaces(), currentStress(),trialStress(), currentStrain(), strainRate()
  {
    setup(nd,r,frictionAng,peakShearStra,refPress,pressDependCoe,cohesi,numberOfYieldSurf);
  }
   
XC::PressureMultiYieldBase::PressureMultiYieldBase(int tag, int classTag) 
 : XC::NDMaterial(tag,classTag), 
   theSurfaces(), committedSurfaces(),currentStress(), trialStress(), currentStrain(), 
   strainRate() {}


XC::PressureMultiYieldBase::PressureMultiYieldBase(const PressureMultiYieldBase & a)
 : XC::NDMaterial(a), currentStress(a.currentStress), trialStress(a.trialStress), 
  currentStrain(a.currentStrain), strainRate(a.strainRate)
  {
    matN = a.matN;
    e2p = a.e2p;
    committedSurfaces= a.committedSurfaces;
    theSurfaces= a.theSurfaces;
    activeSurfaceNum = a.activeSurfaceNum;
    committedActiveSurf = a.committedActiveSurf;
    currentStress= a.currentStress;
    trialStress = a.trialStress;
    currentStrain= a.currentStrain;
    strainRate= a.strainRate;
  }


//! @brief Send members del objeto through the channel being passed as parameter.
int XC::PressureMultiYieldBase::sendData(CommParameters &cp)
  {
    int res= NDMaterial::sendData(cp);
    res+= cp.sendInts(matN,e2p,activeSurfaceNum,committedActiveSurf,getDbTagData(),CommMetaData(1));
    //theSurfaces; // XXX no se envía (reservamos la posición 2).
    //committedSurfaces;  // XXX no se envía (reservamos la posición 3).
    res+= cp.sendMovable(currentStress,getDbTagData(),CommMetaData(4));
    res+= cp.sendMovable(trialStress,getDbTagData(),CommMetaData(5));
    res+= cp.sendMovable(currentStrain,getDbTagData(),CommMetaData(6));
    res+= cp.sendMovable(strainRate,getDbTagData(),CommMetaData(7));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::PressureMultiYieldBase::recvData(const CommParameters &cp)
  {
    int res= NDMaterial::recvData(cp);
    res+= cp.receiveInts(matN,e2p,activeSurfaceNum,committedActiveSurf,getDbTagData(),CommMetaData(1));
    //theSurfaces; // XXX no se envía (reservamos la posición 2).
    //committedSurfaces;  // XXX no se envía (reservamos la posición 3).
    res+= cp.receiveMovable(currentStress,getDbTagData(),CommMetaData(4));
    res+= cp.receiveMovable(trialStress,getDbTagData(),CommMetaData(5));
    res+= cp.receiveMovable(currentStrain,getDbTagData(),CommMetaData(6));
    res+= cp.receiveMovable(strainRate,getDbTagData(),CommMetaData(7));
    return res;
  }


