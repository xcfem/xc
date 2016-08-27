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
                                                                        
// $Revision: 1.1 $
// $Date: 2006/03/03 18:52:40 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/Concrete02.cpp,v $
                                                                      
// Written: fmk
// Created: 03/06
//
// Description: This file contains the class implementation of Concrete02. 
// This Concrete02 is based on an f2c of the FEDEAS material
// Concr2.f which is:
//-----------------------------------------------------------------------
// concrete model with damage modulus    
//       by MOHD YASSIN (1993)
// adapted to FEDEAS material library
// by D. Sze and Filip C. Filippou in 1994
//-----------------------------------------------------------------------


#include <material/uniaxial/concrete/Concrete02.h>
#include <cfloat>

void XC::Concrete02::setup_parameters(void)
  {
    hstvP.ecmin= 0.0;
    hstvP.dept= 0.0;

    const double initTang= getInitialTangent();
    hstvP.setup_parameters(initTang);
    hstv.setup_parameters(initTang);
  }

XC::Concrete02::Concrete02(int tag, double _fpc, double _epsc0, double _fpcu,
                       double _epscu, double _rat, double _ft, double _Ets)
  :  RawConcrete(tag, MAT_TAG_Concrete02,_fpc,_epsc0,_epscu),
     fpcu(_fpcu), rat(_rat), ft(_ft), Ets(_Ets)
  {
    setup_parameters();
  }

XC::Concrete02::Concrete02(int tag):
  RawConcrete(tag, MAT_TAG_Concrete02),
  fpcu(0.0), rat(0.0), ft(0.0), Ets(0.0)
  {
    setup_parameters();
  }

XC::UniaxialMaterial* XC::Concrete02::getCopy(void) const
  { return new Concrete02(*this); }

//! @brief Assigns concrete compressive strenght.
void XC::Concrete02::setFpcu(const double &d)
  {
    fpcu= d;
    if(fpcu > 0.0)
      {
        fpcu= -fpcu;
        std::clog << "Warning!, compressive strength must be negative." << std::endl;
      }
  }

//! @brief Returns concrete compressive strenght.
double XC::Concrete02::getFpcu(void) const
  { return fpcu; }

//! @brief Assigns concrete tensile strenght.
void XC::Concrete02::setFt(const double &d)
  {
    ft= d;
    if(ft < 0.0)
      {
        ft= -ft;
        std::clog << "Warning!, tensile strength must be positive." << std::endl;
      }
  }

//! @brief Returns concrete tensile strenght.
double XC::Concrete02::getFt(void) const
  { return ft; }


//! @brief tension softening stiffness (absolute value) (slope of the linear tension softening branch).
void XC::Concrete02::setEts(const double &d)
  {
    Ets= d;
    if(Ets < 0.0)
      {
        Ets= -Ets;
        std::clog << "Warning!, tensile softening stiffness must be positive (absolute value)." << std::endl;
      }
  }

//! @brief Returns concrete tensile strenght.
double XC::Concrete02::getEts(void) const
  { return Ets; }

//! @brief ratio between unloading slope at $epscu and initial slope
void XC::Concrete02::setLambda(const double &d)
  { rat= d; }

//! @brief Returns concrete tensile strenght.
double XC::Concrete02::getLambda(void) const
  { return rat; }


int XC::Concrete02::setTrialStrain(double trialStrain, double strainRate)
  {
    std::cout << "setTrialStrain begins; trialStrain= " << trialStrain << std::endl;
    const double ec0= getInitialTangent();

    // retrieve commited history variables
    hstv.ecmin= hstvP.ecmin;
    hstv.dept= hstvP.dept;

    // calculate current strain
    hstv.eps= trialStrain;
    const double deps= hstv.eps - hstvP.eps;

    // if the current strain is less than the smallest previous strain 
    // call the monotonic envelope in compression and reset minimum strain 
    if(hstv.eps < hstv.ecmin)
      {
        this->Compr_Envlp(hstv.eps, hstv.sig, hstv.e);
        hstv.ecmin= hstv.eps;
      }
    else
      {
        // else, if the current strain is between the minimum strain and ept 
        // (which corresponds to zero stress) the material is in the unloading- 
        // reloading branch and the stress remains between sigmin and sigmax 
    
        // calculate strain-stress coordinates of point R that determines 
        // the reloading slope according to Fig.2.11 in EERC Report 
        // (corresponding equations are 2.31 and 2.32 
        // the strain of point R is epsR and the stress is sigmR 
    
        const double epsr= (fpcu - rat * ec0 * epscu) / (ec0 * (1.0 - rat));
        const double sigmr= ec0 * epsr;
    
        // calculate the previous minimum stress sigmm from the minimum 
        // previous strain hstv.ecmin and the monotonic envelope in compression 
    
        double sigmm;
        double dumy;
        this->Compr_Envlp(hstv.ecmin, sigmm, dumy);
    
        // calculate current reloading slope Er (Eq. 2.35 in EERC Report) 
        // calculate the intersection of the current reloading slope Er 
        // with the zero stress axis (variable ept) (Eq. 2.36 in EERC Report) 
    
        const double er= (sigmm - sigmr) / (hstv.ecmin - epsr);
        const double ept= hstv.ecmin - sigmm / er;
    
        if(hstv.eps <= ept)
          {
            const double sigmin= sigmm + er * (hstv.eps - hstv.ecmin);
            const double sigmax= er * .5f * (hstv.eps - ept);
            hstv.sig= hstvP.sig + ec0 * deps;
            hstv.e= ec0;
	    hstv.cutStress(sigmin,sigmax,er);
          }
        else
          {
            // else, if the current strain is between ept and epn 
            // (which corresponds to maximum remaining tensile strength) 
            // the response corresponds to the reloading branch in tension 
            // Since it is not saved, calculate the maximum remaining tensile 
            // strength sicn (Eq. 2.43 in EERC Report) 
      
            // calculate first the strain at the peak of the tensile stress-strain 
            // relation epn (Eq. 2.42 in EERC Report) 
      
            const double epn= ept + hstv.dept;
            double sicn;
            if(hstv.eps <= epn)
              {
                this->Tens_Envlp(hstv.dept, sicn, hstv.e);
                if(hstv.dept != 0.0)
                  { hstv.e= sicn / hstv.dept; }
                else
                  { hstv.e= ec0; }
                hstv.sig= hstv.e * (hstv.eps - ept);
              }
            else
              {
                // else, if the current strain is larger than epn the response 
                // corresponds to the tensile envelope curve shifted by ept 
                const double epstmp= hstv.eps - ept;
                this->Tens_Envlp(epstmp, hstv.sig, hstv.e);
                hstv.dept= hstv.eps - ept;
              }
          }
      }
    std::cout << "setTrialStrain ends; trialStrain= " << hstv << std::endl;
    return 0;
  }



int XC::Concrete02::commitState(void)
  {
    hstvP= hstv;
    return 0;
  }

int XC::Concrete02::revertToLastCommit(void)
  {
    hstv= hstvP;
    return 0;
  }

int XC::Concrete02::revertToStart(void)
  {
    setup_parameters();
    return 0;
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::Concrete02::sendData(CommParameters &cp)
  {
    int res= RawConcrete::sendData(cp);
    res+= cp.sendDoubles(fpc,epsc0,fpcu,epscu,getDbTagData(),CommMetaData(2));
    res+= cp.sendDoubles(rat,ft,Ets,hstvP.ecmin,hstvP.dept,getDbTagData(),CommMetaData(3));
    res+= cp.sendDoubles(hstvP.eps,hstvP.sig,hstvP.e,hstv.ecmin,hstv.dept,hstv.sig,getDbTagData(),CommMetaData(4));
    res+= cp.sendDoubles(hstv.e,hstv.eps,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::Concrete02::recvData(const CommParameters &cp)
  {
    int res= RawConcrete::recvData(cp);
    res+= cp.receiveDoubles(fpc,epsc0,fpcu,epscu,getDbTagData(),CommMetaData(2));
    res+= cp.receiveDoubles(rat,ft,Ets,hstvP.ecmin,hstvP.dept,getDbTagData(),CommMetaData(3));
    res+= cp.receiveDoubles(hstvP.eps,hstvP.sig,hstvP.e,hstv.ecmin,hstv.dept,hstv.sig,getDbTagData(),CommMetaData(4));
    res+= cp.receiveDoubles(hstv.e,hstv.eps,getDbTagData(),CommMetaData(5));
    return res;
  }

int XC::Concrete02::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(6);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

int XC::Concrete02::recvSelf(const CommParameters &cp)
  {
    inicComm(6);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << nombre_clase() << "::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
          std::cerr << nombre_clase() << "::recvSelf - failed to receive data.\n";
      }
    return res;
  }

void XC::Concrete02::Print(std::ostream &s, int flag)
  {
    hstv.Print(s);
  }


void XC::Concrete02::Tens_Envlp(double epsc, double &sigc, double &Ect)
  {
    /*-----------------------------------------------------------------------
    ! monotonic envelope of concrete in tension (positive envelope)
    !
    !   ft = concrete tensile strength
    !   Ec0= initial tangent modulus of concrete 
    !   Ets= tension softening modulus
    !   hstv.eps= strain
    !
    !   returned variables
    !    sigc= stress corresponding to hstv.eps
    !    Ect= tangent concrete modulus
    !-----------------------------------------------------------------------*/
  
    const double Ec0= getInitialTangent();

    const double eps0= ft/Ec0;
    const double epsu= ft*(1.0/Ets+1.0/Ec0);
    if(epsc<=eps0)
      {
        sigc= epsc*Ec0;
        Ect= Ec0;
      }
    else
      {
        if(epsc<=epsu)
	  {
            Ect= -Ets;
            sigc= ft-Ets*(epsc-eps0);
          }
	else
	  {
            // Ect= 0.0
            Ect= 1.0e-10;
            sigc= 0.0;
          }
      }
    return;
  }

  
void XC::Concrete02::Compr_Envlp(double epsc, double &sigc, double &Ect) 
  {
    /*-----------------------------------------------------------------------
    ! monotonic envelope of concrete in compression (negative envelope)
    !
    !   fpc= concrete compressive strength
    !   epsc0= strain at concrete compressive strength
    !   fpcu= stress at ultimate (crushing) strain 
    !   epscu= ultimate (crushing) strain
    !   Ec0= initial concrete tangent modulus
    !   epsc= strain
    !
    !   returned variables
    !   sigc= current stress
    !   Ect= tangent concrete modulus
    -----------------------------------------------------------------------*/

    const double Ec0= getInitialTangent();

    const double ratLocal= epsc/epsc0;
    if(epsc>=epsc0)
      {
        sigc= fpc*ratLocal*(2.0-ratLocal);
        Ect= Ec0*(1.0-ratLocal);
      }
    else
      {
        //   linear descending branch between epsc0 and epscu
        if(epsc>epscu)
	  {
            sigc= (fpcu-fpc)*(epsc-epsc0)/(epscu-epsc0)+fpc;
            Ect= (fpcu-fpc)/(epscu-epsc0);
          }
	else
	  {
            // flat friction branch for strains larger than epscu
            sigc= fpcu;
            Ect= 1.0e-10;
            //       Ect= 0.0
          }
      }
    return;
  }
