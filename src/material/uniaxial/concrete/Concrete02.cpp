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
#include <utility/matrix/Vector.h>

XC::Concrete02::Concrete02(int tag, double _fc, double _epsc0, double _fcu,
                       double _epscu, double _rat, double _ft, double _Ets):
  UniaxialMaterial(tag, MAT_TAG_Concrete02),
  fc(_fc), epsc0(_epsc0), fcu(_fcu), epscu(_epscu), rat(_rat), ft(_ft), Ets(_Ets)
{
  ecminP = 0.0;
  deptP = 0.0;

  eP = 2.0*fc/epsc0;
  epsP = 0.0;
  sigP = 0.0;
  eps = 0.0;
  sig = 0.0;
  e = 2.0*fc/epsc0;
}

XC::Concrete02::Concrete02(void):
  UniaxialMaterial(0, MAT_TAG_Concrete02)
  {}

XC::UniaxialMaterial* XC::Concrete02::getCopy(void) const
  { return new Concrete02(*this); }

//! @brief Assigns concrete compressive strenght.
void XC::Concrete02::setFpcu(const double &d)
  {
    fcu= d;
    if(fcu > 0.0)
      {
        fcu= -fcu;
        std::clog << "Warning!, compressive strength must be negative." << std::endl;
      }
  }

//! @brief Returns concrete compressive strenght.
double XC::Concrete02::getFpcu(void) const
  { return fcu; }

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
  double         ec0 = fc * 2. / epsc0;

  // retrieve concrete hitory variables

  ecmin = ecminP;
  dept = deptP;

  // calculate current strain

  eps = trialStrain;
  double deps = eps - epsP;

  // if the current strain is less than the smallest previous strain 
  // call the monotonic envelope in compression and reset minimum strain 

  if (eps < ecmin) {
    this->Compr_Envlp(eps, sig, e);
    ecmin = eps;
  } else {;

    // else, if the current strain is between the minimum strain and ept 
    // (which corresponds to zero stress) the material is in the unloading- 
    // reloading branch and the stress remains between sigmin and sigmax 
    
    // calculate strain-stress coordinates of point R that determines 
    // the reloading slope according to Fig.2.11 in EERC Report 
    // (corresponding equations are 2.31 and 2.32 
    // the strain of point R is epsR and the stress is sigmR 
    
    double epsr = (fcu - rat * ec0 * epscu) / (ec0 * (1.0 - rat));
    double sigmr = ec0 * epsr;
    
    // calculate the previous minimum stress sigmm from the minimum 
    // previous strain ecmin and the monotonic envelope in compression 
    
    double sigmm;
    double dumy;
    this->Compr_Envlp(ecmin, sigmm, dumy);
    
    // calculate current reloading slope Er (Eq. 2.35 in EERC Report) 
    // calculate the intersection of the current reloading slope Er 
    // with the zero stress axis (variable ept) (Eq. 2.36 in EERC Report) 
    
    double er = (sigmm - sigmr) / (ecmin - epsr);
    double ept = ecmin - sigmm / er;
    
    if (eps <= ept) {
      double sigmin = sigmm + er * (eps - ecmin);
      double sigmax = er * .5f * (eps - ept);
      sig = sigP + ec0 * deps;
      e = ec0;
      if (sig <= sigmin) {
        sig = sigmin;
        e = er;
      }
      if (sig >= sigmax) {
        sig = sigmax;
        e = 0.5 * er;
      }
    } else {
      
      // else, if the current strain is between ept and epn 
      // (which corresponds to maximum remaining tensile strength) 
      // the response corresponds to the reloading branch in tension 
      // Since it is not saved, calculate the maximum remaining tensile 
      // strength sicn (Eq. 2.43 in EERC Report) 
      
      // calculate first the strain at the peak of the tensile stress-strain 
      // relation epn (Eq. 2.42 in EERC Report) 
      
      double epn = ept + dept;
      double sicn;
      if (eps <= epn) {
        this->Tens_Envlp(dept, sicn, e);
        if (dept != 0.0) {
          e = sicn / dept;
        } else {
          e = ec0;
        }
        sig = e * (eps - ept);
      } else {
        
        // else, if the current strain is larger than epn the response 
        // corresponds to the tensile envelope curve shifted by ept 
        
        double epstmp = eps - ept;
        this->Tens_Envlp(epstmp, sig, e);
        dept = eps - ept;
      }
    }
  }

  return 0;
}



double XC::Concrete02::getStrain(void) const
  { return eps; }

double XC::Concrete02::getStress(void) const
  { return sig; }

double XC::Concrete02::getTangent(void) const
  { return e; }

int XC::Concrete02::commitState(void)
{
  ecminP = ecmin;
  deptP = dept;
  
  eP = e;
  sigP = sig;
  epsP = eps;
  return 0;
}

int 
XC::Concrete02::revertToLastCommit(void)
{
  ecmin = ecminP;
  dept = deptP;
  
  e = eP;
  sig = sigP;
  eps = epsP;
  return 0;
}

int XC::Concrete02::revertToStart(void)
  {
  ecminP = 0.0;
  deptP = 0.0;

  eP = 2.0*fc/epsc0;
  epsP = 0.0;
  sigP = 0.0;
  eps = 0.0;
  sig = 0.0;
  e = 2.0*fc/epsc0;

  return 0;
}

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::Concrete02::sendData(CommParameters &cp)
  {
    int res= UniaxialMaterial::sendData(cp);
    res+= cp.sendDoubles(fc,epsc0,fcu,epscu,getDbTagData(),CommMetaData(2));
    res+= cp.sendDoubles(rat,ft,Ets,ecminP,deptP,getDbTagData(),CommMetaData(3));
    res+= cp.sendDoubles(epsP,sigP,eP,ecmin,dept,sig,getDbTagData(),CommMetaData(4));
    res+= cp.sendDoubles(e,eps,getDbTagData(),CommMetaData(5));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::Concrete02::recvData(const CommParameters &cp)
  {
    int res= UniaxialMaterial::recvData(cp);
    res+= cp.receiveDoubles(fc,epsc0,fcu,epscu,getDbTagData(),CommMetaData(2));
    res+= cp.receiveDoubles(rat,ft,Ets,ecminP,deptP,getDbTagData(),CommMetaData(3));
    res+= cp.receiveDoubles(epsP,sigP,eP,ecmin,dept,sig,getDbTagData(),CommMetaData(4));
    res+= cp.receiveDoubles(e,eps,getDbTagData(),CommMetaData(5));
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
  s << "Concrete02:(strain, stress, tangent) " << eps << " " << sig << " " << e << std::endl;
}


void
XC::Concrete02::Tens_Envlp (double epsc, double &sigc, double &Ect)
{
/*-----------------------------------------------------------------------
! monotonic envelope of concrete in tension (positive envelope)
!
!   ft    = concrete tensile strength
!   Ec0   = initial tangent modulus of concrete 
!   Ets   = tension softening modulus
!   eps   = strain
!
!   returned variables
!    sigc  = stress corresponding to eps
!    Ect  = tangent concrete modulus
!-----------------------------------------------------------------------*/
  
  double Ec0  = 2.0*fc/epsc0;

  double eps0 = ft/Ec0;
  double epsu = ft*(1.0/Ets+1.0/Ec0);
  if (epsc<=eps0) {
    sigc = epsc*Ec0;
    Ect  = Ec0;
  } else {
    if (epsc<=epsu) {
      Ect  = -Ets;
      sigc = ft-Ets*(epsc-eps0);
    } else {
      //      Ect  = 0.0
      Ect  = 1.0e-10;
      sigc = 0.0;
    }
  }
  return;
}

  
void
XC::Concrete02::Compr_Envlp (double epsc, double &sigc, double &Ect) 
{
/*-----------------------------------------------------------------------
! monotonic envelope of concrete in compression (negative envelope)
!
!   fc    = concrete compressive strength
!   epsc0 = strain at concrete compressive strength
!   fcu   = stress at ultimate (crushing) strain 
!   epscu = ultimate (crushing) strain
!   Ec0   = initial concrete tangent modulus
!   epsc  = strain
!
!   returned variables
!   sigc  = current stress
!   Ect   = tangent concrete modulus
-----------------------------------------------------------------------*/

  double Ec0  = 2.0*fc/epsc0;

  double ratLocal = epsc/epsc0;
  if (epsc>=epsc0) {
    sigc = fc*ratLocal*(2.0-ratLocal);
    Ect  = Ec0*(1.0-ratLocal);
  } else {
    
    //   linear descending branch between epsc0 and epscu
    if (epsc>epscu) {
      sigc = (fcu-fc)*(epsc-epsc0)/(epscu-epsc0)+fc;
      Ect  = (fcu-fc)/(epscu-epsc0);
    } else {
           
      // flat friction branch for strains larger than epscu
      
      sigc = fcu;
      Ect  = 1.0e-10;
      //       Ect  = 0.0
    }
  }
  return;
}
