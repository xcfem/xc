//----------------------------------------------------------------------------
//  programa XC; cálculo mediante el método de los elementos finitos orientado
//  a la solución de problemas estructurales.
//
//  Copyright (C)  Luis Claudio Pérez Tato
//
//  El programa deriva del denominado OpenSees <http://opensees.berkeley.edu>
//  desarrollado por el «Pacific earthquake engineering research center».
//
//  Salvo las restricciones que puedan derivarse del copyright del
//  programa original (ver archivo copyright_opensees.txt) este
//  software es libre: usted puede redistribuirlo y/o modificarlo 
//  bajo los términos de la Licencia Pública General GNU publicada 
//  por la Fundación para el Software Libre, ya sea la versión 3 
//  de la Licencia, o (a su elección) cualquier versión posterior.
//
//  Este software se distribuye con la esperanza de que sea útil, pero 
//  SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
//  Consulte los detalles de la Licencia Pública General GNU para obtener 
//  una información más detallada. 
//
// Debería haber recibido una copia de la Licencia Pública General GNU 
// junto a este programa. 
// En caso contrario, consulte <http://www.gnu.org/licenses/>.
//----------------------------------------------------------------------------
// $Revision: 1.30 $
// $Date: 2004/08/27 18:31:17 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/PressureIndependMultiYield.cpp,v $
                                                                        
// Written: ZHY
// Created: August 2000

//
// PressureIndependMultiYield.cpp
// -------------------
//
#include <cmath>
#include <cstdlib>
#include <material/nD/soil/PressureIndependMultiYield.h>
#include <domain/mesh/element/Information.h>
#include <utility/matrix/ID.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/nD/soil/MultiYieldSurface.h>
#include <utility/matrix/Matrix.h>

#include "material/nD/TipoMaterialND.h"

void XC::PressureIndependMultiYield::setup(int nd, double r, double refShearModul, double refBulkModul, double cohesi, double peakShearStra, double frictionAng, double refPress, double pressDependCoe, int numberOfYieldSurf, double * gredu)
  {
    if(refShearModul <= 0)
      {
        std::cerr << "FATAL:XC::PressureIndependMultiYield::PressureIndependMultiYield: refShearModulus <= 0" << std::endl;
        exit(-1);
      }
    if(refBulkModul <= 0)
      {
        std::cerr << "FATAL:XC::PressureIndependMultiYield::PressureIndependMultiYield: refBulkModulus <= 0" << std::endl;
        exit(-1);
      }
    if(frictionAng < 0.)
      {
        std::cerr << "WARNING:XC::PressureIndependMultiYield::PressureIndependMultiYield: frictionAngle < 0" << std::endl;
        std::cerr << "Will reset frictionAngle to zero." << std::endl;
        frictionAng = 0.;
      }
    if(frictionAng == 0. && cohesi <= 0. )
      {
        std::cerr << "FATAL:XC::PressureIndependMultiYield::PressureIndependMultiYield: frictionAngle && cohesion <= 0." << std::endl;
        exit(-1);
      }
    if(pressDependCoe > 0 && frictionAng == 0)
      {
        std::cerr << "WARNING:XC::PressureIndependMultiYield::PressureIndependMultiYield: pressDependCoe > 0 while frictionAngle = 0" << std::endl;
        std::cerr << "Will reset pressDependCoe to zero." << std::endl;
        pressDependCoe = 0.;
      }

    refShearModulus = refShearModul;
    refBulkModulus = refBulkModul;

    setUpSurfaces(gredu);  // residualPress is calculated inside.
  }


XC::PressureIndependMultiYield::PressureIndependMultiYield(int tag, int nd, double r, double refShearModul, double refBulkModul, double cohesi, double peakShearStra, double frictionAng, double refPress, double pressDependCoe, int numberOfYieldSurf, double * gredu)
 : XC::PressureMultiYieldBase(tag,ND_TAG_PressureIndependMultiYield,nd,r,frictionAng,peakShearStra,refPress,pressDependCoe,cohesi,numberOfYieldSurf)
  {
    setup(nd, r, refShearModul, refBulkModul, cohesi, peakShearStra, frictionAng, refPress, pressDependCoe,numberOfYieldSurf,gredu);
  }
   

XC::PressureIndependMultiYield::PressureIndependMultiYield(int tag) 
 : XC::PressureMultiYieldBase(tag,ND_TAG_PressureIndependMultiYield)
  {}

void XC::PressureIndependMultiYield::elast2Plast(void) const
{
  int loadStage = loadStagex[matN];
  double frictionAngle = frictionAnglex[matN];
  int numOfSurfaces = numOfSurfacesx[matN];

  if(loadStage != 1 || e2p == 1) return;
  e2p = 1;

  if(currentStress.volume() > 0. && frictionAngle > 0.) {
    //std::cerr << "WARNING:XC::PressureIndependMultiYield::elast2Plast(): material in tension." << std::endl;
    currentStress.setData(currentStress.deviator(),0);
  }

  paramScaling();  // scale surface parameters corresponding to initial confinement

  // Active surface is 0, return
  if(currentStress.deviatorLength() == 0.) return;

  // Find active surface
  while(yieldFunc(currentStress, committedSurfaces, ++committedActiveSurf) > 0) {
    if(committedActiveSurf == numOfSurfaces) {
      //std::cerr <<"WARNING:XC::PressureIndependMultiYield::elast2Plast(): stress out of failure surface"<<std::endl;
      deviatorScaling(currentStress, committedSurfaces, numOfSurfaces);
      initSurfaceUpdate();
      return;
    }
  } 
  committedActiveSurf--;
  initSurfaceUpdate();
}


int XC::PressureIndependMultiYield::setTrialStrain(const Vector &strain)
{
  int ndm = ndmx[matN];

  static XC::Vector temp(6);
  if(ndm==3 && strain.Size()==6) 
    temp = strain;
  else if(ndm==2 && strain.Size()==3) {
    temp[0] = strain[0];
    temp[1] = strain[1];
    temp[2] = 0.0;
    temp[3] = strain[2];
    temp[4] = 0.0;
    temp[5] = 0.0;
  }
  else {
    std::cerr << "Fatal:XC::D2PressDepMYS:: Material dimension is: " << ndm << std::endl;
    std::cerr << "But strain vector size is: " << strain.Size() << std::endl;
    exit(-1);
  }
        
  //strainRate.setData(temp-currentStrain.t2Vector(1),1);
  temp -= currentStrain.t2Vector(1);
  strainRate.setData(temp, 1);
        
  return 0;
}


int XC::PressureIndependMultiYield::setTrialStrain(const Vector &strain, const XC::Vector &rate)
{
  return setTrialStrain(strain);
}


int XC::PressureIndependMultiYield::setTrialStrainIncr(const XC::Vector &strain)
{
  int ndm = ndmx[matN];

  static XC::Vector temp(6);
  if(ndm==3 && strain.Size()==6) 
    temp = strain;
  else if(ndm==2 && strain.Size()==3) {
    temp[0] = strain[0];
    temp[1] = strain[1];
    temp[3] = strain[2];
  }
  else {
    std::cerr << "Fatal:XC::D2PressDepMYS:: Material dimension is: " << ndm << std::endl;
    std::cerr << "But strain vector size is: " << strain.Size() << std::endl;
    exit(-1);
  }
  
  strainRate.setData(temp,1);
  return 0;
}


int XC::PressureIndependMultiYield::setTrialStrainIncr(const XC::Vector &strain, const XC::Vector &rate)
{
  return setTrialStrainIncr(strain);
}


const XC::Matrix &XC::PressureIndependMultiYield::getTangent(void) const
{
  int loadStage = loadStagex[matN];
  int ndm = ndmx[matN];

  if(loadStage == 1 && e2p == 0) elast2Plast();

  if(loadStage!=1) {  //linear elastic
    for(int i=0;i<6;i++) 
      for(int j=0;j<6;j++) {
        theTangent(i,j) = 0.;
        if(i==j) theTangent(i,j) += refShearModulus;
        if(i<3 && j<3 && i==j) theTangent(i,j) += refShearModulus;
        if(i<3 && j<3) theTangent(i,j) += (refBulkModulus - 2.*refShearModulus/3.);
      }

  }
  else {
    double coeff;
    static XC::Vector devia(6);
    
    /*if(committedActiveSurf > 0) {
      //devia = currentStress.deviator()-committedSurfaces[committedActiveSurf].center();
      devia = currentStress.deviator();
      devia -= committedSurfaces[committedActiveSurf].center();
        
      double size = committedSurfaces[committedActiveSurf].size();
      double plastModul = committedSurfaces[committedActiveSurf].modulus();
      coeff = 6.*refShearModulus*refShearModulus/(2.*refShearModulus+plastModul)/size/size;
    }*/
    if(activeSurfaceNum > 0) {
      //devia = currentStress.deviator()-committedSurfaces[committedActiveSurf].center();
      devia = trialStress.deviator();
      devia -= theSurfaces[activeSurfaceNum].center();
        
      double size = theSurfaces[activeSurfaceNum].size();
      double plastModul = theSurfaces[activeSurfaceNum].modulus();
      coeff = 6.*refShearModulus*refShearModulus/(2.*refShearModulus+plastModul)/size/size;
    }

    else coeff = 0.;
    
    for(int i=0;i<6;i++) 
      for(int j=0;j<6;j++) {
        theTangent(i,j) = - coeff*devia[i]*devia[j];
        if(i==j) theTangent(i,j) += refShearModulus;
        if(i<3 && j<3 && i==j) theTangent(i,j) += refShearModulus;
        if(i<3 && j<3) theTangent(i,j) += (refBulkModulus - 2.*refShearModulus/3.);
      }
  }

  if(ndm==3) 
    return theTangent;
  else {
    static XC::Matrix workM(3,3);
    workM(0,0) = theTangent(0,0);
    workM(0,1) = theTangent(0,1);
    workM(0,2) = theTangent(0,3);
    workM(1,0) = theTangent(1,0);
    workM(1,1) = theTangent(1,1);
    workM(1,2) = theTangent(1,3);
    workM(2,0) = theTangent(3,0);
    workM(2,1) = theTangent(3,1);
    workM(2,2) = theTangent(3,3);
    return workM;
  }
}


const XC::Matrix & XC::PressureIndependMultiYield::getInitialTangent(void) const
{
  int ndm = ndmx[matN];

  for(int i=0;i<6;i++) 
    for(int j=0;j<6;j++) {
      theTangent(i,j) = 0.;
      if(i==j) theTangent(i,j) += refShearModulus;
      if(i<3 && j<3 && i==j) theTangent(i,j) += refShearModulus;
      if(i<3 && j<3) theTangent(i,j) += (refBulkModulus - 2.*refShearModulus/3.);
    }

  if(ndm==3) 
    return theTangent;
  else {
    static XC::Matrix workM(3,3);
    workM(0,0) = theTangent(0,0);
    workM(0,1) = theTangent(0,1);
    workM(0,2) = theTangent(0,3);
    workM(1,0) = theTangent(1,0);
    workM(1,1) = theTangent(1,1);
    workM(1,2) = theTangent(1,3);
    workM(2,0) = theTangent(3,0);
    workM(2,1) = theTangent(3,1);
    workM(2,2) = theTangent(3,3);
    return workM;
  }
}


const XC::Vector & XC::PressureIndependMultiYield::getStress(void) const
{
  int loadStage = loadStagex[matN];
  int numOfSurfaces = numOfSurfacesx[matN];
  int ndm = ndmx[matN];

  int i;
  if(loadStage == 1 && e2p == 0) elast2Plast();

  if(loadStage!=1) {  //linear elastic
    //trialStrain.setData(currentStrain.t2Vector() + strainRate.t2Vector());
    getTangent();
    static XC::Vector a(6);
    a = currentStress.t2Vector();
        a.addMatrixVector(1.0, theTangent, strainRate.t2Vector(1), 1.0);
    trialStress.setData(a);
  }

  else {
    for(i=1; i<=numOfSurfaces; i++) theSurfaces[i] = committedSurfaces[i];
    activeSurfaceNum = committedActiveSurf;
    subStrainRate = strainRate;
    setTrialStress(currentStress);
    if(isLoadReversal()) {
      updateInnerSurface();
      activeSurfaceNum = 0;
    }
    int numSubIncre = setSubStrainRate();
    
    for(i=0; i<numSubIncre; i++) {
      if(i==0)  
        setTrialStress(currentStress);
      else 
        setTrialStress(trialStress);
      if(activeSurfaceNum==0 && !isCrossingNextSurface()) continue;
      if(activeSurfaceNum==0) activeSurfaceNum++;
      stressCorrection(0);
      updateActiveSurface();
    }
    //volume stress change
    double volum = refBulkModulus*(strainRate.volume()*3.);
    volum += currentStress.volume();
     //if(volum > 0) volum = 0.;
    trialStress.setData(trialStress.deviator(),volum);
  }

  if(ndm==3)
    return trialStress.t2Vector();
  else {
    static XC::Vector workV(3);
    workV[0] = trialStress.t2Vector()[0];
    workV[1] = trialStress.t2Vector()[1];
    workV[2] = trialStress.t2Vector()[3];
    return workV;
  }
}


const XC::Vector & XC::PressureIndependMultiYield::getStrain(void) const
  { return getCommittedStrain(); }


int XC::PressureIndependMultiYield::commitState(void)
{
  int loadStage = loadStagex[matN];
  int numOfSurfaces = numOfSurfacesx[matN];

  currentStress = trialStress;
  
  //currentStrain = XC::T2Vector(currentStrain.t2Vector() + strainRate.t2Vector());
  static XC::Vector temp(6);
  temp = currentStrain.t2Vector();
  temp += strainRate.t2Vector();
  currentStrain.setData(temp);
  temp.Zero();
  strainRate.setData(temp);
  
  if(loadStage==1) {
    committedActiveSurf = activeSurfaceNum;
    for(int i=1; i<=numOfSurfaces; i++) committedSurfaces[i] = theSurfaces[i];
  }

  return 0;
}
 

int XC::PressureIndependMultiYield::revertToLastCommit(void)
{
  return 0;
}


XC::NDMaterial *XC::PressureIndependMultiYield::getCopy(void) const
  { return new XC::PressureIndependMultiYield(*this); }

XC::NDMaterial * XC::PressureIndependMultiYield::getCopy(const std::string &code) const
  {
    PressureIndependMultiYield *copy= nullptr;
    if((code==strTipoPressureIndependMultiYield) || (code==strTipoPlaneStrain)
        || (code==strTipoThreeDimensional))
      copy = new XC::PressureIndependMultiYield(*this);
    return copy;
  }


const std::string &XC::PressureIndependMultiYield::getType(void) const
  {
    int ndm = ndmx[matN];
    return (ndm == 2) ? strTipoPlaneStrain : strTipoThreeDimensional;
  }


int XC::PressureIndependMultiYield::getOrder(void) const
{
  int ndm = ndmx[matN];

  return (ndm == 2) ? 3 : 6;
}


int XC::PressureIndependMultiYield::updateParameter(int responseID, Information &info)
{
        if(responseID<10)
                loadStagex[matN] = responseID;

  return 0;
}

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::PressureIndependMultiYield::sendData(CommParameters &cp)
  {
    int res= PressureMultiYieldBase::sendData(cp);
    res+= cp.sendDoubles(refShearModulus,refBulkModulus,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::PressureIndependMultiYield::recvData(const CommParameters &cp)
  {
    int res= PressureMultiYieldBase::recvData(cp);
    res+= cp.receiveDoubles(refShearModulus,refBulkModulus,getDbTagData(),CommMetaData(8));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PressureIndependMultiYield::sendSelf(CommParameters &cp)
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
int XC::PressureIndependMultiYield::recvSelf(const CommParameters &cp)    
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


XC::Response *XC::PressureIndependMultiYield::setResponse(const std::vector<std::string> &argv, Information &matInfo)
  {
    if(argv[0] == "stress" || argv[0] == "stresses")
      return new XC::MaterialResponse(this, 1, this->getCommittedStress());
    else if(argv[0] == "strain" || argv[0] == "strains")
      return new XC::MaterialResponse(this, 2, this->getCommittedStrain());
    else if(argv[0] == "tangent")
      return new XC::MaterialResponse(this, 3, this->getTangent());
    else if(argv[0] == "backbone")
      {
        const int argc= argv.size();
        int numOfSurfaces = numOfSurfacesx[matN];
        static Matrix curv(numOfSurfaces+1,(argc-1)*2);
        for(int i=1; i<argc; i++)
          curv(0,(i-1)*2) = atoi(argv[i]);
        return new XC::MaterialResponse(this, 4, curv);
      }
    else
      return 0;
  }


int XC::PressureIndependMultiYield::getResponse(int responseID, Information &matInfo)
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


void XC::PressureIndependMultiYield::getBackbone(XC::Matrix & bb)
{
  double residualPress = residualPressx[matN];
  double refPressure = refPressurex[matN];
  double pressDependCoeff =pressDependCoeffx[matN];
  int numOfSurfaces = numOfSurfacesx[matN];

  double vol, conHeig, scale, factor, shearModulus, stress1, 
                     stress2= 0.0, strain1, strain2= 0.0, plastModulus, elast_plast, gre;

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
                                stress2 = committedSurfaces[i].size()*factor/sqrt(3.0);
                                strain2 = stress2/shearModulus;
                                bb(1,k*2) = strain2; bb(1,k*2+1) = shearModulus;
                        } else {
                                stress1 = stress2; strain1 = strain2;
                                plastModulus = factor*committedSurfaces[i-1].modulus();
                                elast_plast = 2*shearModulus*plastModulus/(2*shearModulus+plastModulus);
                                stress2 = factor*committedSurfaces[i].size()/sqrt(3.0);
                          strain2 = 2*(stress2-stress1)/elast_plast + strain1;
                                gre = stress2/strain2;
                bb(i,k*2) = strain2; bb(i,k*2+1) = gre;
                        }
                }
        }

}

void XC::PressureIndependMultiYield::Print(std::ostream &s, int flag )
  {  s << strTipoPressureIndependMultiYield << std::endl; }


const XC::Vector & XC::PressureIndependMultiYield::getCommittedStress(void) const
  {
        int ndm = ndmx[matN];
        int numOfSurfaces = numOfSurfacesx[matN];

        double scale = sqrt(3./2.)*currentStress.deviatorLength()/committedSurfaces[numOfSurfaces].size();
        if(loadStagex[matN] != 1) scale = 0.;
        if(ndm==3) {
                static XC::Vector temp7(7), temp6(6);
                temp6 = currentStress.t2Vector();
    temp7[0] = temp6[0];
    temp7[1] = temp6[1];
    temp7[2] = temp6[2];
    temp7[3] = temp6[3];
    temp7[4] = temp6[4];
    temp7[5] = temp6[5];
    temp7[6] = scale;
        return temp7;
        }
  else {
    static XC::Vector temp5(5), temp6(6);
                temp6 = currentStress.t2Vector();
    temp5[0] = temp6[0];
    temp5[1] = temp6[1];
    temp5[2] = temp6[2];
    temp5[3] = temp6[3];
    temp5[4] = scale;
    return temp5;
  }
}


const XC::Vector & XC::PressureIndependMultiYield::getCommittedStrain(void) const
{        
        int ndm = ndmx[matN];

  if(ndm==3)
    return currentStrain.t2Vector(1);
  else {
    static XC::Vector workV(3), temp6(6);
                temp6 = currentStrain.t2Vector(1);
    workV[0] = temp6[0];
    workV[1] = temp6[1];
    workV[2] = temp6[3];
    return workV;
  }
}


// NOTE: surfaces[0] is not used 
void XC::PressureIndependMultiYield::setUpSurfaces(double * gredu)
  { 
    double residualPress = residualPressx[matN];
    double refPressure = refPressurex[matN];
    double pressDependCoeff =pressDependCoeffx[matN];
    int numOfSurfaces = numOfSurfacesx[matN];
    double frictionAngle = frictionAnglex[matN];
    double cohesion = cohesionx[matN];
    double peakShearStrain = peakShearStrainx[matN];

    double  stress1, stress2, strain1, strain2, size= 0.0, elasto_plast_modul, plast_modul;
    double pi = 3.14159265358979;
    double refStrain= 0.0, peakShear= 0.0, coneHeight= 0.0;

    if(gredu == 0)
      {  //automatic generation of surfaces
          if(frictionAngle > 0) {
            double sinPhi = sin(frictionAngle * pi/180.);
            double Mnys = 6.*sinPhi/(3.-sinPhi);
            residualPress = 3.* cohesion / (sqrt(2.) * Mnys);
                  coneHeight = - (refPressure - residualPress);
                  peakShear = sqrt(2.) * coneHeight * Mnys / 3.; 
                  refStrain = (peakShearStrain * peakShear) 
                                    / (refShearModulus * peakShearStrain - peakShear);
                }

          else if(frictionAngle == 0.) { // cohesion = peakShearStrength
                  peakShear = 2*sqrt(2.)*cohesion/3;
                  refStrain = (peakShearStrain * peakShear) 
                                    / (refShearModulus * peakShearStrain - peakShear);
                  residualPress = 0.;
                }
        
          double stressInc = peakShear / numOfSurfaces;

          for(int ii=1; ii<=numOfSurfaces; ii++){
        stress1 = ii * stressInc; 
                                stress2 = stress1 + stressInc;
        strain1 = stress1 * refStrain / (refShearModulus * refStrain - stress1);
        strain2 = stress2 * refStrain / (refShearModulus * refStrain - stress2);
        if(frictionAngle > 0.) size = 3. * stress1 / sqrt(2.) / coneHeight;
        else if(frictionAngle == 0.) size = 3. * stress1 / sqrt(2.);
 
        elasto_plast_modul = 2.*(stress2 - stress1)/(strain2 - strain1);

        if( (2.*refShearModulus - elasto_plast_modul) <= 0) 
                                        plast_modul = UP_LIMIT;
        else 
                                        plast_modul = (2.*refShearModulus * elasto_plast_modul)/
                        (2.*refShearModulus - elasto_plast_modul);
        if(plast_modul < 0) plast_modul = 0;
        if(plast_modul > UP_LIMIT) plast_modul = UP_LIMIT;
        if(ii==numOfSurfaces) plast_modul = 0;

                    static XC::Vector temp(6);
        committedSurfaces[ii] = MultiYieldSurface(temp,size,plast_modul);
                }  // ii
        } 
        else {  //user defined surfaces
                if(frictionAngle > 0) {   // ignore user defined frictionAngle 
                  int ii = 2*(numOfSurfaces-1);
                        double tmax = refShearModulus*gredu[ii]*gredu[ii+1];
                        double Mnys = -(sqrt(3.) * tmax - 2. * cohesion) / refPressure;
                        if(Mnys <= 0) {   // also ignore user defined cohesion
         cohesion = sqrt(3.)/2 * tmax;
         frictionAngle = 0.;  
                     coneHeight = 1.;
                     residualPress = 0.;
      } else {
         double sinPhi = 3*Mnys /(6+Mnys);
               if(sinPhi<0. || sinPhi>1.) {
                                         std::cerr <<"\nPressureMultiYieldBase " <<this->getTag()<<": Invalid friction angle, please modify ref. pressure or G/Gmax curve."<<std::endl;
           exit(-1);
                                 } 
              residualPress = 2. * cohesion / Mnys;
        if(residualPress < 0.01) residualPress = 0.01; 
        coneHeight = - (refPressure - residualPress);
        frictionAngle = asin(sinPhi)*180/pi;
                        }
    }  else if(frictionAngle == 0.) {   // ignore user defined cohesion
                                int ii = 2*(numOfSurfaces-1);
                                double tmax = refShearModulus*gredu[ii]*gredu[ii+1];
        cohesion = sqrt(3.)/2 * tmax;
                                coneHeight = 1.;
                                residualPress = 0.;
                }

                std::cerr << "\nPressureMultiYieldBase " <<this->getTag()<<": Friction angle = "<<frictionAngle
                                                           <<", Cohesion = "<<cohesion<<"\n"<<std::endl;

    if(frictionAngle == 0.) pressDependCoeff = 0.; // ignore user defined pressDependCoeff

                for(int i=1; i<numOfSurfaces; i++) {
                        int ii = 2*(i-1);
                        strain1 = gredu[ii]; 
      stress1 = refShearModulus*gredu[ii+1]*strain1; 
                        strain2 = gredu[ii+2]; 
      stress2 = refShearModulus*gredu[ii+3]*strain2; 

      size = sqrt(3.) * stress1 / coneHeight;
      elasto_plast_modul = 2.*(stress2 - stress1)/(strain2 - strain1);
                        if( (2.*refShearModulus - elasto_plast_modul) <= 0) 
                                        plast_modul = UP_LIMIT;
      else 
                                        plast_modul = (2.*refShearModulus * elasto_plast_modul)/
                        (2.*refShearModulus - elasto_plast_modul);
      if(plast_modul <= 0) {
                                std::cerr << "\nPressureMultiYieldBase " <<this->getTag()<<": Surface " << i 
                                           << " has plastic modulus < 0.\n Please modify G/Gmax curve.\n"<<std::endl;
        exit(-1);
      }
      if(plast_modul > UP_LIMIT) plast_modul = UP_LIMIT;

                  static XC::Vector temp(6);
      committedSurfaces[i] = MultiYieldSurface(temp,size,plast_modul);

                        if(i==(numOfSurfaces-1)) {
                                plast_modul = 0;
                                size = sqrt(3.) * stress2 / coneHeight;
        committedSurfaces[i+1] = MultiYieldSurface(temp,size,plast_modul);
                        }
                }
  }  

  residualPressx[matN] = residualPress;
  frictionAnglex[matN] = frictionAngle;
  cohesionx[matN] = cohesion;
}


double XC::PressureIndependMultiYield::yieldFunc(const XC::T2Vector & stress, const std::vector<MultiYieldSurface> &surfaces, int surfaceNum) const
  {
    static Vector temp(6);
    //temp = stress.deviator() - surfaces[surfaceNum].center();
    temp = stress.deviator();
    temp -= surfaces[surfaceNum].center();

    const double sz = surfaces[surfaceNum].size();
    return 3./2.*(temp && temp) - sz * sz;
  }


void XC::PressureIndependMultiYield::deviatorScaling(T2Vector &stress,const std::vector<MultiYieldSurface> &surfaces, int surfaceNum, int count) const
{
        count++;
        int numOfSurfaces = numOfSurfacesx[matN];
    
        double diff = yieldFunc(stress, surfaces, surfaceNum);

        if( surfaceNum < numOfSurfaces && diff < 0. ) {
                double sz = surfaces[surfaceNum].size();
                double deviaSz = sqrt(sz*sz + diff);
                static XC::Vector devia(6);
                devia = stress.deviator(); 
                static XC::Vector temp(6);
                temp = devia - surfaces[surfaceNum].center();
                double coeff = (sz-deviaSz) / deviaSz;
                if(coeff < 1.e-13) coeff = 1.e-13;
                devia.addVector(1.0, temp, coeff);
                stress.setData(devia, stress.volume());
                deviatorScaling(stress, surfaces, surfaceNum, count);  // recursive call
        }

        if(surfaceNum==numOfSurfaces && fabs(diff) > LOW_LIMIT) {
                double sz = surfaces[surfaceNum].size();
                static XC::Vector newDevia(6);
                newDevia.addVector(0.0, stress.deviator(), sz/sqrt(diff+sz*sz));
                stress.setData(newDevia, stress.volume());
        }
}


void XC::PressureIndependMultiYield::initSurfaceUpdate(void) const
{
        if(committedActiveSurf == 0) return; 

        int numOfSurfaces = numOfSurfacesx[matN];

        static XC::Vector devia(6);
        devia = currentStress.deviator();
        double Ms = sqrt(3./2.*(devia && devia));
        static XC::Vector newCenter(6);

        if(committedActiveSurf < numOfSurfaces) { // failure surface can't move
                //newCenter = devia * (1. - committedSurfaces[activeSurfaceNum].size() / Ms); 
                newCenter.addVector(0.0, devia, 1.0-committedSurfaces[committedActiveSurf].size()/Ms);
                committedSurfaces[committedActiveSurf].setCenter(newCenter);
        }

        for(int i=1; i<committedActiveSurf; i++) {
          newCenter = devia * (1. - committedSurfaces[i].size() / Ms);
          committedSurfaces[i].setCenter(newCenter); 
        }
}


void XC::PressureIndependMultiYield::paramScaling(void) const
{
        int numOfSurfaces = numOfSurfacesx[matN];
        double frictionAngle = frictionAnglex[matN];
    double residualPress = residualPressx[matN];
    double refPressure = refPressurex[matN];
    double pressDependCoeff =pressDependCoeffx[matN];

        if(frictionAngle == 0.) return;

        double conHeig = - (currentStress.volume() - residualPress);
        double scale = -conHeig / (refPressure-residualPress);
           
        scale = pow(scale, pressDependCoeff); 
        refShearModulus *= scale;
           refBulkModulus *= scale;

        double plastModul, size;
        static XC::Vector temp(6);
        for(int i=1; i<=numOfSurfaces; i++) {
          plastModul = committedSurfaces[i].modulus() * scale;
          size = committedSurfaces[i].size() * conHeig;
          committedSurfaces[i] =  MultiYieldSurface(temp,size,plastModul);
        }

}


void XC::PressureIndependMultiYield::setTrialStress(const T2Vector &stress) const
{
  static XC::Vector devia(6);
  //devia = stress.deviator() + subStrainRate.deviator()*2.*refShearModulus;
  devia = stress.deviator();
  devia.addVector(1.0, subStrainRate.deviator(), 2.*refShearModulus);
  
  trialStress.setData(devia, stress.volume());
}


int XC::PressureIndependMultiYield::setSubStrainRate(void) const
{
    int numOfSurfaces = numOfSurfacesx[matN];

        //if(activeSurfaceNum==numOfSurfaces) return 1;

        //if(strainRate==XC::T2Vector()) return 0;
        if(strainRate.isZero()) return 0;

        double elast_plast_modulus;
        if(activeSurfaceNum==0) 
          elast_plast_modulus = 2*refShearModulus;
        else {
          double plast_modulus = theSurfaces[activeSurfaceNum].modulus();
          elast_plast_modulus = 2*refShearModulus*plast_modulus 
            / (2*refShearModulus+plast_modulus);
        }
        static XC::Vector incre(6);
        //incre = strainRate.deviator()*elast_plast_modulus;
        incre.addVector(0.0, strainRate.deviator(),elast_plast_modulus);

        static XC::T2Vector increStress;
        increStress.setData(incre, 0);
        double singleCross = theSurfaces[numOfSurfaces].size() / numOfSurfaces;
        double totalCross = 3.*increStress.octahedralShear() / sqrt(2.);
        int numOfSub = static_cast<int>(totalCross/singleCross + 1);
        if(numOfSub > numOfSurfaces) numOfSub = numOfSurfaces;
        //incre = strainRate.t2Vector() / numOfSub;
        incre = strainRate.t2Vector();
        incre /= numOfSub;
        subStrainRate.setData(incre);

        return numOfSub;
}


void XC::PressureIndependMultiYield::getContactStress(T2Vector &contactStress) const
{
        static XC::Vector center(6);
        center = theSurfaces[activeSurfaceNum].center(); 
        static XC::Vector devia(6);
        //devia = trialStress.deviator() - center;
        devia = trialStress.deviator();
        devia -= center;

        double Ms = sqrt(3./2.*(devia && devia));
        //devia = devia * theSurfaces[activeSurfaceNum].size() / Ms + center;
        devia *= theSurfaces[activeSurfaceNum].size() / Ms;
        devia += center;

        contactStress.setData(devia,trialStress.volume()); 
}


int XC::PressureIndependMultiYield::isLoadReversal(void) const
{
  if(activeSurfaceNum == 0) return 0;

  static XC::Vector surfaceNormal(6);
  getSurfaceNormal(currentStress, surfaceNormal);
 
  //(((trialStress.deviator() - currentStress.deviator()) && surfaceNormal) < 0) 
  // return 1;
  static XC::Vector a(6);
  a = trialStress.deviator();
  a-= currentStress.deviator();
  if((a && surfaceNormal) < 0) 
    return 1;

  return 0;   
}


void XC::PressureIndependMultiYield::getSurfaceNormal(const XC::T2Vector & stress, XC::Vector &surfaceNormal) const
{
  //Q = stress.deviator() - theSurfaces[activeSurfaceNum].center();
  // return Q / sqrt(Q && Q);

  surfaceNormal = stress.deviator();
  surfaceNormal -= theSurfaces[activeSurfaceNum].center();
  surfaceNormal /= sqrt(surfaceNormal && surfaceNormal);
}


double XC::PressureIndependMultiYield::getLoadingFunc(const XC::T2Vector & contactStress, 
                                                                         const XC::Vector & surfaceNormal, int crossedSurface) const
{
  double loadingFunc;
  double temp1 = 2. * refShearModulus ;
  double temp2 = theSurfaces[activeSurfaceNum].modulus();

  //for crossing first surface
  double temp = temp1 + temp2;
  //loadingFunc = (surfaceNormal && (trialStress.deviator()-contactStress.deviator()))/temp;
  static XC::Vector tmp(6);
  tmp =trialStress.deviator();
  tmp -= contactStress.deviator();
  loadingFunc = (surfaceNormal && tmp)/temp;
   //for crossing more than one surface
  if(crossedSurface) {
    double temp3 = theSurfaces[activeSurfaceNum-1].modulus();
    loadingFunc *= (temp3 - temp2)/temp3;
  }

  return loadingFunc;
}


void XC::PressureIndependMultiYield::stressCorrection(int crossedSurface) const
{
        static XC::T2Vector contactStress;
        this->getContactStress(contactStress);
        static XC::Vector surfaceNormal(6);
        this->getSurfaceNormal(contactStress, surfaceNormal);
        double loadingFunc = getLoadingFunc(contactStress, surfaceNormal, crossedSurface);
        static XC::Vector devia(6);

        //devia = trialStress.deviator() - surfaceNormal * 2 * refShearModulus * loadingFunc;
        devia.addVector(0.0, surfaceNormal, -2*refShearModulus*loadingFunc);
        devia += trialStress.deviator();

        trialStress.setData(devia, trialStress.volume());
        deviatorScaling(trialStress, theSurfaces, activeSurfaceNum);

        if(isCrossingNextSurface()) {
                activeSurfaceNum++;
                stressCorrection(1);  //recursive call
        }
}


void XC::PressureIndependMultiYield::updateActiveSurface(void) const
{
  int numOfSurfaces = numOfSurfacesx[matN];

  if(activeSurfaceNum == numOfSurfaces) return;

        double A, B, C, X;
        static XC::T2Vector direction;
        static XC::Vector t1(6);
        static XC::Vector t2(6);
        static XC::Vector temp(6);
        static XC::Vector center(6);
        center = theSurfaces[activeSurfaceNum].center();
        double size = theSurfaces[activeSurfaceNum].size();
        static XC::Vector outcenter(6);
        outcenter= theSurfaces[activeSurfaceNum+1].center();
        double outsize = theSurfaces[activeSurfaceNum+1].size();


        //t1 = trialStress.deviator() - center;
        //t2 = center - outcenter;
        t1 = trialStress.deviator();
        t1 -= center;
        t2 = center;
        t2 -= outcenter;

        A = t1 && t1;
        B = 2. * (t1 && t2);
        C = (t2 && t2) - 2./3.* outsize * outsize;
        X = secondOrderEqn(A,B,C,0);
        if( fabs(X-1.) < LOW_LIMIT ) X = 1.;
        if(X < 1.){
          std::cerr << "FATAL:XC::PressureIndependMultiYield::updateActiveSurface(): error in Direction of surface motion." 
               << std::endl; 
          exit(-1);
        }

        //temp = (t1 * X + center) * (1. - size / outsize) - (center - outcenter * size / outsize);
        temp = center;
        temp.addVector(1.0, t1, X);
        temp *= (1.0 - size/outsize);
        t2 = center;
        t2.addVector(1.0, outcenter, -size/outsize);
        temp -= t2;

        direction.setData(temp);

        if(direction.deviatorLength() < LOW_LIMIT) return;

        temp = direction.deviator();  
        A = temp && temp;
        B = - 2 * (t1 && temp);
        if(fabs(B) < LOW_LIMIT) B = 0.; 
        C = (t1 && t1) - 2./3.* size * size;
        if( fabs(C) < LOW_LIMIT || fabs(C)/(t1 && t1) < LOW_LIMIT ) return;

        if(B > 0. || C < 0.) {
          std::cerr << "FATAL:XC::PressureIndependMultiYield::updateActiveSurface(): error in surface motion.\n" 
               << "A= " <<A <<" B= " <<B <<" C= "<<C <<" (t1&&t1)= "<<(t1&&t1) <<std::endl; 
          exit(-1);
        }
        X = secondOrderEqn(A,B,C,1);  

        //center += temp * X;
        center.addVector(1.0, temp, X);
        theSurfaces[activeSurfaceNum].setCenter(center);
}      


void XC::PressureIndependMultiYield::updateInnerSurface(void) const
{
        if(activeSurfaceNum <= 1) return;

        static XC::Vector devia(6);
        devia = currentStress.deviator();
        static XC::Vector center(6);
        center = theSurfaces[activeSurfaceNum].center();
        double size = theSurfaces[activeSurfaceNum].size();
        static XC::Vector newcenter(6);

        for(int i=1; i<activeSurfaceNum; i++) {
                //newcenter = devia - (devia - center) * theSurfaces[i].size() / size;
                newcenter = center; 
                newcenter -= devia;
                newcenter *= theSurfaces[i].size()/size;
                newcenter += devia;

                theSurfaces[i].setCenter(newcenter);
        }
}


int XC::PressureIndependMultiYield::isCrossingNextSurface(void) const
{
  int numOfSurfaces = numOfSurfacesx[matN];
  if(activeSurfaceNum == numOfSurfaces) return 0;  

  if(yieldFunc(trialStress, theSurfaces, activeSurfaceNum+1) > 0) return 1;
  
  return 0;
}
 
