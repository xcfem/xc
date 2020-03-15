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
// $Revision: 1.34 $

// $Date: 2005/06/17 17:53:39 $

// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/PressureDependMultiYield.cpp,v $

                                                                        

// Written: ZHY

// Created: August 2000



//

// PressureDependMultiYield.cpp

// -------------------

//



#include <cmath>
#include <cstdlib>
#include <material/nD/soil/PressureDependMultiYield.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/matrix/ID.h>
#include <utility/recorder/response/MaterialResponse.h>

#include <material/nD/soil/MultiYieldSurface.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/nDarray/Tensor.h>
#include <utility/matrix/nDarray/stresst.h>
#include "material/nD/NDMaterialType.h"

double* XC::PressureDependMultiYield::liquefyParam4x=0;

const double pi = 3.14159265358979;

XC::PressureDependMultiYield::PressureDependMultiYield(int tag, int nd, 
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
 : XC::PressureDependMultiYieldBase(tag,ND_TAG_PressureDependMultiYield),
   reversalStress(), lockStress(), reversalStressCommitted(), 
   lockStressCommitted()
  {

  if(volLim1 < 0) {
    std::cerr << "WARNING:XC::PressureDependMultiYield:: volLim1 < 0" << std::endl;
    std::cerr << "Will reset volLimit to 0.8" << std::endl;
    volLim1 = 0.8;
  }

    if(matCount%20 == 0)
      {
        double *temp1 = liquefyParam4x;
        double *temp2 = volLimit1x;
        double *temp3 = volLimit2x;
        double *temp4 = volLimit3x;


        liquefyParam4x = new double[matCount+20];
        volLimit1x = new double[matCount+20];
        volLimit2x = new double[matCount+20];
        volLimit3x = new double[matCount+20];

        for(int i=0; i<matCount; i++)
          {
            liquefyParam4x[i] = temp1[i];
            volLimit1x[i] = temp2[i];
            volLimit2x[i] = temp3[i];
            volLimit3x[i] = temp4[i];
          }
     
         if(matCount > 0)
           {
             delete [] temp1; delete [] temp2; delete [] temp3; delete [] temp4; 
           }
      }
    volLimit1x[matCount] = volLim1;
    volLimit2x[matCount] = volLim2;
    volLimit3x[matCount] = volLim3;
    liquefyParam4x[matCount] = liquefactionParam4;

    setUpSurfaces(gredu);  // residualPress and stressRatioPT are calculated inside.
  }
   
XC::PressureDependMultiYield::PressureDependMultiYield(int tag) 
 : XC::PressureDependMultiYieldBase(tag,ND_TAG_PressureDependMultiYield), 
   reversalStress(), lockStress(), reversalStressCommitted(), 
   lockStressCommitted()
  {}

XC::PressureDependMultiYield::PressureDependMultiYield() 
 : XC::PressureDependMultiYieldBase(0,ND_TAG_PressureDependMultiYield), 
   reversalStress(), lockStress(), reversalStressCommitted(), 
   lockStressCommitted()
  {}


XC::PressureDependMultiYield::PressureDependMultiYield(const XC::PressureDependMultiYield & a)
 : XC::PressureDependMultiYieldBase(a), 
   reversalStress(a.reversalStress),
   lockStress(a.lockStress), reversalStressCommitted(a.reversalStressCommitted), 
   lockStressCommitted(a.lockStressCommitted)
  {}


const XC::Matrix & XC::PressureDependMultiYield::getTangent(void) const
{
  int loadStage = loadStagex[matN];
  double refShearModulus = refShearModulusx[matN];
  double refBulkModulus = refBulkModulusx[matN];
  double pressDependCoeff = pressDependCoeffx[matN];
  double refPressure = refPressurex[matN];
  double residualPress = residualPressx[matN];
  int ndm = ndmx[matN];

  if(loadStage == 1 && e2p == 0) elast2Plast();
  if(loadStage==2 && initPress==refPressure) 
          initPress = currentStress.volume();

  if(loadStage==0 || loadStage==2) {  //linear elastic
        double factor;
        if(loadStage==0) factor = 1.0;
        else {
                factor = (initPress-residualPress)/(refPressure-residualPress);
                if(factor <= 1.e-10) factor = 1.e-10;
                else factor = pow(factor, pressDependCoeff);
                factor = (1.e-10>factor) ? 1.e-10 : factor;
        }
    for(int i=0;i<6;i++) 
      for(int j=0;j<6;j++) {
            theTangent(i,j) = 0.;
        if(i==j) theTangent(i,j) += refShearModulus*factor;
        if(i<3 && j<3 && i==j) theTangent(i,j) += refShearModulus*factor;
        if(i<3 && j<3) theTangent(i,j) += (refBulkModulus - 2.*refShearModulus/3.)*factor;
      }
  }
  else {
    double coeff1, coeff2, coeff3, coeff4;
    double factor = getModulusFactor(currentStress);
    double shearModulus = factor*refShearModulus;
    double bulkModulus = factor*refBulkModulus;                

        // volumetric plasticity
        if(Hvx[matN] != 0. && trialStress.volume()<=maxPress && strainRate.volume()<0.) {
          double tp = fabs(trialStress.volume() - residualPress);
      bulkModulus = (bulkModulus*Hvx[matN]*pow(tp,Pvx[matN]))/(bulkModulus+Hvx[matN]*pow(tp,Pvx[matN]));
        }
        
    if(loadStage!=0 && committedActiveSurf > 0) {
      getSurfaceNormal(currentStress, workT2V);
      workV6 = workT2V.deviator();
      double volume = workT2V.volume();
      double Ho = 9.*bulkModulus*volume*volume + 2.*shearModulus*(workV6 && workV6);
      double plastModul = factor*committedSurfaces[committedActiveSurf].modulus();
      coeff1 = 9.*bulkModulus*bulkModulus*volume*volume/(Ho+plastModul);
      coeff2 = 4.*shearModulus*shearModulus/(Ho+plastModul); 
      /* non-symmetric stiffness
      getSurfaceNormal(currentStress, workT2V);
      workV6 = workT2V.deviator();
      double qq = workT2V.volume();
                        double XC::pp=getPlasticPotential(currentStress, workT2V); 
      double Ho = 9.*bulkModulus*pp*qq + 2.*shearModulus*(workV6 && workV6);
      double plastModul = factor*committedSurfaces[committedActiveSurf].modulus();
      coeff1 = 9.*bulkModulus*bulkModulus*pp*qq/(Ho+plastModul);
      coeff2 = 4.*shearModulus*shearModulus/(Ho+plastModul);
                        coeff3 = 6.*shearModulus*pp/(Ho+plastModul);
                        coeff4 = 6.*shearModulus*qq/(Ho+plastModul);*/

          }
    else {
      coeff1 = coeff2 = coeff3 = coeff4 = 0.;
      workV6.Zero();
    }

    for(int i=0;i<6;i++) 
      for(int j=0;j<6;j++) {
              theTangent(i,j) = - coeff2*workV6[i]*workV6[j];
        if(i==j) theTangent(i,j) += shearModulus;
        if(i<3 && j<3 && i==j) theTangent(i,j) += shearModulus;
              if(i<3 && j<3) theTangent(i,j) += (bulkModulus - 2.*shearModulus/3. - coeff1);
/* non-symmetric stiffness
                                if(i<3) theTangent(i,j) -= coeff3 * workV6[j];
                                if(j<3) theTangent(i,j) -= coeff4 * workV6[i];*/
      }
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


const XC::Vector & XC::PressureDependMultiYield::getStress(void) const
{
  int loadStage = loadStagex[matN];
  int numOfSurfaces = numOfSurfacesx[matN];
  int ndm = ndmx[matN];

  int i, is;
  if(loadStage == 1 && e2p == 0) 
    elast2Plast();

  if(loadStage!=1) {  //linear elastic
    //trialStrain.setData(currentStrain.t2Vector() + strainRate.t2Vector());
    getTangent();
    workV6 = currentStress.t2Vector();
        workV6.addMatrixVector(1.0, theTangent, strainRate.t2Vector(1), 1.0);
    trialStress.setData(workV6);
  }
  else {
    for(i=1; i<=numOfSurfaces; i++) theSurfaces[i] = committedSurfaces[i];
    activeSurfaceNum = committedActiveSurf;
    pressureD = pressureDCommitted;
    reversalStress = reversalStressCommitted;
    onPPZ = onPPZCommitted;
    PPZSize = PPZSizeCommitted;
    cumuDilateStrainOcta = cumuDilateStrainOctaCommitted;
    maxCumuDilateStrainOcta = maxCumuDilateStrainOctaCommitted;
    cumuTranslateStrainOcta = cumuTranslateStrainOctaCommitted;
    prePPZStrainOcta = prePPZStrainOctaCommitted;
    oppoPrePPZStrainOcta = oppoPrePPZStrainOctaCommitted;
    PPZPivot = PPZPivotCommitted;
    PPZCenter = PPZCenterCommitted;
    lockStress = lockStressCommitted;

    subStrainRate = strainRate;
    setTrialStress(currentStress);
    if(activeSurfaceNum>0 && isLoadReversal(currentStress)) {
      updateInnerSurface();
      activeSurfaceNum = 0;
    }

    if(activeSurfaceNum==0 && !isCrossingNextSurface()) {
             workV6 = currentStrain.t2Vector();
            workV6.addVector(1.0, strainRate.t2Vector(), 1.0);
            trialStrain.setData(workV6);
                }
                else {
      int numSubIncre = setSubStrainRate();

      for(i=0; i<numSubIncre; i++) {
//      trialStrain.setData(currentStrain.t2Vector() 
//                             + subStrainRate.t2Vector()*(i+1));
               workV6 = currentStrain.t2Vector();
              workV6.addVector(1.0, subStrainRate.t2Vector(), (i+1));
              trialStrain.setData(workV6);

                if(i==0)  {
                          setTrialStress(currentStress);
              is = isLoadReversal(currentStress);
                }  
                else {
                        workT2V.setData(trialStress.t2Vector());
                        setTrialStress(trialStress); 
            is = isLoadReversal(workT2V);
                }
        if(activeSurfaceNum>0 && is) {
          updateInnerSurface();
          activeSurfaceNum = 0;
                }
        if(activeSurfaceNum==0 && !isCrossingNextSurface()) continue;
        if(activeSurfaceNum==0) activeSurfaceNum++;
        int lock = stressCorrection(0);
        if(lock==0) updateActiveSurface();
                //std::cerr<<i<<" "<<activeSurfaceNum<<" "<<is<<" "<<subStrainRate.t2Vector()[3]<<std::endl;
                }
    }
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

XC::NDMaterial *XC::PressureDependMultiYield::getCopy(void) const
  { return new PressureDependMultiYield(*this); }

XC::NDMaterial * XC::PressureDependMultiYield::getCopy(const std::string &code) const
  {
    PressureDependMultiYield *copy= nullptr;
    if((code==strTypePlaneStrain) || (code==strTypeThreeDimensional))
      copy = new PressureDependMultiYield(*this);
    return copy;
  }


const std::string &XC::PressureDependMultiYield::getType(void) const
  {
    int ndm = ndmx[matN];
    return (ndm == 2) ? strTypePlaneStrain : strTypeThreeDimensional;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::PressureDependMultiYield::sendData(CommParameters &cp)
  {
    int res= PressureDependMultiYieldBase::sendData(cp);
    res+= cp.sendMovable(reversalStress,getDbTagData(),CommMetaData(20));
    res+= cp.sendMovable(lockStress,getDbTagData(),CommMetaData(21));
    res+= cp.sendMovable(reversalStressCommitted,getDbTagData(),CommMetaData(22));
    res+= cp.sendMovable(lockStressCommitted,getDbTagData(),CommMetaData(23));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::PressureDependMultiYield::recvData(const CommParameters &cp)
  {
    int res= PressureDependMultiYieldBase::recvData(cp);
    res+= cp.receiveMovable(reversalStress,getDbTagData(),CommMetaData(20));
    res+= cp.receiveMovable(lockStress,getDbTagData(),CommMetaData(21));
    res+= cp.receiveMovable(reversalStressCommitted,getDbTagData(),CommMetaData(22));
    res+= cp.receiveMovable(lockStressCommitted,getDbTagData(),CommMetaData(23));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::PressureDependMultiYield::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(24);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::PressureDependMultiYield::recvSelf(const CommParameters &cp)    
  {
    inicComm(24);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::recvSelf - failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << getClassName() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::PressureDependMultiYield::Print(std::ostream &s, int flag ) const
{
  s << "PressureDependMultiYield" << std::endl;
}





// NOTE: surfaces[0] is not used 
void XC::PressureDependMultiYield::setUpSurfaces(double * gredu)
{ 
    double residualPress = residualPressx[matN];
    double refPressure = refPressurex[matN];
    //double pressDependCoeff= pressDependCoeffx[matN];
    double refShearModulus = refShearModulusx[matN];
    int numOfSurfaces = numOfSurfacesx[matN];
    double frictionAngle = frictionAnglex[matN];
        double cohesion = cohesionx[matN];
    double peakShearStrain = peakShearStrainx[matN];
    double phaseTransfAngle = phaseTransfAnglex[matN];
        double stressRatioPT = stressRatioPTx[matN];

    double refStrain, peakShear, coneHeight;
    double stress1, stress2, strain1, strain2, size, elasto_plast_modul, plast_modul;
    double ratio1, ratio2;
  
        if(gredu==0) {
          double sinPhi = sin(frictionAngle * pi/180.);
    double Mnys = 6.*sinPhi/(3.-sinPhi);
    double sinPhiPT = sin(phaseTransfAngle * pi/180.);
    stressRatioPT = 6.*sinPhiPT/(3.-sinPhiPT);
                // tao = cohesion * sqrt(8.0)/3.
    residualPress = 2 * cohesion / Mnys;
    // a small nonzero residualPress for numerical purpose only
    if(residualPress < 0.01) residualPress = 0.01; 
    coneHeight = - (refPressure - residualPress);
    peakShear = sqrt(2.) * coneHeight * Mnys / 3.; 
    refStrain = (peakShearStrain * peakShear) 
                                  / (refShearModulus * peakShearStrain - peakShear);

    double stressInc = peakShear / numOfSurfaces;

    for(int ii=1; ii<=numOfSurfaces; ii++){
      stress1 = ii * stressInc; 
      stress2 = stress1 + stressInc;
      ratio1 = 3. * stress1 / sqrt(2.) / coneHeight;
      ratio2 = 3. * stress2 / sqrt(2.) / coneHeight;
      strain1 = stress1 * refStrain / (refShearModulus * refStrain - stress1);
      strain2 = stress2 * refStrain / (refShearModulus * refStrain - stress2);
    
      if(ratio1 <= stressRatioPT && ratio2 >= stressRatioPT) {
        double ratio = (ratio2 - stressRatioPT)/(ratio2 - ratio1);
        strainPTOcta = strain2 - ratio * (strain2 - strain1);
                        }

      size = ratio1;
      elasto_plast_modul = 2.*(stress2 - stress1)/(strain2 - strain1);
      if( (2.*refShearModulus - elasto_plast_modul) <= 0) 
        plast_modul = UP_LIMIT;
      else 
        plast_modul = (2.*refShearModulus * elasto_plast_modul)/
                            (2.*refShearModulus - elasto_plast_modul);
      if(plast_modul < 0) plast_modul = 0;
      if(plast_modul > UP_LIMIT) plast_modul = UP_LIMIT;
      if(ii==numOfSurfaces) plast_modul = 0;
      workV6.Zero();
          //std::cerr<<size<<std::endl;
      committedSurfaces[ii] = MultiYieldSurface(workV6,size,plast_modul);
                }  // ii  
        } 
        else {  //user defined surfaces   
                int ii = 2*(numOfSurfaces-1);
                double tmax = refShearModulus*gredu[ii]*gredu[ii+1];
                double Mnys = -(sqrt(3.) * tmax - 2.* cohesion) / refPressure;
    residualPress = 2 * cohesion / Mnys;
    if(residualPress < 0.01) residualPress = 0.01; 
    coneHeight = - (refPressure - residualPress);

    double sinPhi = 3*Mnys /(6+Mnys);
                if(sinPhi<0. || sinPhi>1.) {
                        std::cerr <<"\nPressureDependMultiYieldBase " <<this->getTag()<<": Invalid friction angle, please modify ref. pressure or G/Gmax curve."<<std::endl;
     exit(-1);
                } 

                frictionAngle = asin(sinPhi)*180/pi;
                std::cerr << "\nPressureDependMultiYieldBase " <<this->getTag()<<": Friction angle is "<<frictionAngle<<"\n"<<std::endl;
    if(phaseTransfAngle > frictionAngle) {
                        std::cerr << "\nPressureDependMultiYieldBase " <<this->getTag()<<": phase Transformation Angle > friction Angle," 
                                   << "will set phase Transformation Angle = friction Angle.\n" <<std::endl;
                        phaseTransfAngle = frictionAngle;
                }
                double sinPhiPT = sin(phaseTransfAngle * pi/180.);
    stressRatioPT = 6.*sinPhiPT/(3.-sinPhiPT);

                for(int i=1; i<numOfSurfaces; i++) {
                        int ii = 2*(i-1);
                        strain1 = gredu[ii]; 
      stress1 = refShearModulus*gredu[ii+1]*strain1; 
                        strain2 = gredu[ii+2]; 
      stress2 = refShearModulus*gredu[ii+3]*strain2; 

      ratio1 = sqrt(3.) * stress1 / coneHeight;
      ratio2 = sqrt(3.) * stress2 / coneHeight;  
      if(ratio1 <= stressRatioPT && ratio2 >= stressRatioPT) {
        double ratio = (ratio2 - stressRatioPT)/(ratio2 - ratio1);
                          // gamma_oct = sqrt(6.0)/3*gamma12
        strainPTOcta = sqrt(6.)/3 * (strain2 - ratio * (strain2 - strain1));
                        }

      size = ratio1;
      elasto_plast_modul = 2.*(stress2 - stress1)/(strain2 - strain1);
        
                        if( (2.*refShearModulus - elasto_plast_modul) <= 0) 
                                        plast_modul = UP_LIMIT;
      else 
                                        plast_modul = (2.*refShearModulus * elasto_plast_modul)/
                        (2.*refShearModulus - elasto_plast_modul);
      if(plast_modul <= 0) {
                                std::cerr << "\nPressureDependMultiYieldBase " <<this->getTag()<<": Surface " << i 
                                           << " has plastic modulus < 0.\n Please modify G/Gmax curve.\n"<<std::endl;
       exit(-1);
      }
      if(plast_modul > UP_LIMIT) plast_modul = UP_LIMIT;

      workV6.Zero();
                        //std::cerr<<size<<" "<<i<<" "<<plast_modul<<" "<<gredu[ii]<<" "<<gredu[ii+1]<<std::endl;
      committedSurfaces[i] = MultiYieldSurface(workV6,size,plast_modul);

                        if(i==(numOfSurfaces-1)) {
                                plast_modul = 0;
                                size = ratio2;
                          //std::cerr<<size<<" "<<i+1<<" "<<plast_modul<<" "<<gredu[ii+2]<<" "<<gredu[ii+3]<<std::endl;
        committedSurfaces[i+1] = MultiYieldSurface(workV6,size,plast_modul);
                        }
                }
  }  

  residualPressx[matN] = residualPress;
  frictionAnglex[matN] = frictionAngle;
  cohesionx[matN] = cohesion;
  phaseTransfAnglex[matN] = phaseTransfAngle;
  stressRatioPTx[matN] = stressRatioPT;
}





void XC::PressureDependMultiYield::initStrainUpdate(void)
{
    double residualPress = residualPressx[matN];
    double refPressure = refPressurex[matN];
    double pressDependCoeff =pressDependCoeffx[matN];
    double refShearModulus = refShearModulusx[matN];
        double refBulkModulus = refBulkModulusx[matN];
    double stressRatioPT = stressRatioPTx[matN];

  // elastic strain state
  double stressRatio = currentStress.deviatorRatio(residualPress);
  double ratio = (-currentStress.volume()+residualPress)/(-refPressure+residualPress);
  ratio = pow(ratio, 1.-pressDependCoeff);
  modulusFactor = getModulusFactor(currentStress);
  double shearCoeff = 1./(2.*refShearModulus*modulusFactor);
  double bulkCoeff = 1./(3.*refBulkModulus*modulusFactor);

  //currentStrain = currentStress.deviator()*shearCoeff
  //              + currentStress.volume()*bulkCoeff;

  // modified fmk as discussed with z.yang
  workV6.addVector(0.0, currentStress.deviator(), shearCoeff);
  currentStrain.setData(workV6, currentStress.volume()*bulkCoeff);
        
  double octalStrain = currentStrain.octahedralShear(1);
  if(octalStrain <= LOW_LIMIT) octalStrain = LOW_LIMIT;

  // plastic strain state(scaled from elastic strain)
  double scale, PPZLimit;
  if(stressRatio >= stressRatioPT) {  //above PT
    onPPZ = 2;
    prePPZStrainOcta = strainPTOcta * ratio;
    PPZLimit = getPPZLimits(1,currentStress);
    scale = sqrt(prePPZStrainOcta+PPZLimit)/octalStrain;
  }
  else {  // below PT
    onPPZ = -1;
    prePPZStrainOcta = octalStrain;
    if(prePPZStrainOcta > strainPTOcta * ratio) prePPZStrainOcta=strainPTOcta*ratio;
    scale = sqrt(prePPZStrainOcta)/octalStrain;
  }
  //currentStrain.setData(currentStrain.deviator()*scale, currentStrain.volume());
  workV6.addVector(0.0, currentStrain.deviator(), scale);
  currentStrain.setData(workV6, currentStrain.volume());
  PPZPivot = currentStrain;
}

double XC::PressureDependMultiYield::getPlasticPotential(const XC::T2Vector & contactStress,
                                                     const XC::T2Vector & surfaceNormal) const
  {
    double residualPress = residualPressx[matN];
    double stressRatioPT = stressRatioPTx[matN];
    int numOfSurfaces = numOfSurfacesx[matN];
        double contractParam1 = contractParam1x[matN];
    double dilateParam1 = dilateParam1x[matN];
    double dilateParam2 = dilateParam2x[matN];

  double plasticPotential, contractRule, unloadRule, dilateRule, shearLoading, temp;

  double contactRatio = contactStress.deviatorRatio(residualPress);
  temp = contactRatio/stressRatioPT;
  double factorPT = (temp*temp - 1)/(temp*temp + 1)/3.;
  //double volume = contactStress.volume();
  contractRule = factorPT*contractParam1;
  if(contractRule > 0.) contractRule = -contractRule;
        if(contractRule<-5.0e4) contractRule = -5.0e4;
  temp = currentStress.volume() - pressureD;
  if(temp >= 0.) unloadRule = 0.;
  else {
    double conHeiD = pressureD-residualPress;
    double temp1 = sqrt(3./2.)*currentStress.deviatorLength() 
      + stressRatioPT*conHeiD;
    temp = temp1/(-temp);
    if(temp < theSurfaces[numOfSurfaces].size()) 
      temp = theSurfaces[numOfSurfaces].size();
    temp1 = (reversalStress.volume()-residualPress)/conHeiD;
    unloadRule = -sqrt(3./2.)*surfaceNormal.deviatorLength()*temp1/temp;
  }
          
  double currentRatio = currentStress.deviatorRatio(residualPress);
  double trialRatio = trialStress.deviatorRatio(residualPress);
  shearLoading = currentStress.deviator() && trialStress.deviator();

  if(factorPT < 0.) {  //below PT
    if(pressureD == 0.) 
      plasticPotential = contractRule;
    else if(trialStress.volume() >= pressureD) {
      pressureD = 0.;
      plasticPotential = contractRule;
    }
    else if(trialRatio > currentRatio && shearLoading >= 0.)
      plasticPotential = contractRule;
    else  plasticPotential = unloadRule;
  }
  
  else {  //above PT
    if(trialRatio > currentRatio && shearLoading >= 0.) {  //dilation
      if(pressureD == 0.) pressureD = currentStress.volume();
      reversalStress = currentStress;
      updatePPZ(contactStress);  
      if(onPPZ==-1 || onPPZ==1) return LOCK_VALUE; 
      if(isCriticalState(contactStress))  
              dilateRule = 0;
      else
        dilateRule = factorPT*dilateParam1*exp(dilateParam2*cumuDilateStrainOcta);
                        if(dilateRule>5.0e4) dilateRule = 5.0e4;
      return dilateRule;
    }
    else if(pressureD == 0.) plasticPotential = contractRule;
    else if(trialStress.volume() >= pressureD) {
      pressureD = 0.;
      plasticPotential = contractRule;
    }
    else plasticPotential = unloadRule;
  }

  if(onPPZ > 0) onPPZ = 0;
  if(onPPZ != -1) PPZTranslation(contactStress);  
  if(isCriticalState(contactStress)) return 0.;
  return plasticPotential;
}


int XC::PressureDependMultiYield::isCriticalState(const XC::T2Vector & stress) const
  {
        double einit = einitx[matN];
        double volLimit1 = volLimit1x[matN];
        double volLimit2 = volLimit2x[matN];
        double volLimit3 = volLimit3x[matN];

  double vol = trialStrain.volume()*3.0;
        double etria = einit + vol + vol*einit;
        vol = currentStrain.volume()*3.0;
        double ecurr = einit + vol + vol*einit;
 
        double ecr1, ecr2;
        if(volLimit3 != 0.)
          {
            ecr1 = volLimit1 - volLimit2*pow(std::abs(-stress.volume()/pAtm), volLimit3);
            ecr2 = volLimit1 - volLimit2*pow(std::abs(-currentStress.volume()/pAtm), volLimit3);
        } else {
	  ecr1 = volLimit1 - volLimit2*log(std::abs(-stress.volume()/pAtm));
          ecr2 = volLimit1 - volLimit2*log(std::abs(-currentStress.volume()/pAtm));
  }

        if(ecurr < ecr2 && etria < ecr1) return 0;
        if(ecurr > ecr2 && etria > ecr1) return 0;        
        return 1;
}


void XC::PressureDependMultiYield::updatePPZ(const XC::T2Vector & contactStress) const
{
  double liquefyParam1 = liquefyParam1x[matN];
  double residualPress = residualPressx[matN];
  double refPressure = refPressurex[matN];
  double pressDependCoeff =pressDependCoeffx[matN];

  // PPZ inactive if liquefyParam1==0.
  if(liquefyParam1==0.) {
    if(onPPZ==2) {
                  workT2V.setData(trialStrain.t2Vector() - PPZPivot.t2Vector());
      cumuDilateStrainOcta = workT2V.octahedralShear(1);
    }
    else if(onPPZ != 2) {
      onPPZ = 2;
      PPZPivot = trialStrain;
      cumuDilateStrainOcta = 0.;
    }
    return;
  }

  // dilation: calc. cumulated dilative strain
  if(onPPZ==2) {
    PPZPivot = trialStrain;
    workV6 = PPZPivot.t2Vector();
    workV6 -= PPZCenter.t2Vector();
    workT2V.setData(workV6);
    //cumuDilateStrainOcta = workT2V.octahedralShear(1) - PPZSize;
                cumuDilateStrainOcta += subStrainRate.octahedralShear(1);
    if(cumuDilateStrainOcta > maxCumuDilateStrainOcta) 
      maxCumuDilateStrainOcta = cumuDilateStrainOcta;
    return;
  }

  // calc. PPZ size.
  double PPZLimit = getPPZLimits(1,contactStress);
        double TransLimit = getPPZLimits(2,contactStress);
        //if(PPZLimit==0.) return;

  if(onPPZ==-1 || onPPZ==0) {
    workV6 = trialStrain.t2Vector();
    workV6 -= PPZPivot.t2Vector();
    workT2V.setData(workV6);
                double temp = workT2V.octahedralShear(1);
                if(temp > cumuDilateStrainOcta) {
      double volume = -contactStress.volume();
      oppoPrePPZStrainOcta = prePPZStrainOcta;
      double ratio = (volume+residualPress)/(-refPressure+residualPress);
      ratio = pow(ratio, 1.-pressDependCoeff);
      prePPZStrainOcta = ratio * strainPTOcta;
      if(oppoPrePPZStrainOcta == 0.) oppoPrePPZStrainOcta = prePPZStrainOcta;
                }
  }
  if(onPPZ > -1) PPZSize = PPZLimit 
    + (prePPZStrainOcta+oppoPrePPZStrainOcta+TransLimit+maxCumuDilateStrainOcta)/2.;
        else PPZSize = PPZLimit 
    + (prePPZStrainOcta+oppoPrePPZStrainOcta+maxCumuDilateStrainOcta)/2.;

  // calc. new PPZ center.
  if(onPPZ==0 || onPPZ==1) { 
    //workT2V.setData(PPZPivot.t2Vector() - PPZCenter.t2Vector());
    workV6 = PPZPivot.t2Vector();
    workV6 -= PPZCenter.t2Vector();
    workT2V.setData(workV6);
    
    double coeff = (PPZSize-cumuTranslateStrainOcta)/workT2V.octahedralShear(1);
    //PPZCenter.setData(PPZPivot.t2Vector() - workT2V.t2Vector()*coeff);
    workV6 = PPZPivot.t2Vector();
    workV6.addVector(1.0, workT2V.t2Vector(), -coeff);
    PPZCenter.setData(workV6);
  }

  //workT2V.setData(trialStrain.t2Vector() - PPZCenter.t2Vector());
  workV6 = trialStrain.t2Vector();
  workV6 -= PPZCenter.t2Vector();
  workT2V.setData(workV6);
  double temp = subStrainRate.t2Vector() && workV6;

  if((workT2V.octahedralShear(1) > PPZSize) && (temp > 0. || PPZLimit==0.)) {  //outside PPZ
    workV6 = trialStrain.t2Vector();
    workV6 -= PPZPivot.t2Vector();
    workT2V.setData(workV6);
                double temp1 = workT2V.octahedralShear(1);
                if(temp1 > cumuDilateStrainOcta) {
      cumuDilateStrainOcta = 0.;
      if(PPZLimit == 0.) maxCumuDilateStrainOcta = 0.;
                }
    onPPZ = 2;
    PPZPivot = trialStrain;
    cumuTranslateStrainOcta = 0.;
  }
  else {  //inside PPZ
    if(onPPZ == 0 || onPPZ == 1) PPZTranslation(contactStress);
    if(onPPZ == -1 || onPPZ == 0) lockStress = contactStress;
    if(onPPZ == 0) onPPZ = 1;
  }
}


void XC::PressureDependMultiYield::PPZTranslation(const XC::T2Vector & contactStress) const
{
        double liquefyParam1 = liquefyParam1x[matN];

  if(liquefyParam1==0.) return;

  double PPZLimit = getPPZLimits(1,contactStress);
        if(PPZLimit==0.) return;

  double PPZTransLimit = getPPZLimits(2,contactStress);

  //workT2V.setData(trialStrain.deviator()-PPZPivot.deviator());
  workV6 = trialStrain.deviator();
  workV6 -= PPZPivot.deviator();
  workT2V.setData(workV6);
  
  double temp = workT2V.octahedralShear(1);
  if(cumuTranslateStrainOcta < temp) cumuTranslateStrainOcta = temp;
        if(maxCumuDilateStrainOcta == 0.) temp = PPZTransLimit;
        else temp = PPZTransLimit*cumuDilateStrainOcta/maxCumuDilateStrainOcta;
  if(cumuTranslateStrainOcta > temp) cumuTranslateStrainOcta = temp;
}


double XC::PressureDependMultiYield::getPPZLimits(int which, const XC::T2Vector & contactStress) const
{
        double liquefyParam1 = liquefyParam1x[matN];
        double liquefyParam2 = liquefyParam2x[matN];
        double liquefyParam4 = liquefyParam4x[matN];

  double PPZLimit, temp;
  double volume = -contactStress.volume();

  if(volume >= liquefyParam1) PPZLimit = 0.;
  else {
    temp = volume*pi/liquefyParam1/2.;
                // liquefyParam3 = 3.0 default
    PPZLimit = liquefyParam2 * pow(cos(temp), 3.);
  }
  
  if(which==1) 
    return PPZLimit;
  else if(which==2) 
    return liquefyParam4 * PPZLimit;
  else {
    std::cerr << "FATAL:XC::PressureDependMultiYield::getPPZLimits: unknown argument value" << std::endl;
   exit(-1);
    return 0.0;
  }
}


int XC::PressureDependMultiYield::stressCorrection(int crossedSurface) const
{
    double refShearModulus = refShearModulusx[matN];
        double refBulkModulus = refBulkModulusx[matN];

  static XC::T2Vector contactStress;
  getContactStress(contactStress);
  static XC::T2Vector surfNormal;
  getSurfaceNormal(contactStress, surfNormal);
  double plasticPotential = getPlasticPotential(contactStress,surfNormal);
  if(plasticPotential==LOCK_VALUE && (onPPZ == -1 || onPPZ == 1)) {
    trialStress = lockStress;
    return 1;
  }

        double tVolume = trialStress.volume();
  double loadingFunc = getLoadingFunc(contactStress, surfNormal, 
                                      plasticPotential, crossedSurface);
  double volume = tVolume - plasticPotential*3*refBulkModulus*modulusFactor*loadingFunc;

  //workV6 = trialStress.deviator() 
  //                 - surfNormal.deviator()*2*refShearModulus*modulusFactor*loadingFunc;
  workV6 = trialStress.deviator();
        
  if(volume > 0. && volume != tVolume) {
                double coeff = tVolume / (tVolume - volume);
    coeff *= -2*refShearModulus*modulusFactor*loadingFunc;
    workV6.addVector(1.0, surfNormal.deviator(), coeff);
                volume = 0.;
  } else if(volume > 0.) {
                volume = 0.;
        } else {
                double coeff = -2*refShearModulus*modulusFactor*loadingFunc;
    workV6.addVector(1.0, surfNormal.deviator(), coeff);
        }
        /*
          if(volume>0.)volume = 0.;
                double coeff = -2*refShearModulus*modulusFactor*loadingFunc;
    workV6.addVector(1.0, surfNormal.deviator(), coeff);
  */
        trialStress.setData(workV6, volume);
  deviatorScaling(trialStress, theSurfaces, activeSurfaceNum);

  if(isCrossingNextSurface()) {
    activeSurfaceNum++;
    return stressCorrection(1);  //recursive call
  }
  return 0;
}


