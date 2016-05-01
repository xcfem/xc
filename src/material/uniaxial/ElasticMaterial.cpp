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
                                                                        
// $Revision: 1.5 $
// $Date: 2003/02/25 23:33:38 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/ElasticMaterial.cpp,v $
                                                                        
                                                                        
// File: ~/material/ElasticMaterial.C
//
// Written: fmk 
// Created: 07/98
// Revision: A
//
// Description: This file contains the class implementation for 
// ElasticMaterial. 
//
// What: "@(#) ElasticMaterial.C, revA"

#include <material/uniaxial/ElasticMaterial.h>
#include "domain/component/Parameter.h"
#include <utility/matrix/Vector.h>

#include <domain/mesh/element/Information.h>


//! @brief Constructor.
XC::ElasticMaterial::ElasticMaterial(int tag, double e, double et)
  :ElasticBaseMaterial(tag,MAT_TAG_ElasticMaterial,e), trialStrainRate(0.0), eta(et)
  {}

//! @brief Constructor.
XC::ElasticMaterial::ElasticMaterial(int tag, int classtag)
  :ElasticBaseMaterial(tag,classtag,0.0),  trialStrainRate(0.0), eta(0.0)
  {}

//! @brief Constructor por defecto.
XC::ElasticMaterial::ElasticMaterial(void)
  :ElasticBaseMaterial(0,MAT_TAG_ElasticMaterial,0.0),  trialStrainRate(0.0), eta(0.0)
  {}

int XC::ElasticMaterial::setTrialStrain(double strain, double strainRate)
  {
    trialStrain     = strain;
    trialStrainRate = strainRate;
    return 0;
  }

int XC::ElasticMaterial::setTrial(double strain, double &stress, double &tangent, double strainRate)
  {
    trialStrain     = strain;
    trialStrainRate = strainRate;

    stress = getStress();
    tangent = E;

    return 0;
  }

double XC::ElasticMaterial::getStress(void) const
  { return E*def_total() + eta*trialStrainRate; }


int XC::ElasticMaterial::commitState(void)
  { return 0; }


int XC::ElasticMaterial::revertToLastCommit(void)
  { return 0; }


int XC::ElasticMaterial::revertToStart(void)
  {
    trialStrain      = 0.0;
    trialStrainRate  = 0.0;
    return 0;
  }

//! @brief Constructor virtual.
XC::UniaxialMaterial *XC::ElasticMaterial::getCopy(void) const
  { return new ElasticMaterial(*this); }

//! @brief Envía los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ElasticMaterial::sendData(CommParameters &cp)
  {
    int res= ElasticBaseMaterial::sendData(cp);
    res+= cp.sendDoubles(trialStrainRate, eta,getDbTagData(),CommMetaData(3));
    return res;
  }

//! @brief Recibe los miembros del objeto a través del canal que se pasa como parámetro.
int XC::ElasticMaterial::recvData(const CommParameters &cp)
  {
    int res= ElasticBaseMaterial::recvData(cp);
    res+= cp.receiveDoubles(trialStrainRate, eta,getDbTagData(),CommMetaData(3));
    return res;
  }

int XC::ElasticMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(4);
    int res= sendData(cp);
    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << "ElasticMaterial::sendSelf() - failed to send data\n";
    return res;
  }

int XC::ElasticMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(4);
    const int dataTag= getDbTag();
    int res= cp.receiveIdData(getDbTagData(),dataTag);
    if(res<0)
      std::cerr << "ElasticMaterial::recvSelf - failed to receive ids.\n";
    else
      {
        res= recvData(cp);
        if(res < 0)
          std::cerr << "ElasticMaterial::recvSelf() - failed to receive data\n";
      }
    return res;
  }

void XC::ElasticMaterial::Print(std::ostream &s, int flag)
  {
    s << "Elastic tag: " << this->getTag() << std::endl;
    s << "  E: " << E << " eta: " << eta << std::endl;
  }

int XC::ElasticMaterial::setParameter(const std::vector<std::string>  &argv, Parameter &param)
  {
    if(argv[0] == "E")
      return param.addObject(1, this);
    else if(argv[0] == "eta")
      return param.addObject(2, this);
    else
      return -1;
  }

int XC::ElasticMaterial::updateParameter(int parameterID, Information &info)
  {
        switch(parameterID) {
        case -1:
                return -1;
        case 1:
                E = info.theDouble;
                return 0;
        case 2:
                eta = info.theDouble;
                return 0;
        default:
                return -1;
        }
  }

