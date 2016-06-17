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
// $Revision: 1.8 $
// $Date: 2006/01/10 19:00:10 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/soil/PressureDependMultiYield02.cpp,v $

// Written: ZHY
// Created: May 2004

//
// PressureDependMultiYield02.cpp
// -------------------
//

#include <cmath>
#include <cstdlib>
#include <material/nD/soil/PressureDependMultiYield02.h>
#include <domain/mesh/element/Information.h>
#include <utility/matrix/ID.h>
#include <utility/recorder/response/MaterialResponse.h>
#include <material/nD/soil/MultiYieldSurface.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/nDarray/Tensor.h>
#include <utility/matrix/nDarray/stresst.h>

#include "material/nD/TipoMaterialND.h"

double* XC::PressureDependMultiYield02::contractParam2x=0;
double* XC::PressureDependMultiYield02::contractParam3x=0;
double* XC::PressureDependMultiYield02::dilateParam3x=0;

const        double pi = 3.14159265358979;

//double check;

XC::PressureDependMultiYield02::PressureDependMultiYield02 (int tag, int nd,
                                                    double r, double refShearModul,
                                                    double refBulkModul, double frictionAng,
                                                    double peakShearStra, double refPress,
                                                    double pressDependCoe,
                                                    double phaseTransformAng,
                                                double contractionParam1,
                                                double contractionParam3,
                                                    double dilationParam1,
                                                        double dilationParam3,
                                                int   numberOfYieldSurf,
                                                    double * gredu,
                                                    double contractionParam2,
                                                double dilationParam2,
                                                double liquefactionParam1,
                                                    double liquefactionParam2,
                                                    double ei,
                                                    double volLim1, double volLim2, double volLim3,
                                                    double atm, double cohesi,
                                                        double hv, double pv)
 : PressureDependMultiYieldBase(tag,ND_TAG_PressureDependMultiYield02), check(0), updatedTrialStress(),
   PivotStrainRate(6), PivotStrainRateCommitted(6)
  {

  /*if(phaseTransformAng > frictionAng) {
    std::cerr << "WARNING:XC::PressureDependMultiYield02:: phaseTransformAng > frictionAng" << std::endl;
    std::cerr << "Will set phaseTransformAng = frictionAng." <<std::endl;
    phaseTransformAng = frictionAng+0.1;
  }*/


  if(volLim1 < 0) {
    std::cerr << "WARNING:XC::PressureDependMultiYield02:: volLim1 < 0" << std::endl;
    std::cerr << "Will reset volLimit to 0.8" << std::endl;
    volLim1 = 0.8;
  }

    if(matCount%20 == 0)
      {
        double *temp1a = contractParam2x;
        double *temp1b = contractParam3x;
        double *temp2 = dilateParam3x;
        double *temp3 = volLimit1x;
        double *temp4 = volLimit2x;
        double *temp5 = volLimit3x;
        contractParam2x = new double[matCount+20];
        contractParam3x = new double[matCount+20];
        dilateParam3x = new double[matCount+20];
        volLimit1x = new double[matCount+20];
        volLimit2x = new double[matCount+20];
        volLimit3x = new double[matCount+20];

        for(int i=0; i<matCount; i++)
          {
         contractParam2x[i] = temp1a[i];
         contractParam3x[i] = temp1b[i];
         dilateParam3x[i] = temp2[i];
         volLimit1x[i] = temp3[i];
         volLimit2x[i] = temp4[i];
         volLimit3x[i] = temp5[i];
      }

         if(matCount > 0)
           {
             delete [] temp1a; delete [] temp1b; delete [] temp2; delete [] temp3; delete [] temp4;
             delete [] temp5;
           }
    }

  contractParam2x[matCount] = contractionParam2;
  contractParam3x[matCount] = contractionParam3;
  volLimit1x[matCount] = volLim1;
  volLimit2x[matCount] = volLim2;
  volLimit3x[matCount] = volLim3;
  dilateParam3x[matCount] = dilationParam3;
  einitx[matCount] = ei;
  Hvx[matCount] = hv;
  Pvx[matCount] = pv;

  damage = 0.;

  setUpSurfaces(gredu);  // residualPress and stressRatioPT are calculated inside.
}


XC::PressureDependMultiYield02::PressureDependMultiYield02(int tag)
 : PressureDependMultiYieldBase(tag,ND_TAG_PressureDependMultiYield02),
   PivotStrainRate(6), PivotStrainRateCommitted(6)
  {}

XC::PressureDependMultiYield02::PressureDependMultiYield02()
 : PressureDependMultiYieldBase(0,ND_TAG_PressureDependMultiYield02),
   PivotStrainRate(6), PivotStrainRateCommitted(6)
  {}


XC::PressureDependMultiYield02::PressureDependMultiYield02 (const XC::PressureDependMultiYield02 &a)
 : PressureDependMultiYieldBase(a),
   check(0),updatedTrialStress(a.updatedTrialStress),
   PivotStrainRate(a.PivotStrainRate), PivotStrainRateCommitted(a.PivotStrainRateCommitted)
  { damage = a.damage; }


const XC::Matrix &XC::PressureDependMultiYield02::getTangent(void) const
{
  int loadStage = loadStagex[matN];
  double refShearModulus = refShearModulusx[matN];
  double refBulkModulus = refBulkModulusx[matN];
  double pressDependCoeff = pressDependCoeffx[matN];
  double refPressure = refPressurex[matN];
  double residualPress = residualPressx[matN];
  int ndm = ndmx[matN];

  if(loadStage == 1 && e2p == 0) {
      initPress = currentStress.volume();
      elast2Plast();
  }
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
    double factor = getModulusFactor(updatedTrialStress);
    double shearModulus = factor*refShearModulus;
    double bulkModulus = factor*refBulkModulus;

        // volumetric plasticity
        if(Hvx[matN] != 0. && trialStress.volume()<=maxPress
                && strainRate.volume()<0. && loadStage == 1) {
          double tp = fabs(trialStress.volume() - residualPress);
      bulkModulus = (bulkModulus*Hvx[matN]*pow(tp,Pvx[matN]))/(bulkModulus+Hvx[matN]*pow(tp,Pvx[matN]));
        }

    /*if(loadStage!=0 && committedActiveSurf > 0) {
      getSurfaceNormal(updatedTrialStress, workT2V);
      workV6 = workT2V.deviator();
      double volume = workT2V.volume();
      double Ho = 9.*bulkModulus*volume*volume + 2.*shearModulus*(workV6 && workV6);
      double plastModul = factor*committedSurfaces[committedActiveSurf].modulus();
      coeff1 = 9.*bulkModulus*bulkModulus*volume*volume/(Ho+plastModul);
      coeff2 = 4.*shearModulus*shearModulus/(Ho+plastModul);
      // non-symmetric stiffness
      getSurfaceNormal(updatedTrialStress, workT2V);
      workV6 = workT2V.deviator();
      double qq = workT2V.volume();
                        double XC::pp=getPlasticPotential(updatedTrialStress, workT2V);
      double Ho = 9.*bulkModulus*pp*qq + 2.*shearModulus*(workV6 && workV6);
      double plastModul = factor*committedSurfaces[committedActiveSurf].modulus();
      coeff1 = 9.*bulkModulus*bulkModulus*pp*qq/(Ho+plastModul);
      coeff2 = 4.*shearModulus*shearModulus/(Ho+plastModul);
                        coeff3 = 6.*shearModulus*pp/(Ho+plastModul);
                        coeff4 = 6.*shearModulus*qq/(Ho+plastModul);
    }*/
    if(loadStage!=0 && activeSurfaceNum > 0) {
      factor = getModulusFactor(trialStress);
      shearModulus = factor*refShearModulus;
      bulkModulus = factor*refBulkModulus;
      getSurfaceNormal(trialStress, workT2V);
      workV6 = workT2V.deviator();
      double volume = workT2V.volume();
      double Ho = 9.*bulkModulus*volume*volume + 2.*shearModulus*(workV6 && workV6);
      double plastModul = factor*theSurfaces[activeSurfaceNum].modulus();
      coeff1 = 9.*bulkModulus*bulkModulus*volume*volume/(Ho+plastModul);
      coeff2 = 4.*shearModulus*shearModulus/(Ho+plastModul);
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


const XC::Vector & XC::PressureDependMultiYield02::getStress(void) const
{
  int loadStage = loadStagex[matN];
  int numOfSurfaces = numOfSurfacesx[matN];
  int ndm = ndmx[matN];

  int i, is;
  if(loadStage == 1 && e2p == 0) {
      initPress = currentStress.volume();
      elast2Plast();
  }

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
    onPPZ = onPPZCommitted;
    PPZSize = PPZSizeCommitted;
    cumuDilateStrainOcta = cumuDilateStrainOctaCommitted;
    maxCumuDilateStrainOcta = maxCumuDilateStrainOctaCommitted;
    cumuTranslateStrainOcta = cumuTranslateStrainOctaCommitted;
    prePPZStrainOcta = prePPZStrainOctaCommitted;
    oppoPrePPZStrainOcta = oppoPrePPZStrainOctaCommitted;
    PPZPivot = PPZPivotCommitted;
    PivotStrainRate = PivotStrainRateCommitted;
    PPZCenter = PPZCenterCommitted;

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
                          updatedTrialStress = currentStress;
                          setTrialStress(currentStress);
              is = isLoadReversal(currentStress);
                }
                else {
            updatedTrialStress = trialStress;
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
        stressCorrection(0);
        updateActiveSurface();

            double refBulkModulus = refBulkModulusx[matN];
                //modulusFactor was calculated in setTrialStress
        double B = refBulkModulus*modulusFactor;
                //double deltaD = 3.*subStrainRate.volume()
                //                 - (trialStress.volume()-updatedTrialStress.volume())/B;
                //if(deltaD<0) deltaD /=2 ;
        //pressureD += deltaD;
                pressureD += 3.*subStrainRate.volume()
                                 - (trialStress.volume()-updatedTrialStress.volume())/B;
                if(pressureD < 0.) pressureD = 0.;
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


XC::NDMaterial *XC::PressureDependMultiYield02::getCopy(void) const
  { return new XC::PressureDependMultiYield02(*this); }

XC::NDMaterial * XC::PressureDependMultiYield02::getCopy(const std::string &code) const
  {
    PressureDependMultiYield02 *copy= nullptr;
    if((code==strTipoPressureIndependMultiYield02) || (code==strTipoPlaneStrain)
      || (code==strTipoThreeDimensional))
      copy = new XC::PressureDependMultiYield02(*this);
    return copy;
  }


const std::string &XC::PressureDependMultiYield02::getType(void) const
  {
    int ndm = ndmx[matN];
    return (ndm == 2) ? strTipoPlaneStrain : strTipoThreeDimensional;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::PressureDependMultiYield02::sendData(CommParameters &cp)
  {
    int res= PressureDependMultiYieldBase::sendData(cp);
    res+= cp.sendDoubles(damage,check,getDbTagData(),CommMetaData(20));
    res+= cp.sendMovable(updatedTrialStress,getDbTagData(),CommMetaData(21));
    res+= cp.sendVector(PivotStrainRate,getDbTagData(),CommMetaData(22));
    res+= cp.sendVector(PivotStrainRateCommitted,getDbTagData(),CommMetaData(23));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::PressureDependMultiYield02::recvData(const CommParameters &cp)
  {
    int res= PressureDependMultiYieldBase::recvData(cp);
    res+= cp.receiveDoubles(damage,check,getDbTagData(),CommMetaData(20));
    res+= cp.receiveMovable(updatedTrialStress,getDbTagData(),CommMetaData(21));
    res+= cp.receiveVector(PivotStrainRate,getDbTagData(),CommMetaData(22));
    res+= cp.receiveVector(PivotStrainRateCommitted,getDbTagData(),CommMetaData(23));
    return res;
  }

int XC::PressureDependMultiYield02::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(24);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }


int XC::PressureDependMultiYield02::recvSelf(const CommParameters &cp)
  {
    inicComm(24);
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

void XC::PressureDependMultiYield02::Print(std::ostream &s, int flag )
{
  s << strTipoPressureIndependMultiYield02 << std::endl;
}


const XC::Vector & XC::PressureDependMultiYield02::getCommittedStress(void)
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
    /*temp7[7] = committedActiveSurf;
        temp7[8] = stressRatioPTx[matN];
        temp7[9] = currentStress.deviatorRatio(residualPressx[matN]);
    temp7[10] = pressureDCommitted;
    temp7[11] = cumuDilateStrainOctaCommitted;
    temp7[12] = maxCumuDilateStrainOctaCommitted;
    temp7[13] = cumuTranslateStrainOctaCommitted;
    temp7[14] = onPPZCommitted;
    temp7[15] = PPZSizeCommitted;*/
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
        temp5[6] = PPZCenterCommitted.deviator()[3];
        temp5[7] = PPZPivotCommitted.deviator()[3];
    temp5[8] = pressureDCommitted;
    temp5[9] = cumuDilateStrainOctaCommitted;
    temp5[10] = maxCumuDilateStrainOctaCommitted;
    temp5[11] = cumuTranslateStrainOctaCommitted;
    temp5[12] = onPPZCommitted;
    temp5[13] = PPZSizeCommitted;
        temp5[14] = PivotStrainRateCommitted[3];
    temp5[15] = initPress;*/
    return temp5;
  }
}


// NOTE: surfaces[0] is not used
void XC::PressureDependMultiYield02::setUpSurfaces(double * gredu)
{
    double residualPress = residualPressx[matN];
    double refPressure = refPressurex[matN];
    //double pressDependCoeff =pressDependCoeffx[matN];
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
                // tao = cohesion * sqrt(8)/3.
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
          //std::cerr<<ii<<" "<<size<<" "<<plast_modul<<std::endl;
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
                          // gamma_oct = sqrt(6)/3*gamma12
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


void XC::PressureDependMultiYield02::initStrainUpdate(void)
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

  // plastic strain state (scaled from elastic strain)
  double scale, PPZLimit;
  if(stressRatio >= stressRatioPT) {  //above PT
    onPPZCommitted = 2;
    prePPZStrainOctaCommitted = strainPTOcta * ratio;
    PPZLimit = getPPZLimits(1,currentStress);
    scale = sqrt(prePPZStrainOctaCommitted+PPZLimit)/octalStrain;
  }
  else {  // below PT
    onPPZCommitted = -1;
    prePPZStrainOctaCommitted = octalStrain;
    if(prePPZStrainOctaCommitted > strainPTOcta * ratio)
                prePPZStrainOctaCommitted=strainPTOcta*ratio;
    scale = sqrt(prePPZStrainOctaCommitted)/octalStrain;
  }
  //currentStrain.setData(currentStrain.deviator()*scale, currentStrain.volume());
  workV6.addVector(0.0, currentStrain.deviator(), scale);
  currentStrain.setData(workV6, currentStrain.volume());
  PPZPivotCommitted = currentStrain;
}

double XC::PressureDependMultiYield02::getPlasticPotential(const XC::T2Vector & contactStress,
                                                     const XC::T2Vector & surfaceNormal) const
{
    double residualPress = residualPressx[matN];
    double stressRatioPT = stressRatioPTx[matN];
        double contractParam1 = contractParam1x[matN];
        double contractParam2 = contractParam2x[matN];
        double contractParam3 = contractParam3x[matN];
    double dilateParam1 = dilateParam1x[matN];
    double dilateParam2 = dilateParam2x[matN];

  double plasticPotential, contractRule, shearLoading, angle;

  double contactRatio = contactStress.deviatorRatio(residualPress);
  double factorPT = contactRatio/stressRatioPT;
  //double currentRatio = currentStress.deviatorRatio(residualPress);
  double currentRatio = updatedTrialStress.deviatorRatio(residualPress);
  double trialRatio = trialStress.deviatorRatio(residualPress);
  shearLoading = updatedTrialStress.deviator() && trialStress.deviator();
  //shearLoading = currentStress.deviator() && trialStress.deviator();

  if(factorPT >= 1. && trialRatio >= currentRatio && shearLoading >= 0.) {  //dilation
      updatePPZ(contactStress);
      if(onPPZ==1)
                  plasticPotential = 0.;
      else if(onPPZ==2) {
          factorPT -= 1.0;
                            double dilateParam3 = dilateParam3x[matN];
                  double ppp=pow((fabs(contactStress.volume())+fabs(residualPress))/pAtm, -dilateParam3);
          plasticPotential = ppp*factorPT*(factorPT)*(dilateParam1+pow(cumuDilateStrainOcta,dilateParam2));
          if(plasticPotential < 0.) plasticPotential = -plasticPotential;
                  if(plasticPotential>5.0e4) plasticPotential = 5.0e4;
      }
          else {
          std::cerr << "FATAL: Wrong onPPZ value: " << onPPZ << std::endl;
          exit(-1);
          }
  } else {  //contraction
          if(currentRatio==0.) angle = 1.0;
      else {
         workV6 = trialStress.deviator();
                 workV6 /= (fabs(trialStress.volume())+fabs(residualPress));
                 workV6 -= (updatedTrialStress.deviator()/(fabs(updatedTrialStress.volume())+fabs(residualPress)));
                 //workV6        -= currentStress.deviator()/(fabs(currentStress.volume())+fabs(residualPress));
                 //workV6.Normalize();
                 //angle = updatedTrialStress.unitDeviator() && workV6;
                 workT2V = XC::T2Vector(workV6);
                 if(workT2V.deviatorLength() == 0.) angle = 1.0;
                 //angle = (currentStress.deviator() && workV6)/workT2V.deviatorLength()/currentStress.deviatorLength();
                 else angle = (updatedTrialStress.deviator() && workV6)/workT2V.deviatorLength()/updatedTrialStress.deviatorLength();
          }
      factorPT = factorPT*angle - 1.0;

          contractRule = pow((fabs(contactStress.volume())+fabs(residualPress))/pAtm, contractParam3);
      if(contractRule < 0.1) contractRule = 0.1;

          //plasticPotential = factorPT*(contractParam1+pressureD*contractParam2)*contractRule;
          //plasticPotential = factorPT*(contractParam1+cumuDilateStrainOcta*contractParam2)*contractRule;
          //double dd = maxCumuDilateStrainOcta > cumuDilateStrainOcta ? maxCumuDilateStrainOcta : cumuDilateStrainOcta;
          //plasticPotential = -(factorPT)*(factorPT)*(contractParam1+dd*contractParam2)*contractRule;
          plasticPotential = -(factorPT)*(factorPT)*(contractParam1+maxCumuDilateStrainOcta*contractParam2)*contractRule;

      if(plasticPotential > 0.) plasticPotential = -plasticPotential;
          //if(contractRule<-5.0e4) contractRule = -5.0e4;
      if(onPPZ > 0) onPPZ = 0;
      if(onPPZ != -1) PPZTranslation(contactStress);
  }

  if(isCriticalState(contactStress)) plasticPotential = 0;
  return plasticPotential;
}


int XC::PressureDependMultiYield02::isCriticalState(const XC::T2Vector & stress) const
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
        if(volLimit3 != 0.) {
	  ecr1 = volLimit1 - volLimit2*pow(std::abs(-stress.volume()/pAtm), volLimit3);
          ecr2 = volLimit1 - volLimit2*pow(std::abs(-updatedTrialStress.volume()/pAtm), volLimit3);
        } else {
	  ecr1 = volLimit1 - volLimit2*log(std::abs(-stress.volume()/pAtm));
          ecr2 = volLimit1 - volLimit2*log(std::abs(-updatedTrialStress.volume()/pAtm));
        }

        if(ecurr < ecr2 && etria < ecr1) return 0;
        if(ecurr > ecr2 && etria > ecr1) return 0;
        return 1;
}


void XC::PressureDependMultiYield02::updatePPZ(const XC::T2Vector & contactStress) const
{
  double liquefyParam1 = liquefyParam1x[matN];
  double residualPress = residualPressx[matN];
  double refPressure = refPressurex[matN];
    double pressDependCoeff =pressDependCoeffx[matN];
    //double liquefyParam2 = liquefyParam2x[matN];

  // onPPZ=-1 may not be needed. can start with onPPZ=0  ****

  double temp = strainRate.deviator() && PivotStrainRateCommitted;
  check = strainRate.deviator()[3];

  if(onPPZ < 1) {
    damage = 0.0;
    if((maxPress-currentStress.volume())/(maxPress-residualPress) > 0.)
          damage = pow((maxPress-currentStress.volume())/(maxPress-residualPress),0.25);
  }

  // PPZ inactive if liquefyParam1==0.
  if(liquefyParam1==0. || (onPPZ < 1 && damage < 0.)) {
    if(onPPZ==2) {
       PPZPivot = trialStrain;
           //PivotStrainRate = strainRate.deviator();
           cumuDilateStrainOcta += subStrainRate.octahedralShear(1);
    }
    else if(onPPZ != 2) {
       onPPZ = 2;
       PPZPivot = trialStrain;
           PivotStrainRate = strainRate.deviator();
       if(temp < 0.) cumuDilateStrainOcta = 0.;
          //if(temp < 0.) maxCumuDilateStrainOcta = 0.;
    }
    return;
  }

  // dilation: calc. cumulated dilative strain
  if(onPPZ==2) {
    PPZPivot = trialStrain;
        //PivotStrainRate = strainRate.deviator();
        cumuDilateStrainOcta += subStrainRate.octahedralShear(1);

        double zzz = 0.;
        if(damage > zzz) zzz = damage;
        maxCumuDilateStrainOcta += zzz*liquefyParam1*subStrainRate.octahedralShear(1);
    return;
  }

  if(onPPZ==-1 || onPPZ==0) {

          // moved to the opposite direction, update prePPZStrainOcta and
          // oppoPrePPZStrainOcta (they are small values, may consider drop these
          // parameters altogether).

          if(temp < 0.) {
        double volume = -contactStress.volume();
        oppoPrePPZStrainOcta = prePPZStrainOcta;
        double ratio = (volume+residualPress)/(-refPressure+residualPress);
        ratio = pow(ratio, 1.-pressDependCoeff);
        prePPZStrainOcta = ratio * strainPTOcta;
        if(oppoPrePPZStrainOcta == 0.) oppoPrePPZStrainOcta = prePPZStrainOcta;
          }
  }

  //double dd = maxCumuDilateStrainOcta > cumuDilateStrainOcta ? maxCumuDilateStrainOcta : cumuDilateStrainOcta;

  //PPZSize = (cumuTranslateStrainOcta+dd)/2.;
  PPZSize = (cumuTranslateStrainOcta+maxCumuDilateStrainOcta)/2.;
         //(prePPZStrainOcta+oppoPrePPZStrainOcta+cumuTranslateStrainOcta+maxCumuDilateStrainOcta)/2.;

  // calc. new PPZ center.
  if(onPPZ==0 || (onPPZ==1 && temp < 0.0)) {
        // new center lies on the vector of PPZPivot-PPZCenter,
        // its distance from PPZPivot is (PPZSize-cumuTranslateStrainOcta).

    //workT2V.setData(PPZPivot.t2Vector() - PPZCenter.t2Vector());
    workV6 = PPZPivot.t2Vector();
    workV6.addVector(1.0, PPZCenter.t2Vector(), -1.);
    workT2V.setData(workV6);

    double coeff;
        if(workT2V.octahedralShear(1)==0.) coeff = 0.;
        else coeff = (PPZSize-cumuTranslateStrainOcta)/workT2V.octahedralShear(1);
    //PPZCenter.setData(PPZPivot.t2Vector() - workT2V.t2Vector()*coeff);
    workV6 = PPZPivot.t2Vector();
    workV6.addVector(1.0, workT2V.t2Vector(), -coeff);
    PPZCenter.setData(workV6);
  }

  //workT2V.setData(trialStrain.t2Vector() - PPZCenter.t2Vector());
  workV6 = trialStrain.t2Vector();
  workV6.addVector(1.0, PPZCenter.t2Vector(), -1.);
  workT2V.setData(workV6);

  //outside PPZ
  //if(workT2V.octahedralShear(1) > PPZSize && temp > 0. || PPZLimit==0.) {
  if(workT2V.octahedralShear(1) > PPZSize) {

        // rezero cumuDilateStrainOcta only when load reverses
        // (partial unloading does not erase cumuDilateStrainOcta.

    //if(temp < 0.) {
         cumuDilateStrainOcta = 0.;
         //if(PPZLimit == 0.) maxCumuDilateStrainOcta = 0.;
        //}
    onPPZ = 2;
    PPZPivot = trialStrain;
        PivotStrainRate = strainRate.deviator();
    cumuTranslateStrainOcta = 0.;
  }
  else {  //inside PPZ
    if(onPPZ == 0 || onPPZ == 1) PPZTranslation(contactStress);
    //if(onPPZ == 0) onPPZ = 1;
        if(onPPZ == -1 || onPPZ == 0) onPPZ = 1;
  }
}


void XC::PressureDependMultiYield02::PPZTranslation(const XC::T2Vector & contactStress) const
{
        double liquefyParam1 = liquefyParam1x[matN];
          double liquefyParam2 = liquefyParam2x[matN];
    double residualPress = residualPressx[matN];

        //cumuDilateStrainOcta -= subStrainRate.octahedralShear(1);
    //if(cumuDilateStrainOcta < 0.) cumuDilateStrainOcta = 0.;

    if(liquefyParam1==0.) return;

  //Amount of translation is proportional to the amount of previous unloading,
  //and is also limited by the amount of previous dilation (no translation
  //if there was no dilation), as damage is really caused by dilation.
  damage = 0.0;
  if((maxPress-currentStress.volume())/(maxPress-residualPress) > 0.)
        damage = pow((maxPress-currentStress.volume())/(maxPress-residualPress),0.25);

  double zzz = 0.;
  if(damage > zzz) zzz = damage;

  double temp = strainRate.deviator() && PivotStrainRateCommitted;

  if(temp < 0.0) {  //update only when load reverses

  //workT2V.setData(trialStrain.deviator()-PPZPivot.deviator());
     workV6 = trialStrain.deviator();
     workV6 -= PPZPivot.deviator();
     workT2V.setData(workV6);

     temp = workT2V.octahedralShear(1);
     if(cumuTranslateStrainOcta < zzz*liquefyParam2*temp)
             cumuTranslateStrainOcta = zzz*liquefyParam2*temp;
  //if(maxCumuDilateStrainOcta == 0.) temp = 0.; //PPZTransLimit;
        //\\// else temp = PPZTransLimit*cumuDilateStrainOcta/maxCumuDilateStrainOcta;
  //else temp = dilateParam3*cumuDilateStrainOcta/maxCumuDilateStrainOcta;
  //if(cumuTranslateStrainOcta > temp) cumuTranslateStrainOcta = temp;

  //\\// cumuTranslateStrainOcta = dilateParam3*cumuDilateStrainOcta;
  }
}


double XC::PressureDependMultiYield02::getPPZLimits(int which, const XC::T2Vector & contactStress) const
{
        double liquefyParam1 = liquefyParam1x[matN];
        double liquefyParam2 = liquefyParam2x[matN];
        double dilateParam3 = dilateParam3x[matN];

  double PPZLimit, temp;
  double volume = -contactStress.volume();

  if(volume >= liquefyParam1) PPZLimit = 0.;
  else {
    temp = volume*pi/liquefyParam1/2.;
                // liquefyParam3 = 3.0 default
    PPZLimit = liquefyParam2 * pow(cos(temp), 3.);
        //\\//
    PPZLimit = 0.0;
  }

  if(which==1)
    return PPZLimit;
  else if(which==2)
    return dilateParam3 * PPZLimit;
  else {
    std::cerr << "FATAL:XC::PressureDependMultiYield02::getPPZLimits: unknown argument value" << std::endl;
   exit(-1);
    return 0.0;
  }
}


int XC::PressureDependMultiYield02::stressCorrection(int crossedSurface) const
  {
    double refShearModulus = refShearModulusx[matN];
    double refBulkModulus = refBulkModulusx[matN];

    static XC::T2Vector contactStress;
    getContactStress(contactStress);
    static XC::T2Vector surfNormal;
    getSurfaceNormal(contactStress, surfNormal);
    double plasticPotential = getPlasticPotential(contactStress,surfNormal);
    double tVolume = trialStress.volume();
    double loadingFunc = getLoadingFunc(contactStress, surfNormal,plasticPotential, crossedSurface);
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


