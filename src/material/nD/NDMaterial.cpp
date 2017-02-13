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
**                                                                    **
** Additions and changes by:                                          **
**   Boris Jeremic (@ucdavis.edu)                                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// $Revision: 1.17 $                                                              
// $Date: 2004/07/20 22:39:02 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/XC::NDMaterial.cpp,v $                                                                
                                                                        
// File: ~/material/XC::NDMaterial.C
//
// Written: MHS 
// Created: Feb 2000
// Revision: A
//
// Description: This file contains the class implementation for XC::NDMaterial.
//
// What: "@(#) XC::NDMaterial.C, revA"

#include <material/nD/NDMaterial.h>
#include <domain/mesh/element/utils/Information.h>
#include <utility/matrix/Matrix.h>
#include <utility/matrix/Vector.h>
#include <utility/matrix/nDarray/stresst.h>
#include <utility/matrix/nDarray/straint.h>
#include <utility/recorder/response/MaterialResponse.h>

XC::Matrix XC::NDMaterial::errMatrix(1,1);
XC::Vector XC::NDMaterial::errVector(1);
XC::Tensor XC::NDMaterial::errTensor(2, def_dim_2, 0.0 );
XC::stresstensor XC::NDMaterial::errstresstensor;
XC::straintensor XC::NDMaterial::errstraintensor;

//! @brief Constructor.
XC::NDMaterial::NDMaterial(int tag, int classTag)
  :Material(tag,classTag) {}

//! @brief Constructor.
XC::NDMaterial::NDMaterial(void)
  :Material(0, 0) {}


double XC::NDMaterial::getRho(void) const
  { return 0.0; }

// BJ added 19June2002
double XC::NDMaterial::getE(void) 
  { return 0.0; }

// BJ added 19June2002
double XC::NDMaterial::getnu(void)
  { return 0.0; }

// BJ added 19June2002
double XC::NDMaterial::getpsi(void)
  { return 0.0; }

// nuevos métodos para recorder requested by Zhouhui Yang .. MAY NOT STAY
const XC::Vector &XC::NDMaterial::getCommittedStress(void) 
  { return this->getStress(); }

const XC::Vector &XC::NDMaterial::getCommittedStrain(void) 
  { return this->getStrain(); }

// methods to set and retrieve state.
//! @brief Asigna el valor de la deformación de prueba.
int XC::NDMaterial::setTrialStrain(const Vector &v)
  {
    std::cerr << "XC::NDMaterial::setTrialStrain -- subclass responsibility\n";
    return -1;    
  }

//! @brief Asigna el valor de la deformación de prueba.
int XC::NDMaterial::setTrialStrain(const Vector &v, const Vector &r)
  {
    std::cerr << "XC::NDMaterial::setTrialStrain -- subclass responsibility\n";
    return -1;    
  }

//! @brief Asigna el valor del incremento de la deformación de prueba.
int XC::NDMaterial::setTrialStrainIncr(const Vector &v)
  {
    std::cerr << "XC::NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
    return -1;    
  }

//! @brief Asigna el valor del incremento de la deformación de prueba.
int XC::NDMaterial::setTrialStrainIncr(const Vector &v, const Vector &r)
  {
     std::cerr << "XC::NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
     return -1;    
  }

//! @brief Asigna el valor de la initial deformation.
void XC::NDMaterial::setInitialGeneralizedStrain(const Vector &)
  {
     std::cerr << "XC::NDMaterial::setInitialGeneralizedStrain -- subclass responsibility\n";
  }

//! @brief Return the tangent stiffness matrix.
const XC::Matrix &XC::NDMaterial::getTangent(void) const
  {
     std::cerr << "XC::NDMaterial::getTangent -- subclass responsibility\n";
     return errMatrix;    
  }

//! @brief Return the tensión.
const XC::Vector &XC::NDMaterial::getStress(void) const
  {
    std::cerr << "XC::NDMaterial::getStress -- subclass responsibility\n";
    return errVector;    
  }

//! @brief Return the deformación.
const XC::Vector &XC::NDMaterial::getStrain(void) const
  {
    std::cerr << "XC::NDMaterial::getStrain -- subclass responsibility\n";
    return errVector;    
  }

//! @brief Return the initial deformation.
const XC::Vector &XC::NDMaterial::getInitialGeneralizedStrain(void) const
  {
    std::cerr << "XC::NDMaterial::getStrain -- subclass responsibility\n";
    return errVector;    
  }

//! @brief Asigna el valor de la deformación de prueba.
int XC::NDMaterial::setTrialStrain(const Tensor &v)
  {
    std::cerr << "XC::NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
    return -1;    
  }

//! @brief Asigna el valor de la deformación de prueba.
int XC::NDMaterial::setTrialStrain(const Tensor &v, const XC::Tensor &r)    
  {
    std::cerr << "XC::NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
    return -1;    
  }

//! @brief Asigna el valor del incremento de la deformación de prueba.
int XC::NDMaterial::setTrialStrainIncr(const Tensor &v)
  {
    std::cerr << "XC::NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
    return -1;    
  }

//! @brief Asigna el valor del incremento de la deformación de prueba.
int XC::NDMaterial::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r)
  {
    std::cerr << "XC::NDMaterial::setTrialStrainIncr -- subclass responsibility\n";
    return -1;    
  }

//Zhao (zcheng@ucdavis.edu) 
// added Sept 22 2003 for Large Deformation, F is the Deformation Grandient
int XC::NDMaterial::setTrialF(const straintensor &f)
  {
    std::cerr << "XC::NDMaterial::setTrialF -- subclass responsibility\n";
    return -1;
  }

int XC::NDMaterial::setTrialFIncr(const XC::straintensor &df)
  {
    std::cerr << "XC::NDMaterial::setTrialF -- subclass responsibility\n";
    return -1;
  }

int XC::NDMaterial::setTrialC(const XC::straintensor &c)
  {
     std::cerr << "XC::NDMaterial::setTrialC -- subclass responsibility\n";
     return -1;
  }

int XC::NDMaterial::setTrialCIncr(const XC::straintensor &c)
  {
     std::cerr << "XC::NDMaterial::setTrialC -- subclass responsibility\n";
     return -1;
  }

const XC::stresstensor XC::NDMaterial::getPK1StressTensor(void)
  {
    std::cerr << "XC::NDMaterial::getPK1StressTensor -- subclass responsibility\n";
    return errstresstensor;    
  }

const XC::stresstensor XC::NDMaterial::getCauchyStressTensor(void)
  {
     std::cerr << "XC::NDMaterial::getCauchyStressTensor -- subclass responsibility\n";
     return errstresstensor;    
  }

const XC::straintensor &XC::NDMaterial::getF(void) const
  {
    std::cerr << "XC::NDMaterial::getF -- subclass responsibility\n";
    return errstraintensor;    
  }

const XC::straintensor &XC::NDMaterial::getC(void) const
  {
    std::cerr << "XC::NDMaterial::getF -- subclass responsibility\n";
    return errstraintensor;    
  }

const XC::straintensor XC::NDMaterial::getFp(void)
  {
    std::cerr << "XC::NDMaterial::getFp -- subclass responsibility\n";
    return errstraintensor;    
  }
// Only For Large Deformation, END////////////////////////////

const XC::Tensor &XC::NDMaterial::getTangentTensor(void) const
  {
    std::cerr << "XC::NDMaterial::getTangentTensor -- subclass responsibility\n";
    return errTensor;    
  }

const XC::stresstensor &XC::NDMaterial::getStressTensor(void) const
  {
    std::cerr << "XC::NDMaterial::getStressTensor -- subclass responsibility\n";
    return errstresstensor;    
  }

const XC::straintensor &XC::NDMaterial::getStrainTensor(void) const
{
   std::cerr << "XC::NDMaterial::getStrainTensor -- subclass responsibility\n";
   return errstraintensor;    
}

const XC::straintensor &XC::NDMaterial::getPlasticStrainTensor(void) const
{
   std::cerr << "XC::NDMaterial::getPlasticStrainTensor -- subclass responsibility\n";
   return errstraintensor;    
}


//const XC::Tensor &XC::NDMaterial::getStrainTensor(void)
//{
//   std::cerr << "XC::NDMaterial::getStrainTensor -- subclass responsibility\n";
//   return errTensor;    
//}

XC::Response* XC::NDMaterial::setResponse(const std::vector<std::string> &argv, Information &matInfo)
  {
    if(argv[0] == "stress" || argv[0] == "stresses")
      return new XC::MaterialResponse(this, 1, this->getStress());
    else if(argv[0] == "strain" || argv[0] == "strains")
      return new XC::MaterialResponse(this, 2, this->getStrain());
    else if(argv[0] == "tangent")
      return new XC::MaterialResponse(this, 3, this->getTangent());
    else
      return 0;
  }

int XC::NDMaterial::getResponse(int responseID, Information &matInfo)
  {
    switch(responseID)
      {
      case 1:
        return matInfo.setVector(this->getStress());
      case 2:
        return matInfo.setVector(this->getStrain());
      case 3:
        return matInfo.setMatrix(this->getTangent());
      default:
        return -1;
      }
  }

//! @brief Send members del objeto through the channel being passed as parameter.
int XC::NDMaterial::sendData(CommParameters &cp)
  {
    setDbTagDataPos(0,getTag());
    return 0;
  }

//! @brief Receives members del objeto through the channel being passed as parameter.
int XC::NDMaterial::recvData(const CommParameters &cp)
  {
    setTag(getDbTagDataPos(0));
    return 0;
  }

// AddingSensitivity:BEGIN ////////////////////////////////////////
int XC::NDMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }

int XC::NDMaterial::updateParameter(int parameterID, Information &info)
  { return -1; }

int XC::NDMaterial::activateParameter(int parameterID)
  { return -1; }

const XC::Vector &XC::NDMaterial::getStressSensitivity(int gradNumber, bool conditional)
  {
    static XC::Vector dummy(1);
    return dummy;
  }

const XC::Vector &XC::NDMaterial::getStrainSensitivity(int gradNumber)
  {
    static XC::Vector dummy(1);
    return dummy;
  }

double XC::NDMaterial::getRhoSensitivity(int gradNumber)
  { return 0.0; }

const XC::Matrix &XC::NDMaterial::getDampTangentSensitivity(int gradNumber)
  {
    static XC::Matrix dummy(1,1);
    return dummy;
  }

const XC::Matrix &XC::NDMaterial::getTangentSensitivity(int gradNumber)
  {
    static XC::Matrix dummy(1,1);
    return dummy;
  }

int XC::NDMaterial::commitSensitivity(XC::Vector & strainSensitivity, int gradNumber, int numGrads)
  { return 0; }
// AddingSensitivity:END //////////////////////////////////////////

//! @brief Recibe a pointer a material through the channel being passed as parameter.
//! @param posClassTag: Posición de ID del identificador de la clase del material.
//! @param posDbTag: Posición de ID en la que se guarda el dbTag.
XC::NDMaterial *XC::receiveNDMaterialPtr(NDMaterial *ptr,DbTagData &dt,const CommParameters &cp,const BrokedPtrCommMetaData &md)
  {
    NDMaterial *retval= nullptr;
    Material *tmp= nullptr;
    tmp= receiveMaterialPtr(ptr,dt,cp,md);
    if(tmp)
      {
        retval= dynamic_cast<NDMaterial *>(tmp);
        if(!retval)
          {
            std::cerr <<"WARNING receiveNDMaterialPtr - "
                      << " failed to get material." << std::endl;
            delete tmp;
          }
      }
    return retval;
  }
