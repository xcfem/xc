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
//PressureDependMultiYieldBase.cpp

#include <cmath>
#include <cstdlib>
#include <material/nD/soil/PressureDependMultiYieldBase.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/matrix/ID.h>
#include <utility/recorder/response/MaterialResponse.h>

#include <material/nD/soil/MultiYieldSurface.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/nDarray/Tensor.h>
#include <utility/matrix/nDarray/stresst.h>
#include "material/nD/TipoMaterialND.h"

double* XC::PressureDependMultiYieldBase::refShearModulusx=0;
double* XC::PressureDependMultiYieldBase::refBulkModulusx=0;
double* XC::PressureDependMultiYieldBase::phaseTransfAnglex=0; 
double* XC::PressureDependMultiYieldBase::contractParam1x=0;
double* XC::PressureDependMultiYieldBase::dilateParam1x=0;
double* XC::PressureDependMultiYieldBase::dilateParam2x=0;
double* XC::PressureDependMultiYieldBase::liquefyParam1x=0;
double* XC::PressureDependMultiYieldBase::liquefyParam2x=0;
double* XC::PressureDependMultiYieldBase::einitx=0;    //initial void ratio
double* XC::PressureDependMultiYieldBase::volLimit1x=0;
double* XC::PressureDependMultiYieldBase::volLimit2x=0;
double* XC::PressureDependMultiYieldBase::volLimit3x=0;
double* XC::PressureDependMultiYieldBase::Hvx=0;
double* XC::PressureDependMultiYieldBase::Pvx=0;

double XC::PressureDependMultiYieldBase::pAtm = 101.;
XC::T2Vector XC::PressureDependMultiYieldBase::trialStrain;
XC::Vector XC::PressureDependMultiYieldBase::workV6(6);
XC::T2Vector XC::PressureDependMultiYieldBase::workT2V;

const double pi= 3.14159265358979;

XC::PressureDependMultiYieldBase::PressureDependMultiYieldBase(int tag, int classTag, int nd, 
                                                    double r, double refShearModul,
                                                    double refBulkModul, double frictionAng,
                                                    double peakShearStra, double refPress, 
                                                    double pressDependCoe,
                                                    double phaseTransformAng, 
                                                    double contractionParam1,
                                                    double dilationParam1,
                                                    double dilationParam2,
                                                    double liquefactionParam1,
                                                    double liquefactionParam2,
                                                    double liquefactionParam4,
                                                    int numberOfYieldSurf, 
                                                                double * gredu,
                                                    double ei,
                                                    double volLim1, double volLim2, double volLim3,
                                                    double atm, double cohesi,
                                                        double hv, double pv)
  : XC::PressureMultiYieldBase(tag,classTag,nd,r,frictionAng,peakShearStra,refPress,pressDependCoe,cohesi,numberOfYieldSurf), PPZPivot(), PPZCenter(), PPZPivotCommitted(), PPZCenterCommitted()
   {
  if(refShearModul <= 0) {
    std::cerr << "FATAL:XC::PressureDependMultiYieldBase:: refShearModulus <= 0" << std::endl;
   exit(-1);
  }
  if(refBulkModul <= 0) {
    std::cerr << "FATAL:XC::PressureDependMultiYieldBase:: refBulkModulus <= 0" << std::endl;
   exit(-1);
  }
  if(frictionAng <= 0.) {
    std::cerr << "FATAL:XC::PressureDependMultiYieldBase:: frictionAngle <= 0" << std::endl;
   exit(-1);
  }
  if(frictionAng >= 90.) {
    std::cerr << "FATAL:XC::PressureDependMultiYieldBase:: frictionAngle >= 90" << std::endl;
   exit(-1);
  }
  if(phaseTransformAng <= 0.) {
    std::cerr << "FATAL:XC::PressureDependMultiYieldBase:: phaseTransformAng <= 0" << std::endl;
   exit(-1);
  }
  if(phaseTransformAng > frictionAng) {
    std::cerr << "WARNING:XC::PressureDependMultiYieldBase:: phaseTransformAng > frictionAng" << std::endl;
    std::cerr << "Will set phaseTransformAng = frictionAng." <<std::endl;
    phaseTransformAng = frictionAng;
  }

  if(ei < 0) {
    std::cerr << "FATAL:XC::PressureDependMultiYieldBase:: e <= 0" << std::endl;
   exit(-1);
  }

  if(matCount%20 == 0)
    {
      double * temp4 = refShearModulusx;
      double * temp5 = refBulkModulusx;
      double * temp13 = phaseTransfAnglex; 
      double * temp14 = contractParam1x;
      double * temp15 = dilateParam1x;
      double * temp16 = dilateParam2x;
      double * temp17 = liquefyParam1x;
      double * temp18 = liquefyParam2x;
      double * temp19 = einitx;    //initial void ratio
      double * temp20 = Hvx;
      double * temp21 = Pvx;


     refShearModulusx = new double[matCount+20];
     refBulkModulusx = new double[matCount+20];
     phaseTransfAnglex = new double[matCount+20]; 
     contractParam1x = new double[matCount+20];
     dilateParam1x = new double[matCount+20];
     dilateParam2x = new double[matCount+20];
     liquefyParam1x = new double[matCount+20];
     liquefyParam2x = new double[matCount+20];
     einitx = new double[matCount+20];    //initial void ratio
     Hvx = new double[matCount+20];
     Pvx = new double[matCount+20];

     for(int i=0; i<matCount; i++)
       {
         refShearModulusx[i] = temp4[i];
         refBulkModulusx[i] = temp5[i];
         phaseTransfAnglex[i] = temp13[i]; 
         contractParam1x[i] = temp14[i];
         dilateParam1x[i] = temp15[i];
         dilateParam2x[i] = temp16[i];
         liquefyParam1x[i] = temp17[i];
         liquefyParam2x[i] = temp18[i];
         einitx[i] = temp19[i];    //initial void ratio
         Hvx[i] = temp20[i];
         Pvx[i] = temp21[i];
       }
     
         if(matCount > 0)
           {
             delete [] temp4; 
             delete [] temp5; 
             delete [] temp13; delete [] temp14; delete [] temp15; delete [] temp16; 
             delete [] temp17; delete [] temp18; delete [] temp19; delete [] temp20; 
             delete [] temp21;
           }
  }

  refShearModulusx[matCount] = refShearModul;
  refBulkModulusx[matCount] = refBulkModul;
  phaseTransfAnglex[matCount] = phaseTransformAng;
  contractParam1x[matCount] = contractionParam1;
  dilateParam1x[matCount] = dilationParam1;
  dilateParam2x[matCount] = dilationParam2;
  liquefyParam1x[matCount] = liquefactionParam1;
  liquefyParam2x[matCount] = liquefactionParam2;
  einitx[matCount] = ei;
  Hvx[matCount] = hv;
  Pvx[matCount] = pv;

  pAtm = atm;

  //int numOfSurfaces = numOfSurfacesx[matN];
  initPress = refPressurex[matN];

  onPPZCommitted = onPPZ = -1 ; 
  PPZSizeCommitted = PPZSize = 0.;
  pressureDCommitted = pressureD = modulusFactor = 0.;
  cumuDilateStrainOctaCommitted    = cumuDilateStrainOcta = 0.;
  maxCumuDilateStrainOctaCommitted = maxCumuDilateStrainOcta = 0.;
  cumuTranslateStrainOctaCommitted = cumuTranslateStrainOcta = 0.;
  prePPZStrainOctaCommitted = prePPZStrainOcta = 0.;
  oppoPrePPZStrainOctaCommitted = oppoPrePPZStrainOcta = 0.;
  maxPress = 0.;

  }
   
XC::PressureDependMultiYieldBase::PressureDependMultiYieldBase(int tag, int classTag) 
 : XC::PressureMultiYieldBase(tag,classTag), PPZPivot(), PPZCenter(),
   PPZPivotCommitted(), PPZCenterCommitted()
  {}


XC::PressureDependMultiYieldBase::PressureDependMultiYieldBase(const PressureDependMultiYieldBase & a)
 : XC::PressureMultiYieldBase(a), 
   PPZPivot(a.PPZPivot), PPZCenter(a.PPZCenter), 
   PPZPivotCommitted(a.PPZPivotCommitted), 
   PPZCenterCommitted(a.PPZCenterCommitted)
  {
    strainPTOcta = a.strainPTOcta;
    modulusFactor = a.modulusFactor;
    pressureDCommitted     = a.pressureDCommitted;
    onPPZCommitted = a.onPPZCommitted; 
    PPZSizeCommitted      = a.PPZSizeCommitted;
    cumuDilateStrainOctaCommitted    = a.cumuDilateStrainOctaCommitted;
    maxCumuDilateStrainOctaCommitted = a.maxCumuDilateStrainOctaCommitted;
    cumuTranslateStrainOctaCommitted = a.cumuTranslateStrainOctaCommitted;
    prePPZStrainOctaCommitted        = a.prePPZStrainOctaCommitted;
    oppoPrePPZStrainOctaCommitted    = a.oppoPrePPZStrainOctaCommitted;
    pressureD     = a.pressureD;
    onPPZ = a.onPPZ; 
    PPZSize      = a.PPZSize;
    cumuDilateStrainOcta    = a.cumuDilateStrainOcta;
    maxCumuDilateStrainOcta = a.maxCumuDilateStrainOcta;
    cumuTranslateStrainOcta = a.cumuTranslateStrainOcta;
    prePPZStrainOcta        = a.prePPZStrainOcta;
    oppoPrePPZStrainOcta    = a.oppoPrePPZStrainOcta;
    initPress = a.initPress;
    maxPress = a.maxPress;
  }


void XC::PressureDependMultiYieldBase::elast2Plast(void) const
  {
    int loadStage = loadStagex[matN];
  int numOfSurfaces = numOfSurfacesx[matN];

  if(loadStage != 1 || e2p == 1) return;
  e2p = 1;

  if(currentStress.volume() > 0.) {
    //std::cerr << "WARNING:XC::PressureDependMultiYieldBase::elast2Plast(): material in tension." << std::endl;
    currentStress.setData(currentStress.deviator(),0);
  }

  // Active surface is 0, return
  if(currentStress.deviatorLength() == 0.) return;

  // Find active surface
  while(yieldFunc(currentStress, committedSurfaces, ++committedActiveSurf) > 0)
    {
      if(committedActiveSurf == numOfSurfaces)
        {
      //std::cerr <<"WARNING:XC::PressureDependMultiYieldBase::elast2Plast(): stress out of failure surface"<<std::endl;
      deviatorScaling(currentStress, committedSurfaces, numOfSurfaces);
      initSurfaceUpdate();
      return;
    }
  } 

  committedActiveSurf--;
  initSurfaceUpdate();
}

 
int XC::PressureDependMultiYieldBase::setTrialStrain(const XC::Vector &strain)
{
  int ndm = ndmx[matN];

  if(ndm==3 && strain.Size()==6) 
    workV6 = strain;
  else if(ndm==2 && strain.Size()==3) {
    workV6[0] = strain[0];
    workV6[1] = strain[1];
    workV6[2] = 0.0;
    workV6[3] = strain[2];
    workV6[4] = 0.0;
    workV6[5] = 0.0;
  }
  else {
    std::cerr << "Fatal:XC::PressureDependMultiYieldBase:: Material dimension is: " << ndm << std::endl;
    std::cerr << "But strain vector size is: " << strain.Size() << std::endl;
   exit(-1);
  }

  //strainRate.setData(workV6-currentStrain.t2Vector(1),1);
  workV6 -= currentStrain.t2Vector(1);
  strainRate.setData(workV6, 1);

  return 0;
}


int XC::PressureDependMultiYieldBase::setTrialStrain(const XC::Vector &strain, const XC::Vector &rate)
{
  return setTrialStrain(strain);
}


int XC::PressureDependMultiYieldBase::setTrialStrainIncr(const XC::Vector &strain)
{
  int ndm = ndmx[matN];

  if(ndm==3 && strain.Size()==6) 
    workV6 = strain;
  else if(ndm==2 && strain.Size()==3) {
    workV6[0] = strain[0];
    workV6[1] = strain[1];
    workV6[2] = 0.0;
    workV6[3] = strain[2];
    workV6[4] = 0.0;
    workV6[5] = 0.0;
  }
  else {
    std::cerr << "Fatal:XC::PressureDependMultiYieldBase:: Material dimension is: " << ndm << std::endl;
    std::cerr << "But strain vector size is: " << strain.Size() << std::endl;
   exit(-1);
  }

  strainRate.setData(workV6,1);
  return 0;
}


int XC::PressureDependMultiYieldBase::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
{
  return setTrialStrainIncr(strain);
}


const XC::Matrix & XC::PressureDependMultiYieldBase::getInitialTangent(void)
{
  int loadStage = loadStagex[matN];
  double refShearModulus = refShearModulusx[matN];
  double refBulkModulus = refBulkModulusx[matN];
  double pressDependCoeff = pressDependCoeffx[matN];
  double refPressure = refPressurex[matN];
  double residualPress = residualPressx[matN];
  int ndm = ndmx[matN];

  if(loadStage==2 && initPress==refPressure) 
          initPress = currentStress.volume();
  double factor= 0; 
  if(loadStage==0)
          factor = 1.;
  else if(loadStage==2) {
                factor = (initPress-residualPress)/(refPressure-residualPress);
                if(factor <= 1.e-10) factor = 1.e-10;
                else factor = pow(factor, pressDependCoeff);
                factor = (1.e-10>factor) ? 1.e-10 : factor;
  }
  else if(loadStage==1)
          factor = getModulusFactor(currentStress);
  
  for(int i=0;i<6;i++) 
    for(int j=0;j<6;j++) {
      theTangent(i,j) = 0.;
      if(i==j) theTangent(i,j) += refShearModulus*factor;
      if(i<3 && j<3 && i==j) theTangent(i,j) += refShearModulus*factor;
      if(i<3 && j<3) theTangent(i,j) += (refBulkModulus - 2.*refShearModulus/3.)*factor;
    }

  if(ndm==3) 
    return theTangent;
  else {
    static XC::Matrix workM(3,3);
    workM(0,0) = theTangent(0,0);
    workM(0,1) = theTangent(0,1);
    workM(0,2) = 0.;

    workM(1,0) = theTangent(1,0);
    workM(1,1) = theTangent(1,1);
    workM(1,2) = 0.; 

    workM(2,0) = 0.; 
    workM(2,1) = 0.; 
    workM(2,2) = theTangent(3,3);

/* non-symmetric stiffness
    workM(0,2) = theTangent(0,3);
    workM(1,2) = theTangent(1,3);
    workM(2,0) = theTangent(3,0);
    workM(2,1) = theTangent(3,1);*/
    return workM;
  }
}


const XC::Vector & XC::PressureDependMultiYieldBase::getStrain(void)
  { return getCommittedStrain(); }


int XC::PressureDependMultiYieldBase::commitState(void)
{
  int loadStage = loadStagex[matN];
  //int numOfSurfaces = numOfSurfacesx[matN];

  currentStress = trialStress;
  //currentStrain = T2Vector(currentStrain.t2Vector() + strainRate.t2Vector());
  workV6 = currentStrain.t2Vector();
  workV6 += strainRate.t2Vector();
  currentStrain.setData(workV6);

  workV6.Zero();
  strainRate.setData(workV6);

  if(loadStage==1)
    {
      committedActiveSurf = activeSurfaceNum;
      committedSurfaces= theSurfaces;
      pressureDCommitted = pressureD;
      onPPZCommitted = onPPZ;
      PPZSizeCommitted = PPZSize;
      cumuDilateStrainOctaCommitted = cumuDilateStrainOcta;
      maxCumuDilateStrainOctaCommitted = maxCumuDilateStrainOcta;
      cumuTranslateStrainOctaCommitted = cumuTranslateStrainOcta;
      prePPZStrainOctaCommitted = prePPZStrainOcta;
      oppoPrePPZStrainOctaCommitted = oppoPrePPZStrainOcta;
      PPZPivotCommitted = PPZPivot;
      PPZCenterCommitted = PPZCenter;
      if(currentStress.volume() < maxPress) maxPress = currentStress.volume();
    }
    return 0;
  }


int XC::PressureDependMultiYieldBase::revertToLastCommit(void)
  { return 0; }


int XC::PressureDependMultiYieldBase::getOrder(void) const
{
  int ndm = ndmx[matN];

  return (ndm == 2) ? 3 : 6;
}


int XC::PressureDependMultiYieldBase::updateParameter(int responseID, Information &info)
{
        if(responseID<10) 
                loadStagex[matN] = responseID;

        else {
                if(responseID==10) refShearModulusx[matN]=info.theDouble;
                if(responseID==11) refBulkModulusx[matN]=info.theDouble;
        }

  return 0;
}

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::PressureDependMultiYieldBase::sendData(CommParameters &cp)
  {
    int res= PressureMultiYieldBase::sendData(cp);
    res+= cp.sendDoubles(modulusFactor,initPress,getDbTagData(),CommMetaData(8));
    res+= cp.sendMovable(subStrainRate,getDbTagData(),CommMetaData(9));
    res+= cp.sendDoubles(pressureD,pressureDCommitted,strainPTOcta,PPZSize,PPZSizeCommitted,getDbTagData(),CommMetaData(10));
    res+= cp.sendInts(onPPZ,onPPZCommitted,getDbTagData(),CommMetaData(11));
    res+= cp.sendDoubles(cumuDilateStrainOcta,maxCumuDilateStrainOcta,cumuTranslateStrainOcta,getDbTagData(),CommMetaData(12));
    res+= cp.sendDoubles(prePPZStrainOcta,oppoPrePPZStrainOcta,cumuDilateStrainOctaCommitted,getDbTagData(),CommMetaData(13));
    res+= cp.sendMovable(PPZPivot,getDbTagData(),CommMetaData(14));
    res+= cp.sendMovable(PPZCenter,getDbTagData(),CommMetaData(15));
    res+= cp.sendDoubles(maxCumuDilateStrainOctaCommitted,cumuTranslateStrainOctaCommitted,getDbTagData(),CommMetaData(16));
    res+= cp.sendDoubles(prePPZStrainOctaCommitted,oppoPrePPZStrainOctaCommitted,maxPress,getDbTagData(),CommMetaData(17));
    res+= cp.sendMovable(PPZPivotCommitted,getDbTagData(),CommMetaData(18));
    res+= cp.sendMovable(PPZCenterCommitted,getDbTagData(),CommMetaData(19));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::PressureDependMultiYieldBase::recvData(const CommParameters &cp)
  {
    int res= PressureMultiYieldBase::recvData(cp);
    res+= cp.receiveDoubles(modulusFactor,initPress,getDbTagData(),CommMetaData(8));
    res+= cp.receiveMovable(subStrainRate,getDbTagData(),CommMetaData(9));
    res+= cp.receiveDoubles(pressureD,pressureDCommitted,strainPTOcta,PPZSize,PPZSizeCommitted,getDbTagData(),CommMetaData(10));
    res+= cp.receiveInts(onPPZ,onPPZCommitted,getDbTagData(),CommMetaData(11));
    res+= cp.receiveDoubles(cumuDilateStrainOcta,maxCumuDilateStrainOcta,cumuTranslateStrainOcta,getDbTagData(),CommMetaData(12));
    res+= cp.receiveDoubles(prePPZStrainOcta,oppoPrePPZStrainOcta,cumuDilateStrainOctaCommitted,getDbTagData(),CommMetaData(13));
    res+= cp.receiveMovable(PPZPivot,getDbTagData(),CommMetaData(14));
    res+= cp.receiveMovable(PPZCenter,getDbTagData(),CommMetaData(15));
    res+= cp.receiveDoubles(maxCumuDilateStrainOctaCommitted,cumuTranslateStrainOctaCommitted,getDbTagData(),CommMetaData(16));
    res+= cp.receiveDoubles(prePPZStrainOctaCommitted,oppoPrePPZStrainOctaCommitted,maxPress,getDbTagData(),CommMetaData(17));
    res+= cp.receiveMovable(PPZPivotCommitted,getDbTagData(),CommMetaData(18));
    res+= cp.receiveMovable(PPZCenterCommitted,getDbTagData(),CommMetaData(19));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PressureDependMultiYieldBase::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(20);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


//! @brief Receives object through the channel being passed as parameter.
int XC::PressureDependMultiYieldBase::recvSelf(const CommParameters &cp)    
  {
    inicComm(20);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }


XC::Response *XC::PressureDependMultiYieldBase::setResponse(const std::vector<std::string> &argv, Information &matInfo)
  {
    if(argv[0] == "stress" || argv[0] == "stresses")
      return new MaterialResponse(this, 1, this->getCommittedStress());
    else if(argv[0] == "strain" || argv[0] == "strains")
      return new MaterialResponse(this, 2, this->getCommittedStrain());
    else if(argv[0] == "tangent")
      return new MaterialResponse(this, 3, this->getTangent());
    else if(argv[0] == "backbone")
      {
        const int argc= argv.size();
        const int numOfSurfaces = numOfSurfacesx[matN];
        static XC::Matrix curv(numOfSurfaces+1,(argc-1)*2);
        for(int i=1; i<argc; i++)
          curv(0,(i-1)*2) = atoi(argv[i]);
        return new MaterialResponse(this, 4, curv);
      }
    else
      return 0;
  }


void XC::PressureDependMultiYieldBase::getBackbone(XC::Matrix & bb)
{
  double residualPress = residualPressx[matN];
  double refPressure = refPressurex[matN];
  double pressDependCoeff =pressDependCoeffx[matN];
  double refShearModulus = refShearModulusx[matN];
  int numOfSurfaces = numOfSurfacesx[matN];

  double vol, conHeig, scale, factor, shearModulus, stress1, 
                     stress2= 0.0, strain1= 0.0, strain2= 0.0, plastModulus, elast_plast, gre;

        for(int k=0; k<bb.noCols()/2; k++) {
                vol = bb(0,k*2);
                if(vol<=0.) {
                        std::cerr <<k<< "\nPressureMultiYieldBase " <<this->getTag()
                          <<": invalid confinement for backbone recorder, " << vol << std::endl;
                        continue;
                }
                conHeig = vol + residualPress;
                scale = -conHeig / (refPressure-residualPress);
                factor = pow(scale, pressDependCoeff); 
                shearModulus = factor*refShearModulus;

                for(int i=1; i<=numOfSurfaces; i++) {
                        if(i==1) {
                                stress2 = theSurfaces[i].size()*conHeig/sqrt(3.0);
                                strain2 = stress2/shearModulus;
                                bb(1,k*2) = strain2; bb(1,k*2+1) = shearModulus;
                        } else {
                                stress1 = stress2; strain1 = strain2;
                                plastModulus = factor*theSurfaces[i-1].modulus();
                                elast_plast = 2*shearModulus*plastModulus/(2*shearModulus+plastModulus);
                                stress2 = theSurfaces[i].size()*conHeig/sqrt(3.0);
                          strain2 = 2*(stress2-stress1)/elast_plast + strain1;
                                gre = stress2/strain2;
        bb(i,k*2) = strain2; bb(i,k*2+1) = gre;
                        }
                }
        }

}


int XC::PressureDependMultiYieldBase::getResponse(int responseID, Information &matInfo)
{
  switch(responseID) {
  case -1:
    return -1;
  case 1:
    if(matInfo.theVector != 0)
      *(matInfo.theVector) = getCommittedStress();
    return 0;
  case 2:
    if(matInfo.theVector != 0)
      *(matInfo.theVector) = getCommittedStrain();
    return 0;
  case 3:
    if(matInfo.theMatrix != 0)
      *(matInfo.theMatrix) = getTangent();
    return 0;
  case 4:
    if(matInfo.theMatrix != 0) 
      getBackbone(*(matInfo.theMatrix));
    return 0;
  default:
    return -1;
  }
}


void XC::PressureDependMultiYieldBase::Print(std::ostream &s, int flag )
{
  s << "PressureDependMultiYieldBase" << std::endl;
}


const XC::Vector & XC::PressureDependMultiYieldBase::getCommittedStress(void)
{
        int ndm = ndmx[matN];
        int numOfSurfaces = numOfSurfacesx[matN];
    double residualPress = residualPressx[matN];

        double scale = currentStress.deviatorRatio(residualPress)/committedSurfaces[numOfSurfaces].size();
        if(loadStagex[matN] != 1) scale = 0.;
  if(ndm==3) {
                static XC::Vector temp7(7);
                workV6 = currentStress.t2Vector();
    temp7[0] = workV6[0];
    temp7[1] = workV6[1];
    temp7[2] = workV6[2];
    temp7[3] = workV6[3];
    temp7[4] = workV6[4];
    temp7[5] = workV6[5];
    temp7[6] = scale;
                return temp7;
        }

  else {
    static XC::Vector temp5(5);  
                workV6 = currentStress.t2Vector();
    temp5[0] = workV6[0];
    temp5[1] = workV6[1];
    temp5[2] = workV6[2];
    temp5[3] = workV6[3];
    temp5[4] = scale;
    /*temp5[5] = committedActiveSurf;
        temp5[7] = currentStress.deviatorRatio(residualPressx[matN]);
    temp5[8] = pressureDCommitted;
    temp5[9] = cumuDilateStrainOctaCommitted;
    temp5[10] = maxCumuDilateStrainOctaCommitted;
    temp5[11] = cumuTranslateStrainOctaCommitted;
    temp5[12] = onPPZCommitted;
    temp5[13] = PPZSizeCommitted;*/
    return temp5;
  }
}


const XC::Vector & XC::PressureDependMultiYieldBase::getCommittedStrain(void)
{
        int ndm = ndmx[matN];

  if(ndm==3)
    return currentStrain.t2Vector(1);
  else {
                static XC::Vector workV(3);
                workV6 = currentStrain.t2Vector(1);
    workV[0] = workV6[0];
    workV[1] = workV6[1];
    workV[2] = workV6[3];
    return workV;
  }
}


double XC::PressureDependMultiYieldBase::yieldFunc(const XC::T2Vector & stress, 
						   const std::vector<MultiYieldSurface> &surfaces, int surfaceNum) const
  {
    const double residualPress = residualPressx[matN];
    const double coneHeight = stress.volume() - residualPress;
    //workV6 = stress.deviator() - surfaces[surfaceNum].center()*coneHeight;
    workV6 = stress.deviator();
    workV6.addVector(1.0, surfaces[surfaceNum].center(), -coneHeight);
    const double sz = surfaces[surfaceNum].size()*coneHeight;
    return 3./2.*(workV6 && workV6) - sz * sz;
  }


void XC::PressureDependMultiYieldBase::deviatorScaling(T2Vector & stress, 
                                               const std::vector<MultiYieldSurface> &surfaces, 
                                               int surfaceNum) const
{
  double residualPress = residualPressx[matN];
  int numOfSurfaces = numOfSurfacesx[matN];

  double diff = yieldFunc(stress, surfaces, surfaceNum);
  double coneHeight = stress.volume() - residualPress;

  if( surfaceNum < numOfSurfaces && diff < 0. ) {
    double sz = -surfaces[surfaceNum].size()*coneHeight;
    double deviaSz = sqrt(sz*sz + diff);
    static XC::Vector devia(6);
    devia = stress.deviator(); 
    workV6 = devia;
    workV6.addVector(1.0, surfaces[surfaceNum].center(), -coneHeight);
    double coeff = (sz-deviaSz) / deviaSz;
    if(coeff < 1.e-13) coeff = 1.e-13;
    //devia += workV6 * coeff;
    devia.addVector(1.0, workV6, coeff);
    stress.setData(devia, stress.volume());
    deviatorScaling(stress, surfaces, surfaceNum);  // recursive call
  }

  if(surfaceNum==numOfSurfaces && fabs(diff) > LOW_LIMIT) {
    double sz = -surfaces[surfaceNum].size()*coneHeight;
    //workV6 = stress.deviator() * sz/sqrt(diff+sz*sz);
    workV6 = stress.deviator();
    workV6 *= sz/sqrt(diff+sz*sz);
    stress.setData(workV6, stress.volume());
  }
}


void XC::PressureDependMultiYieldBase::initSurfaceUpdate(void) const
{
  double residualPress = residualPressx[matN];
  int numOfSurfaces = numOfSurfacesx[matN];

  if(committedActiveSurf == 0) return; 
  
  double coneHeight = - (currentStress.volume() - residualPress);
  static XC::Vector devia(6); 
  devia = currentStress.deviator();
  double Ms = sqrt(3./2.*(devia && devia));

  if(committedActiveSurf < numOfSurfaces) { // failure surface can't move
    //workV6 = devia * (1. - committedSurfaces[committedActiveSurf].size()*coneHeight / Ms);
    workV6.addVector(0.0, devia, (1. - committedSurfaces[committedActiveSurf].size()*coneHeight / Ms));
         
    //workV6 = workV6 / -coneHeight;
    workV6 /= -coneHeight;
    committedSurfaces[committedActiveSurf].setCenter(workV6);
  }

  for(int i=1; i<committedActiveSurf; i++) {
    //workV6 = devia * (1. - committedSurfaces[i].size()*coneHeight / Ms);
    workV6.addVector(0.0, devia, (1. - committedSurfaces[i].size()*coneHeight / Ms));
    //workV6 = workV6 / -coneHeight;
    workV6 /= -coneHeight;
    committedSurfaces[i].setCenter(workV6); 
    theSurfaces[i] = committedSurfaces[i];
  }
  activeSurfaceNum = committedActiveSurf;
}


double XC::PressureDependMultiYieldBase::getModulusFactor(const T2Vector & stress) const
  {
    double residualPress = residualPressx[matN];
    double refPressure = refPressurex[matN];
    double pressDependCoeff =pressDependCoeffx[matN];

    double conHeig = stress.volume() - residualPress;
    double scale = conHeig / (refPressure-residualPress);  
    scale = pow(scale, pressDependCoeff);
        
    return (1.e-10>scale) ? 1.e-10 : scale; 
  }


void XC::PressureDependMultiYieldBase::setTrialStress(const T2Vector &stress) const
{
    double refShearModulus = refShearModulusx[matN];
        double refBulkModulus = refBulkModulusx[matN];

  modulusFactor = getModulusFactor(stress);
  //workV6 = stress.deviator() 
  //                     + subStrainRate.deviator()*2.*refShearModulus*modulusFactor;
  workV6 = stress.deviator();
  workV6.addVector(1.0, subStrainRate.deviator(), 2*refShearModulus*modulusFactor);
  
  double B = refBulkModulus*modulusFactor;
  
  if(Hvx[matN] != 0. && trialStress.volume()<=maxPress && subStrainRate.volume()<0.) {
     double tp = fabs(trialStress.volume() - residualPressx[matN]);
     B = (B*Hvx[matN]*pow(tp,Pvx[matN]))/(B+Hvx[matN]*pow(tp,Pvx[matN]));
  }
  
  double volume = stress.volume() + subStrainRate.volume()*3.*B;

  if(volume > 0.) volume = 0.;
  trialStress.setData(workV6, volume);
}


int XC::PressureDependMultiYieldBase::setSubStrainRate(void) const
{
    double residualPress = residualPressx[matN];
    double refShearModulus = refShearModulusx[matN];
        int numOfSurfaces = numOfSurfacesx[matN];

  if(activeSurfaceNum==numOfSurfaces) return 1;
  if(strainRate.isZero()) return 0;  

  double elast_plast_modulus;
  double conHeig = -(currentStress.volume() - residualPress);
  double factor = getModulusFactor(currentStress);
  if(activeSurfaceNum==0) 
    elast_plast_modulus = 2*refShearModulus*factor;
  else {
    double plast_modulus = theSurfaces[activeSurfaceNum].modulus()*factor;
    elast_plast_modulus = 2*refShearModulus*factor*plast_modulus 
      / (2*refShearModulus*factor+plast_modulus);
  }
  //workV6 = strainRate.deviator()*elast_plast_modulus;
  workV6.addVector(0.0, strainRate.deviator(), elast_plast_modulus);
  workT2V.setData(workV6,0);

  double singleCross = theSurfaces[numOfSurfaces].size()*conHeig / numOfSurfaces;
  double totalCross = 3.*workT2V.octahedralShear() / sqrt(2.);
  int numOfSub = totalCross/singleCross + 1;
  if(numOfSub > numOfSurfaces) numOfSub = numOfSurfaces;
        
  int numOfSub1 = strainRate.octahedralShear(1) / 1.0e-4;
  int numOfSub2 = strainRate.volume() / 1.e-5;
  if(numOfSub1 > numOfSub) numOfSub = numOfSub1;
  if(numOfSub2 > numOfSub) numOfSub = numOfSub2;

  workV6.addVector(0.0, strainRate.t2Vector(), 1.0/numOfSub);

  subStrainRate.setData(workV6);

  return numOfSub;
}


void XC::PressureDependMultiYieldBase::getContactStress(T2Vector &contactStress) const
{
    double residualPress = residualPressx[matN];

  double conHeig = trialStress.volume() - residualPress;
  static XC::Vector center(6);
  center = theSurfaces[activeSurfaceNum].center(); 
  //workV6 = trialStress.deviator() - center*conHeig;
  workV6 = trialStress.deviator();
  workV6.addVector(1.0, center, -conHeig);
  double Ms = sqrt(3./2.*(workV6 && workV6));
  //workV6 = workV6 * theSurfaces[activeSurfaceNum].size()*(-conHeig) / Ms + center*conHeig;
  workV6.addVector(theSurfaces[activeSurfaceNum].size()*(-conHeig) / Ms, center, conHeig);
  //return XC::T2Vector(workV6,trialStress.volume()); 
  contactStress.setData(workV6,trialStress.volume()); 
}


int XC::PressureDependMultiYieldBase::isLoadReversal(const XC::T2Vector & stress) const
  {
  if(activeSurfaceNum == 0) return 0;

  getSurfaceNormal(stress, workT2V);

  //if(((trialStress.t2Vector() - currentStress.t2Vector()) 
  //        && workT2V.t2Vector()) < 0) return 1;
  workV6 = trialStress.t2Vector();
  workV6 -= currentStress.t2Vector();
 
  if((workV6 && workT2V.t2Vector()) < 0) return 1; 

  return 0;   
}


void XC::PressureDependMultiYieldBase::getSurfaceNormal(const XC::T2Vector & stress, T2Vector &normal) const
{
    double residualPress = residualPressx[matN];

  double conHeig = stress.volume() - residualPress;
  workV6 = stress.deviator();
  static XC::Vector center(6);
  center = theSurfaces[activeSurfaceNum].center(); 
  double sz = theSurfaces[activeSurfaceNum].size();
  double volume = conHeig*((center && center) - 2./3.*sz*sz) - (workV6 && center);
  //workT2V.setData((workV6-center*conHeig)*3., volume);
  workV6.addVector(1.0, center, -conHeig);
  workV6 *= 3.0;
  workT2V.setData(workV6, volume);
  
  normal.setData(workT2V.unitT2Vector());
}


double XC::PressureDependMultiYieldBase::getLoadingFunc(const XC::T2Vector & contactStress, 
                                                const XC::T2Vector & surfaceNormal,
                                                double plasticPotential,
                                                int crossedSurface) const
{
    int numOfSurfaces = numOfSurfacesx[matN];
    double refShearModulus = refShearModulusx[matN];
        double refBulkModulus = refBulkModulusx[matN];

  double loadingFunc, limit;
  double modul = theSurfaces[activeSurfaceNum].modulus();
  double temp1 = 2. * refShearModulus * modulusFactor 
    * (surfaceNormal.deviator() && surfaceNormal.deviator()) ;
  double temp2 = 9. * refBulkModulus * modulusFactor 
    * surfaceNormal.volume() * plasticPotential ;
        
  //for the first crossing 
  double temp = temp1 + temp2 + modul * modulusFactor;
  if(activeSurfaceNum == numOfSurfaces) 
    limit = theSurfaces[activeSurfaceNum-1].modulus() * modulusFactor /2.;
  else limit = modul * modulusFactor /2.;
  if(temp < limit) {
    plasticPotential = (temp2 + limit - temp)/(9. * refBulkModulus * modulusFactor 
                                               * surfaceNormal.volume());
    temp = limit;
  }
  //loadingFunc = (surfaceNormal.t2Vector() 
  //                     && (trialStress.deviator()-contactStress.deviator()))/temp;
  workV6 = trialStress.deviator();
  workV6 -= contactStress.deviator();  
  loadingFunc = (surfaceNormal.t2Vector() && workV6) /temp;
  
  if(loadingFunc < 0.) loadingFunc = 0;

  //for more than one crossing 
  if(crossedSurface) {
    temp = (theSurfaces[activeSurfaceNum-1].modulus() - modul)
      /theSurfaces[activeSurfaceNum-1].modulus();
    loadingFunc *= temp;
  }

  return loadingFunc;
}


void XC::PressureDependMultiYieldBase::updateActiveSurface(void) const
{
    double residualPress = residualPressx[matN];
    int numOfSurfaces = numOfSurfacesx[matN];

  if(activeSurfaceNum == numOfSurfaces) return;

  double A, B, C, X;
  static XC::Vector t1(6);
  static XC::Vector t2(6);
  static XC::Vector center(6);
  static XC::Vector outcenter(6);
  double conHeig = trialStress.volume() - residualPress;
  center = theSurfaces[activeSurfaceNum].center();
  double size = theSurfaces[activeSurfaceNum].size();
  outcenter = theSurfaces[activeSurfaceNum+1].center();
  double outsize = theSurfaces[activeSurfaceNum+1].size();

  //t1 = trialStress.deviator() - center*conHeig;
  //t2 = (center - outcenter)*conHeig;
  t1 = trialStress.deviator();
  t1.addVector(1.0, center, -conHeig);
  t2 = center;
  t2 -= outcenter;
  t2 *=conHeig;
 
  A = t1 && t1;
  B = 2. * (t1 && t2);
  C = (t2 && t2) - 2./3.* outsize * outsize * conHeig * conHeig;
  X = secondOrderEqn(A,B,C,0);
  if( fabs(X-1.) < LOW_LIMIT ) X = 1.;
  if(X < 1.) return;

  if(X < 1.){
    //t2 = trialStress.deviator() - outcenter*conHeig;
    t2 = trialStress.deviator();
    t2.addVector(1.0, outcenter, -conHeig);
    
    double xx1 = (t2 && t2) - 2./3.* outsize * outsize * conHeig * conHeig;
    double xx2 = (t1 && t1) - 2./3.* size * size * conHeig * conHeig;
    std::cerr << "FATAL:XC::PressureDependMultiYieldBase::updateActiveSurface(): error in Direction of surface motion." << std::endl; 
    std::cerr << "X-1= " << X-1 <<" A= "<<A<<" B= "<<B<<" C= "<<C <<" M= "<<activeSurfaceNum<<" low_limit="<<LOW_LIMIT<< std::endl;
    std::cerr << "diff1= "<<xx1 <<" diff2= "<<xx2 <<" p= "<<conHeig<<" size= "<<size<<" outs= "<<outsize<<std::endl; 
   exit(-1);
  }

  //workV6 = (t1 * X + center*conHeig) * (1. - size / outsize) 
  //             - (center - outcenter * size / outsize) * conHeig;
  
  workV6.addVector(0.0, t1, X);
  workV6.addVector(1.0, center, conHeig);
  workV6 *= (1.0 - size / outsize);
  t2 = center;
  t2.addVector(1.0, outcenter, -size/outsize);
  t2 *= conHeig;
  workV6 -= t2;

  workT2V.setData(workV6);
  if(workT2V.deviatorLength() < LOW_LIMIT) return;

  workV6 = workT2V.deviator();  
  A = conHeig * conHeig * (workV6 && workV6);
  B = 2 * conHeig * (t1 && workV6);
  if(fabs(B) < LOW_LIMIT) B = 0.; 
  C = (t1 && t1) - 2./3.* size * size * conHeig * conHeig;
  if( fabs(C) < LOW_LIMIT || fabs(C)/(t1 && t1) < LOW_LIMIT ) return;
  if(B > 0. || C < 0.) {
    std::cerr << "FATAL:XC::PressureDependMultiYieldBase::updateActiveSurface(): error in surface motion.\n" 
         << "A= " <<A <<" B= " <<B <<" C= "<<C <<" (t1&&t1)= "<<(t1&&t1) <<std::endl; 
   exit(-1);
  }
  X = secondOrderEqn(A,B,C,1);  

  center.addVector(1.0, workV6, -X);
  theSurfaces[activeSurfaceNum].setCenter(center);
}      


void XC::PressureDependMultiYieldBase::updateInnerSurface(void) const
{
    double residualPress = residualPressx[matN];

        if(activeSurfaceNum <= 1) return;
        static XC::Vector devia(6);
        static XC::Vector center(6);

        double conHeig = currentStress.volume() - residualPress;
        devia = currentStress.deviator();
        center = theSurfaces[activeSurfaceNum].center();
        double size = theSurfaces[activeSurfaceNum].size();

        for(int i=1; i<activeSurfaceNum; i++) {
                workV6.addVector(0.0, center, conHeig);
                workV6 -= devia;
                workV6 *= theSurfaces[i].size()/size;
                workV6 += devia;

                //workV6 = devia - (devia - center*conHeig) * theSurfaces[i].size() / size;
                
                workV6 /= conHeig;
                theSurfaces[i].setCenter(workV6);
        }
}


int XC::PressureDependMultiYieldBase::isCrossingNextSurface(void) const
{
    int numOfSurfaces = numOfSurfacesx[matN];

  if(activeSurfaceNum == numOfSurfaces) return 0;  

  if(yieldFunc(trialStress, theSurfaces, activeSurfaceNum+1) > 0) return 1;
  
  return 0;
}
