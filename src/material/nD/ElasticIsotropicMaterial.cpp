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
** ****************************************************************** */
                                                                        
// $Revision: 1.19 $                                                              
// $Date: 2004/02/24 20:50:58 $                                                                  
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/ElasticIsotropicMaterial.cpp,v $                                                                
                                                                        
                                                                        
// File: ~/material/ElasticIsotropicMaterial.C
//
// Written: MHS 
// Created: Feb 2000
// Revision: A
// Boris Jeremic (@ucdavis.edu) 19June2002 added getE, getnu
//
// Description: This file contains the class implementation for XC::ElasticIsotropicMaterial.
//
// What: "@(#) ElasticIsotropicMaterial.C, revA"

#include <cstring>

#include <material/nD/ElasticIsotropicMaterial.h>
#include <material/nD/elastic_isotropic/ElasticIsotropicPlaneStress2D.h>
#include <material/nD/elastic_isotropic/ElasticIsotropicPlaneStrain2D.h>
#include <material/nD/elastic_isotropic/ElasticIsotropicAxiSymm.h>
#include <material/nD/elastic_isotropic/ElasticIsotropic3D.h>
#include <material/nD/elastic_isotropic/PressureDependentElastic3D.h>
#include <material/nD/elastic_isotropic/ElasticIsotropicPlateFiber.h>
#include <material/nD/elastic_isotropic/ElasticIsotropicBeamFiber.h>



#include "utility/matrix/Matrix.h"

#include "material/nD/TipoMaterialND.h"

//! @brief Constructor.
XC::ElasticIsotropicMaterial::ElasticIsotropicMaterial(int tag, int classTag, int eps_size, double e, double nu, double r)
  :XC::NDMaterial(tag, classTag), E(e), v(nu), rho(r), epsilon(eps_size) {}

//! @brief Constructor.
XC::ElasticIsotropicMaterial::ElasticIsotropicMaterial(int tag, int eps_size)
  :XC::NDMaterial(tag, ND_TAG_ElasticIsotropic), E(0.0), v(0.0), rho(0.0), epsilon(eps_size) {}

//! @brief Constructor.
XC::ElasticIsotropicMaterial::ElasticIsotropicMaterial(int tag, int eps_size, double e, double nu, double r)
  :XC::NDMaterial(tag, ND_TAG_ElasticIsotropic), E(e), v(nu), rho(r), epsilon(eps_size) {}

double XC::ElasticIsotropicMaterial::getRho(void) const
  { return rho; }

// Boris Jeremic (@ucdavis.edu) 19June2002
double XC::ElasticIsotropicMaterial::getE(void)
  { return E; }

// Boris Jeremic (@ucdavis.edu) 19June2002
double XC::ElasticIsotropicMaterial::getnu(void)
  { return v; }

XC::NDMaterial *XC::ElasticIsotropicMaterial::getCopy(const std::string &type) const
  {
    if((type==strTipoPlaneStress2D) || (type==strTipoPlaneStress))
      {
        ElasticIsotropicPlaneStress2D *theModel;
        theModel = new XC::ElasticIsotropicPlaneStress2D(this->getTag(), E, v, rho);
                // DOES NOT COPY sigma, D, and epsilon ...
                // This function should only be called during element instantiation, so
                // no state determination is performed on the material model object
                // prior to copying the material model (calling this function)
        return theModel;
      }

    else if((type==strTipoPlaneStrain2D) || (type==strTipoPlaneStrain))
      {
        ElasticIsotropicPlaneStrain2D *theModel;
        theModel = new XC::ElasticIsotropicPlaneStrain2D(this->getTag(), E, v, rho);
                // DOES NOT COPY sigma, D, and epsilon ...
                // This function should only be called during element instantiation, so
                // no state determination is performed on the material model object
                // prior to copying the material model (calling this function)
        return theModel;
      }
    else if((type==strTipoAxiSymmetric2D) || (type==strTipoAxiSymmetric))
      {
        ElasticIsotropicAxiSymm *theModel;
        theModel = new XC::ElasticIsotropicAxiSymm(this->getTag(), E, v, rho);
                // DOES NOT COPY sigma, D, and epsilon ...
                // This function should only be called during element instantiation, so
                // no state determination is performed on the material model object
                // prior to copying the material model (calling this function)
        return theModel;
      }
///////////////////////////////
    else if((type==strTipoThreeDimensional) || 
             (type==strTipo3D))
      {
        ElasticIsotropic3D *theModel;
        theModel = new XC::ElasticIsotropic3D(this->getTag(), E, v, rho);
                // DOES NOT COPY sigma, D, and epsilon ...
                // This function should only be called during element instantiation, so
                // no state determination is performed on the material model object
                // prior to copying the material model (calling this function)
        return theModel;
      }
///////////////////////////////
    else if((type==strTipoPlateFiber))
      {
        ElasticIsotropicPlateFiber *theModel;
        theModel = new XC::ElasticIsotropicPlateFiber(this->getTag(), E, v, rho);
                // DOES NOT COPY sigma, D, and epsilon ...
                // This function should only be called during element instantiation, so
                // no state determination is performed on the material model object
                // prior to copying the material model (calling this function)
        return theModel;
      }
    else if((type==strTipoBeamFiber))
      {
        ElasticIsotropicBeamFiber *theModel;
        theModel = new XC::ElasticIsotropicBeamFiber(this->getTag(), E, v, rho);
                // DOES NOT COPY sigma, D, and epsilon ...
                // This function should only be called during element instantiation, so
                // no state determination is performed on the material model object
                // prior to copying the material model (calling this function)
        return theModel;
      }
    else // Handle other cases
      {
        std::cerr << "ElasticIsotropicMaterial::getModel failed to get model: " << type << std::endl;
        exit(-1);
      }
    
    return 0;
  }

int XC::ElasticIsotropicMaterial::setTrialStrain(const XC::Vector &v)
  {
    epsilon = v;
    return 0;
  }

int XC::ElasticIsotropicMaterial::setTrialStrain(const XC::Vector &v, const XC::Vector &rate)
  {
    epsilon = v;
    return 0;
  }

int XC::ElasticIsotropicMaterial::setTrialStrainIncr(const XC::Vector &v)
  {
    std::cerr << "ElasticIsotropicMaterial::setTrialStrainIncr -- subclass responsibility\n";
    exit(-1);
    return -1;
  }

int XC::ElasticIsotropicMaterial::setTrialStrainIncr(const XC::Vector &v, const XC::Vector &rate)
  {
    std::cerr << "ElasticIsotropicMaterial::setTrialStrainIncr -- subclass responsibility\n";
    exit(-1);
    return -1;
  }

const XC::Matrix &XC::ElasticIsotropicMaterial::getTangent(void) const
  {
    std::cerr << "ElasticIsotropicMaterial::getTangent -- subclass responsibility\n";
    exit(-1);

    // Just to make it compile
    XC::Matrix *ret = new XC::Matrix();
    return *ret;
  }

const XC::Matrix &XC::ElasticIsotropicMaterial::getInitialTangent(void) const
  {
    std::cerr << "ElasticIsotropicMaterial::getInitialTangent -- subclass responsibility\n";
    exit(-1);

    // Just to make it compile
    XC::Matrix *ret = new Matrix();
    return *ret;
  }

const XC::Vector &XC::ElasticIsotropicMaterial::getStress(void) const
  {
    std::cerr << "ElasticIsotropicMaterial::getStress -- subclass responsibility\n";
    exit(-1);
    
    // Just to make it compile
    XC::Vector *ret = new Vector();
    return *ret;
  }

const XC::Vector &XC::ElasticIsotropicMaterial::getStrain(void) const
  { return epsilon; }

int XC::ElasticIsotropicMaterial::setTrialStrain(const XC::Tensor &v)
  {
    std::cerr << "ElasticIsotropicMaterial::setTrialStrain -- subclass responsibility\n";
    exit(-1);

    return -1;
  }

int XC::ElasticIsotropicMaterial::setTrialStrain(const XC::Tensor &v, const XC::Tensor &r)
  {
    std::cerr << "ElasticIsotropicMaterial::setTrialStrain -- subclass responsibility\n";
    exit(-1);

    return -1;
  }

int XC::ElasticIsotropicMaterial::setTrialStrainIncr(const XC::Tensor &v)
  {
    std::cerr << "ElasticIsotropicMaterial::setTrialStrainIncr -- subclass responsibility\n";
    exit(-1);

    return -1;
  }

int XC::ElasticIsotropicMaterial::setTrialStrainIncr(const XC::Tensor &v, const XC::Tensor &r)
  {
    std::cerr << "ElasticIsotropicMaterial::setTrialStrainIncr -- subclass responsibility\n";
    return -1;
  }

const XC::Tensor &XC::ElasticIsotropicMaterial::getTangentTensor(void) const
  {
    std::cerr << "ElasticIsotropicMaterial::getTangentTensor -- subclass responsibility\n";
    exit(-1);
  
    // Just to make it compile
    XC::Tensor *t = new XC::Tensor;
    return *t;
  }

const XC::stresstensor &XC::ElasticIsotropicMaterial::getStressTensor(void) const
  {
    std::cerr << "ElasticIsotropicMaterial::getStressTensor -- subclass responsibility\n";
    exit(-1);

    // Just to make it compile
    static XC::stresstensor t;
    return t;
  }

const XC::straintensor &XC::ElasticIsotropicMaterial::getStrainTensor(void) const
  {
    std::cerr << "ElasticIsotropicMaterial::getStrainTensor -- subclass responsibility\n";
    exit(-1);

    // Just to make it compile
    static XC::straintensor t;
    return t;
  }

const XC::straintensor &XC::ElasticIsotropicMaterial::getPlasticStrainTensor(void) const
  {
    std::cerr << "ElasticIsotropicMaterial::getPlasticStrainTensor -- subclass responsibility\n";
    exit(-1);
        
    // Just to make it compile
    static XC::straintensor t;
    return t;
  }

int XC::ElasticIsotropicMaterial::commitState(void)
  {
    std::cerr << "ElasticIsotropicMaterial::commitState -- subclass responsibility\n";
    exit(-1);
    return -1;
  }

int XC::ElasticIsotropicMaterial::revertToLastCommit(void)
  {
    std::cerr << "ElasticIsotropicMaterial::revertToLastCommit -- subclass responsibility\n";
    exit(-1);
    
    return -1;
  }

int XC::ElasticIsotropicMaterial::revertToStart(void)
  {
    std::cerr << "ElasticIsotropicMaterial::revertToStart -- subclass responsibility\n";
    exit(-1);
    return -1;
  }

XC::NDMaterial *XC::ElasticIsotropicMaterial::getCopy(void) const
  {
    std::cerr << "ElasticIsotropicMaterial::getCopy -- subclass responsibility\n";
    exit(-1);
    return 0;
  }

const std::string &XC::ElasticIsotropicMaterial::getType(void) const
  {
    static std::string retval= "";
    std::cerr << "ElasticIsotropicMaterial::getType -- subclass responsibility\n";
    exit(-1);
    return retval;
  }

int XC::ElasticIsotropicMaterial::getOrder(void) const
  {
    std::cerr << "ElasticIsotropicMaterial::getOrder -- subclass responsibility\n";
    exit(-1);
    return -1;
  }

//! @brief Send object members through the channel being passed as parameter.
int XC::ElasticIsotropicMaterial::sendData(CommParameters &cp)
  {
    int res= NDMaterial::sendData(cp);
    res+= cp.sendDoubles(E,v,rho,getDbTagData(),CommMetaData(1));
    res+= cp.sendVector(epsilon,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the channel being passed as parameter.
int XC::ElasticIsotropicMaterial::recvData(const CommParameters &cp)
  {
    int res= NDMaterial::recvData(cp);
    res+= cp.receiveDoubles(E,v,rho,getDbTagData(),CommMetaData(1));
    res+= cp.receiveVector(epsilon,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object through the channel being passed as parameter.
int XC::ElasticIsotropicMaterial::sendSelf(CommParameters &cp)
  {
    setDbTag(cp);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(cp);

    res+= cp.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << nombre_clase() << "sendSelf() - failed to send data\n";
    return res;
  }

//! @brief Receives object through the channel being passed as parameter.
int XC::ElasticIsotropicMaterial::recvSelf(const CommParameters &cp)
  {
    inicComm(3);
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

void XC::ElasticIsotropicMaterial::Print(std::ostream &s, int flag)
  {
    s << "Elastic Isotropic XC::Material Model" << std::endl;
    s << "\tE:  " << E << std::endl;
    s << "\tv:  " << v << std::endl;
    s << "\trho:  " << rho << std::endl;
    return;
  }

int XC::ElasticIsotropicMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }

int XC::ElasticIsotropicMaterial::updateParameter(int parameterID, Information &info)
  { return -1; }
