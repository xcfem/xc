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
//  MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO.
//  GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// junto a este programa.
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

// $Revision: 1.4 $
// $Date: 2006/01/19 19:19:12 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ReinforcingSteel.cpp,v $

/* ****************************************************************** **
** THIS FILE WAS DEVELOPED AT UC DAVIS                                **
**                                                                    **
** Programmed by: Jon Mohle (jfmohle@ucdavis.edu)                     **
** Supervisor: Sashi Kunnath (skkunnath@ucdavis.edu)                  **
**                                                                    **
********************************************************************* */
// Written: Jon Mohle
// Created: October 2003
// Updated: January 2006
//
// Description: This file contains the class definition for
// ReinforcingSteel

#include "material/uniaxial/ReinforcingSteel.h"
#include <utility/matrix/Vector.h>

#include <cmath>
#include <cfloat>
#include "utility/actor/actor/ArrayCommMetaData.h"

#ifdef HelpDebugMat
  int XC::ReinforcingSteel::classCount= 0;
#endif

// LastRule_RS must be greater than or equal to 12.  Add branches 4 at a time
// eq 12, 16, 20, 24 etc... failure to add branches 4 at a time will cause problems.
// A higher number will result in better tracking of the loop memory effects at the cost of additional memory usage
// each pair of rules requires 11 additional double variables.
//const int XC::ReinforcingSteel::LastRule_RS=20;  // must be divisable by 4!!!!!!!!!!!
//const int XC::ReinforcingSteel::vSize= LastRule_RS/2+1;

XC::ReinforcingSteel::ReinforcingSteel(int tag, double fy, double fsu, double Es, double Esh_, double esh_, double esu,
                                   int buckModel, double slenderness, double alpha, double r, double gama,
                                   double Fatigue1, double Fatigue2, double Degrade, double rc1, double rc2, double rc3,
                                   double A1, double HardLim)
  :UniaxialMaterial(tag,MAT_TAG_ReinforcingSteel),
  fsu_fraction(gama),beta(alpha), esh(esh_), Esh(Esh_), a1(A1), hardLim(HardLim),
   T_ePlastic(vSize), C_ePlastic(vSize),
   LDratio(slenderness),Fat1(Fatigue1),BuckleModel(buckModel),RC1(rc1),RC2(rc2),RC3(rc3), 
  CR(vSize), Cfch(vSize), CQ(vSize), CEsec(vSize), Cea(vSize), Cfa(vSize), CEa(vSize), Ceb(vSize), Cfb(vSize), CEb(vSize)
  {
  if((r>=0.0) & (r<=1.0))
    reduction=r;
  else
    if(r<=0)
      reduction= 0.0;
    else
      reduction= 1.0;

        if((Fatigue1==0) || (Fatigue2==0)) {
                Fat1=9.9e30;
                Fat2=1.0;
                Deg1=0.0;
  } else {
    Fat2=1.0/Fatigue2;
    if(Degrade != 0.0)
      Deg1=pow(Fat1/Degrade,Fat2);
    else
      Deg1=0.0;
  }

  //initial yield point in natural stress-strain
        eyp=log(1.0+fy/Es);
        fyp=fy*(1.0+fy/Es);
        Esp=fyp/eyp;

  // ultimate strain and slope in natural stress-strain
        esup=log(1.0+esu);
  Esup=fsu*(1.0+esu);

  //updateHardeningLoaction(1.0);  done in revert to start

  ZeroTol=1.0E-14;
  this->revertToStart();
}

void XC::ReinforcingSteel::updateHardeningLoaction(double PlasticStrain)
{
  double ep;
  double pBranchStrain_t= Temax - Backbone_f(Temax)/Esp;
  double pBranchStrain_c= Temin + Backbone_f(Temin)/Esp;
  if (pBranchStrain_t > -pBranchStrain_c)
    ep= PlasticStrain - pBranchStrain_t;
  else
    ep= PlasticStrain + pBranchStrain_c;
  THardFact= 1.0 - a1*ep;
  if (THardFact<hardLim) THardFact= hardLim;
  if (THardFact>1.0) THardFact= 1.0;
  updateHardeningLoactionParams();
}

void XC::ReinforcingSteel::updateHardeningLoactionParams()
{
  double ey= exp(eyp)-1.0;
  double fy= fyp/(1.0+ey);
  double eshLoc= THardFact*(esh-ey)+ey;

  // strain hardened point in natural stress-strain
        eshp=log(1.0+eshLoc);
        fshp=fy*(1.0+eshLoc);

  // ultimate stress in natural stress-strain
  fsup=Esup-(esup-eshp)*Esup;

  // strain hardedned slope, yield plateau slope, and intersect
  Eshp=Esh*pow(1.0+eshLoc,2.0)+fshp - Esup;
        Eypp=(fshp-fyp)/(eshp-eyp);
        fint= fyp-Eypp*eyp;

  p= Eshp*(esup-eshp)/(fsup-fshp);
        // Set backbone transition variables
        double fTemp= Backbone_fNat(eshp+0.0002);
        Eshpb= Eshp*pow((fsup-fTemp)/(fsup-fshp),1.0-1.0/p);
  eshpa= eshp + 0.0002 - 2.0*(fTemp-fshp)/Eshpb;
}

XC::ReinforcingSteel::ReinforcingSteel(int tag)
  :UniaxialMaterial(tag,MAT_TAG_ReinforcingSteel),
   T_ePlastic(vSize), C_ePlastic(vSize),
   CR(vSize), Cfch(vSize), CQ(vSize), CEsec(vSize), Cea(vSize), Cfa(vSize), CEa(vSize), Ceb(vSize), Cfb(vSize), CEb(vSize)
  {
#ifdef HelpDebugMat
    thisClassNumber= ++classCount;
    thisClassCommit= 0;
#endif
    ZeroTol=1.0E-14;
  }

/***************** material state determination methods ***********/
int XC::ReinforcingSteel::setTrialStrain(double strain, double strainRate) {
  int res= 0;
  #ifdef HelpDebugMat
    thisClassStep++;
    if (thisClassCommit == 4000 && thisClassStep == 1)
      if(scalefactor()<1.0)
        std::cerr << scalefactor() << "\n";
  #endif
  // Reset Trial History Variables to Last Converged State
  revertToLastCommit();



  if(strain< -0.95) {
    std::cerr << "Large trial compressive strain\n";
    return -1;
  } else
    trial.Strain()= log(1.0 + strain);

  if (trial.Strain() == converged.getStrain()) return 0;

  if (TBranchNum==0){
                if (trial.getStrain()>0.0) TBranchNum= 1;
                if (trial.getStrain()<0.0) TBranchNum= 2;
  }


  if(thisClassNumber==51 && thisClassCommit==781) {
    thisClassCommit= thisClassCommit;
  }
  res= BranchDriver(res);



  if (res==0)
    return 0;
  else
    return -1;
}

double XC::ReinforcingSteel::getStrain(void) const
  { return exp(trial.getStrain())-1.0; }

double XC::ReinforcingSteel::getStress(void) const
  {
    if(theBarFailed) return 0.0;
    double tempstr=trial.getStress();
    switch(BuckleModel)
      {
      case 1:
        tempstr= Buckled_stress_Gomes(trial.getStrain(),trial.getStress());
        break;
      case 2:
        tempstr= Buckled_stress_Dhakal(trial.getStrain(),trial.getStress());
        break;
      }
    double tempOut= tempstr*scalefactor()/exp(trial.getStrain());
    return tempOut;
  }

double XC::ReinforcingSteel::getTangent(void) const
  {
    double taTan= trial.getTangent();
    switch(BuckleModel)
      {
      case 1:
        taTan= Buckled_mod_Gomes(trial.getStrain(),trial.getStress(),trial.getTangent());
        break;
      case 2:
        taTan= Buckled_mod_Dhakal(trial.getStrain(),trial.getStress(),trial.getTangent());
        break;
      }
    double scfact= scalefactor();
    double tempOut= (taTan+trial.getStress())*scfact/pow(exp(trial.getStrain()),2.0);
    return tempOut;
  }

double XC::ReinforcingSteel::getInitialTangent(void) const
  { return Esp; }

/***************** path dependent behavior methods ***********/
int XC::ReinforcingSteel::commitState(void)
  {
#ifdef HelpDebugMat
  thisClassCommit++;
  thisClassStep= 0;
#endif

  if(TBranchNum <= 1)
        TBranchMem=0;
  else
        TBranchMem= (TBranchNum+1)/2;

    C_ePlastic=T_ePlastic;

  CFatDamage= TFatDamage;

  // commit trial history variables
  CBranchNum= TBranchNum;
  Ceo_p= Teo_p;
  Ceo_n= Teo_n;
  Cemax= Temax;
  Cemin= Temin;
  CeAbsMax= TeAbsMax;
  CeAbsMin= TeAbsMin;
  CeCumPlastic= TeCumPlastic;
  CHardFact= THardFact;

  if(TBranchNum > 2)
    {
      CR[TBranchMem]= TR;
      Cfch[TBranchMem]= Tfch;
      CQ[TBranchMem]= TQ;
      CEsec[TBranchMem]= TEsec;
      Cea[TBranchMem]= Tea;
      Cfa[TBranchMem]= Tfa;
      CEa[TBranchMem]= TEa;
      Ceb[TBranchMem]= Teb;
      Cfb[TBranchMem]= Tfb;
      CEb[TBranchMem]= TEb;
    }

  // commit trial state variables
  converged= trial;
  return 0;
}

int XC::ReinforcingSteel::revertToLastCommit(void)
  {
    T_ePlastic=C_ePlastic;

    TFatDamage= CFatDamage;

    // Reset trial history variables to last committed state
    TBranchNum= CBranchNum;
    Teo_p= Ceo_p;
    Teo_n= Ceo_n;
    Temax= Cemax;
    Temin= Cemin;
    TeAbsMax= CeAbsMax;
    TeAbsMin= CeAbsMin;
    TeCumPlastic= CeCumPlastic;
    THardFact= CHardFact;
    updateHardeningLoactionParams();

    if(TBranchNum > 2) SetPastCurve(TBranchNum);

    // Reset trial state variables to last committed state
    trial= converged;

    return 0;
  }

int XC::ReinforcingSteel::revertToStart(void)
{
  theBarFailed= 0;

  THardFact= 1.0;
  CHardFact= 1.0;
  updateHardeningLoactionParams();

  CFatDamage= TFatDamage;
  for(int i=0; i<vSize; i++) {
    C_ePlastic[i]= 0.0;
    T_ePlastic[i]= 0.0;
          CR[i]= 0.0;
          Cfch[i]= 0.0;
          CQ[i]= 0.0;
          CEsec[i]= 0.0;
          Cea[i]= 0.0;
          Cfa[i]= 0.0;
          CEa[i]= 0.0;
          Ceb[i]= 0.0;
          Cfb[i]= 0.0;
          CEb[i]= 0.0;
  }
  TR= 0.0;
  Tfch= 0.0;
  TQ= 0.0;
  TEsec= 0.0;
  Tea= 0.0;
  Tfa= 0.0;
  TEa= 0.0;
  Teb= 0.0;
  Tfb= 0.0;
  TEb= 0.0;

  // reset trial history variables
  CBranchNum= 0;
  TBranchNum= 0;
  Ceo_p= 0.0;
  Teo_p= 0.0;
  Ceo_n= 0.0;
  Teo_n= 0.0;
  Cemax= 0.0;
  Temax= 0.0;
  Cemin= 0.0;
  Temin= 0.0;
  CeAbsMax= 0.0;
  TeAbsMax= 0.0;
  CeAbsMin= 0.0;
  TeAbsMin= 0.0;
  TeCumPlastic= 0.0;
  CeCumPlastic= 0.0;

  // reset state variables
  trial.revertToStart(Esp);
  converged.revertToStart(Esp);

  CFatDamage= 0.0;
  TFatDamage= 0.0;

  return 0;
}

//! @brief Virtual constructor.
XC::UniaxialMaterial * XC::ReinforcingSteel::getCopy(void) const
  { return new ReinforcingSteel(*this); }

//! @brief Send object members through the channel being passed as parameter.
int XC::ReinforcingSteel::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    // Strength degradation parameters
    res+= cp.sendVector(T_ePlastic,getDbTagData(),CommMetaData(2));
    res+= cp.sendVector(C_ePlastic,getDbTagData(),CommMetaData(3));
    // Converged Menegotto-Pinto Equation parameters
    res+= cp.sendVector(CR,getDbTagData(),CommMetaData(4));
    res+= cp.sendVector(Cfch,getDbTagData(),CommMetaData(5));
    res+= cp.sendVector(CQ,getDbTagData(),CommMetaData(6));
    res+= cp.sendVector(CEsec,getDbTagData(),CommMetaData(7));
    res+= cp.sendVector(Cea,getDbTagData(),CommMetaData(8));
    res+= cp.sendVector(Cfa,getDbTagData(),CommMetaData(9));
    res+= cp.sendVector(CEa,getDbTagData(),CommMetaData(10));
    res+= cp.sendVector(Ceb,getDbTagData(),CommMetaData(11));
    res+= cp.sendVector(Cfb,getDbTagData(),CommMetaData(12));
    res+= cp.sendVector(CEb,getDbTagData(),CommMetaData(13));

    res+= cp.sendDoubles(ZeroTol,reduction,fsu_fraction,beta,getDbTagData(),CommMetaData(14));
    // natural stress-strain variables
    res+= cp.sendDoubles(p,Esp,eshp,fshp,Eshp,getDbTagData(),CommMetaData(15));
    res+= cp.sendDoubles(esup,fsup,Esup,getDbTagData(),CommMetaData(16));
    res+= cp.sendDoubles(Eypp,fint,eyp,fyp,getDbTagData(),CommMetaData(17));



    res+= cp.sendDoubles(esh,Esh,eshpa,Eshpb,getDbTagData(),CommMetaData(18));

    res+= cp.sendDoubles(a1,hardLim,THardFact,CHardFact,getDbTagData(),CommMetaData(19));
    res+= cp.sendDoubles(TFatDamage,CFatDamage,LDratio,Fat1,Fat2,getDbTagData(),CommMetaData(20));
    res+= cp.sendDoubles(Deg1,BackStress,re,rE1,rE2,getDbTagData(),CommMetaData(21));



    // Menegotto-Pinto Calibration Constants
    res+= cp.sendDoubles(RC1,RC2,RC3,getDbTagData(),CommMetaData(22));

    // Menegotto-Pinto Equation parameters
    res+= cp.sendDoubles(TR,Tfch,TQ,TEsec,Tea,Tfa,getDbTagData(),CommMetaData(23));
    res+= cp.sendDoubles(TEa,Teb,Tfb,TEb,getDbTagData(),CommMetaData(24));

    res+= cp.sendDoubles(Teo_p,Teo_n,Temax,Temin,getDbTagData(),CommMetaData(25));
    res+= cp.sendDoubles(TeAbsMax,TeAbsMin,TeCumPlastic,getDbTagData(),CommMetaData(26));

    // Converged History Variables
    res+= cp.sendDoubles(Ceo_p,Ceo_n,Cemax,Cemin,getDbTagData(),CommMetaData(27));
    res+= cp.sendDoubles(CeAbsMax,CeAbsMin,CeCumPlastic,getDbTagData(),CommMetaData(28));

    res+= cp.sendMovable(converged,getDbTagData(),CommMetaData(29));
    res+= cp.sendMovable(trial,getDbTagData(),CommMetaData(30));

    res+= cp.sendInts(theBarFailed,BuckleModel,TBranchMem,TBranchNum,CBranchNum,getDbTagData(),CommMetaData(31));

    return res;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::ReinforcingSteel::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    // Strength degradation parameters
    res+= cp.receiveVector(T_ePlastic,getDbTagData(),CommMetaData(2));
    res+= cp.receiveVector(C_ePlastic,getDbTagData(),CommMetaData(3));
    // Converged Menegotto-Pinto Equation parameters
    res+= cp.receiveVector(CR,getDbTagData(),CommMetaData(4));
    res+= cp.receiveVector(Cfch,getDbTagData(),CommMetaData(5));
    res+= cp.receiveVector(CQ,getDbTagData(),CommMetaData(6));
    res+= cp.receiveVector(CEsec,getDbTagData(),CommMetaData(7));
    res+= cp.receiveVector(Cea,getDbTagData(),CommMetaData(8));
    res+= cp.receiveVector(Cfa,getDbTagData(),CommMetaData(9));
    res+= cp.receiveVector(CEa,getDbTagData(),CommMetaData(10));
    res+= cp.receiveVector(Ceb,getDbTagData(),CommMetaData(11));
    res+= cp.receiveVector(Cfb,getDbTagData(),CommMetaData(12));
    res+= cp.receiveVector(CEb,getDbTagData(),CommMetaData(13));

    res+= cp.receiveDoubles(ZeroTol,reduction,fsu_fraction,beta,getDbTagData(),CommMetaData(14));
    // natural stress-strain variables
    res+= cp.receiveDoubles(p,Esp,eshp,fshp,Eshp,getDbTagData(),CommMetaData(15));
    res+= cp.receiveDoubles(esup,fsup,Esup,getDbTagData(),CommMetaData(16));
    res+= cp.receiveDoubles(Eypp,fint,eyp,fyp,getDbTagData(),CommMetaData(17));



    res+= cp.receiveDoubles(esh,Esh,eshpa,Eshpb,getDbTagData(),CommMetaData(18));

    res+= cp.receiveDoubles(a1,hardLim,THardFact,CHardFact,getDbTagData(),CommMetaData(19));
    res+= cp.receiveDoubles(TFatDamage,CFatDamage,LDratio,Fat1,Fat2,getDbTagData(),CommMetaData(20));
    res+= cp.receiveDoubles(Deg1,BackStress,re,rE1,rE2,getDbTagData(),CommMetaData(21));



    // Menegotto-Pinto Calibration Constants
    res+= cp.receiveDoubles(RC1,RC2,RC3,getDbTagData(),CommMetaData(22));

    // Menegotto-Pinto Equation parameters
    res+= cp.receiveDoubles(TR,Tfch,TQ,TEsec,Tea,Tfa,getDbTagData(),CommMetaData(23));
    res+= cp.receiveDoubles(TEa,Teb,Tfb,TEb,getDbTagData(),CommMetaData(24));

    res+= cp.receiveDoubles(Teo_p,Teo_n,Temax,Temin,getDbTagData(),CommMetaData(25));
    res+= cp.receiveDoubles(TeAbsMax,TeAbsMin,TeCumPlastic,getDbTagData(),CommMetaData(26));

    // Converged History Variables
    res+= cp.receiveDoubles(Ceo_p,Ceo_n,Cemax,Cemin,getDbTagData(),CommMetaData(27));
    res+= cp.receiveDoubles(CeAbsMax,CeAbsMin,CeCumPlastic,getDbTagData(),CommMetaData(28));

    res+= cp.receiveMovable(converged,getDbTagData(),CommMetaData(29));
    res+= cp.receiveMovable(trial,getDbTagData(),CommMetaData(30));

    res+= cp.receiveInts(theBarFailed,BuckleModel,TBranchMem,TBranchNum,CBranchNum,getDbTagData(),CommMetaData(31));

    return res;
  }
//! @brief Sends object through the channel being passed as parameter.
int XC::ReinforcingSteel::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(32);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ReinforcingSteel::recvSelf(const CommParameters &cp)
  {
    inicComm(32);
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


void XC::ReinforcingSteel::Print(std::ostream &s, int flag)
{
  if(flag == 3) {
        s << converged.getStrain() << "  " << converged.getStress() << "  " << converged.getTangent() << std::endl;
  } else {
    s << "ReinforcingSteel, tag: " << this->getTag() << std::endl;
    s << "  N2p: " << CFatDamage << std::endl;
    //s << "  sigmaY: " << sigmaY << std::endl;
    //s << "  Hiso: " << Hiso << std::endl;
    //s << "  Hkin: " << Hkin << std::endl;
    //s << "  eta: " << eta << std::endl;
  }
}

// int XC::ReinforcingSteel::Sign(double x)
//   {
//     if(x<0.0)
//       return -1;
//     else
//       return 1;
//   }

/*****************************************************************************************/
/*************************     Base Menegotto-Pinto Equation       ***********************/
/*****************************************************************************************/
double XC::ReinforcingSteel::MPfunc(double a)
{
  if(a>=1.0)
    std::cerr << "a is one in XC::ReinforcingSteel::MPfunc()\n";
  //double temp1= pow(a,TR+1);
  //double temp2= pow(a,TR);
  //double temp3= TEa*a*(1-temp2)/(1-a);
  //double temp4= TEsec*(1-temp1)/(1-a);
  //return TEb-temp4+temp3;
  return TEb-TEsec*(1-pow(a,TR+1))/(1-a)+TEa*a*(1-pow(a,TR))/(1-a);
}

int XC::ReinforcingSteel::SetMP()
{
  double Rmin;
  double a=0.01;
  double ao;
  double da;
  bool notConverge(true);


        if (TEb-TEsec == 0.0) {
          TQ=1.0;
          Tfch=Tfb;
  } else {
    if (TEsec!=TEa) {
      Rmin= (TEb-TEsec)/(TEsec-TEa);
            if (Rmin < 0.0) {
                    std::cerr << "R is negative in XC::ReinforcingSteel::SetMP()\n";
                    Rmin= 0.0;
            }
            if (TR <= Rmin) TR=Rmin + 0.01;
            while(notConverge) {
      if (1.0-a != 1.0) {
              if (MPfunc(a)*MPfunc(1.0-a)>0.0)
                      a=a/2.0;
              else
                      notConverge=false;
      } else
                    notConverge=false;
          }

            ao= Rmin/TR;
      if (ao >= 1.0) ao=0.999999;
            notConverge=true;
            while(notConverge) {
      if (1.0-a != 1.0) {
              if (MPfunc(ao)*MPfunc(1.0-a)<0.0)
                      ao=sqrt(ao);
              else
                      notConverge=false;
      } else
        notConverge=false;
      if(ao > 0.999999) notConverge=false;
          }

            notConverge=true;
            if (ao >= 1.0) ao=0.999999;
            while(notConverge) {
      double ao_last=ao;

            da=0.49*(1-ao);
      if (da>ao/10.0) da=ao/10.0;
      if (ao+da>=1.0) da= (1.0-ao)/10.0;

      double tempdenom= MPfunc(ao+da)-MPfunc(ao-da);
      if (tempdenom != 0.0){
              ao=ao-2*MPfunc(ao)*da/tempdenom;
        if (ao>0.99999999999) ao=0.99999999999;
        if (ao < 0.0) {
          ao= 0.0;
          notConverge=false;
        }
      }
  #ifdef _WIN32
      if(_fpclass(ao)< 8 || _fpclass(ao)==512) {
        std::cerr << "Stuck in infinite loop, return error, XC::ReinforcingSteel::SetMP()\n";
        da=da/100.0;
        ao=ao_last;
        ao=ao-2*MPfunc(ao)*da/(MPfunc(ao+da)-MPfunc(ao-da));
        return -1;
      }
  #endif
      if(fabs(ao_last-ao)<0.0001) notConverge=false;
          }
            if (ao>0.99999999) ao=0.99999999;
    } else
      ao=0.99999999;
    TQ=(TEsec/TEa-ao)/(1-ao);
          double temp1= pow(ao,TR);
    double temp2= pow(1.0-temp1,1.0/TR);
          double b=temp2/ao;
          Tfch=Tfa+TEa/b*(Teb-Tea);
  }
  if(fabs(Teb-Tea)<1.0e-7)
    TQ= 1.0;
  return 0;
}

double XC::ReinforcingSteel::MP_f(double e) const
  { return Tfa+TEa*(e-Tea)*(TQ-(TQ-1.0)/pow(pow(fabs(TEa*(e-Tea)/(Tfch-Tfa)),TR)+1.0,1/TR)); }

double XC::ReinforcingSteel::MP_E(double e) const
  {
    if(TR>100.0 || e==Tea)
      { return TEa; }
    else
      {
        const double Esec=(MP_f(e)-Tfa)/(e-Tea);
        return Esec-(Esec-TQ*TEa)/(pow(fabs(TEa*(e-Tea)/(Tfch-Tfa)),-TR)+1.0);
      }
  }

void  XC::ReinforcingSteel::SetTRp(void)
  { TR=pow(fyp/Esp,RC1)*RC2*(1.0-RC3*(Teb-Tea)); }

void XC::ReinforcingSteel::SetTRn(void)
  { TR=pow(fyp/Esp,RC1)*RC2*(1.0-RC3*(Tea-Teb)); }

void XC::ReinforcingSteel::SetTRp1(void)
  { TR=pow(fyp/Esp,RC1)*RC2*(1.0-RC3*(Teb-Tea)); }

void XC::ReinforcingSteel::SetTRn1(void)
  { TR=pow(fyp/Esp,RC1)*RC2*(1.0-RC3*(Tea-Teb)); }

void XC::ReinforcingSteel::SetPastCurve(int branch)
  {
    if(branch == 1)
      TBranchMem=0;
    else
      TBranchMem= (branch+1)/2;

    Tea= Cea[TBranchMem];
    Teb= Ceb[TBranchMem];
    Tfa= Cfa[TBranchMem];
    Tfb= Cfb[TBranchMem];
    TEa= CEa[TBranchMem];
    TEb= CEb[TBranchMem];
    TR= CR [TBranchMem];
    Tfch= Cfch[TBranchMem];
    TQ= CQ [TBranchMem];
    TEsec= CEsec[TBranchMem];
  }
/*****************************************************************************************/
/***********************        Base Stress-Strain Relations         *********************/
/*****************************************************************************************/
double XC::ReinforcingSteel::Backbone_f(double e) const
  {
    if(e<0.0)
      return -Backbone_fNat(fabs(e));
    else
      return  Backbone_fNat(fabs(e));
  }

double XC::ReinforcingSteel::Backbone_fNat(double essp) const
  {
    if(essp>eshpa)
      {
        if(essp>esup)
          return fsup + (essp-eshp)*Esup;
        else
          {
            if(essp < eshp+0.0002)
              return (Eshpb-Eypp)*pow(essp-eshpa,2.0)/(2*(eshp+0.0002-eshpa))+ essp*Eypp + fint;
            else
              return fshp + (essp-eshp)*Esup + (fsup-fshp)*(1.0-pow((esup-essp)/(esup-eshp),p));
          }
      }
    else
      return essp * ((Esp - Eypp) / pow(1 + pow((Esp - Eypp) * essp / fint,10.0),0.1) + Eypp);
  }

double XC::ReinforcingSteel::Backbone_E(double e) const
  {
    double essp= fabs(e);

    if(essp<=eshpa)
      return (Esp - Eypp) / pow(1.0 + pow((Esp - Eypp) * essp / fint,10.0),1.1) + Eypp;
    else
      {
        if(essp>esup)
          return Esup;
        else
          {
            if(essp < eshp+0.0002)
              return (Eshpb-Eypp)*(essp-eshpa)/(eshp+0.0002-eshpa) + Eypp;
            else
              {
        //double temp1= (fsup-fshp-(fsup-fshp)*(1.0-pow((esup-essp)/(esup-eshp),p)));
                return Eshp*pow((fsup-fshp-(fsup-fshp)*(1.0-pow((esup-essp)/(esup-eshp),p)))/(fsup-fshp),1.0-1.0/p)+Esup;
              }
          }
      }
  }

/*****************************************************************************************/
/***********************       Buckled Stress-Strain Relations       *********************/
/*****************************************************************************************/

double XC::ReinforcingSteel::Buckled_stress_Dhakal(double ess, double fss) const
  {
    if(LDratio <= 0.0) return fss;
    double aveStress= 0.0;
    const double e_cross= Temax - fsup/Esp;
    const double e=ess-e_cross;

    if(e < -eyp)
      {
        double eStar=55.0-2.3*sqrt(fyp/Esp*2000)*LDratio;
        if(eStar < 7.0) eStar=7.0;
        eStar= -eStar*eyp;
        const double fStarL=Backbone_f(eStar);
        double fStar=fStarL*beta*(1.1 - 0.016*sqrt(fyp/Esp*2000)*LDratio);
        if(fStar > -0.2*fyp)
          fStar= -0.2*fyp;
        if(TBranchNum%4 > 1)
          {
            if((e< -eyp) && (e>=eStar))
              { aveStress= fss*(1.0-(1.0-fStar/fStarL)*(e+eyp)/(eStar+eyp)); }
            else if(e<eStar)
              {
                aveStress= fss*(fStar-0.02*Esp*(e-eStar))/fStarL;
                if(aveStress>-0.2*fyp)
                  aveStress=-0.2*fyp;
              }
            return aveStress;
          }
        else
          {
            if(TBranchNum == 4 || TBranchNum == 5)
              BackStress= MP_f(e_cross-eyp);
            if((e< -eyp) && (e>=eStar))
              { aveStress= Tfa*(1.0-(1.0-fStar/fStarL)*(e+eyp)/(eStar+eyp)); }
            else if(e<eStar)
              {
                aveStress= Tfa*(fStar-0.02*Esp*(e-eStar))/fStarL;
                if(aveStress>-0.2*fyp)
                  aveStress=-0.2*fyp;
              }
            return BackStress - (BackStress-fss)*(BackStress-aveStress)/(BackStress-Tfa);
          }
      }
    else
      { return fss; }
  }

double XC::ReinforcingSteel::Buckled_stress_Gomes(double ess, double fss) const
  {
        if (LDratio <= 0.0) return fss;

        double e_cross= Temax - fsup/Esp;
        if (ess>=e_cross) return fss;

        double beta=1.0;
        double gama= 0.1;
        double Dft= 0.25;

        double fs_buck= beta*sqrt(32.0/(e_cross-ess))/(9.42477796076938*LDratio);
        double stress_diff=fabs(fs_buck-1.0);
        if (stress_diff <= Dft)        beta= 1-gama*(Dft-stress_diff)/Dft;

        //double factor= ((1.0>fs_buck)?fs_buck:1.0)*beta + reduction)/(1.0+reduction);
        double factor= ((1.0>fs_buck)?fs_buck:1.0)*beta*(1-reduction)+reduction;

        double t_s_out= fsup*fsu_fraction-(factor+fsu_fraction)*(fsup*fsu_fraction-fss)/(1.0+fsu_fraction);
        return t_s_out;
  }

double XC::ReinforcingSteel::Buckled_mod_Gomes(double ess, double fss, double Ess) const
  {
        double Etmp= Ess + (Buckled_stress_Gomes(ess+0.00005, fss)-Buckled_stress_Gomes(ess-0.00005, fss))/0.0001;
        return Etmp;
  }
double XC::ReinforcingSteel::Buckled_mod_Dhakal(double ess, double fss, double Ess) const
  {
    double Etmp= Ess + (Buckled_stress_Dhakal(ess+0.00005, fss)-Buckled_stress_Gomes(ess-0.00005, fss))/0.0001;
    return Etmp;
  }
/*****************************************************************************************/
/*************************        Branch Rule Definitions          ***********************/
/*****************************************************************************************/

int XC::ReinforcingSteel::BranchDriver(int res)
{
  switch(TBranchNum) {
  case  1:        res += Rule1(res);
                        break;
  case  2:        res += Rule2(res);
                        break;
  case  3:        res += Rule3(res);
                        break;
  case  4:        res += Rule4(res);
                        break;
  case  5:        res += Rule5(res);
                        break;
  case  6:        res += Rule6(res);
                        break;
  case  7:        res += Rule7(res);
                        break;
  case  8:        res += Rule8(res);
                        break;
  case -1:  trial.Stress()= 0.0;
                        trial.Tangent()= Esp/1000000.0;
                        break;
  case  0:  trial.Stress()= 0.0;
                        trial.Tangent()= Esp;
                        break;
  default:        switch(TBranchNum%4) {
                        case 0: res += Rule12(res);
                                break;
                        case 1: res += Rule9(res);
                                break;
                        case 2: res += Rule10(res);
                                break;
                        case 3: res += Rule11(res);
                                        break;
                        }
                        break;
  }
  return res;
}

// Rule 1: Tension Envelope Branch
int
XC::ReinforcingSteel::Rule1(int res)
{
  double strain=trial.getStrain()-Teo_p;
  // check for load reversal
  if (trial.getStrain()-converged.getStrain()<0.0) {
          if (strain - eshp > -ZeroTol) {
            double emin;
            // reversal from strain hardening range
            Tea=converged.getStrain();
            Temax=Tea-Teo_p;
            if (converged.getStrain() > TeAbsMax) TeAbsMax= converged.getStrain();

            if(Temin>-eshp)
                    emin=-eshp-1.0E-14;
            else
                    emin=Temin;

            double ea= Teo_p + eshp - fshp/Esp;
            double eb= Teo_p + Temax - converged.getStress()/Esp;
            double krev= exp(-Temax/(5000*eyp*eyp));
            double eon= ea*krev+eb*(1.0-krev);
      if (eon > Teo_n) {
        emin-=(eon-Teo_n);
        Teo_n=eon;
      }
            Teb=Teo_n+emin;

            // set stress dependent curve parameters
            Tfa= converged.getStress();
            Cfa[0]= converged.getStress();
            TEa=ReturnSlope(Tea-Teo_n-Temin);

            updateHardeningLoaction(TeCumPlastic+Tea-emin-(Tfa-Backbone_f(emin))/Esp);
            Tfb= Backbone_f(emin);
            TEb= Backbone_E(emin);
                  TEsec= (Tfb-Tfa)/(Teb-Tea);

                  if(TEsec < TEb) {
                          Teo_n= (Tfb-Tfa)/TEb+Tea - emin;
                          Teb=Teo_n+emin;
                          TEsec= (Tfb-Tfa)/(Teb-Tea);
                          std::cerr << "Adjusted Compressive Curve anchor in XC::ReinforcingSteel::Rule1()\n";
                  }

            SetTRn();
            res += SetMP();

            T_ePlastic[2]=0.0;
            TBranchNum=3;
            Rule3(res);
          } else if (strain - eyp > -ZeroTol) {
            double emin;
            // Reversal from Yield Plateau
            Tea=converged.getStrain();
            Temax=Tea-Teo_p;
            if (converged.getStrain() > TeAbsMax) TeAbsMax= converged.getStrain();

            Tfa=converged.getStress();
      Cfa[0]=converged.getStress();
            TEa=ReturnSlope(Tea-Teo_n-Temin);

            double pr=(Temax-eyp)/(eshp-eyp);
            emin=pr*(eyp-eshp)-eyp;
            Teo_n=Tea-Tfa/Esp;
            Teb=Teo_n+emin;

            // set stress dependent curve parameters
      updateHardeningLoaction(TeCumPlastic+Tea-emin-(Tfa-Backbone_f(emin))/Esp);
            Tfb=Backbone_f(emin);
            TEb=(1.0/(1.0/Esp+pr*(1.0/Eshp - 1.0/Esp)));

            SetTRn();
            TEsec= (Tfb-Tfa)/(Teb-Tea);
            if (TEsec<TEb) TEb=TEsec*0.999;
            if (TEsec>TEa) TEa=TEsec*1.001;
            res += SetMP();

            T_ePlastic[2]=0.0;
            TBranchNum=3;
            Rule3(res);
          } else if (strain > -ZeroTol) {
            //if(Temax < strain) Temax=strain;
            trial.Stress()= Backbone_f(strain);
            trial.Tangent()= Backbone_E(strain);
          } else {
            TBranchNum=2;
            Rule2(res);
          }
  } else {
    trial.Stress()= Backbone_f(strain);
          trial.Tangent()= Backbone_E(strain);
          //if(Temin<0.0) {
            TFatDamage-=damage(T_ePlastic[0]);
      TeCumPlastic -= T_ePlastic[0];
            T_ePlastic[0]=getPlasticStrain(trial.getStrain()-TeAbsMin,trial.getStress()-Cfa[1]);
            TFatDamage+=damage(T_ePlastic[0]);
      TeCumPlastic += T_ePlastic[0];
          //}
  }
  return res;
}

// Rule 2: Compression Envelope Branch
int
XC::ReinforcingSteel::Rule2(int res)
{
  double strain= trial.getStrain()-Teo_n;

  // check for load reversal
  if (trial.getStrain()-converged.getStrain()>0.0) {
        if (strain+eshp< ZeroTol) {
          double emax;
          // reversal from strain hardening range
          Tea=converged.getStrain();
          Temin=Tea-Teo_n;
          if (converged.getStrain() < TeAbsMin) TeAbsMin= converged.getStrain();

          if(Temax<eshp)
                emax=eshp+1.0E-14;
          else
                emax=Temax;

          double ea= Teo_n - eshp + fshp/Esp;
          double eb= Teo_n + Temin - converged.getStress()/Esp;
          double krev= exp(Temin/(5000*eyp*eyp));
          double eop=ea*krev+eb*(1.0-krev);
    if (eop<Teo_p) {
      emax+=(Teo_p-eop);
      Teo_p=eop;
    }
      Teb=Teo_p+emax;

          // set stress dependent curve parameters
          Tfa=converged.getStress();
    Cfa[1]=converged.getStress();
          TEa=ReturnSlope(Temax + Teo_p -Tea);

    updateHardeningLoaction(TeCumPlastic+emax-Tea-(Backbone_f(emax)-Tfa)/Esp);
          Tfb= Backbone_f(emax);
          TEb= Backbone_E(emax);

          SetTRp();
          TEsec= (Tfb-Tfa)/(Teb-Tea);
          res += SetMP();

          T_ePlastic[2]=0.0;
          TBranchNum=4;
          Rule4(res);

        } else if (strain+eyp < ZeroTol) {
          double emax;
          // reversal form yield plateau
          Tea=converged.getStrain();
          Temin=Tea-Teo_n;
          if (converged.getStrain() < TeAbsMin) TeAbsMin= converged.getStrain();

          Tfa=converged.getStress();
    Cfa[1]=converged.getStress();
          TEa=ReturnSlope(Temax + Teo_p -Tea);

          double pr=(Temin+eyp)/(eyp-eshp);
          emax=eyp+pr*(eshp-eyp);
          Teo_p=Tea-Tfa/Esp;
          Teb=Teo_p+emax;

          // stress dependent curve parameters
    updateHardeningLoaction(TeCumPlastic+emax-Tea-(Backbone_f(emax)-Tfa)/Esp);
          Tfb= Backbone_f(emax);
          TEb=1.0/(1.0/Esp+pr*(1.0/Eshp - 1.0/Esp));

          SetTRp();
          TEsec= (Tfb-Tfa)/(Teb-Tea);
          if (TEsec<TEb) TEb=TEsec*0.999;
          if (TEsec>TEa) TEa=TEsec*1.001;
          res += SetMP();

          T_ePlastic[2]=0.0;
          TBranchNum=4;
          Rule4(res);

        } else if (strain<ZeroTol) {
          //if(Temin>strain) Temin=strain;
          trial.Stress()= Backbone_f(strain);
          trial.Tangent()= Backbone_E(strain);
        } else {
          TBranchNum=1;
          Rule1(res);
        }
  } else {
    trial.Stress()= Backbone_f(strain);
    trial.Tangent()= Backbone_E(strain);
          //if(Temax>0.0) {
            TFatDamage-=damage(T_ePlastic[1]);
      TeCumPlastic -= T_ePlastic[1];
            T_ePlastic[1]=getPlasticStrain(TeAbsMax-trial.getStrain(),Cfa[0]-trial.getStress());
            TFatDamage+=damage(T_ePlastic[1]);
      TeCumPlastic += T_ePlastic[1];
          //}
  }
  return res;
}

// Rule 3: Unloading Reversal Branch
int
XC::ReinforcingSteel::Rule3(int res)
{
  if(trial.getStrain()-converged.getStrain() > 0.0)
    {        // reversal from branch
        if(Temin > converged.getStrain()-Teo_n) Temin=converged.getStrain()-Teo_n;

        Tea=converged.getStrain();
        double dere= Cea[2]-Tea-fyp/(1.2*Esp);
    if (dere<0.0)
          dere=0.0;
        else if (dere>fyp/3/Esp)
          dere=fyp/3/Esp;
        Teb=Teo_p+Temax+dere;

        Tfa=converged.getStress();
        TEa=ReturnSlope(Cea[2]-converged.getStrain());

  updateHardeningLoaction(TeCumPlastic+Teb-Tea-(Backbone_f(Teb-Teo_p)-Tfa)/Esp);
        Tfb= Backbone_f(Teb-Teo_p);
        TEb= Backbone_E(Teb-Teo_p);

        SetTRp();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        if (TEsec<TEb) TEb=TEsec*0.999;
        if (TEsec>TEa) TEa=TEsec*1.001;
        res += SetMP();
        #ifdef _WIN32
  if(_fpclass(trial.getStress())< 8 || _fpclass(trial.getStress())==512 || _fpclass(trial.getTangent())< 8 || _fpclass(trial.getTangent())==512) {
    std::cerr << "bad stress or tangent\n";
    return -1;
  }
#endif
        T_ePlastic[3]=0.0;
        TBranchNum=5;
        Rule5(res);
  } else {
          if (trial.getStrain() - Teb <= ZeroTol) {
            T_ePlastic[1]=T_ePlastic[2];
            TBranchNum=2;
            Rule2(res);
          } else {
      trial.Stress()= MP_f(trial.getStrain());
            trial.Tangent()= MP_E(trial.getStrain());
            TFatDamage -=damage(T_ePlastic[2]);
      TeCumPlastic -= T_ePlastic[2];
            T_ePlastic[2]=getPlasticStrain(TeAbsMax-trial.getStrain(),Tfa-trial.getStress());
            TFatDamage +=damage(T_ePlastic[2]);
      TeCumPlastic += T_ePlastic[2];
          }
  }
  return res;
}

// Rule 4: Loading Reversal Branch
int
XC::ReinforcingSteel::Rule4(int res)
{
  if (trial.getStrain()-converged.getStrain() < 0.0) {
        if(Temax<converged.getStrain()-Teo_p) Temax=converged.getStrain()-Teo_p;

        Tea=converged.getStrain();
        double dere= Cea[2]-Tea+fyp/(1.2*Esp);
    if (dere>0.0)
          dere=0.0;
        else if (dere<-fyp/3/Esp)
          dere=-fyp/3/Esp;
        Teb=Teo_n+Temin+dere;

        Tfa=converged.getStress();
        TEa=ReturnSlope(converged.getStrain()-Cea[2]);

  updateHardeningLoaction(TeCumPlastic+Tea-Teb-(Tfa-Backbone_f(Teb-Teo_n))/Esp);
        Tfb= Backbone_f(Teb-Teo_n);
        TEb= Backbone_E(Teb-Teo_n);

        SetTRn();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        if (TEsec<TEb) TEb=TEsec*0.999;
        if (TEsec>TEa) TEa=TEsec*1.001;
        res += SetMP();

        T_ePlastic[3]=0.0;
        TBranchNum=6;
        Rule6(res);
  } else {
          if (trial.getStrain() - Teb >= -ZeroTol) {
            T_ePlastic[0]=T_ePlastic[2];
            TBranchNum=1;
            Rule1(res);
          } else {
            trial.Stress()= MP_f(trial.getStrain());
            trial.Tangent()= MP_E(trial.getStrain());
            TFatDamage-=damage(T_ePlastic[2]);
      TeCumPlastic -= T_ePlastic[2];
            T_ePlastic[2]=getPlasticStrain(trial.getStrain()-TeAbsMin,trial.getStress()-Tfa);
            TFatDamage+=damage(T_ePlastic[2]);
      TeCumPlastic += T_ePlastic[2];
          }
  }
  return res;
}

// Rule 5: Loading Returning Branch
int
XC::ReinforcingSteel::Rule5(int res)
{
  if (trial.getStrain()-converged.getStrain() < 0.0) {
        rE1=0.0;
        rE2=0.0;

        Tea= Ceb[3]*(converged.getStrain()-Cea[3])/(Ceb[3]-Cea[3]) + Cea[2]*(Ceb[3]-converged.getStrain())/(Ceb[3]-Cea[3]);
        Teb= Ceb[2];

  updateHardeningLoaction(TeCumPlastic+converged.getStrain()-Tea+(Backbone_f(Tea-Teo_p)-converged.getStress())/Esp);
        Tfa= Backbone_f(Tea-Teo_p);
        TEa= CEa[2];

  updateHardeningLoaction(TeCumPlastic+converged.getStrain()-Teb-(converged.getStress()-Backbone_f(Teb-Teo_n))/Esp);
        Tfb= Backbone_f(Teb-Teo_n);
        TEb= Backbone_E(Teb-Teo_n);

        SetTRn();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        res += SetMP();

        double fb=MP_f(Cea[3]);
        double Eb=MP_E(Cea[3]);

        Tea=converged.getStrain();
        Tfa=converged.getStress();
        TEa=ReturnSlope(converged.getStrain()-Cea[3]);
        Teb=Cea[3];
        Tfb=fb;
        TEb=Eb;

        SetTRn();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        if (TEsec<TEb) TEb=TEsec*0.999;
        if (TEsec>TEa) TEa=TEsec*1.001;
        res += SetMP();

        T_ePlastic[4]=0.0;
        TBranchNum=7;
        Rule7(res);
  } else {
          if (trial.getStrain() - Teb >= -ZeroTol) {
            TFatDamage-=damage(T_ePlastic[3]);
      TeCumPlastic -= T_ePlastic[3];
      double TempPStrain= getPlasticStrain(Teb-Tea,Tfb-Tfa);
            TFatDamage+=damage(TempPStrain);
      TeCumPlastic += TempPStrain;
            TBranchNum=1;
            Rule1(res);
          } else {
            trial.Stress()= MP_f(trial.getStrain());
            trial.Tangent()= MP_E(trial.getStrain());
            TFatDamage-=damage(T_ePlastic[3]);
      TeCumPlastic -= T_ePlastic[3];
            T_ePlastic[3]=getPlasticStrain(trial.getStrain()-Tea,trial.getStress()-Tfa);
            TFatDamage+=damage(T_ePlastic[3]);
      TeCumPlastic += T_ePlastic[3];
          }
  }
  return res;
}

// Rule 6: Unloading Returning Branch
int
XC::ReinforcingSteel::Rule6(int res)
{
  if (trial.getStrain()-converged.getStrain() > 0.0) {
        rE1=0.0;
        rE2=0.0;

        Tea= Ceb[3]*(converged.getStrain()-Cea[3])/(Ceb[3]-Cea[3]) + Cea[2]*(Ceb[3]-converged.getStrain())/(Ceb[3]-Cea[3]);
        Teb= Ceb[2];

  updateHardeningLoaction(TeCumPlastic+Tea-converged.getStrain()+(converged.getStress()-Backbone_f(Tea-Teo_n))/Esp);
        Tfa= Backbone_f(Tea-Teo_n);
        TEa= CEa[2];

  updateHardeningLoaction(TeCumPlastic+Teb-converged.getStrain()-(Backbone_f(Teb-Teo_p)-converged.getStress())/Esp);
        Tfb= Backbone_f(Teb-Teo_p);
        TEb= Backbone_E(Teb-Teo_p);

        SetTRp();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        res += SetMP();

        double fb=MP_f(Cea[3]);
        double Eb=MP_E(Cea[3]);

        Tea=converged.getStrain();
        Tfa=converged.getStress();
        TEa=ReturnSlope(Cea[3]-converged.getStrain());
        Teb=Cea[3];
        Tfb=fb;
        TEb=Eb;

        SetTRp();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        if (TEsec<TEb) TEb=TEsec*0.999;
        if (TEsec>TEa) TEa=TEsec*1.001;
        res += SetMP();

        T_ePlastic[4]=0.0;
        TBranchNum=8;
        Rule8(res);
  } else {
          if (trial.getStrain() - Teb <= ZeroTol) {
            TFatDamage-=damage(T_ePlastic[3]);
      TeCumPlastic -= T_ePlastic[3];
      double TempPStrain= getPlasticStrain(Tea-Teb,Tfa-Tfb);
            TFatDamage+=damage(TempPStrain);
      TeCumPlastic += TempPStrain;
            TBranchNum=2;
            Rule2(res);
          } else {
            trial.Stress()= MP_f(trial.getStrain());
            trial.Tangent()= MP_E(trial.getStrain());
            TFatDamage-=damage(T_ePlastic[3]);
      TeCumPlastic -= T_ePlastic[3];
            T_ePlastic[3]=getPlasticStrain(Tea-trial.getStrain(),Tfa-trial.getStress());
            TFatDamage+=damage(T_ePlastic[3]);
      TeCumPlastic += T_ePlastic[3];
          }
  }
  return res;
}

// Rule 7: Unloading First Transition Branch
int
XC::ReinforcingSteel::Rule7(int res)
{
  if (trial.getStrain()-converged.getStrain() > 0.0) {
        SetPastCurve(TBranchNum-2);

        double fb=MP_f(Cea[4]);
        double Eb=MP_E(Cea[4]);

        Tea=converged.getStrain();
        Tfa=converged.getStress();
        TEa=ReturnSlope(Cea[4]-converged.getStrain());
        Teb=Cea[4];
        Tfb=fb;
        TEb=Eb;

        SetTRp1();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        if (TEsec<TEb) TEb=TEsec*0.999;
        if (TEsec>TEa) TEa=TEsec*1.001;
        res += SetMP();

        re=Tea;

        T_ePlastic[5]=0.0;
        TBranchNum=9;
        Rule9(res);
  } else {
          if (trial.getStrain() - Teb <= ZeroTol) {
            TFatDamage-=damage(T_ePlastic[4]);
      TeCumPlastic -= T_ePlastic[4];
      double TempPStrain= getPlasticStrain(Tea-Teb,Tfa-Tfb);
            TFatDamage+=damage(TempPStrain);
      TeCumPlastic += TempPStrain;
      double tempTeb= Teb;

            Tea= Ceb[3]*(Tea-Cea[3])/(Ceb[3]-Cea[3]) + Cea[2]*(Ceb[3]-Tea)/(Ceb[3]-Cea[3]);
      Teb= Ceb[2];

      updateHardeningLoaction(TeCumPlastic+tempTeb-Tea+(Backbone_f(Tea-Teo_p)-Tfb)/Esp);
            Tfa= Backbone_f(Tea-Teo_p);
            TEa= CEa[2];

      updateHardeningLoaction(TeCumPlastic+tempTeb-Teb-(Tfb-Backbone_f(Teb-Teo_n))/Esp);
            Tfb= Backbone_f(Teb-Teo_n);
            TEb= Backbone_E(Teb-Teo_n);

            SetTRn();
            TEsec= (Tfb-Tfa)/(Teb-Tea);
            res += SetMP();

            TBranchNum=3;
            Rule3(res);
          } else {
            trial.Stress()= MP_f(trial.getStrain());
            trial.Tangent()= MP_E(trial.getStrain());
            TFatDamage-=damage(T_ePlastic[4]);
      TeCumPlastic -= T_ePlastic[4];
            T_ePlastic[4]=getPlasticStrain(Tea-trial.getStrain(),Tfa-trial.getStress());
            TFatDamage+=damage(T_ePlastic[4]);
      TeCumPlastic += T_ePlastic[4];
          }
  }
  return res;
}

// Rule 8: Loading First Transition Branch
int
XC::ReinforcingSteel::Rule8(int res)
{
  if (trial.getStrain()-converged.getStrain() < 0.0) {
        SetPastCurve(TBranchNum-2);

        double fb=MP_f(Cea[4]);
        double Eb=MP_E(Cea[4]);

        Tea=converged.getStrain();
        Tfa=converged.getStress();
        TEa=ReturnSlope(converged.getStrain()-Cea[4]);
        Teb=Cea[4];
        Tfb=fb;
        TEb=Eb;

        SetTRn1();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        if (TEsec<TEb) TEb=TEsec*0.999;
        if (TEsec>TEa) TEa=TEsec*1.001;
        res += SetMP();

        re=Tea;

        T_ePlastic[5]=0.0;
        TBranchNum=10;
        Rule10(res);
  } else {
          if (trial.getStrain() - Teb >= -ZeroTol) {
            TFatDamage-=damage(T_ePlastic[4]);
      TeCumPlastic -= T_ePlastic[4];
      double TempPStrain= getPlasticStrain(Teb-Tea,Tfb-Tfa);
            TFatDamage+=damage(TempPStrain);
      TeCumPlastic += TempPStrain;
            double tempTeb= Teb;

            Tea= Ceb[3]*(Tea-Cea[3])/(Ceb[3]-Cea[3]) + Cea[2]*(Ceb[3]-Tea)/(Ceb[3]-Cea[3]);
            Teb= Ceb[2];

      updateHardeningLoaction(TeCumPlastic+Tea-tempTeb+(Tfb-Backbone_f(Tea-Teo_n))/Esp);
            Tfa= Backbone_f(Tea-Teo_n);
            TEa= CEa[2];

      updateHardeningLoaction(TeCumPlastic+Teb-tempTeb-(Backbone_f(Teb-Teo_p)-Tfb)/Esp);
            Tfb= Backbone_f(Teb-Teo_p);
            TEb= Backbone_E(Teb-Teo_p);

            SetTRp();
            TEsec= (Tfb-Tfa)/(Teb-Tea);
            res += SetMP();

            TBranchNum=4;
            Rule4(res);
          } else {
            trial.Stress()= MP_f(trial.getStrain());
            trial.Tangent()= MP_E(trial.getStrain());
            TFatDamage-=damage(T_ePlastic[4]);
      TeCumPlastic -= T_ePlastic[4];
            T_ePlastic[4]=getPlasticStrain(trial.getStrain()-Tea,trial.getStress()-Tfa);
            TFatDamage+=damage(T_ePlastic[4]);
      TeCumPlastic += T_ePlastic[4];
          }
  }
  return res;
}

// Rule 9: Loading Second Transition Branch
int
XC::ReinforcingSteel::Rule9(int res)
{
  if (trial.getStrain()-converged.getStrain() < 0.0) {
        double eb= Tea;
        if (TBranchNum+4<=LastRule_RS) re=Tea;
        SetPastCurve(TBranchNum-2);

        // set new curve
        double fb=MP_f(re);
        double Eb=MP_E(re);
        Tea=converged.getStrain();
        Tfa=converged.getStress();
        TEa=ReturnSlope(converged.getStrain()-eb);
        Teb=re;
        Tfb=fb;
        TEb=Eb;
        SetTRn1();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        if (TEsec<TEb) TEb=TEsec*0.999;
        if (TEsec>TEa) TEa=TEsec*1.001;
        res += SetMP();

        TBranchNum+=2;
        TBranchMem= (TBranchNum+1)/2;
        T_ePlastic[TBranchMem]=0.0;
        Rule11(res);
  } else {
          if (trial.getStrain() - Teb >= -ZeroTol) {
                  TBranchMem= (TBranchNum+1)/2;
            TFatDamage -=damage(T_ePlastic[TBranchMem]);
      TeCumPlastic -= T_ePlastic[TBranchMem];
      double TempPStrain= getPlasticStrain(Teb-Tea,Tfb-Tfa);
            TFatDamage +=damage(TempPStrain);
      TeCumPlastic += TempPStrain;
                  TBranchNum-=4;
            SetPastCurve(TBranchNum);
            if (TBranchNum==5)
                  Rule5(res);
            else
                  Rule9(res);
          } else {
      trial.Stress()= MP_f(trial.getStrain());
            trial.Tangent()= MP_E(trial.getStrain());
                  TBranchMem= (TBranchNum+1)/2;
            TFatDamage -=damage(T_ePlastic[TBranchMem]);
      TeCumPlastic -= T_ePlastic[TBranchMem];
            T_ePlastic[TBranchMem]=getPlasticStrain(trial.getStrain()-Tea,trial.getStress()-Tfa);
            TFatDamage +=damage(T_ePlastic[TBranchMem]);
      TeCumPlastic += T_ePlastic[TBranchMem];
          }
  }
  return res;
}

// Rule 10: Unloading Second Transition Branch
int
XC::ReinforcingSteel::Rule10(int res)
{
  if (trial.getStrain()-converged.getStrain() > 0.0) {
        double eb= Tea;
        if (TBranchNum+4<=LastRule_RS)
          re=Tea;

        SetPastCurve(TBranchNum-2);

        // set new curve
        double fb=MP_f(re);
        double Eb=MP_E(re);
        Tea=converged.getStrain();
        Tfa=converged.getStress();
        TEa=ReturnSlope(eb-converged.getStrain());
        Teb=re;
        Tfb=fb;
        TEb=Eb;
        SetTRp1();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        if (TEsec<TEb) TEb=TEsec*0.999;
        if (TEsec>TEa) TEa=TEsec*1.001;
        res += SetMP();

        TBranchNum+=2;
        TBranchMem= (TBranchNum+1)/2;
        T_ePlastic[TBranchMem]=0.0;
        Rule12(res);
  } else {
          if (trial.getStrain() - Teb <= ZeroTol) {
                  TBranchMem= (TBranchNum+1)/2;
            TFatDamage-=damage(T_ePlastic[TBranchMem]);
      TeCumPlastic -= T_ePlastic[TBranchMem];
      double TempPStrain= getPlasticStrain(Tea-Teb,Tfa-Tfb);
            TFatDamage+=damage(TempPStrain);
      TeCumPlastic += TempPStrain;

                  TBranchNum-=4;
            SetPastCurve(TBranchNum);
            if (TBranchNum==6)
                  Rule6(res);
            else
                  Rule10(res);
          } else {
            trial.Stress()= MP_f(trial.getStrain());
            trial.Tangent()= MP_E(trial.getStrain());
                  TBranchMem= (TBranchNum+1)/2;
            TFatDamage  -=damage(T_ePlastic[TBranchMem]);
      TeCumPlastic -= T_ePlastic[TBranchMem];
            T_ePlastic[TBranchMem]=getPlasticStrain(Tea-trial.getStrain(),Tfa-trial.getStress());
            TFatDamage  +=damage(T_ePlastic[TBranchMem]);
      TeCumPlastic += T_ePlastic[TBranchMem];
          }
  }
  return res;
}

// Rule 11: Unloading Third Transition Branch
int
XC::ReinforcingSteel::Rule11(int res)
{
  if (trial.getStrain()-converged.getStrain() > 0.0) {
        // reset past curve
        double eb=Tea;
        if(TBranchNum+2>LastRule_RS) {
                TBranchMem= (TBranchNum+1)/2;
          eb=Cea[TBranchMem-2];
          SetPastCurve(TBranchNum-6);
        } else {
          SetPastCurve(TBranchNum-2);
        }
        double fb=MP_f(eb);
        double Eb=MP_E(eb);
        Tea=converged.getStrain();
        Tfa=converged.getStress();
        TEa=ReturnSlope(eb-converged.getStrain());
        Teb=eb;
        Tfb=fb;
        TEb=Eb;
        SetTRp1();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        if (TEsec<TEb) TEb=TEsec*0.999;
        if (TEsec>TEa) TEa=TEsec*1.001;
        res += SetMP();

        if(TBranchNum+2>LastRule_RS)
          TBranchNum-=2;
        else
          TBranchNum+=2;

        TBranchMem= (TBranchNum+1)/2;
        T_ePlastic[TBranchMem]=0.0;
        Rule9(res);
  } else {
          if (trial.getStrain() - Teb <= ZeroTol) {
                  TBranchMem= (TBranchNum+1)/2;
            TFatDamage-=damage(T_ePlastic[TBranchMem-2]);
      TeCumPlastic -= T_ePlastic[TBranchMem-2];
      double TempPStrain= getPlasticStrain(Tea-Teb,Tfa-Tfb);
            TFatDamage+=damage(TempPStrain);
      TeCumPlastic += TempPStrain;
                  TBranchNum-=4;
            SetPastCurve(TBranchNum);
            if (TBranchNum==7)
                  Rule7(res);
            else
                  Rule11(res);
          } else {
            trial.Stress()= MP_f(trial.getStrain());
            trial.Tangent()= MP_E(trial.getStrain());
                  TBranchMem= (TBranchNum+1)/2;
            TFatDamage-=damage(T_ePlastic[TBranchMem]);
      TeCumPlastic -= T_ePlastic[TBranchMem];
            T_ePlastic[TBranchMem]=getPlasticStrain(Tea-trial.getStrain(),Tfa-trial.getStress());
            TFatDamage+=damage(T_ePlastic[TBranchMem]);
      TeCumPlastic += T_ePlastic[TBranchMem];
          }
  }
  return res;
}

// Rule 12: Loading Third Transition Branch
int
XC::ReinforcingSteel::Rule12(int res)
{
  if (trial.getStrain()-converged.getStrain() < 0.0) {
        // reset past curve
        double eb=Tea;
        if(TBranchNum+2>LastRule_RS) {
                TBranchMem= (TBranchNum+1)/2;
          eb=Cea[TBranchMem-2];
          SetPastCurve(TBranchNum-6);
        } else {
          SetPastCurve(TBranchNum-2);
        }

        double fb=MP_f(eb);
        double Eb=MP_E(eb);
        Tea=converged.getStrain();
        Tfa=converged.getStress();
        TEa=ReturnSlope(converged.getStrain()-eb);
        Teb=eb;
        Tfb=fb;
        TEb=Eb;
        SetTRn1();
        TEsec= (Tfb-Tfa)/(Teb-Tea);
        if (TEsec<TEb) TEb=TEsec*0.999;
        if (TEsec>TEa) TEa=TEsec*1.001;
        res += SetMP();

        if(TBranchNum+2>LastRule_RS)
          TBranchNum-=2;
        else
          TBranchNum+=2;

        TBranchMem= (TBranchNum+1)/2;
        T_ePlastic[TBranchMem]=0.0;
        Rule10(res);
  } else {
          if (trial.getStrain() - Teb >= -ZeroTol) {
                  TBranchMem= (TBranchNum+1)/2;
            TFatDamage-=damage(T_ePlastic[TBranchMem-2]);
      TeCumPlastic -= T_ePlastic[TBranchMem-2];
      double TempPStrain= getPlasticStrain(Teb-Tea,Tfb-Tfa);
            TFatDamage+=damage(TempPStrain);
      TeCumPlastic += TempPStrain;
                  TBranchNum-=4;
            SetPastCurve(TBranchNum);
            if (TBranchNum==8)
                  Rule8(res);
            else
                  Rule12(res);
          } else {
      trial.Stress()= MP_f(trial.getStrain());
            trial.Tangent()= MP_E(trial.getStrain());
                  TBranchMem= (TBranchNum+1)/2;
            TFatDamage-=damage(T_ePlastic[TBranchMem]);
      TeCumPlastic -= T_ePlastic[TBranchMem];
            T_ePlastic[TBranchMem]=getPlasticStrain(trial.getStrain()-Tea,trial.getStress()-Tfa);
            TFatDamage+=damage(T_ePlastic[TBranchMem]);
      TeCumPlastic += T_ePlastic[TBranchMem];
          }
  }
  return res;
}



/*****************************************************************************************/
/********        Strength and stiffness degradation including buckling         ***********/
/*****************************************************************************************/
double XC::ReinforcingSteel::getPlasticStrain(double ehalf, double stressAmp)
  {
    double ehalfPlastic= fabs(ehalf)-fabs(stressAmp/Esp);
    if(ehalfPlastic>0.0)
      return ehalfPlastic;
    else
      return 0.0;
  }
double XC::ReinforcingSteel::damage(double ehalfPlastic)
  { return pow(ehalfPlastic/Fat1,Fat2); }

double XC::ReinforcingSteel::scalefactor(void) const
  {
    if(theBarFailed) return 0.0;
    double sf=1.0-Deg1*TFatDamage;
    if(TFatDamage>1.0) sf-= (TFatDamage-1.0)/0.04;
    if(sf<0.0)
      {
        theBarFailed=1;
        TBranchNum= -1;
        std::cerr << "-------------------------Bar failed---------------------------\n";
        return 0.0;
      }
    else
      { return sf; }
  }

double XC::ReinforcingSteel::ReturnSlope(double dea)
  {
    if(TeAbsMax > -TeAbsMin) //Dodd and Cooke
      return Esp*(0.82+1.0/(5.55+1000.0*TeAbsMax));
    else
      return Esp*(0.82+1.0/(5.55-1000.0*TeAbsMin));
    //return Esp;
  }
