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
// $Source: /usr/local/cvs/OpenSees/SRC/material/nD/elastic_isotropic/ElasticIsotropicMaterial.cpp,v $                                                                
                                                                        
                                                                        
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

#include <material/nD/elastic_isotropic/ElasticIsotropicMaterial.h>
#include <material/nD/elastic_isotropic/ElasticIsotropicPlaneStress2D.h>
#include <material/nD/elastic_isotropic/ElasticIsotropicPlaneStrain2D.h>
#include <material/nD/elastic_isotropic/ElasticIsotropicAxiSymm.h>
#include <material/nD/elastic_isotropic/ElasticIsotropic3D.h>
#include <material/nD/elastic_isotropic/PressureDependentElastic3D.h>
#include <material/nD/elastic_isotropic/ElasticIsotropicPlateFiber.h>
#include <material/nD/elastic_isotropic/ElasticIsotropicBeamFiber.h>



#include "utility/matrix/Matrix.h"

#include "material/nD/NDMaterialType.h"

//! @brief Constructor.
//!
//! To construct an ElasticIsotropicMaterial whose unique integer tag
//! among NDMaterials in the domain is given by \p tag, and whose class
//! tag is given by \p classTag.  These tags are passed to the
//! NDMaterial class constructor.
//!
//! @param tag: material identifier.
//! @param classTag: material class identifier.
//! @param eps_size: size of the generalized strain vector.
//! @param e: material Young modulus.
//! @param nu: material Poisson coefficient.
//! @param r: material density.
XC::ElasticIsotropicMaterial::ElasticIsotropicMaterial(int tag, int classTag, int eps_size, double e, double nu, double r)
  : NDMaterial(tag, classTag), E(e), v(nu), rho(r),
  epsilon(eps_size), epsilon0(eps_size) {}

//! @brief Constructor.
XC::ElasticIsotropicMaterial::ElasticIsotropicMaterial(int tag, int eps_size)
  : NDMaterial(tag, ND_TAG_ElasticIsotropic), E(0.0), v(0.0), rho(0.0),
  epsilon(eps_size), epsilon0(eps_size) {}

//! @brief Constructor.
XC::ElasticIsotropicMaterial::ElasticIsotropicMaterial(int tag, int eps_size, double e, double nu, double r)
  : NDMaterial(tag, ND_TAG_ElasticIsotropic), E(e), v(nu), rho(r),
  epsilon(eps_size), epsilon0(eps_size) {}

// Boris Jeremic (@ucdavis.edu) 19June2002
//! @brief Return the elastic modulus.
double XC::ElasticIsotropicMaterial::getE(void) const
  { return E; }

// Boris Jeremic (@ucdavis.edu) 19June2002
//! @brief Return the material Poisson's ratio.
double XC::ElasticIsotropicMaterial::getnu(void) const
  { return v; }

//! @brief Returns a specific implementation of an ElasticIsotropicMaterial by
//! switching on \p type.  Outputs an error if \p type is not valid.
//! This is the prototype method. Create a copy of just the material parameters
//! Called by the continuum elements.
XC::NDMaterial *XC::ElasticIsotropicMaterial::getCopy(const std::string &type) const
  {
    if((type==strTypePlaneStress2D) || (type==strTypePlaneStress))
      {
        ElasticIsotropicPlaneStress2D *theModel;
        theModel = new ElasticIsotropicPlaneStress2D(this->getTag(), E, v, rho);
	// DOES NOT COPY sigma, D, and epsilon ...
	// This function should only be called during element instantiation, so
	// no state determination is performed on the material model object
	// prior to copying the material model (calling this function)
        return theModel;
      }

    else if((type==strTypePlaneStrain2D) || (type==strTypePlaneStrain))
      {
        ElasticIsotropicPlaneStrain2D *theModel;
        theModel = new ElasticIsotropicPlaneStrain2D(this->getTag(), E, v, rho);
	// DOES NOT COPY sigma, D, and epsilon ...
	// This function should only be called during element instantiation, so
	// no state determination is performed on the material model object
	// prior to copying the material model (calling this function)
        return theModel;
      }
    else if((type==strTypeAxiSymmetric2D) || (type==strTypeAxiSymmetric))
      {
        ElasticIsotropicAxiSymm *theModel;
        theModel = new ElasticIsotropicAxiSymm(this->getTag(), E, v, rho);
	// DOES NOT COPY sigma, D, and epsilon ...
	// This function should only be called during element instantiation, so
	// no state determination is performed on the material model object
	// prior to copying the material model (calling this function)
        return theModel;
      }
///////////////////////////////
    else if((type==strTypeThreeDimensional) || 
             (type==strType3D))
      {
        ElasticIsotropic3D *theModel;
        theModel = new ElasticIsotropic3D(this->getTag(), E, v, rho);
	// DOES NOT COPY sigma, D, and epsilon ...
	// This function should only be called during element instantiation, so
	// no state determination is performed on the material model object
	// prior to copying the material model (calling this function)
        return theModel;
      }
///////////////////////////////
    else if((type==strTypePlateFiber))
      {
        ElasticIsotropicPlateFiber *theModel;
        theModel = new ElasticIsotropicPlateFiber(this->getTag(), E, v, rho);
	// DOES NOT COPY sigma, D, and epsilon ...
	// This function should only be called during element instantiation, so
	// no state determination is performed on the material model object
	// prior to copying the material model (calling this function)
        return theModel;
      }
    else if((type==strTypeBeamFiber))
      {
        ElasticIsotropicBeamFiber *theModel;
        theModel = new ElasticIsotropicBeamFiber(this->getTag(), E, v, rho);
	// DOES NOT COPY sigma, D, and epsilon ...
	// This function should only be called during element instantiation, so
	// no state determination is performed on the material model object
	// prior to copying the material model (calling this function)
        return theModel;
      }
    else // Handle other cases
      {
        std::cerr << getClassName() << "::" << __FUNCTION__
		  << "; failed to get model: " << type << std::endl;
        exit(-1);
      }
    
    return 0;
  }

//! @brief Sets the initial strain value.
//! @param eps: strain value.
int XC::ElasticIsotropicMaterial::setInitialStrain(const Vector &eps)
  {
    epsilon0= eps;
    return 0;
  }

//! @brief Increments initial strain.
//! @param epsInc: value of the strain increment.
int XC::ElasticIsotropicMaterial::incrementInitialStrain(const Vector &epsInc)
  {
    epsilon0+= epsInc;
    return 0;
  }

//! @brief Zeroes the initial strain.
void XC::ElasticIsotropicMaterial::zeroInitialStrain(void)
  { epsilon0.Zero(); }

//! @brief Returns the value of the initial strain.
const XC::Vector &XC::ElasticIsotropicMaterial::getInitialStrain(void) const
  { return epsilon0; }

//! @brief Set the value of the trial strain.
//! @param eps: value of the trial strain.
int XC::ElasticIsotropicMaterial::setTrialStrain(const Vector &eps)
  {
    epsilon= eps;
    return 0;
  }

//! @brief Set the value of the trial strain.
//! @param eps: value of the trial strain.
//! @param rate: not used.
int XC::ElasticIsotropicMaterial::setTrialStrain(const Vector &eps, const XC::Vector &rate)
  {
    epsilon= eps;
    return 0;
  }

//! @brief Increment the value of the trial strain.
//! @param epsInc: value of the trial strain increment.
int XC::ElasticIsotropicMaterial::setTrialStrainIncr(const Vector &epsInc)
  {
    epsilon+= epsInc;
    return 0;
  }

//! @brief Increment the value of the trial strain.
//! @param epsInc: value of the trial strain increment.
//! @param rate: not used.
int XC::ElasticIsotropicMaterial::setTrialStrainIncr(const Vector &epsInc, const Vector &rate)
  {
    epsilon+= epsInc;
    return -1;
  }

//! @brief Returns material tangent stiffness matrix.
const XC::Matrix &XC::ElasticIsotropicMaterial::getTangent(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility.\n";
    exit(-1);

    // Just to make it compile
    static Matrix ret;
    return ret;
  }

//! @brief Returns material initial tangent stiffness matrix.
const XC::Matrix &XC::ElasticIsotropicMaterial::getInitialTangent(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility.\n";
    exit(-1);

    // Just to make it compile
    static Matrix ret;
    return ret;
  }

//! @brief Returns the material stress.
const XC::Vector &XC::ElasticIsotropicMaterial::getStress(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility.\n";
    exit(-1);
    
    // Just to make it compile
    static Vector ret= Vector();
    return ret;
  }

//! @brief Return the material strain.
const XC::Vector &XC::ElasticIsotropicMaterial::getStrain(void) const
  {
    static Vector retval;
    retval= epsilon-epsilon0;
    return retval;
  }

//! @brief zeroes initial generalized strain
void XC::ElasticIsotropicMaterial::zeroInitialGeneralizedStrain(void)
  { epsilon0.Zero(); }

//! @brief To accept the current value of the trial strain vector
//! as being on the solution path. To return \f$0\f$ if
//! successful, a negative number if not.
int XC::ElasticIsotropicMaterial::commitState(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility.\n";
    exit(-1);
    return -1;
  }

//! @brief To cause the material to revert to its last committed state. To
//! return \f$0\f$ if successful, a negative number if not.
int XC::ElasticIsotropicMaterial::revertToLastCommit(void)
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility.\n";
    exit(-1);
    
    return -1;
  }

//! @biref Invoked to cause the material to revert to its original state in its
//! undeformed configuration. To return \f$0\f$ if successful, a negative
//! number if not.
int XC::ElasticIsotropicMaterial::revertToStart(void)
  {
    int retval= NDMaterial::revertToStart();
    epsilon.Zero();
    return retval;
  }

//! @brief Virtual constructor. Create a copy of material parameters
//! AND state variables. Called by GenericSectionXD
XC::NDMaterial *XC::ElasticIsotropicMaterial::getCopy(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility.\n";
    exit(-1);
    return 0;
  }

const std::string &XC::ElasticIsotropicMaterial::getType(void) const
  {
    static std::string retval= "";
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility.\n";
    exit(-1);
    return retval;
  }

int XC::ElasticIsotropicMaterial::getOrder(void) const
  {
    std::cerr << getClassName() << "::" << __FUNCTION__
              << "; subclass responsibility.\n";
    exit(-1);
    return -1;
  }

//! @brief Send object members through the communicator argument.
int XC::ElasticIsotropicMaterial::sendData(Communicator &comm)
  {
    int res= NDMaterial::sendData(comm);
    res+= comm.sendDoubles(E,v,rho,getDbTagData(),CommMetaData(1));
    res+= comm.sendVector(epsilon,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Receives object members through the communicator argument.
int XC::ElasticIsotropicMaterial::recvData(const Communicator &comm)
  {
    int res= NDMaterial::recvData(comm);
    res+= comm.receiveDoubles(E,v,rho,getDbTagData(),CommMetaData(1));
    res+= comm.receiveVector(epsilon,getDbTagData(),CommMetaData(2));
    return res;
  }

//! @brief Sends object through the communicator argument.
int XC::ElasticIsotropicMaterial::sendSelf(Communicator &comm)
  {
    setDbTag(comm);
    const int dataTag= getDbTag();
    inicComm(3);
    int res= sendData(comm);

    res+= comm.sendIdData(getDbTagData(),dataTag);
    if(res < 0)
      std::cerr << getClassName() << "::" << __FUNCTION__
	        << "; failed to send data\n";
    return res;
  }

//! @brief Receives object through the communicator argument.
int XC::ElasticIsotropicMaterial::recvSelf(const Communicator &comm)
  {
    inicComm(3);
    const int dataTag= getDbTag();
    int res= comm.receiveIdData(getDbTagData(),dataTag);

    if(res<0)
      std::cerr << getClassName() << "::" << __FUNCTION__
                << "; failed to receive ids.\n";
    else
      {
        setTag(getDbTagDataPos(0));
        res+= recvData(comm);
        if(res<0)
          std::cerr << getClassName() << "::" << __FUNCTION__
                    << "; failed to receive data.\n";
      }
    return res;
  }

//! @brief Print stuff.
void XC::ElasticIsotropicMaterial::Print(std::ostream &s, int flag) const
  {
    s << "Elastic isotropic material model" << std::endl;
    s << "\tE:  " << E << std::endl;
    s << "\tv:  " << v << std::endl;
    s << "\trho:  " << rho << std::endl;
    return;
  }

int XC::ElasticIsotropicMaterial::setParameter(const std::vector<std::string> &argv, Parameter &param)
  { return -1; }

int XC::ElasticIsotropicMaterial::updateParameter(int parameterID, Information &info)
  { return -1; }
