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
                                                                        
// $Revision: 1.6 $
// $Date: 2003/02/14 23:01:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ElasticPPMaterial.cpp,v $
                                                                        
                                                                        
// File: ~/material/ElasticPPMaterial.C
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class implementation for 
// ElasticMaterial. 
//
// What: "@(#) ElasticPPMaterial.C, revA"


#include <material/uniaxial/ElasticPPMaterial.h>
#include <utility/matrix/Vector.h>

#include <cmath>
#include <cfloat>


//! @brief Asigna el valor de la deformación de cedencia a tracción.
void XC::ElasticPPMaterial::set_fyp(const double &f)
  {
    fyp= f;
    if(fyp < 0)
      {
        std::cerr << "XC::ElasticPPMaterial::ElasticPPMaterial() - fyp < 0, setting > 0\n";
        fyp*= -1.;
      }
  }

//! @brief Asigna el valor de la deformación de cedencia a tracción.
void XC::ElasticPPMaterial::set_eyp(const double &eyp)
  { set_fyp(E*eyp); }

//! @brief Asigna el valor de la deformación de cedencia a compresión.
void XC::ElasticPPMaterial::set_fyn(const double &f)
  {
    fyn= f;
    if(fyn > 0)
      {
        std::cerr << "XC::ElasticPPMaterial::ElasticPPMaterial() - fyn > 0, setting < 0\n";
        fyn*= -1.;
      }  
  }

//! @brief Asigna el valor de la deformación de cedencia a compresión.
void XC::ElasticPPMaterial::set_eyn(const double &eyn)
  { set_fyn(E*eyn); }

//! @brief Constructor.
XC::ElasticPPMaterial::ElasticPPMaterial(int tag, double e, double eyp)
  :EPPBaseMaterial(tag,MAT_TAG_ElasticPPMaterial,e,0.0) 
  {
    set_eyp(eyp);
    fyn = -fyp;
  }

//! @brief Constructor.
XC::ElasticPPMaterial::ElasticPPMaterial(int tag, double e, double eyp,double eyn, double ez )
  :EPPBaseMaterial(tag,MAT_TAG_ElasticPPMaterial,e,ez), fyp(0.0), fyn(0.0)
  {
    set_eyp(eyp);
    set_eyn(eyn);
  }

//! @brief Constructor.
XC::ElasticPPMaterial::ElasticPPMaterial(void)
  :EPPBaseMaterial(0,MAT_TAG_ElasticPPMaterial,0.0,0.0), fyp(0.0), fyn(0.0) {}

//! @brief Constructor.
XC::ElasticPPMaterial::ElasticPPMaterial(int tag)
  :EPPBaseMaterial(tag,MAT_TAG_ElasticPPMaterial,0.0,0.0), fyp(0.0), fyn(0.0) {}

//! Asigna el valor de la tensión de prueba.
int XC::ElasticPPMaterial::setTrialStrain(double strain, double strainRate)
  {
  /*
    if (fabs(trialStrain - strain) < DBL_EPSILON)
      return 0;
  */
    trialStrain = strain;

    // compute trial stress
    const double sigtrial= E * def_total(); // trial stress

    const double f= yield_function(sigtrial); //yield function

    const double fYieldSurface= - E * DBL_EPSILON;
    if(f<=fYieldSurface )
      {
        // elastic
        trialStress = sigtrial;
        trialTangent = E;
      }
    else
      {
        // plastic
        if(sigtrial>0.0)
          { trialStress = fyp; }
        else
          { trialStress = fyn; }
        trialTangent = 0.0;
      }
    return 0;
  }

int XC::ElasticPPMaterial::commitState(void)
  {

    // compute trial stress
    const double sigtrial= E * def_total(); // trial stress

    const double f= yield_function(sigtrial); //yield function

    const double fYieldSurface= - E * DBL_EPSILON;
    if(f>fYieldSurface )
      {
        // plastic
        if(sigtrial>0.0)
          { commitStrain+= f / E; }
        else
          { commitStrain-= f / E; }
      }
    return 0;
  }        

//! @brief Devuelve el estado del material al del último commit.
int XC::ElasticPPMaterial::revertToLastCommit(void)
  { return 0; }


//! @brief Devuelve el material a su estado inicial.
int XC::ElasticPPMaterial::revertToStart(void)
  {
    commitStrain= 0.0;
    return 0;
  }


XC::UniaxialMaterial *XC::ElasticPPMaterial::getCopy(void) const
  { return new ElasticPPMaterial(*this); }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::ElasticPPMaterial::sendData(CommParameters &cp)
  {
    int res= EPPBaseMaterial::sendData(cp);
    res+= cp.sendDoubles(fyp, fyn,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::ElasticPPMaterial::recvData(const CommParameters &cp)
  {
    int res= EPPBaseMaterial::recvData(cp);
    res+= cp.receiveDoubles(fyp, fyn,getDbTagData(),CommMetaData(4));
    return res;
  }

int XC::ElasticPPMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(5);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "WARNING ElasticPPMaterial::sendSelf() - " 
                << dataTag << " failed to send.";
    return res;
  }

int XC::ElasticPPMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(5);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << "WARNING ElasticPPMaterial::recvSelf() - "
                << dataTag << " failed to receive ID\n";
    else
      res+= recvData(cp);
    return res;
  }

void XC::ElasticPPMaterial::Print(std::ostream &s, int flag)
  {
    s << "ElasticPP tag: " << this->getTag() << std::endl;
    s << "  E: " << E << std::endl;
    s << "  ep: " << commitStrain << std::endl;
    s << "  Otress: " << trialStress << " tangent: " << trialTangent << std::endl;
  }

double XC::ElasticPPMaterial::get_fyp(void) const
  { return fyp; }

double XC::ElasticPPMaterial::get_eyp(void) const
  { return fyp/E; }

double XC::ElasticPPMaterial::get_fyn(void) const
  { return fyn; }

double XC::ElasticPPMaterial::get_eyn(void) const
  { return fyn/E; }
