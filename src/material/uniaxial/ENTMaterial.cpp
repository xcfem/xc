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
** file 'COPYRIGHT'  in XC::main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.3 $
// $Date: 2003/02/25 23:33:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ENTMaterial.cpp,v $
                                                                        
                                                                        
// File: ~/material/ENTMaterial.C
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class implementation for 
// ENTMaterial. 
//
// What: "@(#) ENTMaterial.C, revA"

#include <material/uniaxial/ENTMaterial.h>
#include "domain/component/Parameter.h"
#include <utility/matrix/Vector.h>
#include <domain/mesh/element/Information.h>
#include "xc_utils/src/base/CmdStatus.h"
#include "xc_utils/src/base/any_const_ptr.h"

XC::ENTMaterial::ENTMaterial(int tag, const double &e,const double &A,const double &B)
  :ElasticBaseMaterial(tag,MAT_TAG_ENTMaterial,e), a(A), b(B), parameterID(0) {}

//! @brief Lee un objeto XC::ENTMaterial desde archivo
bool XC::ENTMaterial::procesa_comando(CmdStatus &status)
  {
    const std::string cmd= deref_cmd(status.Cmd());
    if(verborrea>2)
      std::clog << "(ENTMaterial) Procesando comando: " << cmd << std::endl;
    if(cmd == "a")
      {
        a= interpretaDouble(status.GetString());
        return true;
      }
    else if(cmd == "b")
      {
        b= interpretaDouble(status.GetString());
        return true;
      }
    else
      return ElasticBaseMaterial::procesa_comando(status);
  }

int XC::ENTMaterial::setTrialStrain(double strain, double strainRate)
  {
    trialStrain = strain;
    return 0;
  }

//! @brief Devuelve la tensión en el material.
double XC::ENTMaterial::getStress(void) const
  {
    if(trialStrain<0.0)
      return E*trialStrain;
    else if (a == 0.0)
      return 0.0;
    else
      return a*E*tanh(trialStrain*b);
  }

//! @brief Devuelve el módulo elástico tangente.
double XC::ENTMaterial::getTangent(void) const
  {
    if(trialStrain<=0.0)
      return E;
    else if(a == 0.0)
      return 0.0;
    else
      {
        const double tanhB = tanh(trialStrain*b);
        return a*E*(1.0-tanhB*tanhB);
      }
  }

int XC::ENTMaterial::commitState(void)
  { return 0; }

int XC::ENTMaterial::revertToLastCommit(void)
  { return 0; }

int XC::ENTMaterial::revertToStart(void)
  { return 0; }

XC::UniaxialMaterial *XC::ENTMaterial::getCopy(void) const
  { return new ENTMaterial(*this); }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ENTMaterial::sendData(CommParameters &cp)
  {
    int res= ElasticBaseMaterial::sendData(cp);
    res+= cp.sendDoubles(a,b,getDbTagData(),CommMetaData(3));
    res+= cp.sendInt(parameterID,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ENTMaterial::recvData(const CommParameters &cp)
  {
    int res= ElasticBaseMaterial::recvData(cp);
    res+= cp.receiveDoubles(a,b,getDbTagData(),CommMetaData(3));
    res+= cp.receiveInt(parameterID,getDbTagData(),CommMetaData(4));
    return res;
  }

//! @brief Envía el objeto a través del canal que se pasa como parámetro.
int XC::ENTMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(5); 
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ENTMaterial::sendSelf - failed to send data.\n";
    return res;
  }

//! @brief Recibe el objeto a través del canal que se pasa como parámetro.
int XC::ENTMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(5);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ENTMaterial::recvSelf - failed to receive ids.\n";
    else
      {
        //setTag(getDbTagDataPos(0));
        res+= recvData(cp);
        if(res<0)
           std::cerr << "ENTMaterial::recvSelf - failed to receive data.\n";
      }
    return res;
  }

//! \brief Devuelve la propiedad del objeto cuyo código (de la propiedad) se pasa
//! como parámetro.
//!
//! Soporta los códigos:
//! nnod: Devuelve el número de nodos del dominio.
any_const_ptr XC::ENTMaterial::GetProp(const std::string &cod) const
  {
    if(cod=="getA")
      return any_const_ptr(a);
    if(cod=="getB")
      return any_const_ptr(b);
    else
      return ElasticBaseMaterial::GetProp(cod);
  }

void XC::ENTMaterial::Print(std::ostream &s, int flag)
  {
    s << "ENTMaterial, tag: " << this->getTag() << std::endl;
    s << "  E: " << E << std::endl;
  }

int XC::ENTMaterial::setParameter(const std::vector<std::string > &argv, Parameter &param)
  {
    if(argv[0] == "E")
      return param.addObject(1, this);
    else
      return -1;
  }

int XC::ENTMaterial::updateParameter(int parameterID, Information &info)
  {
    switch(parameterID)
      {
	case -1:
	  return -1;
	case 1:
	  E = info.theDouble;
	  return 0;
	default:
	  return -1;
      }
  }

int XC::ENTMaterial::activateParameter(int paramID)
  {
    parameterID = paramID;
    return 0;
  }

double XC::ENTMaterial::getStressSensitivity(int gradIndex, bool conditional)
  {
    if(parameterID == 1 && trialStrain < 0.0)
      return trialStrain;
    else
      return 0.0;
  }

double XC::ENTMaterial::getInitialTangentSensitivity(int gradIndex)
  { return 0.0; }

int XC::ENTMaterial::commitSensitivity(double strainGradient,int gradIndex, int numGrads)
  {
    // Nothing to commit ... path independent
    return 0.0;
  }
